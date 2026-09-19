#include "LWCharacter.h"
#include "LWAppearance.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
namespace {
struct FRecoil50 {TWeakObjectPtr<AActor> Body;TArray<TWeakObjectPtr<UBoxComponent>> Bones;float Time=0;FVector Start;};
TMap<TWeakObjectPtr<ALWCharacter>,FRecoil50> RecoilStates50;
}
void ALWCharacter::StartRecoil50(FVector Impulse){
 if(Recoiling50||Vehicle||Health<=0||!GetWorld())return;
 for(auto It=RecoilStates50.CreateIterator();It;++It)if(!It.Key().IsValid())It.RemoveCurrent();
 auto& R=RecoilStates50.FindOrAdd(this);R=FRecoil50();R.Start=GetActorLocation();
 auto* Body=GetWorld()->SpawnActor<AActor>();if(!Body)return;R.Body=Body;Body->SetLifeSpan(15);
 auto* Root=NewObject<USceneComponent>(Body);Body->SetRootComponent(Root);Root->RegisterComponent();Body->SetActorLocationAndRotation(GetActorLocation()-FVector(0,0,88),FRotator(0,GetControlRotation().Yaw,0));
 const FVector Positions[]={FVector(0,0,132),FVector(0,0,94),FVector(0,0,174),FVector(0,-22,125),FVector(0,22,125),FVector(0,-9,48),FVector(0,9,48)};
 const FVector Extents[]={FVector(13,19,23),FVector(13,16,13),FVector(10,10,12),FVector(7,7,25),FVector(7,7,25),FVector(8,8,35),FVector(8,8,35)};
 for(int I=0;I<7;I++){auto* B=NewObject<UBoxComponent>(Body);Body->AddInstanceComponent(B);B->SetupAttachment(Root);B->SetBoxExtent(Extents[I]);B->SetRelativeLocation(Positions[I]);B->SetCollisionObjectType(ECC_PhysicsBody);B->SetCollisionResponseToAllChannels(ECR_Ignore);B->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);B->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Block);B->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);B->SetCanEverAffectNavigation(false);B->RegisterComponent();R.Bones.Add(B);}
 TArray<TObjectPtr<UStaticMeshComponent>> Visual;LWAppearance::Build(Body,Root,Identity,Visual,false);
 for(auto& V:Visual){int Closest=0;float Best=MAX_flt;for(int I=0;I<7;++I){float D=FVector::DistSquared(V->Bounds.Origin,R.Bones[I]->GetComponentLocation());if(D<Best){Best=D;Closest=I;}}V->AttachToComponent(R.Bones[Closest].Get(),FAttachmentTransformRules::KeepWorldTransform);V->SetCollisionEnabled(ECollisionEnabled::NoCollision);}
 for(int I=1;I<7;++I){auto* J=NewObject<UPhysicsConstraintComponent>(Body);Body->AddInstanceComponent(J);J->RegisterComponent();int Parent=I>=5?1:0;J->SetWorldLocation((R.Bones[I]->GetComponentLocation()+R.Bones[Parent]->GetComponentLocation())*.5);J->SetDisableCollision(true);J->SetLinearXLimit(LCM_Locked,0);J->SetLinearYLimit(LCM_Locked,0);J->SetLinearZLimit(LCM_Locked,0);J->SetAngularSwing1Limit(ACM_Limited,I==2?35:65);J->SetAngularSwing2Limit(ACM_Limited,45);J->SetAngularTwistLimit(ACM_Limited,30);J->SetConstrainedComponents(R.Bones[Parent].Get(),NAME_None,R.Bones[I].Get(),NAME_None);}
 for(auto& B:R.Bones){B->SetSimulatePhysics(true);B->SetUseCCD(true);B->SetMassOverrideInKg(NAME_None,12);B->SetLinearDamping(.25);B->SetAngularDamping(1.5);B->SetPhysicsLinearVelocity(Impulse);B->SetPhysicsAngularVelocityInDegrees(FVector(0,220,30));}
 Recoiling50=true;StopAttack();CancelReload();bAim=bSprint=false;GetCharacterMovement()->StopMovementImmediately();GetCharacterMovement()->DisableMovement();GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);WeaponRoot->SetVisibility(false,true);if(SurvivorRoot)SurvivorRoot->SetVisibility(false,true);
 Camera->bUsePawnControlRotation=false;
}
bool ALWCharacter::TickRecoil50(float Dt){
 if(!Recoiling50)return false;auto* R=RecoilStates50.Find(this);if(!R)return false;
 R->Time+=FMath::Max(0.f,Dt);auto* Hip=R->Bones.IsValidIndex(1)?R->Bones[1].Get():nullptr;auto* Head=R->Bones.IsValidIndex(2)?R->Bones[2].Get():nullptr;
 if(Hip&&Head){SetActorLocation(Hip->GetComponentLocation()+FVector(0,0,20),false,nullptr,ETeleportType::TeleportPhysics);Camera->SetWorldLocationAndRotation(Head->GetComponentLocation()+Head->GetForwardVector()*15,Head->GetComponentRotation());}
 if(Health>0&&R->Body.IsValid()&&Hip&&R->Time<8&&(R->Time<2.5f||Hip->GetPhysicsLinearVelocity().Size()>100))return true;
 FVector Stand=R->Start;FCollisionQueryParams Q(NAME_None,false,this);if(R->Body.IsValid())Q.AddIgnoredActor(R->Body.Get());
 const FVector Here=Hip?Hip->GetComponentLocation():R->Start;
 for(int I=0;I<13;++I){FVector Candidate=Here+FVector(FMath::Cos(I*2.4)*I*35,FMath::Sin(I*2.4)*I*35,0);FHitResult Hit;if(GetWorld()->LineTraceSingleByChannel(Hit,Candidate+FVector(0,0,180),Candidate-FVector(0,0,450),ECC_Visibility,Q)&&Hit.ImpactNormal.Z>.65f){Candidate=Hit.ImpactPoint+FVector(0,0,90);if(!GetWorld()->OverlapBlockingTestByChannel(Candidate,FQuat::Identity,ECC_Pawn,FCollisionShape::MakeCapsule(32,88),Q)){Stand=Candidate;break;}}}
 if(R->Body.IsValid())R->Body->Destroy();RecoilStates50.Remove(this);Recoiling50=false;SetActorLocation(Stand,false,nullptr,ETeleportType::TeleportPhysics);Camera->SetRelativeLocationAndRotation(FVector(0,0,66),FRotator::ZeroRotator);Camera->bUsePawnControlRotation=true;GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);GetCharacterMovement()->SetMovementMode(MOVE_Walking);BuildSurvivorBody();AttackTimer=0;ConfigureWeaponParts();return false;
}
