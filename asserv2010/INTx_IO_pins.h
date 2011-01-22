#ifndef INTX_IO_PINS_H
#define INTX_IO_PINS_H

void INTx_IO_Init(void);
void __attribute__((__interrupt__)) _INT0Interrupt(void);
void __attribute__((__interrupt__)) _INT1Interrupt(void);

#endif
