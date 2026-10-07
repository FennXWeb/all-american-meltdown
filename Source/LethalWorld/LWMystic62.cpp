#include "LWMystic62.h"
#include "LWArsenal62.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Engine/StaticMesh.h"
#include "Misc/App.h"
ULWMystic62::ULWMystic62(){PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.bTickEvenWhenPaused=true;PrimaryComponentTick.TickInterval=0;}
void ULWMystic62::Initialize(ALWWorld* W,int Weapon,bool Preview){WeaponIndex=Weapon;Theme=LWArsenal62::Theme(Weapon);IsPreview=Preview;if(auto* Body=Cast<UStaticMeshComponent>(GetAttachParent());Body&&Body->GetStaticMesh())Origin=Body->GetStaticMesh()->GetBoundingBox().GetCenter();for(int I=0;I<8;I++){auto* C=NewObject<UStaticMeshComponent>(GetOwner());C->SetupAttachment(this);C->SetStaticMesh(W->Mesh(TEXT("MysticDrop62")));C->SetMaterial(0,W->Material(FName(*FString::Printf(TEXT("MysticFX62_%d"),Theme))));C->SetCollisionEnabled(ECollisionEnabled::NoCollision);C->SetCastShadow(false);C->SetLightingChannels(false,!Preview,Preview);C->RegisterComponent();Drops.Add(C);}}
void ULWMystic62::TickComponent(float Dt,ELevelTick T,FActorComponentTickFunction* F){Super::TickComponent(Dt,T,F);Clock+=FMath::Min(float(FApp::GetDeltaTime()),.1f);auto* P=Cast<ALWCharacter>(GetOwner());const bool Visible=IsPreview||(P&&P->CanAct());for(int I=0;I<Drops.Num();I++){auto* C=Drops[I].Get();C->SetVisibility(Visible);if(!Visible)continue;float Life=FMath::Frac(Clock*.38f+I*.137f);FVector V=Origin+FVector((I%4-1.5f)*7,((I%2)*2-1)*5,-4);if(Theme==0||Theme==1||Theme==4){V+=GetComponentTransform().InverseTransformVectorNoScale(FVector(0,0,-1))*(Life*Life*23);C->SetRelativeScale3D(FVector(.5f,.5f,1+Life*2)*FMath::Sin(PI*Life));}else{V=Origin+FVector(FMath::Sin(Clock*.8+I)*12,FMath::Cos(Clock+I)*10,FMath::Sin(Clock+I)*10);C->SetRelativeScale3D(FVector(.65f));C->SetRelativeRotation(FRotator(Clock*35,I*45,Clock*65));}C->SetRelativeLocation(V);}}
