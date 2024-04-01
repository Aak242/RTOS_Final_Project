#include "RTE_Components.h"
#include  CMSIS_device_header
#include "system_MKL25Z4.h"             // Keil::Device:Startup
#include "MKL25Z4.h"                    // Device header
#include "stdio.h"
#include "stdbool.h"
#include "constants.h"


void moveStop(void);
void moveForward(int power);
void moveBackward(int power);
void moveLeft(int power);
void moveRight(int power);
void initMotors(void);

void moveStop(void)
{
	TPM0_C0V = 0;
	TPM0_C1V = 0;
	TPM0_C2V = 0;
	TPM0_C3V = 0;
}

void moveForward(int power)
{
	if (power > 100)
		power = 100;
	if (power < 0)
		power = 0;
	
	float coeff = (float) power / 100.0f;
	
    TPM0_C0V = (uint16_t)((uint16_t)(TPM0->MOD) * coeff);
    TPM0_C1V = 0;
    TPM0_C2V = (uint16_t)((uint16_t)(TPM0->MOD) * coeff);
    TPM0_C3V = 0;
}

void moveBackward(int power)
{
	if (power > 100)
		power = 100;
	if (power < 0)
		power = 0;
	
	float coeff = (float) power / 100.0f;
	
    TPM0_C0V = 0;
    TPM0_C1V = (uint16_t)((uint16_t)(TPM0->MOD) * coeff);
    TPM0_C2V = 0;
    TPM0_C3V = (uint16_t)((uint16_t)(TPM0->MOD) * coeff);
}

void moveLeft(int power)
{
	if (power > 100)
		power = 100;
	if (power < 0)
		power = 0;
	
	float coeff = (float) power / 100.0f;
	
    TPM0_C0V = 0;
    TPM0_C1V = (uint16_t)((uint16_t)(TPM0->MOD) * coeff);
    TPM0_C2V = (uint16_t)((uint16_t)(TPM0->MOD) * coeff);
    TPM0_C3V = 0;
}

void moveRight(int power)
{
	if (power > 100)
		power = 100;
	if (power < 0)
		power = 0;
	
	float coeff = (float) power / 100.0f;
	
    TPM0_C0V = (uint16_t)((uint16_t)(TPM0->MOD) * coeff);
    TPM0_C1V = 0;
    TPM0_C2V = 0;
    TPM0_C3V = (uint16_t)((uint16_t)(TPM0->MOD) * coeff);
}

void initMotors(void) 
{
	//Enable clock for Port A and D
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
	
	// Configure all to Timer
	PORTD->PCR[LEFT_MOTOR_FWD] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[LEFT_MOTOR_FWD] |= PORT_PCR_MUX(4);
	PORTA->PCR[LEFT_MOTOR_RVS] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[LEFT_MOTOR_RVS] |= PORT_PCR_MUX(3);
	PORTD->PCR[RIGHT_MOTOR_FWD] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[RIGHT_MOTOR_FWD] |= PORT_PCR_MUX(4);
	PORTD->PCR[RIGHT_MOTOR_RVS] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[RIGHT_MOTOR_RVS] |= PORT_PCR_MUX(4);
	
	//Enable clock for TPM0 module
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	
	//Select MCGFLLCLK for system
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	//Set timers to 50Hz
	TPM0->MOD = 7500;
	
	//Set prescaler to 1/128
	//Select up-counting mode
	TPM0->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM0->SC |= ((TPM_SC_CMOD(1)) | (TPM_SC_PS(7)));
	TPM0->SC &= ~(TPM_SC_CPWMS_MASK);
	
	//Select edge-aligned PWM with high-true pulses, for TPM0_CH0, TPM0_CH1, TPM0_CH2, TPM0_CH3
	TPM0_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM0_C0SC |= (TPM_CnSC_ELSB(1)) | (TPM_CnSC_MSB(1));
	TPM0_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM0_C1SC |= (TPM_CnSC_ELSB(1)) | (TPM_CnSC_MSB(1));
	TPM0_C2SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM0_C2SC |= (TPM_CnSC_ELSB(1)) | (TPM_CnSC_MSB(1));
	TPM0_C3SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM0_C3SC |= (TPM_CnSC_ELSB(1)) | (TPM_CnSC_MSB(1));
}
