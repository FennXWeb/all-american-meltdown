#pragma once
#include "CoreMinimal.h"
namespace LWSurface26 {
using Polygon=TArray<FVector2D>;
inline double Side(FVector2D A,FVector2D B,FVector2D P){return (B.X-A.X)*(P.Y-A.Y)-(B.Y-A.Y)*(P.X-A.X);}
inline Polygon Clip(const Polygon& P,FVector2D A,FVector2D B,bool Inside){Polygon Out;if(P.IsEmpty())return Out;for(int I=0;I<P.Num();I++){FVector2D X=P[I],Y=P[(I+1)%P.Num()];double SX=Side(A,B,X),SY=Side(A,B,Y);bool IX=Inside?SX>=-.001:SX<=.001,IY=Inside?SY>=-.001:SY<=.001;if(IX)Out.Add(X);if(IX!=IY&&FMath::Abs(SX-SY)>1.e-9)Out.Add(X+(Y-X)*(SX/(SX-SY)));}return Out;}
inline double Area(const Polygon& P){double A=0;for(int I=0;I<P.Num();I++){auto X=P[I],Y=P[(I+1)%P.Num()];A+=X.X*Y.Y-X.Y*Y.X;}return FMath::Abs(A)*.5;}
inline TArray<Polygon> Subtract(const Polygon& P,const Polygon& Cutter){TArray<Polygon> Out;Polygon Rest=P;for(int I=0;I<Cutter.Num()&&Rest.Num()>=3;I++){auto A=Cutter[I],B=Cutter[(I+1)%Cutter.Num()];auto Outside=Clip(Rest,A,B,false);if(Outside.Num()>=3&&Area(Outside)>.01)Out.Add(Outside);Rest=Clip(Rest,A,B,true);}return Out;}
}
