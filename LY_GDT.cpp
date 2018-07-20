#include "LY_GDT.hpp"
#include "LY_Screen.hpp"

struct LY::GDT::Entry gdt[3];
struct LY::GDT::Ptr gp;
	
static void gdt_flush()
{
	asm volatile("lgdtl (gp)");
	asm volatile(
		"movw $0x10, %ax \n"
		"movw %ax, %ds \n"
		"movw %ax, %es \n"
		"movw %ax, %fs \n"
		"movw %ax, %gs \n"
		"movw %ax, %ss \n"
		"ljmp $0x08, $next \n"
		"next:          \n"
	);
}
void LY::GDT::SetGate(int num, uint32_t base, uint32_t limit, uint8_t access,uint8_t gran)
{
	gdt[num].base_low=(base & 0xFFFF);
	gdt[num].base_middle=(base >> 16) & 0xFF;
	gdt[num].base_high=(base >> 24) & 0xFF;
	
	gdt[num].limit_low=(limit & 0xFFFF);
	gdt[num].granularity=(limit >> 16) & 0x0F;
	
	gdt[num].granularity |= (gran & 0xF0);
	gdt[num].access=access;
}
void LY::GDT::Install()
{
	LY::Screen::SetColor(LY_SIDE_FOREGROUND,LY_COLOR_WHITE);
	LY::Screen::PutString("Installing GDT...");

	gp.limit=(sizeof(struct LY::GDT::Entry)*3)-1;
	gp.base=(uint32_t)&gdt;
	
	LY::GDT::SetGate(0,0,0,0,0); /* NULL segmente entry */
	LY::GDT::SetGate(1,0,0xFFFFFFFF,0x9A,0xCF); /* 4 GiB for Code Segment */
	LY::GDT::SetGate(2,0,0xFFFFFFFF,0x92,0xCF); /* 4 GiB for Data segment */
	
	gdt_flush();
	
	LY::Screen::SetColor(LY_SIDE_FOREGROUND,LY_COLOR_WHITE);
	LY::Screen::PutString("done");
}
