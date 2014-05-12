/**
*****************************************************************************
** Kommunikációs mérés - glcd.c
** A grafikus LCD függvényei
*****************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "glcd.h"
#include "globalfunctions.h"
#include "font.h"
//#include "Operate.h"
/* Defines -------------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//******************************************************************************************
// @leírás: Inicializálja a kijelzõt, majd kirajzolja a kezdõképet.
// 			Nem olvassa a "Busy"-flaget.
// 			Az idõzítések szoftveres számláló (GLCD_Delay) alapján történik.
//******************************************************************************************
void GLCD_Init(void){
	GPIO_ResetBits(LCD_E_PORT, LCD_E_PIN);	//GLCD_E = 0
	GPIO_SetBits(LCD_RST_PORT, LCD_RST_PIN);	//GLCD_RESET = 1
	GLCD_Write(3,0,0x3F); 	//GLCD bekapcsolása
	GLCD_Write(3,0,0xC0);	//GLCD Start Line
	GLCD_Clear();			//Kijelzõ törlése
}

//******************************************************************************************
// @leírás: Kijelzõ meghajtó kimenetét engedélyezõ függvény
// @paraméter: newState = ENABLE, #GLCDEN = 0
//			   newStare = DISABLE, #GLCDEN = 1
//******************************************************************************************
void GLCDEN(FunctionalState newState){
	if(newState){
		GPIO_ResetBits(LCD_EN_PORT, LCD_EN_PIN);
	}else{
		GPIO_SetBits(LCD_EN_PORT, LCD_EN_PIN);
	}
}

//******************************************************************************************
// @leírás: Beírja a g_data értéket a kiválasztott kijelzõvezérlõbe (cs_s->CS1, CS2)
// 			utasítás/adat paraméternek megfelelõen.
// 			Általánosan felhasználható 8bit (adat/utasítás) beírására a kijelzõ vezérlõjébe.
// @paraméter: cs_s, 1 = CS1, 2 = CS2, 3 = CS1&CS2
// @paraméter: d_i, 0 = instruction, 1 = data
//******************************************************************************************
void GLCD_Write(char cs_s,char d_i,char g_data){
	uint16_t data = 0x0000;
	switch(cs_s){
		case 1:
			GPIO_SetBits(LCD_CS1_PORT, LCD_CS1_PIN);	//CS1 = 1
		break;
		case 2:
		 	GPIO_SetBits(LCD_CS2_PORT, LCD_CS2_PIN);	//CS2 = 1
		break;
		case 3:
			GPIO_SetBits(LCD_CS1_PORT, LCD_CS1_PIN);	//CS1 = 1
		 	GPIO_SetBits(LCD_CS2_PORT, LCD_CS2_PIN);	//CS2 = 1
		break;
	}
	switch(d_i){
	case 0:
		GPIO_ResetBits(LCD_DI_PORT, LCD_DI_PIN);	//PD6 = 0 -> Instruction
		break;
	case 1:
		GPIO_SetBits(LCD_DI_PORT, LCD_DI_PIN);	//PD6 = 1 -> Data
		break;
	}

	data = LCD_DATA_PORT->IDR;
	data &= LCD_DATA_PORT_MASK;
	data |= g_data << LCD_DATA_PORT_SHIFT;
	LCD_DATA_PORT->ODR = data;

	GLCD_Delay(1);
	GPIO_SetBits(LCD_E_PORT, LCD_E_PIN);	//GLCD_E = 1
	GLCD_Delay(2);
	GPIO_ResetBits(LCD_E_PORT, LCD_E_PIN);	//GLCD_E = 0
	GLCD_Delay(4);
	GPIO_ResetBits(LCD_CS1_PORT, LCD_CS1_PIN);	//CS1 = 0
	GPIO_ResetBits(LCD_CS2_PORT, LCD_CS2_PIN);	//CS2 = 0
}
//******************************************************************************************
// @leírás: Közvetlenül törli a kijelzõt.
// @paraméter: nincs
//******************************************************************************************
void GLCD_Clear(void){
	char x,y;
	for(x=0;x<8;x++){
		GLCD_Write(3,0,0x40);
		GLCD_Write(3,0,(0xB8|x));
		for(y=0;y<64;y++){
			GLCD_Write(3,1,0x00);
	  }//for
	}//for
}
//******************************************************************************************
// @leírás: A kijelzõ adott sor-oszlop metszetet állítja az m_data értéknek megfelelõen.
// @paraméter: m_data: adott metszet rajzolata hexába kódolva (lásd.: BitFont.excel
//			   cX: sor (0-7)
//			   cY: oszlop (0-127)
//******************************************************************************************
void GLCD_Write_Block(char m_data,char cX,char cY){
	char chip=1;
	if(cY>=64){
		chip=2;
		cY-=64;
	}
	GLCD_Write(chip,0,(0x40|cY));
	GLCD_Write(chip,0,(0xB8|cX));
	GLCD_Write(chip,1,m_data);
}
//******************************************************************************************
// @leírás: Az átadott stringet kiírja a kijelzõre a font-data alapján.
// @paraméter: string: tetszõleges szöveg
//			   X: Kijelzõ egy sorát adja meg (0-7)
//			   Y: Kijelzõ egy oszlopát választja ki (0-127)
//******************************************************************************************
void GLCD_WriteString(const char* string,char Y, char X){
	char temp = 0;
	int i=0;
	while(string[i]!='\0'){
		temp = string[i];
		GLCD_Write_Char(temp-32,X,Y+6*i);
		i++;
	}
}
//******************************************************************************************
// @leírás: Egy karaktert ír ki a kijelzõre (fontdata-alapján).
// @paraméter: cPlace: (karakter ASCII-értéke)-32
//			   cX: kijelzõ egy sorát adja meg
//			   cY: kijelzõ egy oszlopát adja meg
//******************************************************************************************
void GLCD_Write_Char(char cPlace,char cX,char cY){
	char i=0;
	char chip=1;
	if(cY>=64){
		chip=2;
		cY-=64;
	}//if
	GLCD_Write(chip,0,(0x40|cY));
	GLCD_Write(chip,0,(0xB8|cX));
	for (i=0;i<5;i++){
	  if (cY+i >= 64){
		  chip=2;
		  GLCD_Write(chip,0,(0x40|(cY+i-64)));
		  GLCD_Write(chip,0,(0xB8|cX));
	  }//if
	  GLCD_Write(chip,1,fontdata[cPlace*5+i]);
	  }//for
}
//******************************************************************************************
// @leírás: Kijelzõ írás-késleltetési idõ.
//******************************************************************************************
void GLCD_Delay(char value){
	Sys_DelayUs(value);
}
