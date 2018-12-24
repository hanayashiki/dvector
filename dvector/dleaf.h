#pragma once
#include "dslice.h"
#include "dnode_base.h"

namespace dv
{
    template<class T, class Allocator = std::allocator<T>>
    struct dleaf : public dnode_base
    {
    public:
        using ValueType = dslice<T, Allocator>;
        ValueType value;

        explicit dleaf(const ValueType & value)
            : value(value)
        {
            dnode_base::type = 'l';
        }

        explicit dleaf(ValueType && value)
            : value(std::move(value))
        {
            dnode_base::type = 'l';
        }

        virtual size_t count() const noexcept override
        {
            return value.size();
        }
 
        ValueType & operator==(const dleaf &) = delete;

        virtual ~dleaf() {}
    };
}