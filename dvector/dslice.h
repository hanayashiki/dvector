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
			: base_vector(std::make_shared<Vector>(std::move(_base_vector)))
		{
		}

		_dslice_base(const Vector & _base_vector)
			: base_vector(std::make_shared<Vector>(_base_vector))
		{
		}

		_dslice_base(const std::shared_ptr<Vector> & _base_vector)
			: base_vector(_base_vector)
		{
		}

		_dslice_base(std::shared_ptr<Vector> && _base_vector)
			: base_vector(std::move(_base_vector))
		{
		}
	};

	template<class T, class Allocator = std::allocator<T>>
	class dslice : public _dslice_base<T, Allocator>
	{
	private:
		using Self = dslice<T, Allocator>;
		using Base = _dslice_base<T, Allocator>;
		const size_t start;
		const size_t end;
	public:
		template<class... InitArgs>
		dslice(const size_t start, const size_t end, InitArgs&&... args) : 
			Base(std::forward<InitArgs>(args)...),
			start(start), end(end)
		{
		}

		const T & operator[] (const size_t index) const
		{
			assert(start + index < end);
			return (*Base::base_vector)[start + index];
		}

		dslice slice(const size_t _start, const size_t _end) const
		{
			assert(_start <= _end);
			assert(_end - _start <= size());
			return Self(start + _start, start + _end, Base::base_vector);
		}

		T & operator[] (const size_t index)
		{
			assert(start + index < end);
			return (*Base::base_vector)[start + index];
		}

		const size_t size() const noexcept
		{
			return end - start;
		}
	};
}