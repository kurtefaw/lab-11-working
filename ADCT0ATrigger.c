// ADCT0ATrigger.c
// Runs on LM4F120/TM4C123
// Provide a function that initializes Timer0A to trigger ADC
// SS3 conversions and request an interrupt when the conversion
// is complete.
// Daniel Valvano
// May 2, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include <stdint.h>
#include "tm4c123gh6pm.h"

#define LEFT 1
#define RIGHT 0


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode


// Initializes ADC0 to trigger upon Timer0A timeout
// Uses ADC0 Sequencer 2
// After each Timer0A timeout, ADC0 samples two channels consecutively: PD1 (AIN6) then PD0 (AIN7)
// PD1: Audio Left Channel
// PD0: Audio Right Channel
void ADC0_InitTimer0ATriggerSeq2PD1PD0(uint32_t period){
  volatile uint32_t delay;
  SYSCTL_RCGCADC_R |= 0x01;     // 1) activate ADC0 
  SYSCTL_RCGCGPIO_R |= 0x08;    // Port D clock
  delay = SYSCTL_RCGCGPIO_R;    // allow time for clock to stabilize
  GPIO_PORTD_DIR_R &= ~0x03;    // make PD0,PD1 input
  GPIO_PORTD_AFSEL_R |= 0x03;   // enable alternate function on PD0,PD1
  GPIO_PORTD_DEN_R &= ~0x03;    // disable digital I/O on PD0,PD1
  GPIO_PORTD_AMSEL_R |= 0x03;   // enable analog functionality on PD0,PD1
  ADC0_PC_R = 0x01;             // 2) configure for 125K samples/sec
  ADC0_SSPRI_R = 0x3210;        // 3) sequencer 0 is highest, sequencer 3 is lowest
  SYSCTL_RCGCTIMER_R |= 0x01;   // 4) activate timer0 
  delay = SYSCTL_RCGCGPIO_R;
  TIMER0_CTL_R = 0x00000000;    // disable timer0A during setup
  TIMER0_CTL_R |= 0x00000020;   // enable timer0A trigger to ADC
  TIMER0_CFG_R = 0;             // configure for 32-bit timer mode
  TIMER0_TAMR_R = 0x00000002;   // configure for periodic mode, default down-count settings
  TIMER0_TAPR_R = 0;            // prescale value for trigger
  TIMER0_TAILR_R = period-1;    // start value for trigger
  TIMER0_IMR_R = 0x00000000;    // disable all interrupts
  TIMER0_CTL_R |= 0x00000001;   // enable timer0A 32-b, periodic, no interrupts
  ADC0_ACTSS_R &= ~0x04;        // 5) disable sample sequencer 2
  ADC0_EMUX_R = (ADC0_EMUX_R&0xFFFFF0FF)+0x0500; // 6) timer trigger event
  ADC0_SSMUX2_R = 0x0076;       // 7) 1st: PD1 (AIN6),  2nd: PD0 (AIN7)
  ADC0_SSCTL2_R = 0x0060;       // 8) set flag and end after 2nd channel conversion (PD0)                       
  ADC0_IM_R |= 0x04;            // 9) enable SS2 interrupts
  ADC0_ACTSS_R |= 0x04;         // 10) enable sample sequencer 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0xFFFFFF00)|0x00000040; // 11)priority 2
  NVIC_EN0_R = 1<<16;           // 12) enable interrupt 16 in NVIC (SS2)

}


volatile uint32_t ADC_In[2];    // volatile so it can be shared between main program and ISR
volatile uint32_t adcFlag = 0; 

void ADC0Seq2_Handler(void){
	
	GPIO_PORTF_DATA_R ^= 0x04;             // toggle LED PF2
	
  ADC0_ISC_R = 0x04;          					// acknowledge ADC sequence 2 completion
  ADC_In[LEFT] = ADC0_SSFIFO2_R;  			// PD1, Channel 6 first
  ADC_In[RIGHT] = ADC0_SSFIFO2_R;  			// PD0, Channel 7 second	
	
	adcFlag = 1;

	
	GPIO_PORTF_DATA_R ^= 0x04;             // toggle LED PF2
	GPIO_PORTF_DATA_R ^= 0x04;             // toggle LED PF2
}
