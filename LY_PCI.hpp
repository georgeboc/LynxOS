#ifndef __PCI_H_
#define __PCI_H_

struct __pci_driver;

typedef struct
{
	uint32_t vendor;
	uint32_t device;
	uint32_t clase;
	uint32_t subclase;
	uint32_t progIF;
	uint32_t bus;
	uint32_t slot;
	uint32_t func;
	uint32_t BAR0;
	uint32_t BAR1;
	uint32_t BAR2;
	uint32_t BAR3;
	uint32_t BAR4;
	uint32_t BAR5;
	struct __pci_driver *driver;
} pci_device;

typedef struct
{
	uint32_t vendor;
	uint32_t device;
	uint32_t func;
} pci_device_id;

typedef struct __pci_driver
{
	pci_device_id *table;
	char *name;
	uint8_t (*init_one)(pci_device *);
	uint8_t (*init_driver)(void);
	uint8_t (*exit_driver)(void);
} pci_driver;

namespace LY
{
	namespace PCI
	{
		void Add_pci_device(pci_device *pdev);
		uint16_t ReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
		uint32_t ReadLong(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
		uint16_t GetVendorID(uint16_t bus, uint16_t device, uint16_t function);
		uint16_t GetDeviceID(uint16_t bus, uint16_t device, uint16_t function);
		uint16_t GetClassID(uint16_t bus, uint16_t device, uint16_t function);
		uint16_t GetSubClassID(uint16_t bus, uint16_t device, uint16_t function);
		uint16_t GetProgIF(uint16_t bus, uint16_t device, uint16_t function);
		uint32_t GetBAR0(uint16_t bus, uint16_t device, uint16_t function);
		uint32_t GetBAR1(uint16_t bus, uint16_t device, uint16_t function);
		uint32_t GetBAR2(uint16_t bus, uint16_t device, uint16_t function);
		uint32_t GetBAR3(uint16_t bus, uint16_t device, uint16_t function);
		uint32_t GetBAR4(uint16_t bus, uint16_t device, uint16_t function);
		uint32_t GetBAR5(uint16_t bus, uint16_t device, uint16_t function);
		void Probe(void);
		uint16_t CheckVendor(uint8_t bus, uint8_t slot);
		void Init(void);
		void Register_driver(pci_driver *driv);
		void Proc_dump(uint8_t *buffer);
	}
}

#endif
