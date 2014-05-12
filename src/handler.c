/*
 * handler.c
 *
 *  Created on: May 11, 2014
 *      Author: palotasb
 */

#include "init.h"
#include "handler.h"
#include "stm32f4_discovery.h"
#include "LIS3DSH.h"

Coord deletedCursorPosition = { .x = 0, .y = 0 };
int16_t accMax[3] = { 0, 0, 0 }, accMin[3] = { 0, 0, 0 }, accMaxAge[3] = { -1,
        -1, -1 }, accMinAge[3] = { -1, -1, -1 }, accShaken = 0;
#define MAX_ACC_AGES        25      // 20 ms * 25 = 500 ms#define ACC_DIFF_THRESHOLD  25000   // Just because
#define ACC_SHAKE_MIN       3       // 3 * 500 ms = 1.5 s

void HandleDelete(char doUndeleteToo);
void ReadMEMS(void);
void DoDelete(void);
void DoUndelete(void);

void HandleSignals(void) {
    /*
     ImageData_SetByte(&thePicture, coord(0, 16), GPIO_ReadInputData(GPIOA));
     ImageData_SetByte(&thePicture, coord(1, 16), GPIO_ReadInputData(GPIOA) >> 8);
     ImageData_SetByte(&thePicture, coord(2, 16), GPIO_ReadInputData(GPIOB));
     ImageData_SetByte(&thePicture, coord(3, 16), GPIO_ReadInputData(GPIOB) >> 8);
     // */
    if (signals & SIG_BUTTON_0_PRESSED) {
        signals &= ~SIG_BUTTON_0_PRESSED;

    }
    if (signals & SIG_BUTTON_0_RELEASED) {
        signals &= ~SIG_BUTTON_0_RELEASED;

    }
    if (signals & SIG_BUTTON_1_PRESSED) {
        signals &= ~SIG_BUTTON_1_PRESSED;
        HandleDelete(1);

    }
    if (signals & SIG_BUTTON_1_RELEASED) {
        signals &= ~SIG_BUTTON_1_RELEASED;

    }
    if (signals & SIG_BUTTON_2_PRESSED) {
        signals &= ~SIG_BUTTON_2_PRESSED;
    }
    if (signals & SIG_BUTTON_2_RELEASED) {
        signals &= ~SIG_BUTTON_2_RELEASED;
    }
    if (signals & SIG_ENCODER_A_ROTATED_LEFT) {
        signals &= ~SIG_ENCODER_A_ROTATED_LEFT;
        if (0 < cursorPosition.x)
            cursorPosition.x--;
    }
    if (signals & SIG_ENCODER_A_ROTATED_RIGHT) {
        signals &= ~SIG_ENCODER_A_ROTATED_RIGHT;
        if (cursorPosition.x + 1 < PICTURE_WIDTH * 2)
            cursorPosition.x++;
    }
    if (signals & SIG_ENCODER_B_ROTATED_LEFT) {
        signals &= ~SIG_ENCODER_B_ROTATED_LEFT;
        if (0 < cursorPosition.y)
            cursorPosition.y--;
    }
    if (signals & SIG_ENCODER_B_ROTATED_RIGHT) {
        signals &= ~SIG_ENCODER_B_ROTATED_RIGHT;
        if (cursorPosition.y + 1 < PICTURE_HEIGHT * 2)
            cursorPosition.y++;
    }
    if (signals & SIG_UPDATE_LCD) {
        signals &= ~SIG_UPDATE_LCD;
        Image_DisplayOnLCD(&lcdImage, coord(0, 0), coord(127, 63));
    }
    if (signals & SIG_READ_MEMS) {
        signals &= ~SIG_READ_MEMS;
        ReadMEMS();
    }
}

void DoDelete(void) {
    unsigned int i;
    for (i = 0; i < PICTURE_WIDTH * PICTURE_HEIGHT / 8; i++) {
        deletedPictureData[i] = thePictureData[i];
        thePictureData[i] = 0;
    }
    deletedCursorPosition = cursorPosition;
}

void DoUndelete(void) {
    unsigned int i;
    for (i = 0; i < PICTURE_WIDTH * PICTURE_HEIGHT / 8; i++) {
        thePictureData[i] = deletedPictureData[i];
        deletedPictureData[i] = 0;
    }
    deletedCursorPosition.x = -10;
}

void HandleDelete(char doUndeleteToo) {
    if (cursorPosition.x < deletedCursorPosition.x - 5
            || deletedCursorPosition.x + 5 < cursorPosition.x
            || cursorPosition.y < deletedCursorPosition.y - 5
            || deletedCursorPosition.y + 5 < cursorPosition.y) {
        DoDelete();
    } else if (doUndeleteToo) {
        DoUndelete();
    }
}

void ReadMEMS(void) {
    int16_t acc, i, diff;
    for (i = 0; i < 3; i++) {
        switch (i) {
        case 0:
            acc = (LIS3DSH_Read(LIS3DSH_Reg_X_Out_H) << 8)
                    | LIS3DSH_Read(LIS3DSH_Reg_X_Out_L);
            break;
        case 1:
            acc = (LIS3DSH_Read(LIS3DSH_Reg_X_Out_H) << 8)
                    | LIS3DSH_Read(LIS3DSH_Reg_X_Out_L);
            break;
        case 2:
            acc = (LIS3DSH_Read(LIS3DSH_Reg_Z_Out_H) << 8)
                    | LIS3DSH_Read(LIS3DSH_Reg_Z_Out_L);
            break;
        }
        if (accMax[i] < acc || accMaxAge[i] < 0
                || MAX_ACC_AGES < accMaxAge[i]) {
            if (MAX_ACC_AGES < accMaxAge[i])
            {
                accShaken = 0;
            }
            accMax[i] = acc;
            accMaxAge[i] = 0;
        } else
            accMaxAge[i]++;
        if (acc < accMin[i] || accMinAge[i] < 0
                || MAX_ACC_AGES < accMinAge[i]) {
            if (MAX_ACC_AGES < accMinAge[i])
            {
                accShaken = 0;
            }
            accMin[i] = acc;
            accMinAge[i] = 0;
        } else
            accMinAge[i]++;
        diff = accMax[i] - accMin[i];
        if (ACC_DIFF_THRESHOLD < diff) {
            accShaken++;
        }
        if (ACC_SHAKE_MIN <= accShaken) {
            HandleDelete(0);
            accShaken = 0;
        }
    }
}
