#pragma once
#include "LWOpening.h"
namespace LWAppearance {
 void Build(AActor* Owner,class USceneComponent* Parent,const FLWIdentity& V,TArray<TObjectPtr<class UStaticMeshComponent>>& Out,bool FirstPerson=false);
 void StyleNPC(AActor* Owner,const TArray<TObjectPtr<class UStaticMeshComponent>>& Parts,FName Identity,bool Female,bool Raider=false,bool Undead=false);
}
