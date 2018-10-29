#include "usart.h"


uint8_t u_rx_flag  ; // 首先要立一个flag
USART_RX u_rx;
char recv_data[RX_DATA_LEN];	//data
char write_data[RX_DATA_LEN];	//data
uint8_t recv_index = 0;

#define USART_HEADER  ((uint16_t)0xAFAF)
#define USART_FOOTER  ((uint16_t)0xFAFA)

void USART_Configure(void)
{
    GPIO_InitPara GPIO_InitStructure;
    USART_InitPara USART_InitStructure;

    // 初始化 GPIO

    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE6,GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB,GPIO_PINSOURCE7,GPIO_AF_0);
    GPIO_InitStructure.GPIO_Pin     = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStructure.GPIO_Mode    = GPIO_MODE_AF;
    GPIO_InitStructure.GPIO_OType     = GPIO_OTYPE_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_SPEED_2MHZ;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PUPD_NOPULL;
    GPIO_Init(GPIOB,&GPIO_InitStructure);


    // 设置串口 参数
    USART_DeInit(USART1);
    USART_InitStructure.USART_BRR      = 9600;
    USART_InitStructure.USART_WL       = USART_WL_8B;
    USART_InitStructure.USART_STBits   = USART_STBITS_1;
    USART_InitStructure.USART_Parity   = USART_PARITY_RESET;
    USART_InitStructure.USART_HardwareFlowControl = USART_HARDWAREFLOWCONTROL_NONE;
    USART_InitStructure.USART_RxorTx              = USART_RXORTX_RX | USART_RXORTX_TX;
    USART_Init(USART1,&USART_InitStructure);

    //USART_DEEPSLEEPModeWakeUpSourceConfig(USART1,USART_WAKEUPSOURCE_STARTBIT);

    USART_Enable(USART1,ENABLE);

    //while(USART_GetBitState(USART1,USART_FLAG_REA)==RESET)
    //{}

    //USART_DEEPSLEEPModeEnable(USART1,ENABLE);

    //USART_INT_Set(USART1,USART_INT_WU,ENABLE);
}


void USART1_IRQHandler(void)
{
    char cha ;
    uint8_t i = 0 ;
    uint8_t sum_c=0;

    if(USART_GetIntBitState(USART1, USART_FLAG_ORE) != RESET)
    {
        USART_ClearIntBitState(USART1, USART_FLAG_ORE);
        USART_DataReceive(USART1);
    }
    if(USART_GetIntBitState(USART1,USART_INT_RBNE)!=RESET)
    {
        cha=USART_DataReceive(USART1);
        if(recv_index < RX_DATA_LEN && u_rx_flag == 0) {
            recv_data[recv_index] = cha;
            recv_index ++ ;
            // 接收 到两个字节的时候
            if (recv_index == 1) {
                i = recv_data[0]  ;

                if (i != 0xAF ) {
                    recv_index = 0 ;
                }
                i = 0 ;
            }
            else if (recv_index == 2) {
                sum_c = recv_data[1]  ;
                if(sum_c != 0xAF) {
                    recv_index = 0 ;
                }
                sum_c = 0 ;
            }
        }
        else {
            // 多余的数据就舍弃掉
            //recv_index = 0 ;
        }
        if(recv_index == RX_DATA_LEN) {
						//usart_send(recv_data,recv_index);

            u_rx.header=(recv_data[0]<<8)+recv_data[1];	//帧头

            sum_c = (uint8_t)u_rx.header ;
            sum_c += (uint8_t)(u_rx.header >> 8) ;

            u_rx.dev_type = recv_data[2]; //device type
            sum_c += u_rx.dev_type ;

            u_rx.cmd = recv_data[3]; //cmd
            sum_c += u_rx.cmd ;

            u_rx.len = recv_data[4]; // valid data len
            sum_c += u_rx.len ;


            for(i=0; i < DATA_LEN; i++) {	//循环接收数据：固定长度

                u_rx.data[i]=recv_data[5+i];
                sum_c += u_rx.data[i];
            }

            u_rx.sum = recv_data[5+DATA_LEN];	//sum
            u_rx.footer = (recv_data[6+DATA_LEN]<<8)+recv_data[7+DATA_LEN];	//帧头

            if(u_rx.header == USART_HEADER && u_rx.footer == USART_FOOTER) {


                //u_rx.dev_type = sum_c ;

                if(sum_c == u_rx.sum) {
                    u_rx_flag = 1 ;
                }

                else {
                    // checksum 不正确
                    USART_RX *p = &u_rx ;
                    //usart_send((char*) p,sizeof(USART_RX));
                }


            }


            recv_index = 0 ;
        }


    } else if(USART_GetIntBitState(USART1,USART_INT_WU)==SET)
    {
        USART_ClearIntBitState(USART1,USART_INT_WU);
    }
}


//串口1发送一个字节
void USART1_Sendstr(char *p)
{
	
    while(*p)
    {
        USART_DataSend( USART1, *p );
        while (USART_GetBitState( USART1, USART_FLAG_TBE) == RESET)
        {
        }
        p++;
    }
}

void usart_send(char *usartdat,int length)
{
    int i,j;
    for(i=0; i<length; i++)
    {
        j=8000000;
        while((USART_GetBitState(USART1,USART_FLAG_TC)==RESET)&&(j>0))
        {
            j--;
        }
        if(j>0)
        {
            USART_DataSend(USART1,usartdat[i]);
        } else
        {
            break;
        }
    }
}

/*
	向串口发送回应.
	@param cmd  表示 USART_RX 中的 cmd ,一般为请求命令加一
	@param data 表示 USART_RX 中的 data
  @param len  表示 USART_RX 中的 len ,为有效数据的长度
*/
void usart_send_ack(uint8_t cmd,uint8_t data[],uint8_t len) {

    USART_RX *p = &u_rx ;
    uint8_t sum_c = 0, i = 0;

    u_rx.header = USART_HEADER ;

    sum_c = (uint8_t)u_rx.header ;

    write_data[0] = (uint8_t)u_rx.header ;

    sum_c += (uint8_t)(u_rx.header >> 8) ;

    write_data[1] = (uint8_t)(u_rx.header >> 8);

    sum_c += u_rx.dev_type ;
    write_data[2] = u_rx.dev_type ;

    u_rx.cmd = cmd; //cmd
    sum_c += u_rx.cmd ;
    write_data[3] = u_rx.cmd ;


    u_rx.len = len ;
    sum_c += u_rx.len ;

    write_data[4] = u_rx.cmd ;

    for(i=0; i<32; i++) {	//循环接收数据：固定长度

        if(i >= len) {
            u_rx.data[i] = 0x0;

            write_data[5 + i] = 0x0 ;

        }
        else {
            u_rx.data[i] = data[i];

            write_data[5 + i] = data[i] ;

        }

        sum_c += u_rx.data[i];
    }

    u_rx.sum = sum_c ;

    write_data[37] = u_rx.sum ;


    u_rx.footer = USART_FOOTER ;
    write_data[38] = (uint8_t)(u_rx.footer >> 8);



    write_data[39] = (uint8_t)u_rx.header ;


//usart_send(write_data,40);
    usart_send((char*) p,sizeof(USART_RX));


}


