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

#include "hal_oled.h"
#include "flash.h"

#define ADDR 0x0800F000
uint32_t data[2]={0,2333};
void display();
void read_num();

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
		OLED_Init();
		read_num();
		display();
}


void save_num(){
	STMFLASH_Write(ADDR,data,2);
}

void read_num(){
	uint32_t temp[2]={0,0};
	STMFLASH_Read(ADDR,temp,2);
	if(temp[1]==2333){
		data[0]=temp[0];
	}
}
uint8_t buff[256];
void display(){
	OLED_Clear();
	OLED_ShowCHinese(40,0,52);
	OLED_ShowCHinese(40+16,0,53);
	OLED_ShowCHinese(40+32,0,54);
	OLED_ShowCHinese(20,3,55);
	OLED_ShowCHinese(20+16,3,56);
	OLED_ShowChar(20+32,3,':');
	sprintf(buff,"%d",data[0]);
	OLED_ShowString(70,3,buff);
	
	OLED_ShowCHinese(20,6,57);
	OLED_ShowCHinese(20+16,6,58);
	OLED_ShowChar(20+32,6,':');
	sprintf(buff,"%d",data[0]*2);
	OLED_ShowString(70,6,buff);
}

/**********************************************************************************************
*������void KeyDownHandler( void )
*���ܣ���ť�¼�����
*���룺��
*�������
*����˵������
**********************************************************************************************/
void KeyDownHandler(void) {
	if(isKey2Pressed()){
		data[0]--;
		display();
		resetKey2();
	}
	if(isKey3Pressed()){
		data[0]++;
		display();
		resetKey3();
	}
	if(isKey4Pressed()){
		GpioWrite( &Led2, 0 );
		HAL_Delay(1000);
		GpioWrite( &Led2, 1 );
		save_num();
		resetKey4();
	}
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
			KeyDownHandler();
				
    }
}
