#ifndef I2C_H
#define I2C_H
#define NO_COMMAND 10  
#define NEW_COMMAND_CONTINOUS 20
#define NEW_COMMAND_DIFFERENT 25
#define OLD_COMMAND 30  
#define WRONG_COMMAND 40
#define NO_COMMAND_TIMEOUT 100
   
void	oInitI2C(void);
void oWaitI2Cdata(void);
uInt8 oGetCommand(void);
uInt8 * oGetReceptionBuffer(void);
uInt16 getBufferSize(void);
uInt8 checkNewCommandI2C(void);
void fillSendBuffer(void);
#endif
