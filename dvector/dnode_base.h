#pragma once

namespace dv
{
	struct dnode;

	struct dnode_base 
	{
		char type; /*
				     'n' : indicates non-leaf node
					 'l' : indicates leaf node
				   */
		dnode * p = nullptr;
		virtual size_t count() const noexcept = 0;
		virtual ~dnode_base() {}
	};
}