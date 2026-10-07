#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h"
#include "LWNPCLife.generated.h"

// Cosmetic animation only: movement, attacks and the seven physical limbs remain authoritative.
UCLASS(ClassGroup=(Animation))
class LETHALWORLD_API ULWNPCLife : public UActorComponent {
 GENERATED_BODY()
public:
 ULWNPCLife();
 virtual void BeginPlay() override;
 virtual void EndPlay(const EEndPlayReason::Type Reason) override;
 virtual void TickComponent(float Dt,ELevelTick Type,FActorComponentTickFunction* Function) override;
 void Speak(class UAudioComponent* Audio,const FString& Text);
 UFUNCTION() void Envelope(const class USoundWave* Wave,float Value);
 UPROPERTY(EditAnywhere,Category="NPC Animation") float ExpressionStrength=1;
 UPROPERTY(EditAnywhere,Category="NPC Animation") float GestureStrength=1;
 UPROPERTY(EditAnywhere,Category="NPC Animation") float DetailDistance=1800;
 UPROPERTY(VisibleAnywhere,Category="NPC Animation") FString AnimationState;
 UPROPERTY(VisibleAnywhere,Category="NPC Animation") int32 Gesture=0;
 UPROPERTY(VisibleAnywhere,Category="NPC Animation") float MouthOpen=0,Blink=0;
 static float BlinkShape(float Time);
 static FVector Bend(FVector Vertex,float Joint,float Degrees);
 // Campaign contact targets are world-space wrist anchors. The limb solver
 // controls the same visual arm used by locomotion; no duplicate floating hands.
 FVector HandTargets77[2];float HandWeights77[2]={0,0};
 int32 PerformancePose77=0; // 1 restrained seat, 2 kneel, 3 seat, 4 supine
 FVector HandPosition77(int32 Side)const;
private:
 UPROPERTY(Transient) TObjectPtr<class ALWZombie> NPC;
 UPROPERTY(Transient) TArray<TObjectPtr<UProceduralMeshComponent>> Visuals;
 UPROPERTY(Transient) TObjectPtr<UProceduralMeshComponent> Mouth;
 TArray<TArray<FProcMeshSection>> Originals;
 TArray<TWeakObjectPtr<class UStaticMesh>> Sources;
 TWeakObjectPtr<class UAudioComponent> Speech;
 FRandomStream Random;
 bool Initialized=false,HasEnvelope=false,HadSpeechAudio=false;
 float Clock=0,Phase=0,NextGesture=0,GestureStart=0,NextBlink=0,BlinkStart=-10;
 float VoiceEnd=0,VoiceLevel=0,EnvelopeClock=-10,DetailClock=0,PreviousHealth=-1,Flinch=0;
 float Personality=0,PreviousSpeed=0,SmoothedSpeed=0,GazeClock=0;
 FRotator Pose[7],Gaze=FRotator::ZeroRotator;
 float Elbows[2]={},Knees[2]={};
 FVector RestLocations77[7];
 void CreatureAnimation51(float Dt,class ALWCharacter* P);
 bool CreatureReady51=false; FVector CreatureRest51[7]; FRotator CreaturePose51[7]; float CreaturePhase51=0,Quadruped54=0;
 void BuildVisuals();
 void RestoreVisuals();
 void Deform(float Dt,bool Talking,bool Angry);
};
