
#include <p30fxxxx.h>

//Defines for System Clock Timing -
//For oscillator configuration XT x PLL16 mode,
//Device Throughput in MIPS = Fcy = 7372800*16/4 = ~29.48 MIPS
//Instruction Cycle time = Tcy = 1/(Fcy) = ~34 nanoseconds
#define XTFREQ          7372800         //On-board Crystal frequency
#define PLLMODE         16               //On-chip PLL setting
#define FCY             XTFREQ*PLLMODE/4        //Instruction Cycle Frequency

//Defines that equate Switches on board to specific interrupt pins on device
#define Switch_S5       PORTEbits.RE8   //Switch S5 is connected to RE8
#define Switch_S6       PORTDbits.RD0   //Switch S6 is connected to RD0






