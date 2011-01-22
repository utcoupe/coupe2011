#ifndef CONVERSION_AN_H
#define CONVERSION_AN

void ADC_Init(void);
void __attribute__((__interrupt__, __no_auto_psv__)) _ADCInterrupt(void);
#endif
