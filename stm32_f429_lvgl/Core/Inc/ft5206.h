#ifndef _ft5206_H
#define _ft5206_H

#include "stm32f4xx_hal.h"
#include "touch.h"

#define TOUCH_MAX_NUM 5

typedef struct TouchTypedef_TAG
{
	uint8_t touch_type;
	uint8_t dir;
	uint16_t pix_w;
	uint16_t pix_h;
	uint8_t touch_num;
	uint16_t x[TOUCH_MAX_NUM];
	uint16_t y[TOUCH_MAX_NUM];
}TouchTypedef;

void ft5206_init(void);
uint8_t ft5206_scan(TouchTypedef *touch);
#endif
