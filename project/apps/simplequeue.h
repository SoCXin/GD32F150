#include "gd32f1x0.h"


typedef enum { FALSE = 0, TRUE } BOOL;
#define SMPQUEUE_MAX 100
#define SMPQUEUE_STATUE_OK 0x00;
#define SMPQUEUE_STATUE_EMPTY	0x01;
#define SMPQUEUE_STATUE_FULL	0x02;

void smpQueue_Init(void);
BOOL smpQueue_Empty(void);
BOOL smpQueue_Full(void);
int smpQueue_Size(void);
int smpQueue_Add(uint16_t atq);
int smpQueue_Del(uint16_t *dfq);
