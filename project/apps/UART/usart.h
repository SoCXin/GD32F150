#ifndef __USART_H
#define __USART_H	

#include "gd32f1x0.h"



#define CMD_CHECK_VERSION (uint8_t)0xb0  /* 检查版本 */
#define RESP_CHECK_VERSION (uint8_t)0xb1 /* 检查版本的响应结果 */

#define CMD_CHECK_STATUS (uint8_t)0xb2 /* 检查程序状态 */
#define RESP_CHECK_STATUS (uint8_t)0xb3 /* 检查程序状态的响应结果 */

#define CMD_START_APP (uint8_t)0xb4 /* 跳转到App */
#define RESP_START_APP (uint8_t)0xb5 /* 跳转到App的响应结果 */

// BOOT 程序中应该不会用到这个命令
#define CMD_GOTO_BOOT (uint8_t)0xb6 /* 跳转到BOOT */
#define RESP_GOTO_BOOT (uint8_t)0xb7 /* 跳转到BOOT 的响应结果 */

#define CMD_EARSE_APP (uint8_t)0xb8 /* 擦除 APP 的FLASH 区域 */
#define RESP_EARSE_APP (uint8_t)0xb9 /* 擦除APP 的FLASH 区域的返回结果 */

#define CMD_WRITE_DATA (uint8_t)0xba /* 串口写数据 */
#define RESP_WRITE_DATA (uint8_t)0xbb /* 串口写数据的响应结果 */

#define CMD_SET_PARAM (uint8_t)0xbc /* 设置参数( App 的大小,checksum ,App 版本 )*/
#define RESP_SET_PARAM (uint8_t)0xbd /* 设置 参数的响应结果 */

#define CMD_FETCH_DOG (uint8_t)0xbe  /* 喂狗,同时设置喂狗时间 */
#define RESP_FETCH_DOG (uint8_t)0xbf  /* 喂狗响应 */

#define STATUS_COUNTER_DOWN (uint8_t)0xa0  /* counter_down 状态 */
#define STATUS_APP (uint8_t)0xa1  /* counter_down 状态 */

#define RESP_REBOOT (uint8_t)0xa2  /* counter_down 状态 */



#define RESP_BEFORE_PW_RESET (uint8_t)0xa3  /* counter_down 状态 */

#define RESP_BEFORE_PW_SET (uint8_t)0xa4  /* counter_down 状态 */


#define RESP_BEFORE_PWKEY_RESET (uint8_t)0xa5  /* counter_down 状态 */

#define RESP_BEFORE_PWKEY_SET (uint8_t)0xa6  /* counter_down 状态 */


#define DATA_LEN 32 
#define RX_DATA_LEN sizeof(USART_RX) 

typedef struct {
	
	// 稳定40个字节
	// 数据内容
	uint16_t header ; // header
	uint8_t dev_type ; // device type
	uint8_t cmd ; // cmd
	uint8_t len ; //valid data len 

	uint8_t data[DATA_LEN];	//data
	
	uint8_t sum ; // checksum
	
	uint16_t footer ;// footer
	
} USART_RX ;




extern USART_RX u_rx;


extern	uint8_t u_rx_flag  ; // 首先要立一个flag
	
void USART_Configure(void);


//串口1发送一个字节
void USART1_Sendstr(char *p);

void usart_send(char *usartdat,int length) ;

/* 
	向串口发送回应.
	@param cmd  表示 USART_RX 中的 cmd ,一般为请求命令加一
	@param data 表示 USART_RX 中的 data 
  @param len  表示 USART_RX 中的 len ,为有效数据的长度 
*/
void usart_send_ack(uint8_t cmd,uint8_t data[],uint8_t len);


#endif
