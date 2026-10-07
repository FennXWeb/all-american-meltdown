#pragma once
#include "CoreMinimal.h"
#include "LWCampaign76State.h"
#include "LWResident.h"
#include "LWWorldObject.h"
#include "LWCampaign76.generated.h"
namespace LWGen {struct FSite;}
class ALWChunk;class ALWWorld;

struct FLWC76Choice {FString Text;FName Next,Stage;TArray<FString> Needs,Effects;};
struct FLWC76Beat {FName Who;FString Text;TArray<FString> Needs;FName Gesture,Cue;FVector Move=FVector::ZeroVector;float Pause=0;};
struct FLWC76Scene {FName Id;TArray<FLWC76Beat> Beats;TArray<FLWC76Choice> Choices;FName Next;};
struct FLWC76Action {FName Id,Scene,Next;FString Label;FName Mesh;FVector At;TArray<FString> Needs,Effects;float Work=0;};
struct FLWC76Stage {FName Id,Mission,Site;FString Title,Goal;TArray<FLWC76Action> Actions;TArray<FName> Cast;FName Arrival,Next;TArray<FString> Needs;int32 Enemies=0;float Duration=0;};
struct FLWC76Person {FName Id;FString Name;bool Female=false;FName Voice;};
namespace LWCampaign76 {
 const TArray<FLWC76Stage>& Stages();const TArray<FLWC76Scene>& Scenes();const TArray<FLWC76Person>& Cast();
 const FLWC76Stage* Stage(FName Id);const FLWC76Scene* Scene(FName Id);const FLWC76Person* Person(FName Id);
 bool Meets(const FLWCampaign76State& S,const TArray<FString>& Conditions);
 void Apply(FLWCampaign76State& S,const TArray<FString>& Effects);
 bool Alive(const FLWCampaign76State& S,FName Who);
 FString Ending(const FLWCampaign76State& S);
 bool Reserved(FVector2D P);FVector2D Site(FName Id);uint32 SiteId(FName Id);
 FTransform Frame(FName Id);int32 MarketZone();
 bool Dress(class ALWChunk* C,class ALWWorld* W,const LWGen::FSite& S);
}

UCLASS()
class ALWCampaignNode76:public ALWWorldObject {
 GENERATED_BODY()
public:
 UPROPERTY() TObjectPtr<class ALWCampaign76> Campaign;
 FName Action;FString Label;bool Usable=true;
 virtual void Use(class ALWCharacter* P)override;
 virtual FString Prompt()const override;
 virtual float TakeDamage(float D,const FDamageEvent& E,AController* I,AActor* C)override;
};
UCLASS()
class ALWCampaignPerson76:public ALWResident {
 GENERATED_BODY()
public:
 UPROPERTY() TObjectPtr<class ALWCampaign76> Campaign;
 FName Person;FVector Mark=FVector::ZeroVector;float MoveTime=0;
 virtual void Tick(float Dt)override;
 virtual float TakeDamage(float D,const FDamageEvent& E,AController* I,AActor* C)override;
};
UCLASS()
class ALWCampaignEnemy76:public ALWZombie {
 GENERATED_BODY()
public:
 UPROPERTY() TObjectPtr<class ALWCampaign76> Campaign;
 int32 Index=0;float Sabotage=0;
 virtual void Tick(float Dt)override;
 virtual float TakeDamage(float D,const FDamageEvent& E,AController* I,AActor* C)override;
};

// A narrative adapter: reuses resident navigation/combat, props, dialogue UI,
// audio/facial animation, quest checkpoints, inventory and the existing map.
UCLASS()
class ALWCampaign76:public AActor {
 GENERATED_BODY()
public:
 ALWCampaign76();
 UPROPERTY() TObjectPtr<class ALWCharacter> Player;
 UPROPERTY() TObjectPtr<class ALWWorld> World;
 UPROPERTY() TArray<TObjectPtr<AActor>> Actors;
 UPROPERTY() TMap<FName,TObjectPtr<ALWCampaignPerson76>> People;
 UPROPERTY() TArray<TObjectPtr<ALWCampaignEnemy76>> Enemies;
 UPROPERTY() TObjectPtr<class UPointLightComponent> Heater;
 UPROPERTY() TObjectPtr<class UAudioComponent> SceneSound;
 UPROPERTY() TObjectPtr<class UAudioComponent> RoomSound;
 UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> Dressing;
 UPROPERTY() TArray<TObjectPtr<class UPointLightComponent>> SceneLights;
 UPROPERTY() TObjectPtr<class ULWCampaignProduction77> Production;
 UPROPERTY() TObjectPtr<class ALWWorldObject> Carrier;
 UPROPERTY() TObjectPtr<class UStaticMeshComponent> CarrierShutter;
 UPROPERTY() TObjectPtr<class UStaticMeshComponent> CarrierTurret;
 UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> CarrierShell;
 FVector CarrierOrigin=FVector::ZeroVector;float DefenseClock=0,PlatformClock=0;
 int PreviousWeather=-1;bool HasWeather=false;
 static ALWCampaign76* Ensure(class ALWCharacter* P);
 FLWCampaign76State& State()const;
 void Begin(bool Fresh=false);void SetStage(FName Id);void Spawn();void Clear();
 void Show(FName Scene,FName Afterwards=NAME_None);void ShowBeat();void FinishScene();
 bool Choose(int I);bool Talk(ALWResident* N);bool Use(FName Action);
 void Effects(const TArray<FString>& Effects,FName Once=NAME_None);
 bool CanApply(const TArray<FString>& Effects);
 void Sync();void Refresh();void StageDressing();void AdvanceBeat();bool Click(FName Id);
 void PrepareStage();bool Travel(FName Destination);bool CastAvailable(FName Id)const;bool Hostile(FName Id)const;
 void TickStage(float Dt);void RestoreWeather();void BuildCarrier();void AfterEffect();
 void Route();void Pause();void Resume();
 void PersonDied(FName Id);FVector At(FVector Local)const;FVector Goal()const;
 bool Fighting()const;void Journal(class ALWHUD& HUD);bool Screen(class ALWHUD& HUD);
 virtual void Tick(float Dt)override;virtual void EndPlay(const EEndPlayReason::Type R)override;
 FName SpawnedStage,WorkAction;float WorkSeconds=0,BeatWait=0,SceneClock=0;int32 Page=0;
 TArray<int32> ShownChoices;FString Speech,SpeakerName;bool SceneOpen=false;
private:
 bool Applying=false,FreshStart=false;
 void CompleteAction(const FLWC76Action& Action);
};
