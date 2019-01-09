#pragma once

#include <cstdint>
#include "dnode_base.h"

namespace dv
{
    struct dnode : public dnode_base
    {
        static inline char _id = 'a';

        int8_t h = 0;
        dnode_base * left = nullptr;
        dnode_base * right = nullptr;
        size_t elem_count = 0;
        char id;

        dnode()
        {
            id = _id;
            _id = (_id - 'a') % 26 + 1 + 'a';
            dnode_base::type = 'n';
        }

        virtual size_t count() const noexcept override
        {
            return elem_count;
        }

        bool is_left_child(dnode_base * n)
        {
            return this->left == n;
        }

        bool is_right_child(dnode_base * n)
        {
            return this->right == n;
        }

        void set_left(dnode_base * n)
        {
            this->left = n;
            n->p = this;
        }

        void set_right(dnode_base * n)
        {
            this->right = n;
            n->p = this;
        }

        void renew_count() noexcept
        {
            this->elem_count = this->left->count() + this->right->count();
            if (this->p != nullptr)
            {
                this->p->renew_count();
            }
        }

        void renew_height(char source, int height_change) noexcept
        {
            if (source == 'l')
            {
                this->h += height_change;
            }
            else
            {
                this->h -= height_change;
            }
        }

        virtual ~dnode() { /*std::cout << "~dnode is called!!!";*/ }
    };
}