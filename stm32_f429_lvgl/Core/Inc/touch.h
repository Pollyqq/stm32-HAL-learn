#ifndef _touch_H
#define _touch_H

#include "sdio.h"



void touch_init(void);
uint8_t touch_scan(void);
uint8_t touch_get(uint16_t *x,uint16_t *y);


#endif
