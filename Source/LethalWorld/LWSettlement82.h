#pragma once
#include "CoreMinimal.h"
#include "LWWorldObject.h"
#include "LWSettlement82State.h"
#include "LWGeneration.h"
#include "LWSettlement82.generated.h"
class ALWCharacter;class ALWResident;class ALWVehicle;class ALWHUD;
struct FLWBuild82 {
 FName Id,Model,Use;FString Name;int Category=0,Cost=1;FVector Size=FVector(100);
 bool Structural()const{return Category==0;}
 bool Wall()const{return Use==TEXT("wall")||Use==TEXT("door")||Use==TEXT("window");}
 bool Deck()const{return Use==TEXT("foundation")||Use==TEXT("floor")||Use==TEXT("roof");}
 bool Job()const{return Category==3;}
};
namespace LWBuilding82 {
 const TArray<FLWBuild82>& Catalog();
 const FLWBuild82* Find(FName Id);
 FName Finish(int Index);const TCHAR* FinishName(int Index);
 FLinearColor Color(int Index);const TCHAR* ColorName(int Index);
 FVector Extent(const FLWBuild82& D);FVector Offset(const FLWBuild82& D);
 bool Overlap(const FTransform& A,FVector HalfA,const FTransform& B,FVector HalfB,double Margin=2);
 int Beds(const FLWClaim82& Claim);
 bool DependsOn(const FLWClaim82& Claim,FName Support);
 FTransform RoadSnap(FVector Aim,const LWGen::FRoad& Road,FVector Size);
}
UCLASS()
class ALWBuildPiece82:public ALWWorldObject {
 GENERATED_BODY()
public:
 UPROPERTY() FName Claim;
 UPROPERTY() FName CatalogId;
 UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> Shell;
 void Build(class ALWSettlement82* Manager,const FLWConstruction82& Piece);
 class UStaticMeshComponent* Panel(FVector At,FVector Size,FName Material,int Paint=0,FRotator Rotation=FRotator::ZeroRotator);
 virtual void Use(ALWCharacter* P)override;
 virtual FString Prompt()const override;
};
UCLASS()
class ALWSettlement82:public AActor {
 GENERATED_BODY()
public:
 ALWSettlement82();
 static ALWSettlement82* Ensure(ALWCharacter* P);
 UPROPERTY() TObjectPtr<ALWCharacter> Player;
 UPROPERTY() TObjectPtr<ALWWorld> World;
 UPROPERTY() TObjectPtr<class UCameraComponent> Camera;
 UPROPERTY() TObjectPtr<class UStaticMeshComponent> Preview;
 UPROPERTY() TMap<FName,TObjectPtr<ALWBuildPiece82>> Actors;
 UPROPERTY() TMap<FName,TObjectPtr<ALWResident>> People;
 FName Selected,Moving,Support,SelectedResident,SelectedLot;
 int Category=0,Page=0,ResidentPage=0,Panel=0,SelectedCatalog=0,Outside=0,Inside=1,OutsideColor=0,InsideColor=0;
 bool Building=false,Palette=false,Claiming=false,Snap=true,RoadsSnap=true,Valid=false,WaitClick=true;
 float Yaw=0,Height=0,Poll=0,SaveClock=0;
 FTransform Placement;FString Message;
 TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;
 TArray<TPair<FName,FName>> Queue;
 TSet<FName> Loaded;
 FLWClaim82* State();const FLWClaim82* State()const;
 void Reset();void Open(FName Id=NAME_None);void Close();bool BeginClaim();void BeginBuild(int Index=-1);void SetPalette(bool Show);
 virtual void Tick(float Dt)override;
 virtual void EndPlay(const EEndPlayReason::Type Reason)override;
 bool Input(FKey Key,bool Down);
 void TickBuild(float Dt);void Aim(FVector Point,FVector Normal);void SelectPiece();
 bool ClaimAt(FVector At);bool ValidateClaim(FVector At,FString& Reason)const;
 bool Validate(const FLWBuild82& D,const FTransform& T,FName Parent,FName Ignore,FString& Reason)const;
 bool Place();bool Dismantle();bool Paint();void Refresh(FName Piece);void Stream();
 int Scrap()const;bool Spend(int Cost);void Deposit();
 void Economy(float Seconds);bool Recruit(FName ClaimId);bool Assign(FName Resident,FName Station);bool Crew(FName Resident,bool Follow);
 bool ResidentTick(ALWResident* NPC,float Dt);FVector Home(FName Resident)const;FName HomeClaim(FName Resident)const;
 bool Park(ALWVehicle* Car,FName Lot);bool ReleaseCar(FName Id);void TickParking(float Seconds);
 FVector Bay(const FLWConstruction82& Lot,int Index,int Size)const;
 void Draw(ALWHUD& H);bool Action(FName Id);
};
