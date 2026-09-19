#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LWOpening.h"
#include "LWZombie.generated.h"
class ALWCharacter;
UENUM()
enum class ELWEnemyKind:uint8 {Zombie,Raider,Dog,Mannequin,Moose,Titan,Deathclaw,Scorpion,Karen,Behemoth,Colossus,WorldEater};
UCLASS()
class LETHALWORLD_API ALWZombie : public ACharacter
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere,Category="Animation") TObjectPtr<class ULWNPCLife> LifeAnimation;
    UPROPERTY() FLWIdentity Appearance35;
    ALWZombie();
    virtual void BeginPlay() override;
    virtual void Tick(float Dt) override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    virtual float TakeDamage(float D,const FDamageEvent& Event,AController* Instigator,AActor* Causer) override;
    UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> Parts;
    UPROPERTY(Transient) TArray<TObjectPtr<class UPhysicsConstraintComponent>> RagdollJoints;
    UPROPERTY() TObjectPtr<class ALWWorld> World;
    uint32 PersistentId=0;
    FName CorpseRecord;
    int Stars=0;float MaximumHealth=100;bool LegendaryInitialized=false;
    TWeakObjectPtr<class ALWVehicle> HeldCar49;float GrabTimer49=0,ThrowCooldown49=4;bool TickCarAttack49(float Dt,class ALWCharacter* P);
    float BossClock=0;int BurrowPhase=0;FVector BurrowOrigin,BurrowDestination;
    UPROPERTY() TObjectPtr<class ALWWorldObject> LootProxy;
    void EnsureLegendary();void ConfigureBoss();void TickBoss(float Dt,ALWCharacter* P);void MakeCorpseLoot();void LootBody(ALWCharacter* P);FString EnemyName()const;
    float LegendaryDamage()const{return 1.f+Stars*.35f;}

    FString CustomName;
    float Health=100,Stagger=0,Alert=0,AttackCooldown=0,VoiceClock=0,ThinkClock=0,PathClock=0;
    FVector Home,Interest;
    TArray<FVector> Path;
    bool bDead=false;
    TSharedPtr<struct FLWHostileRoute54> HostileRoute54;
    FRandomStream CombatRandom54;bool BrainReady54=false;
    float Suppression54=0,TacticHold54=0,CoverHold54=0,AttackRecovery54=0,Strafe54=1,FootDistance54=0;
    FVector Cover54=FVector::ZeroVector,LastFootPosition54=FVector::ZeroVector;
    int FootSide54=0,AttackStyle54=0;float FootPhase54=0;
    void ReactTactics54(float Dt,ALWCharacter* P);void CreatureFootfall54(float Phase,ALWCharacter* P);
    static void SuppressAlong54(ALWCharacter* Shooter,FVector Start,FVector End);
    float ShockTime=0,PoisonTime=0,BurnTime=0,StatusClock=0;TWeakObjectPtr<ALWCharacter> StatusSource;
    bool TickStatus(float Dt);
    uint8 SeveredMask=0;float BloodTime=-100;float LimbDamage[7]={};
    bool bCrawling=false;
    float HeadlessTime=0;FVector HeadlessDirection=FVector::ForwardVector;
    TWeakObjectPtr<AActor> InjuryCauser;TWeakObjectPtr<AController> InjuryInstigator;
    bool Missing(int Part)const{return (SeveredMask&(1<<Part))!=0;}
    float BodyMassScale()const{return Kind==ELWEnemyKind::Behemoth?135.f:Kind==ELWEnemyKind::Colossus?3645.f:Kind==ELWEnemyKind::WorldEater?1000.f:Kind==ELWEnemyKind::Titan?5.f:Kind==ELWEnemyKind::Moose?4.f:Kind==ELWEnemyKind::Deathclaw?3.f:Kind==ELWEnemyKind::Dog?.45f:1.f;}
    float InjurySpeed()const;float InjuryAttackDelay()const;bool CanUseGun()const;
    void TickInjuries(float Dt,ALWCharacter* P);
    void RagdollForce(FVector Velocity,FVector Contact,float Strength=1);
    void Die(const FDamageEvent& Event,AActor* Causer,float Damage,AController* Credit);
    void BloodHit(const FDamageEvent& Event,AActor* Causer,float Damage);
    void Dismember(const FDamageEvent& Event,AActor* Causer,float Damage);

    UPROPERTY() ELWEnemyKind Kind=ELWEnemyKind::Zombie;
    UPROPERTY() TObjectPtr<class UStaticMeshComponent> Gun;
    bool bAggressive=false;
    int32 RaiderRounds=12;
    void ConfigureKind(ELWEnemyKind InKind);
 void ConfigureCreature();void TickCreature(float Dt,ALWCharacter* P);
 void TickBrain(float Dt,ALWCharacter* P);
 float DecisionClock=0,AttackWindup=0,ReloadClock=0,ContactMemory=0; bool HasVisual=false; FVector TacticalGoal,CommittedAttack; int32 Tactic=0;
 float CreatureWindup=0,CreatureCharge=0;FVector ChargeDirection=FVector::ZeroVector;
    bool IsObserved(const class ALWCharacter* Player) const;
    void TickVariant(float Dt,class ALWCharacter* Player);
    void Hear(FVector Source,float Radius);
    void FindRoute(FVector Goal);

private:
    void StartRagdoll(const FDamageEvent& Event,AActor* Causer,float Damage);
    void ClearTarget();
    bool CrossesSafehouse(const FVector& Start,const FVector& End) const;
    FBox SafehouseBounds=FBox(ForceInit);
};
