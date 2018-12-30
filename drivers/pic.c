#include "pic.h"
#include "cpuio.h"
#include "kio.h"

#define PIC_MASTER_CTRL 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_CTRL 0xA0
#define PIC_SLAVE_DATA 0xA1

void pic_init(void)
{
  koutb(PIC_MASTER_CTRL, 0x11);
  koutb(PIC_MASTER_DATA, 0x20);

  koutb(PIC_MASTER_DATA, 0x04);
  koutb(PIC_MASTER_DATA, 0x01);

  koutb(PIC_SLAVE_CTRL, 0x11);
  koutb(PIC_SLAVE_DATA, 0x28);

  koutb(PIC_SLAVE_DATA, 0x02);
  koutb(PIC_SLAVE_DATA, 0x01);

  // testing keyborad now
  koutb(PIC_MASTER_DATA, 0xfd);
  koutb(PIC_SLAVE_DATA, 0xff);

  kputs("[INFO] PIC set\n");
}
