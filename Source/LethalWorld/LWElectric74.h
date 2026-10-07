#pragma once
#include "CoreMinimal.h"
namespace LWElectric74 {
inline float Capacity(bool Coach){return Coach?420.f:125.f;}
// Integral of a 06:00-18:00 half-sine. Stable across midnight and time skips.
inline double SunPrimitive(double Hour){
 const double Day=FMath::FloorToDouble(Hour/24.),T=Hour-Day*24.;
 return Day*24./PI+(T<=6?0:T>=18?24./PI:12./PI*(1-FMath::Cos((T-6)*PI/12.)));
}
inline double SunHours(double Start,double End){return End>Start?FMath::Max(0.,SunPrimitive(End)-SunPrimitive(Start)):0.;}
inline float Sunlight(float Hour){return FMath::Max(0.f,FMath::Sin((Hour-6)*PI/12));}
inline FVector CabinScale(bool Coach){return Coach?FVector(1.075,1.12,1.04):FVector(1);}
}
