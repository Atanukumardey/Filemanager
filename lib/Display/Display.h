/*
    Author : Atanu Kumar Dey
            University of Chittagong
            Department of CSE (19th batch)
            atanukumar251@gmail.com
*/

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

#define _DEBUG


#ifdef __cplusplus
extern "C++"{
#endif
bool Displaydata(const char *printdata, uint8_t datalength);

bool Displaydata(char printdata[], uint8_t datalength);

#ifdef __cplusplus
}
#endif
#endif