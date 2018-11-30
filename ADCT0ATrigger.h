// ADCT0ATrigger.h


// Initializes ADC0 to trigger upon Timer0A timeout
// Uses ADC0 Sequencer 2
// After each Timer0A timeout, ADC0 samples two channels consecutively: PD1 (AIN6) then PD0 (AIN7)
// PD1: Audio Left Channel
// PD0: Audio Right Channel
void ADC0_InitTimer0ATriggerSeq2PD1PD0(uint32_t period);
