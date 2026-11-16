#ifndef WOW_SOURCE_WOWSVCS_WOWSVCSCLIENT_FILEDOWNLOAD_H
#define WOW_SOURCE_WOWSVCS_WOWSVCSCLIENT_FILEDOWNLOAD_H

#include <cstdint>
#include "Engine/Source/Storm/H/Types.h"
#include "Engine/Source/Storm/H/stpl.h"

class HttpDownload;

namespace Storm {
class IOUnit;
}

// FileDownload::GetStatus results, named after how the downloads and glue
// screens use them (inferred names; 1 is never assigned in this build).
enum FILE_DOWNLOAD_STATUS : int32_t {
    FILE_DOWNLOAD_STATUS_IN_PROGRESS = 0,
    FILE_DOWNLOAD_STATUS_UNKNOWN_1   = 1,
    FILE_DOWNLOAD_STATUS_COMPLETE    = 2,
    FILE_DOWNLOAD_STATUS_FAILED      = 3, // m_reason says why
    FILE_DOWNLOAD_STATUS_REDIRECT    = 4, // "redirect monolithic" patch instruction
};

// FileDownload::GetReason after a failure, also used by CGlueMgr::PatchFailed
// for patch-apply errors. DISK_FULL selects PATCH_FAILED_DISK_FULL; the others
// are named after where they are set (inferred names).
enum DOWNLOAD_FAILURE_REASON : int32_t {
    DOWNLOAD_FAILURE_NONE              = 0,
    DOWNLOAD_FAILURE_FILE_ERROR        = 1,
    DOWNLOAD_FAILURE_DISK_FULL         = 2,
    DOWNLOAD_FAILURE_STOPPED           = 3,
    DOWNLOAD_FAILURE_ARCHIVE_NOT_FOUND = 4, // wow-patch.mpq missing
    DOWNLOAD_FAILURE_ARCHIVE_INVALID   = 5, // fails to open or authenticate
    DOWNLOAD_FAILURE_PREPATCH_FAILED   = 6,
};

class FileDownload {
    public:
    /* 0004 */ char m_fileName[260];
    /* 0108 */ bool m_partial;
    /* 010C */ FILE_DOWNLOAD_STATUS m_status;
    /* 0110 */ DOWNLOAD_FAILURE_REASON m_reason;
    /* 0114 */ bool m_accepted;
    /* 0118 */ uint32_t m_bytesSinceSave;
    /* 011C */ uint32_t m_reasonData;

    virtual ~FileDownload();
    virtual bool Initialize(const char* fileName, const uint8_t* md5, uint64_t size, uint64_t& offset) = 0;
    virtual void Destroy(bool deleteFile) = 0;
    virtual FILE_DOWNLOAD_STATUS GetStatus() = 0;
    virtual DOWNLOAD_FAILURE_REASON GetReason(uint32_t* data) = 0;
    virtual void Proceed(const uint8_t* data, uint32_t size) = 0;
    virtual float GetProgress() = 0;
    virtual void StopDownload() = 0;
    virtual void CompleteDownload() = 0;
    virtual bool Accepted() = 0;

    FileDownload();
};

class GruntFileDownload : public FileDownload {
    public:
    struct Header {
        /* 0000 */ uint32_t m_magic;
        /* 0004 */ uint32_t m_version;
        /* 0008 */ uint8_t m_md5[16];
        /* 0018 */ uint64_t m_received;
        /* 0020 */ uint64_t m_size;
    };

    static void STORMAPI CalculateMD5(Storm::IOUnit* unit, uint64_t offset, uint64_t size, uint8_t* digest);

    /* 0120 */ Storm::IOUnit* m_unit;
    /* 0124 */ uint32_t unused_0124;
    /* 0128 */ Header m_header;

    ~GruntFileDownload() override;
    bool Initialize(const char* fileName, const uint8_t* md5, uint64_t size, uint64_t& offset) override;
    void Destroy(bool deleteFile) override;
    FILE_DOWNLOAD_STATUS GetStatus() override;
    DOWNLOAD_FAILURE_REASON GetReason(uint32_t* data) override;
    void Proceed(const uint8_t* data, uint32_t size) override;
    float GetProgress() override;
    void StopDownload() override;
    void CompleteDownload() override;
    bool Accepted() override;

    GruntFileDownload();
    void Close(bool deleteFile, bool rename);
    int32_t Delete();
    void RenamePartial();
    bool ValidateForContinue(bool& complete);
    bool VerifyFile();
    bool VerifyMD5(uint64_t size);
    void WriteHeader();
};

class BattlenetFileDownload : public FileDownload {
    public:
    /* 0120 */ int32_t m_numDownloads;
    /* 0124 */ TSGrowableArray<HttpDownload*> m_downloads;

    ~BattlenetFileDownload() override;
    bool Initialize(const char* fileName, const uint8_t* md5, uint64_t size, uint64_t& offset) override;
    void Destroy(bool deleteFile) override;
    FILE_DOWNLOAD_STATUS GetStatus() override;
    DOWNLOAD_FAILURE_REASON GetReason(uint32_t* data) override;
    void Proceed(const uint8_t* data, uint32_t size) override;
    float GetProgress() override;
    void StopDownload() override;
    void CompleteDownload() override;
    bool Accepted() override;

    BattlenetFileDownload();
};

#endif
