
/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "util.h"
#include "usart.h"
#include "fmc.h"

/* Private variables ---------------------------------------------------------*/

#define GPIO_WKUP	GPIO_PIN_0
#define PORT_WKUP	GPIOA

#define GPIO_ACCDET GPIO_PIN_3
#define PORT_ACCDET GPIOA

#define GPIO_PWKEY GPIO_PIN_7
#define PORT_PWKEY GPIOA

#define GPIO_OPVEN GPIO_PIN_5
#define PORT_OPVEN GPIOA

#define GPIO_PW5VEN GPIO_PIN_8
#define PORT_PW5VEN GPIOA

#define GPIO_PW4VEN GPIO_PIN_6
#define PORT_PW4VEN GPIOA

#define GPIO_ISENSADC GPIO_PIN_2
#define PORT_ISENSADC GPIOA
#define CHANN_ISENSADC ADC_CHANNEL_2

#define GPIO_VBATADC GPIO_PIN_1
#define PORT_VBATADC GPIOA
#define CHANN_VBATADC ADC_CHANNEL_1

#define VBAT_STDBY 		0x4F5  //0x50A		//10v    
#define VBAT_ALERT 		0x585  //0x585	  //11v  
#define ICURRENT_STB 	0x50


#define POWERKEYTIMEOUT 	 60000
#define VBAT_STDBYTIMEOUT	 30000
#define	ISENSCONTFLAGINIT  30000
#define REPORT_VBAT					10000

uint32_t boot_version = 0x04030201 ;

ADC_InitPara  ADC_InitStructure;
uint8_t flag_notACCON=0,b_onoffflag=0 ;
int presspowerkeytime=0,vbat_stdbyflag=0,b_isenscontflag=0,vbatuse=0,isensuse=0,report_volt=0;
bool maskshake=false;

int count = 0 ;//readcount

uint8_t data[DATA_LEN];	//data

uint32_t writeDataBuffer[DATA_LEN/4 + 1] ; // 写入Flash 中的数据

uint8_t temp = 0x00 ;

uint8_t counterdown = 0 ;

uint32_t temp_32 = 0;


/* Private function prototypes -----------------------------------------------*/


void clearData(void) {
    uint8_t i = 0 ;
    for(i = 0 ; i<32 ; i++) {
        data[i] = 0x00 ;
    }
}


void delay_us(uint16_t delay)
{
    TIMER_Enable(TIMER2, ENABLE);
    TIMER_SetCounter(TIMER2, delay);
    while(delay > 1)
    {
        delay = TIMER_GetCounter(TIMER2);
    }
    TIMER_Enable(TIMER2, DISABLE);
}


void delay_ms(uint16_t delay)
{
    while(delay--) {
        delay_us(1000);
    }
}


void RCC_Configuration(void)
{
    /* ADCCLK = PCLK2/6 */
    RCC_ADCCLKConfig(RCC_ADCCLK_APB2_DIV6);
    /* Enable GPIOC clock */
    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB, ENABLE);
    /* Enable ADC1 and GPIO_LED clock */
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_ADC1, ENABLE);
    /* Enable SYSCFG clock */
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_CFG | RCC_APB2PERIPH_USART1, ENABLE);

    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_PWR, ENABLE);

    RCC_USARTCLKConfig(RCC_USART1CLK_HSI);



    RCC_AHBPeriphClock_Enable(RCC_AHBPERIPH_GPIOA | RCC_AHBPERIPH_GPIOB,ENABLE );
    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_USART2,ENABLE);
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_USART1,ENABLE);
    RCC_APB2PeriphClock_Enable(RCC_APB2PERIPH_SPI1,ENABLE);

}

void GPIO_BootConfiguration(void)
{
    GPIO_InitPara GPIO_InitStructure;

    /* Configure PC1 (ADC Channel11) as analog input -------------------------*/
    GPIO_InitStructure.GPIO_Pin = GPIO_VBATADC | GPIO_ISENSADC;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_AN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_WKUP ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
    GPIO_Init(PORT_WKUP, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_ACCDET ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_PULLUP;
    GPIO_Init(PORT_ACCDET, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin = GPIO_PWKEY ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_PULLUP;
    GPIO_Init(PORT_PWKEY, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PW5VEN ;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_2MHZ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_PULLDOWN;
    GPIO_Init(PORT_PW5VEN, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_PW4VEN ;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_2MHZ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_PULLDOWN;
    GPIO_Init(PORT_PW4VEN, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_OPVEN ;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_2MHZ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_PULLDOWN;
    GPIO_Init(PORT_OPVEN, &GPIO_InitStructure);

    //USART1
    GPIO_InitStructure.GPIO_Pin     = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_IN;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PUPD_PULLDOWN;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    //GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE13,GPIO_AF_0);
    //GPIO_PinAFConfig(GPIOA,GPIO_PINSOURCE14,GPIO_AF_0);
}


void NVIC_Configuration(void)
{
    NVIC_InitPara NVIC_InitStructure;
    EXTI_InitPara EXTI_InitStructure;

    /* Configure and enable ADC interrupt */
    NVIC_InitStructure.NVIC_IRQ = ADC1_CMP_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_PRIGroup_Enable(NVIC_PRIGROUP_1);
    NVIC_InitStructure.NVIC_IRQ = EXTI2_3_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Connect EXTI3 Line to PA3 pin */
    SYSCFG_EXTILine_Config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN3);
    EXTI_InitStructure.EXTI_LINE = EXTI_LINE3;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LINEEnable = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Connect EXTI7 Line to PA7 pin */
    SYSCFG_EXTILine_Config(EXTI_SOURCE_GPIOA, EXTI_SOURCE_PIN7);
    EXTI_InitStructure.EXTI_LINE = EXTI_LINE7;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LINEEnable = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_PRIGroup_Enable(NVIC_PRIGROUP_1);
    NVIC_InitStructure.NVIC_IRQ = EXTI4_15_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*Configure EXTI Line17(RTC Alarm) to generate an interrupt on rising edge*/
    EXTI_ClearIntBitState(EXTI_LINE17);
    EXTI_InitStructure.EXTI_LINE = EXTI_LINE17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LINEEnable = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* NVIC configuration */
    NVIC_InitStructure.NVIC_IRQ = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* USART1 */
    /*EXTI_ClearIntBitState(EXTI_LINE25);
    EXTI_InitStructure.EXTI_LINE = EXTI_LINE25;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LINEEnable = ENABLE;
    EXTI_Init(&EXTI_InitStructure);*/
    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQ = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQPreemptPriority = 0;
    NVIC_InitStructure.NVIC_IRQSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQEnable = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
void ADC_Configuration(void)
{
    /* Config ADC1  ----------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode_Scan = DISABLE;
    ADC_InitStructure.ADC_Mode_Continuous = DISABLE;
    ADC_InitStructure.ADC_Trig_External = ADC_EXTERNAL_TRIGGER_MODE_NONE;
    ADC_InitStructure.ADC_Data_Align = ADC_DATAALIGN_RIGHT;
    ADC_InitStructure.ADC_Channel_Number = 1;
    ADC_Init(&ADC_InitStructure);

    /* Enable ADC1 */
    ADC_Enable(ENABLE);

    /* ADC1 calibration */
    ADC_Calibration();
    delay_ms(5);
}

int RTC_ALARM_PreConfig(bool RST)
{
    int i,j;
    /* Allow access to RTC */
    PWR_BackupAccess_Enable(ENABLE);

    if(RST)
    {
        RCC_BackupReset_Enable(ENABLE);
        RCC_BackupReset_Enable(DISABLE);
    }

    /* Enable the LSE OSC */
    RCC_LSEConfig(ENABLE);
    /* Wait till LSE is ready */
    for(j=0; j<200; j++)
    {
        for(i=0; i<20000; i++)
        {
            if(!(RCC_GetBitState(RCC_FLAG_LSESTB) == RESET))
                break;
        }
        if(i<20000)
            break;
    }
    if(j>=200)
    {
        return 0x01;
    }
    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* Enable the RTC Clock */
    RCC_RTCCLK_Enable(ENABLE);

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitRSF_ToSetAgain();

    return 0;
}
void Delayus_Configuration(void)
{
    TIMER_BaseInitPara  TIMER_TimeBaseStructure;

    RCC_APB1PeriphClock_Enable(RCC_APB1PERIPH_TIMER2,ENABLE);

    TIMER_TimeBaseStructure.TIMER_Period = 1;
    TIMER_TimeBaseStructure.TIMER_Prescaler = ((SystemCoreClock / 1000000) - 1);
    TIMER_TimeBaseStructure.TIMER_ClockDivision = TIMER_CDIV_DIV1;
    TIMER_TimeBaseStructure.TIMER_CounterMode = TIMER_COUNTER_DOWN;
    TIMER_BaseInit(TIMER2, &TIMER_TimeBaseStructure);
}


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	
	int vbattmp=0,isenstmp=0;
	
    /*Config System clocks --------------------------------------------------*/
    RCC_Configuration();

    /*Config GPIO ports -----------------------------------------------------*/
    GPIO_BootConfiguration();

    /*NVIC configuration ----------------------------------------------------*/
    NVIC_Configuration();
    NVIC_VectTableSet(NVIC_VECTTAB_FLASH,0x0000);



    /* Enable write access to IWDG_PSR and IWDG_RLDR registers */
    IWDG_Write_Enable(IWDG_WRITEACCESS_ENABLE);

    /* IWDG counter clock: 40KHz(LSI) / 256 = 0.156 KHz */
    IWDG_SetPrescaler(IWDG_PRESCALER_256);


    /* Set counter reload value to 3120 */
    IWDG_SetReloadValue(3900);  //156=1S //3120

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();

#if NOTECODE
    RTC_ALARM_Config();
#endif

    ADC_Configuration();


    // 电源 置 为高位
		GPIO_SetBits(PORT_PW5VEN,GPIO_PW5VEN);
    GPIO_SetBits(PORT_PW4VEN,GPIO_PW4VEN);

    USART_Configure();


    USART_INT_Set(USART1,USART_INT_RBNE,ENABLE);

    //clrtimbuff();


    Delayus_Configuration();

    //USART1_Sendstr(" -- current is boot project \r\n");

    //delay_ms(500);
		
		setAppFlag();
		

    clearData();
    data[0] = 0 ;
    usart_send_ack(RESP_GOTO_BOOT,data,1);

		b_onoffflag=1;
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
			
		if(vbatuse<VBAT_STDBY)
		{
			if(vbat_stdbyflag>VBAT_STDBYTIMEOUT)
			{
				GPIO_ResetBits(PORT_PW5VEN,GPIO_PW5VEN);
				GPIO_ResetBits(PORT_PW4VEN,GPIO_PW4VEN);
				b_onoffflag=0;
				vbat_stdbyflag=0;
			}else
			{
				vbat_stdbyflag++;
			}
			b_isenscontflag=0;       		//add 20161229
		}
		else if(vbatuse<VBAT_ALERT)
		{
			if(!(GPIO_ReadInputBit(PORT_ACCDET,GPIO_ACCDET)))  	//ACC is high normal,connect 12v is low
			{
				if(b_onoffflag>0)
				{
					GPIO_SetBits(PORT_PW5VEN,GPIO_PW5VEN);
					GPIO_SetBits(PORT_PW4VEN,GPIO_PW4VEN);
				}
				b_isenscontflag=0;      	//add 20161229
			}
			else if(isensuse>ICURRENT_STB)
			{
				if(b_isenscontflag<ISENSCONTFLAGINIT)
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
			}else
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
			}
			b_isenscontflag=0;
			vbat_stdbyflag=0;
		}
			
			
			
        IWDG_ReloadCounter();
        delay_ms(1000);

        counterdown ++ ;

        if(u_rx_flag == 0x01) {

            counterdown = 0 ;

            if(u_rx.cmd == CMD_CHECK_VERSION) {
                // 检查版本
                // 首先清空数据
                clearData();
                // 从Flash 中读取设置的参数
                readParamFromData();

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

                usart_send_ack(RESP_CHECK_VERSION,data,8);
            }
            else if(u_rx.cmd == CMD_CHECK_STATUS) {
                // 检查 App 的 状态
                // 清空 发送数据的 缓存区
                clearData();
                // 检查 App的状态
                temp = checkAppStatus();

                data[0] = temp ;

                usart_send_ack(RESP_CHECK_STATUS,data,1);


            }
            else if(u_rx.cmd == CMD_START_APP) {
                // 跳转到App
                clearData();

                // 检查 App的状态
                temp = checkAppStatus();

                data[0] = temp ;

                if(temp == 0x0) {


                    delay_ms(500);
                    startApplication() ;
                }
                else {
                    data[0] = temp ;
                    usart_send_ack(RESP_START_APP,data,1);
                }


            }
            else if(u_rx.cmd == CMD_EARSE_APP) {
                // 清除App 区域的Flash中的数据
                clearData() ;
                // FMC 清除操作
                clearAppArea();

                // 通常认为是成功的
                usart_send_ack(RESP_EARSE_APP,data,0);

            }
            else if(u_rx.cmd == CMD_SET_PARAM) {
                // 设置参数 ( App 的大小,checksum ,App 版本 )
                clearData();
                // 从Flash 中读取设置的参数
                readParamFromData();

                if(u_rx.len == 12) {

                    // 长度满足的话.  第一个四位 : App 大小. 第二个四位: checksum  .第三个四位: App版本.(遵循CHECK_VERSION中的版本定义要求)

                    write_app_len = (u_rx.data[3] << 24) + (u_rx.data[2] << 16) + (u_rx.data[1] << 8) + u_rx.data[0] ;
                    write_app_checksum = (u_rx.data[7] << 24) + (u_rx.data[6] << 16) + (u_rx.data[5] << 8) + u_rx.data[4] ;
                    app_version = (u_rx.data[11] << 24) + (u_rx.data[10] << 16) + (u_rx.data[9] << 8) + u_rx.data[8] ;
									app_flag = APP_FLAG_READY_TO_WRITE ;

                    writeParamToData();

                    usart_send_ack(RESP_SET_PARAM,data,0);

                }
                else {

                    // 长度不满足的话...返回参数错误 3
                    data[0] = 3 ;
                    usart_send_ack(RESP_SET_PARAM,data,1);

                }

            }
            else if(u_rx.cmd == CMD_WRITE_DATA) {
                // 通过串口写数据

                uint8_t i = 0 ;
                uint32_t offset = 0 ;

                clearData();

                if(u_rx.len > 4) {

                    // 数据写入偏移
                    offset = (u_rx.data[3] << 24) + (u_rx.data[2] << 16) + (u_rx.data[1] << 8) + u_rx.data[0] ;

                    // 除以4 ,如果len 不是4的倍数 ,则舍掉最后的多余的部分
                    for(i = 0 ; i < u_rx.len / 4 - 1; i++) {
                        temp_32 = (u_rx.data[7+i*4] << 24) + (u_rx.data[6+i*4] << 16) + (u_rx.data[5+i*4] << 8) + u_rx.data[4+i*4] ;
                        writeDataBuffer[i] = temp_32 ;
                    }

                    // 写入到flash 中
                    temp = writeData(offset,writeDataBuffer,u_rx.len / 4 - 1);

                    data[0] = temp ;
                    usart_send_ack(RESP_WRITE_DATA,data,1);


                }
                else {

                    // 长度不满足的话...返回参数错误 3
                    data[0] = 3 ;
                    usart_send_ack(RESP_WRITE_DATA,data,1);

                }
            } else if(u_rx.cmd == CMD_GOTO_BOOT ) {
                clearData();
                data[0] = 0 ;
                usart_send_ack(RESP_GOTO_BOOT,data,1);
            }
            else if(u_rx.cmd == CMD_FETCH_DOG ) {
                clearData();
                data[0] = 0 ;
                usart_send_ack(RESP_FETCH_DOG,data,1);
            }
            u_rx_flag = 0 ;





        }
        else {

            temp = checkAppStatus();

            if (counterdown > 10) {
                if (temp == 0x00) {
                    delay_ms(500);
                    startApplication() ;

                }
                else {

                    clearData();
                    data[0] = temp ;
                    //  usart_send_ack(STATUS_APP,data,1);

                }
            } else {
                clearData();
                data[0] = counterdown ;
                data[1] = u_rx_flag ;
                data[2] = u_rx.cmd ;

                //usart_send_ack(STATUS_COUNTER_DOWN,data,3);
            }


        }







    }

}
