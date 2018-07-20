//INCOMPLETE

#include "LY_SATA.hpp"
#define KERNEL_BASE 0x00100000
#define DA 0x1000000

char test[20]="6317065029";
HBA_MEM *abar;
extern char fs_buf[1024];

/*HBA_MEM *abar;
bool EjectOdd = false;
HBA_PORT *port;
uint8_t *tmpbuffer, classinter, cmd;	
HBA_CMD_HEADER *cmdlist;
HBA_CMD_TBL *cmdheader;
FIS_REG_H2D *fish2d;*/

void LY_AHCI_Handler(struct regs* r)
{
	LY::Screen::PutString("AHCI Interrupt Handler\n");
	if(abar->ports[0].is & HBA_PxIS_TFES)
	{
		LY::Screen::PutString("Read disk error\n");
	}
	LY::Screen::PutString("\nTFD=[");
	LY::Screen::itoa((unsigned long long)((HBA_PORT *)(&abar->ports[0]))->tfd);
	LY::Screen::PutString("]\nSSTS=[");
	LY::Screen::itoa((unsigned long long)((HBA_PORT *)(&abar->ports[0]))->ssts);
	LY::Screen::PutString("]\nIE=[");
	LY::Screen::itoa((unsigned long long)((HBA_PORT *)(&abar->ports[0]))->ie);
	LY::Screen::PutString("]\nSERR=[");
	LY::Screen::itoa((unsigned long long)((HBA_PORT *)(&abar->ports[0]))->serr);
	LY::Screen::PutString("]\nIS=[");
	LY::Screen::itoa((unsigned long long)((HBA_PORT *)(&abar->ports[0]))->is);
	abar->ports[0].is = 0xFFFF;
	//while(1);
}

bool LY::SATA::Read_ATAPI_Port(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count, DWORD *buf)
{
	port->is = (DWORD) - 1;
	int spin = 0;
	int slot = LY::SATA::Find_CMDSlot(port);
	LY::Screen::PutString("\nSlot: ");
	LY::Screen::itoa(slot);
	LY::Screen::PutString("\n");
	if(slot == -1)
	{
		return false;
	}

	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*) port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(DWORD);
	cmdheader->w = 0;
	cmdheader->prdtl = (WORD)((count - 1) >> 4) + 1;
	cmdheader->a = 1;

	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);

	LY::Memory::Set((void *)cmdtbl, 0, (sizeof(HBA_CMD_TBL) + (cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY))); //+ (cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY)

	int i;
	for(i = 0; i < (cmdheader->prdtl - 1); i++)
	{
		cmdtbl->prdt_entry[i].dba = (DWORD)buf;
		cmdtbl->prdt_entry[i].dbc = 8 * 1024;	
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4 * 1024;
		count -= 16;
	}
	cmdtbl->prdt_entry[i].dba = (DWORD)buf;
	cmdtbl->prdt_entry[i].dbc = count << 9;	
	cmdtbl->prdt_entry[i].i = 1;

	cmdtbl->acmd[0] = ATAPI_CMD_READ;
	cmdtbl->acmd[1] = 0x00;
	cmdtbl->acmd[2] = (startl >> 24) & 0xFF;
	cmdtbl->acmd[3] = (startl >> 16) & 0xFF;
	cmdtbl->acmd[4] = (startl >> 8) & 0xFF;
	cmdtbl->acmd[5] = (startl >> 0) & 0xFF;
	cmdtbl->acmd[6] = 0x00;
	cmdtbl->acmd[7] = 0x00;
	cmdtbl->acmd[8] = 0x00;
	cmdtbl->acmd[9] = count;
	cmdtbl->acmd[10] = 0x00;
	cmdtbl->acmd[11] = 0x00;

	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*) (&cmdtbl->cfis);

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;
	cmdfis->command = ATA_CMD_PACKET; //EX

	cmdfis->lba0 = (BYTE) startl;
	cmdfis->lba1 = (BYTE) (startl >> 8);
	cmdfis->lba2 = (BYTE) (startl >> 16);
	cmdfis->device = 1 << 6;

	cmdfis->lba3 = (BYTE) (startl >> 24);
	cmdfis->lba4 = (BYTE) starth;
	cmdfis->lba5 = (BYTE) (starth >> 8);
	
	cmdfis->countl = count;
	cmdfis->counth = 0;

	while((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}	
	if(spin == 1000000)
	{
		LY::Screen::PutString("\nPort is hung");
		return false;
	}
	//port->ie = 1;
	port->ci = 1 << slot;	
	
	//LY::Timer::Wait(18);

	while(1)
	{
			/*LY::Screen::PutString("Clb: ");
			LY::Screen::itoa(port->clb);		
			LY::Screen::PutString("\n");
			LY::Screen::PutString("Fb: ");
			LY::Screen::itoa(port->fb);		
			LY::Screen::PutString("\n");
			LY::Screen::PutString("Is: ");
			LY::Screen::itoa(port->is);		
			LY::Screen::PutString("\n");
			LY::Screen::PutString("Ie: ");
			LY::Screen::itoa(port->ie);		
			LY::Screen::PutString("\n");
			LY::Screen::PutString("Cmd: ");
			LY::Screen::itoa(port->cmd);		
			LY::Screen::PutString("\n");*/
		if((port->ci & (1 << slot)) == 0)
		{
			break;
		}		
		if(port->is & HBA_PxIS_TFES)
		{
			LY::Screen::PutString("Read disk error1\nPort is:");
			LY::Screen::itoa(port->is);
			LY::Screen::PutString("\nHBA_PxIS_TFES: ");
			LY::Screen::itoa(HBA_PxIS_TFES);
			LY::Screen::PutString("\nPxSAE: ");
			LY::Screen::itoa(port->serr);
			LY::Screen::PutString("\n");
			LY::Screen::itoa(port->ssts);
			LY::Screen::PutString("\n");
			return false;
		}
	}
	if((port->is & HBA_PxIS_TFES) == 1)
	{
		LY::Screen::PutString("Read disk error2\n");
		return false;
	}
	int k = 0;
	/*LY::Screen::PutString("\nSATA active: ");
	LY::Screen::itoa(port->sact);*/
	/*while(port->ci != 0 && port->is==1)
	{
		LY::Screen::PutString("\n[");
		LY::Screen::itoa(k);
		k++;
		LY::Screen::PutString("]");*/
		/*FIS_DATA* datos;
		LY::Memory::Set(&datos, 0, sizeof(FIS_DATA));
		datos->fis_type = FIS_TYPE_DATA;
		LY::Screen::itoa(datos->data[1]);
		LY::Screen::PutString(" ");
	}*/
	return true;
}

bool LY::SATA::Read_Port(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count, DWORD *buf)
{
	port->is = (DWORD) - 1;
	int spin = 0;
	int slot = LY::SATA::Find_CMDSlot(port);
	LY::Screen::PutString("\nSlot: ");
	LY::Screen::itoa(slot);
	LY::Screen::PutString("\n");
	if(slot == -1)
	{
		return false;
	}

	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*) port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(DWORD);
	cmdheader->w = 0;
	cmdheader->prdtl = (WORD)((count - 1) >> 4) + 1;

	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);

	LY::Memory::Set((void *)cmdtbl, 0, (sizeof(HBA_CMD_TBL) + (cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY))); //+ (cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY)

	int i;
	for(i = 0; i < (cmdheader->prdtl - 1); i++)
	{
		cmdtbl->prdt_entry[i].dba = (DWORD)buf;
		cmdtbl->prdt_entry[i].dbc = 8 * 1024;	
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4 * 1024;
		count -= 16;
	}
	cmdtbl->prdt_entry[i].dba = (DWORD)buf;
	cmdtbl->prdt_entry[i].dbc = count << 9;	
	cmdtbl->prdt_entry[i].i = 1;

	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*) (&cmdtbl->cfis);

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;
	cmdfis->command = ATA_CMD_READ_DMA_EX; //EX

	cmdfis->lba0 = (BYTE) startl;
	cmdfis->lba1 = (BYTE) (startl >> 8);
	cmdfis->lba2 = (BYTE) (startl >> 16);
	cmdfis->device = 1 << 6;

	cmdfis->lba3 = (BYTE) (startl >> 24);
	cmdfis->lba4 = (BYTE) starth;
	cmdfis->lba5 = (BYTE) (starth >> 8);
	
	cmdfis->countl = count;
	cmdfis->counth = 0;

	while((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}	
	if(spin == 1000000)
	{
		LY::Screen::PutString("\nPort is hung");
		return false;
	}
	//port->ie = 1;
	port->ci = 1 << slot;	
	
	//LY::Timer::Wait(18);

	while(1)
	{
			/*LY::Screen::PutString("Clb: ");
			LY::Screen::itoa(port->clb);		
			LY::Screen::PutString("\n");
			LY::Screen::PutString("Fb: ");
			LY::Screen::itoa(port->fb);		
			LY::Screen::PutString("\n");
			LY::Screen::PutString("Is: ");
			LY::Screen::itoa(port->is);		
			LY::Screen::PutString("\n");
			LY::Screen::PutString("Ie: ");
			LY::Screen::itoa(port->ie);		
			LY::Screen::PutString("\n");
			LY::Screen::PutString("Cmd: ");
			LY::Screen::itoa(port->cmd);		
			LY::Screen::PutString("\n");*/
		if((port->ci & (1 << slot)) == 0)
		{
			break;
		}		
		if(port->is & HBA_PxIS_TFES)
		{
			LY::Screen::PutString("Read disk error1\nPort is:");
			LY::Screen::itoa(port->is);
			LY::Screen::PutString("\nHBA_PxIS_TFES: ");
			LY::Screen::itoa(HBA_PxIS_TFES);
			LY::Screen::PutString("\nPxSAE: ");
			LY::Screen::itoa(port->serr);
			LY::Screen::PutString("\n");
			LY::Screen::itoa(port->ssts);
			LY::Screen::PutString("\n");
			return false;
		}
	}
	if((port->is & HBA_PxIS_TFES) == 1)
	{
		LY::Screen::PutString("Read disk error2\n");
		return false;
	}
	int k = 0;
	/*LY::Screen::PutString("\nSATA active: ");
	LY::Screen::itoa(port->sact);*/
	/*while(port->ci != 0 && port->is==1)
	{
		LY::Screen::PutString("\n[");
		LY::Screen::itoa(k);
		k++;
		LY::Screen::PutString("]");*/
		/*FIS_DATA* datos;
		LY::Memory::Set(&datos, 0, sizeof(FIS_DATA));
		datos->fis_type = FIS_TYPE_DATA;
		LY::Screen::itoa(datos->data[1]);
		LY::Screen::PutString(" ");
	}*/
	return true;
}

bool LY::SATA::Write_Port(HBA_PORT *port, DWORD startl, DWORD starth, DWORD count, DWORD *buf)
{
	//LY::Screen::PutString("Inside writeport \n");
	port->is = (DWORD) - 1;
	int spin = 0;
	int slot = LY::SATA::Find_CMDSlot(port);
	if(slot == -1)
	{
		return false;
	}

	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*) port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(DWORD);

	cmdheader->w = 1;
	cmdheader->prdtl = (WORD)((count - 1) >> 4) + 1;

	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);
	LY::Memory::Set(cmdtbl, 0, sizeof(HBA_CMD_TBL) + (cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY));

	int i;
	for(i = 0; i < cmdheader->prdtl - 1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (DWORD)buf;
		cmdtbl->prdt_entry[i].dbc = 8 * 1024;	
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4 * 1024;
		count -= 16;
	}

	cmdtbl->prdt_entry[i].dba = (DWORD)buf;
	cmdtbl->prdt_entry[i].dbc = count << 9;	
	cmdtbl->prdt_entry[i].i = 1;

	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*) (&cmdtbl->cfis);

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;
	cmdfis->command = ATA_CMD_WRITE_DMA_EX;

	cmdfis->lba0 = (BYTE) startl;
	cmdfis->lba1 = (BYTE) (startl >> 8);
	cmdfis->lba2 = (BYTE) (startl >> 16);
	cmdfis->device = 1 << 6;

	cmdfis->lba3 = (BYTE) (startl >> 24);
	cmdfis->lba4 = (BYTE) starth;
	cmdfis->lba5 = (BYTE) (starth >> 8);
	
	cmdfis->countl = count;
	cmdfis->counth = 0;

	while((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}	
	if(spin == 1000000)
	{
		LY::Screen::PutString("\nPort is hung");
		return false;
	}
	
	port->ci = 1 << slot;	
	
	while(1)
	{
		if((port->ci & (1 << slot)) == 0)
			break;
		if(port->is & HBA_PxIS_TFES)
		{
			LY::Screen::PutString("Write disk error\n");
			return false;
		}
	}
	if(port->is & HBA_PxIS_TFES)
	{
		LY::Screen::PutString("Write disk error\n");
		return false;
	}
	/*while(port->ci != 0)
	{
		LY::Screen::PutString("\n[");
		LY::Screen::itoa(k);
		k++;
		LY::Screen::PutString("]");
	}*/
	return true;
}

int LY::SATA::Find_CMDSlot(HBA_PORT *port)
{
  	DWORD slots = (port->sact | port->ci);
	int num_of_slots = (abar->cap & 0x0F00) >> 8;
	int i;
	for(i = 0; i < num_of_slots; i++)
	{
		if((slots & 1) == 0)
		{
			return i;
		}
		slots >>= 1;
	}
	LY::Screen::PutString("Cannot find free command list entry\n");
	return -1;
}

static int LY::SATA::Check_Type(HBA_PORT *port)
{
	DWORD ssts = port->ssts;
	BYTE ipm = (ssts >> 8) & 0x0F;
	BYTE det = ssts & 0x0F;
	if(det != HBA_PORT_DET_PRESENT)
		return AHCI_DEV_NULL;
	if(ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
	switch(port->sig)
	{
		case SATA_SIG_ATAPI:
			return AHCI_DEV_SATAPI;
		case SATA_SIG_SEMB:
			return AHCI_DEV_SEMB;
		case SATA_SIG_PM:
			return AHCI_DEV_PM;
		default:
			return AHCI_DEV_SATA;
	}
	return 0;
}

void LY::SATA::Rebase_Port(HBA_PORT *port, int portno)
{
	LY::SATA::Stop_CMD(port);
	
	port->clb = AHCI_BASE + (portno << 10);
	port->clbu = 0;
	LY::Memory::Set((void *)(port->clb), 0, 1024);

	port->fb = AHCI_BASE + (32 << 10) + (portno << 8);
	port->fbu = 0;
	LY::Memory::Set((void *)(port->fb), 0, 256);

	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)(port->clb);
	for(int i = 0; i < 32; i++)
	{
		cmdheader[i].p = 1;
		cmdheader[i].prdtl = 8;
		cmdheader[i].ctba = AHCI_BASE + (40 << 10) + (portno << 13) + (i << 8);
		cmdheader[i].ctbau = 0;
		LY::Memory::Set((void*)cmdheader[i].ctba, 0, 256);
	}
	
	LY::SATA::Start_CMD(port);
}

void LY::SATA::Start_CMD(HBA_PORT *port)
{
	while(port->cmd & HBA_PxCMD_CR);
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST;
}

void LY::SATA::Stop_CMD(HBA_PORT *port)
{
	port->cmd &= ~HBA_PxCMD_ST;
	while(1)
	{
		if(port->cmd & HBA_PxCMD_FR)
			continue;
		if(port->cmd & HBA_PxCMD_CR)
			continue;		
		break;
	}
	port->cmd &= ~HBA_PxCMD_FRE;
}

void LY::SATA::Probe_Port(HBA_MEM *abar_temp)
{
	LY::Screen::PutString("Inside probe port\n");
	abar = abar_temp;
	DWORD pi = abar_temp->pi;
	int i = 0;
	while(i < 32)
	{
		if(pi & 1)
		{
			int dt = LY::SATA::Check_Type((HBA_PORT *) &abar_temp->ports[i]);
			if(dt == AHCI_DEV_SATA)
			{
				LY::Screen::PutString("SATA drive found at port ");
				LY::Screen::itoa(i);
				LY::Screen::PutString("\n");	
				abar = abar_temp;
				LY::SATA::Rebase_Port(abar_temp->ports, i);
				LY::Screen::PutString("\nDone AHCI initialisation: Port rebase");
				LY_IRQ_InstallHandler(9,&LY_AHCI_Handler);
				LY::Screen::PutString("\nDone AHCI initialisation: Interrupt handler\n");
				//return;
			}
			else if(dt == AHCI_DEV_SATAPI)
			{
				LY::Screen::PutString("\nSATAPI drive found at port ");
				LY::Screen::itoa(i);
				LY::Screen::PutString("\n");
				abar = abar_temp;
				LY::SATA::Rebase_Port(abar_temp->ports, i);
				LY::Screen::PutString("\nDone AHCI initialisation: Port rebase");
				LY_IRQ_InstallHandler(9,&LY_AHCI_Handler);
				LY::Screen::PutString("\nDone AHCI initialisation: Interrupt handler\n");				
			}
			else if(dt == AHCI_DEV_SEMB)
			{
				LY::Screen::PutString("\nSEMB drive found at port ");
				LY::Screen::itoa(i);
				LY::Screen::PutString("\n");				
			}
			else if(dt == AHCI_DEV_PM)
			{
				LY::Screen::PutString("\nPM drive found at port ");
				LY::Screen::itoa(i);
				LY::Screen::PutString("\n");				
			}
			else
			{
				LY::Screen::PutString("\nNo drive found at port ");
				LY::Screen::itoa(i);
				LY::Screen::PutString("\n");				
			}
		}
		pi >>= 1;
		i++;
	}
	LY::Screen::PutString("\nProbe port complete\n");
}

void LY::SATA::Init_HBA(HBA_MEM *abar)
{
	if(abar->cap2 & 1)
	{
		LY::Screen::PutString("\nAHCI: requesting AHCI ownership change\n");
		abar->bohc |= (1 << 1);
		while((abar->bohc & 1) || !(abar->bohc & (1 << 1)));
		LY::Screen::PutString("AHCI: ownership change completed\n");
	}
	
	abar->ghc |= HBA_GHC_AHCI_ENABLE;
	abar->ghc |= HBA_GHC_RESET;
	
	while(abar->ghc & HBA_GHC_RESET);

	abar->ghc |= HBA_GHC_AHCI_ENABLE;
	abar->ghc |= HBA_GHC_INTERRUPT_ENABLE;
	LY::Timer::Wait(1);
	LY::Screen::PutString("\nAHCI: caps and ver: ");
	LY::Screen::itoa(abar->cap);
	LY::Screen::PutString(" ");
	LY::Screen::itoa(abar->cap2);
	LY::Screen::PutString(" v ");
	LY::Screen::itoa(abar->vs);
	LY::Screen::PutString(", ctl: ");
	LY::Screen::itoa(abar->ghc);
}

bool LY::SATA::Identify_SATA(HBA_PORT *port, DWORD *buf)
{
	port->is = (DWORD) - 1;
	int spin = 0;
	int slot = LY::SATA::Find_CMDSlot(port);
	if(slot == -1)
	{
		return false;
	}

	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*) port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(DWORD);
	cmdheader->prdtl = 1;

	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);
	LY::Memory::Set(cmdtbl, 0, sizeof(HBA_CMD_TBL) + (cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY));
	
	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*) (&cmdtbl->cfis);

	cmdfis->fis_type = 0x27;
	cmdfis->c = 1;
	cmdfis->command = 0xEC;
	cmdfis->device = 0xA0;

	cmdtbl->prdt_entry[0].dba = (DWORD)buf;
	cmdtbl->prdt_entry[0].i = 1;
	cmdtbl->prdt_entry[0].dbc = 0x1FF;

	while((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}	
	if(spin == 1000000)
	{
		LY::Screen::PutString("\nPort is hung");
		return false;
	}
	
	port->ci = 1 << slot;	
	
	while(1)
	{
		if((port->ci & (1 << slot)) == 0)
			break;
		if(port->is & HBA_PxIS_TFES)
		{
			LY::Screen::PutString("Identify CMD error1\n");
			return false;
		}
	}
	if(port->is & HBA_PxIS_TFES)
	{
		LY::Screen::PutString("Identify CMD error2\n");
		return false;
	}

	while(!((port->is >> 1) & 0x01));
	while(!((port->is >> 5) & 0x01));	
	
	port->is |= 0x02;
	port->is |= 0x20;

	while(port->ci & 0x01);

	LY::Screen::PutString("\nReturned true: ");
	LY::Screen::itoa(port->is);
	LY::Screen::PutString("\nSSTS: ");
	LY::Screen::itoa(port->ssts);
	LY::Screen::PutString("\nSERR: ");
	LY::Screen::itoa(port->serr);
	LY::Screen::PutString("\n");
	return true;
	
}

uint32_t LY::SATA::Flush_Commands(HBA_PORT *port)
{
	volatile uint32_t c = port->cmd;
	c = c;
	return c;
}
