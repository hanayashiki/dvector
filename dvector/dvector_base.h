#pragma once

#include <cassert>
#include <tuple>
#include "dnode.h"
#include "dslice.h"
#include "dleaf.h"
#include "dalloc.h"
#include <functional>
#include <sstream>

namespace dv
{
    template<class T, class Allocator = std::allocator<T>>
    struct dvector_base : public dvector_alloc_base<T, Allocator>
    {
    public:
        using Vector = std::vector<T, Allocator>;
        using ElemType = T;
        using LeafType = dleaf<T, Allocator>;
        using SliceType = dslice<T, Allocator>;

        dnode_base * root;

        dvector_base(const Vector & init_vector)
        {
            root = this->leaf_construct(SliceType(0, init_vector.size(), init_vector));
        }

        dvector_base(Vector && init_vector)
        {
            root = this->leaf_construct(SliceType(0, init_vector.size(), std::move(init_vector)));
        }

        void _replace_node(dnode_base * new_node, dnode_base * old_node)
        {
            // The place of old_node is taken by new_node
            
            if (this->root == old_node) { this->root = new_node; } // If old_node takes the root
            else
            {
                // old_node has parent
                auto p = old_node->p;
                new_node->p = p;
                // parent's child is renewed
                if (p->left == old_node) { p->left = new_node; } 
                else { p->right = new_node; }
            }
        }

        dnode * _build_parent(LeafType * leaf)
        {
            /*       o                   o
                    / \                 / \
                 leaf       ->         p
                                      / \
                                   null null
             */
            dnode * p = this->node_construct();
            _replace_node(p, leaf);
            return p;
        }

        dnode * _forced_get_uncle(dnode * n)
        {
            return nullptr;
        }

        std::tuple<LeafType*, size_t>
        _access_node(dnode_base * node, const size_t index) const
        {
            // std::cout << "node->count() == " << node->count() << std::endl;
            assert(index <= node->count()); // tail + 1 also legal here
            auto local_index = index;
            while (node->type != 'l')
            {
                auto n = (dnode *)node;
                if (local_index < n->left->count())
                {
                    node = n->left;
                }
                else 
                {
                    local_index = local_index - n->left->count();
                    node = n->right;
                }
            }
            return std::make_tuple((LeafType*)node, local_index);
        }

        void _insert_node(dnode_base * root_node, const size_t index, const Vector & elements)
        {
            _insert_node(root_node, index, this->leaf_construct(SliceType(0, elements.size(), elements)));
        }

        void _insert_node(dnode_base * root_node, const size_t index, Vector && elements)
        {
            _insert_node(root_node, index, this->leaf_construct(SliceType(0, elements.size(), std::move(elements))));
        }

        void _insert_node(dnode_base * root_node, const size_t index, LeafType * new_node)
        {
            LeafType * leaf;
            size_t local_index;

            std::tie(leaf, local_index) = this->_access_node(root_node, index); 
            // Find insertion place

            size_t local_len = leaf->count();
            dnode * p = _build_parent(leaf);

            leaf->p = p;
            new_node->p = p;

            if (local_index == 0)
            {
                p->left = new_node;
                p->right = leaf;
                p->renew_count();
                // _rebalance
            }
            else if (local_index == local_len)
            {
                p->left = leaf;
                p->right = new_node;
                p->renew_count();
                // _rebalance
            }
            else
            {
                p->left = this->leaf_construct(SliceType(0, local_index, leaf->value));
                p->left->p = p;
                p->right = new_node;
                // _rebalance
                // _insert_node()
                this->destory(p->left);
            }
        }

        void _post_order_traverse(dnode_base * node, const std::function<void(dnode_base*)> & action)
        {
            if (node != nullptr) 
            {
                if (node->type == 'n') 
                {
                    dnode * dn = static_cast<dnode*>(node);
                    _post_order_traverse(dn->left, action);
                    _post_order_traverse(dn->right, action);
                    action(node);
                }
                else if (node->type == 'l')
                {
                    action(node);
                }
                else
                {
                    assert(false);
                }
            }
        }

        void _pre_order_traverse(dnode_base * node, const std::function<void(dnode_base*, int)> & action, int level = 0)
        {
            if (node != nullptr)
            {
                if (node->type == 'n')
                {
                    dnode * dn = static_cast<dnode*>(node);
                    action(node, level);
                    _pre_order_traverse(dn->right, action, level + 1);
                    _pre_order_traverse(dn->left, action, level+1);
                }
                else if (node->type == 'l')
                {
                    action(node, level);
                }
                else
                {
                    assert(false);
                }
            }
        }

        std::string visualize()
        {
            std::stringstream ss;
            this->_pre_order_traverse(this->root, [&ss](dnode_base* ptr, int level) {
                for (int i = 0; i < level; i++)
                {
                    ss << "  ";
                }
                switch (ptr->type)
                {
                case 'n':
                    ss << '*';
                    break;
                case 'l':
                    ss << "[";
                    for (int i = 0; i < static_cast<LeafType*>(ptr)->value.size(); i++)
                    {
                        ss << static_cast<LeafType*>(ptr)->value[i];
                        if (i != static_cast<LeafType*>(ptr)->value.size() - 1)
                        {
                            ss << ", ";
                        }
                    }
                    ss << "]";
                    break;
                }
                ss << std::endl;
            });
            return ss.str();
        }

        ~dvector_base()
        {
            _post_order_traverse(this->root, [this](auto ptr) { this->destory(ptr); });
        }

    };
}