/*
 * init.c
 *
 *  Created on: May 7, 2014
 *      Author: palotasb
 */

#include "stm32f4xx.h"
#include "init.h"
#include "signals.h"
#include "LIS3DSH.h"

void Init_System(void) {
    SysTick_Config(SystemCoreClock / 1000000);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

void Init_Leds(void) {
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED4);
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED6);
}

void Init_Inputs(void) {
    /*	Input pinek, portok:
     *	Gomb:
     *		Port B Pin 8
     *	Encoder 1:
     *		Port A Pin 1
     *		Port A Pin 2
     *		Port A Pin 3
     *	Encoder 2:
     *		Port B Pin 0
     *		Port B Pin 1
     *		Port B Pin 2
     */
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2
            | GPIO_Pin_8;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    Signals_Init();
}

void Init_LCD(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIMx_TimeBaseStructure;
    TIM_OCInitTypeDef TIMx_OCInitStructure;
    // Init_LCD_GPIO

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    // GPIOB
    GPIO_InitStructure.GPIO_Pin = LCD_CS1_PIN | LCD_CS2_PIN | LCD_EN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // GPIOD
    GPIO_InitStructure.GPIO_Pin = LCD_E_PIN | LCD_RST_PIN | LCD_DI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // GPIOE
    GPIO_InitStructure.GPIO_Pin = LCD_RW_PIN | GPIO_Pin_8 | GPIO_Pin_9
            | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13
            | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_ResetBits(LCD_RW_PORT, LCD_RW_PIN);    //GLCD_RW = 0
    GPIO_SetBits(LCD_E_PORT, LCD_E_PIN);        //GLCD_E = 1
    GPIO_ResetBits(LCD_RST_PORT, LCD_RST_PIN);  //GLCD_RESET = 0
    GPIO_ResetBits(LCD_EN_PORT, LCD_EN_PIN);    //#GLCDEN = 0

    // Timer3_CH3 - PC8, GLCD BackLight PWM
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    // TIMER 3 a PWM háttérvilágításhoz
    /* TIM3 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    /**
     * APB1 = 42MHz
     * fpwm = TIM3_CLK/(Period+1)
     * TIM3_CLK = APB1_CLK / (prescaler + 1)
     * Duty_Cycle = (TIM_Pulse / Tim_Period)*100
     */
    TIMx_TimeBaseStructure.TIM_Prescaler = 349;
    TIMx_TimeBaseStructure.TIM_Period = 2399;
    TIMx_TimeBaseStructure.TIM_ClockDivision = 0;
    TIMx_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIMx_TimeBaseStructure);

    TIMx_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIMx_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIMx_OCInitStructure.TIM_Pulse = 2000;  // (2000/2399)*100=83%
    TIMx_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OC3Init(TIM3, &TIMx_OCInitStructure);

    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM3, ENABLE);

    /* TIM3 enable counter */
    TIM_Cmd(TIM3, ENABLE);

    // LCD IC-it inicializáló fv.

    GLCDEN(ENABLE);

    GLCD_Init();
}

void Init_Image(void) {
    int i;

    cursorPosition.x = PICTURE_WIDTH;
    cursorPosition.y = PICTURE_HEIGHT;
    cursorCoordinateSystem = DESCARTES;
    cursorMode = PENCIL;

    ImageData_Init(&thePicture, coord(PICTURE_WIDTH, PICTURE_HEIGHT),
            thePictureData);
    ImageData_Init(&overlay_DescartesCursor,
            coord(OVERLAY_WIDTH, OVERLAY_HEIGHT), overlay_DescartesCursorData);
    ImageData_Init(&overlay_PolarCursor, coord(OVERLAY_WIDTH, OVERLAY_HEIGHT),
            overlay_PolarCursorData);
    ImageData_Init(&overlay_PencilCursor, coord(OVERLAY_WIDTH, OVERLAY_HEIGHT),
            overlay_PencilCursorData);
    ImageData_Init(&overlay_Eraser, coord(OVERLAY_WIDTH, OVERLAY_HEIGHT),
            overlay_EraserData);

    for (i = 0; i < PICTURE_WIDTH * PICTURE_HEIGHT / 8; i++) {
        thePictureData[i] = 0;
    }

    VirtualImage_Init(&lcdImage, coord(PICTURE_WIDTH, PICTURE_HEIGHT),
            (Image*) &thePicture, &Image_Empty, coord(0, 0),
            coord((PICTURE_WIDTH - OVERLAY_WIDTH) / 2,
                    (PICTURE_HEIGHT - OVERLAY_HEIGHT) / 2));
    lcdImage.combinator = VIC_JUST_B;

}

void Init_Timer(void) {
    TIM_TimeBaseInitTypeDef TimeBaseStruct;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_TimeBaseStructInit(&TimeBaseStruct);

    // APB1 clock = 84 MHz
    // Timer clock = 84 MHz / 84 = 1 MHz
    // Timer update = 1 us * 1000 = 1 ms
    TimeBaseStruct.TIM_Prescaler = 83;
    TimeBaseStruct.TIM_Period = 1000;
    TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TimeBaseStruct);
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);

    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}

void Init_MEMS(void) {
    LIS3DSH_Init();
}

