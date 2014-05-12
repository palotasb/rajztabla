/*
 * init.h
 *
 *  Created on: May 7, 2014
 *      Author: palotasb
 */

#ifndef INIT_H_
#define INIT_H_

#include "stm32f4_discovery.h"
#include "stm32f4xx.h"
#include "glcd.h"
#include "image.h"

#define ENCODER_A_PORT  GPIOB
#define ENCODER_A_PIN_A GPIO_Pin_0
#define ENCODER_A_PIN_B GPIO_Pin_1
#define ENCODER_A_PIN_S GPIO_Pin_2
#define ENCODER_B_PORT  GPIOA
#define ENCODER_B_PIN_A GPIO_Pin_1
#define ENCODER_B_PIN_B GPIO_Pin_2
#define ENCODER_B_PIN_S GPIO_Pin_3
#define BUTTON_PORT     GPIOB
#define BUTTON_PIN      GPIO_Pin_8

#define PICTURE_WIDTH	128
#define PICTURE_HEIGHT	64
#define OVERLAY_WIDTH	16
#define OVERLAY_HEIGHT	16

Coord cursorPosition;
enum CursorMode {
    PENCIL = 1, ERASER = 0
} cursorMode;
enum CursorCoordinateSystem {
    DESCARTES = 0, POLAR = 1
} cursorCoordinateSystem;
ImageData thePicture;
uint8_t thePictureData[PICTURE_HEIGHT * PICTURE_WIDTH / 8];
uint8_t deletedPictureData[PICTURE_HEIGHT * PICTURE_WIDTH / 8];
VirtualImage lcdImage;

ImageData overlay_DescartesCursor;
uint8_t overlay_DescartesCursorData[OVERLAY_HEIGHT * OVERLAY_WIDTH / 8];
ImageData overlay_PolarCursor;
uint8_t overlay_PolarCursorData[OVERLAY_HEIGHT * OVERLAY_WIDTH / 8];
ImageData overlay_PencilCursor;
uint8_t overlay_PencilCursorData[OVERLAY_HEIGHT * OVERLAY_WIDTH / 8];
ImageData overlay_Eraser;
uint8_t overlay_EraserData[OVERLAY_HEIGHT * OVERLAY_WIDTH / 8];

volatile enum Signals {
    SIG_ENCODER_A_ROTATED_LEFT  = 0x0001,
    SIG_ENCODER_A_ROTATED_RIGHT = 0x0002,
    SIG_ENCODER_B_ROTATED_LEFT  = 0x0004,
    SIG_ENCODER_B_ROTATED_RIGHT = 0x0008,
    SIG_BUTTON_0_PRESSED        = 0x0010,
    SIG_BUTTON_1_PRESSED        = 0x0020,
    SIG_BUTTON_2_PRESSED        = 0x0040,
    SIG_BUTTON_0_RELEASED       = 0x0080,
    SIG_BUTTON_1_RELEASED       = 0x0100,
    SIG_BUTTON_2_RELEASED       = 0x0200,
    SIG_UPDATE_LCD              = 0x0400,
    SIG_READ_MEMS               = 0x0800
} signals;

void Init_System(void);
void Init_Leds(void);
void Init_Inputs(void);
void Init_LCD(void);
void Init_Image(void);
void Init_Timer(void);
void Init_MEMS(void);

#endif /* INIT_H_ */
