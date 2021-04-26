#include "ata.h"
#include "pci.h"
#include <cpu/irq.h>
#include "io.h"

pci_dev *ata_dev;

uint8 ide_buf[512] = {0};

void ide_write(unsigned char channel, unsigned char reg, unsigned char data) {
    if (reg > 0x07 && reg < 0x0C) {
        ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    }

    if (reg < 0x08) {
        outb(channels[channel].bmide + reg - 0x00, data);
    } else if (reg < 0x0C) {
        outb(channels[channel].bmide + reg - 0x06, data);
    } else if (reg < 0x0E) {
        outb(channels[channel].bmide + reg - 0x0A, data);
    } else if (reg < 0x16) {
        outb(channels[channel].bmide + reg - 0x0E, data);
    }

    if (reg > 0x07 && reg < 0x0C) {
        ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
    }
}

unsigned char ide_read(unsigned char channel, unsigned char reg) {
    unsigned char res;
    if (reg > 0x07 && reg < 0x0C) {
        ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    } 

    if (reg < 0x08) {
        res = inb(channels[channel].base + reg - 0x00);
    } else if (reg < 0x0C) {
        res = inb(channels[channel].base + reg - 0x06);
    } else if (reg < 0x0E) {
        res = inb(channels[channel].base + reg - 0x0A);
    } else if (reg < 0x16) {
        res = inb(channels[channel].base + reg - 0x0E);
    }

    if (reg > 0x07 && reg < 0x0C) {
        ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
    }

    return res;
}

void ata_delay_400ns(uint16 io) {
    int i;

    for (i = 0; i < 4; i++) {
        inb(io + ATA_REG_ALTSTATUS);
    }
}

unsigned char ide_polling(unsigned char channel, unsigned int adv_check) {
    ata_delay_400ns(channel);

    while(ide_read(channel, ATA_REG_STATUS) & ATA_STATUS_REG_BSY)
        ;

    if (adv_check) {
        unsigned char state = ide_read(channel, ATA_REG_STATUS);

        if (state & ATA_STATUS_REG_ERR) {
            return 2;
        }

        if (state & ATA_STATUS_REG_DF) {
            return 1;
        }

        if ((state & ATA_STATUS_REG_DRQ) == 0) {
            return 3;
        }
    }

    return 0;
}

void ide_init(unsigned int BAR0, unsigned int BAR1, unsigned int BAR2, unsigned int BAR3, unsigned int BAR4) {
    int j, km count = 0;

    channels[ATA_MASTER].base = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
    channels[ATA_MASTER].ctrl = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
    channels[ATA_SLAVE].base = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
    channels[ATA_SLAVE].ctrl = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
    channels[ATA_MASTER].bmide = (BAR4 & 0xFFFFFFFC) + 0;
    channels[ATA_SLAVE].bmide = (BAR4 & 0xFFFFFFFC) + 8;
}


void ata_handler (registers_t *regs) {

}

void ata_sel_drive (uint8 bus, uint8 i) {
    if (bus == ATA_PRIMARY) {
        if (i == ATA_MASTER) {
            outb(ATA0_IOADDR + ATA_REG_HDDEVSEL, 0xA0);
        } else {
            outb(ATA0_IOADDR + ATA_REG_HDDEVSEL, 0xB0);
        }
    } else {
        if (i == ATA_MASTER) {
            outb(ATA1_IOADDR + ATA_REG_HDDEVSEL, 0xA0);
        } else {
            outb(ATA1_IOADDR + ATA_REG_HDDEVSEL, 0xB0);
        }
    }
}

uint8 ata_identify(uint8 bus, uint8 drive) {
    uint16 io = (bus == ATA_PRIMARY) ? ATA0_IOADDR : ATA1_IOADDR;
    ata_sel_drive(bus, drive);

    outb(io + ATA_REG_SECCOUNT0, 0);
    outb(io + ATA_REG_LBA0, 0);
    outb(io + ATA_REG_LBA1, 0);
    outb(io + ATA_REG_LBA2, 0);

    outb(io + ATA_REG_COMMAND , ATA_CMD_IDENTIFY);

    uint8 status = inb(io + ATA_REG_STATUS);

    if (!(inb()))

}

uint8 ata_poll(uint16 io) {
    uint status;
    ata_delay_400ns(io);

    while(1) {

        while (inb(io_base + ATA_REG_STATUS) & ATA_STATUS_REG_BSY) 
            ;

        status = inb(io_base + ATA_REG_STATUS);

        if (status & ATA_STATUS_REG_ERR) {
            //print and ret error
        }

        if (inb(io_base + ATA_REG_STATUS) & ATA_STATUS_REG_DRQ) {
            break;
        }
    }

    return 0;
}

void ata_init() {
    ata_dev = find_pci_dev(ATA_VEN_ID, ATA_DEV_ID);

    irq_install_handler(32 + ATA0_IRQ, ata_handler);

    ata_dev_detect();
    ata_dev_detect();
    ata_dev_detect();
    ata_dev_detect();
}
