#pragma once
#include <memory>

#include "dleaf.h"
#include "dnode.h"

#include <iostream>
#include <typeinfo>

namespace dv
{
    template<class _Alloc,
        class _Value_type>
        using _Rebind_alloc_t = typename std::allocator_traits<_Alloc>::template rebind_alloc<_Value_type>;

    template<class T, class Allocator>
    struct dvector_alloc_base
    {
        using LeafType = dleaf<T, Allocator>;
        using LeafAlloc = _Rebind_alloc_t<Allocator, LeafType>;
        using NodeAlloc = _Rebind_alloc_t<Allocator, dnode>;
        using NodeBaseAlloc = _Rebind_alloc_t<Allocator, dnode_base>;

        template<typename... Args>
        LeafType * leaf_construct(Args&&... args)
        {
            auto ptr = LeafAlloc().allocate(1);
            ptr = new (ptr) LeafType(std::forward<Args>(args)...);
            return ptr;
        }

        template<typename... Args>
        dnode * node_construct(Args&&... args)
        {
            auto ptr = NodeAlloc().allocate(1);
            ptr = new (ptr) dnode(std::forward<Args>(args)...);
            return ptr;
        }

        template<typename T>
        void destory(T *)
        {
            throw std::exception("Not implemented destructor for T. ");
        }

        template<>
        void destory<dnode_base>(dnode_base * n)
        {
            n->~dnode_base();
            NodeBaseAlloc().deallocate(n, 1);
        }
    };
}