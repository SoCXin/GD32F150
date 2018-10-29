#include "conf.h"


ADC_InitPara  ADC_InitStructure;



void delay1us(int us)
{
    int i,j;
    for(i=0; i<us; i++)
        for(j=0; j<1; j++)
        {}
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
/*
    
     GPIO_InitStructure.GPIO_Pin = GPIO_WKUP ;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_2MHZ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_PULLDOWN;
    GPIO_Init(PORT_WKUP, &GPIO_InitStructure);
*/
    

    GPIO_InitStructure.GPIO_Pin = GPIO_ACCDET ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_PULLUP;
    GPIO_Init(PORT_ACCDET, &GPIO_InitStructure);

/*
    GPIO_InitStructure.GPIO_Pin = GPIO_PWKEY ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_PULLUP;
    GPIO_Init(PORT_PWKEY, &GPIO_InitStructure);
*/

     GPIO_InitStructure.GPIO_Pin = GPIO_PWKEY ;
    GPIO_InitStructure.GPIO_Speed = GPIO_SPEED_2MHZ;
    GPIO_InitStructure.GPIO_Mode = GPIO_MODE_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OTYPE_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_PULLUP;
    GPIO_Init(PORT_PWKEY, &GPIO_InitStructure);
		
		GPIO_SetBits(PORT_PWKEY,GPIO_PWKEY);
    

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
    /*
        GPIO_InitStructure.GPIO_Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
        GPIO_InitStructure.GPIO_Mode = GPIO_MODE_IN;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PUPD_NOPULL;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    */
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
    //RTC_WaitRSF_ToSetAgain();

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

