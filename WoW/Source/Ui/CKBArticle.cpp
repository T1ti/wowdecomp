#include "CKBArticle.h"
#include "Engine/Source/Base/DynamicString.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"

// STATIC: 0x00ACD600
CKBArticle g_KnowledgeBaseArticle;

// INLINED
CKBArticle::CKBArticle() {
}

// VERIFY: 431409c4 auto
// OFFSET: 0x005EEB70
void CKBArticle::_ComputeCachedData() {
}

// VERIFY: ffc220ae auto
// OFFSET: 0x00551430
int32_t CKBArticle::BeginLoading(int32_t articleId, int32_t searchType) {
    if (this->m_eState == CKBPage::KBState_LoadInProgress || this->m_inQueue) {
        return 0;
    }

    CDynamicString url;
    url.SetCapacity(1024);
    url += KnowledgeBaseGetURLForLocale();
    url += "sendKBArticleQuery.xml";
    url += "?articleId=";
    url += articleId;
    url += "&languageId=";
    url += s_locale;
    url += "&searchType=";
    url += searchType;
    url += "&locale=";
    url += s_localeCode;

    if (!OsURLDownload(url.m_buffer, &CKBPage::_LoadingCallback, this)) {
        this->m_eState = CKBPage::PAGE_STATE::DOWNLOAD_START_FAILED;
        this->_SendFailureEvent();
        return 0;
    }

    return this->_BeginLoading();
}

// VERIFY: 224696f2 auto
// OFFSET: 0x00552340
void CKBArticle::_SendSuccessEvent() {
    if (CGGameUI::m_initialized) {
        FrameScript_SignalEvent(SCRIPT_EVENT_KNOWLEDGE_BASE_ARTICLE_LOAD_SUCCESS, nullptr);
    }
}

// VERIFY: cd1f15d5 auto
// OFFSET: 0x00552360
void CKBArticle::_SendFailureEvent() {
    if (CGGameUI::m_initialized) {
        FrameScript_SignalEvent(SCRIPT_EVENT_KNOWLEDGE_BASE_ARTICLE_LOAD_FAILURE, nullptr);
    }
}
