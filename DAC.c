/*
Kurt Efaw
This file will set up the TLV5616 DAC for use in Lab 11
Will use SSI2 with the pins listed below
TX - PB7
RX - PB6  (reserved but not being used)
Fss - PB5
Clk - PB4
*/
#include "tm4c123gh6pm.h"
#include <stdio.h>
#include <stdint.h>


#define REF_VOLTAGE 1.5
#define DAC_CTRL_VAL 0

// Initialize Port B to be used for SSI2
void GPIOB_Init(void) {
  SYSCTL_RCGCGPIO_R |= 0x02;           // 1) activate clock for Port B
  while((SYSCTL_PRGPIO_R&0x02)==0){};  // allow time for clock to start
  GPIO_PORTB_PCTL_R |= (GPIO_PORTB_PCTL_R&0x0000FFFF)+0x22220000;     // 3) select SSI functionality for PB4-PB7
  GPIO_PORTB_AMSEL_R &= ~0xF0;         // 4) disable analog function on PB4-PB7
  GPIO_PORTB_AFSEL_R |= 0xF0;          // 6) alternative port function
	GPIO_PORTB_DIR_R |= 0xB0;            // 5a) set output pins PB7,PB5,PB4(TX, Clk, Fss)  // not sure if we should actually set this register
  GPIO_PORTB_DIR_R &= ~0x40;           // 5b) set input pins PB6 (RX)
  GPIO_PORTB_DEN_R |= 0xF0;            // 7) enable digital port
}


// Initialize SSI2 Settings
void SSI2_Init(void) {
  while(SYSCTL_PRSSI_R & SYSCTL_PRSSI_R0 == 0) {};
  GPIOB_Init();
  
  SSI2_CR1_R = 0x0;
  
  SSI2_CC_R = SSI_CC_CS_SYSPLL; // Set clock source to system clock
  // Set SSI bit rate to 1.481MHz (max data rate of 46.3kHz)
  // Set CPSR to 2
  SSI2_CPSR_R = 0x2;
  // Set CR0 (SCR) to 26 and 16-bit data
  SSI2_CR0_R = 26 << 8 | SSI_CR0_SPH | 0xF;  //SPH = 1, not sure what SPO is set to....
  
  SYSCTL_RCGCSSI_R |= 0x4;
  
  SSI1_CR1_R |= 0x2;
}

void DAC_Init(void){ 
  SSI2_Init();
}

// This only takes positive voltages from 0-2048
void DAC_Out(int vLeft, int vRight) {
  // outputVoltage = 2*REF*(CODE/2^12)
  // 12-bit number to be transmitted (data bits of SSI transmission only)
  if (vLeft < 0) {
    vLeft = 0;
  }
  if (vLeft > 2048) {
    vLeft = 2048;
  }
	if (vRight < 0) {
    vRight = 0;
  }
  if (vRight > 2048) {
    vRight = 2048;
  }
  
  SSI0_DR_R = (uint16_t) (DAC_CTRL_VAL<<12 | vRight | 0x1000); // Write Right Channel to Buffer 
	
	SSI0_DR_R = (uint16_t) (DAC_CTRL_VAL<<12 | vLeft | 0x8000); // Write Left Channel to DAC A, update DAC B with buffer contents (Right Channel)
  
}
