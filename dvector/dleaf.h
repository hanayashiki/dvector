#pragma once
#include "dslice.h"
#include "dnode_base.h"
#include "dnode.h"

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
 
        dnode_base * get_brother()
        {
            if (p != nullptr)
            {
                auto ptr = static_cast<dnode*>(p);
                if (p->is_left_child(this))
                {
                    return p->right;
                }
                else if (p->is_right_child(this))
                {
                    return p->left;
                }
                else
                {
                    return nullptr;
                }
            }
            return nullptr;
        }

        ValueType & operator==(const dleaf &) = delete;

        virtual ~dleaf() { /*std::cout << "~dleaf is called!!!";*/ }
    };
}