#pragma once
#include "LWGeneration.h"

// Names and survey addresses are pure: no global counter or streaming-order dependency.
namespace LWSites {
inline FString Label(const LWGen::FSite& Site) {
 if(!Site.PlaceName69.IsEmpty())return Site.PlaceName69;
 if(Site.Canadian){const TCHAR* Names[]={TEXT("TORONTO HEALTH"),TEXT("NORTHSTAR OUTFITTERS"),TEXT("CEDAR TABLE CAFE"),TEXT("TORONTO RESIDENCES")};return FString::Printf(TEXT("%s %03u"),Names[Site.Type==2?0:Site.Type==18?1:Site.Type==4?2:3],Site.Id%1000);}
 if(LWLandmarks::Unique(Site.Type))return LWPlaces::Name(Site.Type);
 static const TCHAR* First[]={TEXT("CEDAR"),TEXT("IRON"),TEXT("COPPER"),TEXT("WILLOW"),TEXT("COLD"),TEXT("SILVER"),TEXT("OAK"),TEXT("RED"),TEXT("ASH"),TEXT("PINE"),TEXT("STONE"),TEXT("GOLDEN"),TEXT("BLUE"),TEXT("FOX"),TEXT("RAVEN"),TEXT("MAPLE"),TEXT("BIRCH"),TEXT("SOUTH"),TEXT("NORTH"),TEXT("EAST"),TEXT("WEST"),TEXT("ELK"),TEXT("CROW"),TEXT("SUN"),TEXT("MOON"),TEXT("DEEP"),TEXT("SALT"),TEXT("RUST"),TEXT("CLEAR"),TEXT("BLACK"),TEXT("WHITE"),TEXT("MILL")};
 static const TCHAR* Last[]={TEXT("CREEK"),TEXT("RIDGE"),TEXT("VALE"),TEXT("HOLLOW"),TEXT("CROSSING"),TEXT("GROVE"),TEXT("FALLS"),TEXT("POINT"),TEXT("HAVEN"),TEXT("RUN"),TEXT("BLUFF"),TEXT("FIELD"),TEXT("BROOK"),TEXT("SPRINGS"),TEXT("FORD"),TEXT("HILL")};
 static const TCHAR* Type[]={TEXT("SERVICE STATION"),TEXT("MOTEL"),TEXT("CLINIC"),TEXT("DEPOT"),TEXT("DINER"),TEXT("ARCADE"),TEXT("APARTMENTS"),TEXT("RANCH"),TEXT("COTTAGE"),TEXT("TOWNHOUSE"),TEXT("MEGAMARKET"),TEXT("PLAZA"),TEXT("FURNISHINGS"),TEXT("WAREHOUSE"),TEXT("BURGER STOP"),TEXT("DRIVE-IN"),TEXT("PARKING"),TEXT("PRECINCT"),TEXT("ATELIER"),TEXT("TAVERN"),TEXT("FINANCIAL TOWER"),TEXT("CASINO RESORT"),TEXT("PUMPWORKS"),TEXT("QUARANTINE"),TEXT("REMAND CENTER"),TEXT("DEPOSITORY"),TEXT("BROADCAST CENTER"),TEXT("STEELWORKS"),TEXT("RESEARCH ANNEX"),TEXT("MISSILE COMMAND"),TEXT("INTERCHANGE"),TEXT("ARCOLOGY"),TEXT("INTERNATIONAL AIRPORT")};
 const uint32 H=LWGen::Hash(int32(Site.Id),Site.Type,9173);
 FString Kind=Site.Type<UE_ARRAY_COUNT(Type)?Type[FMath::Max(0,Site.Type)]:Site.Type==53?TEXT("SURVIVAL BUNKER"):Site.Type==54?TEXT("CAVERNS"):Site.Type==55?TEXT("SERVICE TUNNELS"):Site.Type==56?TEXT("RESIDENTIAL TOWER"):Site.Type==57?TEXT("MEDICAL TOWER"):Site.Type==58?TEXT("SHOPPING MALL"):Site.Type==59?TEXT("PENITENTIARY"):Site.Type==60?TEXT("CHURCH"):Site.Type==61?TEXT("MOTORS"):Site.Type==62?TEXT("HARDWARE"):Site.Type==64?TEXT("MILITARY BASE"):Site.Type==65?TEXT("RESERVE BANK"):Site.Type==66?TEXT("SHOOTING RANGE"):Site.Type==67?TEXT("SPORTING GOODS"):TEXT("ADVENTURE PARK");
 return FString::Printf(TEXT("%s %s %s"),First[H%32],Last[(H>>5)%16],*Kind);
}
inline FString Address(const LWGen::FSite& Site) {
 // Centimeter survey coordinates distinguish even sites with the same 32-bit hash.
 return FString::Printf(TEXT("%lld%s %lld%s"),FMath::Abs(FMath::RoundToInt64(Site.Position.X)),Site.Position.X<0?TEXT("W"):TEXT("E"),FMath::Abs(FMath::RoundToInt64(Site.Position.Y)),Site.Position.Y<0?TEXT("S"):TEXT("N"));
}
inline FString Name(const LWGen::FSite& Site) { return LWLandmarks::Unique(Site.Type)?Label(Site):Label(Site)+TEXT(" / ")+Address(Site); }
}
