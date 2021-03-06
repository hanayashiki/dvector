#include "pch.h"
#include "dbgalloc.h"
#include <map>

namespace dbgalloc
{
    size_t alloc_no = 0;
    size_t stop_no = -1;
    std::map<void *, size_t> ptr_to_no;
    std::map<void *, size_t> ptr_to_size;

    void set_exception_on_alloc_no(size_t no)
    {
        stop_no = no;
    }
}