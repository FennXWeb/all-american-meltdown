#pragma once
#include "LWZombie.h"
namespace LWVoice44 {
inline FName Human(bool Female){return Female?TEXT("HumanFemale44"):TEXT("HumanMale44");}
inline FName Enemy(ELWEnemyKind Kind,int Body=0){
 if(Kind==ELWEnemyKind::Raider)return Human(Body==1);
 const TCHAR* Slots[]={TEXT("Zombie"),TEXT("HumanMale44"),TEXT("DogGrowl"),TEXT("MannequinMove"),TEXT("MooseRoar"),TEXT("TitanRoar"),TEXT("DeathclawRoar"),TEXT("ScorpionHiss"),TEXT("KarenShriek"),TEXT("BehemothVoice44"),TEXT("ColossusVoice44"),TEXT("WorldEaterVoice44"),TEXT("Hornet57"),TEXT("Bear57"),TEXT("Rogue57")};
 return Slots[FMath::Clamp(int(Kind),0,14)];
}
inline FName Story(const FString& Name){return Human(Name.Contains(TEXT("Mara"))||Name.Contains(TEXT("Inez"))||Name.Contains(TEXT("Elsie"))||Name.Contains(TEXT("Voss")));}
}
