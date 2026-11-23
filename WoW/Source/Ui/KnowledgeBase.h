#ifndef WOW_SOURCE_UI_KNOWLEDGEBASE_H
#define WOW_SOURCE_UI_KNOWLEDGEBASE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Os/OsURLDownload.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Ui/CKBIterator.h"

class CDynamicString;
class XMLNode;
struct XMLTree;
struct lua_State;

class CGKnowledgeBase {
    public:
    /* 0000 */ uint8_t unused_0000[4];
    /* 0004 */ uint32_t unused_0004;
    /* 0008 */ uint8_t unused_0008[44];
    /* 0034 */ uint32_t unused_0034;
    /* 0038 */ uint8_t unused_0038[8];
    /* 0040 */ uint32_t unused_0040;
    /* 0044 */ uint8_t unused_0044[4];
    /* 0048 */ uint32_t unused_0048;
};

class CKBPage {
    public:
    static void Initialize();
    static void Shutdown();
    static void UpdateLoadingQueue();

    enum PAGE_STATE : uint32_t {
        IDLE = 0,
        KBState_LoadInProgress = 1,
        KBState_LoadSuccessful = 2,
        DOWNLOAD_START_FAILED = 3,
        CALLBACK_FAILED = 4,
        TIMEOUT = 5,
        NOT_FOUND = 6,
        BUFFER_ALLOCATION_FAILED = 7,
        XML_LOAD_FAILED = 8,
    };

    private:
    static CKBPage* s_pLoadingQueue_Head;

    protected:
    static bool _LoadingCallback(void* param, const char* data, uint32_t size, bool done, HTTPError::ErrorCode error);

    /* 0004 */ PAGE_STATE m_eState;
    private:
    /* 0008 */ SCritSect m_lock;
    /* 0030 */ CDynamicString* m_data;
    protected:
    /* 0034 */ XMLTree* m_pXMLTree;
    private:
    /* 0038 */ CKBPage* m_pLoadingQueue_Next;
    protected:
    /* 003C */ uint32_t m_inQueue;

    int32_t _BeginLoading();

    public:
    bool IsLoaded() const { return this->m_eState == KBState_LoadSuccessful; } // (inferred)
    bool CanReadXMLTree() const { // (inferred)
        STORM_ASSERT(m_eState == KBState_LoadSuccessful);

        if (!this->IsLoaded()) {
            return false;
        }

        STORM_ASSERT(m_pXMLTree);
        return this->m_pXMLTree != nullptr;
    }

    CKBPage();
    virtual ~CKBPage();
    virtual void _ComputeCachedData();
    virtual void _SendSuccessEvent();
    virtual void _SendFailureEvent();
    XMLNode* _GetRoot(CKBIterator& iterator);
    int32_t _GetInt(XMLNode* node, const char* attribute) const;
    int32_t _GetInt(const char* element, const char* attribute) const;
    bool _GetBool(XMLNode* node, const char* attribute) const;
    const char* _GetBodyString(XMLNode* node) const;
    const char* _GetBodyString(const char* element, const char* child) const;
    bool _GetBodyBool(const char* element, const char* child) const;
    int32_t _GetBodyInt(const char* element, const char* child) const;
};

class CKBSetup : public CKBPage {
    public:
    /* 0040 */ int32_t m_numLanguages;
    /* 0044 */ int32_t m_numCategories;
    /* 0048 */ int32_t m_numSubCategories;
    /* 004C */ int32_t m_numArticleHeaders;
    /* 0050 */ int32_t m_currentCategory;
    /* 0054 */ CKBIterator m_languages;
    /* 005C */ CKBIterator m_categories;
    /* 0064 */ CKBIterator m_subCategories;
    /* 006C */ CKBIterator m_articleHeaders;

    CKBSetup();
    int32_t BeginLoading(int32_t numArticles, int32_t pageNumber);
    XMLNode* GetLanguage(int32_t index);
    XMLNode* GetCategoryNode(int32_t index);
    XMLNode* GetArticleHeader(int32_t index);
    void GetCategory(int32_t category);
    XMLNode* GetSubCategory(int32_t category, int32_t subCategory);
    virtual void _ComputeCachedData();
    virtual void _SendSuccessEvent();
    virtual void _SendFailureEvent();
};

extern CKBSetup g_KnowledgeBaseSetup;

const char* KnowledgeBaseGetURLForLocale();
void KnowledgeBase_RegisterScriptFunctions();
void KnowledgeBase_UnregisterScriptFunctions();

#endif
