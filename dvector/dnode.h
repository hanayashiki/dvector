#pragma once

#include <cstdint>
#include "dnode_base.h"

namespace dv
{
	struct dnode : public dnode_base
	{
		int8_t h = 0;
		dnode_base * left = nullptr;
		dnode_base * right = nullptr;
		size_t elem_count = 0;

		dnode()
		{
			dnode_base::type = 'n';
		}

		virtual ~dnode() {}
	};
}