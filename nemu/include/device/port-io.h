#ifndef __PORT_IO_H__
#define __PORT_IO_H__

#include "common.h"

typedef void(*pio_callback_t)(ioaddr_t, int, bool);

void* add_pio_map(ioaddr_t, int, pio_callback_t);

uint32_t pio_read_l(ioaddr_t addr);
uint32_t pio_read_w(ioaddr_t addr);
uint32_t pio_read_b(ioaddr_t addr);
void pio_write_l(ioaddr_t addr, uint32_t data);
void pio_write_w(ioaddr_t addr, uint32_t data);
void pio_write_b(ioaddr_t addr, uint32_t data);

#endif
