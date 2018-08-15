//calib.c
//calibration values for ADC to temperature conversion
//4/8/2018

#include <stdint.h>
#define OFFSET 100
/*DATA LOOK-UP TABLES*/

//adc value conversion relative to Tdata. These are relative points for easier conversion, be sure to round down
uint16_t const ADCdata[53]={0,12,52,93,135,179,223,269,317,365,415,
     467,520,574,630,687,747,807,870,934,1000,
     1067,1137,1208,1282,1357,1434,1513,1595,1678,1764,
     1852,1942,2034,2128,2225,2324,2426,2530,2636,2744,
     2855,2969,3085,3203,3324,3447,3572,3700,3830,3963,4095,4096};
											

uint16_t const Tdata[53]={4000,4000,3920,3840,3760,3680,3600,3520,3440,3360,3280,
     3200,3120,3040,2960,2880,2800,2720,2640,2560,2480,
     2400,2320,2240,2160,2080,2000,1920,1840,1760,1680,
     1600,1520,1440,1360,1280,1200,1120,1040,960,880,
     800,720,640,560,480,400,320,240,160,80,0,0};
	
		
								
/********** Temp_Conv ***********
 * convert adc value into temperature fixed point 
 * inputs: none
 * outputs: temp fixed point with resolution of .01
 */ 		 
int32_t Temp_Conv(int32_t ADC_Sample){
	
	int32_t i;

	for(i=0; i < 53; i++){
		if(ADCdata[i] > ADC_Sample){
			break;
		}
	}
	//for more precise output
	/*
	float ADCDataDiff = (float)(ADCdata[i]) - (float)(ADCdata[i-1]);
	float TDataDiff = (float)(Tdata[i]) - (float)(Tdata[i-1]);
	float slope = TDataDiff/ADCDataDiff;
	
	return (int32_t)((slope)*(ADC_Sample - ADCdata[i-1]) + Tdata[i-1]) + OFFSET; 
	*/
	//for output
	
	i -= 1;
	
	return Tdata[i] + OFFSET;
	
	
}
