#include "LY_Ports.hpp"
#include "LY_Types.hpp"

uint8_t LY::Ports::InputB(uint16_t _port)
{
	unsigned char rv;
	asm volatile("inb %1, %0" : "=a"(rv) : "dN"(_port));
	return rv;
}
uint16_t LY::Ports::InputW(uint16_t port)
{
	uint16_t rv;
	asm volatile("inw %1, %0" : "=a"(rv) : "dN"(port));
	return rv;
}
uint32_t LY::Ports::InputL(uint16_t port)
{
	uint32_t rv;
	asm volatile("inl %1, %0" : "=a"(rv) : "dN"(port));
	return rv;
}
void LY::Ports::InputSL(uint16_t port,unsigned int buffer, int count)
{
	/*asm volatile (
	"cld;"
	"repne; insl;"
	: "=D" (addr), "=c" (cnt)
	: "d" (port), "0" (addr), "1" (cnt)
	: "memory", "cc");*/
	
	asm volatile("cld; rep; insl;"::"D"(buffer), "d"(port), "c"(count));
}
void LY::Ports::InputSW(uint16_t port,unsigned int buffer, int count)
{
	asm volatile("cld; rep; insw;"::"D"(buffer), "d"(port), "c"(count));
}
/**
 * @brief Sets the output for the 8bit port in the selected port
 * @param port The port to set the value
 * @param value The value to set
 * */
unsigned int LY::Ports::OutputSL(uint16_t port, int count)
{
	unsigned int buffer;
	asm volatile("cld; rep; outsl;":"=D"(buffer):"d"(port), "c"(count));
	return buffer;
}

unsigned int LY::Ports::OutputSW(uint16_t port, int count)
{
	unsigned int buffer;
	asm volatile("cld; rep; outsw;":"=D"(buffer):"d"(port), "c"(count));
	return buffer;
}

void LY::Ports::OutputB(uint16_t port, uint8_t value)
{
	asm volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

void LY::Ports::OutputW(uint16_t port, uint16_t value)
{
	asm volatile("outw %1, %0" : : "dN"(port), "a"(value));
}

void LY::Ports::OutputL(uint16_t port, uint32_t value)
{
	asm volatile("outl %1, %0" : : "dN"(port), "a"(value));
}
