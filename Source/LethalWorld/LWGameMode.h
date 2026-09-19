#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LWGameMode.generated.h"

struct FLWV2SmokeState;
UCLASS()
class LETHALWORLD_API ALWGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    ALWGameMode();
    virtual void StartPlay() override;
    virtual void Tick(float Dt) override;
    float TestClock=0;
    int32 TestPhase=0;
    bool bSmoke=false;
    UPROPERTY() TObjectPtr<class ALWZombie> TestEnemy;
    int32 TestFailures=0;
    void Check(bool Passed,const TCHAR* Label);

private:
    TSharedPtr<FLWV2SmokeState> V2;
    void BuildV3Smoke(class ALWCharacter& Player);
    void BuildUIClickSmoke(class ALWCharacter& P);
    void BuildCombat29Smoke(class ALWCharacter& P);
    void BuildVehicle30Smoke(class ALWCharacter& P);
    void BuildVehicle42Smoke(class ALWCharacter& P);
    void BuildLandmarks28Smoke(class ALWCharacter& P);
    void BuildDungeonSmoke(class ALWCharacter& P);
    void BuildCreator35Smoke(class ALWCharacter& Player);
    void BuildRecovery37Smoke(class ALWCharacter& Player);
    void BuildGameplay40Smoke(class ALWCharacter& Player);
    void BuildWorkbench39Smoke(class ALWCharacter& Player);
    void BuildRevision38Smoke(class ALWCharacter& Player);
    void BuildTradingCards36Smoke(class ALWCharacter& Player);
    void BuildNPCLife34Smoke(class ALWCharacter& Player);
    void BuildRoad33Smoke(class ALWCharacter& Player);
    void BuildCharacters32Smoke(class ALWCharacter& Player);
    void BuildStory31Smoke(class ALWCharacter& Player);
    void BuildPOI30Smoke(class ALWCharacter& Player);
    void BuildWorld26Smoke(class ALWCharacter& P);
    void BuildGear25Smoke(class ALWCharacter& P);
    void BuildArsenalSmoke(class ALWCharacter& P);
    void BuildImpactSmoke(class ALWCharacter& P);
    void BuildCasinoSmoke(class ALWCharacter& P);
    void BuildUrbanSmoke(class ALWCharacter& P);
    void BuildCompanionNavSmoke(class ALWCharacter& P);
    void BuildV18Smoke(class ALWCharacter& P);
    void BuildV17Smoke(class ALWCharacter& P);
    void BuildV16Smoke(class ALWCharacter& P);
    void BuildV15Smoke(class ALWCharacter& P);
    void BuildV14Smoke(class ALWCharacter& Player);
    void BuildV11Smoke(class ALWCharacter& Player);
    void BuildV10Smoke(class ALWCharacter& Player);
    void BuildV9Smoke(class ALWCharacter& Player);
    void BuildV7Smoke(class ALWCharacter& Player);
    void BuildV6Smoke(class ALWCharacter& Player);
    void BuildV5Smoke(class ALWCharacter& Player);
    void BuildV4Smoke(class ALWCharacter& Player);
    void BuildV2Smoke(class ALWCharacter& Player);
    void FinishV2Smoke();
    void BuildVehicle49Smoke(class ALWCharacter& Player);
    void BuildBoss48Smoke(class ALWCharacter& Player);
    void BuildConsole47Smoke(class ALWCharacter& Player);
    void BuildUI46Smoke(class ALWCharacter& Player);
    void BuildBunker45Smoke(class ALWCharacter& Player);
    void BuildSave43Smoke(class ALWCharacter& Player);
    void BuildNPCAudio44Smoke(class ALWCharacter& Player);
    void CaptureV2(const TCHAR* Name);
    bool RequireV2(bool Passed,const TCHAR* Label);
    bool SpawnV2Target(class ALWCharacter& Player);
    void AuditV2(const class ALWCharacter& Player,const TCHAR* Context);
};
