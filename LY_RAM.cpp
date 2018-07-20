#include "LY_RAM.hpp"
//#include "LY_Multiboot.hpp"

#define MMAP_NEXT(m) \
			 (memory_map_t*)((uint32_t)m +m->size + sizeof(uint32_t))

unsigned int address=0;
unsigned short *memory = (unsigned short*) 0x00000;
int eax=0;

void LY::RAM::Write(int address, int data)
{
memory[address]=data;	
}

int LY::RAM::Read(int address)
{
return memory[address];	
}

void LY::RAM::Read_mmap(multiboot_info_t* mbt){
	memory_map_t* mmap=(memory_map_t*) mbt->mmap_addr;

	while((uint32_t)mmap < mbt->mmap_addr + mbt->mmap_length){
		mmap = MMAP_NEXT(mmap);
	}
	LY::Screen::SetColor(LY_SIDE_FOREGROUND, LY_COLOR_WHITE);
	LY::Screen::PutString("done");
	LY::Screen::PutString("\nRam detected: ");
	LY::Screen::itoa(mbt->mem_upper);
	LY::Screen::PutString(" KB\n");
	/*LY::Screen::PutString("Ram detected II: ");
	LY::Screen::itoa(mbt->mmap_addr);
	LY::Screen::PutString(" KB\n");*/
	//LY::Screen::PutString("Length: ");
	//LY::Screen::itoa(mbt->mmap_length);
	//LY::Screen::PutString(" KB\n");
	/*LY::Screen::PutString("Mem lower: ");
	LY::Screen::itoa(mbt->mem_lower);
	LY::Screen::PutString("\n");*/
	/*LY::Screen::PutString("Mem upper: ");
	LY::Screen::itoa(mbt->mem_upper);
	LY::Screen::PutString("\n");*/
	LY::Screen::PutString("Boot device: ");
	LY::Screen::itoa(mbt->boot_device);
	LY::Screen::PutString("\n");
}

unsigned long long LY::RAM::GetMmapLength(multiboot_info_t* mbt)
{
	memory_map_t* mmap=(memory_map_t*) mbt->mmap_addr;
	return mbt->mmap_length;
}

void LY::RAM::Information(void)
{
//for(int i=0;i<=100;i++){
//asm volatile("popl %eax");
//asm volatile("movw $0xE881,%ax");
//asm volatile("int $0x15");
//asm volatile("movl 0x100000,%eax");
//asm volatile("\t movl %%eax,%0":"=r"(address));
LY::Screen::SetColor(LY_SIDE_FOREGROUND, LY_COLOR_WHITE);
LY::Screen::PutString("\nSetup RAM controller...");
asm volatile("movl 0x100000,%eax");
asm volatile("\t movl %%eax,%0":"=r"(address));
LY::RAM::Read_mmap((multiboot_info_t*)address);
}
void LY::RAM::Status(void)
{
	
}

