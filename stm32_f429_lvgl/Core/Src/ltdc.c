/**
  ******************************************************************************
  * File Name          : LTDC.c
  * Description        : This file provides code for the configuration
  *                      of the LTDC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ltdc.h"

/* USER CODE BEGIN 0 */
#include "dma2d.h"

//定义最大屏分辨率时,LCD所需的帧缓存数组大小
uint16_t g_ltdc_framebuf[PIXELS_W][PIXELS_H] __attribute__((at(LCD_FRAME_BUF_ADDR)));
void LTDC_Layer_Parameter_Config(uint8_t layerx,uint32_t bufaddr,uint8_t pixformat,uint8_t alpha,uint8_t alpha0,uint8_t bfac1,uint8_t bfac2,uint32_t bkcolor);

/* USER CODE END 0 */

LTDC_HandleTypeDef hltdc;

/* LTDC init function */
void MX_LTDC_Init(void)
{
  LTDC_LayerCfgTypeDef pLayerCfg = {0};
  LTDC_LayerCfgTypeDef pLayerCfg1 = {0};

  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 20-1;
  hltdc.Init.VerticalSync = 2;
  hltdc.Init.AccumulatedHBP = 20-1+140;
  hltdc.Init.AccumulatedVBP = 22;
  hltdc.Init.AccumulatedActiveW = 20-1+140+PIXELS_W;
  hltdc.Init.AccumulatedActiveH = 622;
  hltdc.Init.TotalWidth = 20-1+140+PIXELS_W+160;
  hltdc.Init.TotalHeigh = 634;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = PIXELS_W;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = PIXELS_H;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = LTDC_BUFF_ADDR;
  pLayerCfg.ImageWidth = 0;
  pLayerCfg.ImageHeight = 0;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg1.WindowX0 = 0;
  pLayerCfg1.WindowX1 = 1024;
  pLayerCfg1.WindowY0 = 0;
  pLayerCfg1.WindowY1 = 600;
  pLayerCfg1.Alpha = 0;
  pLayerCfg1.Alpha0 = 255;
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg1.FBStartAdress = 0;
  pLayerCfg1.ImageWidth = 0;
  pLayerCfg1.ImageHeight = 0;
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_LTDC_MspInit(LTDC_HandleTypeDef* ltdcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(ltdcHandle->Instance==LTDC)
  {
  /* USER CODE BEGIN LTDC_MspInit 0 */

  /* USER CODE END LTDC_MspInit 0 */
    /* LTDC clock enable */
    __HAL_RCC_LTDC_CLK_ENABLE();

    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    /**LTDC GPIO Configuration
    PI9     ------> LTDC_VSYNC
    PI10     ------> LTDC_HSYNC
    PF10     ------> LTDC_DE
    PH9     ------> LTDC_R3
    PH10     ------> LTDC_R4
    PH11     ------> LTDC_R5
    PH12     ------> LTDC_R6
    PG6     ------> LTDC_R7
    PG7     ------> LTDC_CLK
    PH13     ------> LTDC_G2
    PH14     ------> LTDC_G3
    PH15     ------> LTDC_G4
    PI0     ------> LTDC_G5
    PI1     ------> LTDC_G6
    PI2     ------> LTDC_G7
    PG11     ------> LTDC_B3
    PI4     ------> LTDC_B4
    PI5     ------> LTDC_B5
    PI6     ------> LTDC_B6
    PI7     ------> LTDC_B7
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_0|GPIO_PIN_1
                          |GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN LTDC_MspInit 1 */



  /* USER CODE END LTDC_MspInit 1 */
  }
}

void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef* ltdcHandle)
{

  if(ltdcHandle->Instance==LTDC)
  {
  /* USER CODE BEGIN LTDC_MspDeInit 0 */

  /* USER CODE END LTDC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LTDC_CLK_DISABLE();

    /**LTDC GPIO Configuration
    PI9     ------> LTDC_VSYNC
    PI10     ------> LTDC_HSYNC
    PF10     ------> LTDC_DE
    PH9     ------> LTDC_R3
    PH10     ------> LTDC_R4
    PH11     ------> LTDC_R5
    PH12     ------> LTDC_R6
    PG6     ------> LTDC_R7
    PG7     ------> LTDC_CLK
    PH13     ------> LTDC_G2
    PH14     ------> LTDC_G3
    PH15     ------> LTDC_G4
    PI0     ------> LTDC_G5
    PI1     ------> LTDC_G6
    PI2     ------> LTDC_G7
    PG11     ------> LTDC_B3
    PI4     ------> LTDC_B4
    PI5     ------> LTDC_B5
    PI6     ------> LTDC_B6
    PI7     ------> LTDC_B7
    */
    HAL_GPIO_DeInit(GPIOI, GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_0|GPIO_PIN_1
                          |GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7);

    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_10);

    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_11);

  /* USER CODE BEGIN LTDC_MspDeInit 1 */

  /* USER CODE END LTDC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

// 电源打开
void LTDC_ON(void)
{
    HAL_GPIO_WritePin(LCD_BK_GPIO_Port,LCD_BK_Pin,1);
}
// 电源关闭
void LTDC_OFF(void)
{
    HAL_GPIO_WritePin(LCD_BK_GPIO_Port,LCD_BK_Pin,0);
}

uint8_t LTDC_Clk_Set(uint32_t pllsain,uint32_t pllsair,uint32_t pllsaidivr)
{
	RCC_PeriphCLKInitTypeDef PeriphClkIniture;
	
	//LTDC输出像素时钟，需要根据自己所使用的LCD数据手册来配置！
    PeriphClkIniture.PeriphClockSelection=RCC_PERIPHCLK_LTDC;	//LTDC时钟 	
	PeriphClkIniture.PLLSAI.PLLSAIN=pllsain;    
	PeriphClkIniture.PLLSAI.PLLSAIR=pllsair;  
	PeriphClkIniture.PLLSAIDivR=pllsaidivr;
	if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkIniture)==HAL_OK)    //配置像素时钟
    {
        return 0;   //成功
    }
    else return 1;  //失败    
}


// 初始化
void LTDC_Init(void)
{
		
	  __HAL_RCC_DMA2D_CLK_ENABLE();               //使能DMA2D时钟
	
		LTDC_Layer_Parameter_Config(0,0XC0000000,0X02,255,0,6,7,0X000000);//层参数配置
    HAL_LTDC_SetWindowPosition(&hltdc,0,0,0);  //设置窗口的位置
    HAL_LTDC_SetWindowSize(&hltdc,PIXELS_W,PIXELS_H,0);//设置窗口大小
    LTDC_ON();
    LTDC_Clear(0XFFFFFFFF);         //清屏
    return;
}

// 清屏
void LTDC_Clear(uint32_t color)
{
	if(PIXELS_DIR){
		 LTDC_Fill(0,0,PIXELS_W-1,PIXELS_H-1,color);
	}else{
		 LTDC_Fill(0,0,PIXELS_H-1,PIXELS_W-1,color);
	}
   
}

//画点函数
void LTDC_Draw_Point(uint16_t x,uint16_t y,uint32_t color)
{
    //坐标系转换
    if(PIXELS_DIR)	//横屏
    {
        *(uint16_t*)((uint32_t)LCD_FRAME_BUF_ADDR+PIXELS_BYTE*(PIXELS_W*y+x))=color;
    } else			//竖屏
    {
        *(uint16_t*)((uint32_t)LCD_FRAME_BUF_ADDR+PIXELS_BYTE*(PIXELS_W*(PIXELS_H-x-1)+y))=color;
    }
}
//读点函数
uint32_t LTDC_Read_Point(uint16_t x,uint16_t y)
{
    //坐标系转换
    if(PIXELS_DIR)	//横屏
    {
        return *(uint16_t*)((uint32_t)LCD_FRAME_BUF_ADDR+PIXELS_BYTE*(PIXELS_W*y+x));
    } else			//竖屏
    {
        return *(uint16_t*)((uint32_t)LCD_FRAME_BUF_ADDR+PIXELS_BYTE*(PIXELS_W*(PIXELS_H-x-1)+y));
    }
}

//LTDC填充矩形,DMA2D填充
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
//color:要填充的颜色
//有时候需要频繁的调用填充函数，所以为了速度，填充函数采用寄存器版本，
//不过下面有对应的库函数版本的代码。
void LTDC_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color)
{
    uint32_t psx,psy,pex,pey;	//以LCD面板为基准的坐标系,不随横竖屏变化而变化
    uint32_t timeout=0;
    uint16_t offline;
    uint32_t addr;

    //坐标系转换
    if(PIXELS_DIR)	//横屏
    {
        psx=sx;
        psy=sy;
        pex=ex;
        pey=ey;
    } else			//竖屏
    {
        psx=sy;
        psy=PIXELS_H-ex-1;
        pex=ey;
        pey=PIXELS_H-sx-1;
    }
    offline=PIXELS_W-(pex-psx+1);
    addr=(LCD_FRAME_BUF_ADDR+PIXELS_BYTE*(PIXELS_W*psy+psx));

    __HAL_RCC_DMA2D_CLK_ENABLE();	//使能DM2D时钟
    DMA2D->CR&=~(DMA2D_CR_START);	//先停止DMA2D
    DMA2D->CR=DMA2D_R2M;			//寄存器到存储器模式
    DMA2D->OPFCCR=LTDC_PIXEL_FORMAT_RGB565;	//设置颜色格式
    DMA2D->OOR=offline;				//设置行偏移

    DMA2D->OMAR=addr;				//输出存储器地址
    DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16);	//设定行数寄存器
    DMA2D->OCOLR=color;						//设定输出颜色寄存器
    DMA2D->CR|=DMA2D_CR_START;				//启动DMA2D
    while((DMA2D->ISR&(DMA2D_FLAG_TC))==0)	//等待传输完成
    {

    }
    DMA2D->IFCR|=DMA2D_FLAG_TC;		//清除传输完成标志
}

//LTDC,基本参数设置.
//注意:此函数,必须在LTDC_Layer_Window_Config之前设置.
//layerx:层值,0/1.
//bufaddr:层颜色帧缓存起始地址
//pixformat:颜色格式.0,ARGB8888;1,RGB888;2,RGB565;3,ARGB1555;4,ARGB4444;5,L8;6;AL44;7;AL88
//alpha:层颜色Alpha值,0,全透明;255,不透明
//alpha0:默认颜色Alpha值,0,全透明;255,不透明
//bfac1:混合系数1,4(100),恒定的Alpha;6(101),像素Alpha*恒定Alpha
//bfac2:混合系数2,5(101),恒定的Alpha;7(111),像素Alpha*恒定Alpha
//bkcolor:层默认颜色,32位,低24位有效,RGB888格式
//返回值:无
void LTDC_Layer_Parameter_Config(uint8_t layerx,uint32_t bufaddr,uint8_t pixformat,uint8_t alpha,uint8_t alpha0,uint8_t bfac1,uint8_t bfac2,uint32_t bkcolor)
{
	LTDC_LayerCfgTypeDef pLayerCfg;
	
	pLayerCfg.WindowX0=0;                       //窗口起始X坐标
	pLayerCfg.WindowY0=0;                       //窗口起始Y坐标
	pLayerCfg.WindowX1=PIXELS_W ;          //窗口终止X坐标
	pLayerCfg.WindowY1=PIXELS_H;         //窗口终止Y坐标
	pLayerCfg.PixelFormat=pixformat;		    //像素格式
	pLayerCfg.Alpha=alpha;				        //Alpha值设置，0~255,255为完全不透明
	pLayerCfg.Alpha0=alpha0;			        //默认Alpha值
	pLayerCfg.BlendingFactor1=(uint32_t)bfac1<<8;    //设置层混合系数
	pLayerCfg.BlendingFactor2=(uint32_t)bfac2<<8;	//设置层混合系数
	pLayerCfg.FBStartAdress=bufaddr;	        //设置层颜色帧缓存起始地址
	pLayerCfg.ImageWidth=PIXELS_W;        //设置颜色帧缓冲区的宽度    
	pLayerCfg.ImageHeight=PIXELS_H;      //设置颜色帧缓冲区的高度
	pLayerCfg.Backcolor.Red=(uint8_t)(bkcolor&0X00FF0000)>>16;   //背景颜色红色部分
	pLayerCfg.Backcolor.Green=(uint8_t)(bkcolor&0X0000FF00)>>8;  //背景颜色绿色部分
	pLayerCfg.Backcolor.Blue=(uint8_t)bkcolor&0X000000FF;        //背景颜色蓝色部分
	HAL_LTDC_ConfigLayer(&hltdc,&pLayerCfg,layerx);   //设置所选中的层
}  

//矩形彩色填充函数
void LTDC_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{
		uint32_t psx,psy,pex,pey;	//以LCD面板为基准的坐标系,不随横竖屏变化而变化
    uint32_t timeout=0;
    uint16_t offline;
    uint32_t addr;

    //坐标系转换
    if(PIXELS_DIR)	//横屏
    {
        psx=sx;
        psy=sy;
        pex=ex;
        pey=ey;
    } else			//竖屏
    {
        psx=sy;
        psy=PIXELS_H-ex-1;
        pex=ey;
        pey=PIXELS_H-sx-1;
    }
    offline=PIXELS_W-(pex-psx+1);
    addr=(LCD_FRAME_BUF_ADDR+PIXELS_BYTE*(PIXELS_W*psy+psx));

	__HAL_RCC_DMA2D_CLK_ENABLE();	//使能DM2D时钟
	DMA2D->CR&=~(DMA2D_CR_START);	//先停止DMA2D
	DMA2D->CR=DMA2D_M2M;			//存储器到存储器模式
	DMA2D->FGPFCCR=0X02;	//设置颜色格式
	DMA2D->FGOR=0;					//前景层行偏移为0
	DMA2D->OOR=offline;				//设置行偏移 

	DMA2D->FGMAR=(uint32_t)color;		//源地址
	DMA2D->OMAR=addr;				//输出存储器地址
	DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16);	//设定行数寄存器 
	DMA2D->CR|=DMA2D_CR_START;					//启动DMA2D
	while((DMA2D->ISR&(DMA2D_FLAG_TC))==0)		//等待传输完成
	{

	} 
	DMA2D->IFCR|=DMA2D_FLAG_TC;				//清除传输完成标志  	
}



/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
