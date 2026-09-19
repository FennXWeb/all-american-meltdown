#include "LWVoice44.h"
#include "LWZombie.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
void ALWZombie::ConfigureCreature(){
 for(auto* C:GetComponentsByTag(UStaticMeshComponent::StaticClass(),TEXT("CharacterHair32")))C->DestroyComponent();for(auto& C:Parts)C->EmptyOverrideMaterials();
 const TCHAR* Names[]={TEXT("Moose"),TEXT("Titan"),TEXT("Deathclaw"),TEXT("Scorpion"),TEXT("Karen")};int K=FMath::Clamp(int(Kind)-4,0,4);const TCHAR* Suffix[]={TEXT("Torso"),TEXT("Head"),TEXT("Pelvis"),TEXT("Arm"),TEXT("Arm"),TEXT("Leg"),TEXT("Leg")};
 const float Half[]={130,152,135,70,92},Radius[]={68,55,55,70,48},HP[]={320,700,480,230,200};Health=HP[K];
 double Old=GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();GetCapsuleComponent()->SetCapsuleSize(Radius[K],Half[K]);SetActorLocation(GetActorLocation()+FVector(0,0,Half[K]-Old));
 FVector Pos[]={FVector(0,0,45),FVector(0,0,80),FVector(0,0,0),FVector(0,-33,55),FVector(0,33,55),FVector(0,-16,-20),FVector(0,16,-20)};
 if(K==0){FVector A[]={FVector(0,0,20),FVector(75,0,55),FVector(-65,0,15),FVector(60,-30,5),FVector(60,30,5),FVector(-70,-28,5),FVector(-70,28,5)};for(int I=0;I<7;I++)Pos[I]=A[I];}
 if(K==3){FVector A[]={FVector(0,0,-20),FVector(70,0,-15),FVector(-60,0,-20),FVector(60,-45,-20),FVector(60,45,-20),FVector(-20,-30,-20),FVector(-20,30,-20)};for(int I=0;I<7;I++)Pos[I]=A[I];}
 if(K==4){FVector A[]={FVector(0,0,25),FVector(0,0,74),FVector(0,0,-25),FVector(0,-35,40),FVector(0,35,40),FVector(30,-18,-12),FVector(30,18,-12)};for(int I=0;I<7;I++)Pos[I]=A[I];}
 for(int I=0;I<7;I++){Parts[I]->SetStaticMesh(World->Mesh(FName(*(FString(Names[K])+Suffix[I]+TEXT("32")))));Parts[I]->SetRelativeLocation(Pos[I]*(K==1?1.5f:1.f));Parts[I]->SetRelativeScale3D(FVector(K==1?1.5f:1.f));}
 if(K==3){Parts[3]->SetRelativeRotation(FRotator(0,-30,0));Parts[4]->SetRelativeRotation(FRotator(0,30,0));Parts[5]->SetRelativeRotation(FRotator(0,180,0));}
 if(K==4){for(int I:{3,4})Parts[I]->SetRelativeRotation(FRotator(-70,0,0));for(int I:{5,6})Parts[I]->SetRelativeRotation(FRotator(60,0,0));}
 Home=GetActorLocation();Interest=Home;
}
void ALWZombie::TickCreature(float Dt,ALWCharacter* P){
 int K=FMath::Clamp(int(Kind)-4,0,4);FVector Here=GetActorLocation(),Delta=P->GetActorLocation()-Here;float Distance=Delta.Size2D();if(Distance>13000){ClearTarget();return;}
 AttackCooldown=FMath::Max(0.f,AttackCooldown-Dt);PathClock-=Dt;VoiceClock-=Dt;Stagger=FMath::Max(0.f,Stagger-Dt);
 FHitResult H;FCollisionQueryParams Q(NAME_None,false,this);FVector Eye=Here+FVector(0,0,30);bool See=!GetWorld()->LineTraceSingleByChannel(H,Eye,P->GetActorLocation(),ECC_Visibility,Q)||H.GetActor()==P;
 if(See&&Distance<(K==0?3200:5000)){Alert=12;Interest=P->GetActorLocation();}else Alert=FMath::Max(0.f,Alert-Dt);
 if(Alert<=0)Interest=Home+FVector(FMath::Sin(GetWorld()->GetTimeSeconds()*.05f)*600,FMath::Cos(GetWorld()->GetTimeSeconds()*.04f)*600,0);
 if(VoiceClock<=0&&Alert>0){World->Sound(LWVoice44::Enemy(Kind),Here,.8f);VoiceClock=7+K;}
 if(Stagger>0&&K!=1)return;
 const float Reach=K==0?240:K==1?255:K==2?220:K==3?210:175;
 if(CreatureWindup>0){CreatureWindup-=Dt;GetCharacterMovement()->StopMovementImmediately();if(CreatureWindup<=0){if(K==0||K==2||K==4){CreatureCharge=K==0?1.3f:K==2?.6f:1.1f;ChargeDirection=Delta.GetSafeNormal2D();}else if(Distance<Reach+70&&See){UGameplayStatics::ApplyDamage(P,K==1?40:22,nullptr,this,nullptr);if(K==3)P->Stamina=FMath::Max(0.f,P->Stamina-35);AttackCooldown=K==1?2.2f:1.6f;}}return;}
 if(CreatureCharge>0){CreatureCharge-=Dt;GetCharacterMovement()->MaxWalkSpeed=K==0?850:K==2?1000:600;FVector End=Here+ChargeDirection*GetCharacterMovement()->MaxWalkSpeed*Dt;FCollisionQueryParams MoveQ(NAME_None,false,this);MoveQ.AddIgnoredActor(P);if(GetWorld()->SweepSingleByChannel(H,Here,End,FQuat::Identity,ECC_WorldStatic,FCollisionShape::MakeCapsule(GetCapsuleComponent()->GetScaledCapsuleRadius(),GetCapsuleComponent()->GetScaledCapsuleHalfHeight()*.7f),MoveQ)){CreatureCharge=0;AttackCooldown=2.5f;}else if(!CrossesSafehouse(Here,End))AddMovementInput(ChargeDirection,1,true);if(Distance<Reach&&See){UGameplayStatics::ApplyDamage(P,K==0?36:K==2?38:24,nullptr,this,nullptr);CreatureCharge=0;AttackCooldown=2;}if(CreatureCharge<=0)AttackCooldown=FMath::Max(AttackCooldown,1.5f);return;}
 if(See&&Alert>0&&AttackCooldown<=0&&Distance<((K==0||K==2||K==4)?1000:Reach)){CreatureWindup=K==1?.9f:K==3?.65f:.75f;GetCharacterMovement()->StopMovementImmediately();return;}
 FVector Target=Interest;if(PathClock<=0){FindRoute(Target);PathClock=1.8f;}while(Path.Num()&&FVector::Dist2D(Here,Path[0])<100)Path.RemoveAt(0);if(Path.Num())Target=Path[0];FVector Direction=(Target-Here).GetSafeNormal2D();const float Speeds[]={300,190,370,260,220};GetCharacterMovement()->MaxWalkSpeed=Alert>0?Speeds[K]:90;
 if(Distance>Reach*.7f&&!CrossesSafehouse(Here,Here+Direction*120)){AddMovementInput(Direction,1,true);SetActorRotation(FMath::RInterpTo(GetActorRotation(),Direction.Rotation(),Dt,K==4?2:5));}
 if(K!=4){float G=GetWorld()->GetTimeSeconds()*(K==3?14:7),Move=FMath::Clamp(GetVelocity().Size2D()/200,0.,1.);for(int I=3;I<7;I++){float Angle=FMath::Sin(G+(I%2)*PI)*20*Move;Parts[I]->SetRelativeRotation(K==3?FRotator(0,(I==5?180:0)+Angle,0):FRotator(Angle,0,0));}}
}
