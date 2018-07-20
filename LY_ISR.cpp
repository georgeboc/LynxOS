#include "LY_ISR.hpp"
#include "LY_Screen.hpp"
#include "LY_Panic.hpp"

const char* exception_messages[]=
{
	"Division by zero",
	"Debug exception",
	"Non maskable interrupt exception",
	"Breakpoint exception",
	"Into detected overflow exception",
	"Out of bounds exception",
	"Invalid opcode",
	"No coprocessor exception",
	"Double fault exception",
	"Coprocessor segment overrdide exception",
	"Bad TSS exception",
	"Segment not present exception",
	"Stack fault exception",
	"General protection fault exception",
	"Page fault exception",
	"Unknown interrupt exception",
	"Coprocessor fault exception",
	"Alignment check exception",
	"Machine check exception",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

extern "C"
void LY_ISR_Handler(struct regs *r)
{
	if(r->int_no < 32)
	{
		LY::Panic::Show(exception_messages[r->int_no]);
	}
	LY::Screen::SetColor(LY_SIDE_FOREGROUND,LY_COLOR_GREEN);
	LY::Screen::PutString("\n GS = ");
	LY::Screen::itoa(r->gs);
	LY::Screen::PutString("   FS = ");
	LY::Screen::itoa(r->fs);
	LY::Screen::PutString("\n ES = ");
	LY::Screen::itoa(r->es);
	LY::Screen::PutString("\n DS = ");
	LY::Screen::itoa(r->ds);
	LY::Screen::PutString("\nEDI = ");
	LY::Screen::itoa(r->edi);
	LY::Screen::PutString("   ESI = ");
	LY::Screen::itoa(r->esi);
	LY::Screen::PutString("\nEBP = ");
	LY::Screen::itoa(r->ebp);
	LY::Screen::PutString("   ESP = ");
	LY::Screen::itoa(r->esp);
	LY::Screen::PutString("\nEBX = ");
	LY::Screen::itoa(r->ebx);
	LY::Screen::PutString("   EDX = ");
	LY::Screen::itoa(r->edx);
	LY::Screen::PutString("\nECX = ");
	LY::Screen::itoa(r->ecx);
	LY::Screen::PutString("   EAX = ");
	LY::Screen::itoa(r->eax);
	LY::Screen::PutString("\nINT_NO = ");
	LY::Screen::itoa(r->int_no);
	LY::Screen::PutString("   ERR_CODE = ");
	LY::Screen::itoa(r->err_code);
	LY::Screen::PutString("\nEIP = ");
	LY::Screen::itoa(r->eip);
	LY::Screen::PutString("    CS = ");
	LY::Screen::itoa(r->cs);
	LY::Screen::PutString("\nEFLAGS = ");
	LY::Screen::itoa(r->eflags);
	LY::Screen::PutString("   USERESP = ");
	LY::Screen::itoa(r->useresp);
	LY::Screen::PutString("\n SS = ");
	LY::Screen::itoa(r->ss);
	for(;;);
}

extern "C"{
void LY_ISR_Common()
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
	"movl $LY_ISR_Handler, %eax \n"
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

void LY::ISR::ISR1()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x00\n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR2()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x01 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR3()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x02 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR4()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x03 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR5()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x04 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR6()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x05 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR7()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x06 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR8()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x07 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR9()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x08 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR10()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x09 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR11()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x0A \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR12()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x0B \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR13()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x0C \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR14()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x0D \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR15()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x0E \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR16()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x0F \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR17()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x10 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR18()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x11 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR19()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x12 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR20()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x13 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR21()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x14 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR22()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x15 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR23()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x16 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR24()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x17 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR25()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x18 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR26()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x19 \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR27()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x1A \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR28()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x1B \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR29()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x1C \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR30()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x1D \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR31()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x1E \n"
	"jmp LY_ISR_Common \n"
	);
}
void LY::ISR::ISR32()
{
	asm volatile(
	"popl %ebp	   \n"	
	"cli \n"
	"pushl $0x00 \n"
	"pushl $0x1F \n"
	"jmp LY_ISR_Common \n"
	);
}
}
