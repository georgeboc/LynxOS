#include "LY_IDT.hpp"
#include "LY_Screen.hpp"
#include "LY_Color.hpp"
#include "LY_Memory.hpp"
#include "LY_ISR.hpp"
#include "LY_IRQ.hpp"

struct LY::IDT::Entry idt[256];
struct LY::IDT::Ptr idtptr;

void LY::IDT::Flush()
{
	asm volatile("lidtl (idtptr)");
}
void LY::IDT::SetGate(uint8_t num,uint32_t base,uint16_t sel, uint8_t flags)
{
	idt[num].base_low=(base & 0xFFFF);
	idt[num].base_high=(base >> 16) & 0xFFFF;

	idt[num].sel=sel;
	idt[num].always0 = 0;
	idt[num].flags=flags;
}
void LY::IDT::Install()
{
	LY::Screen::SetColor(LY_SIDE_FOREGROUND,LY_COLOR_WHITE);
	LY::Screen::PutString("\nInstalling IDT...");

	idtptr.limit=(sizeof(struct LY::IDT::Entry)*256)-1;
	idtptr.base=(uint32_t)&idt;

	LY::Memory::Set(&idt,0,sizeof(struct LY::IDT::Entry)*256);

	/* Here it goes the ISR */

	LY::IDT::SetGate(0,(uint32_t)LY::ISR::ISR1,0x08,0x8E);
	LY::IDT::SetGate(1,(uint32_t)LY::ISR::ISR2,0x08,0x8E);
	LY::IDT::SetGate(2,(uint32_t)LY::ISR::ISR3,0x08,0x8E);
	LY::IDT::SetGate(3,(uint32_t)LY::ISR::ISR4,0x08,0x8E);
	LY::IDT::SetGate(4,(uint32_t)LY::ISR::ISR5,0x08,0x8E);
	LY::IDT::SetGate(5,(uint32_t)LY::ISR::ISR6,0x08,0x8E);
	LY::IDT::SetGate(6,(uint32_t)LY::ISR::ISR7,0x08,0x8E);
	LY::IDT::SetGate(7,(uint32_t)LY::ISR::ISR8,0x08,0x8E);
	LY::IDT::SetGate(8,(uint32_t)LY::ISR::ISR9,0x08,0x8E);
	LY::IDT::SetGate(9,(uint32_t)LY::ISR::ISR10,0x08,0x8E);
	LY::IDT::SetGate(10,(uint32_t)LY::ISR::ISR11,0x08,0x8E);
	LY::IDT::SetGate(11,(uint32_t)LY::ISR::ISR12,0x08,0x8E);
	LY::IDT::SetGate(12,(uint32_t)LY::ISR::ISR13,0x08,0x8E);
	LY::IDT::SetGate(13,(uint32_t)LY::ISR::ISR14,0x08,0x8E);
	LY::IDT::SetGate(14,(uint32_t)LY::ISR::ISR15,0x08,0x8E);
	LY::IDT::SetGate(15,(uint32_t)LY::ISR::ISR16,0x08,0x8E);
	LY::IDT::SetGate(16,(uint32_t)LY::ISR::ISR17,0x08,0x8E);
	LY::IDT::SetGate(17,(uint32_t)LY::ISR::ISR18,0x08,0x8E);
	LY::IDT::SetGate(18,(uint32_t)LY::ISR::ISR19,0x08,0x8E);
	LY::IDT::SetGate(19,(uint32_t)LY::ISR::ISR20,0x08,0x8E);
	LY::IDT::SetGate(20,(uint32_t)LY::ISR::ISR21,0x08,0x8E);
	LY::IDT::SetGate(21,(uint32_t)LY::ISR::ISR22,0x08,0x8E);
	LY::IDT::SetGate(22,(uint32_t)LY::ISR::ISR23,0x08,0x8E);
	LY::IDT::SetGate(23,(uint32_t)LY::ISR::ISR24,0x08,0x8E);
	LY::IDT::SetGate(24,(uint32_t)LY::ISR::ISR25,0x08,0x8E);
	LY::IDT::SetGate(25,(uint32_t)LY::ISR::ISR26,0x08,0x8E);
	LY::IDT::SetGate(26,(uint32_t)LY::ISR::ISR27,0x08,0x8E);
	LY::IDT::SetGate(27,(uint32_t)LY::ISR::ISR28,0x08,0x8E);
	LY::IDT::SetGate(28,(uint32_t)LY::ISR::ISR29,0x08,0x8E);
	LY::IDT::SetGate(29,(uint32_t)LY::ISR::ISR30,0x08,0x8E);
	LY::IDT::SetGate(30,(uint32_t)LY::ISR::ISR31,0x08,0x8E);
	LY::IDT::SetGate(31,(uint32_t)LY::ISR::ISR32,0x08,0x8E);

	LY::IDT::Flush();
//	asm volatile ("sti");

	LY::Screen::PutString("done");
}
