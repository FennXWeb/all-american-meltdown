#include "LWCampaign76Corridor.h"
#include "LWCampaignProduction77.h"
#include "LWNewYork69.h"
#include "LWGeneration.h"
const TArray<LWCampaign76::FStop>& LWCampaign76::Corridor(){static const TArray<FStop> Stops={
 {TEXT("bellwether"),TEXT("BELLWETHER SERVICE PLAZA"),0,LWNY69::Project(43.075,-75.98)+FVector2D(5000,7000)},
 {TEXT("rome"),TEXT("ROME / CANAL FREIGHT TERMINAL"),13,LWNY69::Project(43.1990,-75.4470)},
 {TEXT("guard"),TEXT("EASTERN CORRIDOR / CIVIC HOSPITAL"),2,LWNY69::Project(43.267,-75.511)},
 {TEXT("holding"),TEXT("TRANSFER ANNEX 14"),17,LWNY69::Project(43.288,-75.491)},
 {TEXT("tughill"),TEXT("TUG HILL / ROAD MAINTENANCE SHELTER"),1,LWNY69::Project(43.648,-75.802)},
 {TEXT("freehold"),TEXT("TUG HILL / PIKE WINTER COOPERATIVE"),7,LWNY69::Project(43.667,-75.835)},
 {TEXT("witness"),TEXT("TUG HILL / WARDEN'S CABIN"),8,LWNY69::Project(43.755,-75.684)},
 {TEXT("watertown"),TEXT("WATERTOWN / NORTHBANK RECEIVING STATION"),2,LWNY69::Project(43.970,-75.899)},
 {TEXT("ferry"),TEXT("THOUSAND ISLANDS / NIGHT FERRY WAREHOUSE"),13,LWNY69::Project(44.223,-76.077)},
 {TEXT("crossing"),TEXT("THOUSAND ISLANDS / AMERICAN STAGING GROUND"),16,LWNY69::Project(44.230,-76.088)},
 {TEXT("labor"),TEXT("ADIRONDACK TRANSFER WORKS"),3,LWNY69::Project(43.715,-75.065)},
 {TEXT("meridian"),TEXT("CAMP MERIDIAN / CONTINUITY RESORT"),21,LWNY69::Project(43.776,-74.896)},
 {TEXT("transport"),TEXT("MERIDIAN / EXECUTIVE TRANSPORT HALL"),13,LWNY69::Project(43.796,-74.884)},
 {TEXT("harbor"),TEXT("NIGHT FERRY / RIVER LANDING"),16,LWNY69::Project(44.236,-76.088)}
 };return Stops;}
void LWCampaign76::AddCorridor(TArray<LWGen::FSite>& Sites,TArray<LWGen::FRoad>& Roads){
 // Compressed narrative travel east of the existing lake mask. Cities and authored landmarks stay in place.
 // These are fictional story stops within the named regions, not surveyed replicas of public facilities.
 // The regional survey now provides the Rome/Tug Hill/Watertown approaches.
 auto Dry=[](FVector2D A,FVector2D B){const int N=FMath::Max(1,FMath::CeilToInt((B-A).Size()/1500));for(int I=0;I<=N;I++)if(LWNY69::WaterDepth(FMath::Lerp(A,B,double(I)/N))>0)return false;return true;};
 for(int I=0;I<Corridor().Num();I++){
  const auto& Stop=Corridor()[I];LWGen::FSite S;S.Position=Stop.Position;S.Type=Stop.Type;S.Size=LWProduction77::Centered(Stop.Id)?LWProduction77::Parcel(Stop.Id):Stop.Id==TEXT("transport")?FVector2D(4800,4200):LWPlaces::Size(S.Type);S.Yaw=0;S.Id=0xEC760000u+I;S.Friendly=true;S.PlaceName69=Stop.Name;S.Floors=S.Type==21?5:1;S.AccessibleFloors=S.Floors;
  const FVector2D Door=LWGen::Entrance(S);FVector2D Join=Door;double Best=DBL_MAX;
  for(const auto& R:Roads){FVector2D Q;const double Dist=LWGen::DistanceToSegment(Door,R,&Q);if(Dist<Best&&!LWGen::RoadOverlaps(S,{Q,Door,400,false})&&Dry(Q,Door)){Best=Dist;Join=Q;}}
  Sites.Add(S);if(Best<DBL_MAX)Roads.Add({Join,Door,400,false});
  if(Stop.Id==TEXT("bellwether")){
   // A visible public road is 21m from the start, with services across the street.
   const FVector2D A=Stop.Position+FVector2D(-7500,-2100),B=Stop.Position+FVector2D(7500,-2100);
   Roads.Add({A,B,600,false});Roads.Add({Stop.Position+FVector2D(0,-600),Stop.Position+FVector2D(0,-2100),320,false});
   if(Best<DBL_MAX)Roads.Add({Join,A,500,false});
   const int Types[]={4,8,7,0};const FVector2D Offsets[]={{-3900,-4350},{0,-4250},{4100,-4450},{5100,700}};
   const TCHAR* Names[]={TEXT("BELLWETHER DINER"),TEXT("MILLER'S COTTAGE"),TEXT("BELLWETHER RANCH"),TEXT("BELLWETHER FUEL")};
   for(int J=0;J<4;J++){LWGen::FSite Lot;Lot.Id=0xEC78A000u+J;Lot.Type=Types[J];Lot.Position=Stop.Position+Offsets[J];Lot.Size=LWPlaces::Size(Lot.Type);Lot.Yaw=J==3?0:180;Lot.Friendly=true;Lot.PlaceName69=Names[J];Sites.Add(Lot);const auto E=LWGen::Entrance(Lot);Roads.Add({E,{E.X,Stop.Position.Y-2100},320,false});}
  }
 }
}
