#include "KnowledgeBase.h"
#include <cstdlib>
#include "Engine/Source/Base/DynamicString.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Ui/CKBArticle.h"
#include "WoW/Source/Ui/CKBQuery.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/FrameScript/FrameScript.h"

// STATIC: 0x00ACD538
CKBSetup g_KnowledgeBaseSetup;

// STATIC: 0x00BEA7F8
CKBPage* CKBPage::s_pLoadingQueue_Head;

// VERIFY: 17635375 auto
// OFFSET: 0x00550AE0
CKBPage::CKBPage() {
    this->m_eState = CKBPage::PAGE_STATE::IDLE;
    this->m_data = nullptr;
    this->m_pXMLTree = nullptr;
    this->m_pLoadingQueue_Next = nullptr;
    this->m_inQueue = 0;
}

// VERIFY: 8d159cad manual
// OFFSET: 0x00550B80
bool CKBPage::_LoadingCallback(void* param, const char* data, uint32_t size, bool done, HTTPError::ErrorCode error) {
    CKBPage* pPage = static_cast<CKBPage*>(param);

    STORM_ASSERT(pPage);

    if (!pPage) {
        return false;
    }

    pPage->m_lock.Enter();
    bool loading = pPage->m_eState == CKBPage::KBState_LoadInProgress;

    if (!done) {
        if (!data) {
            loading = false;
        }

        if (!size) {
            pPage->m_lock.Leave();
            return true;
        }
    }

    if (error || !loading) {
        if (error == 2) {
            pPage->m_eState = CKBPage::PAGE_STATE::TIMEOUT;
        } else if (error == 3) {
            pPage->m_eState = CKBPage::PAGE_STATE::NOT_FOUND;
        } else {
            pPage->m_eState = CKBPage::PAGE_STATE::CALLBACK_FAILED;
        }

        pPage->m_lock.Leave();
        return false;
    }

    if (!pPage->m_data) {
        pPage->m_data = new (SMemAlloc(sizeof(CDynamicString), __FILE__, __LINE__, 0)) CDynamicString();
        pPage->m_data->SetCapacity(0x8000);
    }

    if (!pPage->m_data) {
        pPage->m_eState = CKBPage::PAGE_STATE::BUFFER_ALLOCATION_FAILED;
        pPage->m_lock.Leave();
        return false;
    }

    pPage->m_data->Append(data, size);

    if (!done) {
        pPage->m_lock.Leave();
        return true;
    }

    if (pPage->m_pXMLTree) {
        XMLTree_Free(pPage->m_pXMLTree);
    }

    pPage->m_pXMLTree = XMLTree_Load(pPage->m_data->m_buffer, pPage->m_data->m_length);
    CDynamicString* buffer = pPage->m_data;

    if (buffer) {
        buffer->~CDynamicString();
        SMemFree(buffer, "delete", SLOG_FUNCTION, 0);
    }

    pPage->m_data = nullptr;

    if (!pPage->m_pXMLTree) {
        pPage->m_eState = CKBPage::PAGE_STATE::XML_LOAD_FAILED;
        pPage->m_lock.Leave();
        return false;
    }

    pPage->_ComputeCachedData();
    pPage->m_eState = CKBPage::KBState_LoadSuccessful;
    pPage->m_lock.Leave();
    return true;
}

// VERIFY: ec9f19a2 auto
// OFFSET: 0x00550E10
int32_t CKBSetup::BeginLoading(int32_t numArticles, int32_t pageNumber) {
    if (this->m_eState == CKBPage::KBState_LoadInProgress || this->m_inQueue) {
        return 0;
    }

    CDynamicString url;
    url.SetCapacity(1024);
    url += KnowledgeBaseGetURLForLocale();
    url += "getKBSetup.xml";
    url += "?languageId=";
    url += s_locale;
    url += "&numArticles=";
    url += numArticles;
    url += "&pageNumber=";
    url += pageNumber;
    url += "&locale=";
    url += s_localeCode;

    if (!OsURLDownload(url.m_buffer, &CKBPage::_LoadingCallback, this)) {
        this->m_eState = CKBPage::PAGE_STATE::DOWNLOAD_START_FAILED;
        this->_SendFailureEvent();
        return 0;
    }

    this->m_currentCategory = -1;
    this->m_languages.m_node = nullptr;
    this->m_categories.m_node = nullptr;
    this->m_subCategories.m_node = nullptr;
    this->m_articleHeaders.m_node = nullptr;
    return this->_BeginLoading();
}

// VERIFY: 772164f4 auto
// OFFSET: [0x00550B10, 0x005526D0]
CKBPage::~CKBPage() {
    if (this->m_pXMLTree) {
        XMLTree_Free(this->m_pXMLTree);
        XMLTree_Cleanup();
    }

    if (this->m_data) {
        CDynamicString* data = this->m_data;
        data->~CDynamicString();
        SMemFree(data, "delete", SLOG_FUNCTION, 0);
    }
}

// VERIFY: 14b81430 auto
// OFFSET: 0x005EEB70
void CKBPage::_ComputeCachedData() {
}

// VERIFY: 8bb4a93d auto
// OFFSET: 0x00550B60
int32_t CKBPage::_BeginLoading() {
    STORM_ASSERT(m_eState != KBState_LoadInProgress);
    this->m_eState = KBState_LoadInProgress;
    STORM_ASSERT(s_pLoadingQueue_Head != this);
    this->m_pLoadingQueue_Next = CKBPage::s_pLoadingQueue_Head;
    CKBPage::s_pLoadingQueue_Head = this;
    this->m_inQueue = 1;
    return 1;
}

// VERIFY: de149e18 manual
// OFFSET: 0x00550D10
XMLNode* CKBPage::_GetRoot(CKBIterator& iterator) {
    XMLNode* root = XMLTree_GetRoot(this->m_pXMLTree);

    if (root) {
        iterator.m_node = root;
        iterator.m_index = 0;
    }

    return root;
}

// VERIFY: bb9406ad auto
// OFFSET: [0x00550D40, 0x00551010]
int32_t CKBPage::_GetInt(XMLNode* node, const char* attribute) const {
    if (!this->CanReadXMLTree() || !node) {
        return 0;
    }

    const char* value = node->GetAttributeByName(attribute);
    return value ? atol(value) : 0;
}

// VERIFY: 24e29a0d auto
// OFFSET: 0x00550D80
const char* CKBPage::_GetBodyString(XMLNode* node) const {
    if (!this->CanReadXMLTree()) {
        return nullptr;
    }

    return node->m_body;
}

// VERIFY: c12a4731 auto
// OFFSET: 0x00550DA0
bool CKBPage::_GetBodyBool(const char* element, const char* child) const {
    const char* body = this->_GetBodyString(element, child);
    return body && !SStrCmpI(body, "true", STORM_MAX_STR);
}

// VERIFY: 9b5ccfb5 manual
// OFFSET: [0x005510B0, 0x005512B0]
bool CKBPage::_GetBool(XMLNode* node, const char* attribute) const {
    if (!this->CanReadXMLTree() || !node) {
        return false;
    }

    const char* value = node->GetAttributeByName(attribute);
    return value && !SStrCmpI(value, "true", STORM_MAX_STR);
}

// VERIFY: d61059d6 manual
// OFFSET: 0x00551300
int32_t CKBPage::_GetInt(const char* element, const char* attribute) const {
    if (!this->CanReadXMLTree()) {
        return 0;
    }

    XMLNode* root = XMLTree_GetRoot(this->m_pXMLTree);

    if (!root) {
        return 0;
    }

    XMLNode* node = root->GetChildByName(element);

    if (!node) {
        return 0;
    }

    const char* value = node->GetAttributeByName(attribute);
    return value ? atol(value) : 0;
}

// INLINED
const char* CKBPage::_GetBodyString(const char* element, const char* child) const {
    if (!this->CanReadXMLTree()) {
        return nullptr;
    }

    XMLNode* root = XMLTree_GetRoot(this->m_pXMLTree);

    if (!root) {
        return nullptr;
    }

    XMLNode* node = root->GetChildByName(element);

    if (!node) {
        return nullptr;
    }

    node = node->GetChildByName(child);
    return node ? node->m_body : nullptr;
}

// VERIFY: 58d9c09c manual
// OFFSET: [0x00551550, 0x005515A0]
int32_t CKBPage::_GetBodyInt(const char* element, const char* child) const {
    const char* body = this->_GetBodyString(element, child);
    return body ? atol(body) : 0;
}

// VERIFY: 8c81adf5 manual
// OFFSET: 0x00550F50
XMLNode* CKBSetup::GetLanguage(int32_t index) {
    if (!this->CanReadXMLTree()) {
        return nullptr;
    }

    if (!this->m_languages.m_node) {
        XMLNode* root = XMLTree_GetRoot(this->m_pXMLTree);

        if (root) {
            XMLNode* languages = root->GetChildByName("Languages");

            if (languages) {
                this->m_languages.m_node = languages->m_child;
                this->m_languages.m_index = 0;
            }
        }
    }

    this->m_languages.GoTo(index);
    return this->m_languages.m_node;
}

// VERIFY: 6baf22c5 manual
// OFFSET: 0x00550FB0
XMLNode* CKBSetup::GetCategoryNode(int32_t index) {
    if (!this->CanReadXMLTree()) {
        return nullptr;
    }

    if (!this->m_categories.m_node) {
        XMLNode* root = XMLTree_GetRoot(this->m_pXMLTree);

        if (root) {
            XMLNode* categories = root->GetChildByName("Categories");

            if (categories) {
                this->m_categories.m_node = categories->m_child;
                this->m_categories.m_index = 0;
            }
        }
    }

    this->m_categories.GoTo(index);
    return this->m_categories.m_node;
}

// VERIFY: 80098012 manual
// OFFSET: 0x00551050
XMLNode* CKBSetup::GetArticleHeader(int32_t index) {
    if (!this->CanReadXMLTree()) {
        return nullptr;
    }

    if (!this->m_articleHeaders.m_node) {
        XMLNode* root = XMLTree_GetRoot(this->m_pXMLTree);

        if (root) {
            XMLNode* headers = root->GetChildByName("ArticleHeaders");

            if (headers) {
                this->m_articleHeaders.m_node = headers->m_child;
                this->m_articleHeaders.m_index = 0;
            }
        }
    }

    this->m_articleHeaders.GoTo(index);
    return this->m_articleHeaders.m_node;
}

// VERIFY: 34a727b3 manual
// OFFSET: 0x00551100
void CKBSetup::_ComputeCachedData() {
    XMLNode* node = nullptr;
    XMLNode* root = XMLTree_GetRoot(this->m_pXMLTree);

    if (root) {
        XMLNode* languages = root->GetChildByName("Languages");

        if (languages) {
            node = languages->m_child;
            this->m_articleHeaders.m_node = node;
            this->m_articleHeaders.m_index = 0;
        }
    }

    for (this->m_numLanguages = 0; node; node = node->m_next) {
        this->m_numLanguages++;
    }

    root = XMLTree_GetRoot(this->m_pXMLTree);

    if (root) {
        XMLNode* categories = root->GetChildByName("Categories");

        if (categories) {
            node = categories->m_child;
            this->m_articleHeaders.m_node = node;
            this->m_articleHeaders.m_index = 0;
        }
    }

    for (this->m_numCategories = 0; node; node = node->m_next) {
        this->m_numCategories++;
    }

    this->m_numSubCategories = 0;
    root = XMLTree_GetRoot(this->m_pXMLTree);

    if (root) {
        XMLNode* headers = root->GetChildByName("ArticleHeaders");

        if (headers) {
            node = headers->m_child;
            this->m_articleHeaders.m_node = node;
            this->m_articleHeaders.m_index = 0;
        }
    }

    for (this->m_numArticleHeaders = 0; node; node = node->m_next) {
        this->m_numArticleHeaders++;
    }
}

// VERIFY: 63e48dff auto
// OFFSET: 0x005511E0
void CKBSetup::GetCategory(int32_t category) {
    this->m_subCategories.m_node = nullptr;
    this->m_subCategories.m_index = 0;
    this->m_numSubCategories = 0;
    this->m_currentCategory = -1;

    XMLNode* node = this->GetCategoryNode(category);

    if (!node) {
        return;
    }

    XMLNode* subCategories = node->GetChildByName("SubCategories");

    if (!subCategories) {
        return;
    }

    XMLNode* child = subCategories->m_child;

    if (!child) {
        return;
    }

    this->m_currentCategory = category;
    this->m_subCategories.m_node = child;

    do {
        this->m_numSubCategories++;
        child = child->m_next;
    } while (child);
}

// VERIFY: a20cef53 manual
// OFFSET: 0x005520B0
XMLNode* CKBSetup::GetSubCategory(int32_t category, int32_t subCategory) {
    if (!this->CanReadXMLTree() || category < 0) {
        return nullptr;
    }

    if (this->m_currentCategory != category || !this->m_subCategories.m_node) {
        this->GetCategory(category);
    }

    this->m_subCategories.GoTo(subCategory);
    return this->m_subCategories.m_node;
}

// VERIFY: f258e870 auto
// OFFSET: 0x00552100
void CKBSetup::_SendSuccessEvent() {
    if (CGGameUI::m_initialized) {
        FrameScript_SignalEvent(SCRIPT_EVENT_KNOWLEDGE_BASE_SETUP_LOAD_SUCCESS, nullptr);
    }
}

// VERIFY: f1e62326 auto
// OFFSET: 0x005EEB70
void CKBPage::_SendSuccessEvent() {
}

// VERIFY: 5d3a5e30 auto
// OFFSET: 0x005EEB70
void CKBPage::_SendFailureEvent() {
}

// INLINED
CKBSetup::CKBSetup() {
    this->m_numLanguages = 0;
    this->m_numCategories = 0;
    this->m_numSubCategories = 0;
    this->m_numArticleHeaders = 0;
    this->m_currentCategory = -1;
    this->m_languages.m_node = nullptr;
    this->m_languages.m_index = 0;
    this->m_categories.m_node = nullptr;
    this->m_categories.m_index = 0;
    this->m_subCategories.m_node = nullptr;
    this->m_subCategories.m_index = 0;
    this->m_articleHeaders.m_node = nullptr;
    this->m_articleHeaders.m_index = 0;
}

// VERIFY: 1b2d7a48 manual
// OFFSET: 0x005EEB70
void CKBPage::Initialize() {
}

// VERIFY: c721f005 auto
// OFFSET: 0x00550940
const char* KnowledgeBaseGetURLForLocale() {
    switch (LocaleNameToIndex(s_localeCode)) {
        case DB_LOCALE_deDE:
        case DB_LOCALE_enGB:
        case DB_LOCALE_esES:
        case DB_LOCALE_frFR:
        case DB_LOCALE_ruRU:
            return "http://support.wow-europe.com/kb/";

        case DB_LOCALE_koKR:
            return "http://support.worldofwarcraft.co.kr/kb/";

        case DB_LOCALE_zhCN:
        case DB_LOCALE_enCN:
            return "http://cn.kbase.blizzard.com/kb/wow/";

        case DB_LOCALE_zhTW:
        case DB_LOCALE_enTW:
            return "http://support.wowtaiwan.com.tw/kb/";

        default:
            return "http://support.worldofwarcraft.com/kb/";
    }
}

// VERIFY: 8a5437bf manual
// OFFSET: 0x00550A80
void CKBPage::UpdateLoadingQueue() {
    CKBPage* pPrevPage = nullptr;
    CKBPage* pCurPage = CKBPage::s_pLoadingQueue_Head;

    while (pCurPage) {
        if (pCurPage->m_eState == CKBPage::KBState_LoadInProgress) {
            pPrevPage = pCurPage;
            pCurPage = pCurPage->m_pLoadingQueue_Next;
            continue;
        }

        if (pPrevPage) {
            STORM_ASSERT(pCurPage->m_pLoadingQueue_Next != pPrevPage);
            pPrevPage->m_pLoadingQueue_Next = pCurPage->m_pLoadingQueue_Next;
        } else {
            CKBPage::s_pLoadingQueue_Head = pCurPage->m_pLoadingQueue_Next;
        }

        if (pCurPage->m_eState == CKBPage::KBState_LoadSuccessful) {
            pCurPage->_SendSuccessEvent();
        } else {
            pCurPage->_SendFailureEvent();
        }

        CKBPage* next = pCurPage->m_pLoadingQueue_Next;
        pCurPage->m_inQueue = 0;
        pCurPage->m_pLoadingQueue_Next = nullptr;
        pCurPage = next;
    }
}

// VERIFY: c9bdf799 manual
// OFFSET: 0x00552740
void CKBPage::Shutdown() {
    while (CKBPage::s_pLoadingQueue_Head) {
        OsSleep(1);
        CKBPage::UpdateLoadingQueue();
    }

    if (g_KnowledgeBaseSetup.m_pXMLTree) {
        XMLTree_Free(g_KnowledgeBaseSetup.m_pXMLTree);
        XMLTree_Cleanup();
        g_KnowledgeBaseSetup.m_pXMLTree = nullptr;
    }

    if (g_KnowledgeBaseSetup.m_data) {
        CDynamicString* data = g_KnowledgeBaseSetup.m_data;
        data->~CDynamicString();
        SMemFree(data, "delete", SLOG_FUNCTION, 0);
        g_KnowledgeBaseSetup.m_data = nullptr;
    }

    g_KnowledgeBaseSetup.m_eState = CKBPage::PAGE_STATE::IDLE;

    if (g_KnowledgeBaseQuery.m_pXMLTree) {
        XMLTree_Free(g_KnowledgeBaseQuery.m_pXMLTree);
        XMLTree_Cleanup();
        g_KnowledgeBaseQuery.m_pXMLTree = nullptr;
    }

    if (g_KnowledgeBaseQuery.m_data) {
        CDynamicString* data = g_KnowledgeBaseQuery.m_data;
        data->~CDynamicString();
        SMemFree(data, "delete", SLOG_FUNCTION, 0);
        g_KnowledgeBaseQuery.m_data = nullptr;
    }

    g_KnowledgeBaseQuery.m_eState = CKBPage::PAGE_STATE::IDLE;

    if (g_KnowledgeBaseArticle.m_pXMLTree) {
        XMLTree_Free(g_KnowledgeBaseArticle.m_pXMLTree);
        XMLTree_Cleanup();
        g_KnowledgeBaseArticle.m_pXMLTree = nullptr;
    }

    if (g_KnowledgeBaseArticle.m_data) {
        CDynamicString* data = g_KnowledgeBaseArticle.m_data;
        data->~CDynamicString();
        SMemFree(data, "delete", SLOG_FUNCTION, 0);
        g_KnowledgeBaseArticle.m_data = nullptr;
    }

    g_KnowledgeBaseArticle.m_eState = CKBPage::PAGE_STATE::IDLE;
}

// VERIFY: 22ec21ae auto
// OFFSET: 0x00552120
void CKBSetup::_SendFailureEvent() {
    if (CGGameUI::m_initialized) {
        FrameScript_SignalEvent(SCRIPT_EVENT_KNOWLEDGE_BASE_SETUP_LOAD_FAILURE, nullptr);
    }
}
