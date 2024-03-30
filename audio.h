#include "RTE_Components.h"
#include  CMSIS_device_header
#include "system_MKL25Z4.h"             // Keil::Device:Startup
#include "MKL25Z4.h"                    // Device header
#include "stdio.h"
#include "stdbool.h"
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

#define PWM_PERIOD 20000 // PWM period in clock cycles (20 ms)
#define FREQUENCY_TO_MOD(x) (375000 / (x))
#define SONGCONNEST_NOTE_COUNT 32
#define SONGMAIN_NOTE_COUNT 64
#define SONGRUNFIN_NOTE_COUNT 8
#define C4 261
#define D4 293
#define E4 329
#define F4 349
#define G4 392
#define A4 440
#define As4 466
#define B4 493
#define C5 523

void PWM_Init(void);
void playMelody(void);
void audioConnEst(void);
void audioRunFin(void);
void audioSong(uint32_t note);
void audioConnEst(void);
void delay(volatile uint32_t nof);
//void music (void);
void music (void *argument);

volatile static int currNote = 0;

static int songConnEst[SONGCONNEST_NOTE_COUNT] = {
As4, As4, 0, 0, As4, As4, 0, 0, As4, As4, 0, 0, As4, As4, 0, 0,
As4, As4, As4, B4, B4, B4, F4 * 2, F4 * 2, As4, As4, 0, 0, As4, As4, 0, 0
};

static int songMain[SONGMAIN_NOTE_COUNT] = {
A4, G4, A4, E4 * 2, D4 * 2, C5, A4, A4, A4, G4, A4, E4 * 2, D4 * 2, C5, A4, A4,
C5, B4, G4, C5, C5, B4, G4, G4, C5, B4, G4, D4 * 2, D4 * 2, B4, G4, G4, 
A4, G4, A4, E4 * 2, D4 * 2, C5, A4, A4, A4, G4, A4, E4 * 2, D4 * 2, C5, A4, A4,
A4, A4, 0, A4, B4, 0, B4, 0, C5, C5, 0, C5, B4, 0, B4, 0
};

static int songRunFin[SONGRUNFIN_NOTE_COUNT] = {C4, D4, E4, F4, G4, A4, B4, C5};

void delay(volatile uint32_t nof) {
    while(nof!=0) {
        __ASM("NOP");
        nof--;
    }
}

void PWM_Init(void) {
    // Enable clock for PORTB
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    
    // Enable clock for TPM1
    SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
    
    // Set PTB0 to TPM1_CH0 mode
    PORTB->PCR[0] |= PORT_PCR_MUX(3);
    
    // Set TPM clock source to MCGFLLCLK (24 MHz)
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
    
    // Set TPM prescaler to divide by 128
    TPM1->SC |= TPM_SC_PS(7);
    
    // Set TPM module to up counting mode
    TPM1->SC |= TPM_SC_CMOD(1);
    
    // Set TPM period
    TPM1->MOD = PWM_PERIOD;
    
    // Set TPM1_CH0 pulse width to 50% duty cycle
    TPM1->CONTROLS[0].CnV = 0;
    
    // Set TPM1_CH0 mode to PWM high-true pulses
    TPM1->CONTROLS[0].CnSC |= TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK;
}

void audioConnEst(void)
{
    for (int i = 0; i < SONGMAIN_NOTE_COUNT; i++) {
        TPM1->MOD = FREQUENCY_TO_MOD((uint32_t)songConnEst[i] * 4);       // play at music note frequency
        TPM1_C0V = FREQUENCY_TO_MOD((uint32_t)songConnEst[i] * 4) / 2;    // mantain 50% duty cycle
        //delay(100);
        osDelay(100);
    }
    TPM1->MOD = 0;
    TPM1_C0V = 0;
}

void audioSong(uint32_t note)
{
  TPM1->MOD = FREQUENCY_TO_MOD((uint32_t)songMain[note] * 4);
  TPM1_C0V = FREQUENCY_TO_MOD((uint32_t)songMain[note] * 4) / 2;
  //delay(90);
  osDelay(90);
}

void audioRunFin(void)
{
  for (int i = 0; i < SONGRUNFIN_NOTE_COUNT; i++) {
        TPM1->MOD = FREQUENCY_TO_MOD((uint32_t)songRunFin[i] * 4);       // play at music note frequency
        TPM1_C0V = FREQUENCY_TO_MOD((uint32_t)songRunFin[i] * 4) / 2;    // mantain 50% duty cycle
        //delay(150);
        osDelay(150);
    }
    TPM1->MOD = 0;
    TPM1_C0V = 0;
}

void music (void *argument) {
	for(;;) {
    audioSong((uint32_t)currNote);
    currNote = currNote + 1 == SONGMAIN_NOTE_COUNT ? 0 : currNote + 1;
  }
}
