#include "dht11_driver.h"
#include "tim.h"
#include "gpio.h"
DHT11_driver DHT11_Data;
//澶嶄綅DHT11
void DHT11_Rst(void)
{

	DHT11_IO_OUT(); //SET OUTPUT
	DHT11_DQ_OUT_SET;
	//DHT11_DQ_OUT=0; 	//鎷変綆DQ
	DHT11_DQ_OUT_RESET;
	HAL_Delay(20); //鎷変綆鑷冲皯18ms
	//DHT11_DQ_OUT=1; 	//DQ=1
	DHT11_DQ_OUT_SET;
	delay_us(30); //涓绘満鎷夐珮20~40us
}
//绛夊緟DHT11鐨勫洖搴?
//杩斿洖1:鏈娴嬪埌DHT11鐨勫瓨鍦?
//杩斿洖0:瀛樺湪
u8 DHT11_Check(void)
{
	u16 retry = 0;
	DHT11_IO_IN();							//SET INPUT
	while (DHT11_DQ_READ_PIN && retry < 10) //DHT11浼氭媺浣?40~80us
	{
		retry++;
		delay_us(10);
	};
	if (retry >= 10)
		return 1;
	else
		retry = 0;
	while (!DHT11_DQ_READ_PIN && retry < 10) //DHT11鎷変綆鍚庝細鍐嶆鎷夐珮40~80us
	{
		retry++;
		delay_us(10);
	};
	if (retry >= 10)
		return 1;
	return 0;
}
//浠嶥HT11璇诲彇涓€涓綅
//杩斿洖鍊硷細1/0
u8 DHT11_Read_Bit(void)
{
	u8 retry = 0;
	while (DHT11_DQ_READ_PIN && retry < 10) //绛夊緟鍙樹负浣庣數骞?
	{
		retry++;
		delay_us(10);
	}
	retry = 0;

	while (!DHT11_DQ_READ_PIN && retry < 10) //绛夊緟鍙橀珮鐢靛钩
	{
		retry++;
		delay_us(10);
	}
	delay_us(30); //绛夊緟40us
	if (DHT11_DQ_READ_PIN)
		return 1;
	else
		return 0;
}
//浠嶥HT11璇诲彇涓€涓瓧鑺?
//杩斿洖鍊硷細璇诲埌鐨勬暟鎹?
u8 DHT11_Read_Byte(void)
{
	u8 i, dat;
	dat = 0;
	for (i = 0; i < 8; i++)
	{
		dat <<= 1;
		dat |= DHT11_Read_Bit();
	}
	return dat;
}
//浠嶥HT11璇诲彇涓€娆℃暟鎹?
//temp:娓╁害鍊?(鑼冨洿:0~50掳)
//humi:婀垮害鍊?(鑼冨洿:20%~90%)
//杩斿洖鍊硷細0,姝ｅ父;1,璇诲彇澶辫触
u8 DHT11_Read_Data(u8 *temp, u8 *humi)
{
	u8 buf[5] = {0};
	u8 i;

	DHT11_Rst();
	if (DHT11_Check() == 0)
	{
		for (i = 0; i < 5; i++) //璇诲彇40浣嶆暟鎹?
		{
			buf[i] = DHT11_Read_Byte();
		}

		if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
		{
			if (sizeof(humi) > 1)
			{
				humi[0] = buf[0];
				humi[1] = buf[1];
			}

			else
			{
				*humi = buf[0];
			}
			if (sizeof(temp) > 1)
			{
				temp[0] = buf[2];
				temp[1] = buf[3];
			}
			else
			{
				*temp = buf[2];
			}
			return DHT11_OK;
		}
		else
			return DHT11_ERROR;
	}
	else
		return DHT11_ERROR;
}
u8 DHT11_Read_Data_Float(float *Temp, float *Humi)
{

	uint8_t tem[2], hum[2];
	if (DHT11_Read_Data(tem, hum) != DHT11_OK)
	{
		return DHT11_ERROR;
	}
	*Temp = tem[0] + tem[1] / 10.00;
	*Humi = hum[0] + hum[1] / 10.00;
	return DHT11_OK;
}
//鍒濆鍖朌HT11鐨処O鍙? DQ 鍚屾椂妫€娴婦HT11鐨勫瓨鍦?
//杩斿洖1:涓嶅瓨鍦?
//杩斿洖0:瀛樺湪
u8 DHT11_Init(void)
{
	u8 ret = DHT11_ERROR;
	DHT11_Rst(); //澶嶄綅DHT11
	ret = DHT11_Check();
	printf("dht11 init %s. \r\n", ret == 0 ? "ok" : "failed");
	return ret;
}
