#include "LY_Memory.hpp"
#define MAX_PAGE_ALIGNED_ALLOCS 32

uint32_t last_alloc = 0;
uint32_t heap_end = 0;
uint32_t heap_begin = 0;
uint32_t pheap_begin = 0;
uint32_t pheap_end = 0;
uint8_t *pheap_desc = 0;
uint32_t memory_used = 0;

void* LY::Memory::Set(void* buf, int c, size_t len)
{
	unsigned char* tmp=(unsigned char*)buf;
	while(len--)
	{
		*tmp++=c;
	}
	return buf;
}

void* LY::Memory::Set16(void* buf, uint16_t c, uint32_t len)
{
	uint16_t* tmp = (uint16_t*) buf;
	while(len--)
	{
		*tmp++=c;
	}
	return buf;
}

void* LY::Memory::Copy(void* dest,const void* src, size_t len)
{
	const unsigned char* sp=(const unsigned char*)src;
	unsigned char* dp=(unsigned char*)dest;
	for(;len!=0;len--) *dp++=*sp++;
	return dest;
}

int LY::Memory::Compare(const void* p1, const void* p2, size_t len)
{
	const char* a=(const char*)p1;
	const char* b=(const char*)p2;
	size_t i=0;
	for(;i<len;i++)
	{
		if(a[i] < b[i])
			return -1;
		else if(a[i] > b[i])
			return 1;
	}
	return 0;
}

void LY::Memory::MM_init(uint32_t kernel_end)
{
	last_alloc = kernel_end + 0x1000;
	heap_begin = last_alloc;
	pheap_end = 0x400000;
	pheap_begin = pheap_end - (MAX_PAGE_ALIGNED_ALLOCS * 4096);
	heap_end = pheap_begin;
	LY::Memory::Set((char *)heap_begin, 0, heap_end - heap_begin);
	pheap_desc = (uint8_t *)LY::Memory::Malloc(MAX_PAGE_ALIGNED_ALLOCS);
	/*LY::Screen::PutString("\nKernel heap starts at: ");
	LY::Screen::itoa(last_alloc);*/
}

void LY::Memory::MM_print_out()
{
	LY::Screen::PutString("\nMemory used: ");
	LY::Screen::itoa(memory_used);
	LY::Screen::PutString("bytes\nMemory free: ");
	LY::Screen::itoa(heap_end - heap_begin - memory_used);
	LY::Screen::PutString("bytes\nHeap size: ");
	LY::Screen::itoa(heap_end - heap_begin);
	LY::Screen::PutString("bytes\nHeap start: ");
	LY::Screen::itoa(heap_begin);
	LY::Screen::PutString("\nHeap end: ");
	LY::Screen::itoa(heap_end);
	LY::Screen::PutString("\nPheap start: ");
	LY::Screen::itoa(pheap_begin);
	LY::Screen::PutString("\nPheap end: ");
	LY::Screen::itoa(pheap_end);
}

void LY::Memory::Free(void *mem)
{
	/*alloc_t *alloc = (mem - sizeof(alloc_t));
	memory_used -= alloc->size + sizeof(alloc_t);
	alloc->status = 0;*/
}

void LY::Memory::PFree(void *mem)
{
	/*if(mem < pheap_begin || mem > pheap_end) return;
	uint32_t ad = (uint32_t)mem;
	ad -= pheap_begin;
	ad /= 4096;
	pheap_desc[ad] = 0;
	return;*/
}

char* LY::Memory::PMalloc(size_t size)
{
	/*for(int i = 0; i < MAX_PAGE_ALIGNED_ALLOCS; i++)
	{
		if(pheap_desc[i]) continue;
		pheap_desc[i] = 1;
		LY::Screen::PutString("\nPAllocated from ");
		LY::Screen::itoa(pheap_begin + i * 4096);
		LY::Screen::PutString("to "); 
		LY::Screen::itoa(pheap_begin + (i+1) * 4096);
		return (char *)(pheap_begin + i * 4096);
	}
	LY::Screen::PutString("\nPMalloc: FATAL: failure!\n ");
	return 0;*/
}

char* LY::Memory::Malloc(size_t size)
{
	if(!size) return 0;
	
	uint8_t *mem = (uint8_t *)heap_begin;
	while((uint32_t)mem < last_alloc)
	{
		alloc_t *a = (alloc_t *)mem;
		if(!a->size)
			goto nalloc;

		if(a->status)
		{
			mem += a->size;
			mem += sizeof(alloc_t);
			mem += 4;
			continue;
		}
		if(a->size >= size)
		{
			a->status = 1;
			/*LY::Screen::PutString("\nRE:Allocated ");
			LY::Screen::itoa((long long unsigned int)size);
			LY::Screen::PutString("bytes from ");
			LY::Screen::itoa((long long unsigned int)mem + sizeof(alloc_t));
			LY::Screen::PutString(" to ");
			LY::Screen::itoa((long long unsigned int)mem + sizeof(alloc_t) + size);
			LY::Screen::PutString("\n");*/
			LY::Memory::Set((void*)(mem + sizeof(alloc_t)), 0, size);
			memory_used += size + sizeof(alloc_t);
			return (char *)(mem + sizeof(alloc_t));
		}

		mem += a->size;
		mem += sizeof(alloc_t);
		mem += 4;
	}
	
	nalloc:;

	if(last_alloc + size + sizeof(alloc_t) >= heap_end)
	{
		LY::Panic::Show_Set();				
		LY::Screen::PutString("Cannot allocate ");
		LY::Screen::itoa(size);
		LY::Screen::PutString(" bytes! Out of memory.\n");
		LY::Memory::MM_print_out();
	}
	
	alloc_t *alloc = (alloc_t *)last_alloc;
	alloc->status = 1;
	alloc->size = size;
	
	last_alloc += size;
	last_alloc += sizeof(alloc_t);
	last_alloc += 4;
	/*LY::Screen::PutString("\nAllocated ");
	LY::Screen::itoa(size);
	LY::Screen::PutString(" bytes from ");
	LY::Screen::itoa((uint32_t)alloc + sizeof(alloc_t));
	LY::Screen::PutString(" to ");
	LY::Screen::itoa(last_alloc);*/
	memory_used += size + 4 + sizeof(alloc_t);
	LY::Memory::Set((char *)((uint32_t)alloc + sizeof(alloc_t)), 0, size);
	return (char *)((uint32_t)alloc + sizeof(alloc_t));
}












