#include "LY_ACPI.hpp"

unsigned int *LY::ACPI::CheckRSDPtr(unsigned int *ptr)
{
	char *sig = (char*)"RSD PTR ";
	struct RSDPtr *rsdp = (struct RSDPtr *) ptr;
	byte *bptr;
	byte check = 0;
	int i;
	
	if(LY::Memory::Compare(sig, rsdp, 8) == 0)
	{
		bptr = (byte *) ptr;
		for(i=0; i<sizeof(struct RSDPtr); i++)
		{
			check += *bptr;
			bptr++;
		}

		if(check == 0)
		{
			/*if(desc->Revision == 0)
				LY::Screen::PutString("\n ACPI 1");
			else
				LY::Screen::PutString("\n ACPI 2");*/

			return (unsigned int *) rsdp->RsdtAddress;
		}
	}
		
	return NULL;
}

unsigned int *LY::ACPI::GetRSDPtr(void)
{
	unsigned int *addr;
	unsigned int *rsdp;

	for(addr = (unsigned int *) 0x000E0000; (int) addr<0x00100000; addr += (0x10/sizeof(addr)))
	{
		rsdp = LY::ACPI::CheckRSDPtr(addr);
		if(rsdp != NULL)
			return rsdp;	
	}
	
	int ebda = *((short *) 0x40E);
	ebda = ebda * 0x10 & 0x000FFFFF;
	
	for(addr = (unsigned int *) ebda; (int) addr<ebda+1024; addr +=(0x10/sizeof(addr)))
	{
		rsdp = LY::ACPI::CheckRSDPtr(addr);
		if(rsdp != NULL)
			return rsdp;
	}

	return NULL;
}

int LY::ACPI::CheckHeader(unsigned int *ptr, char *sig)
{
	if(LY::Memory::Compare(ptr, sig, 4)==0)
	{
		char *checkPtr = (char *) ptr;
		int len = *(ptr +1);
		char check = 0;
		while(0<len--)
		{
			check += *checkPtr;
			checkPtr++;
		}
		if(check == 0)
			return 0;
	}
	return -1;
}

int LY::ACPI::Enable(void)
{
	if((LY::Ports::InputW((unsigned int) PM1a_CNT) & SCI_EN) == 0)
	{
		if(SMI_CMD != 0 && ACPI_ENABLE != 0)
		{
			LY::Ports::OutputB((unsigned int) SMI_CMD, ACPI_ENABLE);
			int i;
			for(i=0;i<300;i++)
			{
				if((LY::Ports::InputW((unsigned int) PM1a_CNT) & SCI_EN) == 1)
					break;
				LY::Timer::Wait(1);	
				//LY::Screen::itoa(LY::Ports::InputW((unsigned int) PM1a_CNT));			
			}
			if(PM1b_CNT != 0)
			{
				for(; i<300; i++)
				{
					if((LY::Ports::InputW((unsigned int) PM1b_CNT) & SCI_EN) == 1)
						break;
					LY::Timer::Wait(1);	
				}
			}
			if(i < 300)
			{
				LY::Screen::PutString("\nACPI enabled");
				return 0;
			}
			else
			{
				LY::Screen::PutString("\nCouldn't enable ACPI");
				return -1;				
			}
		}
		else
		{
			LY::Screen::PutString("\nNo known way to enable ACPI");
			return -1;
		}
	}
	else
	{
		LY::Screen::PutString("\nACPI was already enabled");
		return 0;
	}
}

int LY::ACPI::Init(void)
{
	LY::Screen::SetColor(LY_SIDE_FOREGROUND, LY_COLOR_WHITE);
	LY::Screen::PutString("\nSetup ACPI...");
	unsigned int *ptr = LY::ACPI::GetRSDPtr();
	
	if(ptr != NULL && LY::ACPI::CheckHeader(ptr, (char *)"RSDT") == 0)
	{
		int entrys = *(ptr + 1);
		entrys = (entrys - 36) / 4;
		ptr += 36 / 4;

		while(0 < entrys--)
		{
			if(LY::ACPI::CheckHeader((unsigned int *) *ptr, (char *)"FACP") == 0)
			{
				entrys = -2;
				struct FACP *facp = (struct FACP *) *ptr;
				if(LY::ACPI::CheckHeader((unsigned int *) facp->DSDT, (char *)"DSDT") == 0)
				{
					char *S5Addr = (char *) facp->DSDT + 36;
					int dsdtLength = *(facp->DSDT + 1) - 36;
					while(0 < dsdtLength--)
					{
						if(LY::Memory::Compare(S5Addr, (char *)"_S5_", 4) == 0)
							break;
						S5Addr++;
					}
					if(dsdtLength > 0)
					{
						if((*(S5Addr - 1) == 0x08 || (*(S5Addr - 2) == 0x08 && *(S5Addr - 1) == '\\')) && *(S5Addr + 4) == 0x12)
						{
							S5Addr += 5;
							S5Addr += ((*S5Addr &0xC0) >> 6) + 2;
							
							if(*S5Addr == 0x0A)
								S5Addr++;
							SLP_TYPa = *(S5Addr) << 10;

							S5Addr++;
							if(*S5Addr == 0x0A)
								S5Addr++;
							SLP_TYPb = *(S5Addr) << 10;

							SMI_CMD = facp->SMI_CMD;
							
							ACPI_ENABLE = facp->ACPI_ENABLE;							
							ACPI_DISABLE = facp->ACPI_DISABLE;

							PM1a_CNT = facp->PM1a_CNT_BLK;
							PM1b_CNT = facp->PM1b_CNT_BLK;

							PM1_CNT_LEN = facp->PM1_CNT_LEN;
			
							SLP_EN = 1 << 13;		
							SCI_EN = 1;

							LY::Screen::SetColor(LY_SIDE_FOREGROUND, LY_COLOR_WHITE);
							LY::Screen::PutString("done\n");

							return 0;
						}
						else
						{
							LY::Screen::PutString("error");
							LY::Screen::PutString("\n\\_S5 parse error.");
						}
	
					}
					else
					{
						LY::Screen::PutString("error");
						LY::Screen::PutString("\n\\_S5 not present.");
					}
				}
				else
				{
					LY::Screen::PutString("error");
					LY::Screen::PutString("\nDSDT invalid.");
				}				

			}
			ptr++;
		}
		LY::Screen::PutString("error");
		LY::Screen::PutString("\nNo valid FACP present.");
	}
	else
	{
		LY::Screen::PutString("error");
		LY::Screen::PutString("\nNo ACPI");
	}	

	return -1;
}

void LY::ACPI::PowerOff(void)
{
	if(SCI_EN == 0)
		return;
	
	LY::ACPI::Enable();
	LY:Ports::OutputW((unsigned int) PM1a_CNT, SLP_TYPa | SLP_EN);
	if(PM1b_CNT != 0)
	{
		LY::Ports::OutputW((unsigned int) PM1b_CNT, SLP_TYPb | SLP_EN);		
	}
	LY::Screen::PutString("\nACPI poweroff failed.");
}
