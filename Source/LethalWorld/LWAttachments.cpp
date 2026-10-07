#include "LWCharacter.h"
#include "LWWeaponMods.h"
#include "LWWorld.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Engine/World.h"
void ALWCharacter::ConfigureAttachments(){
 for(auto& C:AttachmentParts)if(C)C->DestroyComponent();AttachmentParts.Empty();if(GunLight){GunLight->DestroyComponent();GunLight=nullptr;}if(LaserDot){LaserDot->DestroyComponent();LaserDot=nullptr;}
 if(LaserBeam){LaserBeam->DestroyComponent();LaserBeam=nullptr;}const auto* G=ActiveGun();if(!G||!World)return;
 TArray<TPair<FName,FTransform>> Mounted;for(const auto& A:G->Attachments)if(LWMods::Compatible(G->Definition,A.Value)&&LWMods::Mount(A.Value)==A.Key)Mounted.Emplace(A.Value,FTransform(LWMods::Position(Weapon,A.Key)));for(const auto& P:G->Parts39)if(LWItems::Def(P.Definition).Category==TEXT("Attachment"))Mounted.Emplace(P.Definition,P.Transform);
 for(const auto& Mount:Mounted){const TPair<FName,FName> A(LWMods::Mount(Mount.Key),Mount.Key);
 auto* C=NewObject<UStaticMeshComponent>(this);C->SetupAttachment(WeaponMesh);C->SetStaticMesh(World->Mesh(FName(*(A.Value.ToString()+(LWArsenal62::Find(A.Value)?TEXT(""):TEXT("V14"))))));C->SetRelativeTransform(Mount.Value);C->SetCollisionEnabled(ECollisionEnabled::NoCollision);C->SetCastShadow(false);C->SetLightingChannels(false,true,false);C->ComponentTags.Add(A.Value);C->RegisterComponent();AttachmentParts.Add(C);
 if(A.Value==TEXT("att_light")&&!GunLight){GunLight=NewObject<USpotLightComponent>(this);GunLight->SetupAttachment(C);GunLight->SetRelativeLocation(FVector(8,0,0));GunLight->SetIntensity(350000);GunLight->SetAttenuationRadius(17000);GunLight->SetInnerConeAngle(10);GunLight->SetOuterConeAngle(28);GunLight->RegisterComponent();}
 if(A.Value==TEXT("att_laser")&&!LaserDot){LaserDot=NewObject<UStaticMeshComponent>(this);LaserDot->SetupAttachment(GetRootComponent());LaserDot->SetStaticMesh(World->Mesh(TEXT("Cube")));LaserDot->SetMaterial(0,World->Material(TEXT("LaserV17")));LaserDot->SetWorldScale3D(FVector(.025));LaserDot->SetCollisionEnabled(ECollisionEnabled::NoCollision);LaserDot->SetCastShadow(false);LaserDot->RegisterComponent();LaserBeam=NewObject<UStaticMeshComponent>(this);LaserBeam->SetupAttachment(GetRootComponent());LaserBeam->SetStaticMesh(World->Mesh(TEXT("Cube")));LaserBeam->SetMaterial(0,World->Material(TEXT("LaserV17")));LaserBeam->SetCollisionEnabled(ECollisionEnabled::NoCollision);LaserBeam->SetCastShadow(false);LaserBeam->RegisterComponent();}
 }TickAttachments();
}
void ALWCharacter::TickAttachments(){const bool Show=ActiveGun()&&Weapon>=2&&CanAct()&&!bReloading;
 if(GunLight)GunLight->SetVisibility(Show&&Flashlight&&Flashlight->IsVisible());
 if(LaserDot){FHitResult H;FCollisionQueryParams Q(NAME_None,false,this);FVector A=WeaponMesh->GetComponentTransform().TransformPosition(LWMods::Position(Weapon,TEXT("Laser")));FVector Direction=Camera->GetForwardVector();if(const auto* G=ActiveGun();G&&!G->Parts39.IsEmpty())for(const auto& P:G->Parts39)if(P.Definition==TEXT("att_laser")){A=WeaponMesh->GetComponentTransform().TransformPosition(P.Transform.TransformPosition(FVector(6,0,0)));Direction=WeaponMesh->GetComponentTransform().TransformVectorNoScale(P.Transform.GetRotation().GetForwardVector());break;}bool Hit=Show&&GetWorld()->LineTraceSingleByChannel(H,A,A+Direction*20000,ECC_Visibility,Q);if(LaserBeam){const FVector End=Hit?H.ImpactPoint:A+Direction*20000;LaserBeam->SetVisibility(Show);LaserBeam->SetWorldLocation((A+End)*.5);LaserBeam->SetWorldRotation((End-A).Rotation());LaserBeam->SetWorldScale3D(FVector(FVector::Distance(A,End)/100,.003,.003));}LaserDot->SetVisibility(Hit);if(Hit){LaserDot->SetWorldLocation(H.ImpactPoint+H.ImpactNormal*.6f);LaserDot->SetWorldRotation(H.ImpactNormal.Rotation());}}
}
