#include "MKL25Z4.h" 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "system_MKL25Z4.h"             // Keil::Device:Startup
#include "MKL25Z4.h"                    // Device header
#include "stdio.h"
#include "stdbool.h"
#include "constants.h"


static volatile uint8_t data;
void UART2_IRQHandler(void);

void initUART2(void);
/* Init UART2 */
void initUART2()
{
	uint32_t bus_clock, divisor;
	//enable clock to UART2 and PORTE
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	PORTE->PCR[UART_RX] = 0;
	//connect UART pins for PORTE22, do for 23
	PORTE->PCR[UART_TX] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_TX] |= PORT_PCR_MUX(4);
	PORTE->PCR[UART_RX] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_RX] |= PORT_PCR_MUX(4);
	PORTE->PCR[UART_RX] |= (0x0B << 16);
	
	//ensure Tx and Rx disabled
	UART2->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK | UART_C2_TIE_MASK | UART_C2_RIE_MASK);
	
	//set baud rate
	bus_clock = (DEFAULT_SYSTEM_CLOCK) / 2;
	divisor = bus_clock / (BAUD_RATE * 16);
	UART2_BDH = UART_BDH_SBR(divisor >> 8);
	UART2_BDL = UART_BDL_SBR(divisor);
	
	UART2->C1 = 0;
	UART2->S2 = 0;
	UART2->C2 = 0;
	UART2->C3 = 0;
	
	//Clear & enable Interupts in UART2
	//Priority is set at 128
	NVIC_SetPriority(UART2_IRQn, 128);
	NVIC_ClearPendingIRQ(UART2_IRQn);
	NVIC_EnableIRQ(UART2_IRQn);		
	
	//Enable Tx and Rx
	UART2->C2 |= UART_C2_RE_MASK; //(UART_C2_TE_MASK | UART_C2_RE_MASK);
	//Tx/Rx Interrupt Enable
	UART2->C2 |= UART_C2_RIE_MASK; //(UART_C2_TIE_MASK | UART_C2_RIE_MASK);
	
}
