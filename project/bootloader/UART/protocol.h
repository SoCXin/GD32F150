
#if !defined( _PROTOCOL_H )

#define _PROTOCOL_H

#include <stdint.h>
#include "gd32f1x0.h"

#define RECEIVE_BUFF_SIZE (1024)   //定义 接收数据包长度


#define PROTOCOL_DATA_SIZE (40)


typedef  struct
{
    uint16_t header ; // 协议头
    uint16_t cmd ;	  // 命令
    uint16_t dLen ;   // 协议包长度

    uint8_t  buf[PROTOCOL_DATA_SIZE]; // 协议中包数据

    uint16_t chk ; //协议包 校验
    uint16_t footer ; // 协议尾

} protocol;

typedef struct {
    uint8_t volatile start ;

    uint8_t  buf[RECEIVE_BUFF_SIZE];

} data_receive ;



#endif