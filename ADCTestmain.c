// ADCTestmain.c
// Runs on LM4F120/TM4C123
// Provide a function that initializes Timer0A to trigger ADC
// SS3 conversions and request an interrupt when the conversion
// is complete.

// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V through X/10-ohm ohm resistor
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ADCT0ATrigger.h"
#include "PLL.h"
#include "ST7735.h"
#include "calib.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
volatile uint8_t mode;

#define N 128
// This program periodically samples ADC0 channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.


int main(void){
  PLL_Init(Bus80MHz);                      		// 80 MHz system clock
	ADC0_InitTimer0ATriggerSeq3PD3(800000);			//100 Hz sampling
	Output_Init();
  
  EnableInterrupts();
	ST7735_PlotClear(0,50);
	ST7735_FillScreen(ST7735_WHITE);
	uint32_t adc_value;
	uint16_t temperature, prev_temp;
	int j;
  while(1){		
		adc_value = ADC_current_value();
		temperature = calibrate(adc_value);
		if(temperature-prev_temp>100 || prev_temp-temperature>100){
			ST7735_SetCursor(0,0);
			ST7735_sDecOut2(temperature);
			prev_temp = temperature;
		}

		/*graphing*/
		int32_t graph_temp = temperature/100;
		ST7735_PlotPoint(graph_temp);  // Measured temperature
		if((j&(N-1))==0){          // fs sampling, fs/N samples plotted per second
			ST7735_PlotNextErase();  // overwrites N points on same line
		}
		j++;     				              // counts the number of samples
  }
}

