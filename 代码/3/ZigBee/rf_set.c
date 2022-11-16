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

/*****��Ե�ͨѶ��ַ����******/
#define A

#define RF_CHANNEL                11         // Ƶ�� 11~26
#define PAN_ID                    0x1111     //����id 

#ifdef A
#define MY_ADDR                   0x1111     //����ģ���ַ
#define SEND_ADDR                 0x2222     //���͵�ַ
#else
#define MY_ADDR                   0x2222     //����ģ���ַ
#define SEND_ADDR                 0x1111     //���͵�ַ
#endif
/**************************************************/
static basicRfCfg_t basicRfConfig;
// ����RF��ʼ��
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
    halBoardInit();//ѡ�ֲ����ڴ˺�������Ӵ���
    ConfigRf_Init();//ѡ�ֲ����ڴ˺�������Ӵ���
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