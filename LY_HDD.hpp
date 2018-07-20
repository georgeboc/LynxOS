//#ifndef __ATA_H_
#define __ATA_H_

#define ATA_SR_BSY     0x80
#define ATA_SR_DRDY    0x40
#define ATA_SR_DF      0x20
#define ATA_SR_DSC     0x10
#define ATA_SR_DRQ     0x08
#define ATA_SR_CORR    0x04
#define ATA_SR_IDX     0x02
#define ATA_SR_ERR     0x01

#define ATA_ER_BBK      0x80
#define ATA_ER_UNC      0x40
#define ATA_ER_MC       0x20
#define ATA_ER_IDNF     0x10
#define ATA_ER_MCR      0x08
#define ATA_ER_ABRT     0x04
#define ATA_ER_TK0NF    0x02
#define ATA_ER_AMNF     0x01

#define ATA_DATA(x) (x)
#define	ATA_FEATURES(x)	(x + 1)
#define ATA_SECTOR_COUNT(x) (x + 2)
#define ATA_ADDRESS1(x)	(x + 3)
#define ATA_ADDRESS2(x)	(x + 4)
#define ATA_ADDRESS3(x)	(x + 5)
#define ATA_DRIVE_SELECT(x)	(x + 6)
#define ATA_COMMAND(x)	(x + 7)
#define ATA_DCR(x)	(x + 0x206)

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

#define      ATAPI_CMD_READ       0xA8
#define      ATAPI_CMD_EJECT      0x1B

#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200

#define IDE_ATA        0x00
#define IDE_ATAPI      0x01
 
#define ATA_MASTER     0x00
#define ATA_SLAVE      0x01

#define ATA_REG_DATA       0x00
#define ATA_REG_ERROR      0x01
#define ATA_REG_FEATURES   0x01
#define ATA_REG_SECCOUNT0  0x02
#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05
#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_COMMAND    0x07
#define ATA_REG_STATUS     0x07
#define ATA_REG_SECCOUNT1  0x08
#define ATA_REG_LBA3       0x09
#define ATA_REG_LBA4       0x0A
#define ATA_REG_LBA5       0x0B
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_DEVADDRESS 0x0D

#define ATA_PRIMARY 0x00
#define ATA_SECONDARY 0x01

#define ATA_READ 0x00
#define ATA_WRITE 0x01

struct IDEChannelRegisters
{
	unsigned short base;
	unsigned short ctrl;
	unsigned short bmide;
	unsigned short nIEN;
} channels[2];

struct ide_device 
{
	unsigned char Reserved;
	unsigned char channel;
	unsigned char drive;
	unsigned short Type;
	unsigned short Signature;
	unsigned short Capabilities;
	unsigned int CommandSets;
	unsigned int Size;
	unsigned char model[41];
}ide_devices[4];

namespace LY
{
	namespace HDD
	{
		unsigned char IDE_READ(unsigned char channel, unsigned char reg);
		void IDE_WRITE(unsigned char channel, unsigned char reg, unsigned char data);
		unsigned char IDE_POLLING(unsigned char channel, unsigned int advanced_check);
		unsigned char IDE_PRINT_ERROR(unsigned int drive, unsigned char err);
		void IDE_INITIALIZE(unsigned int BAR0, unsigned int BAR1, unsigned int BAR2, unsigned int BAR3, unsigned int BAR4);
		void IDE_READ_BUFFER(unsigned char channel, unsigned char reg, unsigned int buffer, unsigned int quads);
		unsigned char IDE_ATA_ACCESS(unsigned char direction, unsigned char drive, uint64_t lba, unsigned char numsects);
		unsigned char IDE_ATAPI_READ(unsigned char drive, uint64_t lba, unsigned char numsects);
		void IDE_READ_SECTORS(unsigned char drive, unsigned char numsects, uint64_t lba);
		void IDE_WRITE_SECTORS(unsigned char drive, unsigned char numsects, uint64_t lba);
		void IDE_ATAPI_EJECT(unsigned char drive);
		void IDE_WAIT_IRQ(void);
		void IDE_ATAPI_EJECT_ALL(void);
	}
}

extern "C"
void LY_HDD_ATA_PRIMARY_IRQ_HANDLER(struct regs* r);
void LY_HDD_ATA_SECONDARY_IRQ_HANDLER(struct regs* r);
