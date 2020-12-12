#ifndef __DHT11_H
#define __DHT11_H
#include "stm32f4xx.h"
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef enum
{
  DHT11_OK = 0x00U,
  DHT11_ERROR = 0x01U,
} DHT11_Status;

typedef struct dht11_driver
{
  float temperature_float;
  float humidity_float;
  uint8_t temperature_int;
  uint8_t humidity_int;
} DHT11_driver;

//IO设置
#define DHT11_PORT GPIOG
#define DHT11_PIN GPIO_PIN_9

//IO操作宏定义
#define DHT11_IO_IN()                \
  {                                  \
    GPIOG->MODER &= ~(3 << (9 * 2)); \
    GPIOG->MODER |= 0 << 9 * 2;      \
  } //PG9输入模式
#define DHT11_IO_OUT()               \
  {                                  \
    GPIOG->MODER &= ~(3 << (9 * 2)); \
    GPIOG->MODER |= 1 << 9 * 2;      \
  }                                                                        //PG9输出模式
#define DHT11_DQ_OUT_SET (DHT11_PORT->BSRR = DHT11_PIN)                    //数据端口	PG9
#define DHT11_DQ_OUT_RESET (DHT11_PORT->BSRR = (uint32_t)DHT11_PIN << 16U) //数据端口	PG9
#define DHT11_DQ_READ_PIN HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)          //(DHT11_PORT->IDR & DHT11_PIN)  //数据端口	PG9

u8 DHT11_Init(void);                    //初始化DHT11
u8 DHT11_Read_Data(u8 *temp, u8 *humi); //读取温湿度
u8 DHT11_Read_Byte(void);               //读出一个字节
u8 DHT11_Read_Bit(void);                //读出一个位
u8 DHT11_Check(void);                   //检测是否存在DHT11
void DHT11_Rst(void);                   //复位DHT11
u8 DHT11_Read_Data_Float(float *Temp, float *Humi);

#endif
