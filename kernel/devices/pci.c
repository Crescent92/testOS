#include "pci.h"
#include "io.h"
#include <mem/mem.h>

#define PCI_CREATE_ADDR(bus, dev, fn) ((((uint32)bus) << 16) | (((uint32)dev) << 11) | (((uint32)fn) << 8) | ((uint32)0x80000000))
#define PCI_CREATE_ADDR_OFF(bus, dev, fn, offset) ((PCI_CREATE_ADDR(bus, dev, fn)) | (offset & 0xfc))

pci_dev* pci_dev_list = 0;

void _print_pci(pci_dev *ptr) {
    printf("bdf:%d:%d:%d | vend: %x dev: %x | class:%x subclass: %x\n",
        ptr->bus, ptr->dev, ptr->fn, ptr->ven_id, ptr->dev_id, ptr->class, ptr->subclass);
        ptr = ptr->next;
}

uint32 pci_config_get_addr(uint8 bus, uint8 dev, uint8 fn, uint8 offset) {
    return (PCI_CREATE_ADDR_OFF(bus, dev, fn, offset));
}

uint32 pci_config_read_reg(uint32 addr) {
    outl(PCI_ADDRESS_PORT, addr);
    return inl(PCI_VALUE_PORT);
}

void pci_config_write_reg(uint32 addr, uint32 val) {
    outl(PCI_ADDRESS_PORT, addr);
    outl(PCI_VALUE_PORT, val);
}

uint16 pci_get_dev_id(uint8 bus, uint8 dev, uint8 fn) {
    uint32 reg = pci_config_read_reg((PCI_CREATE_ADDR_OFF(bus, dev, fn, 0x0)));
    return ((reg >> 16) & 0xFFFF);
}

uint16 pci_get_vendor_id(uint8 bus, uint8 dev, uint8 fn) {
    uint32 reg = pci_config_read_reg((PCI_CREATE_ADDR_OFF(bus, dev, fn, 0x0)));
    return ((reg) & 0xFFFF);
}

uint16 pci_get_status(uint8 bus, uint8 dev, uint8 fn) {
    uint32 reg = pci_config_read_reg((PCI_CREATE_ADDR_OFF(bus, dev, fn, 0x4)));
    return ((reg >> 16) & 0xFFFF);
}

uint16 pci_get_cmd(uint8 bus, uint8 dev, uint8 fn) {
    uint32 reg = pci_config_read_reg((PCI_CREATE_ADDR_OFF(bus, dev, fn, 0x4)));
    return ((reg) & 0xFFFF);
}

uint16 pci_get_class_code(uint8 bus, uint8 dev, uint8 fn) {
    uint32 reg = pci_config_read_reg((PCI_CREATE_ADDR_OFF(bus, dev, fn, 0x8)));
    return ((reg >> 24) & 0xF);
}

uint16 pci_get_subclass_code(uint8 bus, uint8 dev, uint8 fn) {
    uint32 reg = pci_config_read_reg((PCI_CREATE_ADDR_OFF(bus, dev, fn, 0x8)));
    return ((reg >> 16) & 0xFF);
}

uint16 pci_get_prog_if(uint8 bus, uint8 dev, uint8 fn) {
    uint32 reg = pci_config_read_reg((PCI_CREATE_ADDR_OFF(bus, dev, fn, 0x8)));
    return ((reg >> 8) & 0xFF);
}

uint16 pci_get_rev_id(uint8 bus, uint8 dev, uint8 fn) {
    uint32 reg = pci_config_read_reg((PCI_CREATE_ADDR_OFF(bus, dev, fn, 0x8)));
    return ((reg) & 0xF);
}

uint16 pci_get_bist(uint8 bus, uint8 dev, uint8 fn) {
    uint32 reg = pci_config_read_reg((PCI_CREATE_ADDR_OFF(bus, dev, fn, 0xC)));
    return ((reg >> 24) & 0xF);
}

uint16 pci_get_headertype(uint8 bus, uint8 dev, uint8 fn) {
    uint32 reg = pci_config_read_reg((PCI_CREATE_ADDR_OFF(bus, dev, fn, 0xC)));
    return ((reg >> 16) & 0xFF);
}

uint16 pci_get_latencytimer(uint8 bus, uint8 dev, uint8 fn) {
    uint32 reg = pci_config_read_reg((PCI_CREATE_ADDR_OFF(bus, dev, fn, 0xC)));
    return ((reg >> 8) & 0xF);
}

uint16 pci_get_cachelnsz(uint8 bus, uint8 dev, uint8 fn) {
    uint32 reg = pci_config_read_reg((PCI_CREATE_ADDR_OFF(bus, dev, fn, 0xC)));
    return ((reg) & 0xF);
}

uint16 pci_get_interrupt_ln(uint8 bus, uint8 dev, uint8 fn) {
    uint32 reg = pci_config_read_reg((PCI_CREATE_ADDR_OFF(bus, dev, fn, 0x3C)));
    return ((reg >> 18) & 0xFF);
}


uint16 pci_get_secondary_bus(uint8 bus, uint8 dev, uint8 fn) {
    uint32 reg = pci_config_read_reg((PCI_CREATE_ADDR_OFF(bus, dev, fn, 0x18)));
    return ((reg) & 0xFF);
}


// void check_fn(uint8 bus, uint8 dev, uint8 fn) {
//     uint16 sec_bus;
//     reg_dev(bus, dev, fn);

//     uint16 base_class = pci_get_class_code(bus, dev, fn);
//     uint16 sub_class = pci_get_subclass_code(bus, dev, fn);

//     if ((base_class == PCI_CLASS_BRIDGE_DEVICE) && (sub_class == PCI_SUBCLASS_PCI_TO_PCI_BRIDGE)) {
//         sec_bus = pci_get_secondary_bus(bus, dev, fn);
//         pci_scan_bus(sec_bus);
//     }
// }


void add_dev_to_list(uint8 bus, uint8 dev, uint8 fn) {
    pci_dev *d = (pci_dev *)kmalloc(sizeof(pci_dev));
    pci_dev *temp = 0;
    
    memset(d, 0, sizeof(pci_dev));
    d->bus = bus;
    d->dev = dev;
    d->fn = fn;
    d->ven_id = pci_get_vendor_id(bus, dev, fn);
    d->class = pci_get_class_code(bus, dev, fn);
    d->subclass = pci_get_subclass_code(bus, dev, fn);
    d->int_line = pci_get_interrupt_ln(bus, dev, fn);
    d->dev_id = pci_get_dev_id(bus, dev, fn);
    d->header_type = pci_get_headertype(bus, dev, fn);
    // _print_pci(d);
    if (pci_dev_list == NULL) {
        pci_dev_list = d;
        return;
    }

    temp = pci_dev_list;

    while (temp->next) {
        temp = temp->next;
    }
    temp->next = d;

}

void check_device(uint8 bus, uint8 dev) {
    uint8 fn = 0;
    uint16 vendor_id;
    uint8 header_type;
    vendor_id = pci_get_vendor_id(bus, dev, fn);
    if (vendor_id == PCI_INVALID_VENDOR_ID) 
        return;
    //reg/add_dev
    add_dev_to_list(bus, dev, fn);
    header_type = pci_get_headertype(bus, dev, fn);
    if((header_type & PCI_MULTIFUNCTION_DEV) != 0) {
        for (fn = 1; fn < 8; fn++) {
            if(pci_get_vendor_id(bus, dev, fn) != PCI_INVALID_VENDOR_ID) {
                //reg/add dev
                add_dev_to_list(bus, dev, fn);
            }
        }
    }
}

pci_dev *find_pci_dev(uint32 ven_id, uint32 dev_id) {
    if (pci_dev_list == NULL) {
        return NULL;
    }

    pci_dev *temp = pci_dev_list;
    while (temp != NULL) {
        if ((temp->ven_id == ven_id) && (temp->dev_id == dev_id)) {
            return temp;
        }
        temp = temp->next;
    }

    return NULL;
}

void _print_pci_list () {
    pci_dev *ptr = pci_dev_list;
    uint32 dev_cnt = 0;
    while (ptr != NULL) {
        printf("bdf:%d:%d:%d | vend: %x dev: %x | class:%x subclass: %x\n",
        ptr->bus, ptr->dev, ptr->fn, ptr->ven_id, ptr->dev_id, ptr->class, ptr->subclass);
        ptr = ptr->next;
        dev_cnt++;
    }
    printf("Total PCI devices: %d\n", dev_cnt);
}

void scan_buses() {
    uint16  bus;
    uint8   dev;

    for (bus = 0; bus < 256; bus++) {
        for (dev = 0; dev < 32; dev++) {
            // printf("dev: %d bus: %d\n", dev, bus);
            check_device(bus, dev);
        }
    }

}

void pci_init() {
    scan_buses();
    _print_pci_list();
}