#ifndef _ATA_H
#define _ATA_H

#include <type.h>





#define ATA_VEN_ID  0x8086
#define ATA_DEV_ID  0x7010

#define ATA_STATUS_REG_BSY  0x80
#define ATA_STATUS_REG_DRDY 0x40
#define ATA_STATUS_REG_DF   0x20
#define ATA_STATUS_REG_DSC  0x10
#define ATA_STATUS_REG_DRQ  0x8
#define ATA_STATUS_REG_CORR 0x04
#define ATA_STATUS_REG_IDX  0x02
#define ATA_STATUS_REG_ERR  0x0


#define ATA_ERROR_BBK   0x80
#define ATA_ERROR_UNC   0x40
#define ATA_ERROR_MC    0x20
#define ATA_ERROR_IDNF  0x10
#define ATA_ERROR_MCR   0x08
#define ATA_ERROR_ABRT  0x04
#define ATA_ERROR_TK0NF 0x02
#define ATA_ERROR_AMNF  0x01

#define ATA_CMD_READ_PIO                0x20
#define ATA_CMD_READ_PIO_EXT            0x24
#define ATA_CMD_READ_PIO_DMA            0xC8
#define ATA_CMD_READ_PIO_DMA_EXT        0x25
#define ATA_CMD_WRITE_PIO               0x30
#define ATA_CMD_WRITE_PIO_EXT           0x34
#define ATA_CMD_WRITE_DMA               0xCA
#define ATA_CMD_WRITE_DMA_EXT           0x35
#define ATA_CMD_WRITE_CACHE_FLUSH       0xE7
#define ATA_CMD_WRITE_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET                  0xA0
#define ATA_CMD_IDENTIFY_PACKET         0xA1
#define ATA_CMD_IDENTIFY                0xEC 

#define ATA_IDENT_DEVICETYPE    0
#define ATA_IDENT_CYLINDERS     2
#define ATA_IDENT_HEADS         6
#define ATA_IDENT_SECTORS       12
#define ATA_IDENT_SERIAL        20
#define ATA_IDENT_MODEL         54
#define ATA_IDENT_CAPABILTIES   98
#define ATA_IDENT_FIELDVALID    106
#define ATA_IDENT_MAX_LBA       120
#define ATA_IDENT_COMMANDSETS   164
#define ATA_IDENT_MAX_LBA_EXT   200


#define IDE_ATA 0x00

#define ATA_MASTER  0x00
#define ATA_SLAVE  0x01


#define ATA_PRIMAY  0x00
#define ATA_SECONDARY  0x01

#define ATA_REG_DATA        0x00
#define ATA_REG_ERROR       0x01
#define ATA_REG_FEATURES    0x01
#define ATA_REG_SECCOUNT0   0x02
#define ATA_REG_LBA0        0x03
#define ATA_REG_LBA1        0x04
#define ATA_REG_LBA2        0x05
#define ATA_REG_HDDEVSEL    0x06
#define ATA_REG_COMMAND     0x07
#define ATA_REG_STATUS      0x07
#define ATA_REG_SECCOUNT1   0x08
#define ATA_REG_LBA3        0x09
#define ATA_REG_LBA4        0x0A
#define ATA_REG_LBA5        0x0B
#define ATA_REG_CONTROL     0x0C
#define ATA_REG_ALTSTATUS   0x0C
#define ATA_REG_DEVADDRESS  0x0D

#define ATA_STATUS_REG_SRV              0x10

#define ATA_STATUS_REG_RDY  0x40





#define ATA0_IOADDR 0x1F0   //0x1f7
#define ATA1_IOADDR 0x170   //~0x177

#define ATA0_DEVCTL 0x3F6
#define ATA1_DEVCTL 0x376

#define ATA0_IRQ    14
#define ATA1_IRQ    15



struct IDEChannelRegisters {
    unsigned short base;
    unsigned short ctrl;
    unsigned short bmide;
    unsigned char  nIEN;
}   channels[2];

struct ide_device {
    unsigned char Reserved;
    unsigned char Channel;
    unsigned char Drive;
    unsigned short Type;
    unsigned short Signature;
    unsigned short Capabilities;
    unsigned int    CommandSets;
    unsigned int     Size;
    unsigned char   Model[41];
} ide_devices[4];










#endif