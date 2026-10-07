#include "LWArsenal62.h"
#include "LWWeaponEffect.h"
#include "LWResident.h"
#include "LWZombie.h"
#include "Components/AudioComponent.h"
#include "LWNPCLife.h"
#include "LWAppearance.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"

ALWZombie::ALWZombie()
{
    LifeAnimation=CreateDefaultSubobject<ULWNPCLife>(TEXT("LifeAnimation"));
    PrimaryActorTick.bCanEverTick=true;
    GetCapsuleComponent()->InitCapsuleSize(29,88);GetCapsuleComponent()->SetCollisionProfileName(TEXT("Zombie"));
    GetCharacterMovement()->MaxWalkSpeed=135;GetCharacterMovement()->bRunPhysicsWithNoController=true;
    GetCharacterMovement()->bOrientRotationToMovement=false;GetCharacterMovement()->MaxStepHeight=36;
    bUseControllerRotationYaw=false;
    const TCHAR* Names[]={TEXT("Torso"),TEXT("Head"),TEXT("Pelvis"),TEXT("LeftArm"),TEXT("RightArm"),TEXT("LeftLeg"),TEXT("RightLeg")};
    for(const TCHAR* N:Names)
    {
        auto* C=CreateDefaultSubobject<UStaticMeshComponent>(N);C->SetupAttachment(RootComponent);
        C->SetMobility(EComponentMobility::Movable);C->SetCanEverAffectNavigation(false);
        C->SetGenerateOverlapEvents(false);C->SetCollisionEnabled(ECollisionEnabled::NoCollision);Parts.Add(C);
    }
}
void ALWZombie::BeginPlay()
{
    Super::BeginPlay();World=ALWWorld::Get(this);if(!World)return;
    const TCHAR* Assets[]={TEXT("ZombieTorso32"),TEXT("ZombieHead32"),TEXT("ZombiePelvis32"),TEXT("ZombieArm32"),TEXT("ZombieArm32"),TEXT("ZombieLeg32"),TEXT("ZombieLeg32")};
    const FVector Positions[]={FVector(0,0,63),FVector(0,0,91),FVector(0,0,12),FVector(0,-18,58),FVector(0,18,58),FVector(0,-9,-1),FVector(0,9,-1)};
    for(int32 I=0;I<7;I++){Parts[I]->SetStaticMesh(World->Mesh(Assets[I]));Parts[I]->SetRelativeLocation(Positions[I]);Parts[I]->SetCollisionObjectType(ECC_Pawn);Parts[I]->SetCollisionEnabled(ECollisionEnabled::QueryOnly);Parts[I]->SetCollisionResponseToAllChannels(ECR_Ignore);Parts[I]->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);}
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Ignore);
    LWAppearance::StyleNPC(this,Parts,FName(*GetActorLocation().ToString()),false,false,true);
    VoiceClock=FMath::FRandRange(2.f,7.f);Home=GetActorLocation();Interest=Home;

    // Match ALWWorld::IsSafePosition, expanded so the entire capsule stays outside the bunker.
    const FVector Entrance=ALWWorld::BunkerDoorPosition(),Spawn=ALWWorld::BunkerSpawn();
    const FVector Center(Entrance.X,Entrance.Y,Spawn.Z);
    const float Margin=GetCapsuleComponent()->GetScaledCapsuleRadius()+25.f;
    const float HalfHeight=GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
    SafehouseBounds=FBox(Center+FVector(-1900-Margin,-1550-Margin,-210-HalfHeight),
                        Center+FVector(1900+Margin,1550+Margin,350+HalfHeight));
}
void ALWZombie::EndPlay(const EEndPlayReason::Type R)
{
    if(WingsAudio57){WingsAudio57->Stop();WingsAudio57->DestroyComponent();}
    // Release joints before component teardown destroys their simulated bodies.
    for(UPhysicsConstraintComponent* Joint:RagdollJoints)if(IsValid(Joint))Joint->TermComponentConstraint();
    if(IsValid(World))World->ZombieCount=FMath::Max(0,World->ZombieCount-1);
    Super::EndPlay(R);
}
void ALWZombie::ClearTarget()
{
    Alert=0;Path.Empty();HostileRoute54.Reset();Interest=Home;ThinkClock=0;PathClock=0;
    ConsumeMovementInputVector();GetCharacterMovement()->StopMovementImmediately();
    if(AController* AI=GetController())AI->StopMovement();
}
bool ALWZombie::CrossesSafehouse(const FVector& Start,const FVector& End) const
{
    if(!SafehouseBounds.IsValid)return false;
    return FMath::LineBoxIntersection(SafehouseBounds,Start,End,End-Start);
}
void ALWZombie::Hear(FVector P,float Radius)
{
    if(bDead||!World)return;
    // Noise currently has no instigator; all gameplay noises come from the single player.
    const ALWCharacter* Player=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
    if(Player&&Player->bSafehouse){ClearTarget();return;}
    if(CrossesSafehouse(P,P))return;
    FHitResult Hit;FCollisionQueryParams Q(NAME_None,false,this);
    const bool Wall=GetWorld()->LineTraceSingleByChannel(Hit,GetActorLocation()+FVector(0,0,30),P,ECC_Visibility,Q)&&!Cast<ALWCharacter>(Hit.GetActor());
    const float AudibleRadius=Radius*(Wall?.35f:1.f);
    if(FVector::DistSquared(GetActorLocation(),P)<FMath::Square(AudibleRadius)) {Interest=P;Alert=FMath::Max(Alert,8.f);}
}
void ALWZombie::StartRagdoll(const FDamageEvent& Event,AActor* Causer,float Damage)
{
    // Capture locomotion before disabling the character. Component physics ticks independently.
    const FVector InheritedVelocity=GetVelocity();
    ClearTarget();GetCharacterMovement()->DisableMovement();GetCharacterMovement()->SetComponentTickEnabled(false);
    DetachFromControllerPendingDestroy();
    bUseControllerRotationYaw=false;bUseControllerRotationPitch=false;bUseControllerRotationRoll=false;
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCapsuleComponent()->SetGenerateOverlapEvents(false);SetActorTickEnabled(false);

    const float Masses[]={28.f,5.f,14.f,4.f,4.f,11.f,11.f};
    for(int32 Index=0;Index<Parts.Num();++Index)
    {
        UStaticMeshComponent* Part=Parts[Index];
        if(!IsValid(Part)||!Part->GetStaticMesh()||Part->IsSimulatingPhysics())continue;
        Part->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        // Ignore other bodies/pawns so overlapping authored segments do not explode or block doors.
        Part->SetCollisionObjectType(ECC_PhysicsBody);
        Part->SetCollisionResponseToAllChannels(ECR_Ignore);
        Part->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
        Part->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);
        Part->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Block);
        Part->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        Part->SetMassOverrideInKg(NAME_None,Masses[Index]*BodyMassScale());
        Part->SetLinearDamping(.35f);Part->SetAngularDamping(2.f);
        Part->BodyInstance.SetPositionSolverIterationCount(12);
        Part->BodyInstance.SetVelocitySolverIterationCount(4);
        Part->SetMaxDepenetrationVelocity(NAME_None,150.f);
        Part->SetUseCCD(true);Part->SetEnableGravity(true);Part->SetSimulatePhysics(true);
        Part->SetPhysicsMaxAngularVelocityInDegrees(540.f);
        Part->SetPhysicsLinearVelocity(InheritedVelocity);
    }

    // Mesh origins are authored at the shoulders/hips. The head origin is above its neck.
    // Initializing both local frames from the SAME world frame preserves the current animated pose.
    struct FJoint {int32 Parent,Child;FVector ChildAnchor;float Swing1,Swing2,Twist;};
    const FJoint Joints[]={
        {0,1,FVector(0,0,-27),35,30,35},
        {0,2,FVector::ZeroVector,30,25,25},
        {0,3,FVector::ZeroVector,75,65,60},
        {0,4,FVector::ZeroVector,75,65,60},
        {2,5,FVector::ZeroVector,55,35,30},
        {2,6,FVector::ZeroVector,55,35,30}
    };
    TArray<FJoint> BodyJoints; if(Kind==ELWEnemyKind::WorldEater){for(int Segment=1;Segment<7;Segment++)BodyJoints.Add({Segment-1,Segment,FVector(35,0,0),25,25,15});}else BodyJoints.Append(Joints,UE_ARRAY_COUNT(Joints));
    for(const FJoint& Joint:BodyJoints)
    {
        if(Missing(Joint.Parent)||Missing(Joint.Child))continue;
        UStaticMeshComponent* Parent=Parts[Joint.Parent];UStaticMeshComponent* Child=Parts[Joint.Child];
        if(!IsValid(Parent)||!IsValid(Child)||!Parent->IsSimulatingPhysics()||!Child->IsSimulatingPhysics())continue;
        UPhysicsConstraintComponent* Constraint=NewObject<UPhysicsConstraintComponent>(this);
        RagdollJoints.Add(Constraint);
        // Local X is the twist axis: align it with the child's vertical limb axis.
        const FQuat JointRotation=Child->GetComponentQuat()*FRotator(90,0,0).Quaternion();
        Constraint->SetWorldLocationAndRotation(Child->GetComponentTransform().TransformPosition(Kind==ELWEnemyKind::Dog?FVector::ZeroVector:Joint.ChildAnchor),JointRotation);
        Constraint->SetLinearXLimit(LCM_Locked,0);Constraint->SetLinearYLimit(LCM_Locked,0);Constraint->SetLinearZLimit(LCM_Locked,0);
        Constraint->SetAngularSwing1Limit(ACM_Limited,Joint.Swing1);Constraint->SetAngularSwing2Limit(ACM_Limited,Joint.Swing2);
        Constraint->SetAngularTwistLimit(ACM_Limited,Joint.Twist);
        Constraint->SetLinearBreakable(false,0);Constraint->SetAngularBreakable(false,0);
        Constraint->SetDisableCollision(true);
        Constraint->SetProjectionEnabled(true);Constraint->SetProjectionParams(0,0,5.f,25.f);
        Constraint->ComponentTags.Add(FName(*FString::FromInt(Joint.Child)));
        Constraint->RegisterComponent();Constraint->SetConstrainedComponents(Parent,NAME_None,Child,NAME_None);
    }

    FVector Direction=Causer?(GetActorLocation()-Causer->GetActorLocation()).GetSafeNormal():-GetActorForwardVector();
    FVector Impact=FVector::ZeroVector;bool bHasImpact=false;
    UStaticMeshComponent* ImpactPart=Parts[0];
    if(Event.IsOfType(FPointDamageEvent::ClassID))
    {
        const FPointDamageEvent& Point=static_cast<const FPointDamageEvent&>(Event);
        Direction=FVector(Point.ShotDirection).GetSafeNormal();Impact=Point.HitInfo.ImpactPoint;bHasImpact=Point.HitInfo.bBlockingHit;
        if(bHasImpact)
        {
            // Live attacks hit the capsule. Choose the nearest simulated segment to that contact.
            double BestDistance=TNumericLimits<double>::Max();
            for(UStaticMeshComponent* Part:Parts)
            {
                if(!IsValid(Part)||!Part->IsSimulatingPhysics())continue;
                const double Distance=FVector::DistSquared(Part->GetCenterOfMass(),Impact);
                if(Distance<BestDistance){BestDistance=Distance;ImpactPart=Part;}
            }
        }
    }
    else if(Event.IsOfType(FRadialDamageEvent::ClassID))
    {
        Direction=(GetActorLocation()-static_cast<const FRadialDamageEvent&>(Event).Origin).GetSafeNormal();
    }
    if(Direction.IsNearlyZero())Direction=-GetActorForwardVector();
    const FVector Kick=Direction*FMath::Clamp(Damage*2.f,100.f,240.f)+FVector(0,0,35);
    for(UStaticMeshComponent* Part:Parts)
    {
        if(!IsValid(Part)||!Part->IsSimulatingPhysics())continue;
        Part->AddImpulse(Kick,NAME_None,true);Part->WakeAllRigidBodies();
    }
    if(IsValid(ImpactPart)&&ImpactPart->IsSimulatingPhysics())
    {
        const FVector Center=ImpactPart->GetCenterOfMass();
        // Bound the lever arm for capsule hits; generic damage still tips the torso over.
        const FVector Contact=bHasImpact?Center+(Impact-Center).GetClampedToMaxSize(35):Center+FVector(0,0,20);
        ImpactPart->AddImpulseAtLocation(Direction*ImpactPart->GetMass()*70.f,Contact);
    }
}
float ALWZombie::TakeDamage(float D,const FDamageEvent& E,AController* I,AActor* C)
{
    if(D<=0||!FMath::IsFinite(D))return 0;EnsureLegendary();if(Kind==ELWEnemyKind::WorldEater&&BurrowPhase==2)return 0;D*=1.f/(1.f+Stars*.12f);if(auto* Attacker=Cast<ALWCharacter>(I?I->GetPawn():C);Attacker&&!bDead){Attacker->CombatTarget=this;Attacker->CombatTargetTime=8;}
    if(!bDead&&E.IsOfType(FPointDamageEvent::ClassID)){const auto& Hit=static_cast<const FPointDamageEvent&>(E);for(int N=0;N<Parts.Num();N++)if(Missing(N)&&Hit.HitInfo.GetComponent()==Parts[N]){Parts[N]->AddImpulse(Hit.ShotDirection*D*4,NAME_None,true);return 0;}}

    if(!bDead){auto* Attacker=Cast<ALWCharacter>(I?I->GetPawn():C);if(Attacker){FName Source=!Attacker->DamageWeapon62.IsNone()?Attacker->DamageWeapon62:Attacker->ActiveGun()?Attacker->ActiveGun()->Definition:NAME_None;if(auto* FX=Cast<ALWWeaponEffect>(C))Source=FX->SourceWeapon62;else if(C!=Attacker)Source=NAME_None;ChallengeWeapon62=Source;ChallengeHead62=E.IsOfType(FPointDamageEvent::ClassID)&&(static_cast<const FPointDamageEvent&>(E).HitInfo.GetComponent()==Parts[1]||static_cast<const FPointDamageEvent&>(E).HitInfo.ImpactPoint.Z>Parts[1]->Bounds.Origin.Z-Parts[1]->Bounds.BoxExtent.Z*.4f);}}
    if(bDead){BloodHit(E,C,D);Dismember(E,C,D);FVector V=C?(GetActorLocation()-C->GetActorLocation()).GetSafeNormal()*D*4:FVector(100,0,0);if(E.IsOfType(FPointDamageEvent::ClassID))V=static_cast<const FPointDamageEvent&>(E).ShotDirection*D*4;if(E.IsOfType(FRadialDamageEvent::ClassID))V=(Parts[0]->Bounds.Origin-static_cast<const FRadialDamageEvent&>(E).Origin).GetSafeNormal()*D*6+FVector(0,0,D);RagdollForce(V,E.IsOfType(FPointDamageEvent::ClassID)?FVector(static_cast<const FPointDamageEvent&>(E).HitInfo.ImpactPoint):Parts[0]->Bounds.Origin);return D;}
    if(E.IsOfType(FRadialDamageEvent::ClassID))D=Super::TakeDamage(D,E,I,C);
    BloodHit(E,C,D);
    InjuryCauser=C;InjuryInstigator=I;Dismember(E,C,D);
    float VitalDamage=D;
    if(E.IsOfType(FPointDamageEvent::ClassID)){auto* HitPart=static_cast<const FPointDamageEvent&>(E).HitInfo.GetComponent();for(int N=3;N<7;N++)if(HitPart==Parts[N]){VitalDamage*=.55f;break;}}
    Suppression54=FMath::Min(1.f,Suppression54+.3f);CoverHold54=0;Health-=VitalDamage;if(HeadlessTime>0)Health=FMath::Max(1.f,Health);Stagger=FMath::Min(.22f,D*.0015f/BodyMassScale());bAggressive=true;
    const ALWCharacter* Player=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
    if(Player&&Player->bSafehouse)ClearTarget();
    else {Alert=15;if(C)Interest=C->GetActorLocation();}
    if(Health<=0)Die(E,C,D,I);
    return D;
}
void ALWZombie::Die(const FDamageEvent& E,AActor* C,float D,AController* I){
 if(bDead)return;EnsureLegendary();if(WingsAudio57)WingsAudio57->Stop();bDead=true;Health=0;HeadlessTime=0;if(Gun)Gun->SetVisibility(false);StartRagdoll(E,C,D);
 if(IsValid(World)){World->KilledZombies.Add(PersistentId);MakeCorpseLoot();}
 ALWCharacter* Credit=Cast<ALWCharacter>(C);if(!Credit&&I)Credit=Cast<ALWCharacter>(I->GetPawn());if(Credit&&!IsA(ALWResident::StaticClass()))LWArsenal62::Credit(Credit,ChallengeWeapon62,ChallengeHead62,Stars>0);if(Credit)Credit->Reward(12+Stars*15+(int(Kind)>=9&&int(Kind)<=11?60:Kind==ELWEnemyKind::Bear?20:Kind==ELWEnemyKind::RogueAI?25:0));
 if(IsValid(World))World->Sound(TEXT("FleshHit"),Parts[0]->Bounds.Origin,.7f,.7f);SetLifeSpan(180);
}
void ALWZombie::Tick(float Dt)
{
    if(bDead)return;
    EnsureLegendary();Super::Tick(Dt);if(!World)return;
    ALWCharacter* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
    if(P&&P->bStarted&&!P->bMenu&&TickStatus(Dt))return;
    if(P&&P->bStarted&&!P->bMenu&&(Missing(1)||Missing(5)||Missing(6))){TickInjuries(Dt,P);return;}
    if(!P||P->bSafehouse||!P->bStarted||P->bMenu||P->Health<=0){ClearTarget();return;}
    if(int(Kind)>=12)Tick57(Dt,P);else if(int(Kind)>=9)TickBoss(Dt,P);else TickBrain(Dt,P);
}
