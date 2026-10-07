#include "LWZombie.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWVehicle.h"
#include "LWNPCLife.h"
#include "LWVoice44.h"
#include "LWWeaponEffect.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
void ALWZombie::Configure57(){
 if(!World)return;const bool Fly=Kind==ELWEnemyKind::Hornet,Bear=Kind==ELWEnemyKind::Bear;
 if(LifeAnimation)LifeAnimation->SetComponentTickEnabled(false);
 for(auto* C:GetComponentsByTag(USceneComponent::StaticClass(),TEXT("CharacterHair32")))C->DestroyComponent();
 const TCHAR* Prefix=Fly?TEXT("Hornet"):Bear?TEXT("Bear"):TEXT("Rogue");const TCHAR* Suffix[]={TEXT("Torso"),TEXT("Head"),TEXT("Pelvis"),TEXT("Arm"),TEXT("Arm"),TEXT("Leg"),TEXT("Leg")};
 const FVector Animal[]={FVector(0,0,0),FVector(65,0,15),FVector(-70,0,-5),FVector(38,-38,-10),FVector(38,38,-10),FVector(-65,-33,-5),FVector(-65,33,-5)};
 const FVector Insect[]={FVector(0,0,0),FVector(32,0,0),FVector(-39,0,0),FVector(8,-23,-12),FVector(8,23,-12),FVector(-20,-20,-12),FVector(-20,20,-12)};
 const FVector Robot[]={FVector(0,0,37),FVector(0,0,88),FVector(0,0,7),FVector(0,-31,55),FVector(0,31,55),FVector(0,-14,-7),FVector(0,14,-7)};
 for(int I=0;I<7;I++){Parts[I]->SetStaticMesh(World->Mesh(FName(*(FString(Prefix)+Suffix[I]+TEXT("57")))));Parts[I]->EmptyOverrideMaterials();Parts[I]->SetRelativeLocation(Fly?Insect[I]:Bear?Animal[I]:Robot[I]);Parts[I]->SetRelativeRotation(FRotator::ZeroRotator);Parts[I]->SetRelativeScale3D(!Fly&&!Bear&&I>=5?FVector(1,1,1.35f):FVector(1));}
 GetCapsuleComponent()->SetCapsuleSize(Fly?48:Bear?80:38,Fly?50:Bear?100:94);GetCharacterMovement()->MaxWalkSpeed=Bear?460:620;Health=Bear?480:Fly?120:280;MaximumHealth=Health;
 if(Fly){GetCharacterMovement()->SetMovementMode(MOVE_Flying);GetCharacterMovement()->MaxFlySpeed=540;GetCharacterMovement()->BrakingDecelerationFlying=800;AddActorWorldOffset(FVector(0,0,160));for(int Side:{-1,1}){auto* Wing=NewObject<UStaticMeshComponent>(this);Wing->SetupAttachment(Parts[0]);Wing->SetStaticMesh(World->Mesh(TEXT("HornetWing57")));Wing->SetRelativeScale3D(FVector(1,Side,1));Wing->SetRelativeLocation(FVector(-8,Side*12,18));Wing->SetCollisionEnabled(ECollisionEnabled::NoCollision);Wing->RegisterComponent();Wings57.Add(Wing);}}
 else if(!Bear){Gun=NewObject<UStaticMeshComponent>(this);Gun->SetupAttachment(Parts[4]);Gun->SetStaticMesh(World->Mesh(TEXT("Rifle")));Gun->SetRelativeLocation(FVector(15,0,-45));Gun->SetRelativeScale3D(FVector(.75f));Gun->SetCollisionEnabled(ECollisionEnabled::NoCollision);Gun->RegisterComponent();}
 Home=GetActorLocation();CombatRandom54.Initialize(PersistentId^57057);SpecialClock57=1+CombatRandom54.FRand();
}
void ALWZombie::Tick57(float Dt,ALWCharacter* P){
 const bool Fly=Kind==ELWEnemyKind::Hornet,Bear=Kind==ELWEnemyKind::Bear;const FVector Here=GetActorLocation(),Delta=P->GetActorLocation()-Here;const float Dist=Delta.Size();const float Time=GetWorld()->GetTimeSeconds();
 AttackCooldown-=Dt;VoiceClock-=Dt;SpecialClock57-=Dt;Stagger=FMath::Max(0.f,Stagger-Dt);PathClock-=Dt;
 FCollisionQueryParams Q(NAME_None,false,this);FHitResult H;const bool See=!GetWorld()->LineTraceSingleByChannel(H,Here+FVector(0,0,Fly?0:40),P->GetActorLocation()+FVector(0,0,25),ECC_Visibility,Q)||H.GetActor()==P||H.GetActor()==P->Vehicle;
 if(See&&Dist<(Fly?3500:Bear?3200:6500)){Alert=10;Interest=P->GetActorLocation();}else Alert=FMath::Max(0.f,Alert-Dt);
 if(VoiceClock<=0&&Dist<5000){World->Sound(LWVoice44::Enemy(Kind),Here,.85f);VoiceClock=CombatRandom54.FRandRange(4,8);}
 FVector Goal=Alert>0?Interest:Home+FVector(FMath::Sin(Time*.09+PersistentId)*250,FMath::Cos(Time*.07)*250,0);
 if(Fly){
  if(Dist<4500&&!WingsAudio57){WingsAudio57=World->Sound(TEXT("HornetWings57"),Here,.3f,1,true);if(WingsAudio57)WingsAudio57->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepWorldTransform);}
  if(WingsAudio57)WingsAudio57->SetVolumeMultiplier(Dist<4500?.3f:0.f);
  for(int I=0;I<Wings57.Num();I++)if(IsValid(Wings57[I]))Wings57[I]->SetRelativeRotation(FRotator(0,0,FMath::Sin(Time*75)*(I?1:-1)*35));
  if(CreatureCharge>0){CreatureCharge-=Dt;Goal=AttackGoal57;if(Dist<145&&See&&AttackCooldown<=0){UGameplayStatics::ApplyDamage(P,17*LegendaryDamage(),nullptr,this,nullptr);World->Sound(TEXT("HornetSting57"),Here);AttackCooldown=2.8f;CreatureCharge=0;}}
  else{Goal+=FVector(FMath::Sin(Time*1.2+PersistentId)*380,FMath::Cos(Time*1.2+PersistentId)*380,180+FMath::Sin(Time*2)*70);if(See&&Alert>0&&SpecialClock57<=0){AttackGoal57=P->GetActorLocation()+FVector(0,0,25);CreatureCharge=1.2f;SpecialClock57=3.5f;}}
  FHitResult Wall;FVector Direction=(Goal-Here).GetSafeNormal();if(GetWorld()->SweepSingleByChannel(Wall,Here,Here+Direction*200,FQuat::Identity,ECC_WorldStatic,FCollisionShape::MakeSphere(45),Q))Direction=(Direction+Wall.ImpactNormal+FVector(0,0,.7)).GetSafeNormal();if(!CrossesSafehouse(Here,Here+Direction*120))AddMovementInput(Direction,1,true);
 }else{
  if(!Bear&&Alert>0&&See){
   if(SpecialClock57<=0){Strafe54=CombatRandom54.RandRange(0,1)?1:-1;SpecialClock57=CombatRandom54.FRandRange(.8f,1.8f);CreatureCharge=.32f;World->Sound(TEXT("RogueDash57"),Here,.65f);}
   const FVector Side=FVector(-Delta.Y,Delta.X,0).GetSafeNormal()*Strafe54;Goal=Here+Side*450+Delta.GetSafeNormal()*(Dist<700?-400:Dist>2000?450:30);CreatureCharge=FMath::Max(0.f,CreatureCharge-Dt);GetCharacterMovement()->MaxWalkSpeed=CreatureCharge>0?1050:420;
   if(AttackCooldown<=0&&Dist<6000&&CanUseGun()){
    const FVector Muzzle=ALWWeaponEffect::GunMuzzle(this,Gun);const FVector Aim=ALWWeaponEffect::EnemyAim(Muzzle,P->GetActorLocation(),GetVelocity().Size2D(),P->GetVelocity().Size2D());FHitResult Hit;if(GetWorld()->LineTraceSingleByChannel(Hit,Muzzle,Muzzle+Aim*6500,ECC_Visibility,Q))UGameplayStatics::ApplyPointDamage(Hit.GetActor(),7*LegendaryDamage(),Aim,Hit,nullptr,this,nullptr);
    ALWWeaponEffect::Gunfire(this,World,Muzzle,Hit.bBlockingHit?Hit.ImpactPoint:Muzzle+Aim*6500,false);World->Sound(TEXT("RogueShot57"),Muzzle);RaiderRounds--;AttackCooldown=RaiderRounds%3==0?1.1f:.16f;if(RaiderRounds<=0){RaiderRounds=12;AttackCooldown=2.1f;}
   }
  }
  if(Bear){
   if(See&&Alert>0&&Dist>350&&Dist<1500&&SpecialClock57<=0){CreatureCharge=1.5f;AttackGoal57=P->GetActorLocation();SpecialClock57=5;World->Sound(TEXT("Bear57"),Here);}
   if(CreatureCharge>0){CreatureCharge-=Dt;Goal=AttackGoal57;GetCharacterMovement()->MaxWalkSpeed=670;}else GetCharacterMovement()->MaxWalkSpeed=Alert>0?400:110;
   if(AttackWindup>0){AttackWindup-=Dt;GetCharacterMovement()->StopMovementImmediately();if(AttackWindup<=0){if(Dist<240&&See)UGameplayStatics::ApplyDamage(P,32*LegendaryDamage(),nullptr,this,nullptr);World->Sound(TEXT("BearSwipe57"),Here);AttackCooldown=1.4f;}}
   else if(Dist<215&&See&&AttackCooldown<=0){AttackWindup=.45f;CreatureCharge=0;}
  }
  if(PathClock<=0){FindRoute(Goal);PathClock=.65f;}while(Path.Num()&&FVector::Dist2D(Here,Path[0])<90)Path.RemoveAt(0);if(Path.Num())Goal=Path[0];FVector D=Goal-Here;D.Z=0;if(D.Size()>60&&Stagger<=0&&AttackWindup<=0&&!CrossesSafehouse(Here,Here+D.GetSafeNormal()*120))AddMovementInput(D.GetSafeNormal(),1,true);
 }
 FVector Facing=(!Bear&&!Fly&&See)?Delta:Goal-Here;Facing.Z=0;if(!Facing.IsNearlyZero())SetActorRotation(FMath::RInterpTo(GetActorRotation(),Facing.Rotation(),Dt,Fly?9:7));
 if(!Bear&&!Fly){FootDistance54+=GetVelocity().Size2D()*Dt;if(FootDistance54>150){FootDistance54=0;World->Sound(TEXT("RogueStep57"),Here-FVector(0,0,80),.45f);}}
 const float Gait=Time*(Bear?9:Fly?20:14),Move=FMath::Clamp(GetVelocity().Size()/400.,0.,1.);for(int I=3;I<7;I++)if(!Missing(I))Parts[I]->SetRelativeRotation(FRotator(FMath::Sin(Gait+I*PI)*Move*(Bear?28:Fly?15:36)+(Bear&&AttackWindup>0&&I<5?-40:0),0,0));
}
