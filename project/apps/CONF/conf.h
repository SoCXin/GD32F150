#include <stdbool.h>
#include "gd32f1x0.h"

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


void delay1us(int us);

void delay_us(uint16_t delay);


void delay_ms(uint16_t delay);
void RCC_Configuration(void);

void GPIO_BootConfiguration(void);

void NVIC_Configuration(void); 
void ADC_Configuration(void);

int RTC_ALARM_PreConfig(bool RST);

void Delayus_Configuration(void);

