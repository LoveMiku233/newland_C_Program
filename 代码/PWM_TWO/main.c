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
#include "hal_timer_32k.h"

#define LED1 P1_0
#define LED2 P1_1
#define KEY P1_2

//ÊµÏÖË«»÷ PWM
#define KEYTIME 5
uint16 pwm=20;
uint16 pwm_cnt=0;
uint16 pwm_tmp=0;
uint8 pwmflag=0;

uint8 key_cnt=0;
uint16 key_time=0;
uint16 ms=0;

void T1_FUNC(){
  ms++;
  pwm_tmp++;
  if(pwm_tmp<pwm_cnt){
    LED1=0;
  }else if(pwm_tmp<pwm){
    LED1=1;
  }else{
    pwm_tmp=0;
  }
  
  if(ms>=100){
    //@100ms
    ms=0;
    key_time++;
    if(pwmflag&&pwm_cnt++>=20){
      pwm_cnt=0;
    }
  }
  
}

void init(){
  //TIMER1 @1ms
  halTimer32kInit(31);
  halTimer32kIntConnect(T1_FUNC);
  halTimer32kIntEnable();
  //key led init
  P1DIR|=0x03;
  LED1=0;LED2=0;
}

uint8 key_scan(){
  if(!KEY){
    halMcuWaitMs(8);
    if(!KEY){
      pwmflag=1;
      while(!KEY);
      pwmflag=0;
      key_cnt++;
      key_time=0;
    }
  }
  if(key_time>5){
    uint8 tmp=key_cnt;
    key_cnt=0;
    key_time=0;
    return tmp;
  }
  return 0;
}

/********************MAIN************************/
void main(void)
{
    halBoardInit();
    init();
    while(1)
    {
      if(key_scan()>1){
        LED2=!LED2;
      }
    }
}