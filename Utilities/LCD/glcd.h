/**
*****************************************************************************
** Kommunikációs mérés - glcd.h
** A grafikus LCD header file-ja
*****************************************************************************
*/

#ifndef GLCD_H_
#define GLCD_H_

#include "stm32f4xx.h"

#define LCD_DATA_PORT	GPIOE
#define LCD_DATA_PORT_MASK	0x00ff
#define LCD_DATA_PORT_SHIFT	8

#define LCD_EN_PORT	GPIOB
#define LCD_EN_PIN	GPIO_Pin_7

#define LCD_E_PORT	GPIOD
#define LCD_E_PIN	GPIO_Pin_7

#define LCD_RST_PORT	GPIOD
#define LCD_RST_PIN	GPIO_Pin_3

#define LCD_CS1_PORT	GPIOB
#define LCD_CS1_PIN	GPIO_Pin_4

#define LCD_CS2_PORT	GPIOB
#define LCD_CS2_PIN	GPIO_Pin_5

#define LCD_DI_PORT	GPIOD
#define LCD_DI_PIN	GPIO_Pin_6

#define LCD_RW_PORT GPIOE
#define LCD_RW_PIN GPIO_Pin_7

void GLCD_Init(void);
void GLCD_Write(char cs_s,char d_i,char g_data);
void GLCD_Clear(void);
void GLCD_Delay(char value);
void GLCD_Write_Block(char m_data,char cX,char cY);
void GLCD_Write_Char(char cPlace,char cX,char cY);
void GLCD_WriteString(const char* string,char X, char Y);

void GLCDEN(FunctionalState newState);

#endif /* GLCD_H_ */
