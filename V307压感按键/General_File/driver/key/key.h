#ifndef __KEY_H
#define __KEY_H
#include "ch32v30x_gpio.h"
#include "filter.h"
#include "param.h"

void Key_Init(void);
void Key_DeInit(void);

void Key_Scan(void);
void Data_Process(void);
void Send_Data(void);

void Adjust_Init(void);
void Adjust_Process(void);
void Adjust_Update(void);

#endif