/* Includes ------------------------------------------------------------------*/
#include "timer_handles.h"
#include "key_scaner.h"
#include "usart1-board.h"
void Time2Handler(){
	keyScanner();
	//USART1_IRQHandler();
	USART1_Process();
}

void Time3Handler(){
}
