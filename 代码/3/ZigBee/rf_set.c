#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_int.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_led.h"
#include "hal_rf.h"
#include "basic_rf.h"
#include "hal_uart.h" 
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define KEY P1_2
#define RALAY P2_0

/*****点对点通讯地址设置******/
#define A

#define RF_CHANNEL                11         // 频道 11~26
#define PAN_ID                    0x1111     //网络id 

#ifdef A
#define MY_ADDR                   0x1111     //本机模块地址
#define SEND_ADDR                 0x2222     //发送地址
#else
#define MY_ADDR                   0x2222     //本机模块地址
#define SEND_ADDR                 0x1111     //发送地址
#endif
/**************************************************/
static basicRfCfg_t basicRfConfig;
// 无线RF初始化
void ConfigRf_Init(void)
{
  
    basicRfConfig.panId       =   PAN_ID;
    basicRfConfig.channel     =   RF_CHANNEL;
    basicRfConfig.myAddr      =   MY_ADDR;
    basicRfConfig.ackRequest  =   TRUE;
    while(basicRfInit(&basicRfConfig) == FAILED);
    basicRfReceiveOn();
 
}


void init(){
  P1DIR|=0x1b;
  P1&=~0x1b;
  P2DIR|=0x01;
  RALAY=0;
}

void key_scan(){
  if(!KEY){
    halMcuWaitMs(8);
    if(!KEY){
      basicRfSendPacket(SEND_ADDR,"1",1);
      while(!KEY);
    }
  }
}

uint8 buff[256];
/********************MAIN************************/
void main(void)
{
    halBoardInit();//选手不得在此函数内添加代码
    ConfigRf_Init();//选手不得在此函数内添加代码
    init();
    while(1)
    {
    /* user code start */
#ifdef A
      key_scan();
#else
      if(basicRfPacketIsReady()&&basicRfReceive(buff,256,NULL)){
        if(buff[0]=='1'){
          RALAY=!RALAY;
        }
      }
#endif
      
       
    /* user code end */
    }
}