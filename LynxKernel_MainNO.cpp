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
	LY::Screen::PutString("\n");
	LY::Memory::MM_init((uint32_t)&end);
	LY::PCI::Init();	
	asm volatile("sti");

	LY::Screen::PutString("Found: \n");	
	for(int i = 0; i <= devs; i++)
	{
		if(pci_devices[i]->clase==0x01)
		{
			LY::Screen::PutString("Subclass: ");		
			LY::Screen::itoa(pci_devices[i]->subclase);
			LY::Screen::PutString(" ProgIF: ");		
			LY::Screen::itoa(pci_devices[i]->progIF);
			LY::Screen::PutString(" Bus: ");		
			LY::Screen::itoa(pci_devices[i]->bus);
			LY::Screen::PutString(" Slot: ");		
			LY::Screen::itoa(pci_devices[i]->slot);
			LY::Screen::PutString(" Function: ");		
			LY::Screen::itoa(pci_devices[i]->func);
			LY::Screen::PutString("\n BAR0: ");		
			LY::Screen::itoa(pci_devices[i]->BAR0);
			LY::Screen::PutString(" BAR1: ");		
			LY::Screen::itoa(pci_devices[i]->BAR1);
			LY::Screen::PutString(" BAR2: ");		
			LY::Screen::itoa(pci_devices[i]->BAR2);
			LY::Screen::PutString(" BAR3: ");		
			LY::Screen::itoa(pci_devices[i]->BAR3);
			LY::Screen::PutString(" BAR4 ");		
			LY::Screen::itoa(pci_devices[i]->BAR4);
			LY::Screen::PutString(" BAR5: ");		
			LY::Screen::itoa(pci_devices[i]->BAR5);
			unsigned int bar_chk = pci_devices[i]->BAR5 & 0x01;
			LY::Screen::PutString("\n Last bit: ");
			LY::Screen::itoa(bar_chk);			
			LY::Screen::PutString("\n");		
		}

		/*if(pci_devices[i]->clase==0x01 && pci_devices[i]->subclase==6 && pci_devices[i]->progIF==0)
		{*/
			//WORD buffer_i[511];
			/*LY::SATA::Probe_Port((HBA_MEM *)pci_devices[i]->BAR5);
			LY::SATA::Init_HBA((HBA_MEM *)pci_devices[i]->BAR5);
			DWORD buf[511];
			HBA_PORT *port=(HBA_PORT*)0;
			if(LY::SATA::Read_Port(port, 0, 0, 2, buf))
			{
				LY::Screen::PutString("Read succ.");
				for(int i = 0; i <= 511; i++)
				{
					LY::Screen::itoa(buf[i]);
					LY::Screen::PutString(" ");
				}
			}
			else
			{
				LY::Screen::PutString("Read failed");
			}*/
			//LY::SATA::Read_Port(0,0,0,1,buffer_i);					
		//}
	}
		LY::Screen::PutString("\nStarting ACPI...");
		LY::ACPI::Init();
		LY::Screen::PutString("\nPowering off the computer");
		LY::Timer::Wait(18);
		LY::Screen::PutString(".");
		LY::Timer::Wait(18);
		LY::Screen::PutString(".");
		LY::Timer::Wait(18);
		LY::Screen::PutString(".");
		LY::Timer::Wait(18);
		LY::ACPI::PowerOff();
			//LY::SATA::Start_CMD(abar->ports);
			/*uint32_t command = ATA_CMD_IDENTIFY;
			HBA_PORT *puerto_trs;			
			int slot = LY::SATA::Find_CMDSlot(puerto_trs);

			HBA_CMD_HEADER *commandhd = (HBA_CMD_HEADER*)puerto_trs->clb;
			commandhd += slot;
			commandhd->cfl = sizeof(FIS_REG_H2D) / sizeof(DWORD);
			commandhd->w=1;
			commandhd->prdtl = (WORD)(*/

			/*FIS_REG_H2D* fis;
			LY::Memory::Set(&fis, 0, sizeof(FIS_REG_H2D));
			fis->fis_type = FIS_TYPE_REG_H2D;
			fis->command = ATA_CMD_IDENTIFY;
			fis->device = 0;
			fis->c = 1;


			puerto_trs->cmd = command;
			puerto_trs->clb = (uint32_t)(&command);
			puerto_trs->fb = (uint32_t)(fis);
			puerto_trs->ie = 1;
			puerto_trs->ci = 1;
			LY::Timer::Wait(18);
			LY::Screen::PutString("\nAHCI: READED: ");
			LY::Screen::itoa(puerto_trs->is);
			LY::Screen::PutString("\nAHCI: READED: ");
			LY::Screen::itoa(puerto_trs->ssts);
			LY::Screen::PutString("\nAHCI: READED: ");
			LY::Screen::itoa(puerto_trs->sctl);
			LY::Screen::PutString("\nAHCI: READED: ");
			LY::Screen::itoa(puerto_trs->serr);
			LY::Screen::PutString("\nAHCI: READED: ");
			LY::Screen::itoa(puerto_trs->sact);

			FIS_PIO_SETUP* port;
			LY::Memory::Set(&port, 0, sizeof(FIS_PIO_SETUP));
			port->fis_type = FIS_TYPE_PIO_SETUP;
			port->device = 0;
	

				LY::Screen::PutString("\nAHCI: READED: ");
				LY::Screen::itoa(port->tc);	
				LY::Screen::PutString("\n");
				LY::Screen::itoa(port->status);	
				LY::Screen::PutString("\n");
				LY::Screen::itoa(port->error);	
				LY::Screen::PutString("\n");
				LY::Screen::itoa(port->i);	
				LY::Screen::PutString("\n");
				LY::Screen::itoa(port->d);	
				LY::Screen::PutString("\n");

			LY::Screen::PutString("\nAHCI: READED: ");
			LY::Screen::itoa(puerto_trs->is);
			LY::Screen::PutString("\nAHCI: READED: ");
			LY::Screen::itoa(puerto_trs->ssts);
			LY::Screen::PutString("\nAHCI: READED: ");
			LY::Screen::itoa(puerto_trs->sctl);
			LY::Screen::PutString("\nAHCI: READED: ");
			LY::Screen::itoa(puerto_trs->serr);
			LY::Screen::PutString("\nAHCI: READED: ");
			LY::Screen::itoa(puerto_trs->sact);*/

			/*FIS_DATA* datos;
			LY::Memory::Set(&datos, 0, sizeof(FIS_DATA));
			datos->fis_type = FIS_TYPE_DATA;*/
			/*for(uint64_t l = 0; l<= port->tc; l++)
			{
				LY::Screen::itoa(datos->data[0]);
				LY::Screen::PutString("\n");
				LY::Screen::itoa(datos->data[1]);
				LY::Screen::PutString("\n");
				LY::Screen::itoa(l);
				LY::Screen::PutString("\n");
			}*/
			/*WORD buffer_i[511];			
			WORD buffer_o[511];

			for(int l = 0; l < 512; l++)
			{
				buffer_o[l]=l;			
			}			
			
			if(LY::SATA::Read_Port((HBA_PORT *)(0), 0,0,1,buffer_i))
			{
				LY::Screen::PutString("\nAHCI: Read successful444\n");
				for(int k = 0; k < 512; k++)
				{
					LY::Screen::itoa(buffer_i[k]);
					LY::Screen::PutString(" ");
				}
			}
			else
			{
				LY::Screen::PutString("\nAHCI: Read error\n");
			}*/


			/*if(LY::SATA::Write_Port((HBA_PORT *)0, 0,0,1,buffer_o))
			{
				LY::Screen::PutString("\nAHCI: Write successful\n");
			}
			else
			{
				LY::Screen::PutString("\nAHCI: Write error\n");
			}*/



/*
			LY::SATA::Start_CMD((HBA_PORT *)0);

			FIS_REG_H2D* fis;
			LY::Memory::Set((void *)&fis, 0, sizeof(FIS_REG_H2D));
			fis->fis_type = FIS_TYPE_REG_H2D;
			fis->command = ATA_CMD_IDENTIFY;
			fis->device = 0;
			fis->c = 1;
			HBA_PORT* fis_port;
			fis_port->clb = fis;
			

			FIS_PIO_SETUP* fis;
			LY::Memory::Set(&fis, 0, sizeof(FIS_PIO_SETUP));
			fis->fis_type = FIS_TYPE_PIO_SETUP;
			fis->device = 0;		
			LY::Screen::PutString("\nAHCI: FIS readed: ");
			LY::Screen::itoa(fis->tc);
			LY::Screen::PutString("\nAHCI: I readed: ");
			LY::Screen::itoa(fis->i);*/

			/*FIS_DATA* fisa;
			LY::Memory::Set(&fisa, 0, sizeof(FIS_DATA));
			fisa->fis_type = FIS_TYPE_PIO_SETUP;	
			LY::Screen::PutString("\nAHCI: D0 readed: ");
			LY::Screen::itoa(fisa->data[0]);
			LY::Screen::PutString("\nAHCI: D1 readed: ");
			LY::Screen::itoa(fisa->data[1]);	*/	
			/*FIS_REG_D2H fis;
			LY::Memory::Set(&fis, 0, sizeof(FIS_REG_D2H));
			fis.fis_type = FIS_TYPE_REG_D2H;
			fis.device = 0;
			if(fis.i == 1)
			{
				LY::Screen::PutString("\nAHCI: FIS int\n");
			}*/


	
	//LY::HDD::IDE_INITIALIZE(0x1F0,0x3F6,0x170,0x376,0x24);	
	//LY::Timer::Wait(18);
	//

	//LY::Memory::MM_print_out();	
	

	//asm volatile("cli");	
	//LY::HDD::IDE_ATA_ACCESS(1, 0, 0x000, 1, 1, 0);





	/*LY::HDD::IDE_ATA_ACCESS(0, 0, 0x000, 1, 1, 0);
	if(buffer_salida_ready==1)
	{
		LY::Screen::PutString("\nBuffer de salida listo\n");
		for(int i = 0; i < 256; i++)
		{
			LY::Screen::itoa(buffer_salida[i]);
			LY::Screen::PutString(" ");
		}
	}*/





	//asm volatile("sti");
	//LY::Screen::itoa(LY::HDD::IDE_ATA_ACCESS(1, 0, 0x10000001, 1, 1, 0));
	//LY::Screen::itoa(LY::HDD::IDE_ATA_ACCESS(0, 0, 0x001, 1, 1, 0));
	/*LY::Screen::PutString("\nReading a bit from HDD...");
	LY::HDD::IDE_READ_SECTORS(0,1,0x10000000,2,0x0F);
	LY::HDD::IDE_READ(0, ATA_CMD_READ_PIO);*/
/*
	LY::HDD::IDE_WRITE(ATA_PRIMARY, 0x00, 0x0A);
	LY::Screen::PutString("done\n");
	LY::Screen::PutString("Reading a bit fron the HDD...");
	LY::Screen::itoa(LY::HDD::IDE_READ(ATA_PRIMARY, 0x00));*/
	
	//LY::Screen::PutString("\nCreating a 5s timer...");
	//LY::Timer::Create(0,5);

	//LY::Screen::PutString("\nReading RAM...(0x100000)\n");
	
	/*for (int addr=0;addr<=10;addr++)
	{*/
		
		/*LY::Screen::itoa(addr);
		LY::Screen::PutString(" : ");
		LY::Screen::itoa(LY::RAM::Read(addr));
		LY::Screen::PutString("\n");*/
		//LY::Screen::PutString("Hola mundoo \n");
		
	//}


return 0;
}
