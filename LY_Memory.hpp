#ifndef LY_MEMORY_HPP
#define LY_MEMORY_HPP

#include "LY_Types.hpp"

typedef struct
{
	uint8_t status;
	uint32_t size;
} alloc_t;

namespace LY{
		namespace Memory{
				void* Set(void* buf, int c, size_t len);
				void* Set16(void* buf, uint16_t c, uint32_t len);
				void* Copy(void* dest,const void* src, size_t len);
				int Compare(const void* p1, const void* p2, size_t len);
				void MM_init(uint32_t kernel_end);
				void MM_print_out();
				void Free(void *mem);
				void PFree(void *mem);
				char* PMalloc(size_t size);
				char* Malloc(size_t size);
			}
	}


#endif
