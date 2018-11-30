//DAC.h

/*
Kurt Efaw
This file will set up the TLV5616 DAC for use in Lab 11
Will use SSI2 with the pins listed below
TX - PB7
RX - PB6  (reserved but not being used)
Fss - PB5
Clk - PB4
*/

void DAC_Init(void);

void DAC_Out(int vLeft, int vRight);

