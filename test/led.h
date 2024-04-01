#include "constants.h"
#include "MKL25Z4.h"                    // Device header
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

void initLED(void);
//void flashGreen(void);
//void RedBlink(void);

void flashGreen(void *argument);
void RedBlink(void *argument);

void delay1(volatile uint32_t ms) {
    volatile uint32_t nof = ms * 48000; // For 1ms delay at 48MHz clock
    while(nof != 0) {
        __ASM("NOP");
        nof--;
    }
}

void initLED(void)
{
	// Enable Clock to PORTA, PORTC, PORTD, PORTE
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	// Configure MUX settings to make all pins GPIO
	PORTD->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[RED_LED] |= PORT_PCR_MUX(1);
	
	PORTC->PCR[GREEN_LED_1] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_LED_1] |= PORT_PCR_MUX(1);
	
	PORTC->PCR[GREEN_LED_2] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_LED_2] |= PORT_PCR_MUX(1);
	
	PORTC->PCR[GREEN_LED_3] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_LED_3] |= PORT_PCR_MUX(1);
	
	PORTC->PCR[GREEN_LED_4] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_LED_4] |= PORT_PCR_MUX(1);
	
	PORTA->PCR[GREEN_LED_5] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[GREEN_LED_5] |= PORT_PCR_MUX(1);
	
	PORTA->PCR[GREEN_LED_6] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[GREEN_LED_6] |= PORT_PCR_MUX(1);
	
	PORTE->PCR[GREEN_LED_7] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[GREEN_LED_7] |= PORT_PCR_MUX(1);
	
	PORTD->PCR[GREEN_LED_8] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[GREEN_LED_8] |= PORT_PCR_MUX(1);
	
	// Data Direction Register (Make all to Output)
	PTC->PDDR |= MASK(GREEN_LED_1) | MASK(GREEN_LED_2) | MASK (GREEN_LED_3) | MASK(GREEN_LED_4);
	PTD->PDDR |= MASK(RED_LED) | MASK(GREEN_LED_8);
	PTA->PDDR |= MASK(GREEN_LED_5) | MASK(GREEN_LED_6);
	PTE->PDDR |= MASK(GREEN_LED_7);
}

void flashGreen(void *argument) {
    for (;;) {
        // Turn on each green LED in sequence and then turn it off
        PTC->PCOR = MASK(GREEN_LED_1);
        //delay1(250);
        osDelay(250);
        PTC->PSOR = MASK(GREEN_LED_1);
        
        PTC->PCOR = MASK(GREEN_LED_2);
        //delay1(250);
        osDelay(250);
        PTC->PSOR = MASK(GREEN_LED_2);
        
        PTC->PCOR = MASK(GREEN_LED_3);
        //delay1(250);
        osDelay(250);
        PTC->PSOR = MASK(GREEN_LED_3);
        
        PTC->PCOR = MASK(GREEN_LED_4);
        //delay1(250);
        osDelay(250);
        PTC->PSOR = MASK(GREEN_LED_4);
        
        PTA->PCOR = MASK(GREEN_LED_5);
        //delay1(250);
        osDelay(250);
        PTA->PSOR = MASK(GREEN_LED_5);
        
        PTA->PCOR = MASK(GREEN_LED_6);
        //delay1(250);
        osDelay(250);
        PTA->PSOR = MASK(GREEN_LED_6);
        
        PTE->PCOR = MASK(GREEN_LED_7);
        //delay1(250);
        osDelay(250);
        PTE->PSOR = MASK(GREEN_LED_7);
        
        PTD->PCOR = MASK(GREEN_LED_8);
        //delay1(250);
        osDelay(250);
        PTD->PSOR = MASK(GREEN_LED_8);
    }
}

void RedBlink(void *argument) {
    for (;;) {
        PTD->PCOR = MASK(RED_LED); // Turn on the red LED
        //delay1(250);
        osDelay(250);
        PTD->PSOR = MASK(RED_LED); // Turn off the red LED
        //delay1(250);
        osDelay(250);
    }
}
