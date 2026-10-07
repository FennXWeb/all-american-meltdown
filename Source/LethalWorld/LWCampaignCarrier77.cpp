#include "LWCampaignProduction77.h"
#include "LWCampaign76.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWNPCLife.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LWVehicleExplosion68.h"
#include "LWWorldTextComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"

void ULWCampaignProduction77::BuildCarrier(){
 auto* D=Director.Get();if(!D)return;
 D->Carrier=GetWorld()->SpawnActor<ALWWorldObject>(D->At(FVector(0,720,0)),LWCampaign76::Frame(TEXT("transport")).Rotator());if(!D->Carrier)return;
 auto* A=D->Carrier.Get();A->World=D->World;A->Kind=ELWObjectKind::Sign;A->SetActorTickEnabled(false);A->Body->SetStaticMesh(D->World->Mesh(TEXT("CarrierHull77")));A->Body->SetCollisionProfileName(TEXT("BlockAll"));D->Actors.Add(A);D->CarrierOrigin=A->GetActorLocation();
 for(int Side:{-1,1})D->CarrierShell.Add(Add(TEXT("CarrierSide77"),FVector(Side*248,0,188),FRotator::ZeroRotator,FVector(1),true,A));
 D->CarrierShell.Add(Add(TEXT("CarrierShutter77"),FVector(0,450,314),FRotator::ZeroRotator,FVector(1),true,A));
 D->CarrierShutter=Add(TEXT("CarrierShutter77"),FVector(0,-450,314),FRotator::ZeroRotator,FVector(1),true,A);
 D->CarrierShell.Add(Add(TEXT("CarrierRoof77"),FVector(0,0,450),FRotator::ZeroRotator,FVector(1),true,A));
 // Six steps, 30cm tread and 30cm rise avoided: use nine comfortable 20cm rises.
 for(int I=0;I<9;I++){auto* Step=Add(TEXT("Cube"),FVector(0,-745+I*33,10+I*10),FRotator::ZeroRotator,FVector(1.6,.34,(I+1)*.2),true,A);if(Step)Step->SetMaterial(0,D->World->Material(TEXT("Steel")));}
 for(int Side:{-1,1})for(int I=0;I<3;I++){
  auto* Glass=Add(TEXT("Cube"),FVector(Side*247,-270+I*270,332),FRotator::ZeroRotator,FVector(.045,2.35,1.15),false,A);if(Glass)Glass->SetMaterial(0,D->World->Material(TEXT("WindowGlass")));D->CarrierShell.Add(Glass);
 }
 Add(TEXT("ExecutiveDesk77"),FVector(0,90,190),FRotator::ZeroRotator,FVector(1),true,A);
 Add(TEXT("ExecutiveChair77"),FVector(0,220,190),FRotator::ZeroRotator,FVector(1),true,A);
 Add(TEXT("ContinuityFlag77"),FVector(-205,360,190),FRotator::ZeroRotator,FVector(.9),false,A);
 Add(TEXT("Bookcase65"),FVector(190,260,190),FRotator(0,-90,0),FVector(.85),true,A);
 Add(TEXT("Sideboard65"),FVector(-186,230,190),FRotator(0,90,0),FVector(.75),true,A);
 Add(TEXT("TableLampV13"),FVector(-85,98,282),FRotator::ZeroRotator,FVector(.85),false,A);
 Add(TEXT("Dossier52"),FVector(25,98,282),FRotator::ZeroRotator,FVector(.8),false,A);
 Add(TEXT("Case77"),FVector(100,260,230),FRotator::ZeroRotator,FVector(1),false,A);
 for(int Side:{-1,1})Add(TEXT("Armchair65"),FVector(Side*155,-160,190),FRotator(0,Side*90,0),FVector(.8),true,A);
 Add(TEXT("CoffeeTable65"),FVector(0,-180,190),FRotator::ZeroRotator,FVector(.85),true,A);
 auto* Carpet=Add(TEXT("Cube"),FVector(0,-40,191),FRotator::ZeroRotator,FVector(3.7,5.2,.013),false,A);if(Carpet)Carpet->SetMaterial(0,D->World->Material(TEXT("Cloth")));
 D->CarrierTurret=Add(TEXT("CarrierTurret77"),FVector(0,-250,466),FRotator::ZeroRotator,FVector(1),true,A);
 auto* Light=NewObject<UPointLightComponent>(A);Light->SetupAttachment(A->GetRootComponent());Light->SetRelativeLocation(FVector(-75,95,350));Light->SetIntensity(32000);Light->SetAttenuationRadius(700);Light->SetLightColor(FLinearColor(1,.86,.64));Light->SetCastShadows(false);Light->RegisterComponent();Lights.Add(Light);
 auto* Label=NewObject<ULWWorldTextComponent>(A);Label->SetupAttachment(D->CarrierShutter);Label->SetRelativeLocation(FVector(0,-16,51));Label->SetRelativeRotation(FRotator(0,-90,0));Label->SetText(FText::FromString(TEXT("OFFICE OF THE PRESIDENT")));Label->SetWorldSize(13);Label->SetHorizontalAlignment(EHTA_Center);Label->RegisterComponent();
 for(int I=0;I<4;I++){auto* Panel=Add(TEXT("CarrierTornPanel77"),FVector(I%2?255:-255,-250+(I/2)*400,188),FRotator(0,90,0),FVector(1),false,A);WreckPanels.Add(Panel);WreckRest.Add(Panel->GetRelativeTransform());Panel->SetVisibility(false);}
 LastDamageTime=D->State().CarrierDamage77;D->PlatformClock=D->DefenseClock=0;UpdateCarrier(0);
}
void ULWCampaignProduction77::UpdateCarrier(float Dt){
 auto* D=Director.Get();if(!D||!D->Carrier||!D->CarrierShutter)return;auto& State=D->State();const bool Broken=State.Values.FindRef(TEXT("carrier_disabled"))!=0;
 if(!Broken){D->CarrierShutter->SetRelativeLocation(FVector(0,-450,314));return;}
 State.CarrierDamage77=FMath::Min(12.f,State.CarrierDamage77+Dt);const float T=State.CarrierDamage77;
 // Causal stages: feed arcs -> actuators release -> shutters tear outward -> roof settles.
 const float Shutter=FMath::SmoothStep(1.f,5.f,T),Wall=FMath::SmoothStep(3.f,8.f,T),Roof=FMath::SmoothStep(5.f,10.f,T);
 // The evacuation is acted in the same scene: the aide drops the case, helps
 // the president up and guides him toward the rear service passage.
 if(auto* President=D->People.FindRef(TEXT("richardson")).Get()){
  President->Tags.AddUnique(TEXT("DirectedMotion77"));
  const float Stand=FMath::SmoothStep(1.f,3.f,T),Move=FMath::SmoothStep(3.f,10.f,T);const FVector Local(155*Move,210+145*Move,FMath::Lerp(246.f,282.f,Stand));
  const FVector Previous=President->GetActorLocation();President->SetActorLocation(D->Carrier->GetActorTransform().TransformPosition(Local),false,nullptr,ETeleportType::TeleportPhysics);President->LifeAnimation->PerformancePose77=Stand<.65f?3:0;
  President->GetCharacterMovement()->Velocity=Dt>0?(President->GetActorLocation()-Previous)/Dt:FVector::ZeroVector;
  if(Move>0)President->SetActorRotation(D->Carrier->GetActorRotation()+FRotator(0,44,0));
  if(auto* Aide=D->People.FindRef(TEXT("executive_aide77")).Get();Aide&&!Aide->bDead){const FVector Old=Aide->GetActorLocation();Aide->SetActorLocation(D->Carrier->GetActorTransform().TransformPosition(Local+FVector(-58,10,282-Local.Z)),false,nullptr,ETeleportType::TeleportPhysics);Aide->SetActorRotation(President->GetActorRotation());Aide->GetCharacterMovement()->Velocity=Dt>0?(Aide->GetActorLocation()-Old)/Dt:FVector::ZeroVector;Contact(Aide,President->Parts[0]->GetComponentLocation()+President->GetActorRightVector()*14,1,FMath::SmoothStep(1.f,3.f,T));}
  if(Case){const float Drop=FMath::SmoothStep(1.f,2.3f,T);Case->SetVisibility(true);Case->SetWorldLocation(D->Carrier->GetActorTransform().TransformPosition(FVector(-55,245,FMath::Lerp(325.f,223.f,Drop))));Case->SetWorldRotation(D->Carrier->GetActorRotation()+FRotator(0,35*Drop,0));}
 }
 const float ShutterFall=FMath::SmoothStep(6.f,10.f,T);D->CarrierShutter->SetRelativeLocation(FMath::Lerp(FVector(0,-450-90*Shutter,314+270*Shutter),FVector(330,-720,30),ShutterFall));D->CarrierShutter->SetRelativeRotation(FQuat::Slerp(FRotator(Shutter*28,0,0).Quaternion(),FRotator(0,20,90).Quaternion(),ShutterFall));D->CarrierShutter->SetCollisionEnabled(T>=4?ECollisionEnabled::NoCollision:ECollisionEnabled::QueryAndPhysics);
 if(D->CarrierShell.Num()>=4){
  for(int I=0;I<2;I++){auto* M=D->CarrierShell[I].Get();M->SetRelativeRotation(FRotator(0,0,(I?1:-1)*Wall*104));M->SetCollisionEnabled(T>=4?ECollisionEnabled::NoCollision:ECollisionEnabled::QueryAndPhysics);M->SetVisibility(T<7);}
  D->CarrierShell[3]->SetRelativeLocation(FVector(0,Roof*135,450-Roof*65));D->CarrierShell[3]->SetRelativeRotation(FRotator(Roof*12,0,Roof*9));
  for(int I=4;I<D->CarrierShell.Num();I++)if(D->CarrierShell[I])D->CarrierShell[I]->SetVisibility(T<3);
 }
 if(D->CarrierTurret)D->CarrierTurret->SetRelativeRotation(FRotator(-32*Shutter,0,18*Shutter));
 for(int I=0;I<WreckPanels.Num();I++){
  const float P=FMath::SmoothStep(4.f+I*.25f,9.f+I*.25f,T);auto* M=WreckPanels[I].Get();M->SetVisibility(T>4);FTransform Rest=WreckRest[I];FVector V=Rest.GetLocation();V.X+=(I%2?1:-1)*P*210;V.Z-=P*165;M->SetRelativeLocation(V);M->SetRelativeRotation(FRotator(P*18,90+P*(I%2?18:-18),P*(I%2?-77:77)));
 }
 for(float At:{.2f,3.2f,5.4f})if(Dt>0&&LastDamageTime<At&&T>=At){
  const FVector V=D->Carrier->GetActorTransform().TransformPosition(FVector(At<1?-210:210,-250,At<1?350:420));
  if(auto* FX=GetWorld()->SpawnActor<ALWVehicleExplosion68>(V,FRotator::ZeroRotator)){FX->Initialize(D->World,At<1?.28f:.65f);D->Actors.Add(FX);}
  D->World->Sound(At<1?TEXT("CarImpact"):TEXT("FuelExplosion"),V,At<1?.5f:.65f);D->Player->Rumble54=.4f;D->Player->RumbleTime54=.7f;
 }
 LastDamageTime=T;
}
