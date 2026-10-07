#pragma once
#include "CoreMinimal.h"
namespace LWTraffic {
inline bool IsAircraft(FName Id){return Id==TEXT("private_jet")||Id==TEXT("airbus")||Id==TEXT("luxury_airbus");}
inline bool IsCamper(FName Id){return Id==TEXT("rv")||Id==TEXT("solstice_rv");}
inline bool IsElectric(FName Id){return Id==TEXT("solstice_rv")||Id==TEXT("apex_ev");}
struct FSpec {
 const TCHAR* Id; const TCHAR* Name;
 float Weight,Acceleration,MaxSpeed,Reverse,Wheelbase,Steering,Brake,Grip;
 int Seats,CargoW,CargoH;
 float HalfLength,HalfWidth,Height;
 const TCHAR* Feature;
};
inline const TArray<FSpec>& Specs(){static const TArray<FSpec> S={
 {TEXT("sedan"),TEXT("SEDAN"),30,390,2400,500,270,34,1500,.85f,4,10,8,205,87,145,TEXT("glove")},
 {TEXT("boxtruck"),TEXT("BOX TRUCK"),8,230,1800,380,480,31,1100,.65f,2,12,28,380,115,330,TEXT("ramp")},
 {TEXT("police"),TEXT("POLICE CRUISER"),5,560,3200,550,290,34,1750,.95f,4,10,10,230,95,155,TEXT("siren")},
 {TEXT("rv"),TEXT("MOTORHOME"),5,185,1800,300,737,29,1050,.65f,6,12,32,605,128,353,TEXT("rest")},
 {TEXT("bus"),TEXT("SCHOOL BUS"),4,180,1700,300,640,28,1000,.62f,11,12,22,490,120,310,TEXT("door")},
 {TEXT("van"),TEXT("CARGO VAN"),15,310,2200,450,340,33,1350,.78f,2,12,16,275,105,245,TEXT("door")},
 {TEXT("pickup"),TEXT("PICKUP"),13,390,2600,520,350,35,1400,.9f,4,12,12,270,105,170,TEXT("tailgate")},
 {TEXT("dirtbike"),TEXT("DIRT BIKE"),8,700,2700,180,145,38,1700,1.1f,1,4,4,115,34,120,TEXT("stand")},
 {TEXT("suv"),TEXT("SUV"),10,380,2600,480,300,33,1450,.93f,7,12,12,245,103,200,TEXT("4wd")},
 {TEXT("muscle"),TEXT("MUSCLE CAR"),4,690,3500,550,285,31,1400,.74f,4,9,7,235,100,140,TEXT("boost")},
 {TEXT("supercar"),TEXT("SUPERCAR"),1,1050,4400,500,270,28,2200,1.12f,2,6,5,225,100,120,TEXT("sport")},
 {TEXT("apc"),TEXT("ARMORED PERSONNEL CARRIER"),.18f,230,2000,350,420,29,1300,1.05f,8,12,22,360,125,250,TEXT("turret")},
 {TEXT("armoredtruck"),TEXT("ARMORED TRUCK"),.35f,280,2200,400,370,30,1500,.9f,2,12,24,310,108,250,TEXT("door")},
 {TEXT("technical"),TEXT("ARMED PICKUP"),.6f,410,2600,480,350,34,1450,.9f,3,12,10,270,105,195,TEXT("turret")},
 {TEXT("helicopter"),TEXT("UTILITY HELICOPTER"),0,650,6200,1800,300,40,1400,1,4,12,12,440,130,310,TEXT("flight")},
 {TEXT("solstice_rv"),TEXT("SOLSTICE ELECTRIC RESIDENCE"),.7f,310,2350,340,792,28,1450,.88f,6,12,38,665,146,375,TEXT("autopilot74")},
 {TEXT("apex_ev"),TEXT("APEX ELECTRIC HYPERCAR"),.35f,1550,5600,620,285,30,2700,1.28f,2,8,7,242,105,128,TEXT("autopilot74")},
 {TEXT("private_jet"),TEXT("AURELIA PRIVATE JET"),0,420,16000,200,1000,25,650,1,8,12,32,1200,1100,540,TEXT("flight84")},
 {TEXT("airbus"),TEXT("AIRBUS 100"),0,340,15000,160,1300,20,580,1,100,12,48,1900,1800,880,TEXT("flight84")},
 {TEXT("luxury_airbus"),TEXT("AURELIA SKY RESIDENCE"),0,370,15500,160,1300,20,600,1,16,12,48,1900,1800,880,TEXT("flight84")}
 };return S;}
inline const FSpec& Get(FName Id){for(const auto& S:Specs())if(Id==S.Id)return S;return Specs()[0];}
inline FName Choose(uint32 Hash){float Total=0;for(const auto& S:Specs())Total+=S.Weight;float Roll=(Hash%100000)/100000.f*Total;for(const auto& S:Specs()){Roll-=S.Weight;if(Roll<0)return S.Id;}return TEXT("sedan");}
inline float FrontOffset(const FSpec& S){return S.HalfLength-(FName(S.Id)==TEXT("solstice_rv")?255:205);}
// Bicycle steering with a lateral-acceleration cap: tight at parking speed, stable at speed.
inline float YawRate(const FSpec& S,float Speed,float WheelAngle,bool Traction){float Raw=Speed/S.Wheelbase*FMath::Tan(FMath::DegreesToRadians(WheelAngle));float Limit=980.f*S.Grip*(Traction?1.15f:1.f)/FMath::Max(180.f,FMath::Abs(Speed));return FMath::Clamp(Raw,-Limit,Limit);}
}
