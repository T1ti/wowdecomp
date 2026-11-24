#ifndef ENGINE_SOURCE_TEMPEST_CPRIORITYQ_H
#define ENGINE_SOURCE_TEMPEST_CPRIORITYQ_H

#include <typeinfo>
#include "Engine/Source/Tempest/cdyntable.h"

template <class T, class B = CDynTable<T>>
class CPriorityQ : protected B {
    public:
    bool Empty() const { // (inferred)
        STORM_ASSERT(this->Used() > 0);
        return this->Used() <= 1;
    }

    using B::Used;

    protected:
    explicit CPriorityQ(const uint32_t* params)
        : B(params[0], params[1], typeid(T).raw_name()) {
        STORM_ASSERT(this->IsValid()); // 12213: "CPriorityQ<T, B>: construction of base class failed."
        const bool grew = this->Grow();
        STORM_ASSERT(grew); // 12213: "this->Grow()"
        (void)grew;
    }
};

#endif
