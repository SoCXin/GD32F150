#include "util.h"

void strcombine(char *strdest,char *strsrc)
{
    int i=0,j=0;
    for(i=0; i<400; i++)
        if(strdest[i]=='\0')
            break;
    for(j=0; j<400; j++)
    {
        strdest[i+j]=strsrc[j];
        if(strsrc[j]=='\0')
            break;
    }
}
void strcombine_4(char *strdest,char *strsrc)
{
    int i=0,j=0;
    for(i=0; i<400; i++)
        if(strdest[i]=='\0')
            break;
    for(j=0; j<4; j++)
    {
        strdest[i+j]=strsrc[j];

    }
}

void inttochars(char *dist,uint32_t src)
{
    char *disttmp=dist;
    uint8_t temp = 0x00 ;
    temp=(src&0xff000000)>>24;
    disttmp[3] = temp  ;
    temp=(src&0xff0000)>>16;
    disttmp[2] = temp ;
    temp=(src&0xff00)>>8;
    disttmp[1] = temp ;
    temp=(src&0xff);

    disttmp[0] = temp ;
}

uint32_t bytesToInt(char* src ) {
    uint32_t count = 0x00,temp0 = 0x00,temp1 = 0x00,temp2 = 0x00,temp3 = 0x00;
    temp0 = src[3] ;
    //temp0 = ((temp0 << 8) << 8) << 8 ;
    temp0 = temp0 << 24 ;
    temp1 = src[2] ;
    temp1 = temp1 << 16;
    temp2 = src[1] ;
    temp2 = temp2 << 8 ;
    temp3 = src[0] ;

    count =  temp0 + temp1 + temp2 + temp3 ;
    return count ;

}


void inttostr(char *dist,int src)
{
    char *disttmp=dist;
    int temp=src;

    if(temp > 10000) {
        dist = "FFFF" ;
        return ;
    }

    if(temp>=1000)
    {
        disttmp[0]=(int)temp/1000+0x30;
        temp-=(int)((int)temp/1000)*1000;

        if (src >= 100 ) {

            disttmp[1]=(int)temp/100+0x30;
            temp-=(int)((int)temp/100)*100;

            if (src >= 10 ) {
                disttmp[2]=(int)temp/10+0x30;
                temp-=(int)((int)temp/10)*10;
            }
            else {
                disttmp[2]= 0x30;
            }
        }
        else {
            disttmp[1] = 0x30 ;
            if (src >= 10 ) {
                disttmp[2]=(int)temp/10+0x30;
                temp-=(int)((int)temp/10)*10;
            }
            else {
                disttmp[2]=0x30;
            }
        }

    } else
    {
        disttmp[0]=0x30;
        if (src >= 100 ) {

            disttmp[1]=(int)temp/100+0x30;
            temp-=(int)((int)temp/100)*100;

            if (src >= 10 ) {
                disttmp[2]=(int)temp/10+0x30;
                temp-=(int)((int)temp/10)*10;
            }
            else {

                disttmp[2]=0x30;
            }
        }
        else {
            disttmp[1] = 0x30 ;
            if (src >= 10 ) {
                disttmp[2]=(int)temp/10+0x30;
                temp-=(int)((int)temp/10)*10;
            }
            else {

                disttmp[2]=0x30;
            }
        }


    }
    disttmp[3]=temp+0x30;
}
int zmjcompare(char *src,char *dist)
{
    int i;
    for(i=0; i<255; i++)
    {
        if(src[i]!=dist[i])
            break;
    }
    return i;
}

void vcopy(char* src,char* dist,int start,int len) {
    int i,j ;
    j = start + len ;

    for(i=start; i< j; i++)
    {
        dist[i-start]=src[i];
    }
}
/*
void delay1ms(int ms)
{
    int i,j;
    for(i=0; i<ms; i++)
        for(j=0; j<1250; j++)
        {}
}

void delay1us(int us)
{
    int i,j;
    for(i=0; i<us; i++)
        for(j=0; j<1; j++)
        {}
}

*/





