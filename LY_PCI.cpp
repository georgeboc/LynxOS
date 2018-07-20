#include "LY_PCI.hpp"
#include "LY_Memory.hpp"

/*unsigned int PCI_devices_detected[139][7];
unsigned int PCI_devices_detected_pointer=0;*/

pci_device **pci_devices = 0;
uint32_t devs = 0;

pci_driver **pci_drivers = 0;
uint32_t drivs = 0;

uint64_t *pages_for_ahci_start;

void LY::PCI::Add_pci_device(pci_device *pdev)
{
	pci_devices[devs] = pdev;
	devs++;
	return;
}

uint16_t LY::PCI::ReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
	uint32_t address;
	uint32_t lbus = (uint32_t)bus;
	uint32_t lslot = (uint32_t)slot;
	uint32_t lfunc = (uint32_t)func;
	uint16_t tmp = 0;

	address = (uint32_t)((bus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
	LY::Ports::OutputL(0xCF8, address);
	tmp = (uint16_t)((LY::Ports::InputL(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
	return (tmp);
}

uint32_t LY::PCI::ReadLong(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
	uint32_t RLong = 0;
	RLong = LY::PCI::ReadWord(bus, slot, func, offset + 0x02) << 16;
	RLong |= LY::PCI::ReadWord(bus, slot, func, offset);	
	return RLong;
}

uint16_t LY::PCI::GetVendorID(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = LY::PCI::ReadWord(bus, device, function, 0);
	return r0;
}

uint16_t LY::PCI::GetDeviceID(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = LY::PCI::ReadWord(bus, device, function, 2);
	return r0;
}

uint32_t LY::PCI::GetBAR0(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = LY::PCI::ReadLong(bus, device, function, 0x10);
	if(r0 & 0x01)
		return r0 & 0xFFFFFFF0;
	else
		return r0 & 0xFFFFFFFC;
}

uint32_t LY::PCI::GetBAR1(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = LY::PCI::ReadLong(bus, device, function, 0x14);
	if(r0 & 0x01)
		return r0 & 0xFFFFFFF0;
	else
		return r0 & 0xFFFFFFFC;
}

uint32_t LY::PCI::GetBAR2(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = LY::PCI::ReadLong(bus, device, function, 0x18);
	if(r0 & 0x01)
		return r0 & 0xFFFFFFF0;
	else
		return r0 & 0xFFFFFFFC;
}

uint32_t LY::PCI::GetBAR3(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = LY::PCI::ReadLong(bus, device, function, 0x1C);
	if(r0 & 0x01)
		return r0 & 0xFFFFFFF0;
	else
		return r0 & 0xFFFFFFFC;
}

uint32_t LY::PCI::GetBAR4(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = LY::PCI::ReadLong(bus, device, function, 0x20);
	if(r0 & 0x01)
		return r0 & 0xFFFFFFF0;
	else
		return r0 & 0xFFFFFFFC;
}

uint32_t LY::PCI::GetBAR5(uint16_t bus, uint16_t device, uint16_t function)
{
	uint64_t r0 = LY::PCI::ReadLong(bus, device, function, 0x24);
	if(r0 & 0x01)
		return r0 & 0xFFFFFFF0;
	else
		return r0 & 0xFFFFFFFC;
}


uint16_t LY::PCI::GetClassID(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = LY::PCI::ReadWord(bus, device, function, 0x0A);
	return (r0 & ~0x00FF) >> 8;
}

uint16_t LY::PCI::GetSubClassID(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = LY::PCI::ReadWord(bus, device, function, 0x0A);
	return (r0 & ~0xFF00);
}

uint16_t LY::PCI::GetProgIF(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = LY::PCI::ReadWord(bus, device, function, 0x0C);
	return (r0 & ~0x00FF) >> 8;
}

void LY::PCI::Probe(void)
{
	for(uint32_t bus = 0; bus < 256; bus++)
	{
		for(uint32_t slot = 0; slot < 32; slot++)
		{
			for(uint32_t function = 0; function < 8; function++)
			{
				uint16_t vendor = LY::PCI::GetVendorID(bus,slot,function);
				if(vendor == 0xFFFF) continue;
				uint16_t device = LY::PCI::GetDeviceID(bus, slot, function);
				uint16_t clase = LY::PCI::GetClassID(bus, slot, function);
				uint16_t subclase = LY::PCI::GetSubClassID(bus, slot, function);
				uint16_t progIF = LY::PCI::GetProgIF(bus, slot, function);
				uint32_t BAR0 = LY::PCI::GetBAR0(bus, slot, function);
				uint32_t BAR1 = LY::PCI::GetBAR1(bus, slot, function);
				uint32_t BAR2 = LY::PCI::GetBAR2(bus, slot, function);
				uint32_t BAR3 = LY::PCI::GetBAR3(bus, slot, function);
				uint32_t BAR4 = LY::PCI::GetBAR4(bus, slot, function);
				uint32_t BAR5 = LY::PCI::GetBAR5(bus, slot, function);

				/*if(clase == 0x01)
				{
					LY::Screen::PutString("\nvendor: ");
					LY::Screen::itoa(vendor);
					LY::Screen::PutString(" device: ");
					LY::Screen::itoa(device);
					LY::Screen::PutString(" class: ");
					LY::Screen::itoa(clase);
					LY::Screen::PutString(" subclass: ");
					LY::Screen::itoa(subclase);
					LY::Screen::PutString(" prog IF: ");
					LY::Screen::itoa(progIF);
					LY::Ports::OutputL(0xCF8, (1 << 31) | (bus << 16) | (slot << 11) | (function << 8) | 8);
					if(LY::Ports::InputL(0xCFC) >> 16 != 0xFFFF)
					{
						LY::Ports::OutputL(0xCF8, (1 << 31) | (bus << 16) | (slot << 11) | (function << 8) | 0x3C);
						LY::Ports::OutputB(0xCFC, 0xFE);
						LY::Ports::OutputL(0xCF8, (1 << 31) | (bus << 16) | (slot << 11) | (function << 8) | 0x3C);
						if((LY::Ports::InputL(0xCFC) & 0xFF) == 0xFE)
						{
							LY::Screen::PutString("\nThis device needs an IRQ assignement\n");
						}
						else
						{
							if(clase == 0x01 && subclase== 0x01)
							{
								LY::Screen::PutString("\nThis device is a IDE device\n");
							}
						}
					}
				}*/

				pci_device *pdev = (pci_device *)LY::Memory::Malloc(sizeof(pci_device));
				pdev->vendor = vendor;
				pdev->device = device;
				pdev->clase = clase;
				pdev->subclase = subclase;				
				pdev->progIF = progIF;
				pdev->bus = bus;
				pdev->slot = slot;
				pdev->func = function;
				pdev->driver = 0;
				pdev->BAR0 = BAR0;
				pdev->BAR1 = BAR1;
				pdev->BAR2 = BAR2;
				pdev->BAR3 = BAR3;
				pdev->BAR4 = BAR4;
				pdev->BAR5 = BAR5;
				LY::PCI::Add_pci_device(pdev);
				
				//LY::Timer::Wait(4);
			}
		}
	}
}

uint16_t LY::PCI::CheckVendor(uint8_t bus, uint8_t slot)
{
	uint16_t vendor, device;
	if((vendor = LY::PCI::ReadWord(bus,slot,0,0)) != 0xFFFF)
	{
		device = LY::PCI::ReadWord(bus,slot,0,2);
	}
	return (vendor);
}

void LY::PCI::Init(void)
{
	LY::Screen::SetColor(LY_SIDE_FOREGROUND, LY_COLOR_WHITE);
	LY::Screen::PutString("\nSetup PCI...");
	devs = drivs = 0;
	pci_devices = (pci_device **)LY::Memory::Malloc(32 * sizeof(pci_devices));
	pci_drivers = (pci_driver **)LY::Memory::Malloc(32 * sizeof(pci_driver));
	LY::PCI::Probe();
	LY::Screen::SetColor(LY_SIDE_FOREGROUND, LY_COLOR_WHITE);
	LY::Screen::PutString("done");
	LY::Screen::PutString("\nAfter scanning PCI, were found ");
	LY::Screen::itoa(devs);
	LY::Screen::PutString(" devices");
}

void LY::PCI::Register_driver(pci_driver *driv)
{
	pci_drivers[drivs] = driv;
	drivs ++;
	return;
}

void LY::PCI::Proc_dump(uint8_t *buffer)
{
	for(int i = 0; i < devs; i++)
	{
		pci_device *pci_dev = pci_devices[i];
		if(pci_dev->driver)
		{
			LY::Screen::PutString("\n[");
			LY::Screen::itoa(pci_dev->vendor);
			LY::Screen::PutString(":");
			LY::Screen::itoa(pci_dev->device);
			LY::Screen::PutString(":");
			LY::Screen::itoa(pci_dev->func);
			LY::Screen::PutString("] => ");
			LY::Screen::itoa((unsigned long long)pci_dev->driver->name);
		}
		else
		{
			LY::Screen::PutString("\n[");
			LY::Screen::itoa(pci_dev->vendor);
			LY::Screen::PutString(":");
			LY::Screen::itoa(pci_dev->device);
			LY::Screen::PutString(":");
			LY::Screen::itoa(pci_dev->func);
			LY::Screen::PutString("]");
		}
	}
}
