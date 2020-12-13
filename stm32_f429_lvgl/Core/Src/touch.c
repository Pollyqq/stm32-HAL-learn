#include "touch.h"
#include "ft5206.h"


uint16_t g_x =0;
uint16_t g_y=0;
uint16_t g_touchstate= 0;
TouchTypedef mtouch;

void touch_init(void)
{
	mtouch.dir = 1;
	mtouch.pix_h = PIXELS_H;
	mtouch.pix_w = PIXELS_W;
	
	ft5206_init();
}

uint8_t touch_scan(void)
{
	if(ft5206_scan(&mtouch)){
			g_x = mtouch.x[0];
			g_y = mtouch.y[0];
			g_touchstate = 1;
		}else{
			 g_touchstate = 0;
		}
}

uint8_t touch_get(uint16_t *x,uint16_t *y)
{
	*x = g_x;
	*y = g_y;
	return g_touchstate;
}
