#pragma once
#include <memory>

#include "dleaf.h"
#include "dnode.h"

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

		LeafAlloc leaf_alloc;
		NodeAlloc node_alloc;
		NodeBaseAlloc node_base_alloc;

		template<typename... Args>
		LeafType * leaf_construct(Args&&... args)
		{
			auto ptr = leaf_alloc.allocate(1);
			ptr = new (ptr) LeafType(std::forward<Args>(args)...);
			return ptr;
		}

		template<typename... Args>
		dnode * node_construct(Args&&... args)
		{
			auto ptr = node_alloc.allocate(1);
			ptr = new (ptr) dnode(std::forward<Args>(args)...);
			return ptr;
		}

		template<typename T>
		void destory(T *)
		{
			throw std::exception("Not implemented destructor for T. ");
		}

		template<>
		void destory<LeafType>(LeafType * n)
		{
			n->~dleaf<T>();
			leaf_alloc.deallocate(n, 1);
		}

		template<>
		void destory<dnode>(dnode * n)
		{
			n->~dnode();
			node_alloc.deallocate(n, 1);
		}

		template<>
		void destory<dnode_base>(dnode_base * n)
		{
			n->~dnode_base();
			node_base_alloc.deallocate(n, 1);
		}
	};
}