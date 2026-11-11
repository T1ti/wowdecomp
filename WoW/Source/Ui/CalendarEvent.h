#ifndef WOW_SOURCE_UI_CALENDAREVENT_H
#define WOW_SOURCE_UI_CALENDAREVENT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Common/WowTime.h"
#include "WoW/Source/Object/WGUID.h"

class CDataStore;

struct EventInviteInfo {
    /* 0000 */ uint64_t m_inviteID;
    /* 0008 */ WOWGUID m_guid;
    /* 0010 */ uint32_t m_responseTime;
    /* 0014 */ uint8_t m_level;
    /* 0015 */ uint8_t m_status;
    /* 0016 */ uint8_t m_moderatorStatus;
    /* 0017 */ uint8_t m_inviteType;
    /* 0018 */ char m_notes[128];

    EventInviteInfo();
};

// INLINED
inline EventInviteInfo::EventInviteInfo() {
    this->m_inviteID = 0;
    this->m_guid = WOWGUID();
    this->m_responseTime = 0;
    this->m_level = 0;
    this->m_status = 0;
    this->m_moderatorStatus = 0;
    this->m_inviteType = 0;
    this->m_notes[0] = '\0';
}

struct InviteSortCriterion {
    /* 0000 */ uint32_t m_criterion;
    /* 0004 */ uint8_t m_reverse;
};

class CGCalendarEvent {
    public:
    // Deferred name-cache notification selector; reconstructed names.
    enum INVITE_SIGNAL_TYPE : uint32_t {
        INVITE_SIGNAL_LIST_UPDATE = 0,
        INVITE_SIGNAL_LIST_UPDATE_INITIAL = 1,
        INVITE_SIGNAL_NEW_EVENT = 2,
        INVITE_SIGNAL_NEW_EVENT_COPY = 3,
        INVITE_SIGNAL_OPEN_EVENT = 4,
    };

    /* 0000 */ uint64_t m_eventID;
    /* 0008 */ TSGrowableArray<EventInviteInfo*> m_invites;
    /* 0018 */ uint32_t m_numInvites;
    /* 0020 */ uint64_t m_selectedInviteID;
    /* 0028 */ uint64_t m_moderatorInviteID;
    /* 0030 */ char m_title[128];
    /* 00B0 */ char m_description[1024];
    /* 04B0 */ uint32_t m_type;
    /* 04B4 */ uint32_t m_repeatOption;
    /* 04B8 */ uint32_t m_maxSize;
    /* 04BC */ WowTime m_date;
    /* 04DC */ WowTime m_lockoutDate;
    /* 04FC */ uint32_t m_flags;
    /* 0500 */ uint8_t m_new;
    /* 0501 */ uint8_t m_changed;
    /* 0508 */ WOWGUID m_creatorGUID;
    /* 0510 */ int32_t m_textureID;
    /* 0514 */ InviteSortCriterion m_sortCriteria[6];
    /* 0544 */ uint32_t m_sortType;
    /* 0548 */ uint32_t m_pendingNames;
    /* 054C */ uint32_t m_signalType;
    /* 0550 */ uint32_t m_guildID;

    CGCalendarEvent();
    CGCalendarEvent(WOWGUID guid, uint8_t level, int32_t calendarType);
    ~CGCalendarEvent();
    void Clear();
    void SetTitle(const char* title);
    void SetDescription(const char* description);
    void SetType(uint32_t type);
    void SetRepeatOption(uint32_t repeatOption);
    void SetSize(uint32_t size);
    bool SetDate(uint32_t month, uint32_t monthDay, uint32_t year);
    bool SetTime(uint32_t hour, uint32_t minute);
    bool SetLockoutDate(uint32_t month, uint32_t monthDay, uint32_t year);
    bool SetLockoutTime(uint32_t hour, uint32_t minute);
    void SetTextureID(int32_t textureID);
    void SetFlag(uint32_t flag);
    void ClearFlag(uint32_t flag);
    void SetSignalType(uint32_t signalType);
    bool ProcessUpdateEvent(CDataStore* msg);
    void InviteRemoved(WOWGUID guid);
    void UpdateInviteStatus(WOWGUID guid, uint8_t status, uint32_t responseTime);
    void UpdateInviteNotes(WOWGUID guid, const char* notes);
    bool CanModerate(uint32_t index);
    bool SortInvites(uint32_t criterion, bool toggle);
    void SelectInvite(uint32_t index);
    bool IsModerator();
    bool AddEvent();
    bool RemoveEvent();
    bool Update();
    bool Invite(const char* name);
    bool RemoveInvite(uint32_t index);
    bool RemoveInviteByGUID(WOWGUID guid);
    bool InviteRSVP(uint32_t status);
    bool SignUp(uint8_t tentative);
    void UpdateInviteModeratorStatus(WOWGUID guid, uint8_t status);
    bool CanSetInviteStatus(uint32_t index, uint32_t status);
    void SetInviteStatus(uint32_t index, uint32_t status);
    void SetModeratorStatus(uint32_t index, uint32_t status);
    bool ProcessGetEvent(CDataStore* msg, int32_t* status);
    bool ProcessInitialInviteList(CDataStore* msg);
    void AddInvite(uint64_t inviteID, WOWGUID guid, uint8_t level, uint8_t status, uint8_t inviteType, uint32_t responseTime);
    EventInviteInfo* GetInvite(WOWGUID guid);
    int32_t GetInviteIndex(WOWGUID guid);
    int32_t GetSelectedInviteIndex();
};

const char* GetEventStatusString(uint32_t status);

#endif
