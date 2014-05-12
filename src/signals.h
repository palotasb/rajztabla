/*
 * handlers.h
 *
 *  Created on: May 11, 2014
 *      Author: palotasb
 */

#ifndef SIGNALS_H_
#define SIGNALS_H_

#include "stm32f4xx.h"


void Signals_Init(void);
void SignalEncoderInputs(void);
void SignalButtonInputs(void);
void SignalLCDUpdate(void);
void SignalMEMSRead(void);

#endif /* SIGNALS_H_ */
