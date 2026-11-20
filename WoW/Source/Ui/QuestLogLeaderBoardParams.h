#ifndef WOW_SOURCE_UI_QUESTLOGLEADERBOARDPARAMS_H
#define WOW_SOURCE_UI_QUESTLOGLEADERBOARDPARAMS_H

#include <cstddef>
#include <cstdint>
#include <windows.h>

struct WOWGUID;

class QuestLogLeaderBoardParams {
    public:
    QuestLogLeaderBoardParams();

    /* 0000 */ int32_t m_index;
    /* 0004 */ int32_t m_requireLogEntry;
    /* 0008 */ int32_t m_countEmpty;
    /* 000C */ BOOL m_showProgress;
    /* 0010 */ void (*m_questCallback)(int32_t id, const WOWGUID& guid, void* param, bool success);
    /* 0014 */ void (*m_objectCallback)(int32_t id, const WOWGUID& guid, void* param, bool success);
    /* 0018 */ void (*m_creatureCallback)(int32_t id, const WOWGUID& guid, void* param, bool success);
    /* 001C */ void (*m_itemCallback)(int32_t id, const WOWGUID& guid, void* param, bool success);
    /* 0020 */ void* m_param;
    /* 0024 */ char m_text[4096];
    /* 1024 */ char m_type[64];
    /* 1064 */ int32_t m_complete;
};

#endif
