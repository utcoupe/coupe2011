#ifndef UART_H
#define UART_H
void UART_Init (void);
void WriteUART_to_RS232(void);
void __attribute__((__interrupt__)) _U1TXInterrupt(void);
#endif
