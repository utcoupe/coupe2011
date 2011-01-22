#ifndef QUADRATURE_ENCODER_H
#define QUADRATURE_ENCODER_H

void InitQEI(void);

sInt32 getHardwarePosQEI();

sInt32 getSoftwarePosQEI();
sInt32 getSoftwarePosQEI2();
#endif
