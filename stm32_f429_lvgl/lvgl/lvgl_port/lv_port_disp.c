/**
 * @file lv_port_disp_templ.c
 *
 */

 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"

#include "ltdc.h"
#include "string.h"
#include "dma2d.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

//变量定义
#define LTDC_LCD_FRAMEBUF_SIZE		(1280*800*2)		//ltdc.c中ltdc_lcd_framebuf缓冲区的大小
#define COLOR_BUF_SIZE		(LV_HOR_RES_MAX*LV_VER_RES_MAX)	//全屏的大小
static lv_color_t color_buf[COLOR_BUF_SIZE]	__attribute__((at(LCD_FRAME_BUF_ADDR+LTDC_LCD_FRAMEBUF_SIZE))); //分配到外部SDRAM,需要跳过ltdc.c中分配的帧缓冲区
static lv_color_t color_buf2[COLOR_BUF_SIZE]	__attribute__((at(LCD_FRAME_BUF_ADDR+LTDC_LCD_FRAMEBUF_SIZE+COLOR_BUF_SIZE*2)));//lvgl的第二个缓冲区,紧跟在第一个缓冲区的后面

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

lv_disp_drv_t  g_disp_drv;

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
#if LV_USE_GPU
static void gpu_blend(lv_disp_drv_t * disp_drv, lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa);
static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
        const lv_area_t * fill_area, lv_color_t color);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /* LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed your display drivers `flush_cb` to copy its content to your dispay.
     * The buffer has to be greater than 1 display row
     *
     * There are three buffering configurations:
     * 1. Create ONE buffer with some rows: 
     *      LVGL will draw the display's content here and writes it to your display
     * 
     * 2. Create TWO buffer with some rows: 
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     * 
     * 3. Create TWO screen-sized buffer: 
     *      Similar to 2) but the buffer have to be screen sized. When LVGL is ready it will give the
     *      whole frame to display. This way you only need to change the frame buffer's address instead of
     *      copying the pixels.
     * */

    /* Example for 1) */
			static lv_disp_buf_t draw_buf_dsc_1;
			printf("init \r\n");

			lv_disp_buf_init(&draw_buf_dsc_1, color_buf,color_buf2, 1024*600);   /*Initialize the display buffer*/
//    /* Example for 2) */
//    static lv_disp_buf_t draw_buf_dsc_2;
//    static lv_color_t draw_buf_2_1[LV_HOR_RES_MAX * 10];                        /*A buffer for 10 rows*/
//    static lv_color_t draw_buf_2_1[LV_HOR_RES_MAX * 10];                        /*An other buffer for 10 rows*/
//    lv_disp_buf_init(&draw_buf_dsc_2, draw_buf_2_1, draw_buf_2_1, LV_HOR_RES_MAX * 10);   /*Initialize the display buffer*/

//    /* Example for 3) */
//    static lv_disp_buf_t draw_buf_dsc_3;
//    static lv_color_t draw_buf_3_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];            /*A screen sized buffer*/
//    static lv_color_t draw_buf_3_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];            /*An other screen sized buffer*/
//    lv_disp_buf_init(&draw_buf_dsc_3, draw_buf_3_1, draw_buf_3_2, LV_HOR_RES_MAX * LV_VER_RES_MAX);   /*Initialize the display buffer*/


    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.buffer = &draw_buf_dsc_1;

#if LV_USE_GPU
    /*Optionally add functions to access the GPU. (Only in buffered mode, LV_VDB_SIZE != 0)*/

    /*Blend two color array using opacity*/
    disp_drv.gpu_blend_cb = gpu_blend;

    /*Fill a memory array with a color*/
    disp_drv.gpu_fill_cb = gpu_fill;
#endif

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
		g_disp_drv = disp_drv;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#include "stm32f4xx_hal.h"
volatile uint8_t g_gpu_state = 0;


//void DMA2D_IRQHandler(void)
//{

//  if ((DMA2D->ISR & DMA2D_FLAG_TC) != 0U)
//  {
//    if ((DMA2D->CR & DMA2D_IT_TC) != 0U)
//    {

//			DMA2D->CR &= ~DMA2D_IT_TC;
//			DMA2D->IFCR =DMA2D_FLAG_TC;
//			
//			if(g_gpu_state==1){
//				 g_gpu_state = 0;
//				 lv_disp_flush_ready(&g_disp_drv);
//			 }
//    }
//  }
//}

// DMA2D传输完成回调
static void mDMA2Dcallvack(DMA2D_HandleTypeDef *hdma2d)
{
	 if(g_gpu_state==1){
		 g_gpu_state = 0;
		 lv_disp_flush_ready(&g_disp_drv);
	 }
}

// dma2d采用寄存器初始化
static void dma2d_use_reg_init(void)
{
	HAL_NVIC_SetPriority(DMA2D_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2D_IRQn);
	__HAL_RCC_DMA2D_CLK_ENABLE();
}

// dma2D采用HAL初始化
static void dma2d_use_hal_init(void)
{

	hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_M2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB565;
	hdma2d.XferCpltCallback = mDMA2Dcallvack;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {

  }
}

/* Initialize your display and the required peripherals. */
static void disp_init(void)
{
	//dma2d_use_reg_init();
	dma2d_use_hal_init();
}





/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_disp_flush_ready()' has to be called when finished. */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
	//LTDC_Color_Fill(area->x1,area->y1,area->x2,area->y2,(uint16_t*)(color_p));
   
	uint32_t h = area->y2 - area->y1;
	uint32_t w = area->x2 - area->x1;
	
	uint32_t OffLineSrc = 1024 - (area->x2 - area->x1 +1);
	uint32_t addr = LCD_FRAME_BUF_ADDR + 2*(1024*area->y1 + area->x1);
	
	
	// --- 阻塞传输---
	// 模式
//	DMA2D->CR      = 0x00000000UL | (1 << 9);
//	// 源地址
//	DMA2D->FGMAR   = (uint32_t)(uint16_t*)(color_p);
//	// 目标地址
//	DMA2D->OMAR    = (uint32_t)addr;
//	
//	// 输入偏移
//	DMA2D->FGOR    = 0;
//	// 输出偏移
//	DMA2D->OOR     = OffLineSrc;
//	
//	/* 前景层和输出区域都采用的RGB565颜色格式 */
//	DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT_RGB565;
//	DMA2D->OPFCCR  = LTDC_PIXEL_FORMAT_RGB565;
//	
//	DMA2D->NLR     = (uint32_t)((area->y2-area->y1+1) << 16) | (uint16_t)(area->x2 -area->x1 +1);

//	/* 启动传输 */
//	DMA2D->CR   |= DMA2D_CR_START;   

//	/* 等待DMA2D传输完成 */
//	while (DMA2D->CR & DMA2D_CR_START) {} 
//		
//  lv_disp_flush_ready(disp_drv);
		
		
	// -- 中断传输
	// 模式
	DMA2D->CR      = 0x00000000UL | (1 << 9);
	// 源地址
	DMA2D->FGMAR   = (uint32_t)(uint16_t*)(color_p);
	// 目标地址
	DMA2D->OMAR    = (uint32_t)addr;
	
	// 输入偏移
	DMA2D->FGOR    = 0;
	// 输出偏移
	DMA2D->OOR     = OffLineSrc;
	
	/* 前景层和输出区域都采用的RGB565颜色格式 */
	DMA2D->FGPFCCR = DMA2D_OUTPUT_RGB565;
	DMA2D->OPFCCR  = DMA2D_OUTPUT_RGB565;
	
	// 多少行
	DMA2D->NLR     = (uint32_t)((area->y2-area->y1+1) << 16) | (uint16_t)(area->x2 -area->x1 +1);

	// 开启中断
	DMA2D->CR |= DMA2D_IT_TC|DMA2D_IT_TE|DMA2D_IT_CE;
	
	/* 启动传输 */
	DMA2D->CR   |= DMA2D_CR_START;   
	g_gpu_state = 1;
}


/*OPTIONAL: GPU INTERFACE*/
#if LV_USE_GPU

/* If your MCU has hardware accelerator (GPU) then you can use it to blend to memories using opacity
 * It can be used only in buffered mode (LV_VDB_SIZE != 0 in lv_conf.h)*/
static void gpu_blend(lv_disp_drv_t * disp_drv, lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa)
{
    /*It's an example code which should be done by your GPU*/
    uint32_t i;
    for(i = 0; i < length; i++) {
        dest[i] = lv_color_mix(dest[i], src[i], opa);
    }
}

/* If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color
 * It can be used only in buffered mode (LV_VDB_SIZE != 0 in lv_conf.h)*/
static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
                    const lv_area_t * fill_area, lv_color_t color)
{
    /*It's an example code which should be done by your GPU*/
    int32_t x, y;
    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/

    for(y = fill_area->y1; y <= fill_area->y2; y++) {
        for(x = fill_area->x1; x <= fill_area->x2; x++) {
            dest_buf[x] = color;
        }
        dest_buf+=dest_width;    /*Go to the next line*/
    }
}

#endif  /*LV_USE_GPU*/

#else /* Enable this file at the top */

/* This dummy typedef exists purely to silence -Wpedantic. */
typedef int keep_pedantic_happy;
#endif
