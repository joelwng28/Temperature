// ADCT0ATrigger.c
// Runs on TM4C123
// Provide functions that initialize ADC0 SS3 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result.
// Daniel Valvano
// August 6, 2015


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ADCT0ATrigger.h"

int32_t fifo[FIFO_SIZE];
int32_t fifoStart = 0;
int32_t fifoEnd = 0;

/********** ADC0_InitTimerTriggerSeq3_Ch0 ***********
 * initialize PE3 as ADC input, timer triggered ADC
 * inputs: none
 * outputs: none
 */ 
void ADC0_InitTimerTriggerSeq3_Ch0(void){
	volatile uint32_t delay;
  SYSCTL_RCGCADC_R |= 0x0001;   // 7) activate ADC0
  SYSCTL_RCGCGPIO_R |= 0x10;    // 1) activate clock for Port E
  while((SYSCTL_PRGPIO_R&0x10) != 0x10); // wait for clock on port
  GPIO_PORTE_DIR_R &= ~0x08;      // 2) make PE3 input
  GPIO_PORTE_AFSEL_R |= 0x08;     // 3) enable alternate function on PE3
  GPIO_PORTE_DEN_R &= ~0x08;      // 4) disable digital I/O on PE3
  GPIO_PORTE_AMSEL_R |= 0x08;     // 5) enable analog functionality on PE3

	ADC0_PC_R &= ~0xF;              // 7) clear max sample rate field
  ADC0_PC_R |= 0x1;               //    configure for 125K samples/sec
  ADC0_SSPRI_R = 0x0123;          // 8) Sequencer 3 is highest priority

	// timer setup
	SYSCTL_RCGCTIMER_R |= 0x01;			// enable timer0
	delay = SYSCTL_RCGCGPIO_R;			// delay
	TIMER0_CTL_R = 0x0;							// disable timer
	TIMER0_CTL_R |= 0x020;					// enable timer0 to trigger ADC
	TIMER0_CFG_R = 0;								// configure for 32-bit timer mode
	TIMER0_TAMR_R = 0x02;						// configure for periodic mode
	TIMER0_TAPR_R = 0;							// prescale value for trigger
	TIMER0_TAILR_R = 799999;				// reload time (20Hz)
	TIMER0_IMR_R = 0x0;							// disable all interrupts
	TIMER0_CTL_R |= 0x01;						// enable timer0A 32-b, periodic

  ADC0_ACTSS_R &= ~0x0008;        // 9) disable sample sequencer 3
  ADC0_EMUX_R = (ADC0_EMUX_R & ~0x0F000) | 0x05000;  // 10) seq3 is software trigger
  ADC0_SSMUX3_R &= ~0x000F;       // 11) clear SS3 field
  //ADC0_SSMUX3_R += 0;           // set channel to 0
  ADC0_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
  ADC0_IM_R |= 0x0008;						// 13) disable SS3 interrupts
  ADC0_SAC_R |= 0x6;              // enable x64 sampling
  ADC0_ACTSS_R |= 0x0008;         // 14) enable sample sequencer 3
	
	NVIC_PRI4_R = (NVIC_PRI4_R & ~0x0FF00) | 0x04000; // timer0 priority 2
	NVIC_EN0_R = 1 << 17;						// enable interrupt 17 in NVIC
	
	// "zero" out with bad data
	for(int i = 0; i < FIFO_SIZE; i += 1) {
		fifo[i] = -1;
	}
}
/********** ADC_FIFO_Push ***********
 * push data onto fifo
 * inputs: int32_t data
 * outputs: none
 */ 
void ADC_FIFO_Push(int32_t data) {
	int32_t nextFifoEnd = (fifoEnd + 1) % FIFO_SIZE;
	if(nextFifoEnd == fifoStart) {
		return;
	}
	fifoEnd = nextFifoEnd;
	fifo[fifoEnd] = data;
}

/********** ADC_FIFO_Pop ***********
 * pop data out of fifo
 * inputs: none
 * outputs: int32_t data
 */ 
int32_t ADC_FIFO_Pop(void) {
	if(fifoStart == fifoEnd) {
		return -1;
	}
	int32_t value = fifo[fifoStart];
	fifoStart = (fifoStart + 1) % FIFO_SIZE;
	return value;
}

//------------ADC0_InSeq3------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
void ADC0Seq3_Handler(void){
	ADC0_ISC_R = 0x0008;
	uint32_t result = ADC0_SSFIFO3_R&0xFFF;
	if((fifoStart + 1) % FIFO_SIZE == fifoEnd) {
		ADC_FIFO_Pop();
	}
	ADC_FIFO_Push(result);
}

/********** ADC_FIFO_Get ***********
 * get fifo address location
 * inputs: none
 * outputs: pointer to fifo
 */ 
int32_t* ADC_FIFO_Get(void) {
	return fifo;
}

/********** ADC_FIFO_CurrentValue ***********
 * gets current value of fifo
 * inputs: none
 * outputs: data
 */ 
int32_t ADC_FIFO_CurrentValue(void) {
	return fifo[fifoStart];
}
