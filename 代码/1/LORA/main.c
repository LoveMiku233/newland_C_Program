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
*������void Init( void )
*���ܣ�ƽ̨��ʼ��
*���룺��
*�������
*����˵������
**********************************************************************************************/
void Init() {
    // ������ƽ̨��ʼ��
    BoardInitMcu();
    BoardInitPeriph();
    keys_init();//������ʼ��
    setTimer2Callback(Time2Handler);
    Tim2McuInit(1);//��ʱ����ʼ�������ö�ʱ�ж�1ms�ж�һ��
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
*������void KeyDownHandler( void )
*���ܣ���ť�¼�����
*���룺��
*�������
*����˵������
**********************************************************************************************/
void KeyDownHandler(void) {

}

/**********************************************************************************************
*������void handlerPre10Ms( void )
*���ܣ�10����ѭ��
*���룺��
*�������
*����˵����ѭ��������ʱ��300ms
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
