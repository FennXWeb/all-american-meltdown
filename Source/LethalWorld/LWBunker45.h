#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LWBunker45State.h"
#include "LWBunker45.generated.h"
class ALWCharacter;class ALWWorld;class ALWWorldObject;class ALWVehicle;class ALWHUD;
struct FLWFurniture45 {FName Id,Model,Use;FString Name;int Cost;};
UCLASS()
class ALWBunker45:public AActor {
 GENERATED_BODY()
public:
 ALWBunker45();
 static ALWBunker45* Ensure(ALWCharacter* P);
 static const TArray<FLWFurniture45>& Catalog();
 UPROPERTY() TObjectPtr<ALWCharacter> Player;
 UPROPERTY() TObjectPtr<ALWWorld> World;
 UPROPERTY() TArray<TObjectPtr<AActor>> Built;
 UPROPERTY() TMap<FName,TObjectPtr<AActor>> Furniture;
 TMap<FName,FLWPlaced45> Originals;
 UPROPERTY() TObjectPtr<class UCameraComponent> BuildCamera;
 UPROPERTY() TObjectPtr<class UStaticMeshComponent> Preview;
 UPROPERTY() TObjectPtr<class UStaticMeshComponent> Lift;
 UPROPERTY() TObjectPtr<class UStaticMeshComponent> LiftGate;
 UPROPERTY() TObjectPtr<ALWWorldObject> CabButton;
 UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> Gates;
 UPROPERTY() TArray<int32> GateFloors;
 UPROPERTY() TObjectPtr<ALWVehicle> TransferCar;
 UPROPERTY() TObjectPtr<class UStaticMeshComponent> SurfacePlatform;
 TArray<TFunction<void()>> BuildQueue;
 struct FEntry46{FName Id;FVector2D Center;};TArray<FEntry46> Entries46;FName Focus46;
 void TerminalButton46(ALWHUD& H,FName Id,const FString& Label,float X,float Y,float W);void TickTerminal46();void Navigate46(FVector2D Direction);
 int Page=0,Tab=0,SelectedCatalog=-1,SelectedFloor=0,Floor=0,Destination=0;
 FName SelectedFurniture,GarageCar;
 FTransform Placement;
 bool WaitMouse45=false;
 bool ValidPlacement=false,MovingLift=false,Grid=true;
 float Yaw=0,LiftZ=-1980,Poll=0,TransferClock=0,UtilityClock=0;
 FVector Surface;
 FString Message;
 virtual void Tick(float Dt) override;
 void Rebuild();void QueueFloor(int Index,bool Garage=false);void ApplyFurniture();
 void Open(int NewTab=0);void Close();void Action(FName Id);void Draw(ALWHUD& HUD);
 void BeginEdit(int CatalogIndex=-1);void TickBuild(float Dt);bool Place();bool Dismantle();
 void Select();void GoFloor(int Index);void TickLift(float Dt);
 bool BuyExpansion(FName Id);bool StoreVehicle(ALWVehicle* Car);bool Retrieve(FName Id);
 bool Install(FName Id);bool Paint(int Index);bool Repair();
 void TickGarage(float Dt);void TickUtilities(float Dt);
 FVector UtilityPosition(FName Type)const;
 ALWWorldObject* Terminal(FName Id,FVector At,FName Action);
 class UStaticMeshComponent* Box(FVector At,FVector Size,FName Material,bool Collision=true);
 static FVector Center(){return FVector(-1700,1700,-2000);}
 static float FloorZ(int I){return -2000-I*460;}
};
