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
//#include "UART_PRINT.h"



#define A
//A:长按板A的SW1键不松开
//B:板B D4灯可以实现呼吸灯效果

//双击板B KEY1 板A实现跑马灯

#define KEY P1_2
#define LED_ON(x) halLedSet(x);
#define LED_OFF(x) halLedClear(x);  
#define LED_ALLOFF() for(uint8 i=1;i<5;i++)halLedClear(i);

uint8 pwm_flag=0;
uint8 pwm=90;

uint8 pao_flag=0;
uint8 pao_cnt=0;

uint16 key_time=0;
uint8 key_cnt=0;
  
/*****点对点通讯地址设置******/


#define RF_CHANNEL                12         // 频道 11~26
#define PAN_ID                    0x0022     //网络id 

#ifdef A
#define MY_ADDR                   0x1111     //本机模块地址
#define SEND_ADDR                 0x2222     //发送地址
#else
#define MY_ADDR                   0x2222    //本机模块地址
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

void key_scan(){
#ifdef A //长按
  if(!KEY){
    halMcuWaitMs(8);
    if(!KEY){
      basicRfSendPacket(SEND_ADDR,"1",1);
      while(!KEY);
      basicRfSendPacket(SEND_ADDR,"2",1);
    }
  }
#else
  if(!KEY){
    halMcuWaitMs(8);
    if(!KEY){
      key_cnt++;
      key_time=0;
      while(!KEY);
    }
  }
  if(key_time==5000){
    if(key_cnt>1){
      basicRfSendPacket(SEND_ADDR,"3",1);
    }
    key_cnt=0;
    key_time=0;
  }
#endif
}

void init(){
  P1DIR|=0x1B;
  //关闭LED
  LED_OFF(1);LED_OFF(2);LED_OFF(3);LED_OFF(4);
  //定时器 0.1@ms
  T1CC0L=0x19;
  T1CC0H=0x00;
  T1CCTL0|=0x04;
  T1CTL|=0x0E;
  T1IE=1;
  EA=1;
}

_PRAGMA(vector=T1_VECTOR) __near_func __interrupt void T1_ISR(void){
  T1STAT=0x00;
#ifdef A
  static uint16 ms=0;
  ms++;
  if(ms==10000){
    ms=0;
    if(pao_flag){
      switch(pao_cnt++){
      case 0: LED_ALLOFF();LED_ON(2);break;
      case 1: LED_ALLOFF();LED_ON(1);break;
      case 2: LED_ALLOFF();LED_ON(4);break;
      case 3: LED_ALLOFF();LED_ON(3);break;
      case 4: LED_ALLOFF();pao_flag=0;break;
      }
    }
  }
  
#else
  //pwm
  key_time++;
  if(key_time==10000)key_time=0;
  static uint8 cnt=0;
  cnt++;
  if(cnt<pwm){
    LED_ON(2);
  }else if(cnt<100){
    LED_OFF(2);
  }else{
    if(pwm_flag){
      pwm--;
    }
    if(pwm==0){
      pwm=90;
    }
    cnt=0;
  }
  
#endif
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
      key_scan();
      if(basicRfPacketIsReady()&&basicRfReceive(buff,256,NULL)){
        halUartWrite(buff,strlen(buff));
        if(buff[0]=='1'){
          //pwm on
          pwm_flag=1;
        }else if(buff[0]=='2'){
          //pwm off
          pwm_flag=0;
        }else if(buff[0]=='3'){
          //paoLED on
          pao_flag=1;
          pao_cnt=0;
        }
      }
    /* user code end */
    }
}