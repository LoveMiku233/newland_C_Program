/* Includes ------------------------------------------------------------------*/
#include "timer_handles.h"
#include "key_scaner.h"

#include "hal_oled.h"
#include "NS_Radio.h"
#include "sht3x.h"
#include "adc_reader.h"

#define A
float temp,hum,light;
extern uint8_t mode;
uint8_t cnt=1; //每次只发送一行
uint8_t buff[256];
extern uint8_t led_flag;

void Time2Handler(){
	#ifdef A
	keyScanner();
	static uint16_t ms=0;
	ms++;
	if(ms==500){
		ms=0;
		if(mode==1){
			//获取温湿度
			call_sht11(&temp,&hum);
			//获取光照
			AdcScanChannel();
			light=AdcReadCh0()*50;
			if(light<40){
				led_flag=1;
				SX1276Send("o1",2);
				GpioWrite( &Led1, 0 ); GpioWrite( &Led2, 0 );
			}else{
				led_flag=0;
				SX1276Send("o2",2);
				GpioWrite( &Led1, 1 ); GpioWrite( &Led2, 1 );
			}
			if(cnt==1){
				sprintf(buff,"Light:%0.2f",light);
			}else if(cnt==2){
				sprintf(buff,"Temp:%0.2f",temp);
			}else if(cnt==3){
				sprintf(buff,"Humi:%0.2f",hum);
			}else if(cnt==4){
				if(light<40){
					led_flag=1;
					SX1276Send("o1",2);
					GpioWrite( &Led1, 0 ); GpioWrite( &Led2, 0 );
				}else{
					led_flag=0;
					SX1276Send("o2",2);
					GpioWrite( &Led1, 1 ); GpioWrite( &Led2, 1 );
				}
			}
			if(cnt<4){
				OLED_Clear_Page(cnt*2);
				OLED_ShowString(0,cnt*2,buff);
				SX1276Send(buff,strlen(buff));
			}
			cnt++;
			if(cnt==5){
				cnt=1;
			}
		}
	}
	#else
	
	
	#endif
}

void Time3Handler(){
}
