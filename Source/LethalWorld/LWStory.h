#pragma once
#include "LWWorldObject.h"
#include "LWResident.h"
#include "LWStoryState.h"
#include "LWStory.generated.h"

UCLASS()
class LETHALWORLD_API ALWStoryNode:public ALWWorldObject {
 GENERATED_BODY()
public:
 UPROPERTY() TObjectPtr<class ALWStoryDirector> Director;
 FName Action;FString Label;float Durability52=100;
 virtual float TakeDamage(float D,const FDamageEvent& E,AController* I,AActor* C) override;
 virtual void Use(class ALWCharacter* P)override;
 virtual FString Prompt()const override;
};
UCLASS()
class LETHALWORLD_API ALWStoryPerson:public ALWResident {
 GENERATED_BODY()
public:
 virtual void Tick(float Dt)override;
};
UCLASS()
class LETHALWORLD_API ALWStoryEnemy:public ALWZombie {
 GENERATED_BODY()
public:
 UPROPERTY() TObjectPtr<class ALWStoryDirector> Director;
 int32 Boss=0,EncounterIndex52=0;
 virtual void Tick(float Dt) override;
 virtual float TakeDamage(float D,const FDamageEvent& E,AController* I,AActor* C)override;
};
struct FLWStoryShot {FString Speaker,Line;float Duration=6;FVector From,To,Look;};
UCLASS()
class LETHALWORLD_API ALWStoryDirector:public AActor {
 GENERATED_BODY()
public:
 ALWStoryDirector();
 UPROPERTY() TObjectPtr<class ALWCharacter> Player;
 UPROPERTY() TObjectPtr<class ALWWorld> World;
 UPROPERTY() TObjectPtr<class UCameraComponent> Camera;
 UPROPERTY() TObjectPtr<class UAudioComponent> SceneAudio;
 UPROPERTY() TObjectPtr<class UPointLightComponent> SceneLight;
 UPROPERTY() TMap<int32,TObjectPtr<class ALWChunk>> Sites;
 UPROPERTY() TArray<TObjectPtr<AActor>> Actors;
 UPROPERTY() TArray<TObjectPtr<ALWStoryEnemy>> Enemies;
 UPROPERTY() TMap<FName,TObjectPtr<ALWStoryPerson>> People;
 UPROPERTY() TArray<TObjectPtr<ALWStoryNode>> Nodes;
 TArray<FLWStoryShot> Shots;
 int32 Applied=-1,ShotIndex=0;float ShotTime=0,Clock=0;bool InScene=false,Choosing=false,Failed=false;
 FString Speech,VoiceName;TArray<FString> Choices;
 FName Conversation;
 FLWStoryState& State()const;
 static ALWStoryDirector* Ensure(class ALWCharacter* P);
 double CombatGrace52=0;
 void Migrate52();void SafeRecovery52();FVector Objective52(FName Action)const;
 void Start(bool Fresh);void Advance();void ApplyStage();void Route();
 bool Activate(FName Action);bool Available(FName Action)const;
 void SpeakTo(ALWResident* N);void Choose(int32 I);void FinishScene();void Scene(int32 Stage);
 void Confiscate();void RecoverGear();void RestorePrison();void Claim();
 bool Locked()const{return InScene||Choosing||Failed;}
 int32 Remaining()const;
 FVector Target()const;FVector At(int32 Site,FVector Local=FVector::ZeroVector)const;
 TMap<int32,int32> SiteRevisions38;
 int32 SiteRevision38(int32 I) const;
 void BuildSite(int32 Index);void ClearActors(bool All=true);
 UPROPERTY() TObjectPtr<class UStaticMeshComponent> Transport;
 UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> Fire;
 void AnimateScene(float Dt);
void SpawnStage();
 ALWStoryPerson* Person(FName Id,const FString& Name,FVector At,int Voice=0);
 ALWStoryNode* Node(FName Action,const FString& Label,FVector At,FName Mesh=TEXT("RadioV4"));
 ALWStoryEnemy* Enemy(int32 Index,FVector At,bool Armored=false,int32 Boss=0);
 virtual void Tick(float Dt)override;
 virtual void EndPlay(const EEndPlayReason::Type R)override;
};
