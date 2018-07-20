#include "LY_IRQ.hpp"
#include "LY_Ports.hpp"
#include "LY_IDT.hpp"
#include "LY_ISR.hpp"
#include "LY_Screen.hpp"

void *irq[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

extern "C"
void LY_IRQ_InstallHandler(int irq_code,void (*handler)(struct regs* r))
{
	irq[irq_code]=(void *)handler;
}
void LY::IRQ::UninstallHandler(int irq_code)
{
	irq[irq_code]=0;
}
void LY::IRQ::Install()
{
	LY::Screen::SetColor(LY_SIDE_FOREGROUND,LY_COLOR_WHITE);
	LY::Screen::PutString("\nInstalling IRQ...");

	LY::IRQ::Remap(0x20,0x28);

	LY::IDT::SetGate(32,(uint32_t)LY::IRQ::IRQ1,0x08,0x8E);
	LY::IDT::SetGate(33,(uint32_t)LY::IRQ::IRQ2,0x08,0x8E);
	LY::IDT::SetGate(34,(uint32_t)LY::IRQ::IRQ3,0x08,0x8E);
	LY::IDT::SetGate(35,(uint32_t)LY::IRQ::IRQ4,0x08,0x8E);
	LY::IDT::SetGate(36,(uint32_t)LY::IRQ::IRQ5,0x08,0x8E);
	LY::IDT::SetGate(37,(uint32_t)LY::IRQ::IRQ6,0x08,0x8E);
	LY::IDT::SetGate(38,(uint32_t)LY::IRQ::IRQ7,0x08,0x8E);
	LY::IDT::SetGate(39,(uint32_t)LY::IRQ::IRQ8,0x08,0x8E);
	LY::IDT::SetGate(40,(uint32_t)LY::IRQ::IRQ9,0x08,0x8E);
	LY::IDT::SetGate(41,(uint32_t)LY::IRQ::IRQ10,0x08,0x8E);
	LY::IDT::SetGate(42,(uint32_t)LY::IRQ::IRQ11,0x08,0x8E);
	LY::IDT::SetGate(43,(uint32_t)LY::IRQ::IRQ12,0x08,0x8E);
	LY::IDT::SetGate(44,(uint32_t)LY::IRQ::IRQ13,0x08,0x8E);
	LY::IDT::SetGate(45,(uint32_t)LY::IRQ::IRQ14,0x08,0x8E);
	LY::IDT::SetGate(46,(uint32_t)LY::IRQ::IRQ15,0x08,0x8E);
	LY::IDT::SetGate(47,(uint32_t)LY::IRQ::IRQ16,0x08,0x8E);

	LY::Screen::PutString("done");

//	ND::IDT::Flush();

//	asm volatile("sti");
}
void LY::IRQ::Remap(int pic1, int pic2)
{

#define PIC1 0x20
#define PIC2 0xA0

#define ICW1 0x11
#define ICW4 0x01

	LY::Ports::OutputB(PIC1, ICW1); /* write ICW1 to PICM, we are gonna write commands to PICM */
	LY::Ports::OutputB(PIC2, ICW1); /* write ICW1 to PICS, we are gonna write commands to PICS */

	LY::Ports::OutputB(PIC1 + 1, pic1); /* remap PICM to 0x20 (32 decimal) */
	LY::Ports::OutputB(PIC2 + 1, pic2); /* remap PICS to 0x28 (40 decimal) */

	LY::Ports::OutputB(PIC1 + 1, 0x04); /* IRQ2 -> connection to slave */
	LY::Ports::OutputB(PIC2 + 1, 0x02);

	LY::Ports::OutputB(PIC1 + 1, ICW4); /* write ICW4 to PICM, we are gonna write commands to PICM */
	LY::Ports::OutputB(PIC2 + 1, ICW4); /* write ICW4 to PICS, we are gonna write commands to PICS */

	LY::Ports::OutputB(PIC1 + 1, 0x01); /* enable all IRQs on PICM */
	LY::Ports::OutputB(PIC2 + 1, 0x01); /* enable all IRQs on PICS */

	/* disable all IRQs */
//	LY::Ports::OutputB(PIC1 + 1, 0xFF); /* disable all IRQs on PICM */
//	LY::Ports::OutputB(PIC2 + 1, 0xFF); /* disable all IRQs on PICS */
	LY::Ports::OutputB(PIC1 + 1, 0x00);
	LY::Ports::OutputB(PIC2 + 1, 0x00);
}

extern "C"
void LY_IRQ_Handler(struct regs *r)
{
	void (*handler)(struct regs *r);

	handler = (void (*)(struct regs *r))irq[r->int_no - 0x20];
	if(handler != 0)
	{
		handler(r);
	}
	if(r->int_no >= 0x28)
	{
		LY::Ports::OutputB(0xA0,0x20);
	}
	LY::Ports::OutputB(0x20,0x20);
}

extern "C"{
void LY_IRQ_Common()
{
	asm volatile(
	"popl %ebp	   \n"	
	"pushal	   \n"
	"pushl	%ds\n"//ds
	"pushl	%es\n"//es
	"pushl	%fs\n"//fs
	"pushl	%gs\n"//gs
	"movw $0x10, %ax \n"
	"movw %ax, %ds \n"
	"movw %ax, %es \n"
	"movw %ax, %fs \n"
	"movw %ax, %gs \n"
	"pushl %esp \n"
	"movl $LY_IRQ_Handler, %eax \n"
	"call *%eax \n"
	"popl %esp \n"
	"popl %gs \n"
	"popl %fs \n"
	"popl %es \n"
	"popl %ds \n"
	"popal    \n"
	"add $0x8, %esp \n"
	"sti	\n"
	"iret \n"
	);
}

void LY::IRQ::IRQ1()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli	\n"
	"pushl	$0x00\n"
	"pushl	$0x20\n"
	"jmp LY_IRQ_Common"
	);
}

void LY::IRQ::IRQ2()
{
	asm volatile(
	"popl %ebp	\n"	
	"cli	\n"
	"pushl	$0x00\n"
	"pushl	$0x21\n"
	"jmp LY_IRQ_Common"
	);
}
void LY::IRQ::IRQ3()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli	\n"
	"pushl	$0x00\n"
	"pushl	$0x22\n"
	"jmp LY_IRQ_Common"
	);
}
void LY::IRQ::IRQ4()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli	\n"
	"pushl	$0x00\n"
	"pushl	$0x23\n"
	"jmp LY_IRQ_Common"
	);
}
void LY::IRQ::IRQ5()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli	\n"
	"pushl	$0x00\n"
	"pushl	$0x24\n"
	"jmp LY_IRQ_Common"
	);
}
void LY::IRQ::IRQ6()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli	\n"
	"pushl	$0x00\n"
	"pushl	$0x25\n"
	"jmp LY_IRQ_Common"
	);
}
void LY::IRQ::IRQ7()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli	\n"
	"pushl	$0x00\n"
	"pushl	$0x26\n"
	"jmp LY_IRQ_Common"
	);
}
void LY::IRQ::IRQ8()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli	\n"
	"pushl	$0x00\n"
	"pushl	$0x27\n"
	"jmp LY_IRQ_Common"
	);
}
void LY::IRQ::IRQ9()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli	\n"
	"pushl	$0x00\n"
	"pushl	$0x28\n"
	"jmp LY_IRQ_Common"
	);
}
void LY::IRQ::IRQ10()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli	\n"
	"pushl	$0x00\n"
	"pushl	$0x29\n"
	"jmp LY_IRQ_Common"
	);
}
void LY::IRQ::IRQ11()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli	\n"
	"pushl	$0x00\n"
	"pushl	$0x2A\n"
	"jmp LY_IRQ_Common"
	);
}
void LY::IRQ::IRQ12()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli	\n"
	"pushl	$0x00\n"
	"pushl	$0x2B\n"
	"jmp LY_IRQ_Common"
	);
}
void LY::IRQ::IRQ13()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli	\n"
	"pushl	$0x00\n"
	"pushl	$0x2C\n"
	"jmp LY_IRQ_Common"
	);
}
void LY::IRQ::IRQ14()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli	\n"
	"pushl	$0x00\n"
	"pushl	$0x2D\n"
	"jmp LY_IRQ_Common"
	);
}
void LY::IRQ::IRQ15()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli	\n"
	"pushl	$0x00\n"
	"pushl	$0x2E\n"
	"jmp LY_IRQ_Common"
	);
}
void LY::IRQ::IRQ16()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli	\n"
	"pushl	$0x00\n"
	"pushl	$0x2F\n"
	"jmp LY_IRQ_Common"
	);
}
}
