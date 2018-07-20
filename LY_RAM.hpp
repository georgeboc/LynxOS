#include "LY_Multiboot.hpp"

namespace LY{
	namespace RAM{
			void Write(int address, int data);
			int Read(int address);
			void Information(void);
			void Status(void);
			void Read_mmap(multiboot_info_t* mbt);
			unsigned long long GetMmapLength(void);
			unsigned long long GetMmapLength(multiboot_info_t* mbt);
		}
}
