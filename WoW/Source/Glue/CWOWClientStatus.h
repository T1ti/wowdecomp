#ifndef WOW_SOURCE_GLUE_CWOWCLIENTSTATUS_H
#define WOW_SOURCE_GLUE_CWOWCLIENTSTATUS_H

#include <cstdint>
#include <windows.h>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Storm/H/storm.h"

class CWOWClientStatus : public CStatus {
    public:
    /* 0014 */ HSLOG m_log;

    virtual ~CWOWClientStatus();

    CWOWClientStatus(const char* logFileName);
    CWOWClientStatus(const char* logFileName, BOOL append);
};

#endif
