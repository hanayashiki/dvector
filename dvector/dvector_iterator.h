#pragma once

#include <iterator>
#include "dmarcos.h"
#include "dvector_base.h"
#include <cstddef>

namespace dv
{

    template<class T, class Allocator = std::allocator<T>>
    class dvector_iterator
    {
        /* RandomAccessIterator for dvector.
           Implemented in accordance to:
           http://www.cplusplus.com/reference/iterator/
         */
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;
    DV_PROTECTED:
        using DvectorBase = dvector_base<T, Allocator>;
        using Vector = typename DvectorBase::Vector;
        using ElemType = T;
        using LeafType = dleaf<T, Allocator>;
        using SliceType = dslice<T, Allocator>;
    DV_PROTECTED:
        dnode_base * root;
        LeafType * current_leaf;
        size_t current_local_pos;
        size_t current_pos;
    DV_PROTECTED:
        void advance(difference_type diff)
        {
            /* 
                Get the element at `current_pos+diff`.

                1) Find the node that contains index `current_pos+diff`
                    
                2) locate the element using dvector_base::_access_node
             */

            dnode_base * node = current_leaf;

            if (diff > 0)
            {
                size_t target_pos = current_local_pos + diff;
                while (node->count() <= target_pos)
                {
                    auto old_node = node;
                    node = node->p;
                    if (node)
                    {
                        if (static_cast<dnode*>(node)->is_right_child(old_node))
                            target_pos += static_cast<dnode*>(node)->left->count();
                    }
                    else
                    {
                        current_leaf = nullptr; // Indicates we have come to the end
                        current_local_pos = 0;
                        current_pos = old_node->count();
                        return;
                    }
                }

                std::tie(current_leaf, current_local_pos) = DvectorBase::_access_node(node, target_pos);
                current_pos = current_pos + diff;
            }
            else if (diff < 0)
            {
                ptrdiff_t target_pos = current_local_pos + diff;
                while (target_pos < 0)
                {
                    auto old_node = node;
                    if (old_node == nullptr)
                    {
                        node = root;
                        target_pos += root->count();
                    }
                    else
                    {
                        node = node->p;
                        if (node == nullptr)
                        {
                            throw std::exception("dvector_iterator out of range. ");
                        }
                        if (static_cast<dnode*>(node)->is_right_child(old_node))
                            target_pos += static_cast<dnode*>(node)->left->count();
                    }
                }

                std::tie(current_leaf, current_local_pos) = DvectorBase::_access_node(node, target_pos);
                // std::cout << "current_leaf: " << current_leaf << std::endl;
                current_pos = current_pos + diff;
            }
        }
    public:
        dvector_iterator()
            : current_leaf(nullptr), root(nullptr), current_local_pos(0), current_pos(0)
        {
        }

        dvector_iterator(LeafType * current_leaf, dnode_base * root, size_t current_local_pos = 0, size_t current_pos = 0)
            : current_leaf(current_leaf),
              root(root),
              current_local_pos(current_local_pos),
              current_pos(current_pos)
        {
        }

        size_t pos() const
        {
            return current_pos;
        }

        dvector_iterator & operator++()
        {
            advance(1);
            return *this;
        }

        dvector_iterator operator++(int)
        {
            dvector_iterator tmp(*this);
            operator++();
            return tmp;
        }

        dvector_iterator & operator--()
        {
            advance(-1);
            return *this;
        }

        dvector_iterator operator--(int)
        {
            dvector_iterator tmp(*this);
            operator--();
            return tmp;
        }

        dvector_iterator & operator+=(const difference_type & diff)
        {
            advance(diff);
            return *this;
        }

        dvector_iterator & operator-=(const difference_type & diff)
        {
            advance(-diff);
            return *this;
        }
        
        const T & operator*() const noexcept
        {
            return current_leaf->value[current_local_pos];
        }

        T & operator*() noexcept
        {
            return current_leaf->value[current_local_pos];
        }

        const T * operator->() const noexcept
        {
            return &(current_leaf->value[current_local_pos]);
        }

        T * operator->() noexcept
        {
            return &(current_leaf->value[current_local_pos]);
        }
    };

    template<class T, class Allocator = std::allocator<T>>
    inline bool operator == (
        const dvector_iterator<T, Allocator> & lhs, 
        const dvector_iterator<T, Allocator> & rhs) noexcept
    {
        return lhs.pos() == rhs.pos();
    }

    template<class T, class Allocator = std::allocator<T>>
    inline bool operator != (
        const dvector_iterator<T, Allocator> & lhs,
        const dvector_iterator<T, Allocator> & rhs) noexcept
    {
        return lhs.pos() != rhs.pos();
    }

    template<class T, class Allocator = std::allocator<T>>
    inline 
    dvector_iterator<T, Allocator>
        operator + (
            const dvector_iterator<T, Allocator> & iter,
            const typename dvector_iterator<T, Allocator>::difference_type & diff)
    {
        dvector_iterator<T, Allocator> tmp(iter);
        tmp.advance(diff);
        return tmp;
    }

    template<class T, class Allocator = std::allocator<T>>
    inline dvector_iterator<T, Allocator>
        operator + (
            const typename dvector_iterator<T, Allocator>::difference_type & diff,
            const dvector_iterator<T, Allocator> & iter)
    {
        return iter + diff;
    }

    template<class T, class Allocator = std::allocator<T>>
    inline dvector_iterator<T, Allocator>
        operator - (
            const dvector_iterator<T, Allocator> & iter,
            const typename dvector_iterator<T, Allocator>::difference_type & diff)
    {
        return iter + (-diff);
    }

    template<class T, class Allocator = std::allocator<T>>
    inline typename dvector_iterator<T, Allocator>::difference_type
        operator - (
            const dvector_iterator<T, Allocator> & iter1,
            const dvector_iterator<T, Allocator> & iter2)
    {
        return iter1.pos() - iter2.pos();
    }

    template<class T, class Allocator = std::allocator<T>>
    inline bool operator < (
        const dvector_iterator<T, Allocator> & lhs,
        const dvector_iterator<T, Allocator> & rhs)
    {
        return lhs.pos() < rhs.pos();
    }

    template<class T, class Allocator = std::allocator<T>>
    inline bool operator > (
        const dvector_iterator<T, Allocator> & lhs,
        const dvector_iterator<T, Allocator> & rhs)
    {
        return lhs.pos() > rhs.pos();
    }

    template<class T, class Allocator = std::allocator<T>>
    inline bool operator <= (
        const dvector_iterator<T, Allocator> & lhs,
        const dvector_iterator<T, Allocator> & rhs)
    {
        return lhs.pos() <= rhs.pos();
    }

    template<class T, class Allocator = std::allocator<T>>
    inline bool operator >= (
        const dvector_iterator<T, Allocator> & lhs,
        const dvector_iterator<T, Allocator> & rhs)
    {
        return lhs.pos() >= rhs.pos();
    }
}