#include "LY_Screen.cpp"
#include "LY_Ports.cpp"
#include "LY_GDT.cpp"
#include "LY_IDT.cpp"
#include "LY_ISR.cpp"
#include "LY_IRQ.cpp"
#include "LY_Memory.cpp"
#include "LY_Panic.cpp"
#include "LY_Timer.cpp"
#include "LY_RAM.cpp"
#include <stdlib.h>
#include "LY_HDD.cpp"
#include "LY_SATA.cpp"
#include "LY_ACPI.cpp"
#include "LY_PCI.cpp"
#include "LY_Color.hpp"
//#include "LY_Multiboot.hpp"
//#include <malloc.h>

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

int LynxKernel_Main()
{
	int adr=0xC0000000;
	extern uint32_t end;
	unsigned long long bar5_ahci=0;

	asm volatile("cli");
	LY::Screen::Clear(LY_COLOR_BLUE);
	LY::Screen::SetColor(LY_SIDE_BACKGROUND,LY_COLOR_BLUE);
	LY::Screen::SetColor(LY_SIDE_FOREGROUND,LY_COLOR_WHITE);
	LY::Screen::PutString("Lynx Operative System\n");
	LY::Screen::PutString("Licensed under GNU GPL v2\n");
	LY::GDT::Install();
	LY::IDT::Install();
	LY::IRQ::Install();
	LY::Timer::Setup();
	LY::RAM::Information();
	LY::Screen::PutString("Kernel end: ");	
	LY::Screen::itoa((unsigned long long)&end);
	LY::Memory::MM_init((uint32_t)&end);
	LY::PCI::Init();	
	asm volatile("sti");
	LY::ACPI::Init();
	unsigned int HDD_found_pointer = 0;
	char* HDD_found[255];
	for(int i = 0; i <= devs; i++)
	{
		if(pci_devices[i]->clase==0x01 && pci_devices[i]->subclase==0x01)
		{
			HDD_found[HDD_found_pointer] = (char*)"IDE Controller";
			HDD_found_pointer++;	
		}
		else if(pci_devices[i]->clase==0x01 && pci_devices[i]->subclase==0x06)
		{
			HDD_found[HDD_found_pointer] = (char*)"AHCI Controller";
			LY::Screen::itoa(pci_devices[i]->BAR5);
			bar5_ahci=pci_devices[i]->BAR5;
			LY::Screen::PutString("\n");
			HDD_found_pointer++;
		}
	}
	if(HDD_found_pointer == 0)
	{
		LY::Screen::PutString("Were not found Harddisk controllers\n");
	}
	else
	{
		if(HDD_found_pointer == 1)
		{
			LY::Screen::PutString("Was found 1 Harddisk controller:\n");
		}
		else
		{
			LY::Screen::PutString("Were found ");
			LY::Screen::itoa(HDD_found_pointer);	
			LY::Screen::PutString(" Harddisk controllers:\n");		
		}
		for(int l = 0; l < HDD_found_pointer; l++)
		{
			LY::Screen::PutString(" ");
			LY::Screen::PutString((const char*)HDD_found[l]);
			LY::Screen::PutString("\n");
			if(HDD_found[l] == (char*)"IDE Controller")
			{
				LY::HDD::IDE_INITIALIZE(0x1F0,0x3F6,0x170,0x376,0x00);
				//LY::HDD::IDE_ATA_ACCESS(0, 0, 0x000, 1);
				/*LY::Screen::PutString("\n");
				for(int sector = 0; sector <= sectores_ultimo_ODD; sector++)
				{

					LY::Screen::itoa(LY::HDD::IDE_ATAPI_READ(0,sector,1));
					if(buffer_salida_ATAPI_ready==1)
					{
						for(int i = 0; i < 256; i++)
						{
							LY::Screen::itoa(buffer_salida_ATAPI[i]);
							LY::Screen::PutString(" ");
						}
					}

				}*/
			}
			else if(HDD_found[l] == (char*)"AHCI Controller")
			{
					HBA_MEM* memoria = (HBA_MEM *) (bar5_ahci);
					HBA_PORT* port0 = (HBA_PORT *) (bar5_ahci + 0x100 + 0x80 * 0);

						uint64_t PxSIG = port0->sig;

				LY::SATA::Probe_Port((HBA_MEM *)bar5_ahci);			

					LY::Screen::PutString("\nBAR5: ");
					LY::Screen::itoa(bar5_ahci);
					LY::Screen::PutString("\nCAP: ");
					LY::Screen::itoa(memoria->cap);
					LY::Screen::PutString("\nGHC: ");
					LY::Screen::itoa(memoria->ghc);
					LY::Screen::PutString("\nPI: ");
					LY::Screen::itoa(memoria->pi);
					LY::Screen::PutString("\nCMD: ");
					LY::Screen::itoa(port0->cmd);
					port0->cmd = port0->cmd | 0x10;	
	
					LY::Screen::PutString("\nCMD: ");
					LY::Screen::itoa(port0->cmd);
					LY::Screen::PutString("\nSSTS: ");
					LY::Screen::itoa(port0->ssts);
					LY::Screen::PutString("\nISIG: ");
					LY::Screen::itoa(PxSIG);
					LY::Screen::PutString("\nSIG: ");
					LY::Screen::itoa(port0->sig);
					LY::Screen::PutString("\nSERR: ");
					LY::Screen::itoa(port0->serr);
					
//port0->cmd = port0->cmd | 0x10;
				DWORD buffer_w[512];

				for(int i = 0, m = 0; m < 128; m++)
				{
					buffer_w[m] = (i++ & 0xFFFF) | (((i++) << 16) & 0xFFFF0000);
				}
				LY::SATA::Write_Port((HBA_PORT *)(bar5_ahci + 0x100 + (0x80 * 2)),5,0,1,buffer_w);

				DWORD buf[512];
				LY::SATA::Read_ATAPI_Port((HBA_PORT *)(bar5_ahci + 0x100 + (0x80 * 0)),0,0,1,buf);
				for(int i = 0; i < 128; i++)
				{
					LY::Screen::itoa(buf[i] & 0xFFFF);
					LY::Screen::PutString(" ");
					LY::Screen::itoa((buf[i] & 0xFFFF0000)>>16);
					LY::Screen::PutString(" ");
				}	
				
			}
		}
	}
	//LY::Screen::BlockDisable();
	/*LY::HDD::IDE_ATAPI_READ(3, 0x00, 1);
	if(buffer_salida_ATAPI_ready == 1)
	{
		LY::Screen::PutString("Copying to 1st HDD\n");
		for(int i = 0; i < 256; i++)
		{
			LY::Screen::itoa(buffer_salida_ATAPI[i]);
			LY::Screen::PutString(" ");;
		}
	}*/
return 0;
}
