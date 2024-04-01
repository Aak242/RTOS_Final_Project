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
#define SONGRUNFIN_NOTE_COUNT 18
#define C4 261
#define D4 293
#define E4 329
#define F4 349
#define G4 392
#define A4 440
#define As4 466
#define B4 493
#define C5 523
#define E5 659
#define F5 698
#define GS4 415


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
volatile bool static victoryMusic = false; // Initialize to false

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

int static songFirstSectionDurations[SONGRUNFIN_NOTE_COUNT] = {500, 500, 500, 350, 150, 500, 350, 150, 650, 500, 500, 500, 350, 150, 500, 350, 150, 650};

static int songRunFin[SONGRUNFIN_NOTE_COUNT] = {A4, A4, A4, F4, C5, A4, F4, C5, A4, E5, E5, E5, F5, C5, GS4, F4, C5, A4};

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

//void audioRunFin(void)
//{
//  for (int i = 0; i < SONGRUNFIN_NOTE_COUNT; i++) {
//        TPM1->MOD = FREQUENCY_TO_MOD((uint32_t)songRunFin[i]);
//        TPM1_C0V = TPM1->MOD / 2;    // maintain 50% duty cycle
//        osDelay((uint32_t)songFirstSectionDurations[i]);
//    }
//    TPM1->MOD = 0;
//    TPM1_C0V = 0;
//}

void audioRunFin(void)
{
  for (int i = 0; i < SONGRUNFIN_NOTE_COUNT; i++) {
        TPM1->MOD = FREQUENCY_TO_MOD((uint32_t)songRunFin[i]);
        TPM1_C0V = TPM1->MOD / 2;    // maintain 50% duty cycle
        osDelay((uint32_t)((double)songFirstSectionDurations[i]/2.4));
    }
    TPM1->MOD = 0;
    TPM1_C0V = 0;
}

//void music (void *argument) {
//	for(;;) {
//    audioSong((uint32_t)currNote);
//    currNote = currNote + 1 == SONGMAIN_NOTE_COUNT ? 0 : currNote + 1;
//  }
//}

////////////////////////////FINAL MUSIC/////////////////////////////////
#define C4 261
#define D4 294
#define E4 329
#define F4 349
#define G4 391
#define GS4 415
#define A4 440
#define AS4 455
#define B4 466
#define C5 523
#define CS5 554
#define D5 587
#define DS5 622
#define E5 659
#define F5 698
#define FS5 740
#define G5 784
#define GS5 830
#define A5 880

void playNote(int note, int duration) {
    TPM1->MOD = FREQUENCY_TO_MOD(note);
    TPM1_C0V = TPM1->MOD / 2; // maintain 50% duty cycle
    osDelay(duration/ 2.4);
    TPM1->MOD = 0;
    TPM1_C0V = 0;
}

void firstSection() {
    playNote(A4, 500);
    playNote(A4, 500);
    playNote(A4, 500);
    playNote(F4, 350);
    playNote(C5, 150);
    playNote(A4, 500);
    playNote(F4, 350);
    playNote(C5, 150);
    playNote(A4, 650);
    osDelay(500);
    playNote(E5, 500);
    playNote(E5, 500);
    playNote(E5, 500);
    playNote(F5, 350);
    playNote(C5, 150);
    playNote(GS4, 500);
    playNote(F4, 350);
    playNote(C5, 150);
    playNote(A4, 650);
    osDelay(500);
}

void secondSection() {
    playNote(A5, 500);
    playNote(A4, 300);
    playNote(A4, 150);
    playNote(A5, 500);
    playNote(GS5, 325);
    playNote(G5, 175);
    playNote(FS5, 125);
    playNote(F5, 125);
    playNote(FS5, 250);
    osDelay(325);
    playNote(AS4, 250);
    playNote(DS5, 500);
    playNote(D5, 325);
    playNote(CS5, 175);
    playNote(C5, 125);
    playNote(B4, 125);
    playNote(C5, 250);
    osDelay(350);
}

void firstSectionVariant1() {
    playNote(F4, 250);
    playNote(GS4, 500);
    playNote(F4, 350);
    playNote(A4, 125);
    playNote(C5, 500);
    playNote(A4, 375);
    playNote(C5, 125);
    playNote(E5, 650);
    osDelay(500);
}

void secondSectionVariant1() {
    secondSection();
    firstSectionVariant1();
}

void firstSectionVariant2() {
    playNote(F4, 250);
    playNote(GS4, 500);
    playNote(F4, 375);
    playNote(C5, 125);
    playNote(A4, 500);
    playNote(F4, 375);
    playNote(C5, 125);
    playNote(A4, 650);
    osDelay(650);
}

void secondSectionVariant2() {
    secondSection();
    firstSectionVariant2();
}

////////////////////////////FINAL MUSIC/////////////////////////////////

void music(void *argument) {
    for (;;) {
        if (victoryMusic) {
            audioRunFin();
        } else {
            // Play the main melody (songMain)
            audioSong((uint32_t)currNote);
            currNote = (currNote + 1 == SONGMAIN_NOTE_COUNT) ? 0 : currNote + 1;
        }
    }
}
