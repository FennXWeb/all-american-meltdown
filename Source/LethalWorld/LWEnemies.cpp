#include "LWVoice44.h"
#include "LWWeaponEffect.h"
#include "LWZombie.h"
#include "LWNPCLife.h"
#include "LWAppearance.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

void ALWZombie::ConfigureKind(ELWEnemyKind K)
{
    Kind=K;if(int(K)>=9){ConfigureBoss();return;}if(!World||K==ELWEnemyKind::Zombie)return;if(uint8(K)>=4){ConfigureCreature();return;}
    const TCHAR* Prefix=K==ELWEnemyKind::Raider?TEXT("Raider"):K==ELWEnemyKind::Dog?TEXT("Dog"):TEXT("Mannequin");
    const TCHAR* Suffix[]={TEXT("Torso"),TEXT("Head"),TEXT("Pelvis"),TEXT("Arm"),TEXT("Arm"),TEXT("Leg"),TEXT("Leg")};
    for(int I=0;I<7;I++)Parts[I]->SetStaticMesh(World->Mesh(FName(*(FString(Prefix)+Suffix[I]+TEXT("32")))));
    for(auto* C:GetComponentsByTag(UStaticMeshComponent::StaticClass(),TEXT("CharacterHair32")))C->DestroyComponent();
    for(auto& C:Parts)C->EmptyOverrideMaterials();
    if(K==ELWEnemyKind::Mannequin)Parts[1]->SetRelativeLocation(FVector(0,0,91));
    if(K==ELWEnemyKind::Raider)LWAppearance::StyleNPC(this,Parts,FName(*FString::FromInt(PersistentId)),false,true);
    Health=K==ELWEnemyKind::Raider?120:K==ELWEnemyKind::Dog?65:180;
    if(K==ELWEnemyKind::Dog){
        GetCapsuleComponent()->SetCapsuleSize(26,43);
        const FVector P[]={FVector(0,0,0),FVector(40,0,6),FVector(-28,0,0),FVector(25,-13,-3),FVector(25,13,-3),FVector(-28,-12,-3),FVector(-28,12,-3)};
        for(int I=0;I<7;I++)Parts[I]->SetRelativeLocation(P[I]);
    }
    if(K==ELWEnemyKind::Raider){
        Gun=NewObject<UStaticMeshComponent>(this);Gun->SetupAttachment(RootComponent);Gun->SetStaticMesh(World->Mesh(TEXT("Rifle")));
        Gun->SetRelativeLocation(FVector(25,8,42));Gun->SetCollisionEnabled(ECollisionEnabled::NoCollision);Gun->RegisterComponent();
    }
}
bool ALWZombie::IsObserved(const ALWCharacter* P)const
{
    if(!P||!P->Camera)return false;
    const FVector Eye=P->Camera->GetComponentLocation();const FRotator View=P->Camera->GetComponentRotation();
    const float H=FMath::Tan(FMath::DegreesToRadians(P->Camera->FieldOfView*.5f));
    // Test several body points against the camera frustum and actual occluding geometry.
    int32 Width=0,Height=0;if(auto* PC=Cast<APlayerController>(P->Controller))PC->GetViewportSize(Width,Height);
    const float V=H/(Height>0?float(Width)/Height:FMath::Max(1.f,P->Camera->AspectRatio));
    FCollisionQueryParams Q(NAME_None,false,P);FHitResult Hit;
    for(float Z:{-60.f,0.f,65.f}){
        FVector At=GetActorLocation()+FVector(0,0,Z),Local=View.UnrotateVector(At-Eye);
        if(Local.X<=0||FMath::Abs(Local.Y)>Local.X*H+32||FMath::Abs(Local.Z)>Local.X*V+32)continue;
        if(!GetWorld()->LineTraceSingleByChannel(Hit,Eye,At,ECC_Visibility,Q)||Hit.GetActor()==this)return true;
    }
    return false;
}
void ALWZombie::TickVariant(float Dt,ALWCharacter* P)
{
    if(uint8(Kind)>=4){TickCreature(Dt,P);return;}
    const FVector Here=GetActorLocation(),Delta=P->GetActorLocation()-Here;const float Dist=Delta.Size2D();
    if(Dist>12500){ClearTarget();return;}
    AttackCooldown-=Dt;VoiceClock-=Dt;PathClock-=Dt;Stagger=FMath::Max(0.f,Stagger-Dt);
    FCollisionQueryParams Q(NAME_None,false,this);FHitResult Hit;
    const FVector Eye=Here+FVector(0,0,Kind==ELWEnemyKind::Dog?10:50);
    const bool See=GetWorld()->LineTraceSingleByChannel(Hit,Eye,P->GetActorLocation()+FVector(0,0,30),ECC_Visibility,Q)&&Hit.GetActor()==P;
    if(Kind==ELWEnemyKind::Mannequin){
        if(Dist<220&&See)bAggressive=true;
        if(!bAggressive&&IsObserved(P)){
            ConsumeMovementInputVector();GetCharacterMovement()->StopMovementImmediately();
            return;
        }
        Interest=P->GetActorLocation();
    }else{
        if(See&&Dist<(Kind==ELWEnemyKind::Dog?3800:6000)){Alert=14;Interest=P->GetActorLocation();}
        else Alert=FMath::Max(0.f,Alert-Dt);
        if(Alert<=0){float T=GetWorld()->GetTimeSeconds();Interest=Home+FVector(FMath::Sin(T*.06)*450,FMath::Cos(T*.08)*450,0);}
    }
    if(VoiceClock<=0){auto* VoiceAudio=World->Sound(LWVoice44::Enemy(Kind,Appearance35.Body),Here,.65f);if(LifeAnimation&&Kind==ELWEnemyKind::Raider)LifeAnimation->Speak(VoiceAudio,TEXT("..."));VoiceClock=Kind==ELWEnemyKind::Mannequin?4:7;}
    if(Stagger>0)return;
    if(Kind==ELWEnemyKind::Raider&&Alert>0&&See&&Dist<3800&&AttackCooldown<=0){
        if(RaiderRounds<=0){RaiderRounds=12;AttackCooldown=2.8f;World->Sound(TEXT("RifleMagIn"),Here);return;}
        const FVector Muzzle=ALWWeaponEffect::GunMuzzle(this,Gun);FVector Dir=ALWWeaponEffect::EnemyAim(Muzzle,P->GetActorLocation(),GetVelocity().Size2D(),P->GetVelocity().Size2D());
        if(GetWorld()->LineTraceSingleByChannel(Hit,Muzzle,Muzzle+Dir*6500,ECC_Visibility,Q))UGameplayStatics::ApplyPointDamage(Hit.GetActor(),8,Dir,Hit,nullptr,this,nullptr);
        ALWWeaponEffect::Gunfire(this,World,Muzzle,Hit.bBlockingHit?Hit.ImpactPoint:Muzzle+Dir*6500);RaiderRounds--;AttackCooldown=RaiderRounds%3==0?1.3f:.18f;
        SetActorRotation(Delta.Rotation());
    }
    if(Kind!=ELWEnemyKind::Raider&&Dist<150&&See&&(Kind!=ELWEnemyKind::Mannequin||bAggressive)&&AttackCooldown<=0){
        UGameplayStatics::ApplyDamage(P,Kind==ELWEnemyKind::Dog?16:26,nullptr,this,nullptr);AttackCooldown=Kind==ELWEnemyKind::Dog?.85f:1.3f;
    }
    FVector Target=Interest;
    if(Kind==ELWEnemyKind::Raider&&See&&Dist>600&&Dist<2200)Target=Here+FVector(-Delta.Y,Delta.X,0).GetSafeNormal()*300*FMath::Sin(GetWorld()->GetTimeSeconds()*.8);
    if(Kind==ELWEnemyKind::Raider&&See&&Dist<600)Target=Here-Delta.GetSafeNormal()*300;
    FCollisionQueryParams MoveQ(NAME_None,false,this);MoveQ.AddIgnoredActor(P);
    if(GetWorld()->SweepSingleByChannel(Hit,Here,Target,FQuat::Identity,ECC_WorldStatic,FCollisionShape::MakeCapsule(28,Kind==ELWEnemyKind::Dog?35:46),MoveQ)){
        if(PathClock<=0){FindRoute(Target);PathClock=1.5f;}
        while(Path.Num()&&FVector::Dist2D(Here,Path[0])<65)Path.RemoveAt(0);
        if(Path.Num())Target=Path[0];
    }
    FVector Dir=Target-Here;Dir.Z=0;
    GetCharacterMovement()->MaxWalkSpeed=Kind==ELWEnemyKind::Dog?(Alert>0?440:100):Kind==ELWEnemyKind::Raider?220:bAggressive?510:180;
    if(Dir.Size2D()>70&&!CrossesSafehouse(Here,Here+Dir.GetSafeNormal()*100)){
        AddMovementInput(Dir.GetSafeNormal(),1,true);SetActorRotation(FMath::RInterpTo(GetActorRotation(),Kind==ELWEnemyKind::Raider&&See?Delta.Rotation():Dir.Rotation(),Dt,8));
    }
    float G=GetWorld()->GetTimeSeconds()*(Kind==ELWEnemyKind::Dog?14:8),Move=FMath::Clamp(GetVelocity().Size2D()/200,0.,1.);
    for(int I=3;I<7;I++)Parts[I]->SetRelativeRotation(FRotator(FMath::Sin(G+(I%2)*PI)*25*Move,0,0));
    if(Kind==ELWEnemyKind::Raider){Parts[3]->SetRelativeRotation(FRotator(-60,0,0));Parts[4]->SetRelativeRotation(FRotator(-75,0,0));}
}
