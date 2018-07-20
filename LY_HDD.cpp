#include "LY_HDD.hpp"

#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170

#define ATA_PRIMARY_DCR_AS 0x3F6
#define ATA_SECONDARY_DCR_AS 0x376

#define ATA_PRIMARY_IRQ 14
#define ATA_SECONDARY_IRQ 15

unsigned char ide_buf[2048] = {0};
unsigned static char ide_irq_invoked = 0;
unsigned static char atapi_packet[12] = {0xA8,0,0,0,0,0,0,0,0,0,0,0};
unsigned int package[10];

unsigned int detected_devices[255][1];
unsigned int detected_devices_pointer = 0;

uint16_t buffer_salida_ATA[255]={0};
unsigned int buffer_salida_ATA_ready=0;
uint16_t buffer_salida_ATAPI[255]={0};
unsigned int buffer_salida_ATAPI_ready=0;

uint64_t sectores_ultimo_ODD = 0;

unsigned char LY::HDD::IDE_READ(unsigned char channel, unsigned char reg)
{
	unsigned char result;
	if(reg > 0x07 && reg < 0x0C)
		LY::HDD::IDE_WRITE(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
	if(reg < 0x08)
		result = LY::Ports::InputB(channels[channel].base + reg - 0x00);
	else if(reg < 0x0C)
		result = LY::Ports::InputB(channels[channel].base + reg - 0x06);
	else if(reg < 0x0E)
		result = LY::Ports::InputB(channels[channel].ctrl + reg - 0x0A);
	else if(reg < 0x16)
		result = LY::Ports::InputB(channels[channel].bmide + reg - 0x0E);
	if(reg > 0x07 && reg < 0x0C)
		LY::HDD::IDE_WRITE(channel, ATA_REG_CONTROL, channels[channel].nIEN);
	return result;
}

void LY::HDD::IDE_WRITE(unsigned char channel, unsigned char reg, unsigned char data)
{
	if(reg > 0x07 && reg < 0x0C)
		LY::HDD::IDE_WRITE(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
	if(reg < 0x08)
		LY::Ports::OutputB(channels[channel].base + reg - 0x00, data);
	else if(reg < 0x0C)
		LY::Ports::OutputB(channels[channel].base + reg - 0x06, data);
	else if(reg < 0x0E)
		LY::Ports::OutputB(channels[channel].ctrl + reg - 0x0A, data);
	else if(reg < 0x16)
		LY::Ports::OutputB(channels[channel].bmide + reg - 0x0E, data);
	if(reg > 0x07 && reg < 0x0C)
		LY::HDD::IDE_WRITE(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

void LY::HDD::IDE_READ_BUFFER(unsigned char channel, unsigned char reg, unsigned int buffer, unsigned int quads)
{
	if(reg > 0x07 && reg < 0x0C)
		LY::HDD::IDE_WRITE(channel, ATA_REG_CONTROL, 0x80|channels[channel].nIEN);
		asm("pushw %es; movw %ds, %ax; movw %ax, %es");
		if(reg < 0x08)
			LY::Ports::InputSL(channels[channel].base + reg - 0x00, buffer, quads);
		else if (reg < 0x0C)
			LY::Ports::InputSL(channels[channel].base + reg - 0x06, buffer, quads);
		else if (reg < 0x0E)
			LY::Ports::InputSL(channels[channel].ctrl + reg - 0x0A, buffer, quads);
		else if (reg < 0x16)
			LY::Ports::InputSL(channels[channel].bmide + reg - 0x0E, buffer, quads);
		asm("popw %es;");
		if(reg > 0x07 && reg < 0x0C)
			LY::HDD::IDE_WRITE(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

unsigned char LY::HDD::IDE_POLLING(unsigned char channel, unsigned int advanced_check)
{
	for(int i = 0; i < 4; i++)
		LY::HDD::IDE_READ(channel, ATA_REG_ALTSTATUS);

	while(LY::HDD::IDE_READ(channel, ATA_REG_STATUS) & ATA_SR_BSY);
	if(advanced_check)
	{	
		unsigned char state = LY::HDD::IDE_READ(channel, ATA_REG_STATUS);
		if(state & ATA_SR_ERR)
			return 2;
		if(state & ATA_SR_DF)
			return 1;
		if((state & ATA_SR_DRQ) == 0)
			return 3;
	}

	return 0;
}

unsigned char LY::HDD::IDE_PRINT_ERROR(unsigned int drive, unsigned char err)
{
	if(err == 0)
		return err;

	LY::Screen::PutString("\nIDE: ");
	if(err == 1){
		LY::Screen::PutString("- Device Fault\n   ");
		err = 19;	
	}
	else if(err == 2)
	{
		unsigned char st = LY::HDD::IDE_READ(ide_devices[drive].channel,ATA_REG_ERROR);
		if(st & ATA_ER_AMNF)
		{
			LY::Screen::PutString("- No Address Mark Found\n   ");
			err=7;
		}
		if(st & ATA_ER_TK0NF)
		{
			LY::Screen::PutString("- No Media or Media Error\n   ");
			err=3;
		}
		if(st & ATA_ER_ABRT)
		{
			LY::Screen::PutString("- Command Aborted\n   ");
			err=20;
		}
		if(st & ATA_ER_MCR)
		{
			LY::Screen::PutString("- No Media or Media Error\n   ");
			err=3;
		}
		if(st & ATA_ER_IDNF)
		{
			LY::Screen::PutString("- ID mark not Found\n   ");
			err=21;
		}
		if(st & ATA_ER_MC)
		{
			LY::Screen::PutString("- No Media or Media Error\n   ");
			err=3;
		}
		if(st & ATA_ER_UNC)
		{
			LY::Screen::PutString("- Uncorrectable Data Error\n   ");
			err=22;
		}
		if(st & ATA_ER_BBK)
		{
			LY::Screen::PutString("- Bad Sectors\n   ");
			err=13;
		}	
	}
	else if(err == 3)
	{
		LY::Screen::PutString("- Reads Nothing\n   ");
		err=23;		
	}
	else if(err == 4)
	{
		LY::Screen::PutString("- Write Protected\n   ");
		err=8;		
	}
	LY::Screen::PutString("- [");
	if(!ide_devices[drive].channel)
	{
		LY::Screen::PutString("Primary ");
	}
	else
	{
		LY::Screen::PutString("Secondary ");
	}

	if(!ide_devices[drive].drive)
	{
		LY::Screen::PutString("Master");
	}
	else
	{
		LY::Screen::PutString("Slave");
	}
	LY::Screen::PutString("] ");
	LY::Screen::PutString((const char*)ide_devices[drive].model);
	return err;
}

void LY::HDD::IDE_INITIALIZE(unsigned int BAR0, unsigned int BAR1, unsigned int BAR2, unsigned int BAR3, unsigned int BAR4)
{
	LY::Screen::PutString("  Installing ATA controller...");
	LY_IRQ_InstallHandler(ATA_PRIMARY_IRQ,&LY_HDD_ATA_PRIMARY_IRQ_HANDLER);
	LY_IRQ_InstallHandler(ATA_SECONDARY_IRQ,&LY_HDD_ATA_SECONDARY_IRQ_HANDLER);
	int j,k,count=0;
	channels[ATA_PRIMARY].base = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
	channels[ATA_PRIMARY].ctrl = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
	channels[ATA_SECONDARY].base = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
	channels[ATA_SECONDARY].ctrl = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
	channels[ATA_PRIMARY].bmide = (BAR4 & 0xFFFFFFFC) + 0;
	channels[ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8;
	LY::HDD::IDE_WRITE(ATA_PRIMARY,ATA_REG_CONTROL,2);
	LY::HDD::IDE_WRITE(ATA_SECONDARY,ATA_REG_CONTROL,2);;
	for (int i = 0; i < 2; i++)
		for(j = 0; j < 2; j++)
		{
			unsigned char err = 0, type = IDE_ATA, status;
			ide_devices[count].Reserved = 0;
			LY::HDD::IDE_WRITE(i,ATA_REG_HDDEVSEL,0xA0|(j<<4));
			LY::Timer::Wait(1);
			LY::HDD::IDE_WRITE(i,ATA_REG_COMMAND,ATA_CMD_IDENTIFY);
			LY::Timer::Wait(1);
			if(LY::HDD::IDE_READ(i,ATA_REG_STATUS)==0) continue;
			while(1)
			{
				status = LY::HDD::IDE_READ(i,ATA_REG_STATUS);
				if((status & ATA_SR_ERR)) { err = 1; break; }
				if(!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) break;
			};
			if(err!=0)
			{
				unsigned char cl = LY::HDD::IDE_READ(i,ATA_REG_LBA1);
				unsigned char ch = LY::HDD::IDE_READ(i,ATA_REG_LBA2);
				if(cl == 0x14 && ch == 0xEB)
					type = IDE_ATAPI;
				else if(cl == 0x69 && ch == 0x96)
					type = IDE_ATAPI;
				else
					continue;
				LY::HDD::IDE_WRITE(i,ATA_REG_COMMAND,ATA_CMD_IDENTIFY_PACKET);
				LY::Timer::Wait(1);
			}
			LY::HDD::IDE_READ_BUFFER(i, ATA_REG_DATA, (unsigned int) ide_buf, 128);
			
			ide_devices[count].Reserved = 1;
			ide_devices[count].Type = type;
			ide_devices[count].channel = i;
			ide_devices[count].drive = j;
			ide_devices[count].Signature = *((unsigned short *)(ide_buf + ATA_IDENT_DEVICETYPE));
			ide_devices[count].Capabilities= *((unsigned short *)(ide_buf + ATA_IDENT_CAPABILITIES));
			ide_devices[count].CommandSets = *((unsigned int *)(ide_buf + ATA_IDENT_COMMANDSETS));

			if(ide_devices[count].CommandSets & (1 << 26))
				ide_devices[count].Size = *((unsigned int *)(ide_buf + ATA_IDENT_MAX_LBA_EXT));
			else
				ide_devices[count].Size = *((unsigned int *)(ide_buf + ATA_IDENT_MAX_LBA));

			for(k=0;k<40;k+=2)
			{
				ide_devices[count].model[k] = ide_buf[ATA_IDENT_MODEL + k + 1];
				ide_devices[count].model[k + 1] = ide_buf[ATA_IDENT_MODEL + k];
			}
			ide_devices[count].model[40] = 0;
			count++;
		}
		LY::Screen::PutString("done\n");
		for(int i=0;i<4;i++)
		{
			if(ide_devices[i].Reserved == 1)
			{			
				LY::Screen::PutString("\n   Found ");
				if(ide_devices[i].Type==0)
				{
					LY::Screen::PutString("ATA");
					detected_devices[i][0] = 0;
				}
				else
				{
					LY::Screen::PutString("ATAPI");
					detected_devices[i][0] = 1;
				}
				LY::Screen::PutString(" Drive ");
				LY::Screen::itoa((ide_devices[i].Size / 1024) / 2);
				LY::Screen::PutString("MB Sectors: ");
				sectores_ultimo_ODD = (uint64_t) (ide_buf + ATA_IDENT_SECTORS);
				LY::Screen::itoa((long long unsigned int)(ide_buf + ATA_IDENT_SECTORS));
				LY::Screen::PutString(" Drive: ");
				LY::Screen::itoa(ide_devices[i].drive);	
				LY::Screen::PutString(" - ");
				LY::Screen::PutString((const char*)ide_devices[i].model);			
				detected_devices[i][1]=(ide_devices[i].channel<<1)|ide_devices[i].drive;
				detected_devices_pointer++;
			}			
		}
		LY::Screen::PutString("Detected ");
		LY::Screen::itoa(detected_devices_pointer);
		if(detected_devices_pointer==1)
		{
			LY::Screen::PutString(" device\n");
		}
		else
		{
			LY::Screen::PutString(" devices\n");
		}
}

unsigned char LY::HDD::IDE_ATA_ACCESS(unsigned char direction, unsigned char drive, uint64_t lba, unsigned char numsects)
{
	unsigned char lba_mode,dma,cmd;
	unsigned char lba_io[6];
	unsigned int channel = ide_devices[drive].channel;
	unsigned int slavebit = ide_devices[drive].drive;
	unsigned int bus = channels[channel].base;
	unsigned int words = 256;
	unsigned short cyl, i;
	unsigned char head, sect, err;

	LY::HDD::IDE_WRITE(channel, ATA_REG_CONTROL, channels[channel].nIEN = (ide_irq_invoked = 0x00) + 0x02);

	if(lba >= 0x10000000)
	{
		lba_mode = 2;
		lba_io[0] = (lba & 0x000000FF) >> 0;
		lba_io[1] = (lba & 0x0000FF00) >> 8;
		lba_io[2] = (lba & 0x00FF0000) >> 16;
		lba_io[3] = (lba & 0xFF000000) >> 24;
		lba_io[4] = 0;
		lba_io[5] = 0;
		head = 0;		
	}
	else if(ide_devices[drive].Capabilities && 0x200)
	{
		lba_mode = 1;
		lba_io[0] = (lba & 0x00000FF) >> 0;
		lba_io[1] = (lba & 0x000FF00) >> 8;
		lba_io[2] = (lba & 0x0FF0000) >> 16;
		lba_io[3] = 0;
		lba_io[4] = 0;
		lba_io[5] = 0;
		head = (lba & 0xF000000) >> 24;
	}
	else
	{
		lba_mode = 0;
		sect = (lba % 63) + 1;
		cyl = (lba + 1 - sect) / (16 * 63);
		lba_io[0] = sect;
		lba_io[1] = (cyl >> 0) & 0xFF;
		lba_io[2] = (cyl >> 8) & 0xFF;
		lba_io[3] = 0;
		lba_io[4] = 0;
		lba_io[5] = 0;	
		head = (lba + 1 - sect) % (16 * 63) / (63);	
	}
	dma = 0; //Activate DMA;
	while (LY::HDD::IDE_READ(channel, ATA_REG_STATUS) & ATA_SR_BSY);

	if(lba_mode == 0)
		LY::HDD::IDE_WRITE(channel, ATA_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head);
	else 
		LY::HDD::IDE_WRITE(channel, ATA_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head);
	
	if(lba_mode == 2)
	{
		LY::HDD::IDE_WRITE(channel, ATA_REG_SECCOUNT1, 0);
		LY::HDD::IDE_WRITE(channel, ATA_REG_LBA3, lba_io[3]);
		LY::HDD::IDE_WRITE(channel, ATA_REG_LBA4, lba_io[4]);
		LY::HDD::IDE_WRITE(channel, ATA_REG_LBA5, lba_io[5]);
	}

	LY::HDD::IDE_WRITE(channel, ATA_REG_SECCOUNT0, numsects);
	LY::HDD::IDE_WRITE(channel, ATA_REG_LBA0, lba_io[0]);
	LY::HDD::IDE_WRITE(channel, ATA_REG_LBA1, lba_io[1]);
	LY::HDD::IDE_WRITE(channel, ATA_REG_LBA2, lba_io[2]);
	
	if(lba_mode == 0 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO;
	if(lba_mode == 1 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO;
	if(lba_mode == 2 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO_EXT;
	if(lba_mode == 0 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA;
	if(lba_mode == 1 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA;
	if(lba_mode == 2 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA_EXT;
	if(lba_mode == 0 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO;
	if(lba_mode == 1 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO;
	if(lba_mode == 2 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO_EXT;
	if(lba_mode == 0 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA;
	if(lba_mode == 1 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA;
	if(lba_mode == 2 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA_EXT;
	LY::HDD::IDE_WRITE(channel, ATA_REG_COMMAND, cmd);

	if(dma)
	{
		if(direction == 0);
			else;
	}
	else
	{
		if(direction == 0)
		{
			for(i = 0; i < numsects; i++)	
			{
				if((err = LY::HDD::IDE_POLLING(channel, 1))!=0)
					return err;
				for(int l = 0; l < 256; l++)
				{
					buffer_salida_ATA[l]=LY::Ports::InputW(0x1F0);
				}
				buffer_salida_ATA_ready=1;
				/*asm("pushw %es");
				asm("mov %%ax, %%es" : : "a"(selector));
				asm("rep insw" : : "c"(words), "d"(bus), "D"(edi));
				asm("popw %es");*/
				//return LY::Ports::InputW(bus);
				//return LY::Ports::OutputSW(bus, words);
				//edi += (words*2);
			}
		}
		else
		{
			for(i = 0; i < numsects; i++)	
			{
				//LY::HDD::IDE_POLLING(channel, 0);
				if((err = LY::HDD::IDE_POLLING(channel, 1))!=0)
					return err;
				for(int l = 0; l < 256; l++)
				{
					LY::Ports::OutputW(0x1F0,l);
					LY::HDD::IDE_WRITE(channel, ATA_REG_COMMAND, (char []) {ATA_CMD_CACHE_FLUSH,
									ATA_CMD_CACHE_FLUSH,
									ATA_CMD_CACHE_FLUSH_EXT}[lba_mode]);
					LY::HDD::IDE_POLLING(channel, 0);
				}
				/*asm("pushw %ds");
				asm("mov %%ax, %%ds" : : "a"(selector));
				asm("rep outsw" : : "c"(words), "d"(bus), "S"(edi));
				asm("popw %ds");*/
				//LY::Ports::OutputW(bus,0x0A);
				//LY::Ports::InputSW(bus, 0x0a ,words);
				//edi += (words*2);
			}			
		}
		return 0;	
	}	
}

unsigned char LY::HDD::IDE_ATAPI_READ(unsigned char drive, uint64_t lba, unsigned char numsects)
{
	unsigned int channel = ide_devices[drive].channel;
	unsigned int slavebit = ide_devices[drive].drive;
	unsigned int bus = channels[channel].base;
	unsigned int words = 1024;
	unsigned char err;
	int i;
	
	LY::HDD::IDE_WRITE(channel, ATA_REG_CONTROL, channels[channel].nIEN = ide_irq_invoked = 0x00);
	atapi_packet[0] = ATAPI_CMD_READ;
	atapi_packet[1] = 0x00;
	atapi_packet[2] = (lba >> 24) & 0xFF;
	atapi_packet[3] = (lba >> 16) & 0xFF;
	atapi_packet[4] = (lba >> 8) & 0xFF;
	atapi_packet[5] = (lba >> 0) & 0xFF;
	atapi_packet[6] = 0x00;
	atapi_packet[7] = 0x00;
	atapi_packet[8] = 0x00;
	atapi_packet[9] = numsects;
	atapi_packet[10] = 0x00;
	atapi_packet[11] = 0x00;

	LY::HDD::IDE_WRITE(channel, ATA_REG_HDDEVSEL, slavebit << 4);
	
	for(int i = 0; i < 4; i++)
		LY::HDD::IDE_READ(channel, ATA_REG_ALTSTATUS);

	LY::HDD::IDE_WRITE(channel, ATA_REG_FEATURES, 0);
	LY::HDD::IDE_WRITE(channel, ATA_REG_LBA1, (words * 2) & 0xFF);
	LY::HDD::IDE_WRITE(channel, ATA_REG_LBA2, (words * 2) >> 8);
	LY::HDD::IDE_WRITE(channel, ATA_REG_COMMAND, ATA_CMD_PACKET);
	
	if(err = LY::HDD::IDE_POLLING(channel, 1)) return err;
	
	asm("rep outsw" : : "c"(6), "d"(bus), "S"((uint16_t *)atapi_packet));
	for(i = 0; i < numsects; i++)
	{
		//LY::HDD::IDE_POLLING(1,1);
		LY::HDD::IDE_WAIT_IRQ();
		if(err = LY::HDD::IDE_POLLING(channel, 1))
			return err;
		for(int l = 0; l < 256; l++)
		{
			buffer_salida_ATAPI[l]=LY::Ports::InputW(channels[channel].base);
		}
		buffer_salida_ATAPI_ready=1;
		//edi += (words * 2);
	}
	LY::HDD::IDE_WAIT_IRQ();
	//LY::HDD::IDE_POLLING(1,1);
	while(LY::HDD::IDE_READ(channel, ATA_REG_STATUS) & (ATA_SR_BSY | ATA_SR_DRQ));

	return 0;
}

void LY::HDD::IDE_READ_SECTORS(unsigned char drive, unsigned char numsects, uint64_t lba)
{
	if(drive > 3 || ide_devices[drive].Reserved == 0) package[0] = 0x01;
	else if (((lba + numsects) > ide_devices[drive].Size) && (ide_devices[drive].Type == IDE_ATA))
		package[0] = 0x02;
	else
	{
		unsigned char err;
		if(ide_devices[drive].Type == IDE_ATA)
			err = LY::HDD::IDE_ATA_ACCESS(ATA_READ, drive, lba, numsects);
		else if(ide_devices[drive].Type == IDE_ATAPI)
			for(int i = 0; i< numsects; i++)
				err = LY::HDD::IDE_ATAPI_READ(drive, lba + i, 1);
		package[0] = LY::HDD::IDE_PRINT_ERROR(drive, err);
	}
}

void LY::HDD::IDE_WRITE_SECTORS(unsigned char drive, unsigned char numsects, uint64_t lba)
{
	if(drive > 3 || ide_devices[drive].Reserved == 0)
		package[0] = 0x01;
	else if ((lba + numsects) > ide_devices[drive].Size && (ide_devices[drive].Type == IDE_ATA))
		package[0] = 0x02;
	else
	{
		unsigned char err;
		if(ide_devices[drive].Type == IDE_ATA)
			err = LY::HDD::IDE_ATA_ACCESS(ATA_WRITE, drive, lba, numsects);
		else if(ide_devices[drive].Type == IDE_ATAPI)
			err = 4;
		package[0] = LY::HDD::IDE_PRINT_ERROR(drive, err);
	}
}

void LY::HDD::IDE_ATAPI_EJECT_ALL(void)
{
	unsigned int device = 0;
	for(int i = 0; i < 4; i++)
	{
		if(detected_devices[i][0] == 1)
		{
			LY::HDD::IDE_ATAPI_EJECT(i);
			LY::Screen::PutString("\nDevice ");
			LY::Screen::itoa(i);
			LY::Screen::PutString(" ejected (drive: ");
			device = i & 1;			
			LY::Screen::itoa(device);
			LY::Screen::PutString(", channel: ");
			device = i >> 1;
			LY::Screen::itoa(device);
			LY::Screen::PutString(")\n");
		}
	}
}

void LY::HDD::IDE_ATAPI_EJECT(unsigned char drive)
{
	unsigned int channel = ide_devices[drive].channel;
	unsigned int slavebit = ide_devices[drive].drive;
	unsigned int bus = channels[channel].base;
	unsigned int words = 2048/2;
	unsigned char err = 0;
	ide_irq_invoked = 0;
	
	if(drive > 3 || ide_devices[drive].Reserved == 0)
		package[0] = 0x01;
	else if(ide_devices[drive].Type == IDE_ATA)
		package[0] = 20;
	else	
	{
		LY::HDD::IDE_WRITE(channel, ATA_REG_CONTROL, channels[channel].nIEN = ide_irq_invoked = 0x00);
		atapi_packet[0] = ATAPI_CMD_EJECT;
		atapi_packet[1] = 0x00;
		atapi_packet[2] = 0x00;
		atapi_packet[3] = 0x00;
		atapi_packet[4] = 0x02;
		atapi_packet[5] = 0x00;
		atapi_packet[6] = 0x00;
		atapi_packet[7] = 0x00;
		atapi_packet[8] = 0x00;
		atapi_packet[9] = 0x00;
		atapi_packet[10] = 0x00;
		atapi_packet[11] = 0x00;

		LY::HDD::IDE_WRITE(channel, ATA_REG_HDDEVSEL, slavebit << 4);

		for(int i = 0; i < 4; i++)
			LY::HDD::IDE_READ(channel, ATA_REG_ALTSTATUS);

		LY::HDD::IDE_WRITE(channel, ATA_REG_COMMAND, ATA_CMD_PACKET);
		err = LY::HDD::IDE_POLLING(channel, 1);
		
		if(err == 0)
		{
			asm("rep outsw"::"c"(6), "d"(bus), "S"(atapi_packet));
			LY::HDD::IDE_WAIT_IRQ();
			err = LY::HDD::IDE_POLLING(channel, 1);
			if(err == 3) err = 0;
		}

		package[0] = LY::HDD::IDE_PRINT_ERROR(drive,err);
	} 
}

void LY::HDD::IDE_WAIT_IRQ(void)
{
	while(!ide_irq_invoked);
	ide_irq_invoked = 1;
	return;
}

void LY_HDD_ATA_PRIMARY_IRQ_HANDLER(struct regs* r)
{
		LY::Screen::PutString("\nInterrupt requested from primary IDE");
		ide_irq_invoked = 1;
}

void LY_HDD_ATA_SECONDARY_IRQ_HANDLER(struct regs* r)
{
		LY::Screen::PutString("\nInterrupt requested from secondary IDE");
		ide_irq_invoked = 1;	
}
