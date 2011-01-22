#ifndef TRAJECTORY_H
#define TRAJECTORY_H
#include "ALTypes.h"

// Oui mais bon
#define DIMENSION_Y 3000
#define DIMENSION_X 2100

typedef struct point 
{
  float x;
  float y;
//  sInt16 last; // flag to know if deceleration is needed<
} point; 


uInt16 insidePoint(point pA);
uInt16 orientationHack(void);
void nextTargetPointManagement(float pEcart_d, uInt16 pPlanificationAsked);

/*
#define MAX_NB_POINT 30

typedef struct trajectory
{
  point list[MAX_NB_POINT];
  sInt16 index;
  sInt16 size;
}trajectory;

void initTrajectory(trajectory * pTrajectory)
{
  pTrajectory->index=0;
}

point getNextPoint(trajectory * pTrajectory)
{
  if (pTrajectory->index < pTrajectory->size)
  {
    
  }  
} 

*/
#endif
