#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LWCampaignProduction77.generated.h"
class ALWCampaign76;class ALWCampaignPerson76;class ALWWorld;class ALWChunk;class UStaticMeshComponent;class UPointLightComponent;
namespace LWGen {struct FSite;}
namespace LWProduction77 {
 bool Dress(ALWChunk* C,ALWWorld* W,const LWGen::FSite& S);
 bool Centered(FName Site);FVector2D Parcel(FName Site);
 // Shared by runtime, save reconstruction and analytical regression tests.
 FTransform FerryPose(float Seconds);float FerryDuration();
 FVector ArmElbow(FVector Shoulder,FVector Target,FVector Pole,float Upper,float Lower);
 float ContactEnvelope(float Time,float Duration);
}
USTRUCT()
struct FLWCrowd77 {
 GENERATED_BODY()
 UPROPERTY() TObjectPtr<ALWCampaignPerson76> Person;
 TArray<FVector> Route;int32 Next=0;float Delay=0,Stalled=0,LastDistance=BIG_NUMBER;bool Complete=false;
};
UCLASS()
class ULWCampaignProduction77:public UActorComponent {
 GENERATED_BODY()
public:
 ULWCampaignProduction77();
 UPROPERTY() TObjectPtr<ALWCampaign76> Director;
 UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> Meshes;
 UPROPERTY() TArray<TObjectPtr<UPointLightComponent>> Lights;
 UPROPERTY() TObjectPtr<AActor> Boat;
 UPROPERTY() TObjectPtr<UStaticMeshComponent> BoatDeck;
 UPROPERTY() TObjectPtr<UStaticMeshComponent> Gangway;
 UPROPERTY() TObjectPtr<UStaticMeshComponent> Case;
 UPROPERTY() TObjectPtr<UStaticMeshComponent> Tool;
 UPROPERTY() TObjectPtr<UStaticMeshComponent> Cuffs;
 UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> Mechanisms;
 UPROPERTY() TArray<FLWCrowd77> Crowd;
 UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> WreckPanels;
 TArray<FTransform> WreckRest;
 TWeakObjectPtr<ALWCampaignPerson76> Performer,Recipient;
 FName Performance;float PerformanceTime=0,PerformanceDuration=0;
 FVector WorkContact=FVector::ZeroVector;bool WorkReady=true;
 float MechanismPhase=0,LastDamageTime=0;bool Boarding=false;
 void Build();void Clear();void Update(float Dt);void Beat(FName Scene,int32 Index);
 void Work(FName Action,float Time,float Duration);void StopPerformance();
 bool BeginSailing();void UpdateSailing(float Dt);void RestoreSailing();
 void BuildCarrier();void UpdateCarrier(float Dt);void UpdateCrowd(float Dt);
 bool CrowdReady()const;void StartCrowd(FName Kind);
 void PlaceCast();void StartPerformance(FName Type,FName Who,FName Other,float Duration=4);
 UStaticMeshComponent* Add(FName Mesh,FVector At,FRotator Rotation=FRotator::ZeroRotator,FVector Scale=FVector(1),bool Collision=true,AActor* Parent=nullptr);
private:
 void Contact(ALWCampaignPerson76* Person,FVector Target,int Side=1,float Weight=1);
 void BuildCanal();void BuildFerry();
};
