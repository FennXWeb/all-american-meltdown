#include "LWGeneration.h"
namespace LWGen {
// Included inside LWGen after FSite. All coordinates and IDs are independent of load order.
FVector2D CanadaCity68(){return LWNY69::Project(43.6532,-79.3832);}
double CanadaRoadY68(double X,double Y){
 const double T=(X-LWBorder51::North)/RegionSize;
 return Y+FMath::Sin(T*PI/3)*6000*FMath::Clamp(T,0.,1.);
}
void CanadaRegion68(FIntPoint R,int Seed,TArray<FRoad>& Roads,TArray<FSite>& Sites){
 const FVector2D City=CanadaCity68();
 // A kilometre-wide civic street plan with perimeter boulevards, short blocks, and a central green.
 if(FMath::Abs(R.X*RegionSize-City.X)<90000&&FMath::Abs(R.Y*RegionSize-City.Y)<90000){
  auto Own=[&](FVector2D P){return FIntPoint(FloorDiv(P.X+RegionSize*.5,RegionSize),FloorDiv(P.Y+RegionSize*.5,RegionSize))==R;};
  auto DryRoad=[&](FVector2D A,FVector2D B,float Width){const int Steps=FMath::CeilToInt((B-A).Size()/800);for(int I=0;I<Steps;I++){auto P=FMath::Lerp(A,B,double(I)/Steps),Q=FMath::Lerp(A,B,double(I+1)/Steps);if(Own((P+Q)*.5)&&LWNY69::WaterDepth(P)==0&&LWNY69::WaterDepth(Q)==0&&LWNY69::WaterDepth((P+Q)*.5)==0&&LWGeography84::Canada(P)&&LWGeography84::Canada(Q))Roads.Add({P,Q,Width,Width>1000});}};
  const FVector2D Relief=City+FVector2D(-4500,-4500);
  if(Own(Relief)){FSite S;S.Id=0xEC76FF00u;S.Type=2;S.Position=Relief;S.Size=FVector2D(2400,2000);S.Friendly=S.Canadian=true;S.PlaceName69=TEXT("TORONTO / NORTHBANK COMMUNITY HOUSE");Sites.Add(S);}
  for(int Row=-5;Row<=5;++Row){
   FVector2D A=City+FVector2D(Row*9000,-45000),B=City+FVector2D(Row*9000,45000);
   DryRoad(A,B,Row==0?1600.f:850.f);
   A=City+FVector2D(-45000,Row*9000);B=City+FVector2D(45000,Row*9000);
   DryRoad(A,B,Row==0?1600.f:850.f);
  }
  for(int X=-5;X<5;X++)for(int Y=-5;Y<5;Y++){
   FVector2D Center=City+FVector2D(X*9000+4500,Y*9000+4500);
   if(!Own(Center)||(X==-1&&Y==-1)||(X==0&&Y==0)||LWNY69::WaterDepth(Center)>0||!LWGeography84::Canada(Center))continue;
   FSite S;S.Position=Center;S.Id=Hash(X,Y,Seed,68000);S.Type=(X+Y+20)%4==0?2:(X+Y+20)%4==1?18:(X+Y+20)%4==2?4:6;
   S.Size=FVector2D(4200,4000);S.Friendly=true;S.Canadian=true;S.District=4;S.Floors=S.Type==6?4:1;S.AccessibleFloors=1;
   if(X==1&&Y==-2){S.Type=65;S.Floors=1;S.PlaceName69=TEXT("TORONTO CURRENCY EXCHANGE");}
   bool Dry=true;for(int A:{-1,1})for(int B:{-1,1}){const auto Corner=Center+FVector2D(S.Size.X*.5*A,S.Size.Y*.5*B);Dry&=LWNY69::WaterDepth(Corner)==0&&LWGeography84::Canada(Corner);}if(!Dry)continue;Sites.Add(S);DryRoad(Center+FVector2D(0,-4500),Center+FVector2D(0,-2450),450);
  }
 }
}

}
