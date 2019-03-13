/* Includes ------------------------------------------------------------------*/
#include "simplequeue.h"

int smpQueue_FRONT = 1,smpQueue_REAR = 1;
uint16_t smpQueue_Tube[SMPQUEUE_MAX];

void smpQueue_Init(void)
{
    int i;
    for(i=0; i<SMPQUEUE_MAX; i++)
    {
        smpQueue_Tube[i]=0;
    }
    smpQueue_REAR=0;
    smpQueue_FRONT=0;
}

int smpQueue_Size(void)
{
    int qsize;
    if((smpQueue_REAR>=SMPQUEUE_MAX)||(smpQueue_FRONT>=SMPQUEUE_MAX))
    {
        smpQueue_Init();
        qsize=0;
    } else if(smpQueue_REAR>smpQueue_FRONT)
    {
        qsize=smpQueue_FRONT+SMPQUEUE_MAX-smpQueue_REAR;
    } else
    {
        qsize=smpQueue_FRONT-smpQueue_REAR;
    }
    return qsize;
}
int smpQueue_F(void) {
    return smpQueue_FRONT ;
}
int smpQueue_R(void) {
    return smpQueue_REAR;
}
BOOL smpQueue_Empty(void)
{
    int qsize;
    qsize=smpQueue_Size();

    if(qsize ==0)
        return TRUE;
    else
        return FALSE;
}

BOOL smpQueue_Full(void)
{
    int qsize;
    qsize=smpQueue_Size();

    if(qsize >= SMPQUEUE_MAX-1)
        return TRUE;
    else
        return FALSE;
}

int smpQueue_Add(uint16_t atq)
{
    if(!smpQueue_Full())
    {
        smpQueue_Tube[smpQueue_FRONT]=atq;
        smpQueue_FRONT++;
        if(smpQueue_FRONT>=SMPQUEUE_MAX)
            smpQueue_FRONT-=SMPQUEUE_MAX;
        return SMPQUEUE_STATUE_OK;
    } else
    {
        return SMPQUEUE_STATUE_FULL;
    }
}

int smpQueue_Del(uint16_t* dfq)
{
    if(!smpQueue_Empty())
    {
        *dfq=smpQueue_Tube[smpQueue_REAR];
        smpQueue_REAR++;
        if(smpQueue_REAR>=SMPQUEUE_MAX)
            smpQueue_REAR-=SMPQUEUE_MAX;
        return SMPQUEUE_STATUE_OK;
    } else
    {
        return SMPQUEUE_STATUE_EMPTY;
    }
}

