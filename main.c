#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "system_MKL25Z4.h"             // Keil::Device:Startup
#include "MKL25Z4.h"  
#include "constants.h"
#include "uart.h"
#include "motors.h"
#include "audio.h"
#include "led.h"

//osSemaphoreId_t static uartSem;
void tBrain(void *argument);

void UART2_IRQHandler(void) {
	NVIC_ClearPendingIRQ(UART2_IRQn);
	if ((UART2->S1 & UART_S1_RDRF_MASK)) {
		data = UART2->D;
		//osSemaphoreRelease(uartSem);
	}
}

void tBrain(void *argument){
	
	for (;;){
		//osSemaphoreAcquire(uartSem, osWaitForever);
		if(data == ESP32_MOVE_FORWARD) {
			moveForward(100);
		}
		else if(data == ESP32_MOVE_FORWARD) {
			moveBackward(100);
		}
		else if(data == ESP32_TURN_RIGHT) {
			moveLeft(100);
		}
		else if(data == ESP32_TURN_LEFT) {
			moveRight(100);
		}
		else {
			moveStop();
		}
	}
}


int main (void) {
	SystemCoreClockUpdate();
	initMotors();
	initUART2();
	PWM_Init();
	initLED();
	//music();
	
		
		osKernelInitialize();
	
		//uartSem = osSemaphoreNew(1,1,NULL);
	
		osThreadNew(flashGreen, NULL, NULL);
		osThreadNew(RedBlink, NULL, NULL);
	  osThreadNew(music, NULL, NULL);
	  osThreadNew(tBrain, NULL, NULL);
		osKernelStart();
	
		while(1) {
			//music();
		} 
		
/*
	while (1) {
		if(data == ESP32_MOVE_FORWARD) {
			moveForward(100);
		}
		else if(data == ESP32_MOVE_FORWARD) {
			moveBackward(100);
		}
		else if(data == ESP32_TURN_RIGHT) {
			moveLeft(100);
		}
		else if(data == ESP32_TURN_LEFT) {
			moveRight(100);
		}
		
		else {
			moveStop();
			
		}
	}
	
	for(;;) {
		flashGreen();
		RedBlink();
	}
	*/
}


