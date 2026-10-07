#pragma once
#include "LWZombie.h"
#include "LWResident.generated.h"
UCLASS()
class LETHALWORLD_API ALWResident:public ALWZombie {
 GENERATED_BODY()
public:
 UPROPERTY() TObjectPtr<class ALWVehicle> HomeRV66;
 bool TickRVHome66(class ALWCharacter* P);
 float FollowStuck45=0,FollowCheck45=0,FollowDistance45=0;FVector FollowPosition45=FVector::ZeroVector;
 void RecoverFollow45(float Dt,class ALWCharacter* P);
 UPROPERTY() TObjectPtr<class ALWVehicle> Riding;
 TArray<FVector> ActivitySpots;
 bool CommunityRoutine84=false;
 FVector FollowTarget=FVector::ZeroVector;
 float FollowTimer=0,LeashTime=0,ActivityTime=0;
 int32 Activity=0,SeatIndex=-1;
 TSharedPtr<struct FLWCompanionRoute> CompanionRoute;
 bool NavigateCompanion(FVector Goal,float Speed,float Dt);
 void ResetCompanionNavigation();
 TWeakObjectPtr<ALWZombie> CompanionThreat;
 float ThreatScanClock=0;
 bool TickCompanionThreat(float Dt,class ALWCharacter* P);
 void LeaveVehicle();void TickSocial(float Dt,class ALWCharacter* P,bool Following,int32 Bedroom);
 UPROPERTY() FName ResidentId;
 UPROPERTY() FName SettlementId;
 bool IsTownHostile()const;
 void AlertTown();
 bool DefendSettlement(class ALWCharacter* P);
 UPROPERTY() FName NpcRole;
 UPROPERTY() FString DisplayName;
 UPROPERTY() FString Subtitle;
 UPROPERTY() TObjectPtr<class ALWWorldObject> Shop;
 int32 Voice=0;
 float SubtitleTime=0,ChatterTime=3,DownTime=0,FireTime=0;
 void ConfigureResident(FName Id,FName Job,FString Name,int32 VoiceIndex,int32 BodyOverride=-1);
 void Say(const FString& Line);
 virtual void Tick(float Dt)override;
 virtual void EndPlay(const EEndPlayReason::Type Reason)override;
 virtual float TakeDamage(float D,const FDamageEvent& E,AController* I,AActor* C)override;
};
