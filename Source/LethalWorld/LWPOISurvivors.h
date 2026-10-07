#pragma once
#include "LWGeneration.h"
class ALWChunk;
class ALWWorld;
namespace LWPOISurvivors {
inline bool Eligible(const LWGen::FSite& S){
 if(S.SettlementBuilding)return false;
 if(LWPlaces::Expanded(S.Type))return true;
 if(S.Friendly)return false; // Existing friendly/authored hubs own their own cast.
 switch(S.Type){case 0:case 1:case 3:case 4:case 10:case 11:case 12:case 13:case 14:case 16:return true;default:return false;}
}
inline bool Selected(const LWGen::FSite& S,int Seed){return Eligible(S)&&LWGen::Hash(int32(S.Id),S.Type,Seed,30020)%100<36;}
FString Name(uint32 SiteId,int Role);
void Spawn(ALWChunk* Chunk,ALWWorld* World,const LWGen::FSite& Site,int Role68=-1);
}
