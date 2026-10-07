#include "LWWorld.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/LightComponent.h"
#include "LWCharacter.h"
#include "Kismet/GameplayStatics.h"
void ALWChunk::BeginProp60(FVector Local,const TCHAR* Kind){
 if(!BuildingProp60.IsNone())return;
 const FVector P=GetActorTransform().TransformPosition(Local);
 BuildingProp60=FName(*FString::Printf(TEXT("roadprop60_%s_%lld_%lld_%lld"),Kind,FMath::RoundToInt64(P.X),FMath::RoundToInt64(P.Y),FMath::RoundToInt64(P.Z)));
 BeforeProp60.Empty();TArray<UActorComponent*> Parts;GetComponents(Parts);for(auto* C:Parts)BeforeProp60.Add(C);
 FBreakable60 B;B.Id=BuildingProp60;B.Anchor=P;Breakables60.Add(B);
}
void ALWChunk::EndProp60(){
 if(BuildingProp60.IsNone()||Breakables60.IsEmpty())return;auto& B=Breakables60.Last();TArray<USceneComponent*> Parts;GetComponents(Parts);
 const bool Gone=LightingWorld&&LightingWorld->PropStates.FindRef(B.Id)!=0;
 // Vehicle suspension probes query static/dynamic objects. Signals are neither
 // drivable ground nor vehicle obstacles; keep foot and bullet interaction.
 if(B.Id.ToString().StartsWith(TEXT("roadprop60_signal_")))for(auto* C:Parts)if(!BeforeProp60.Contains(C))if(auto* P=Cast<UPrimitiveComponent>(C)){
  P->ComponentTags.Add(TEXT("TrafficSignal83"));P->SetCollisionObjectType(ECC_Destructible);
  if(P->GetCollisionEnabled()!=ECollisionEnabled::NoCollision){P->SetCollisionResponseToAllChannels(ECR_Ignore);P->SetCollisionResponseToChannel(ECC_Pawn,ECR_Block);P->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);}
  // ISM instances copy their body filters when created. Updating only the
  // component's template leaves already-created mast/arm bodies blocking cars.
  if(P->IsPhysicsStateCreated())P->RecreatePhysicsState();
 }
 for(auto* C:Parts)if(!BeforeProp60.Contains(C)){B.Parts.Add(C);C->ComponentTags.Add(B.Id);if(Gone){C->SetVisibility(false,true);if(auto* P=Cast<UPrimitiveComponent>(C))P->SetCollisionEnabled(ECollisionEnabled::NoCollision);}}
 if(Gone)B.Fall=1;BuildingProp60=NAME_None;BeforeProp60.Empty();
}
bool ALWChunk::BreakProp60(UPrimitiveComponent* Hit,FVector Direction,float Speed){
 if(!Hit||Speed<350||!LightingWorld)return false;
 for(auto& B:Breakables60)if(B.Fall<0&&Hit->ComponentHasTag(B.Id)){
  auto* Pivot=NewObject<USceneComponent>(this);Pivot->SetupAttachment(RootComponent);Pivot->RegisterComponent();Pivot->SetWorldLocation(B.Anchor);B.Pivot=Pivot;
  for(auto& Ref:B.Parts)if(auto* C=Ref.Get()){C->ComponentTags.Add(TEXT("Broken60"));C->SetMobility(EComponentMobility::Movable);if(auto* P=Cast<UPrimitiveComponent>(C))P->SetCollisionEnabled(ECollisionEnabled::NoCollision);if(auto* L=Cast<ULightComponent>(C))L->SetVisibility(false);C->AttachToComponent(Pivot,FAttachmentTransformRules::KeepWorldTransform);}
  Direction.Z=0;Direction.Normalize();B.Target=FQuat(FVector::CrossProduct(FVector::UpVector,Direction).GetSafeNormal(),FMath::DegreesToRadians(88.f));B.Fall=0;
  LightingWorld->PropStates.Add(B.Id,1);LightingWorld->Sound(TEXT("CarImpact"),B.Anchor,.9f);
  if(auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0)))P->RequestSave40();return true;
 }return false;
}
void ALWChunk::TickProps60(float Dt){for(auto& B:Breakables60)if(B.Fall>=0&&B.Fall<1&&B.Pivot.IsValid()){B.Fall=FMath::Min(1.f,B.Fall+Dt*1.7f);B.Pivot->SetWorldRotation(FQuat::Slerp(FQuat::Identity,B.Target,B.Fall*B.Fall));}}
