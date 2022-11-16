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
#include "hal_oled.h"
#include "usart1-board.h"
#include "flash.h"

uint32_t date[3]={2021,10,11};
uint8_t pos=0;
uint8_t mode=0;
#define ADDR 0x0800F000
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
		USART1_Init(115200);
}

void save(){
	STMFLASH_Write(ADDR,date,3);
}

void read(){
	STMFLASH_Read(ADDR,date,3);
	if(date[0]==0||date[1]==0||date[2]==0){
		date[0]=2021;
		date[1]=10;
		date[2]=11;
	}
}

extern uint16_t USART1_RX_COUNT;	
extern bool F_USART1_RX_FINISH;
extern uint8_t USART1_RX_BUF[];
void usart_(){
	if(USART1_RX_COUNT>0&&F_USART1_RX_FINISH){
		if(USART1_RX_BUF[0]==0xfb&&(USART1_RX_BUF[3]==0xfe||USART1_RX_BUF[4]==0xfe)){
			if(USART1_RX_BUF[1]==0x01){
				date[0]=USART1_RX_BUF[2]*100+USART1_RX_BUF[3];
				save();
				printf("%c%c%c",0xfb,0x00,0xfe);
			}else if(USART1_RX_BUF[1]==0x02){
				if(USART1_RX_BUF[2]>0){
					date[1]=USART1_RX_BUF[2];
					save();
					printf("%c%c%c",0xfb,0x00,0xfe);
				}else{
					printf("%c%c%c",0xfb,0x01,0xfe);
				}
			}else if(USART1_RX_BUF[1]==0x02){
				if(USART1_RX_BUF[2]>0){
					date[1]=USART1_RX_BUF[2];
					save();
					printf("%c%c%c",0xfb,0x00,0xfe);
				}else{
					printf("%c%c%c",0xfb,0x01,0xfe);
				}
			}
		}else{
			printf("%c%c%c",0xfb,0x01,0xfe);
		}
		USART1_ReceiveClr();
	}
}

uint8_t temp[256];
void display_main(){
	OLED_Clear();
	OLED_ShowCHinese(0+16*0,0,52);
	OLED_ShowCHinese(0+16*1,0,53);
	OLED_ShowCHinese(0+16*2,0,54);
	OLED_ShowCHinese(0+16*3,0,55);
	OLED_ShowChar(0+16*4,0,':');
	
	sprintf(temp,"%04d",date[0]);
	OLED_ShowString(0,3,temp);
	OLED_ShowCHinese(0+5*8,3,56);
	
	sprintf(temp,"%02d",date[1]);
	OLED_ShowString(0,6,temp);
	OLED_ShowCHinese(0+2*8,6,57);
	
	sprintf(temp,"%02d",date[2]);
	OLED_ShowString(40,6,temp);
	OLED_ShowCHinese(40+2*8,6,58);
}

void display_pos(){
	OLED_ShowChar(80,0,' ');
	OLED_ShowChar(80,3,' ');
	OLED_ShowChar(80,6,' ');
	
	OLED_ShowChar(80,pos*3,'<');
}

void display_edit(){
	OLED_Clear();
	display_pos();
	if(mode==2){
		OLED_ShowChar(100,pos*3,'@');
	}
	sprintf(temp,"%04d",date[0]);
	OLED_ShowString(40,0,temp);
	OLED_ShowCHinese(0,0,56);
	
	
	sprintf(temp,"%02d",date[1]);
	OLED_ShowString(40,3,temp);
	OLED_ShowCHinese(0,3,57);
	
	sprintf(temp,"%02d",date[2]);
	OLED_ShowString(40,6,temp);
	OLED_ShowCHinese(0,6,58);
	
	OLED_ShowChar(16,0,':');
	OLED_ShowChar(16,3,':');
	OLED_ShowChar(16,6,':');
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
		if(mode==1){
			if(pos!=0){
				pos--;
				display_edit();
			}
		}else if(mode==2){
			if(pos==0&&date[0]!=0){
				date[0]--;
			}else if(pos==1&&date[1]!=0){
				date[1]--;
			}else if(pos==2&&date[2]!=0){
				date[2]--;
			}
			display_edit();
		}
		resetKey2();
	}	
	if(isKey3Pressed()){
		if(mode==1){
			if(pos!=2){
				pos++;
				display_edit();
			}
		}else if(mode==2){
			if(pos==0){
				date[0]++;
			}else if(pos==1&&date[1]!=12){
				date[1]++;
			}else if(pos==2&&date[2]!=31){
				date[2]++;
			}
			display_edit();
		}
		resetKey3();
	}	
	if(isKey4Pressed()){
		if(mode==0){
			mode=1;
			display_edit();
		}else if(mode==1){
			mode=2;
			OLED_ShowChar(100,pos*3,'@');
			GpioWrite( &Led2, 0 );
		}else if(mode==2){
			OLED_ShowChar(100,pos*3,' ');
			GpioWrite( &Led2, 1 );
			mode=1;
			save();
		}
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


/**
 * Main application entry point.
 */
int main( void )
{
    Init();
		read();
		display_main();
		printf("test");
    while( 1 )
    {
			usart_();
			KeyDownHandler();
    }
}
