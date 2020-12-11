#ifndef _ft5206_H
#define _ft5206_H

#include "main.h"
#include "touch.h"

void ft5206_init(void);
uint8_t ft5206_scan(TouchTypedef *touch);
#endif
