/*
    Author : Atanu Kumar Dey
            University of Chittagong
            Department of CSE (19th batch)
            atanukumar251@gmail.com
*/

#include "Display.h"


bool Displaydata(const char *printdata, uint8_t datalength)
{
#ifdef _DEBUG
    Serial.println(printdata);
#endif
    return true;
}

bool Displaydata(char printdata[], uint8_t datalength)
{
#ifdef _DEBUG
    Serial.println(printdata);
#endif
    return true;
}