// ADCTestmain.c

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ADCT0ATrigger.h"
#include "PLL.h"
#include "DAC.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

#define LEFT 1
#define RIGHT 0
#define audioSamplingF 44100                       // Sample audio at 44.1kHz
#define audioPeriod 80000000 / audioSamplingF      // ADC interrupt period = 80MHz / sampling frequency

uint32_t delay1;
extern uint32_t ADC_In[2];  // data from ADC  (extern keyword to share between files)
extern uint32_t adcFlag;
uint32_t test[2];

int main(void){
  PLL_Init(Bus80MHz);                      // 80 MHz system clock
	
	//PORTF Init for debugging
			SYSCTL_RCGCGPIO_R |= 0x00000020;         // activate port F
			delay1 = SYSCTL_RCGCGPIO_R;
			GPIO_PORTF_DIR_R |= 0x04;                // make PF2 out (built-in LED)
			GPIO_PORTF_AFSEL_R &= ~0x04;             // disable alt funct on PF2
			GPIO_PORTF_DEN_R |= 0x04;                // enable digital I/O on PF2                                         // configure PF2 as GPIO
			GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF0FF)+0x00000000;
			GPIO_PORTF_AMSEL_R = 0;                  // disable analog functionality on PF
			GPIO_PORTF_DATA_R &= ~0x04;              // turn off LED
	
	ADC0_InitTimer0ATriggerSeq2PD1PD0(audioPeriod); // ADC channel 0, 10 Hz sampling

  EnableInterrupts();
  while(1){
		
		while (adcFlag ==0){}										// wait for new ADC value
    GPIO_PORTF_DATA_R ^= 0x04;             // toggle LED PF2
		
		test[LEFT] = ADC_In[LEFT];
		test[RIGHT] = ADC_In[RIGHT];
		
		DAC_Out(test[LEFT], test[RIGHT]);
		
		
  }
}

