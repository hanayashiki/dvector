#pragma once
#include <vector>
#include <memory>
#include <cassert>

namespace dv
{
    template<class T, class Allocator = std::allocator<T>>
    class _dslice_base
    {
    protected:
        using Vector = std::vector<T, Allocator>;
        const std::shared_ptr<Vector> base_vector;
    public:
        _dslice_base(Vector && _base_vector)
            : base_vector(std::allocate_shared<Vector>(Allocator(), std::move(_base_vector)))
        {
            //std::cout << "_dslice_base called on move" << std::endl;
            //std::cout << "_dslice_base(): use_count == " << base_vector.use_count() << std::endl;
        }

        _dslice_base(const Vector & _base_vector)
            : base_vector(std::allocate_shared<Vector>(Allocator(), _base_vector))
        {
            //std::cout << "_dslice_base called on copy" << std::endl;
        }

        _dslice_base(const std::shared_ptr<Vector> & _base_vector)
            : base_vector(_base_vector)
        {
            //std::cout << "_dslice_base called on copy shared_ptr" << std::endl;
        }

        _dslice_base(std::shared_ptr<Vector> && _base_vector)
            : base_vector(std::move(_base_vector))
        {
            //std::cout << "_dslice_base called on move shared_ptr" << std::endl;
        }

        ~_dslice_base()
        {
            //std::cout << "~_dslice_base(): use_count == " << base_vector.use_count() << std::endl;
        }
    };

    template<class T, class Allocator = std::allocator<T>>
    class dslice : public _dslice_base<T, Allocator>
    {
    private:
        using Self = dslice<T, Allocator>;
        using Base = _dslice_base<T, Allocator>;
        using iterator = typename Base::Vector::iterator;
        using const_iterator = typename Base::Vector::const_iterator;

        size_t _start;
        size_t _end;
    public:
        template<class... InitArgs>
        dslice(const size_t start, const size_t end, InitArgs&&... args) : 
            Base(std::forward<InitArgs>(args)...),
            _start(start), _end(end)
        {
        }

        const T & operator[] (const size_t index) const
        {
            assert(_start + index < end);
            return (*Base::base_vector)[_start + index];
        }

        dslice slice(const size_t __start, const size_t __end) const
        {
            assert(_start <= _end);
            assert(_end - _start <= size());
            return Self(_start + __start, _start + __end, Base::base_vector);
        }

        T & operator[] (const size_t index)
        {
            assert(_start + index < _end);
            return (*Base::base_vector)[_start + index];
        }

        const size_t size() const noexcept
        {
            return _end - _start;
        }

        bool can_push_back()
        {
            //std::cout << "_end == " << _end << std::endl;
            return _end == (Base::base_vector->size());
        }

        void push_back(T && element) // Should be checked before can_push_back
        {
            Base::base_vector->push_back(std::move(element));
            _end += 1;
        }

        void push_back(const T & element) // Should be checked before can_push_back
        {
            Base::base_vector->push_back(element);
            _end += 1;
        }

        iterator begin()
        {
            return Base::base_vector->begin() + _start;
        }

        const_iterator begin() const
        {
            return Base::base_vector->begin() + _start;
        }

        iterator end()
        {
            return Base::base_vector->begin() + _end;
        }

        const_iterator end() const
        {
            return Base::base_vector->begin() + _end;
        }
    };
}