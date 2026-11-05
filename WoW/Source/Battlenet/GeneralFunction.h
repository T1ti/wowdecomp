#ifndef WOW_SOURCE_BATTLENET_GENERALFUNCTION_H
#define WOW_SOURCE_BATTLENET_GENERALFUNCTION_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Battlenet/BSN/BSNEncodeInterface.h"

namespace GeneralFunction {
class Callback;

class Arg {
    public:
    enum Type : uint32_t { // Keep the native tag word unsigned for switch range checks.
        TYPE_NONE = 0,
        TYPE_BOOL = 1,
        TYPE_INT = 2,
        TYPE_UINT = 3,
        TYPE_REAL = 4,
        TYPE_FOURCC = 5,
        TYPE_STRING = 6,
        TYPE_OBJECT = 7,
        TYPE_CALLBACK = 8,
    };

    /* 0000 */ Type m_type;
    /* 0008 */ union {
        bool m_bool;
        int64_t m_int;
        uint64_t m_uint;
        double m_real;
        char m_fourCC[5];
        const char* m_string;
        const BSN::EncodedObject* m_object;
        Callback* m_callback;
    };

    Arg();
    Arg(const Arg& other);
    ~Arg();
    Arg& operator=(const Arg& other);
    Arg& operator=(bool value);
    Arg& operator=(int64_t value);
    Arg& operator=(uint64_t value);
    Arg& operator=(const char* value);
    Arg& operator=(const BSN::EncodedObject* value);
    Arg& operator=(Callback* value);
    void FreeData();
    int64_t ToInt64() const;
    uint64_t ToUInt64() const;
    const char* ToString() const;
    bool ToBool() const;
    const BSN::EncodedObject* ToObject() const;
};

class Callback {
    public:
    /* 0004 */ uint32_t m_refCount;

    virtual ~Callback();
    virtual void Call(int32_t argc, const Arg* args) const = 0;
    virtual void Destroy() const = 0;
    virtual void IncRef() const;
    virtual void DecRef() const;

    Callback();
};

typedef void (*SimpleCallbackFunc)(void* param, int32_t argc, const Arg* args);

class SimpleCallback : public Callback {
    public:
    /* 0008 */ SimpleCallbackFunc m_func;
    /* 000C */ void* m_param;

    void Call(int32_t argc, const Arg* args) const override;
    void Destroy() const override;

    SimpleCallback(SimpleCallbackFunc func, void* param);
};

typedef Arg (*Function)(void* param, int32_t argc, const Arg* args);
}

#endif
