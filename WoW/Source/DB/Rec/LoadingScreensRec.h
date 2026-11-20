#ifndef WOW_SOURCE_DB_REC_LOADINGSCREENSREC_H
#define WOW_SOURCE_DB_REC_LOADINGSCREENSREC_H

#include <cstddef>
#include <cstdint>
#include <windows.h>

class SFileImpl;

class LoadingScreensRec {
    public:
    static const int32_t s_numColumns = 4;
    static const int32_t s_rowSize = 16;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ const char* m_fileName;
    /* 000C */ BOOL m_hasWideScreen;

    LoadingScreensRec();
    ~LoadingScreensRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
