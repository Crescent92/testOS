#ifndef _PCI_H
#define _PCI_H

#include <type.h>


#define PCI_VENDOR_ID   0x00
#define PCI_DEVICE_ID   0X02
#define PCI_COMMAND     0x04
#define PCI_STATUS      0x06
#define PCI_REVISION_ID 0x08

#define PCI_PROG_IF 0x09
#define PCI_SUBCLASS    0x0A
#define PCI_CLASS   0x0B
#define PCI_CACHE_LINE_SZ   0x0C
#define PCI_LATENCY_TIMER   0X0D
#define PCI_HEADER_TYPE 0X0E
#define PCI_BIST    0X0F
#define PCI_BAR0    0x10
#define pCI_BAR1    0x14
#define PCI_BAR2    0x18
#define PCI_BAR3    0x1c
#define PCI_BAR4    0x20
#define PCI_BAR5    0x24

#define PCI_INTERRUPT_LINE  0x3C

#define PCI_MULTIFUNCTION_DEV   0x80

#define PCI_SECONDARY_BUS   0x19

#define PCI_TYPE_BRIDGE 0x604
#define PCI_TYPE_SATA   0x106

#define PCI_HEADER_TYPE_DEVICE  0 
#define PCI_HEADER_TYPE_BRIDGE  1
#define PCI_HEADER_TYPE_CARDBUS 2

#define PCI_ADDRESS_PORT    0xCF8
#define PCI_VALUE_PORT      0xCFC

#define PCI_INVALID_VENDOR_ID   0xFFFF

#define PCI_CLASS_MASS_STORAGE  0x01
#define PCI_CLASS_BRIDGE_DEVICE 0x06

#define PCI_SUBCLASS_IDE    0x01
#define PCI_SUBCLASS_PCI_TO_PCI_BRIDGE  0x04

typedef struct _pci_dev {
    uint8 bus;
    uint8 dev;
    uint8 fn;
    uint32 addr;
    uint32 dev_id;
    uint32 ven_id;
    uint8 class;
    uint8 subclass;
    uint8 header_type;
    uint8 int_line;
    struct _pci_dev *next;
} pci_dev;


void pci_init() ;
pci_dev *find_pci_dev(uint32 ven_id, uint32 dev_id);
#endif