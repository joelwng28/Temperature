// ADCT0ATrigger.h
// 4/8/2018

#ifndef ADC_TRIGGER
#define ADC_TRIGGER 1

#define FIFO_SIZE 160

/********** ADC0_InitTimerTriggerSeq3_Ch0 ***********
 * initialize PE3 as ADC input, timer triggered ADC
 * inputs: none
 * outputs: none
 */ 
void ADC0_InitTimerTriggerSeq3_Ch0(void);

/********** ADC_FIFO_Pop ***********
 * pop data out of fifo
 * inputs: none
 * outputs: int32_t data
 */ 
int32_t ADC_FIFO_Pop(void);

/********** ADC_FIFO_Get ***********
 * get fifo address location
 * inputs: none
 * outputs: pointer to fifo
 */ 
int32_t* ADC_FIFO_Get(void);

/********** ADC_FIFO_CurrentValue ***********
 * gets current value of fifo
 * inputs: none
 * outputs: data
 */ 
int32_t ADC_FIFO_CurrentValue(void);

#endif

