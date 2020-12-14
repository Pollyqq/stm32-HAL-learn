#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sdram.h"
#include "touch.h"
/************************************************
 ALIENTEK 阿波罗STM32F429开发板实验30
 触摸屏实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com  
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
#include "timer.h"
#include "lvgl.h"


int main(void)
{
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
    delay_init(180);                //初始化延时函数
    uart_init(115200);              //初始化USART
    LED_Init();                     //初始化LED 
    KEY_Init();                     //初始化按键
    SDRAM_Init();                   //初始化SDRAM
	
		TIM3_Init(999,89);							//定时器初始化(1ms中断),用于给lvgl提供心跳节拍
	
    LCD_Init();                     //初始化LCD
		LCD_Display_Dir(1);						//如果是RGB屏的话,则强制设置为横屏显示
	
	  tp_dev.init();				    //触摸屏初始化 

	lv_init();											//lvgl系统初始化
	lv_port_disp_init();						//lvgl显示接口初始化,放在lv_init()的后面
	lv_port_indev_init();						//lvgl输入接口初始化,放在lv_init()的后面
	lv_demo_widgets();

	//lv_demo_benchmark();
		while(1)
		{
			tp_dev.scan(0);//触摸扫描
			lv_task_handler();//lvgl的事务处理
			 delay_ms(2);
		}
}
