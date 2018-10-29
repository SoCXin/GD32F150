#ifndef __FMC_H
#define __FMC_H	


#include "gd32f1x0.h"


#define FMC_PAGE_SIZE           ((uint16_t)0x400)

#define AppAddress 0x08006400      // APP Address
#define AppDataArea   0x08009C00			// App Data Area

#define BootAddress   0x08000000      // BootAddress

#define ADDRESS_WRITE_NOW_APP_LEN 0x08009C00 // APP 写入进度 数据的保存地址
#define ADDRESS_WRITE_EXCEPTED_APP_LEN  0x08009C04 // APP 程序总长度 的数据保存地址
#define ADDRESS_WRITE_APP_CHECK_SUM 0x08009C08 // APP 程序的checksum 的数据保存地址
#define ADDRESS_APP_VERSION 0x08009C0C  // App 程序的版本

extern uint32_t write_now_app_len, write_app_len, write_app_checksum,app_version ;
/* 清除App 区域的Flash 中的数据 */
void clearAppArea(void);

void readParamFromData(void) ;

void writeParamToData(void) ;

/*
   启动App
 */
//void startApplication(void) ;

void JumpToBoot(void) ;

/*
		检查App 的状态,实际是比较App的checksum 
		1.比较写入长度和期待长度
		2.比较实际checksum 和 期待checksum
		@return 0 : 表示 App 的状态正常 
		@return 1 : 表示 写入长度 和 期待长度 不一样
		@return 2 : 表示 实际checksum 和 期待checksum 不一样
		@return 3 : 表示 App程序的首位地址不正确.
*/
uint8_t checkAppStatus(void) ;

/*
		向Flash 中写入数据
		@param offset 相对于 AppAddress 的偏移,即指定将数据写入的位置
		@param datas  需要写入到 Flash 中的数据 
		@param len    需要写入到Flash 中的数据的长度
		@return 	0: 	写入成功
		@return 	1: 	写入失败
		
 */
uint8_t writeData(uint32_t offset,uint32_t datas[],uint8_t len) ;

#endif
