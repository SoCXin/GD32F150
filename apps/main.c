/**
  ******************************************************************************
  * @file    ADC/AnalogWatchdog/main.c
  * @author  MCU SD
  * @version V1.0
  * @date    6-Sep-2014
  * @brief   Main program body.
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
//#include "simplequeue.h"
//#include "systick.h"
#include "usart.h"
#include "fmc.h"
#include "conf.h"

/* Private variables ---------------------------------------------------------*/
uint8_t b_onoffflag=0,flag_notACCON=0,flag_stop = 0;
int presspowerkeytime=0,vbat_stdbyflag=0,b_isenscontflag=0,vbatuse=0,isensuse=0,report_volt=0;
bool maskshake=false;
uint16_t vbattmp=0,isenstmp =0;
uint32_t tmp;


#define FMC_PAGE_SIZE           ((uint16_t)0x400)


#define VBAT_STDBY 		0x4F5   // 9V 断电保护 //0x50A		//10v    
#define VBAT_ALERT 		0x585  //0x585	  //11v  
#define ICURRENT_STB 	0x50

#define VBAT_STDBYTIMEOUT	 10
#define	ISENSCONTFLAGINIT  10



uint32_t before_start_time = 30;// 重启后等待时间
uint32_t between_time = 30; // 喂狗后等待时间

uint32_t current_between_time = 0 ; // 当前喂狗等待时间
uint32_t current_start_time = 0 ; // 重启后等待时间

uint32_t boot_version = 0x04030201 ;

uint32_t * mptrd;

uint8_t data[DATA_LEN];	//data
/* Private function prototypes -----------------------------------------------*/
void clearData(void) {
    uint8_t i = 0 ;
    for(i = 0 ; i<32 ; i++) {
        data[i] = 0x00 ;
    }
}



/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    app_version = 0x01000108 ;

    /*Config System clocks --------------------------------------------------*/
    RCC_Configuration();

    /*Config GPIO ports -----------------------------------------------------*/
    GPIO_BootConfiguration();

    /*NVIC configuration ----------------------------------------------------*/
    NVIC_Configuration();
    //NVIC_VectTableSet(NVIC_VECTTAB_FLASH,0x08006400);
    NVIC_VectTableSet(NVIC_VECTTAB_FLASH,0x6400);

    /* Enable write access to IWDG_PSR and IWDG_RLDR registers */
    IWDG_Write_Enable(IWDG_WRITEACCESS_ENABLE);

    /* IWDG counter clock: 40KHz(LSI) / 256 = 0.156 KHz */
    IWDG_SetPrescaler(IWDG_PRESCALER_256);

    /* Set counter reload value to 3120 */
    IWDG_SetReloadValue(3120);  //156=1S //3120

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();

    ADC_Configuration();


    GPIO_SetBits(PORT_PW5VEN,GPIO_PW5VEN);
    GPIO_SetBits(PORT_PW4VEN,GPIO_PW4VEN);


    USART_Configure();

    USART_INT_Set(USART1,USART_INT_RBNE,ENABLE);

    //USART1_Sendstr("now App1");

    Delayus_Configuration();

    //delay_ms(500);
    clearData();
    data[0] = 0 ;
    usart_send_ack(RESP_START_APP,data,1);
    
    mptrd = (uint32_t *)(ADDRESS_APP_VERSION) ;
    tmp = *mptrd ;

    if( tmp != app_version) {

    	readParamFromData();
    	app_version = 0x01000104 ;
    	writeParamToData();

    }
    
		b_onoffflag = 1 ;

    while (1)
    {
        GPIO_SetBits(PORT_OPVEN,GPIO_OPVEN);
        /* Start ADC1 Software Conversion */
        ADC_RegularChannel_Config(CHANN_VBATADC, 1, ADC_SAMPLETIME_13POINT5);
        ADC_SoftwareStartConv_Enable(ENABLE);
        delay_ms(2);

        vbattmp=ADC_GetConversionValue();
        vbatuse=(vbatuse+vbattmp)/2;


        ADC_RegularChannel_Config(CHANN_ISENSADC, 1, ADC_SAMPLETIME_13POINT5);
        ADC_SoftwareStartConv_Enable(ENABLE);
        delay_ms(2);

        isenstmp=ADC_GetConversionValue();
        isensuse=(isensuse+isenstmp)/2;

        GPIO_ResetBits(PORT_OPVEN,GPIO_OPVEN);

        // 从GPIO 中读取 ACC的 状态
        flag_notACCON = GPIO_ReadInputBit(PORT_ACCDET,GPIO_ACCDET);

				// 如果 电压 低于 9V 
        if(vbatuse < VBAT_STDBY)
        {
					// 如果 电压 低于 9V  超过 一定时间
            if(vbat_stdbyflag > VBAT_STDBYTIMEOUT)
            {
                GPIO_ResetBits(PORT_PW5VEN,GPIO_PW5VEN);
                GPIO_ResetBits(PORT_PW4VEN,GPIO_PW4VEN);
                b_onoffflag=0;
                vbat_stdbyflag=0;
								
            } 
						else
            {
                vbat_stdbyflag++;
            }
            b_isenscontflag=0;       		//add 20161229
        }
        else if(vbatuse < VBAT_ALERT)
        {
            if(!(GPIO_ReadInputBit(PORT_ACCDET,GPIO_ACCDET)))  	//ACC is high normal,connect 12v is low
            {
                if(b_onoffflag > 0)
                {
                    GPIO_SetBits(PORT_PW5VEN,GPIO_PW5VEN);
                    GPIO_SetBits(PORT_PW4VEN,GPIO_PW4VEN);
                    flag_stop = 0 ;
                }
                b_isenscontflag=0;      	//add 20161229
            }
            else if(isensuse > ICURRENT_STB)
            {
                if(b_isenscontflag < ISENSCONTFLAGINIT)
                {
                    b_isenscontflag++;
                }
                else
                {
                    GPIO_ResetBits(PORT_PW5VEN,GPIO_PW5VEN);
                    GPIO_ResetBits(PORT_PW4VEN,GPIO_PW4VEN);
                    b_onoffflag=0;
                    b_isenscontflag=0;
                }
            } 
						else
            {
                b_isenscontflag=0;
            }
            vbat_stdbyflag=0;
        }
        else
        {
            if(b_onoffflag>0)
            {
                GPIO_SetBits(PORT_PW5VEN,GPIO_PW5VEN);
                GPIO_SetBits(PORT_PW4VEN,GPIO_PW4VEN);
                flag_stop = 0 ;
            }
            b_isenscontflag=0;
            vbat_stdbyflag=0;
        }
        if(!(GPIO_ReadInputBit(PORT_PWKEY,GPIO_PWKEY)))  			//powerkey high is normal,connect GND is low
        {
            presspowerkeytime++;
        } 
				else
        {
            presspowerkeytime=0;
        }

/*
        if(presspowerkeytime>20)    							//10S
        {
            GPIO_ResetBits(PORT_PW5VEN,GPIO_PW5VEN);
            GPIO_ResetBits(PORT_PW4VEN,GPIO_PW4VEN);
            b_onoffflag=0;
            presspowerkeytime=0;
            maskshake=true;
            flag_stop = 1 ;
        }
*/

        IWDG_ReloadCounter();
        delay_ms(1000);

        if(!flag_notACCON && b_onoffflag != 0) {
            // ACC  ON

            if( current_start_time < before_start_time) {
                current_start_time++ ;
            }
            //before_start_time 20  c= 20 before_start_time = 10 c= 20
            else if(current_start_time >= before_start_time) {
                current_between_time ++ ;
            }


            if(current_between_time > between_time) {
                clearData();
                usart_send_ack(RESP_REBOOT,data,0);
                //USART1_Sendstr("--reboot--\r");

                // 重启机器
                GPIO_ResetBits(PORT_PW5VEN,GPIO_PW5VEN);
                GPIO_ResetBits(PORT_PW4VEN,GPIO_PW4VEN);
                delay_ms(5000);
                //usart_send_ack(RESP_BEFORE_PW_SET,data,0);
                GPIO_SetBits(PORT_PW5VEN,GPIO_PW5VEN);
                GPIO_SetBits(PORT_PW4VEN,GPIO_PW4VEN);

                //delay_ms(2000);
                //usart_send_ack(RESP_BEFORE_PWKEY_RESET,data,0);
                GPIO_ResetBits(PORT_PWKEY,GPIO_PWKEY);
								
							//GPIO_SetBits(PORT_WKUP,GPIO_WKUP);
                //GPIO_ResetBits(PORT_ACCDET,GPIO_ACCDET);
                delay_ms(3000);
                //usart_send_ack(RESP_BEFORE_PWKEY_SET,data,0);
                GPIO_SetBits(PORT_PWKEY,GPIO_PWKEY);
                //GPIO_SetBits(PORT_ACCDET,GPIO_ACCDET);
								//GPIO_ResetBits(PORT_WKUP,GPIO_WKUP);

                current_start_time = 0 ;
                current_between_time = 0 ;

            }

        }
        else if(flag_notACCON) {

            clearData();

            data[0] = current_between_time >> 24 ;
            data[1] = current_between_time >> 16 ;
            data[2] = current_between_time >> 8 ;
            data[3] = current_between_time;

            // app 程序版本
            data[4] = current_start_time >> 24 ;
            data[5] = current_start_time >> 16 ;
            data[6] = current_start_time >> 8 ;
            data[7] = current_start_time ;


            //usart_send_ack(RESP_BEFORE_PW_RESET,data,8);
            delay_ms(200);
        }

        //USART1_Sendstr("--go --on---\r");



        if(u_rx_flag == 0x01) {

            if(u_rx.cmd == CMD_CHECK_VERSION) {
                // 检查版本
                // 首先清空数据
                clearData();
                // 从Flash 中读取设置的参数
                mptrd = (uint32_t *)(ADDRESS_APP_VERSION) ;
                app_version = *mptrd ;

                // boot 程序版本
                data[0] = boot_version >> 24 ;

                data[1] = boot_version >> 16 ;

                data[2] = boot_version >> 8 ;

                data[3] = boot_version;

                // app 程序版本
                data[4] = app_version >> 24 ;

                data[5] = app_version >> 16 ;

                data[6] = app_version >> 8 ;

                data[7] = app_version ;

                delay_ms(500);
                usart_send_ack(RESP_CHECK_VERSION,data,8);
            } else if(u_rx.cmd == CMD_FETCH_DOG) {
                // 检查版本
                // 首先清空数据
                clearData();
                tmp = (u_rx.data[3] << 24) + (u_rx.data[2] << 16) + (u_rx.data[1] << 8) + u_rx.data[0] ;
                if (tmp == 0) {
                    before_start_time = 30 ;
                }
                else {
                    before_start_time = tmp ;
                }
								current_start_time = before_start_time ;
								
                tmp = (u_rx.data[7] << 24) + (u_rx.data[6] << 16) + (u_rx.data[5] << 8) + u_rx.data[4] ;

                if (tmp == 0) {
                    between_time = 30 ;
                }
                else {
                    between_time = tmp ;
                }
                //before_start_time = (u_rx.data[3] << 24) + (u_rx.data[2] << 16) + (u_rx.data[1] << 8) + u_rx.data[0] ;
                //between_time = (u_rx.data[7] << 24) + (u_rx.data[6] << 16) + (u_rx.data[5] << 8) + u_rx.data[4] ;

                current_between_time = 0 ;
                // boot 程序版本

                data[0] = between_time >> 24 ;
                data[1] = between_time >> 16 ;
                data[2] = between_time >> 8 ;
                data[3] = between_time;

                // app 程序版本
                data[4] = current_start_time >> 24 ;
                data[5] = current_start_time >> 16 ;
                data[6] = current_start_time >> 8 ;
                data[7] = current_start_time ;


                delay_ms(500);
                usart_send_ack(RESP_FETCH_DOG,data,8);
            } 
						else if(u_rx.cmd == CMD_CHECK_STATUS) {
                // 检查 App 的 状态
                // 清空 发送数据的 缓存区
                clearData();

                data[0] = 0 ;

                usart_send_ack(RESP_CHECK_STATUS,data,1);


            }
						
						
						else if(u_rx.cmd == CMD_GOTO_BOOT ) {
                //clearData();

                //data[0] = 0 ;
                //usart_send_ack(RESP_GOTO_BOOT,data,1);
                delay_ms(500);
                JumpToBoot();
            }
            else if(u_rx.cmd == CMD_START_APP ) {
                delay_ms(500);
                data[0] = 0 ;
                usart_send_ack(RESP_START_APP,data,1);
            }


            u_rx_flag = 0 ;


        }
        else {
            clearData();

            data[0] = current_between_time >> 24 ;
            data[1] = current_between_time >> 16 ;
            data[2] = current_between_time >> 8 ;
            data[3] = current_between_time;

            // app 程序版本
            data[4] = current_start_time >> 24 ;
            data[5] = current_start_time >> 16 ;
            data[6] = current_start_time >> 8 ;
            data[7] = current_start_time ;


            data[8] = presspowerkeytime ;

            data[9] = vbatuse >> 8 ;

            data[10] = vbatuse ;


            //usart_send_ack(STATUS_APP,data,11);
        }



    }
}
