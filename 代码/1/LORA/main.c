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

#include "adc_reader.h"
#include "hal_oled.h"
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
		ADCS_Init();
		OLED_Init();
		OLED_Clear();
		GpioWrite( &Led1, 0 );
		GpioWrite( &Led2, 1 );
}
float light=0;
uint8_t buff[100];
void display(){
	OLED_Clear();
	AdcScanChannel();
	light=AdcReadCh0();
	if(light<1){
		GpioWrite( &Led2, 0 );
	}else{
		GpioWrite( &Led2, 1 );
	}
	OLED_ShowCHinese(10,3,52);
	OLED_ShowCHinese(10+16*1,3,53);
	OLED_ShowCHinese(10+16*2,3,54);
	OLED_ShowChar(10+16*3,3,':');
	sprintf(buff,"%03dLx",(uint16_t)(light*100));
	OLED_ShowString(10+16*4,3,buff);
}

/**********************************************************************************************
*函数：void KeyDownHandler( void )
*功能：按钮事件监听
*输入：无
*输出：无
*特殊说明：无
**********************************************************************************************/
void KeyDownHandler(void) {

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


/**
 * Main application entry point.
 */
int main( void )
{
    Init();
    while( 1 )
    {
			HAL_Delay(300);
			display();
    }
}
