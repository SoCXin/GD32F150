#include "fmc.h"
#include "usart.h"


volatile FMC_State FMCStatus = FMC_READY ;
__IO uint32_t NbrOfPage = 0x00;
uint32_t EraseCounter = 0x00,address = 0x0;
uint32_t *ptrd;
uint32_t write_now_app_len, write_app_len, write_app_checksum,app_version ;

void clearAppArea() {
    /* Unlock the Flash Bank1 Program Erase controller */
    FMC_Unlock();

    /* Define the number of page to be erased */
    NbrOfPage =  24 ;
    /* Clear All pending flags */
    FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );

    /* Erase the FLASH pages */
    for(EraseCounter = 0; EraseCounter < NbrOfPage; EraseCounter++)
    {
        FMC_ErasePage(AppAddress + (FMC_PAGE_SIZE * EraseCounter));
        FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );
    }

    FMC_Lock();
}


void readParamFromData() {
    // 实际写入长度
    ptrd = (uint32_t*)(ADDRESS_WRITE_NOW_APP_LEN);
    write_now_app_len = *ptrd ;
    //期待写入长度
    ptrd = (uint32_t*)(ADDRESS_WRITE_EXCEPTED_APP_LEN);
    write_app_len = *ptrd ;
    // 期待 checksum
    ptrd = (uint32_t*)(ADDRESS_WRITE_APP_CHECK_SUM);
    write_app_checksum = *ptrd ;
//
    ptrd = (uint32_t *)(ADDRESS_APP_VERSION) ;
    app_version = *ptrd ;

}

void writeParamToData() {

    /* Unlock the Flash Bank1 Program Erase controller */
    FMC_Unlock();

    /* Clear All pending flags */
    FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );

    FMC_ErasePage(AppDataArea);
    FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );

    FMCStatus = FMC_ProgramWord(ADDRESS_WRITE_NOW_APP_LEN,write_now_app_len);

    FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );


    FMCStatus = FMC_ProgramWord(ADDRESS_WRITE_EXCEPTED_APP_LEN,write_app_len);

    FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );

    FMCStatus = FMC_ProgramWord(ADDRESS_WRITE_APP_CHECK_SUM,write_app_checksum);

    FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );
    FMCStatus = FMC_ProgramWord(ADDRESS_APP_VERSION,app_version);

    FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );

    FMC_Lock();


}

/*
		检查App 的状态,实际是比较App的checksum
		1.比较写入长度和期待长度
		2.比较实际checksum 和 期待checksum
		@return 0 : 表示 App 的状态正常
		@return 1 : 表示 写入长度 和 期待长度 不一样
		@return 2 : 表示 实际checksum 和 期待checksum 不一样
		@return 3 : 表示 App程序的首位地址不正确.
*/
uint8_t checkAppStatus(void) {

    uint32_t sum_c = 0x0,address = AppAddress;
	
	
    // 首先从 flash 中读取 数据
    readParamFromData() ;
	
    // 如果 实际写入长度 和 期待写入长度 不同的话.不用继续了
    if(write_app_len != write_now_app_len || write_app_len == 0xFFFFFFFF || write_now_app_len == 0xFFFFFFFF) {

        return 1 ;
    }
		
    // 循环取值,然后累加
    for (address = AppAddress; address < (AppAddress + write_app_len) ; address+=4 ) {
        ptrd = (uint32_t*)(address);
        sum_c += *ptrd ;
    }


    // 如果 checksum 不对的话...那就不对呗
    if(sum_c != write_app_checksum || write_app_checksum == 0xFFFFFFFF) {

        return 2;
    }
		

		// 如果 App 的首位地址没有问题的话
		if (((*(__IO uint32_t*)AppAddress) & 0x20000000 ) != 0x20000000) {

        return 3 ;
    }
		
    return 0 ;
}



/*
typedef void (*pFonction)(void);
pFonction startApp;
uint32_t JumpAddress;

void startApplication(void)
{
    if (((*(__IO uint32_t*)AppAddress) & 0x20000000 ) == 0x20000000) {
        JumpAddress = *(__IO uint32_t*) (AppAddress + 4);
        startApp = (pFonction) JumpAddress;
        __set_MSP(*(__IO uint32_t*) AppAddress);
        __enable_irq();     //PRIMASK=1,关全局中断
			
			
				//USART1_Sendstr(" -- jump success \r\n");
			
        startApp();
    }
		else {
			USART1_Sendstr(" -- jump failed \r\n");
			
		}
}
*/
typedef void (*pFonction)(void);
pFonction JumpToApp;
uint32_t JumpAddress;

void JumpToBoot(void)
{
    if (((*(__IO uint32_t*)BootAddress) & 0x20000000 ) == 0x20000000) {
        JumpAddress = *(__IO uint32_t*) (BootAddress + 4);
        JumpToApp = (pFonction) JumpAddress;
        __set_MSP(*(__IO uint32_t*) BootAddress);
        __enable_irq();     //PRIMASK=1,关全局中断

        //USART1_Sendstr("Jump to Boot application\n\r");
        JumpToApp();
    }
}



/*
		向Flash 中写入数据
		@param offset 相对于 AppAddress 的偏移,即指定将数据写入的位置
		@param datas  需要写入到 Flash 中的数据 
		@param len    需要写入到Flash 中的数据的长度
		@return 	0: 	写入成功
		@return 	1: 	写入失败
		
 */
uint8_t writeData(uint32_t offset,uint32_t datas[],uint8_t len) {
    uint32_t temp0 = 0x0,temp1 = 0x0 ;
    uint8_t i = 0x0,writecount = 0 ;

    // 写入数据
    FMC_Unlock();
    //Address = 0x00 ;
    address = AppAddress + offset  ;
    for (i= 0 ; i < len ; i++ ) {
        temp0 = datas[i] ;

        //FMC_ProgramWord(AppAddress + writeIndex + Address * ((uint16_t)4),tmp);
        FMCStatus = FMC_ProgramWord(address,temp0);
        address = address + 4 ;
        FMC_ClearBitState(FMC_FLAG_EOP | FMC_FLAG_WERR | FMC_FLAG_PERR );

    }

    FMC_Lock();

    // 校验数据 是否写入成功
    ptrd = (uint32_t*)(AppAddress + offset );
    for (i= 0 ; i < len ; i++ ) {

        temp0 = datas[i] ;
        temp1 = *ptrd ;
        if(temp0 != temp1)
        {
            writecount = 1 ;
            break;
        }
        ptrd++;
    }
		
		//将当前写的进度更新
		readParamFromData();
		
		write_now_app_len = ( offset + len * 4 );
		
		writeParamToData();
		
		
    return writecount ;


}





