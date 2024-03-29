#include "MKL25Z4.h"                    // Device header

/* --------------------------- Motors -----------------------------------*/
#define LEFT_MOTOR_FWD 0        //PTD0 - TPM0_CH0
#define LEFT_MOTOR_RVS 4        //PTA4 - TPM0_CH1
#define RIGHT_MOTOR_FWD 2       //PTD2 - TPM0_CH2
#define RIGHT_MOTOR_RVS 3       //PTD3 - TPM0_CH3

#define Q_SIZE (32)
#define BAUD_RATE 9600
#define UART_TX_PORTE22 22
#define UART_TX_PORTE23 23
#define UART2_INT_PRIO 128

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

static void delay(volatile uint32_t nof) {
    while(nof!=0) {
        __ASM("NOP");
        nof--;
    }
}

volatile int currNote = 0;

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
        TPM1->MOD = FREQUENCY_TO_MOD(songConnEst[i] * 4);       // play at music note frequency
        TPM1_C0V = FREQUENCY_TO_MOD(songConnEst[i] * 4) / 2;    // mantain 50% duty cycle
        delay(100);
    }
    TPM1->MOD = 0;
    TPM1_C0V = 0;
}

void audioSong(int note)
{
  TPM1->MOD = FREQUENCY_TO_MOD(songMain[note] * 4);
  TPM1_C0V = FREQUENCY_TO_MOD(songMain[note] * 4) / 2;
  delay(90);
}

void audioRunFin(void)
{
  for (int i = 0; i < SONGRUNFIN_NOTE_COUNT; i++) {
        TPM1->MOD = FREQUENCY_TO_MOD(songRunFin[i] * 4);       // play at music note frequency
        TPM1_C0V = FREQUENCY_TO_MOD(songRunFin[i] * 4) / 2;    // mantain 50% duty cycle
        delay(150);
    }
    TPM1->MOD = 0;
    TPM1_C0V = 0;
}


static volatile uint8_t data;

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
	
	TPM0_C0V = TPM0->MOD * coeff;
	TPM0_C1V = 0;
	TPM0_C2V = TPM0->MOD * coeff;
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
	TPM0_C1V = TPM0->MOD * coeff;
	TPM0_C2V = 0;
	TPM0_C3V = TPM0->MOD * coeff;
}

void moveLeft(int power)
{
	if (power > 100)
		power = 100;
	if (power < 0)
		power = 0;
	
	float coeff = (float) power / 100.0f;
	
	TPM0_C0V = 0;
	TPM0_C1V = TPM0->MOD * coeff;
	TPM0_C2V = TPM0->MOD * coeff;
	TPM0_C3V = 0;
}

void moveRight(int power)
{
	if (power > 100)
		power = 100;
	if (power < 0)
		power = 0;
	
	float coeff = (float) power / 100.0f;
	
	TPM0_C0V = TPM0->MOD * coeff;
	TPM0_C1V = 0;
	TPM0_C2V = 0;
	TPM0_C3V = TPM0->MOD * coeff;
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
	
	//Set LPTPM counter to increment on every counter clock
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

/* Init UART2 */
void InitUART2(uint32_t baud_rate)
{
  uint32_t divisor, bus_clock;
  
  //enable clock to UART2 and PORTE
  SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
  
  //connect UART pins for PTE22 and PTE23
  PORTE->PCR[UART_TX_PORTE22] &= ~PORT_PCR_MUX_MASK;
  PORTE->PCR[UART_TX_PORTE22] |= PORT_PCR_MUX(4); //enable UART functionality
  
  PORTE->PCR[UART_TX_PORTE23] &= ~PORT_PCR_MUX_MASK;
  PORTE->PCR[UART_TX_PORTE23] |= PORT_PCR_MUX(4);
	
	PORTE->PCR[UART_TX_PORTE23] |= (0b1010 << 16);
  
  //Ensure that Tx and Rx are disabled before configuration
  UART2->C2 &= ~( UART_C2_RE_MASK | UART_C2_RIE_MASK);//Transmit Enable and Receive Enable off
  
  //set baud rate to desired value
  bus_clock = (DEFAULT_SYSTEM_CLOCK)/2;
  divisor = bus_clock / (baud_rate * 16); //multiple by 16 to deal with default x16 oversampling by UART2 (Hardware)
  //baud rate register, divided into two parts, BDH and BDL
  UART2->BDH = UART_BDH_SBR(divisor >> 8); 
  UART2->BDL =  UART_BDL_SBR(divisor);
  
  //for error checking (e.g. parity checking), set all to zero cus not using 
  UART2->C1 = 0;
//  UART2->S2 = 0;
  UART2->C3 = 0;
  
  
 //Clear & enable Interupts in UART2
 //Priority is set at 128
// NVIC_SetPriority(UART2_IRQn, 0);
 NVIC_ClearPendingIRQ(UART2_IRQn);
 NVIC_EnableIRQ(UART2_IRQn);
 
 //Enable Tx and Rx
 UART2->C2 |= (UART_C2_RIE_MASK | UART_C2_RE_MASK);
 //Tx/Rx Interrupt Enable
// UART2->C2 |= (UART_C2_TIE_MASK | UART_C2_RIE_MASK);
}

void UART2_Transmit_Poll(uint8_t data)
{
  //TDRE (Transmitter Data Regsiter Empty)
  while(!(UART2->S1 & UART_S1_TDRE_MASK)); //stuck in this loop, until the TDRE is set (meaning that is it now empty, so can transmit new data)
  UART2->D = data; //take data and write into the D register
}

uint8_t UART2_Receive_Poll(void)
{
  while(!(UART2->S1 & UART_S1_RDRF_MASK)); //waits until RDRF (Receive Data Register Full) is set, ensure there is new data avail in receiver
  return (UART2->D); //reads received data and returns it
}

static volatile uint8_t rx_data;

void UART2_IRQHandler(void) {
    // Clear the interrupt flag
    NVIC_ClearPendingIRQ(UART2_IRQn);

    // Check if data is available in the receive buffer
    if (UART2->S1 & UART_S1_RDRF_MASK) {
        // Read the received data from the data register
        rx_data = UART2->D;
        // Update the global data variable
        data = rx_data;
    }
}

void music (void) {
	for(;;) {
    audioSong(currNote);
    currNote = currNote + 1 == SONGMAIN_NOTE_COUNT ? 0 : currNote + 1;
  }
}
volatile uint32_t systemTicks = 0;

// SysTick_Handler function will be called every 1 millisecond
void SysTick_Handler(void) {
    systemTicks++; // Increment the system uptime counter
}

// Initialize SysTick timer to generate interrupts every 1 millisecond
void SysTick_Init(void) {
    SysTick_Config(SystemCoreClock / 1000); // Configure SysTick to interrupt every 1 millisecond
}

// Function to return the system uptime in milliseconds
uint32_t millis(void) {
    return systemTicks; // Return the current value of the system uptime counter
}

int main (void) {
	SystemCoreClockUpdate();
	initMotors();
	InitUART2(BAUD_RATE);
	PWM_Init();
	//music();
	
	uint32_t lastNoteTime = 0;
	uint32_t noteDuration = 90; // Duration of each note in milliseconds
	int counter = 0;
	while (1) {
		uint32_t currentTime = millis(); // You need to implement a millis() function that returns the system uptime in milliseconds

		// Play music notes at the specified interval
		if (currentTime - lastNoteTime >= noteDuration) {
			audioSong(currNote);
			currNote++;
			if (currNote >= SONGMAIN_NOTE_COUNT) {
				currNote = 0;
			}
			lastNoteTime = currentTime;
		}
		counter++;
			data = UART2_Receive_Poll();
		if(data == 0b00110001) {
			moveForward(100);
			data = 0;
		}
		else if(data == 0b00110010) {
			moveBackward(100);
			data = 0;
		}
		else if(data == 0b00110011) {
			moveLeft(100);
			data = 0;
		}
		else if(data == 0b00110100) {
			moveRight(100);
			data = 0;
		}
		
		else {
			moveStop();
		}
	}
}
