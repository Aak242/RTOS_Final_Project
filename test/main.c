/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "system_MKL25Z4.h"             // Keil::Device:Startup
#include "MKL25Z4.h"  
#include "constants.h"
//#include "queueFunctions.h"
#include "uart.h"
#include "motors.h"
#include "audio.h"
#include "led.h"

 
 void InitGPIO(void);
 void tBrain(void *argument);
 
 //Q_T Tx_Q, Rx_Q;
 
 osSemaphoreId_t tBrainSem;
osSemaphoreId_t tMotorControlSem;

//mvState currMvState = STOP;


const osThreadAttr_t highPriority = {
	.priority = osPriorityHigh
};

const osThreadAttr_t lowPriority = {
	.priority = osPriorityLow
};


void UART2_IRQHandler(void) {
	NVIC_ClearPendingIRQ(UART2_IRQn);
	if ((UART2->S1 & UART_S1_RDRF_MASK)) {
		data = UART2->D;
		//osSemaphoreRelease(uartSem);
	}
}

//void UART2_IRQHandler(void) 
//{
//	NVIC_ClearPendingIRQ(UART2_IRQn);
//	
//	//IRQ Reciever
//	if ((UART2->S1 & UART_S1_RDRF_MASK) /*&& currMvState != SELFDRIVING*/) 
//	{
//		// received a character
//		// RDRF cleared when reading from UART2->D
//		if (!Q_Full(&Rx_Q)) 
//		{
//			Q_Enqueue(&Rx_Q, UART2->D);
//			osSemaphoreRelease(tBrainSem);
//		} 
//		
//		else 
//		{
//			// error - RX_Q full.
//			// make space by discarding all information in RX_Q (assume it is not needed anymore)
//			Q_Init(&Rx_Q);
//			Q_Enqueue(&Rx_Q, UART2->D);
//		}
//	}
//}

void tBrain(void *argument){
	
	for (;;){
		//osSemaphoreAcquire(uartSem, osWaitForever);
		if(data == ESP32_MOVE_FORWARD) {
			moveForward(100);
		}
		else if(data == ESP32_MOVE_BACK) {
			moveBackward(100);
		}
		else if(data == ESP32_TURN_RIGHT) {
			moveRight(100);
		}
		else if(data == ESP32_TURN_LEFT) {
			moveLeft(100);
		}
		else if(data == ESP32_MOVE_STOP){
			moveStop();
		}
		else if(data == ESP32_VICTORY_MUSIC){
			victoryMusic = true;
		}
	}
}

//void tMotorControl(void *argument)
//{
//	for (;;) 
//	{
//		osSemaphoreAcquire(tMotorControlSem, osWaitForever);
//		
//		switch (currMvState)
//		{
//				case STOP:
//					moveStop();
//					break;
//				case FORWARD:
//					moveForward(100);
//					break;
//				case BACKWARD:
//					moveBackward(100);
//					break;
//				case LEFT:
//					moveLeft(100);
//					break;
//				case RIGHT:
//					moveRight(100);
//					break;
//				default:
//					break;
//		}
//	}
//}
 

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initMotors();
	initUART2();
	PWM_Init();
	initLED();
	//Q_Init(&Rx_Q);
  // ...
	
	
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS

	osThreadNew(flashGreen, NULL, NULL);
	osThreadNew(RedBlink, NULL, NULL);
	osThreadNew(music, NULL, NULL);
	osThreadNew(tBrain, NULL, NULL);
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
