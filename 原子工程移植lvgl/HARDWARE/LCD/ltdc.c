#include "ltdc.h"
#include "lcd.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK 阿波罗STM32F429开发板
//LTDC驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
LTDC_HandleTypeDef  LTDC_Handler;	    //LTDC句柄
DMA2D_HandleTypeDef DMA2D_Handler; 	    //DMA2D句柄

//根据不同的颜色格式,定义帧缓存数组
#if LCD_PIXFORMAT==LCD_PIXFORMAT_ARGB8888||LCD_PIXFORMAT==LCD_PIXFORMAT_RGB888
	u32 ltdc_lcd_framebuf[1280][800] __attribute__((at(LCD_FRAME_BUF_ADDR)));	//定义最大屏分辨率时,LCD所需的帧缓存数组大小
#else
	u16 ltdc_lcd_framebuf[1024][600] __attribute__((at(LCD_FRAME_BUF_ADDR)));	//定义最大屏分辨率时,LCD所需的帧缓存数组大小
#endif

u32 *ltdc_framebuf[2];					//LTDC LCD帧缓存数组指针,必须指向对应大小的内存区域
_ltdc_dev lcdltdc;						//管理LCD LTDC的重要参数

//打开LCD开关
//lcd_switch:1 打开,0，关闭
void LTDC_Switch(u8 sw)
{
	if(sw==1) __HAL_LTDC_ENABLE(&LTDC_Handler);
	else if(sw==0)__HAL_LTDC_DISABLE(&LTDC_Handler);
}

//开关指定层
//layerx:层号,0,第一层; 1,第二层
//sw:1 打开;0关闭
void LTDC_Layer_Switch(u8 layerx,u8 sw)
{
	if(sw==1) __HAL_LTDC_LAYER_ENABLE(&LTDC_Handler,layerx);
	else if(sw==0) __HAL_LTDC_LAYER_DISABLE(&LTDC_Handler,layerx);
	__HAL_LTDC_RELOAD_CONFIG(&LTDC_Handler);
}

//选择层
//layerx:层号;0,第一层;1,第二层;
void LTDC_Select_Layer(u8 layerx)
{
	lcdltdc.activelayer=layerx;
}

//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LTDC_Display_Dir(u8 dir)
{
    lcdltdc.dir=dir; 	//显示方向
	if(dir==0)			//竖屏
	{
		lcdltdc.width=lcdltdc.pheight;
		lcdltdc.height=lcdltdc.pwidth;	
	}else if(dir==1)	//横屏
	{
		lcdltdc.width=lcdltdc.pwidth;
		lcdltdc.height=lcdltdc.pheight;
	}
}

//画点函数
//x,y:写入坐标
//color:颜色值
void LTDC_Draw_Point(u16 x,u16 y,u32 color)
{ 

}

//读点函数
//x,y:读取点的坐标
//返回值:颜色值
u32 LTDC_Read_Point(u16 x,u16 y)
{ 

}

//LTDC填充矩形,DMA2D填充
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
//有时候需要频繁的调用填充函数，所以为了速度，填充函数采用寄存器版本，
//不过下面有对应的库函数版本的代码。
void LTDC_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color)
{ 
	u32 psx,psy,pex,pey;	//以LCD面板为基准的坐标系,不随横竖屏变化而变化
	u32 timeout=0; 
	u16 offline;
	u32 addr; 
	//坐标系转换
	if(lcdltdc.dir)	//横屏
	{
		psx=sx;psy=sy;
		pex=ex;pey=ey;
	}else			//竖屏
	{
		psx=sy;psy=lcdltdc.pheight-ex-1;
		pex=ey;pey=lcdltdc.pheight-sx-1;
	}
	offline=lcdltdc.pwidth-(pex-psx+1);
	addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*psy+psx));
	__HAL_RCC_DMA2D_CLK_ENABLE();	//使能DM2D时钟
	DMA2D->CR&=~(DMA2D_CR_START);	//先停止DMA2D
	DMA2D->CR=DMA2D_R2M;			//寄存器到存储器模式
	DMA2D->OPFCCR=LCD_PIXFORMAT;	//设置颜色格式
	DMA2D->OOR=offline;				//设置行偏移 

	DMA2D->OMAR=addr;				//输出存储器地址
	DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16);	//设定行数寄存器
	DMA2D->OCOLR=color;						//设定输出颜色寄存器 
	DMA2D->CR|=DMA2D_CR_START;				//启动DMA2D
	while((DMA2D->ISR&(DMA2D_FLAG_TC))==0)	//等待传输完成
	{
		timeout++;
		if(timeout>0X1FFFFF)break;	//超时退出
	} 
	DMA2D->IFCR|=DMA2D_FLAG_TC;		//清除传输完成标志 		
}
//在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
//void LTDC_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color)
//{
//	u32 psx,psy,pex,pey;	//以LCD面板为基准的坐标系,不随横竖屏变化而变化
//	u32 timeout=0; 
//	u16 offline;
//	u32 addr;  
//    if(ex>=lcdltdc.width)ex=lcdltdc.width-1;
//	if(ey>=lcdltdc.height)ey=lcdltdc.height-1;
//	//坐标系转换
//	if(lcdltdc.dir)	//横屏
//	{
//		psx=sx;psy=sy;
//		pex=ex;pey=ey;
//	}else			//竖屏
//	{
//		psx=sy;psy=lcdltdc.pheight-ex-1;
//		pex=ey;pey=lcdltdc.pheight-sx-1;
//	}
//	offline=lcdltdc.pwidth-(pex-psx+1);
//	addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*psy+psx));
//    if(LCD_PIXFORMAT==LCD_PIXEL_FORMAT_RGB565)  //如果是RGB565格式的话需要进行颜色转换，将16bit转换为32bit的
//    {
//        color=((color&0X0000F800)<<8)|((color&0X000007E0)<<5)|((color&0X0000001F)<<3);
//    }
//	//配置DMA2D的模式
//	DMA2D_Handler.Instance=DMA2D;
//	DMA2D_Handler.Init.Mode=DMA2D_R2M;			//内存到存储器模式
//	DMA2D_Handler.Init.ColorMode=LCD_PIXFORMAT;	//设置颜色格式
//	DMA2D_Handler.Init.OutputOffset=offline;		//输出偏移 
//	HAL_DMA2D_Init(&DMA2D_Handler);              //初始化DMA2D
//    HAL_DMA2D_ConfigLayer(&DMA2D_Handler,lcdltdc.activelayer); //层配置
//    HAL_DMA2D_Start(&DMA2D_Handler,color,(u32)addr,pex-psx+1,pey-psy+1);//开启传输
//    HAL_DMA2D_PollForTransfer(&DMA2D_Handler,1000);//传输数据
//    while((__HAL_DMA2D_GET_FLAG(&DMA2D_Handler,DMA2D_FLAG_TC)==0)&&(timeout<0X5000))//等待DMA2D完成
//    {
//        timeout++;
//    }
//    __HAL_DMA2D_CLEAR_FLAG(&DMA2D_Handler,DMA2D_FLAG_TC);       //清除传输完成标志
//}

//在指定区域内填充指定颜色块,DMA2D填充	
//此函数仅支持u16,RGB565格式的颜色数组填充.
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)  
//注意:sx,ex,不能大于lcddev.width-1;sy,ey,不能大于lcddev.height-1!!!
//color:要填充的颜色数组首地址
void LTDC_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
{
	u32 psx,psy,pex,pey;	//以LCD面板为基准的坐标系,不随横竖屏变化而变化
	u32 timeout=0; 
	u16 offline;
	u32 addr; 
	

	//坐标系转换
	if(lcdltdc.dir)	//横屏
	{
		psx=sx;psy=sy;
		pex=ex;pey=ey;
	}else			//竖屏
	{
		psx=sy;psy=lcdltdc.pheight-ex-1;
		pex=ey;pey=lcdltdc.pheight-sx-1;
	}
	offline=lcdltdc.pwidth-(pex-psx+1);
	addr=((u32)ltdc_framebuf[lcdltdc.activelayer]+lcdltdc.pixsize*(lcdltdc.pwidth*psy+psx));
	__HAL_RCC_DMA2D_CLK_ENABLE();	//使能DM2D时钟
	DMA2D->CR&=~(DMA2D_CR_START);	//先停止DMA2D
	DMA2D->CR=DMA2D_M2M;			//存储器到存储器模式
	DMA2D->FGPFCCR=LCD_PIXFORMAT;	//设置颜色格式
	DMA2D->FGOR=0;					//前景层行偏移为0
	DMA2D->OOR=offline;				//设置行偏移 

	DMA2D->FGMAR=(u32)color;		//源地址
	DMA2D->OMAR=addr;				//输出存储器地址
	DMA2D->NLR=(pey-psy+1)|((pex-psx+1)<<16);	//设定行数寄存器 
	DMA2D->CR|=DMA2D_CR_START;					//启动DMA2D
	
	
	while((DMA2D->ISR&(DMA2D_FLAG_TC))==0)		//等待传输完成
	{
		timeout++;
		if(timeout>0X1FFFFF)break;	//3?ê±í?3?
	} 
	DMA2D->IFCR|=DMA2D_FLAG_TC;				//清除传输完成标志  
	
	
}  

//LCD清屏
//color:颜色值
void LTDC_Clear(u32 color)
{
	LTDC_Fill(0,0,lcdltdc.width-1,lcdltdc.height-1,color);
}

//LTDC时钟(Fdclk)设置函数
//Fvco=Fin*pllsain; 
//Fdclk=Fvco/pllsair/2*2^pllsaidivr=Fin*pllsain/pllsair/2*2^pllsaidivr;

//Fvco:VCO频率
//Fin:输入时钟频率一般为1Mhz(来自系统时钟PLLM分频后的时钟,见时钟树图)
//pllsain:SAI时钟倍频系数N,取值范围:50~432.  
//pllsair:SAI时钟的分频系数R,取值范围:2~7
//pllsaidivr:LCD时钟分频系数,取值范围:RCC_PLLSAIDIVR_2/4/8/16,对应分频2~16 
//假设:外部晶振为25M,pllm=25的时候,Fin=1Mhz.
//例如:要得到20M的LTDC时钟,则可以设置:pllsain=400,pllsair=5,pllsaidivr=RCC_PLLSAIDIVR_4
//Fdclk=1*400/5/4=400/20=20Mhz
//返回值:0,成功;1,失败。
u8 LTDC_Clk_Set(u32 pllsain,u32 pllsair,u32 pllsaidivr)
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



u16 LTDC_PanelID_Read(void)
{
	u8 idx=0;
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOG_CLK_ENABLE();       //使能GPIOG时钟
	__HAL_RCC_GPIOI_CLK_ENABLE();       //使能GPIOI时钟
    
    GPIO_Initure.Pin=GPIO_PIN_6;        //PG6
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  //输入
    GPIO_Initure.Pull=GPIO_PULLUP;      //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH; //高速
    HAL_GPIO_Init(GPIOG,&GPIO_Initure); //初始化
    
    GPIO_Initure.Pin=GPIO_PIN_2|GPIO_PIN_7; //PI2,7
    HAL_GPIO_Init(GPIOI,&GPIO_Initure);     //初始化
    
    idx=(u8)HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_6); //读取M0
    idx|=(u8)HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_2)<<1;//读取M1
    idx|=(u8)HAL_GPIO_ReadPin(GPIOI,GPIO_PIN_7)<<2;//读取M2
	if(idx==0)return 0X4342;	    //4.3寸屏,480*272分辨率
	else if(idx==1)return 0X7084;	//7寸屏,800*480分辨率
	else if(idx==2)return 0X7016;	//7寸屏,1024*600分辨率
	else if(idx==3)return 0X7018;	//7寸屏,1280*800分辨率
	else if(idx==4)return 0X8017; 	//8寸屏,1024*768分辨率
	else return 0;
}

//LCD初始化函数
void LTDC_Init(void)
{   
	u16 lcdid=0;
	

		lcdltdc.pwidth=1024;			//面板宽度,单位:像素
		lcdltdc.pheight=600;			//面板高度,单位:像素
    lcdltdc.hsw=20;				    //水平同步宽度
		lcdltdc.vsw=3;				    //垂直同步宽度
		lcdltdc.hbp=140;			    //水平后廊
		lcdltdc.vbp=20;				    //垂直后廊
		lcdltdc.hfp=160;			    //水平前廊
		lcdltdc.vfp=12;				    //垂直前廊


		lcddev.width=lcdltdc.pwidth;
		lcddev.height=lcdltdc.pheight;
    
    lcdltdc.pixsize=2;				//每个像素占2个字节
	ltdc_framebuf[0]=(u32*)&ltdc_lcd_framebuf;

		
		LTDC_Clk_Set(100,2,RCC_PLLSAIDIVR_2);//设置像素时钟  45Mhz 
    //LTDC配置
//    LTDC_Handler.Instance=LTDC;
//    LTDC_Handler.Init.HSPolarity=LTDC_HSPOLARITY_AL;         //水平同步极性
//    LTDC_Handler.Init.VSPolarity=LTDC_VSPOLARITY_AL;         //垂直同步极性
//    LTDC_Handler.Init.DEPolarity=LTDC_DEPOLARITY_AL;         //数据使能极性
//    LTDC_Handler.Init.PCPolarity=LTDC_PCPOLARITY_IPC;        //像素时钟极性
//    LTDC_Handler.Init.HorizontalSync=lcdltdc.hsw-1;          //水平同步宽度
//    LTDC_Handler.Init.VerticalSync=lcdltdc.vsw-1;            //垂直同步宽度
//    LTDC_Handler.Init.AccumulatedHBP=lcdltdc.hsw+lcdltdc.hbp-1; //水平同步后沿宽度
//    LTDC_Handler.Init.AccumulatedVBP=lcdltdc.vsw+lcdltdc.vbp-1; //垂直同步后沿高度
//    LTDC_Handler.Init.AccumulatedActiveW=lcdltdc.hsw+lcdltdc.hbp+lcdltdc.pwidth-1;//有效宽度
//    LTDC_Handler.Init.AccumulatedActiveH=lcdltdc.vsw+lcdltdc.vbp+lcdltdc.pheight-1;//有效高度
//    LTDC_Handler.Init.TotalWidth=lcdltdc.hsw+lcdltdc.hbp+lcdltdc.pwidth+lcdltdc.hfp-1;   //总宽度
//    LTDC_Handler.Init.TotalHeigh=lcdltdc.vsw+lcdltdc.vbp+lcdltdc.pheight+lcdltdc.vfp-1;  //总高度
//    LTDC_Handler.Init.Backcolor.Red=0;           //屏幕背景层红色部分
//    LTDC_Handler.Init.Backcolor.Green=0;         //屏幕背景层绿色部分
//    LTDC_Handler.Init.Backcolor.Blue=0;          //屏幕背景色蓝色部分
//    HAL_LTDC_Init(&LTDC_Handler);
		
		
	#define PIXELS_W 1024
	#define PIXELS_H 600
 	LTDC_HandleTypeDef hltdc;
	hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 19;
  hltdc.Init.VerticalSync = 2;
  hltdc.Init.AccumulatedHBP = 159;
  hltdc.Init.AccumulatedVBP = 22;
  hltdc.Init.AccumulatedActiveW = 159+PIXELS_W;
  hltdc.Init.AccumulatedActiveH = 622;
  hltdc.Init.TotalWidth = 159+PIXELS_W+160;
  hltdc.Init.TotalHeigh = 634;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
 
  }
	
	


	LTDC_LayerCfgTypeDef pLayerCfg = {0};
		pLayerCfg.WindowX0 = 0;
		pLayerCfg.WindowX1 = PIXELS_W;
		pLayerCfg.WindowY0 = 0;
		pLayerCfg.WindowY1 = PIXELS_H;
		pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
		pLayerCfg.Alpha = 255;
		pLayerCfg.Alpha0 = 0;
		pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
		pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
		pLayerCfg.FBStartAdress = 0XC0000000;
		pLayerCfg.ImageWidth = PIXELS_W;
		pLayerCfg.ImageHeight = PIXELS_H;
		pLayerCfg.Backcolor.Blue = 0;
		pLayerCfg.Backcolor.Green = 0;
		pLayerCfg.Backcolor.Red = 0;
		if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
		{

		}
	
	
 	 LTDC_Display_Dir(0);			//默认竖屏
	 LTDC_Select_Layer(0); 			//选择第1层
    LCD_LED=1;         		        //点亮背光
    LTDC_Clear(0XFFFFFFFF);			//清屏
}

//LTDC底层IO初始化和时钟使能
//此函数会被HAL_LTDC_Init()调用
//hltdc:LTDC句柄
void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_LTDC_CLK_ENABLE();                //使能LTDC时钟
    __HAL_RCC_DMA2D_CLK_ENABLE();               //使能DMA2D时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();               //使能GPIOB时钟
    __HAL_RCC_GPIOF_CLK_ENABLE();               //使能GPIOF时钟
    __HAL_RCC_GPIOG_CLK_ENABLE();               //使能GPIOG时钟
    __HAL_RCC_GPIOH_CLK_ENABLE();               //使能GPIOH时钟
    __HAL_RCC_GPIOI_CLK_ENABLE();               //使能GPIOI时钟
    
    //初始化PB5，背光引脚
    GPIO_Initure.Pin=GPIO_PIN_5;                //PB5推挽输出，控制背光
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;      //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;              //上拉        
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
    //初始化PF10
    GPIO_Initure.Pin=GPIO_PIN_10; 
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //复用
    GPIO_Initure.Pull=GPIO_NOPULL;              
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //高速
    GPIO_Initure.Alternate=GPIO_AF14_LTDC;      //复用为LTDC
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
    
    //初始化PG6,7,11
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_11;
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);
    
    //初始化PH9,10,11,12,13,14,15
    GPIO_Initure.Pin=GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|\
                     GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
    
    //初始化PI0,1,2,4,5,6,7,9,10
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|\
                     GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_10;
    HAL_GPIO_Init(GPIOI,&GPIO_Initure); 
}

