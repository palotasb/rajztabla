/*!
 * \author no1wudi
 * \file LIS3DSH_HW.c
 */


#include "LIS3DSH_HW.h"

void LIS3DSH_Init_SPI_Bus(void){
	SPI_InitTypeDef SPI_FOR_LIS3DSH;
	GPIO_InitTypeDef GPIO_FOR_LIS3DSH;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	GPIO_FOR_LIS3DSH.GPIO_Mode = GPIO_Mode_AF;
	GPIO_FOR_LIS3DSH.GPIO_OType = GPIO_OType_PP;
	GPIO_FOR_LIS3DSH.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_FOR_LIS3DSH.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_FOR_LIS3DSH.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_FOR_LIS3DSH);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1);
	
	//CS Pin
	GPIO_FOR_LIS3DSH.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_FOR_LIS3DSH.GPIO_Pin = GPIO_Pin_3;
	GPIO_FOR_LIS3DSH.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE,&GPIO_FOR_LIS3DSH);
	
	CS_H;
	
	SPI_FOR_LIS3DSH.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_FOR_LIS3DSH.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_FOR_LIS3DSH.SPI_CPOL = SPI_CPOL_High;
	SPI_FOR_LIS3DSH.SPI_CRCPolynomial = 7;
	SPI_FOR_LIS3DSH.SPI_DataSize = SPI_DataSize_8b;
	SPI_FOR_LIS3DSH.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_FOR_LIS3DSH.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_FOR_LIS3DSH.SPI_Mode = SPI_Mode_Master;
	SPI_FOR_LIS3DSH.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI1,&SPI_FOR_LIS3DSH);
	SPI_Cmd(SPI1,ENABLE);
}


unsigned char LIS3DSH_SPI_Read_Write(unsigned char Data){
	SPI_I2S_SendData(SPI1,Data);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI1);
}
