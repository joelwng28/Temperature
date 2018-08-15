// Main.c
// Runs on LM4F120/TM4C123
// Temperature Data Acquisition System
// 4/7/2018

/**
 */
 
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "ADCT0ATrigger.h"
#include "ST7735.h"
#include "UART.h"
#include "PLL.h"
#include "calib.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

extern uint32_t currentIndex;
extern uint32_t data[100];

int main(void){ 
	DisableInterrupts();
	PLL_Init(Bus80MHz);	// bus clock at 80 MHz
	UART_Init();
	ST7735_InitR(INITR_REDTAB);
	ADC0_InitTimerTriggerSeq3_Ch0();
		
	EnableInterrupts();
	
	/*
	UART_OutString("Points: \r\n");
	while(currentIndex < 100);
	for(int i = 0; i < 100; i += 1) {
		UART_OutUDec(data[i]);
		UART_OutString("\r\n");
	}
	*/
	while(1) {
		ST7735_SetCursor(0, 0);
		int32_t currentTemperature = Temp_Conv(ADC_FIFO_CurrentValue());
		printf("Temperature %5d.%02d\n", currentTemperature / 100, currentTemperature % 100);
		ST7735_PlotClear(32, 159);
		
		//plot relative change in temperature over time for fun
		for(int i = 0; i < FIFO_SIZE; i += 1) {
			int32_t point = 128 - Temp_Conv(ADC_FIFO_Get()[i]) * 2 / 100; //just for scaling
			ST7735_DrawPixel(i+1, point+1, ST7735_RED);
			ST7735_DrawPixel(i+1, point, ST7735_RED);
			ST7735_DrawPixel(i, point+1, ST7735_RED);
			ST7735_DrawPixel(i, point, ST7735_RED);
		}
		WaitForInterrupt();
	}
}
