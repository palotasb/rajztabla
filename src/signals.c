/*
 * handlers.c
 *
 *  Created on: May 11, 2014
 *      Author: palotasb
 */

#include "stm32f4xx.h"
#include "signals.h"
#include "init.h"

#define BUTTON_DEBOUNCE_CONST   10
uint16_t buttonDebounce[3];

typedef struct EncoderState {
    uint8_t a;
    uint8_t b;
} EncoderState;
EncoderState previousEncoderStates[2];

uint16_t lcdUpdateCounter = 0, memsReadCounter = 0;

void Signals_Init(void) {
    previousEncoderStates[0].a = GPIO_ReadInputDataBit(ENCODER_A_PORT,
    ENCODER_A_PIN_A);
    previousEncoderStates[0].b = GPIO_ReadInputDataBit(ENCODER_A_PORT,
    ENCODER_A_PIN_B);
    previousEncoderStates[1].a = GPIO_ReadInputDataBit(ENCODER_B_PORT,
    ENCODER_B_PIN_A);
    previousEncoderStates[1].b = GPIO_ReadInputDataBit(ENCODER_B_PORT,
    ENCODER_B_PIN_B);
    buttonDebounce[0] = 0;
    buttonDebounce[1] = 0;
    buttonDebounce[2] = 0;
    signals = 0;
}

void SignalEncoderInputs(void) {
    uint8_t a, b, ab, i;
    GPIO_TypeDef* port;
    uint16_t pin_a, pin_b;
    for (i = 0; i < 2; i++) {
        if (i == 0) {
            port = ENCODER_A_PORT;
            pin_a = ENCODER_A_PIN_A;
            pin_b = ENCODER_A_PIN_B;
        } else {
            port = ENCODER_B_PORT;
            pin_a = ENCODER_B_PIN_A;
            pin_b = ENCODER_B_PIN_B;
        }
        a = GPIO_ReadInputDataBit(port, pin_a);
        b = GPIO_ReadInputDataBit(port, pin_b);
        if (a == previousEncoderStates[i].a && b == previousEncoderStates[i].b)
            continue;
        ab = (a << 1) | b;
        switch (ab) {
        case 0:
            if (previousEncoderStates[i].b)
                signals |=
                        i == 0 ?
                                SIG_ENCODER_A_ROTATED_RIGHT :
                                SIG_ENCODER_B_ROTATED_RIGHT;
            if (previousEncoderStates[i].a)
                signals |=
                        i == 0 ?
                                SIG_ENCODER_A_ROTATED_LEFT :
                                SIG_ENCODER_B_ROTATED_LEFT;
            break;
        case 1:
            if (previousEncoderStates[i].a)
                signals |=
                        i == 0 ?
                                SIG_ENCODER_A_ROTATED_RIGHT :
                                SIG_ENCODER_B_ROTATED_RIGHT;
            if (!previousEncoderStates[i].b)
                signals |=
                        i == 0 ?
                                SIG_ENCODER_A_ROTATED_LEFT :
                                SIG_ENCODER_B_ROTATED_LEFT;
            break;
        case 2:
            if (!previousEncoderStates[i].a)
                signals |=
                        i == 0 ?
                                SIG_ENCODER_A_ROTATED_RIGHT :
                                SIG_ENCODER_B_ROTATED_RIGHT;
            if (previousEncoderStates[i].b)
                signals |=
                        i == 0 ?
                                SIG_ENCODER_A_ROTATED_LEFT :
                                SIG_ENCODER_B_ROTATED_LEFT;
            break;
        case 3:
            if (!previousEncoderStates[i].b)
                signals |=
                        i == 0 ?
                                SIG_ENCODER_A_ROTATED_RIGHT :
                                SIG_ENCODER_B_ROTATED_RIGHT;
            if (!previousEncoderStates[i].a)
                signals |=
                        i == 0 ?
                                SIG_ENCODER_A_ROTATED_LEFT :
                                SIG_ENCODER_B_ROTATED_LEFT;
            break;
        }
        previousEncoderStates[i].a = a;
        previousEncoderStates[i].b = b;
    }
}

void SignalButtonInputs(void) {
    uint8_t i, btnPressed;
    for (i = 0; i < 3; i++) {
        switch (i) {
        case 0:
            btnPressed = !GPIO_ReadInputDataBit(ENCODER_A_PORT, ENCODER_A_PIN_S);
            break;
        case 1:
            btnPressed = !GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN);
            break;
        case 2:
            btnPressed = !GPIO_ReadInputDataBit(ENCODER_B_PORT, ENCODER_B_PIN_S);
            break;
        }
        if (btnPressed && buttonDebounce[i] < BUTTON_DEBOUNCE_CONST) {
            buttonDebounce[i]++;
        } else if (btnPressed && buttonDebounce[i] == BUTTON_DEBOUNCE_CONST) {
            buttonDebounce[i]++;
            signals |= i == 0 ? SIG_BUTTON_0_PRESSED :
                       i == 1 ? SIG_BUTTON_1_PRESSED : SIG_BUTTON_2_PRESSED;
        } else if (!btnPressed && buttonDebounce[i] == BUTTON_DEBOUNCE_CONST + 1) {
            signals |= i == 0 ? SIG_BUTTON_0_RELEASED :
                       i == 1 ? SIG_BUTTON_1_RELEASED : SIG_BUTTON_2_RELEASED;
            buttonDebounce[i] = 0;
        } else if (!btnPressed) {
            buttonDebounce[i] = 0;
        }
    }
}

void SignalLCDUpdate(void) {
    if (lcdUpdateCounter < 10) {
        lcdUpdateCounter++;
    } else {
        lcdUpdateCounter = 0;
        signals |= SIG_UPDATE_LCD;
    }
}

/**
 * Run every 20 milliseconds
 */
void SignalMEMSRead(void) {
    if (memsReadCounter < 20) {
        memsReadCounter++;
    } else {
        memsReadCounter = 0;
        signals |= SIG_READ_MEMS;
    }
}
