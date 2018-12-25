#pragma once

#include <cassert>
#include <tuple>
#include "dnode.h"
#include "dslice.h"
#include "dleaf.h"
#include "dalloc.h"
#include <functional>
#include <sstream>
#include <algorithm>

#define DV_CHECKH(x) assert(((x)->h <= 1) && ((x)->h >= -1));

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
            
            if (this->root == old_node) { 
                this->root = new_node; 
                new_node->p = nullptr;
            } // If old_node takes the root
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

        dnode_base * left_rotate(dnode * x)
        {
            /*
                   x                  y
                  / \                / \ 
                 a   y      -->     x   c
                    / \            / \
                   b   c          a   b
             */
            auto y = static_cast<dnode*>(x->right);
            auto b = y->left;
            _replace_node(y, x);
            x->set_right(b);
            y->set_left(x);
            x->elem_count = x->left->count() + x->right->count();
            y->elem_count = x->elem_count + y->right->count();
            return y;
        }

        dnode_base * right_rotate(dnode * y)
        {
            /*
                   x                  y
                  / \                / \
                 a   y      <--     x   c
                    / \            / \
                   b   c          a   b
             */
            auto x = static_cast<dnode*>(y->left);
            auto b = x->right;
            _replace_node(x, y);
            x->set_right(y);
            y->set_left(b);
            y->elem_count = y->left->count() + y->right->count();
            x->elem_count = x->left->count() + y->elem_count;
            return x;
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

        void erase(const size_t index) noexcept
        {
            LeafType * leaf;
            size_t local_index;

            std::tie(leaf, local_index) = this->_access_node(root_node, index);

            if (local_index == 0)
            {
                auto old_slice_right = this->leaf_construct(SliceType(1, local_len, leaf->value));
                _replace_node(old_slice_right, leaf);
                //_rebalance(p, 1); ##TODO##
            }
            else if (local_index == local_len)
            {
                p->set_left(leaf);
                p->set_right(new_node);
                p->renew_count();
                _rebalance(p, 1);
            }
            else
            {
                auto old_slice_left = this->leaf_construct(SliceType(0, local_index, leaf->value));
                auto old_slice_right = this->leaf_construct(SliceType(local_index, local_len, leaf->value));
                p->set_left(old_slice_left);
                p->set_right(new_node);
                _rebalance(p, 1);
                // std::cout << "inserting again" << std::endl;
                _insert_node(new_node, new_node->count(), old_slice_right); // Insert next to new_node
                this->destroy(leaf); // Old leaf no longer there
            }
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

            if (local_index == 0)
            {
                p->set_left(new_node);
                p->set_right(leaf);
                p->renew_count();
                _rebalance(p, 1);
            }
            else if (local_index == local_len)
            {
                p->set_left(leaf);
                p->set_right(new_node);
                p->renew_count();
                _rebalance(p, 1);
            }
            else
            {
                auto old_slice_left = this->leaf_construct(SliceType(0, local_index, leaf->value));
                auto old_slice_right = this->leaf_construct(SliceType(local_index, local_len, leaf->value));
                p->set_left(old_slice_left);
                p->set_right(new_node);
                _rebalance(p, 1);
                // std::cout << "inserting again" << std::endl;
                _insert_node(new_node, new_node->count(), old_slice_right); // Insert next to new_node
                this->destroy(leaf); // Old leaf no longer there
            }
        }

        void _rebalance(dnode * subtree, int height_change)
        {
            dnode * current = static_cast<dnode*>(subtree);
            char last_place = '\0';
            char current_place = '\0';
            bool increase_height = true;

            // std::cout << "_rebalance loop starts" << std::endl;
            while (current != nullptr && current->p != nullptr)
            {
                DV_CHECKH(current)

                if (current->p->is_left_child(current))
                {
                    current_place = 'l';
                }
                else
                {
                    current_place = 'r';
                }

                if (increase_height)
                {
                    current->p->renew_height(current_place, height_change);
                    if (current->p->h == 0) // Can no longer affect more heights
                    {
                        increase_height = false;
                        break;
                    }
                }


                // std::cout << "1. current->p->h == " << (int)current->p->h << std::endl;
                // std::cout << visualize(this->root, false);

                if (current->p->h == -2) 
                {
                    /* Right-Right
                           y                          x
                          / \                       /   \
                         h   x             ->      y    h+1
                            / \                   / \
                      (h,h+1)  (h+1)             h  (h, h+1)
                     */
                    dnode * x = current;
                    int x_h = current->h;
                    dnode * y = current->p;
                    if (last_place == 'l')
                    {
                        /* Right-Left
                                 y                                               y
                                / \                                             / \
                               h   x'                                          h   x
                                  / \                                ->           / \
                                 x  h-1                                          a   x'
                                / \                                                 / \
                              h-1 h-1    or   h-1  h-2   or   h-2   h-1           b   h-1

                              b - (h-1) = 0 -> 0; 1 -> -1; -1 -> 0;
                              x_h = a - (max(b, h-1) + 1) = a - h 
                              a - h = {0, 1} -> -1; -1 -> -2;
                         */
                        dnode * x_ = static_cast<dnode*>(y->right);
                        x = static_cast<dnode*>(x_->left);

                        if (x->h <= 0)
                        {
                            x_->h = 0;
                        } 
                        else
                        {
                            x_->h = -1;
                        }

                        if (x->h >= 0)
                        {
                            x_h = -1;
                        }
                        else
                        {
                            x_h = -2;
                        }

                        // std::cout << "RR: " << static_cast<dnode*>(x_)->id << std::endl;
                        current = static_cast<dnode*>(right_rotate(x_));
                        DV_CHECKH(x);
                        // std::cout << visualize(this->root, false);
                        // std::cout << "x': " << static_cast<dnode*>(x_)->id << std::endl;
                    }

                    // std::cout << "x: " << static_cast<dnode*>(x)->id << std::endl;
                    // std::cout << "y: " << static_cast<dnode*>(y)->id << std::endl;
                    
                    // std::cout << "x_h: " << (int)x_h << std::endl;
                    y->h = -1 - x_h;
                    if (x_h <= -1) x->h = 0;
                    else x->h = 1;

                    // std::cout << "LR: " << static_cast<dnode*>(y)->id << std::endl;
                    current = static_cast<dnode*>(left_rotate(y));
                    // x_h = 0 -> x : (h+1, h+1) -> y : (h, h+1) -> y_h = -1; 
                    // x_h = -1 -> x : (h, h+1) -> y : (h, h) -> y_h = 0; 

                    // std::cout << visualize(this->root);
                    break;
                }
                else if (current->p->h == 2)
                {
                    /* Left-Left
                                   y                   x
                                  / \                 / \
                                 x   h         ->  h+1   y
                                / \                     / \
                          (h+1)  (h,h+1)          (h, h+1) h
                    */

                    dnode * x = current;
                    int x_h = current->h;
                    dnode * y = current->p;

                    if (last_place == 'r')
                    {
                        /* Left-Right
                             y                                               y
                            / \                                             / \
                           x'                                              x   h
                          / \                                             / \
                        h-1  x                                           x'  a
                            / \                                         / \       
                          h-1 h-1    or    h-1  h-2   or   h-2   h-1  h-1  b   
                           b   a
                              0               1                 -1
                              0                0                1
                          (h-1) - b = -1 -> 0; 1 -> 1; 0 -> 0;
                          x_h = (max(b, h-1) + 1) - a = h - a
                          h - a = (0, -1) -> 1; 1 -> 2;
                        */
                        dnode * x_ = static_cast<dnode*>(y->left);
                        x = static_cast<dnode*>(x_->right);

                        if (x->h >= 0)
                        {
                            x_->h = 0;
                            // std::cout << "x: " << static_cast<dnode*>(x)->id << std::endl;
                            // std::cout << "x->h: " << (int)x->h << std::endl;
                            // std::cout << "set x'->h: " << (int)x_->h << std::endl;
                        }
                        else
                        {
                            x_->h = 1;
                            // std::cout << "x: " << static_cast<dnode*>(x)->id << std::endl;
                            // std::cout << "x->h: " << (int)x->h << std::endl;
                            // std::cout << "set x'->h: " << (int)x_->h << std::endl;
                        }

                        if (x->h <= 0)
                        {
                            x_h = 1;
                        }
                        else
                        {
                            x_h = 2;
                        }

                        // std::cout << "x': " << static_cast<dnode*>(x_)->id << std::endl;

                        // std::cout << "LR: " << static_cast<dnode*>(x_)->id << std::endl;
                        current = static_cast<dnode*>(left_rotate(x_));
                        // std::cout << visualize(this->root, false);
                    }

                    y->h = 1 - x_h;
                    if (x_h >= 1) x->h = 0;
                    else x->h = -1;
                    // x_h = 0 -> x : (h+1, h+1) -> y : (h+1, h) -> y_h = 1; 
                    // x_h = 1 -> x : (h+1, h) -> y : (h, h) -> y_h = 0; 

                    // std::cout << "x: " << static_cast<dnode*>(x)->id << std::endl;
                    // std::cout << "y: " << static_cast<dnode*>(y)->id << std::endl;
                    // std::cout << "x_h: " << x_h << std::endl;

                    // std::cout << "RR: " << static_cast<dnode*>(y)->id << std::endl;
                    current = static_cast<dnode*>(right_rotate(y));

                    // std::cout << visualize(this->root);
                    break;
                }
                else
                {
                    // std::cout << "2. current->p->h == " << (int)current->p->h << std::endl;
                    //std::cout << visualize(this->root);
                    assert((current->p->h <= 1) && (current->p->h >= -1));
                }
                

                last_place = current_place;
                current_place = '\0';
                current = current->p;
            }
        }

        template<typename R>
        static
        R _post_order_traverse(dnode_base * node, const std::function<R(const R &, const R &, dnode_base*)> & action)
        {
            if (node != nullptr) 
            {
                if (node->type == 'n') 
                {
                    dnode * dn = static_cast<dnode*>(node);
                    R r1 = _post_order_traverse<R>(dn->left, action);
                    R r2 = _post_order_traverse<R>(dn->right, action);
                    return action(r1, r2, node);
                }
                else if (node->type == 'l')
                {
                    return action(R(), R(), node);
                }
                else
                {
                    assert(false);
                }
            }
            return R();
        }

        static
        void _pre_order_traverse(dnode_base * node, const std::function<void(dnode_base*, int)> & action, int level = 0)
        {
            if (node != nullptr)
            {
                if (node->type == 'n')
                {
                    dnode * dn = static_cast<dnode*>(node);
                    action(node, level);
                    _pre_order_traverse(dn->right, action, level + 1);
                    _pre_order_traverse(dn->left, action, level + 1);
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

        static
        std::string visualize(dnode_base * node, bool check = true)
        {
            std::stringstream ss;
            _pre_order_traverse(node, [&ss](dnode_base* ptr, int level) {
                for (int i = 0; i < level; i++)
                {
                    ss << "  ";
                }
                switch (ptr->type)
                {
                case 'n':
                    assert(static_cast<dnode*>(ptr)->left->p == ptr);
                    assert(static_cast<dnode*>(ptr)->right->p == ptr);
                    ss << static_cast<dnode*>(ptr)->id << "(" << (int)static_cast<dnode*>(ptr)->h << ")";
                    break;
                case 'l':
                    ss << "[";
                    for (size_t i = 0; i < static_cast<LeafType*>(ptr)->value.size(); i++)
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
            if (!check_height(node) && check)
            {
                std::cout << ss.str();
                assert(false);
            }
            return ss.str();
        }

        static
        bool check_height(dnode_base * base)
        {
            bool good = true;

            _post_order_traverse<int>(base, [&good](int lh, int rh, auto ptr)
            {
                if (ptr->type == 'l') return 0;
                else
                {
                    auto p = (dnode*)ptr;
                    if (p->h != lh - rh) good = false;
                    if (p->h >= 2 || p->h <= -2) good = false;
                    return std::max(lh, rh) + 1;
                }
            });
            return good;
        }

        ~dvector_base()
        {
            _post_order_traverse<int>(this->root, [this](int, int, dnode_base* ptr) { this->destroy(ptr); return 0; });
        }

    };
}