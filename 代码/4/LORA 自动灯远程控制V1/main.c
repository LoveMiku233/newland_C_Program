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
#include "NS_Radio.h"
#include "sht3x.h"
#include "adc_reader.h"


//开灯 o1,关灯 o2 
#define LORA 433000000
#define A
uint8_t pos=1;
uint8_t mode=0;
uint8_t led_flag=0;

#define LED_ON() {led_flag=1; GpioWrite( &Led1, 0 ); GpioWrite( &Led2, 0 );}
#define LED_OFF() {led_flag=0; GpioWrite( &Led1, 1 ); GpioWrite( &Led2, 1 );}
/**********************************************************************************************
*函数：void Init( void )
*功能：平台初始化
*输入：无
*输出：无
*特殊说明：无
**********************************************************************************************/
uint8_t bmp[]={
0x00,0x00,0xE0,0xF0,0x78,0x38,0x98,0xDC,0xDC,0xDC,0xDC,0x98,0xB8,0x78,0x70,0xE0,
0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x19,0x1D,0x5D,0xDD,0x5D,0x1D,
0x13,0x03,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

void display_pos(){
	OLED_ShowChar(90,2,' ');
	OLED_ShowChar(90,4,' ');
	OLED_ShowChar(90,6,' ');
	OLED_ShowChar(90,pos*2,'<');
}

void display(){
	//A
	OLED_Clear();
	#ifdef A
	OLED_ShowString(30,0,"Menu");
	OLED_ShowString(0,2,"1.Auto Lamp");
	OLED_ShowString(0,4,"2.Muta Lamp");
	OLED_ShowString(0,6,"3.View Freq");
	display_pos();
	#else
	OLED_ShowString(20,0,"Select Menu");
	OLED_DrawBMP(60,4,20,20,bmp);
	#endif
}

void display_auto(){
	OLED_Clear();
	OLED_ShowString(20,0,"Auto Lamp");
	//call_sht11
}


void Init() {
    // 开发板平台初始化
    BoardInitMcu();
    BoardInitPeriph();
    keys_init();//按键初始化
    setTimer2Callback(Time2Handler);
    Tim2McuInit(1);//定时器初始化，设置定时中断1ms中断一次
		
		//oled init
		OLED_Init();
		OLED_Clear();
		NS_RadioInit(LORA,14,1000,1000);
		ADCS_Init();
		hal_temHumInit();
}

void display_view(){
	OLED_Clear();
	uint8_t te[30];
	sprintf(te,"Freq:%d",LORA);
	OLED_ShowString(30,0,"View Freq");
	OLED_ShowString(0,2,te);
	#ifdef A
	OLED_DrawBMP(0,4,20,20,bmp);
	#endif
}

void display_led(){
	OLED_Clear();
	OLED_ShowString(30,0,"Mula Lamp");
	if(led_flag){
		OLED_ShowString(0,2,"Lamp:Opon ");
	}else{
		OLED_ShowString(0,2,"Lamp:Close");
	}
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
		if(mode==0){
			if(pos!=1){
				pos--;
				display_pos();
			}
		}else if(mode==2){
			LED_ON();
			display_led();
			SX1276Send("o1",2);
		}
		resetKey2();
	}
	if(isKey3Pressed()){
		if(mode==0){
			if(pos!=3){
				pos++;
				display_pos();
			}
		}else if(mode==2){
			LED_OFF();
			display_led();
			SX1276Send("o2",2);
		}
		resetKey3();
	}
	if(isKey4Pressed()){
		if(mode==0){
			if(pos==1){
				display_auto();
				mode=1;
			}else if(pos==2){
				display_led();
				mode=2;
				SX1276Send("2",2);
			}else if(pos==3){
				display_view();
				mode=3;
				SX1276Send("3",2);
			}
		}else{
			mode=0;
			display();
			SX1276Send("0",2);
		}
		resetKey4();
	}
}

uint8_t data[256];
void read(){
	if(ReadRadioRxBuffer(data)>0){
		if(data[0]=='0'){
			display();
			mode=0;
		}else if(data[0]=='2'){
			display_led();
			mode=2;
		}else if(data[0]=='3'){
			display_view();
			mode=3;
		}else if(data[0]=='L'){
			mode=1;
			OLED_ShowString(20,0," Auto Lamp   ");
			OLED_ShowString(0,2,data);
		}else if(data[0]=='T'){
			OLED_ShowString(0,4,data);
		}else if(data[0]=='H'){
			OLED_ShowString(0,6,data);
		}else if(data[0]=='o'){
			if(data[1]=='1'){
				LED_ON();
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_SET);
			}else if(data[1]=='2'){
				LED_OFF();
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_RESET);
			}
			if(mode==2){
				display_led();
			}
		}
		memset(data,'\0',256);
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


/**
 * Main application entry point.
 */
int main( void )
{
    Init();
		display();
    while( 1 )
    {
			#ifdef A
			KeyDownHandler();
			#else
			read();
			#endif
    }
}
