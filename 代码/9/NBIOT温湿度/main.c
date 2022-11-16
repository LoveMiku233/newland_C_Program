/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  */
#include <string.h>
#include "board.h"
#include "hal_key.h"
#include "tim-board.h"
#include "timer_handles.h"
#include "sht3x.h"
#include "adc_reader.h"
#include "hal_oled.h"

uint8_t mode=0;
/**********************************************************************************************
*函数：void Init( void )
*功能：平台初始化
*输入：无
*输出：无
*特殊说明：无
**********************************************************************************************/
void Init() {
    // 开发板平台初始化
    BoardInitMcu();
    BoardInitPeriph();
    keys_init();//按键初始化
    setTimer2Callback(Time2Handler);
    Tim2McuInit(1);//定时器初始化，设置定时中断1ms中断一次
		OLED_Init();
		OLED_Clear();
		hal_temHumInit();
		ADCS_Init();
}

/**********************************************************************************************
*函数：void KeyDownHandler( void )
*功能：按钮事件监听
*输入：无
*输出：无
*特殊说明：无
**********************************************************************************************/
void KeyDownHandler(void) {
	if(isKey2Pressed()){
		mode=1;
		resetKey2();
	}	
	if(isKey3Pressed()){
		mode=2;
		resetKey3();
	}	
	if(isKey4Pressed()){
		mode=0;
		resetKey4();
	}	
}

/**********************************************************************************************
*函数：void handlerPre10Ms( void )
*功能：10毫秒循环
*输入：无
*输出：无
*特殊说明：循环处理总时长300ms
**********************************************************************************************/
void handlerPre10Ms(void) {
    for (int delay = 0; delay < 30; delay++) {
        HAL_Delay(10);
        
    }
}
float light,temp,hum;
uint8_t dat[20];
void display(){
	OLED_Clear();
	call_sht11(&temp,&hum);
	if(mode==0){
		OLED_ShowCHinese(10+16*0,1,52);
		OLED_ShowCHinese(10+16*1,1,53);
		OLED_ShowCHinese(10+16*2,1,54);
		OLED_ShowCHinese(10+16*3,1,55);
		OLED_ShowCHinese(10+16*4,1,56);
		OLED_ShowChar(10+16*5,1,':');
		sprintf(dat,"%dC , %d%%",(uint16_t)temp,(uint16_t)hum);
	}else if(mode==1){
		OLED_ShowCHinese(10+16*0,1,52);
		OLED_ShowCHinese(10+16*1,1,53);
		OLED_ShowCHinese(10+16*2,1,54);
		OLED_ShowCHinese(10+16*3,1,56);
		OLED_ShowChar(10+16*4,1,':');
		sprintf(dat,"%dC",(uint16_t)temp);
	}else if(mode==2){
		OLED_ShowCHinese(10+16*0,1,52);
		OLED_ShowCHinese(10+16*1,1,53);
		OLED_ShowCHinese(10+16*2,1,55);
		OLED_ShowCHinese(10+16*3,1,56);
		OLED_ShowChar(10+16*4,1,':');
		sprintf(dat,"%d%%",(uint16_t)hum);
	}
	OLED_ShowString(10,4,dat);
}

/**
 * Main application entry point.
 */
int main( void )
{
    Init();
    while( 1 )
    {
			display();
			HAL_Delay(500);
			KeyDownHandler();
    }
}
