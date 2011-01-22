#ifndef TIMERS_H
#define TIMERS_H

//Functions and Variables with Global Scope:
void Timer1_Init(void);
void Timer2_Init(void);
void __attribute__((__interrupt__)) _T1Interrupt(void);

#endif
