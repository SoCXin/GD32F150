#if !defined( _UTIL_H )

#define _UTIL_H

#include <stdint.h>
#include "gd32f1x0.h"

void strcombine(char *strdest,char *strsrc);
void strcombine_4(char *strdest,char *strsrc);

void inttochars(char *dist,uint32_t src);

uint32_t bytesToInt(char* src );

void inttostr(char *dist,int src);
int zmjcompare(char *src,char *dist);

void vcopy(char* src,char* dist,int start,int len) ;


/*
void delay1ms(int ms);

void delay1us(int us);
*/
#endif
