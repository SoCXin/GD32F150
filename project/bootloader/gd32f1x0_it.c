/**
  ******************************************************************************
  * @file    gd32f1x0_it.c
  * @author  MCU SD
  * @version V2.0.0
  * @date    15-Jan-2016
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gd32f1x0_it.h"
#include "systick.h"
#include <stdlib.h>
#include <stdio.h> 
#include <stdbool.h>

extern uint8_t	b_onoffflag;
extern int presspowerkeytime;
extern bool maskshake;
uint32_t Time_Delay;
extern void delay_ms(uint16_t ms);


/* Private functions ---------------------------------------------------------*/
/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
     while (1)
     {
     }
    

}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{

}

/**
  * @brief  This function handles External lines 2 to 3 interrupt request. ACC interrupt
  * @param  None
  * @retval None
  */
void EXTI2_3_IRQHandler(void)
{
		/* Reload IWDG counter */
	IWDG_ReloadCounter(); 
	if(EXTI_GetIntBitState(EXTI_LINE3) != RESET)
  {
    /* Clear EXTI line13 bit */
    EXTI_ClearIntBitState(EXTI_LINE3);
		b_onoffflag=1;
  }
} 

/**
  * @brief  This function handles External lines 4 to 15 interrupt request. powerkey interrupt
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{

		/* Reload IWDG counter */
	IWDG_ReloadCounter(); 
	
	if(EXTI_GetIntBitState(EXTI_LINE7) != RESET)
  {
		if(maskshake)
		{
			maskshake=false;
			delay_ms(200);
			/* Clear EXTI line13 bit */
			EXTI_ClearIntBitState(EXTI_LINE7);
		}else
		{
			b_onoffflag=1;
			presspowerkeytime=0;
			delay_ms(200);
		  /* Clear EXTI line13 bit */
			EXTI_ClearIntBitState(EXTI_LINE7);
		}					
  }
	IWDG_ReloadCounter(); 
} 
/******************* (C) COPYRIGHT 2016 GIGADEVICE *****END OF FILE****/
