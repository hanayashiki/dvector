#pragma once

#include <cstdlib>
#include <map>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <new>

namespace dbgalloc
{
	extern size_t alloc_no;
	extern size_t stop_no;
	extern std::map<void *, size_t> no_to_ptr;

	void set_exception_on_alloc_no(size_t no);

	class Dbg
	{
	public:
		Dbg()
		{
			alloc_no = 0;
		}

		~Dbg()
		{
			alloc_no = 0;
			if (no_to_ptr.size() != 0)
			{
				std::cerr << "Memory leak detected. " << std::endl;
				std::vector<std::pair<const void *, size_t>> pairs(no_to_ptr.begin(), no_to_ptr.end());
				std::sort(pairs.begin(), pairs.end(), [](auto & a, auto & b) { return a.second < b.second; });
				for (auto p : pairs)
				{
					std::cerr << "[" << p.second << "] : ##TODO: MORE INFO##" << std::endl;
				}
			}
		}
	};

	template <typename T>
	class DbgAlloc
	{
	public:
		// typedefs...
		typedef T                   value_type;
		typedef value_type*         pointer;
		typedef value_type&         reference;
		typedef value_type const*   const_pointer;
		typedef value_type const&   const_reference;
		typedef size_t              size_type;
		typedef ptrdiff_t           difference_type;

		// rebind...
		template<class U> struct rebind { typedef DbgAlloc<U> other; };

		DbgAlloc() {}
		
		template<class U>
		DbgAlloc(const DbgAlloc<U> &) {}

		[[nodiscard]] pointer allocate(size_type cnt)
		{
			
			if (cnt <= 0)
			{
				return 0;
			}

			void* pMem = nullptr;
			if ((pMem = malloc(cnt * sizeof(value_type))) == NULL)
			{
				throw std::bad_alloc();
			}
			
			if (alloc_no == stop_no)
			{
				std::stringstream s;
				s << "alloc exception by dbgalloc::set_exception_on_alloc_no: alloc_no == " << alloc_no << std::endl;
				std::cerr << s.str();
				throw std::exception();
			}

			no_to_ptr.emplace(pMem, alloc_no++);
			//std::cout << "allocated : " << no_to_ptr.size() << std::endl;

			return static_cast<pointer>(pMem);
		}

		void deallocate(pointer p, size_type)
		{
			no_to_ptr.erase((void*)p);
			//std::cout << "deallocated : " << no_to_ptr.size() << std::endl;
			free(p);
		}

	};

	template<typename T, typename U>
	constexpr bool operator== (const DbgAlloc<T>&, const DbgAlloc<U>&) noexcept {
		return true;
	}

	template<typename T, typename U>
	constexpr bool operator!= (const DbgAlloc<T>&, const DbgAlloc<U>&) noexcept {
		return false;
	}
}