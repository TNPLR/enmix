#include "timer8253.h"
#include "cpuio.h"
#include "interrupt.h"
#include "kio.h"

#define IRQ0_FREQUENCY 10000
#define INPUT_FREQUENCY 1193180
#define COUNTER0_VALUE INPUT_FREQUENCY / IRQ0_FREQUENCY
#define COUNTER0_PORT 0x40
#define COUNTER0_NO 0x0
#define COUNTER_MODE 2
#define READ_WRITE_LATCH 3
#define PIC_CONTROL_PORT 0x43

static void frequency_set(uint8_t counter_port,
    uint8_t counter_no, uint8_t rwl, uint8_t counter_mode,
    uint16_t counter_value)
{
  koutb(PIC_CONTROL_PORT, (uint8_t)(counter_no << 6 | rwl << 4 | counter_mode << 1));
  koutb(counter_port, (uint8_t)counter_value);
  koutb(counter_port, (uint8_t)(counter_value >> 8));
}

void timer8253_init(void)
{
  kputs("[INFO] TIMER 8253 INIT\n");
  frequency_set(COUNTER0_PORT, COUNTER0_NO, READ_WRITE_LATCH,
      COUNTER_MODE, COUNTER0_VALUE);
  kputs("[INFO] TIMER 8253 init done\n");
}
