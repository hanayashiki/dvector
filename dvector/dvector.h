#pragma once

#include "dnode.h"
#include "dslice.h"
#include "dleaf.h"
#include "dvector_base.h"
#include <tuple>

/* 
 *  dvector: logarithmic insert, index-access, deletion array 
 *     
 *  structures:
 *      1. dnode extends base_node:
 *          1. h: Height of left subtree - Height of right subtree
 *          2. left: base_node
 *          3. right: base_node
 *          4. elem_count : cached count()
 *      2. dleaf extends base_node:
 *          1. value: stores elements
 *      3. dnode_base:
 *          1. type: indicates dnode or dleaf
 *          2. p : dnode *
 *          3. count(): count of elements
 *      4. dvector:
 *          1. head: dnode_base
 *  trivial methods:
 *      1. _access_node(node, index)
 *          if index >= node.count() or index < 0:
 *              raise
 *          while node.type != dleaf:
 *              if index < node.left.count():
 *                  node = node.left
 *              else:
 *                  index = index - node.left.count
 *                  node = node.right
 *          return (dleaf)node, index
 *      2. _build_parent(node)
 *          p = new dnode()
 *          _replace_node(node, p)
 *          return p
 *      3. _replace_node(new, old)
 *      4. _get_successor(node)
 *  methods:
 *      1. access(index)
 *          node, local_index =  _access_node(this.head, index)
 *          return node.value[local_index]
 *      2. _insert_node(node, index, elements) 
 *          leaf, local_index = _access_node(node, index)
 *          local_len = node.count()
 *          p = _build_parent(node)
 *          
 *          if local_index == 0:
 *              p.left = new dleaf(elements)
 *                new_node.p = p
 *              p.right = leaf
 *              _rebalance(p, 1)
 *          elif local_index == local_len - 1:
 *              p.left = leaf
 *              new_node.p = p
 *              p.right = new dleaf(elements)
 *              _rebalance(p, 1)
 *          else:
 *              p.left = new dleaf(node.value[0:local_index])
 *              p.right = new dleaf(elements)
 *              p.left.p = p
 *              _rebalance(p, 1)
 *              _insert_node(_get_successor(p.right), 0, leaf.value[local_index:])
 *              delete leaf
 *  
 */

namespace dv
{
    template<class T, class Allocator = std::allocator<T>>
    class dvector : protected dvector_base<T, Allocator>
    {
    private:
        using Base = dvector_base<T, Allocator>;
        using Self = dvector<T, Allocator>;
        using Vector = Base::Vector;
        using LeafType = Base::LeafType;
    public:
        dvector(const Vector & init_vector) : Base(init_vector) 
        {
        }

        dvector(Vector && init_vector) : Base(std::move(init_vector))
        {
        }

        const T & operator[] (const size_t index) const
        {
            LeafType * l;
            size_t local_index;
            std::tie(l, local_index) = Base::_access_node(Base::root, index);

            return l->value[local_index];
        }

        std::string visualize()
        {
            return Base::visualize();
        }

        void insert(const size_t index, const Vector & elements)
        {
            Base::_insert_node(this->root, index, elements);
        }

        void insert(const size_t index, Vector && elements)
        {
            Base::_insert_node(this->root, index, std::move(elements));
        }

        void insert(const size_t index, const T & element)
        {
            Base::_insert_node(this->root, index, Vector{ element });
        }

        void insert(const size_t index, T && element)
        {
            {
                Vector temp;
                temp.push_back(std::move(element));
                Base::_insert_node(this->root, index, std::move(temp));
            }
        }


    };
}