#include "Camera/CameraActor.h"
#include "Engine/StaticMeshActor.h"
#include "InputKeyEventArgs.h"
#include "LWBorderGuard51.h"
#include "LWBorder51.h"
#include "LWPlayerInput51.h"
#include "LWNPCLife.h"
#if WITH_EDITOR
#include "ShaderCompiler.h"
#endif
#include "LWLighting.h"
#include "LWDungeon.h"
#include "LWLandmark.h"
#include "LWUnderground.h"
#include "HAL/IConsoleManager.h"
#include "LWWeaponEffect.h"
#include "LWGameMode.h"
#include "Engine/DamageEvents.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "LWCharacter.h"
#include "LWHUD.h"
#include "LWWorld.h"
#include "LWWorldObject.h"
#include "LWVehicle.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/PlayerInput.h"
#include "LWAudioCatalog.h"
#include "LWZombie.h"
#include "LWResident.h"
#include "LWInteractable.h"
#include "LWSaveGame.h"
#include "AudioDevice.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameUserSettings.h"
#include "GameFramework/PlayerController.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformMisc.h"
#include "HAL/PlatformTime.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundBase.h"
#include "UnrealClient.h"
#include "Misc/Crc.h"

namespace
{
    constexpr uint32 LWV2EnemyId=0xf0021234;
    const TCHAR* LWV2SaveSlot=TEXT("LethalWorld_AutomationV2");
    using FLWV2Action=TFunction<void(ALWCharacter&)>;
    using FLWV2Ready=TFunction<bool(ALWCharacter&)>;
    struct FLWV2Step
    {
        FString Name;
        double MinimumSeconds=0,TimeoutSeconds=25;
        FLWV2Action Begin,End;
        FLWV2Ready Ready;
    };
    // Failed transactions must preserve every saved field, not just counts.
    TArray<uint8> LWV2Snapshot(const TArray<FLWItemInstance>& Items,bool PayloadOnly=false)
    {
        TArray<FLWItemInstance> Copy=Items;
        if(PayloadOnly)
        {
            for(auto& Item:Copy)
            {
                Item.X=Item.Y=-1;Item.bRotated=false;
                if(Item.Slot!=TEXT("Loaded"))Item.Slot=NAME_None;
            }
            Copy.Sort([](const auto& A,const auto& B){return A.Id.ToString()<B.Id.ToString();});
        }
        TArray<uint8> Bytes;FMemoryWriter Writer(Bytes,true);
        FObjectAndNameAsStringProxyArchive Archive(Writer,false);Archive.ArIsSaveGame=true;
        int32 Count=Copy.Num();Archive<<Count;
        for(auto& Item:Copy)FLWItemInstance::StaticStruct()->SerializeItem(Archive,&Item,nullptr);
        return Bytes;
    }
    FGuid LWV2FindId(const TArray<FLWItemInstance>& Items,FName Definition,int32 Rounds=INDEX_NONE)
    {
        for(const auto& Item:Items)if(Item.Definition==Definition&&(Rounds==INDEX_NONE||Item.Rounds==Rounds))return Item.Id;
        return FGuid();
    }
    int32 LWV2CylinderCount(const FLWItemInstance& Gun,int32 State)
    {int32 Count=0;for(int32 Value:Gun.Cylinder)if(Value==State)++Count;return Count;}
    FName LWV2AmmoName(FName Name){return Name==TEXT("ammo_762belt")?FName(TEXT("ammo_762")):Name;}
    TMap<FName,int64> LWV2Ammo(const TArray<FLWItemInstance>& Items)
    {
        TMap<FName,int64> Result;
        for(const auto& Item:Items)
        {
            const auto& Definition=LWItems::Def(Item.Definition);
            if(Definition.Category==TEXT("Ammo"))Result.FindOrAdd(LWV2AmmoName(Item.Definition))+=Item.Count;
            else if(!Definition.AmmoType.IsNone())
                Result.FindOrAdd(LWV2AmmoName(Definition.AmmoType))+=Item.Rounds+(Item.Chamber==1?1:0)+LWV2CylinderCount(Item,1);
        }
        return Result;
    }
    TMap<FName,int64> LWV2Ammo(const ALWCharacter& Player)
    {
        auto Result=LWV2Ammo(Player.Inventory);
        for(const auto& Pair:LWV2Ammo(Player.Stash))Result.FindOrAdd(Pair.Key)+=Pair.Value;
        return Result;
    }
    bool LWV2SameAmmo(const TMap<FName,int64>& A,const TMap<FName,int64>& B)
    {
        for(const auto& Pair:A)if(B.FindRef(Pair.Key)!=Pair.Value)return false;
        for(const auto& Pair:B)if(A.FindRef(Pair.Key)!=Pair.Value)return false;
        return true;
    }
    bool LWV2ValidItems(const TArray<FLWItemInstance>& Items,int32 Height,TSet<FGuid>& Ids)
    {
        TSet<FName> Slots;
        for(const auto& Item:Items)
        {
            const auto& D=LWItems::Def(Item.Definition);
            if(!Item.Id.IsValid()||Ids.Contains(Item.Id)||D.Id.IsNone()||Item.Count<1||Item.Count>D.MaxStack)return false;
            Ids.Add(Item.Id);
            if(Item.Rounds<0||Item.Rounds>D.Capacity||Item.Chamber<0||Item.Chamber>2)return false;
            for(int32 Chamber:Item.Cylinder)if(Chamber<0||Chamber>2)return false;
            if(Item.Definition==TEXT("revolver")&&Item.Cylinder.Num()!=D.Capacity)return false;
            if(Item.Slot.IsNone())
            {if(!LWItems::Fits(Items,Item,Item.X,Item.Y,Item.bRotated,12,Height,Item.Id))return false;}
            else if(Item.Slot!=TEXT("Loaded"))
            {
                if(Slots.Contains(Item.Slot)||!LWItems::CanEquip(Item,Item.Slot))return false;
                Slots.Add(Item.Slot);
            }
            if(Item.LoadedMagazine.IsValid())
            {
                const auto* Mag=Items.FindByPredicate([&](const auto& M){return M.Id==Item.LoadedMagazine;});
                if(!Mag||Mag->Definition!=D.MagazineType||Mag->Slot!=TEXT("Loaded")||Item.Rounds!=0)return false;
            }
            if(Item.Slot==TEXT("Loaded"))
            {
                int32 Owners=0;for(const auto& Gun:Items)if(Gun.LoadedMagazine==Item.Id)++Owners;
                if(D.Category!=TEXT("Magazine")||Owners!=1)return false;
            }
        }
        return true;
    }
    // Find free cells, but execute every transfer through the character's real drag API.
    bool LWV2MoveAuto(ALWCharacter& Player,int32 From,int32 To,FGuid Id)
    {
        const auto* Source=Player.ItemsFor(From);const auto* Target=Player.ItemsFor(To);
        if(!Source||!Target)return false;
        const auto* Found=Source->FindByPredicate([&](const auto& Item){return Item.Id==Id;});if(!Found)return false;
        const FLWItemInstance Item=*Found;
        int32 Height=To==0?10:To==1?14:12;
        if(To==2&&IsValid(Player.OpenObject))
            if(const auto* Record=Player.World->Containers.Find(Player.OpenObject->RecordId))Height=Record->Height;
        for(bool Rotate:{false,true})for(int32 Y=0;Y<Height;++Y)for(int32 X=0;X<12;++X)
            if(LWItems::Fits(*Target,Item,X,Y,Rotate,12,Height,From==To?Id:FGuid()))
                return Player.MoveItem(From,To,Id,X,Y,Rotate);
        return false;
    }
    ALWWorldObject* LWV2Object(ALWCharacter& Player,ELWObjectKind Kind)
    {
        ALWWorldObject* Best=nullptr;double Distance=TNumericLimits<double>::Max();
        for(TActorIterator<ALWWorldObject> It(Player.GetWorld());It;++It)
            if(It->World==Player.World&&It->Kind==Kind)
            {
                const double Candidate=FVector::DistSquared(Player.GetActorLocation(),It->GetActorLocation());
                if(Candidate<Distance){Distance=Candidate;Best=*It;}
            }
        return Best;
    }
    void LWV2Teleport(ALWCharacter& Player,FVector Location)
    {
        Player.GetCharacterMovement()->StopMovementImmediately();
        Player.SetActorLocation(Location,false,nullptr,ETeleportType::TeleportPhysics);
    }
    int32 LWV2DroppedCount(const ALWWorld& World)
    {int32 Count=0;for(const auto& Pair:World.Containers)if(Pair.Value.bDropped)++Count;return Count;}
}
struct FLWV2SmokeState
{
    TArray<FLWV2Step> Steps;
    int32 Index=0,Checks=0;
    double StartedAt=FPlatformTime::Seconds(),StepStartedAt=0;
    bool bEntered=false,bDone=false,bSettingsSaved=false,bAudioSmoke=false;
    float OriginalSensitivity=0,OriginalVolume=0;
    bool bOriginalCrust=false;
    FIntPoint OriginalPendingResolution=FIntPoint::ZeroValue;
    int32 OriginalPendingWindow=0;
    bool bVideoTestActive=false;
    TArray<FString> Screenshots;
    FLWV2Action Observe;
};
#include "LWV4Smoke.inl"
#include "LWV5Smoke.inl"
#include "LWV6Smoke.inl"
#include "LWV7Smoke.inl"
#include "LWV9Smoke.inl"
#include "LWV10Smoke.inl"
#include "LWWeaponMods.h"
#include "LWSpawnTable.h"
#include "LWV14Smoke.inl"
#include "LWV15Smoke.inl"
#include "LWV16Smoke.inl"
#include "LWUIClickSmoke.inl"
#include "LWCompanionNavSmoke.inl"
#include "LWSlotMachine.h"
#include "LWCasinoSmoke.inl"
#include "ProceduralMeshComponent.h"
#include "LWArsenalSmoke.inl"
#include "LWGear25Smoke.inl"
#include "LWWorld26Smoke.inl"
#include "LWDungeonSmoke.inl"
#include "LWPOI30Smoke.inl"
#include "LWStory31Smoke.inl"
#include "LWCharacters32Smoke.inl"
#include "LWRoad33Smoke.inl"
#include "LWNPCLifeSmoke.inl"
#include "LWLandmarks28Smoke.inl"
#include "LWCombat29Smoke.inl"
#include "LWVehicle30Smoke.inl"
#include "LWImpactSmoke.inl"
#include "LWUrbanSmoke.inl"
#include "LWV18Smoke.inl"
#include "LWV17Smoke.inl"
#include "LWV11Smoke.inl"
ALWGameMode::ALWGameMode()
{
    DefaultPawnClass=ALWCharacter::StaticClass();HUDClass=ALWHUD::StaticClass();
    PrimaryActorTick.bCanEverTick=true;PrimaryActorTick.bTickEvenWhenPaused=true;
}
void ALWGameMode::StartPlay()
{
    Super::StartPlay();
    // v1 remains an alias, using inventory-aware equivalents of its original assertions.
    bSmoke=FParse::Param(FCommandLine::Get(),TEXT("LWV17Smoke"))||FParse::Param(FCommandLine::Get(),TEXT("LWV16Smoke"))||FParse::Param(FCommandLine::Get(),TEXT("LWV15Smoke"))||FParse::Param(FCommandLine::Get(),TEXT("LWV14Smoke"))||FParse::Param(FCommandLine::Get(),TEXT("LWV11Smoke"))||FParse::Param(FCommandLine::Get(),TEXT("LWV10Smoke"))||FParse::Param(FCommandLine::Get(),TEXT("LWV9Smoke"))||FParse::Param(FCommandLine::Get(),TEXT("LWV7Smoke"))||FParse::Param(FCommandLine::Get(),TEXT("LWV6Smoke"))||FParse::Param(FCommandLine::Get(),TEXT("LWV5Smoke"))||FParse::Param(FCommandLine::Get(),TEXT("LWV4Smoke"))||FParse::Param(FCommandLine::Get(),TEXT("LWV3Smoke"))||FParse::Param(FCommandLine::Get(),TEXT("LWV2Smoke"))||FParse::Param(FCommandLine::Get(),TEXT("LWSmoke"));
    if(bSmoke)
    {
        V2=MakeShared<FLWV2SmokeState>();V2->bAudioSmoke=FParse::Param(FCommandLine::Get(),TEXT("LWAudioSmoke"));
        UE_LOG(LogTemp,Display,TEXT("LW_V2_BEGIN save=%s audio=%d"),LWV2SaveSlot,V2->bAudioSmoke?1:0);
    }
    if(APlayerController* PC=UGameplayStatics::GetPlayerController(this,0))
    {PC->bEnableClickEvents=true;PC->bEnableMouseOverEvents=true;}
}
void ALWGameMode::Check(bool Passed,const TCHAR* Label)
{
    if(V2.IsValid())++V2->Checks;
    if(Passed){UE_LOG(LogTemp,Display,TEXT("LW_V2_CHECK PASS %s"),Label);}
    else{++TestFailures;UE_LOG(LogTemp,Error,TEXT("LW_V2_CHECK FAIL %s"),Label);}
}
bool ALWGameMode::RequireV2(bool Passed,const TCHAR* Label)
{Check(Passed,Label);if(!Passed)FinishV2Smoke();return Passed;}
void ALWGameMode::CaptureV2(const TCHAR* Name)
{
    const FString Directory=FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()/(FParse::Param(FCommandLine::Get(),TEXT("LWV17Smoke"))?TEXT("ScreenshotsV17"):FParse::Param(FCommandLine::Get(),TEXT("LWV16Smoke"))?TEXT("ScreenshotsV16"):FParse::Param(FCommandLine::Get(),TEXT("LWV15Smoke"))?TEXT("ScreenshotsV15"):FParse::Param(FCommandLine::Get(),TEXT("LWV14Smoke"))?TEXT("ScreenshotsV14"):FParse::Param(FCommandLine::Get(),TEXT("LWV11Smoke"))?TEXT("ScreenshotsV11"):FParse::Param(FCommandLine::Get(),TEXT("LWV10Smoke"))?TEXT("ScreenshotsV10"):FParse::Param(FCommandLine::Get(),TEXT("LWV9Smoke"))?TEXT("ScreenshotsV9"):FParse::Param(FCommandLine::Get(),TEXT("LWV7Smoke"))?TEXT("ScreenshotsV7"):FParse::Param(FCommandLine::Get(),TEXT("LWV6Smoke"))?TEXT("ScreenshotsV6"):FParse::Param(FCommandLine::Get(),TEXT("LWV5Smoke"))?TEXT("ScreenshotsV5"):FParse::Param(FCommandLine::Get(),TEXT("LWV4Smoke"))?TEXT("ScreenshotsV4"):FParse::Param(FCommandLine::Get(),TEXT("LWV3Smoke"))?TEXT("ScreenshotsV3"):TEXT("ScreenshotsV2")));
    if(!IFileManager::Get().MakeDirectory(*Directory,true)){Check(false,TEXT("screenshot directory writable"));return;}
    const FString Filename=Directory/(FString(Name)+TEXT(".png"));
    if(IFileManager::Get().FileExists(*Filename)&&!IFileManager::Get().Delete(*Filename,false,true))
    {Check(false,TEXT("previous smoke screenshot can be replaced"));return;}
    V2->Screenshots.AddUnique(Filename);FScreenshotRequest::RequestScreenshot(Filename,true,false);
    UE_LOG(LogTemp,Display,TEXT("LW_V2_SCREENSHOT %s"),*Filename);
}
void ALWGameMode::FinishV2Smoke()
{
    if(!V2.IsValid()||V2->bDone)return;
    V2->bDone=true;
    if(ALWCharacter* Player=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0)))
    {
        Player->StopAttack();
        if(V2->bSettingsSaved&&IsValid(Player->World)&&IsValid(Player->Camera))
        {
            if(V2->bVideoTestActive){
                if(auto* Settings=UGameUserSettings::GetGameUserSettings()){
                    Settings->SetScreenResolution(V2->OriginalPendingResolution);Settings->SetFullscreenMode(EWindowMode::Type(V2->OriginalPendingWindow));Settings->ApplyResolutionSettings(false);
                }
                Player->bVideoConfirm=false;UGameplayStatics::SetGamePaused(this,false);
            }
            Player->Sensitivity=V2->OriginalSensitivity;Player->MasterVolume=V2->OriginalVolume;Player->bCrust=V2->bOriginalCrust;
            Player->PendingResolution=V2->OriginalPendingResolution;Player->PendingWindowMode=V2->OriginalPendingWindow;
            Player->ApplySettings();
        }
    }
    if(V2->bAudioSmoke&&V2->bSettingsSaved)
        if(FAudioDeviceHandle Device=GetWorld()->GetAudioDevice())Device->SetTransientPrimaryVolume(V2->OriginalVolume);
    if(FParse::Param(FCommandLine::Get(),TEXT("LWVehicle30Smoke"))){UE_LOG(LogTemp,Display,TEXT("LW_VEHICLE30_DONE failures=%d checks=%d"),TestFailures,V2->Checks);}
    if(FParse::Param(FCommandLine::Get(),TEXT("LWV17Smoke"))){UE_LOG(LogTemp,Display,TEXT("AAM_V17_DONE failures=%d checks=%d"),TestFailures,V2->Checks);}
    if(FParse::Param(FCommandLine::Get(),TEXT("LWV16Smoke"))){UE_LOG(LogTemp,Display,TEXT("AAM_V16_DONE failures=%d checks=%d"),TestFailures,V2->Checks);}
    if(FParse::Param(FCommandLine::Get(),TEXT("LWV15Smoke"))){UE_LOG(LogTemp,Display,TEXT("AAM_V15_DONE failures=%d checks=%d"),TestFailures,V2->Checks);}
    if(FParse::Param(FCommandLine::Get(),TEXT("LWV14Smoke"))){UE_LOG(LogTemp,Display,TEXT("LW_V14_DONE failures=%d checks=%d"),TestFailures,V2->Checks);}
    else if(FParse::Param(FCommandLine::Get(),TEXT("LWV11Smoke")))UE_LOG(LogTemp,Display,TEXT("LW_V11_DONE failures=%d checks=%d"),TestFailures,V2->Checks);
    if(FParse::Param(FCommandLine::Get(),TEXT("LWV10Smoke")))UE_LOG(LogTemp,Display,TEXT("LW_V10_DONE failures=%d checks=%d"),TestFailures,V2->Checks);
    if(FParse::Param(FCommandLine::Get(),TEXT("LWV9Smoke")))UE_LOG(LogTemp,Display,TEXT("LW_V9_DONE failures=%d checks=%d"),TestFailures,V2->Checks);
    if(FParse::Param(FCommandLine::Get(),TEXT("LWV7Smoke")))UE_LOG(LogTemp,Display,TEXT("LW_V7_DONE failures=%d checks=%d"),TestFailures,V2->Checks);
    if(FParse::Param(FCommandLine::Get(),TEXT("LWV6Smoke")))UE_LOG(LogTemp,Display,TEXT("LW_V6_DONE failures=%d checks=%d"),TestFailures,V2->Checks);
    if(FParse::Param(FCommandLine::Get(),TEXT("LWV5Smoke")))UE_LOG(LogTemp,Display,TEXT("LW_V5_DONE failures=%d checks=%d"),TestFailures,V2->Checks);
    if(FParse::Param(FCommandLine::Get(),TEXT("LWV4Smoke")))UE_LOG(LogTemp,Display,TEXT("LW_V4_DONE failures=%d checks=%d"),TestFailures,V2->Checks);
    if(FParse::Param(FCommandLine::Get(),TEXT("LWV3Smoke")))UE_LOG(LogTemp,Display,TEXT("LW_V3_DONE failures=%d checks=%d"),TestFailures,V2->Checks);
    UE_LOG(LogTemp,Display,TEXT("LW_V2_DONE failures=%d checks=%d seconds=%.1f"),TestFailures,V2->Checks,FPlatformTime::Seconds()-V2->StartedAt);
    if(FParse::Param(FCommandLine::Get(),TEXT("LWSmoke")))UE_LOG(LogTemp,Display,TEXT("LW_SMOKE_DONE failures=%d"),TestFailures);
    FPlatformMisc::RequestExit(false);
}
void ALWGameMode::AuditV2(const ALWCharacter& Player,const TCHAR* Context)
{
    TSet<FGuid> Ids;
    Check(LWV2ValidItems(Player.Inventory,10,Ids)&&LWV2ValidItems(Player.Stash,14,Ids),
        *FString::Printf(TEXT("%s: unique identities, placement, capacities and magazine links"),Context));
}
bool ALWGameMode::SpawnV2Target(ALWCharacter& Player)
{
    if(IsValid(TestEnemy)&&!TestEnemy->bDead)return true;
    FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    TestEnemy=GetWorld()->SpawnActor<ALWZombie>(FVector(2320,0,125),FRotator(0,180,0),Params);
    if(!RequireV2(IsValid(TestEnemy),TEXT("combat target spawned")))return false;
    ++Player.World->ZombieCount;TestEnemy->PersistentId=LWV2EnemyId;TestEnemy->Health=50000;TestEnemy->Stagger=600;
    TestEnemy->Home=TestEnemy->GetActorLocation();return true;
}
void ALWGameMode::Tick(float Dt)
{
    Super::Tick(Dt);if(!bSmoke||!V2.IsValid()||V2->bDone)return;
    // Exercise a live audio device silently; never use the player's persisted MasterVolume to mute.
    const auto MuteAudio=[this]()
    {
        if(V2->bAudioSmoke&&!V2->bDone)
            if(FAudioDeviceHandle Device=GetWorld()->GetAudioDevice())Device->SetTransientPrimaryVolume(0.f);
    };
    MuteAudio();
    // Settings/save-load phases may call ApplySettings; reapply transient mute on every return.
    // Finish marks bDone before restoring volume, so this guard cannot mute the restored device.
    ON_SCOPE_EXIT {MuteAudio();};
    TestClock=float(FPlatformTime::Seconds()-V2->StartedAt);
    if(TestClock>210){Check(false,TEXT("overall smoke watchdog exceeded 210 seconds"));FinishV2Smoke();return;}
    ALWCharacter* Player=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
    if(!IsValid(Player)||!IsValid(Player->World)||!IsValid(Player->Controller))
    {
        if(TestClock>25||!V2->Steps.IsEmpty()){Check(false,TEXT("player controller and world remain available"));FinishV2Smoke();}
        return;
    }
    if(V2->Steps.IsEmpty()){Player->World->EnableEncounters=false;if(FParse::Param(FCommandLine::Get(),TEXT("LWV17Smoke")))BuildV17Smoke(*Player);else if(FParse::Param(FCommandLine::Get(),TEXT("LWV16Smoke")))BuildV16Smoke(*Player);else if(FParse::Param(FCommandLine::Get(),TEXT("LWV15Smoke")))BuildV15Smoke(*Player);else if(FParse::Param(FCommandLine::Get(),TEXT("LWV14Smoke")))BuildV14Smoke(*Player);else if(FParse::Param(FCommandLine::Get(),TEXT("LWV11Smoke")))BuildV11Smoke(*Player);else if(FParse::Param(FCommandLine::Get(),TEXT("LWV10Smoke")))BuildV10Smoke(*Player);else if(FParse::Param(FCommandLine::Get(),TEXT("LWV9Smoke")))BuildV9Smoke(*Player);else if(FParse::Param(FCommandLine::Get(),TEXT("LWV7Smoke")))BuildV7Smoke(*Player);else if(FParse::Param(FCommandLine::Get(),TEXT("LWV6Smoke")))BuildV6Smoke(*Player);else if(FParse::Param(FCommandLine::Get(),TEXT("LWV5Smoke")))BuildV5Smoke(*Player);else if(FParse::Param(FCommandLine::Get(),TEXT("LWV4Smoke")))BuildV4Smoke(*Player);else if(FParse::Param(FCommandLine::Get(),TEXT("LWV3Smoke")))BuildV3Smoke(*Player);else BuildV2Smoke(*Player);}
    if(V2->bDone)return;
    // Quiet procedural bystanders only; the designated zombie keeps its real AI and physics.
    for(TActorIterator<ALWZombie> It(GetWorld());It;++It)if(*It!=TestEnemy&&!It->bDead&&!Cast<ALWResident>(*It)&&!Cast<ALWStoryEnemy>(*It)&&!Cast<ALWBorderGuard51>(*It))
    {It->SetActorTickEnabled(false);It->GetCharacterMovement()->StopMovementImmediately();It->GetCharacterMovement()->DisableMovement();}
    if(V2->Observe)V2->Observe(*Player);
    if(V2->Index>=V2->Steps.Num()){FinishV2Smoke();return;}
    FLWV2Step& Step=V2->Steps[V2->Index];TestPhase=V2->Index;
    if(!V2->bEntered)
    {
        V2->bEntered=true;V2->StepStartedAt=FPlatformTime::Seconds();
        UE_LOG(LogTemp,Display,TEXT("LW_V2_PHASE %d/%d %s"),V2->Index+1,V2->Steps.Num(),*Step.Name);
        if(Step.Begin)Step.Begin(*Player);
        return; // Always allow a genuine gameplay/HUD frame after an action.
    }
    const double Elapsed=FPlatformTime::Seconds()-V2->StepStartedAt;
    if(Elapsed>Step.TimeoutSeconds)
    {Check(false,*FString::Printf(TEXT("phase timed out: %s"),*Step.Name));FinishV2Smoke();return;}
    if(Elapsed<Step.MinimumSeconds||(Step.Ready&&!Step.Ready(*Player)))return;
    if(Step.End)Step.End(*Player);
    if(V2->bDone)return;
    ++V2->Index;V2->bEntered=false;
}
void ALWGameMode::BuildV2Smoke(ALWCharacter& InitialPlayer)
{
    if(!RequireV2(IsValid(InitialPlayer.Camera)&&IsValid(InitialPlayer.WeaponRoot)&&IsValid(InitialPlayer.WeaponMesh),TEXT("player visual components initialized")))return;
    V2->OriginalSensitivity=InitialPlayer.Sensitivity;V2->OriginalVolume=InitialPlayer.MasterVolume;V2->bOriginalCrust=InitialPlayer.bCrust;
    V2->OriginalPendingResolution=InitialPlayer.PendingResolution;V2->OriginalPendingWindow=InitialPlayer.PendingWindowMode;V2->bSettingsSaved=true;
    auto Add=[this](const TCHAR* Name,double Wait,FLWV2Action Begin,FLWV2Action End=FLWV2Action(),FLWV2Ready Ready=FLWV2Ready(),double Timeout=25)
    {V2->Steps.Add({Name,Wait,Timeout,MoveTemp(Begin),MoveTemp(End),MoveTemp(Ready)});};

    Add(TEXT("title menu"),2,[](ALWCharacter&){},[this](ALWCharacter& P)
    {Check(P.bMenu&&!P.bStarted,TEXT("launch starts at title menu"));CaptureV2(TEXT("01_Menu"));});
    Add(TEXT("settings bounds and pending video choices"),1,[this](ALWCharacter& P)
    {
        P.ToggleSettings();Check(P.bSettings,TEXT("settings panel opens"));
        P.Sensitivity=4.9f;P.CycleSensitivity();Check(FMath::IsNearlyEqual(P.Sensitivity,5.f),TEXT("sensitivity increase reaches 5"));
        P.CycleSensitivity();Check(P.Sensitivity==5.f,TEXT("sensitivity increase clamps at 5"));
        P.Sensitivity=50;P.ApplySettings();Check(P.Sensitivity==5,TEXT("settings clamp oversized sensitivity"));
        P.Sensitivity=-1;P.ApplySettings();Check(FMath::IsNearlyEqual(P.Sensitivity,.025f),TEXT("settings clamp minimum sensitivity"));
        P.LowerSensitivity();Check(FMath::IsNearlyEqual(P.Sensitivity,.025f),TEXT("decreasing sensitivity cannot underflow"));
        P.Sensitivity=2;P.ApplySettings();Check(P.Sensitivity>1&&P.Sensitivity<=5,TEXT("higher sensitivity remains usable"));
        UGameUserSettings* Settings=UGameUserSettings::GetGameUserSettings();
        if(!RequireV2(Settings!=nullptr,TEXT("game user settings available")))return;
        const FIntPoint ActualResolution=Settings->GetScreenResolution();const auto ActualMode=Settings->GetFullscreenMode();
        P.PendingResolution=FIntPoint(1280,720);P.CycleResolution();Check(P.PendingResolution==FIntPoint(1600,900),TEXT("resolution choice advances"));
        for(int32 I=0;I<4;++I)P.CycleResolution();Check(P.PendingResolution==FIntPoint(1280,720),TEXT("resolution choices wrap"));
        P.PendingWindowMode=2;P.CycleWindowMode();Check(P.PendingWindowMode==0&&P.WindowModeName()==TEXT("FULLSCREEN"),TEXT("fullscreen pending choice"));
        P.CycleWindowMode();Check(P.PendingWindowMode==1&&P.WindowModeName()==TEXT("BORDERLESS"),TEXT("borderless pending choice"));
        P.CycleWindowMode();Check(P.PendingWindowMode==2&&P.WindowModeName()==TEXT("WINDOWED"),TEXT("windowed pending choice"));
        Check(Settings->GetScreenResolution()==ActualResolution&&Settings->GetFullscreenMode()==ActualMode&&!P.bVideoConfirm,TEXT("pending video choices do not change offscreen resolution or mode"));
    },[this](ALWCharacter&){CaptureV2(TEXT("02_Settings"));});

    Add(TEXT("apply video resolution"),2,[this](ALWCharacter& P)
    {
        V2->bVideoTestActive=true;P.PendingResolution=FIntPoint(1600,900);P.PendingWindowMode=2;P.ApplyVideo();
    },[this](ALWCharacter& P)
    {
        int32 Width=0,Height=0;Cast<APlayerController>(P.Controller)->GetViewportSize(Width,Height);
        Check(P.bVideoConfirm&&Width==1600&&Height==900,TEXT("applied video mode creates a 1600x900 viewport and confirmation"));
        CaptureV2(TEXT("02A_VideoConfirmation"));
    });
    Add(TEXT("video timeout while paused"),16,[this](ALWCharacter&){UGameplayStatics::SetGamePaused(this,true);},[this](ALWCharacter& P)
    {
        auto* Settings=UGameUserSettings::GetGameUserSettings();
        Check(!P.bVideoConfirm&&Settings->GetScreenResolution()==Settings->GetLastConfirmedScreenResolution()&&Settings->GetFullscreenMode()==Settings->GetLastConfirmedFullscreenMode(),TEXT("unconfirmed video changes revert after 15 real seconds while paused"));
        Settings->SetScreenResolution(V2->OriginalPendingResolution);Settings->SetFullscreenMode(EWindowMode::Type(V2->OriginalPendingWindow));Settings->ApplyResolutionSettings(false);
        P.PendingResolution=V2->OriginalPendingResolution;P.PendingWindowMode=V2->OriginalPendingWindow;V2->bVideoTestActive=false;UGameplayStatics::SetGamePaused(this,false);
    });

    Add(TEXT("new game defaults"),2,[this](ALWCharacter& P)
    {
        P.NewGame();Check(P.bStarted&&!P.bMenu&&!P.bSafehouse&&P.CanAct(),TEXT("new game enters playable outside world"));
        Check(P.Money==150&&P.Health==100&&P.Hunger==100&&P.Thirst==100,TEXT("new game resets credits and survival vitals"));
        Check(P.World->KilledZombies.IsEmpty()&&P.LastDeathBag.IsNone(),TEXT("new game clears defeat and death history"));
        Check(LWV2FindId(P.Inventory,TEXT("crowbar")).IsValid()&&LWV2FindId(P.Inventory,TEXT("shotgun")).IsValid()&&LWV2FindId(P.Inventory,TEXT("revolver")).IsValid(),TEXT("new defaults supply crowbar shotgun and revolver"));
        const auto* Shot=P.FindItem(LWV2FindId(P.Inventory,TEXT("shotgun")));
        Check(Shot&&Shot->Rounds==5&&Shot->Chamber==1&&P.AmmoCount(TEXT("ammo_12g"))==24,TEXT("new shotgun uses separate tube chamber and reserve"));
        const auto* Rev=P.FindItem(LWV2FindId(P.Inventory,TEXT("revolver")));
        Check(Rev&&LWV2CylinderCount(*Rev,1)==6&&P.AmmoCount(TEXT("ammo_357"))==24,TEXT("new revolver supplies six live chambers and reserve"));
        AuditV2(P,TEXT("new defaults"));
    },[this](ALWCharacter& P)
    {
        const int32 Ring=2*FMath::Clamp(P.World->RenderRadius,1,4)+1;
        Check(P.World->Chunks.Num()>=Ring*Ring,TEXT("initial configured chunk ring loaded"));
        Check(!P.GetCharacterMovement()->IsFalling()&&P.GetActorLocation().Z>80&&P.GetActorLocation().Z<240,TEXT("player stands on generated ground"));
        Check(IsValid(P.World->Attenuation)&&P.World->Attenuation->Attenuation.bEnableOcclusion,TEXT("audio occlusion enabled"));
        Check(IsValid(P.World->Attenuation)&&P.World->Attenuation->Attenuation.OcclusionLowPassFilterFrequency<1500,TEXT("occluded audio low-pass configured"));
        if(V2->bAudioSmoke&&!FParse::Param(FCommandLine::Get(),TEXT("NoSound")))
        {
            Check(GetWorld()->GetAudioDevice().IsValid(),TEXT("audio smoke initializes a real audio device"));
            Check(IsValid(P.World->Wind.Get()),TEXT("audio smoke creates wind audio component"));
            Check(IsValid(P.World->Drone.Get()),TEXT("audio smoke creates drone audio component"));
            Check(IsValid(P.World->Music.Get()),TEXT("audio smoke creates music audio component"));
        }
        for(const auto& Pair:P.World->Meshes)Check(IsValid(Pair.Value.Get()),*FString::Printf(TEXT("mesh %s loaded"),*Pair.Key.ToString()));
        for(FName Name:ULWAudioCatalog::GetDefaultSlotNames())Check(IsValid(P.World->Sounds.FindRef(Name).Get()),*FString::Printf(TEXT("sound %s loaded"),*Name.ToString()));
        CaptureV2(TEXT("03_Gameplay"));
    });
    Add(TEXT("inventory panel"),1,[](ALWCharacter& P){P.ToggleInventory();},[this](ALWCharacter& P)
    {Check(P.bInventory&&!P.CanAct(),TEXT("inventory opens and gates combat input"));CaptureV2(TEXT("04_Inventory"));});
    Add(TEXT("field map and waypoint"),1,[](ALWCharacter& P){P.ClosePanels();P.SetWaypoint(FVector2D(ALWWorld::BunkerDoorPosition()));P.ToggleMap();},[this](ALWCharacter& P)
    {Check(P.bMap&&P.bWaypoint&&!P.CanAct(),TEXT("map opens with bunker waypoint and gates combat"));CaptureV2(TEXT("05_Map"));});

    Add(TEXT("inventory does not freeze zombie perception"),.6,[this](ALWCharacter& P)
    {
        P.ClosePanels();P.ClearWaypoint();LWV2Teleport(P,FVector(2000,0,115));if(!SpawnV2Target(P))return;
        P.ToggleInventory();TestEnemy->Alert=0;TestEnemy->Hear(P.GetActorLocation(),1000);
    },[this](ALWCharacter& P)
    {Check(P.bInventory&&IsValid(TestEnemy)&&TestEnemy->Alert>0,TEXT("outside inventory panel preserves live zombie hearing and alert"));});
    Add(TEXT("map does not freeze zombie perception"),.6,[](ALWCharacter& P){P.ClosePanels();P.ToggleMap();},[this](ALWCharacter& P)
    {Check(P.bMap&&IsValid(TestEnemy)&&TestEnemy->Alert>0,TEXT("outside map panel preserves live zombie alert"));});

    Add(TEXT("bunker entrance access"),2,[this](ALWCharacter& P)
    {
        P.ClosePanels();P.LeaveSafehouse();
        const FVector Target=ALWWorld::BunkerDoorPosition()+FVector(0,0,125);
        P.Controller->SetControlRotation((Target-P.GetPawnViewLocation()).Rotation());
    },[this](ALWCharacter& P)
    {
        Check(!P.GetCharacterMovement()->IsFalling()&&P.GetActorLocation().Z>85&&P.GetActorLocation().Z<160,TEXT("bunker exit stands above its level approach"));
        Check(P.World->HeightAt(FVector2D(ALWWorld::BunkerDoorPosition()))==0,TEXT("bunker parcel terrain is flat"));
        Check(IsValid(P.ObjectFocus.Get())&&P.ObjectFocus->Kind==ELWObjectKind::BunkerEntrance,TEXT("bunker entrance reachable by player interaction trace"));
        CaptureV2(TEXT("05A_BunkerEntrance"));
    });

    struct FShelterCase {TWeakObjectPtr<ALWWorldObject> Stash;float Health=0,Hunger=0,Thirst=0;};
    auto Shelter=MakeShared<FShelterCase>();
    Add(TEXT("safehouse protection"),4,[this,Shelter](ALWCharacter& P)
    {
        P.ClosePanels();P.ClearWaypoint();if(!SpawnV2Target(P))return;
        ALWWorldObject* Door=LWV2Object(P,ELWObjectKind::BunkerEntrance);
        if(!RequireV2(IsValid(Door),TEXT("generated bunker entrance exists")))return;
        LWV2Teleport(P,Door->GetActorLocation()+FVector(0,250,110));Door->Use(&P);
        Check(P.bSafehouse&&ALWWorld::IsSafePosition(P.GetActorLocation())&&P.CanAct(),TEXT("airlock enters protected playable bunker"));
        P.Health=72;P.Hunger=44;P.Thirst=53;Shelter->Health=P.Health;Shelter->Hunger=P.Hunger;Shelter->Thirst=P.Thirst;
        const auto Before=LWV2Snapshot(P.Inventory);
        Check(UGameplayStatics::ApplyDamage(&P,1000,nullptr,TestEnemy,nullptr)==0&&P.Health==72,TEXT("safehouse blocks lethal damage"));
        Check(Before==LWV2Snapshot(P.Inventory),TEXT("safehouse damage cannot consume armor or carried gear"));
        TestEnemy->Alert=12;TestEnemy->Interest=P.GetActorLocation();TestEnemy->Path.Add(P.GetActorLocation());
        TestEnemy->Hear(P.GetActorLocation(),100000);
        Check(TestEnemy->Alert==0&&TestEnemy->Path.IsEmpty(),TEXT("safehouse noise clears zombie targeting and route"));
        TestEnemy->FindRoute(P.GetActorLocation());Check(TestEnemy->Path.IsEmpty(),TEXT("zombie rejects route into bunker"));
    },[this,Shelter](ALWCharacter& P)
    {
        Check(P.Health==Shelter->Health&&P.Hunger==Shelter->Hunger&&P.Thirst==Shelter->Thirst,TEXT("safehouse preserves health hunger and thirst during live ticks"));
        Check(P.bSafehouse&&P.bIndoors,TEXT("bunker has protected indoor acoustic state"));
        Check(IsValid(TestEnemy)&&TestEnemy->Alert==0&&TestEnemy->GetVelocity().IsNearlyZero(),TEXT("safehouse suppresses zombie pursuit across ticks"));
        CaptureV2(TEXT("06_Safehouse"));
    });

    Add(TEXT("stash access and atomic drag transactions"),1,[this,Shelter](ALWCharacter& P)
    {
        ALWWorldObject* Stash=LWV2Object(P,ELWObjectKind::Stash);Shelter->Stash=Stash;
        if(!RequireV2(IsValid(Stash),TEXT("generated secured stash exists")))return;
        LWV2Teleport(P,Stash->GetActorLocation()+FVector(0,-200,110));Stash->Use(&P);
        if(!RequireV2(P.ItemsFor(1)==&P.Stash,TEXT("nearby open stash grants access")))return;
        const FGuid Bat=LWV2FindId(P.Stash,TEXT("bat"));
        if(!RequireV2(Bat.IsValid(),TEXT("stash supplies the legacy bat")))return;
        const auto InvBefore=LWV2Snapshot(P.Inventory),StashBefore=LWV2Snapshot(P.Stash);
        Check(!P.MoveItem(1,0,Bat,40,40,false),TEXT("out-of-bounds stash transfer is rejected"));
        Check(InvBefore==LWV2Snapshot(P.Inventory)&&StashBefore==LWV2Snapshot(P.Stash),TEXT("failed transfer preserves all source and destination fields"));
        const auto* Occupied=P.FindItem(LWV2FindId(P.Inventory,TEXT("food")));
        if(!RequireV2(Occupied!=nullptr,TEXT("occupied-cell drag fixture exists")))return;
        Check(!P.MoveItem(1,0,Bat,Occupied->X,Occupied->Y,false),TEXT("occupied inventory cells reject transfer"));
        Check(InvBefore==LWV2Snapshot(P.Inventory)&&StashBefore==LWV2Snapshot(P.Stash),TEXT("occupied-cell rejection is atomic"));
        const FVector Near=P.GetActorLocation();LWV2Teleport(P,Stash->GetActorLocation()+FVector(1300,0,100));
        Check(P.ItemsFor(1)==nullptr&&!P.MoveItem(1,0,Bat,0,0,false),TEXT("stash rejects access beyond 550 units"));
        LWV2Teleport(P,Near);P.ClosePanels();
        Check(P.ItemsFor(1)==nullptr&&!P.MoveItem(1,0,Bat,0,0,false),TEXT("closed stash rejects drag requests"));
        Stash->Use(&P);P.LeaveSafehouse();P.OpenContainer(Stash);
        Check(!P.bSafehouse&&P.ItemsFor(1)==nullptr&&!P.MoveItem(1,0,Bat,0,0,false),TEXT("outside player cannot access secured stash"));
        Check(InvBefore==LWV2Snapshot(P.Inventory)&&StashBefore==LWV2Snapshot(P.Stash),TEXT("denied stash access leaves both inventories unchanged"));
        P.EnterSafehouse();LWV2Teleport(P,Stash->GetActorLocation()+FVector(0,-200,110));Stash->Use(&P);
        Check(LWV2MoveAuto(P,1,0,Bat),TEXT("stash-to-inventory drag succeeds"));
        Check(P.FindItem(Bat)!=nullptr&&!LWV2FindId(P.Stash,TEXT("bat")).IsValid(),TEXT("successful drag transfers original identity exactly once"));
        Check(LWV2MoveAuto(P,0,1,Bat),TEXT("inventory-to-stash drag succeeds"));AuditV2(P,TEXT("stash round trip"));
    },[this](ALWCharacter&){CaptureV2(TEXT("07_Stash"));});

    Add(TEXT("ammunition drag compatibility and conservation"),.5,[this](ALWCharacter& P)
    {
        const FName Mags[]={TEXT("mag_sniper5"),TEXT("mag_smg30"),TEXT("mag_rifle30"),TEXT("mag_lmg100")};
        for(FName Definition:Mags)
        {
            const FGuid MagId=LWV2FindId(P.Stash,Definition,0);
            const FName AmmoType=LWItems::Def(Definition).AmmoType;
            const FGuid AmmoId=LWV2FindId(P.Stash,AmmoType),WrongId=LWV2FindId(P.Stash,AmmoType==TEXT("ammo_556")?FName(TEXT("ammo_9mm")):FName(TEXT("ammo_556")));
            if(!RequireV2(MagId.IsValid()&&AmmoId.IsValid()&&WrongId.IsValid(),*FString::Printf(TEXT("default ammo drag fixtures for %s exist"),*Definition.ToString())))return;
            const auto Before=LWV2Snapshot(P.Stash),InventoryBefore=LWV2Snapshot(P.Inventory);const auto AmmoBefore=LWV2Ammo(P);
            Check(!P.LoadAmmoOnto(1,WrongId,1,MagId),*FString::Printf(TEXT("%s rejects wrong ammunition through drag API"),*Definition.ToString()));
            Check(Before==LWV2Snapshot(P.Stash)&&InventoryBefore==LWV2Snapshot(P.Inventory),TEXT("wrong ammo changes no item fields"));
            Check(P.LoadAmmoOnto(1,AmmoId,1,MagId),*FString::Printf(TEXT("%s accepts matching ammunition through drag API"),*Definition.ToString()));
            const auto* Mag=P.Stash.FindByPredicate([&](const auto& I){return I.Id==MagId;});
            Check(Mag&&Mag->Rounds>0&&Mag->Rounds<=LWItems::Def(Definition).Capacity,TEXT("ammo drag fills only within capacity"));
            Check(LWV2SameAmmo(AmmoBefore,LWV2Ammo(P)),TEXT("ammo drag conserves loose plus loaded rounds"));
        }
        AuditV2(P,TEXT("ammo drag"));
    });

    Add(TEXT("take test loadout from new-game stash"),.5,[this](ALWCharacter& P)
    {
        const auto AmmoBefore=LWV2Ammo(P);TArray<FGuid> Transfer;
        for(const auto& Item:P.Stash)
        {
            const FName Category=LWItems::Def(Item.Definition).Category;
            if(Category==TEXT("Weapon")||Category==TEXT("Magazine")||Category==TEXT("Ammo"))Transfer.Add(Item.Id);
        }
        for(FGuid Id:Transfer)if(!RequireV2(LWV2MoveAuto(P,1,0,Id),TEXT("default weapon magazine or ammo transfers from secured stash")))return;
        Check(!P.Stash.IsEmpty(),TEXT("secured gear remains for death-persistence assertions"));
        Check(LWV2SameAmmo(AmmoBefore,LWV2Ammo(P)),TEXT("loadout transfer conserves complete ammunition ledger"));
        for(FName Gun:{FName(TEXT("bat")),FName(TEXT("sniper")),FName(TEXT("smg")),FName(TEXT("rifle")),FName(TEXT("lmg"))})
            Check(LWV2FindId(P.Inventory,Gun).IsValid(),*FString::Printf(TEXT("default %s is carried for combat"),*Gun.ToString()));
        AuditV2(P,TEXT("complete loadout"));P.LeaveSafehouse();LWV2Teleport(P,FVector(2000,0,115));
    });

    for(int32 Index=0;Index<2;++Index)
    {
        struct FMeleeCase {float Health=0;TMap<FName,int64> Ammo;};
        auto Melee=MakeShared<FMeleeCase>();
        const FName Definition=Index==0?FName(TEXT("crowbar")):FName(TEXT("bat"));
        Add(*FString::Printf(TEXT("%s equip"),*Definition.ToString()),.6,[this,Index,Definition](ALWCharacter& P)
        {
            P.ClosePanels();if(!SpawnV2Target(P))return;
            const FGuid Id=LWV2FindId(P.Inventory,Definition);
            if(!RequireV2(Id.IsValid()&&P.MoveItem(0,0,Id,0,0,false,TEXT("Melee")),TEXT("melee equipment drag succeeds")))return;
            P.Equip(Index);P.bAim=false;
            LWV2Teleport(P,TestEnemy->GetActorLocation()-FVector(140,0,0));
            P.Controller->SetControlRotation((TestEnemy->GetActorLocation()+FVector(0,0,30)-P.GetPawnViewLocation()).Rotation());
        },[this,Index](ALWCharacter& P)
        {Check(P.Weapon==Index&&P.ActiveGun()!=nullptr,TEXT("legacy melee weapon selected from inventory"));CaptureV2(Index==0?TEXT("08_Crowbar"):TEXT("09_Bat"));},
        [](ALWCharacter& P){return P.AttackTimer<=0&&!P.bReloading;});
        Add(*FString::Printf(TEXT("%s actual sweep"),*Definition.ToString()),1.2,[this,Melee](ALWCharacter& P)
        {
            if(!RequireV2(IsValid(TestEnemy)&&P.CanAct()&&!P.bSafehouse,TEXT("melee fixture can attack outside bunker")))return;
            Melee->Health=TestEnemy->Health;Melee->Ammo=LWV2Ammo(P);P.Attack();P.StopAttack();TestEnemy->Stagger=600;
        },[this,Melee,Definition](ALWCharacter& P)
        {
            Check(IsValid(TestEnemy)&&TestEnemy->Health<Melee->Health,*FString::Printf(TEXT("%s sweep damages actual zombie"),*Definition.ToString()));
            Check(LWV2SameAmmo(Melee->Ammo,LWV2Ammo(P)),TEXT("melee attacks consume no ammunition"));
            AuditV2(P,TEXT("melee attack"));
        },[](ALWCharacter& P){return P.AttackTimer<=0;});
    }

    struct FGunCase
    {
        int32 Index=0;
        FName Definition;
        FGuid Id,OldMagazine;
        int32 OldRounds=0,OldChamber=0,ReserveBefore=0;
        float TargetHealth=0,ReloadDuration=0;
        TArray<int32> CylinderBefore;
        TMap<FName,int64> Ammo;
        bool bConserved=true,bKeptLive=true,bSawEjected=false,bSawEmptyAfterEjection=false;
    };
    const FName GunNames[]={TEXT("shotgun"),TEXT("revolver"),TEXT("sniper"),TEXT("smg"),TEXT("rifle"),TEXT("lmg")};
    for(int32 Offset=0;Offset<6;++Offset)
    {
        auto GunCase=MakeShared<FGunCase>();GunCase->Index=Offset+2;GunCase->Definition=GunNames[Offset];
        Add(*FString::Printf(TEXT("%s equip and initial load"),*GunCase->Definition.ToString()),.45,[this,GunCase](ALWCharacter& P)
        {
            P.ClosePanels();if(!SpawnV2Target(P))return;
            GunCase->Id=LWV2FindId(P.Inventory,GunCase->Definition);
            if(!RequireV2(GunCase->Id.IsValid(),TEXT("firearm is present in carried default gear")))return;
            const FName Slot=GunCase->Index==3?FName(TEXT("Sidearm")):FName(TEXT("Primary"));
            if(!RequireV2(P.MoveItem(0,0,GunCase->Id,0,0,false,Slot),TEXT("firearm equipment drag succeeds")))return;
            P.Equip(GunCase->Index);P.StartAim();LWV2Teleport(P,FVector(2000,0,115));
            P.Controller->SetControlRotation((TestEnemy->GetActorLocation()+FVector(0,0,25)-P.GetPawnViewLocation()).Rotation());
            if(!RequireV2(P.CanAct()&&!P.bSafehouse&&P.ActiveGun()&&P.ActiveGun()->Id==GunCase->Id,TEXT("equipped firearm can act outside bunker")))return;
            GunCase->Ammo=LWV2Ammo(P);GunCase->bConserved=true;
            if(!LWItems::Def(GunCase->Definition).MagazineType.IsNone())
            {
                P.Reload();Check(P.bReloading,*FString::Printf(TEXT("%s begins initial magazine load"),*GunCase->Definition.ToString()));
                UE_LOG(LogTemp,Display,TEXT("LW_V2_RELOAD %s initial duration=%.3f"),*GunCase->Definition.ToString(),P.ReloadDuration);
            }
            V2->Observe=[GunCase](ALWCharacter& Player){GunCase->bConserved&=LWV2SameAmmo(GunCase->Ammo,LWV2Ammo(Player));};
        },[this,GunCase](ALWCharacter& P)
        {
            Check(P.Weapon==GunCase->Index&&IsValid(P.WeaponMesh->GetStaticMesh()),*FString::Printf(TEXT("%s runtime weapon mesh configured"),*GunCase->Definition.ToString()));
            if(P.bReloading)CaptureV2(*FString::Printf(TEXT("%02d_%s_InitialReload"),10+GunCase->Index,*GunCase->Definition.ToString()));
        });
        Add(*FString::Printf(TEXT("%s wait for actual initial reload"),*GunCase->Definition.ToString()),.2,[](ALWCharacter&){},[this,GunCase](ALWCharacter& P)
        {
            V2->Observe=FLWV2Action();const auto* Gun=P.FindItem(GunCase->Id);
            if(!RequireV2(Gun!=nullptr,TEXT("initial reload retains firearm identity")))return;
            Check(GunCase->bConserved&&LWV2SameAmmo(GunCase->Ammo,LWV2Ammo(P)),TEXT("initial reload conserves ammunition on every observed tick"));
            if(GunCase->Index>=4)
            {
                const auto* Mag=P.FindItem(Gun->LoadedMagazine);
                Check(Mag&&Mag->Slot==TEXT("Loaded")&&Mag->Definition==LWItems::Def(GunCase->Definition).MagazineType,TEXT("initial reload links a real matching magazine"));
                Check(Gun->Chamber==1&&Gun->Rounds==0,TEXT("detachable firearm chambers one round without mirroring magazine count"));
            }
            P.StartAim();AuditV2(P,*GunCase->Definition.ToString());
        },[](ALWCharacter& P){return !P.bReloading&&P.AttackTimer<=0;},35);
        Add(*FString::Printf(TEXT("%s aimed view"),*GunCase->Definition.ToString()),.6,[](ALWCharacter&){},[this,GunCase](ALWCharacter& P)
        {
            Check(GunCase->Index==4?P.Camera->FieldOfView<35:P.Camera->FieldOfView<75,TEXT("aim changes weapon-appropriate field of view"));
            CaptureV2(*FString::Printf(TEXT("%02d_%s_Equipped"),20+GunCase->Index,*GunCase->Definition.ToString()));
        });

        const int32 Shots=GunCase->Index==3?2:1;
        for(int32 Shot=0;Shot<Shots;++Shot)
        {
            Add(*FString::Printf(TEXT("%s shot %d"),*GunCase->Definition.ToString(),Shot+1),1.7,[this,GunCase](ALWCharacter& P)
            {
                if(!RequireV2(IsValid(TestEnemy)&&P.ActiveGun()&&P.ActiveGun()->Id==GunCase->Id&&!P.bSafehouse&&P.CanAct(),TEXT("live firearm target and input prerequisites")))return;
                GunCase->Ammo=LWV2Ammo(P);GunCase->TargetHealth=TestEnemy->Health;
                P.Controller->SetControlRotation((TestEnemy->GetActorLocation()+FVector(0,0,25)-P.GetPawnViewLocation()).Rotation());
                P.Attack();P.StopAttack();TestEnemy->Stagger=600;
            },[this,GunCase](ALWCharacter& P)
            {
                auto Expected=GunCase->Ammo;--Expected.FindOrAdd(LWV2AmmoName(LWItems::Def(GunCase->Definition).AmmoType));
                Check(LWV2SameAmmo(Expected,LWV2Ammo(P)),*FString::Printf(TEXT("%s firing consumes exactly one live round"),*GunCase->Definition.ToString()));
                Check(IsValid(TestEnemy)&&TestEnemy->Health<GunCase->TargetHealth,*FString::Printf(TEXT("%s actual trace damages zombie"),*GunCase->Definition.ToString()));
                AuditV2(P,TEXT("after firing and action cycle"));
            },[](ALWCharacter& P){return P.AttackTimer<=0&&!P.bReloading;},12);
        }

        Add(*FString::Printf(TEXT("%s tactical or partial reload"),*GunCase->Definition.ToString()),.25,[this,GunCase](ALWCharacter& P)
        {
            const auto* Gun=P.FindItem(GunCase->Id);if(!RequireV2(Gun!=nullptr,TEXT("reload firearm identity exists")))return;
            GunCase->Ammo=LWV2Ammo(P);GunCase->OldMagazine=Gun->LoadedMagazine;GunCase->OldChamber=Gun->Chamber;
            const auto* OldMag=P.FindItem(GunCase->OldMagazine);GunCase->OldRounds=OldMag?OldMag->Rounds:Gun->Rounds;
            GunCase->CylinderBefore=Gun->Cylinder;GunCase->ReserveBefore=P.AmmoCount(LWItems::Def(GunCase->Definition).AmmoType);
            GunCase->bConserved=true;GunCase->bKeptLive=true;GunCase->bSawEjected=false;GunCase->bSawEmptyAfterEjection=false;
            if(GunCase->Index==3)Check(LWV2CylinderCount(*Gun,1)==4&&LWV2CylinderCount(*Gun,2)==2,TEXT("two revolver shots leave four live and two spent chambers"));
            P.Reload();Check(P.bReloading,*FString::Printf(TEXT("%s reload begins after firing"),*GunCase->Definition.ToString()));
            GunCase->ReloadDuration=P.ReloadDuration;
            UE_LOG(LogTemp,Display,TEXT("LW_V2_RELOAD %s tactical duration=%.3f"),*GunCase->Definition.ToString(),P.ReloadDuration);
            V2->Observe=[GunCase](ALWCharacter& Player)
            {
                GunCase->bConserved&=LWV2SameAmmo(GunCase->Ammo,LWV2Ammo(Player));
                if(GunCase->Index!=3)return;
                const auto* Revolver=Player.FindItem(GunCase->Id);if(!Revolver){GunCase->bKeptLive=false;return;}
                for(int32 I=0;I<GunCase->CylinderBefore.Num();++I)
                    if(GunCase->CylinderBefore[I]==1&&(!Revolver->Cylinder.IsValidIndex(I)||Revolver->Cylinder[I]!=1))GunCase->bKeptLive=false;
                if(Player.bReloading&&LWV2CylinderCount(*Revolver,2)==0)
                {
                    GunCase->bSawEjected=true;
                    GunCase->bSawEmptyAfterEjection|=LWV2CylinderCount(*Revolver,0)>0;
                }
            };
        },[this,GunCase](ALWCharacter& P)
        {
            Check(P.bReloading,*FString::Printf(TEXT("%s reload has a visible timed phase"),*GunCase->Definition.ToString()));
            CaptureV2(*FString::Printf(TEXT("%02d_%s_Reload"),30+GunCase->Index,*GunCase->Definition.ToString()));
        });
        Add(*FString::Printf(TEXT("%s wait for actual reload completion"),*GunCase->Definition.ToString()),.2,[](ALWCharacter&){},[this,GunCase](ALWCharacter& P)
        {
            V2->Observe=FLWV2Action();const auto* Gun=P.FindItem(GunCase->Id);
            if(!RequireV2(Gun!=nullptr,TEXT("completed reload retains firearm identity")))return;
            Check(GunCase->bConserved&&LWV2SameAmmo(GunCase->Ammo,LWV2Ammo(P)),*FString::Printf(TEXT("%s entire reload conserves ammo every observed tick"),*GunCase->Definition.ToString()));
            if(GunCase->Index==2)
            {
                Check(Gun->Rounds==5&&Gun->Chamber==1&&P.Shells==6,TEXT("shotgun reload restores legacy five-plus-one capacity"));
                Check(P.AmmoCount(TEXT("ammo_12g"))==GunCase->ReserveBefore-(Gun->Rounds-GunCase->OldRounds),TEXT("shotgun reserve debit matches shells actually inserted"));
            }
            else if(GunCase->Index==3)
            {
                Check(GunCase->bKeptLive,TEXT("partial revolver reload never discards existing live chambers"));
                Check(GunCase->bSawEjected&&GunCase->bSawEmptyAfterEjection,TEXT("revolver ejects spent casings before replacement rounds"));
                Check(LWV2CylinderCount(*Gun,1)==6&&LWV2CylinderCount(*Gun,2)==0,TEXT("revolver finishes with six live chambers and no spent cases"));
                Check(P.AmmoCount(TEXT("ammo_357"))==GunCase->ReserveBefore-2,TEXT("partial revolver reload consumes only two replacement rounds"));
            }
            else
            {
                const auto* OldMag=P.FindItem(GunCase->OldMagazine);const auto* NewMag=P.FindItem(Gun->LoadedMagazine);
                Check(GunCase->OldMagazine.IsValid()&&Gun->LoadedMagazine.IsValid()&&Gun->LoadedMagazine!=GunCase->OldMagazine,TEXT("tactical reload swaps real magazine identities"));
                Check(OldMag&&OldMag->Slot.IsNone()&&OldMag->Rounds==GunCase->OldRounds,TEXT("removed magazine returns to inventory with its remaining rounds"));
                Check(NewMag&&NewMag->Slot==TEXT("Loaded")&&NewMag->Definition==LWItems::Def(GunCase->Definition).MagazineType,TEXT("replacement magazine is linked and compatible"));
                Check(GunCase->OldChamber==1&&Gun->Chamber==1&&Gun->Rounds==0,TEXT("tactical reload preserves live chamber without duplicating feed rounds"));
            }
            AuditV2(P,TEXT("completed reload"));
        },[](ALWCharacter& P){return !P.bReloading&&P.AttackTimer<=0;},35);
    }

    struct FRagdollCase
    {
        float TorsoZ=0,GameTime=0;int64 Money=0;int32 Kills=0;
        TWeakObjectPtr<ALWZombie> Corpse;
    };
    auto Ragdoll=MakeShared<FRagdollCase>();
    Add(TEXT("lethal damage and physics handoff"),1.1,[this,Ragdoll](ALWCharacter& P)
    {
        P.StopAttack();P.ClosePanels();P.StopAim();
        if(!RequireV2(IsValid(TestEnemy)&&!TestEnemy->bDead,TEXT("ragdoll target survived preceding weapon tests")))return;
        TestEnemy->GetCharacterMovement()->StopMovementImmediately();TestEnemy->GetCharacterMovement()->DisableMovement();
        TestEnemy->SetActorLocation(FVector(2320,0,360),false,nullptr,ETeleportType::TeleportPhysics);
        Ragdoll->Money=P.Money;Ragdoll->Kills=P.Kills;Ragdoll->Corpse=TestEnemy;
        Ragdoll->GameTime=GetWorld()->GetTimeSeconds();
        if(!RequireV2(TestEnemy->Parts.Num()==7&&IsValid(TestEnemy->Parts[0]),TEXT("seven segmented zombie parts exist")))return;
        Ragdoll->TorsoZ=TestEnemy->Parts[0]->GetComponentLocation().Z;
        UGameplayStatics::ApplyDamage(TestEnemy,100000,P.Controller,&P,nullptr);
        UGameplayStatics::ApplyDamage(TestEnemy,100000,P.Controller,&P,nullptr);
        Check(TestEnemy->bDead&&P.Money==Ragdoll->Money+12&&P.Kills==Ragdoll->Kills+1,TEXT("lethal damage rewards corpse exactly once"));
        Check(P.World->KilledZombies.Contains(LWV2EnemyId),TEXT("defeated zombie identity recorded"));
        Check(!TestEnemy->IsActorTickEnabled()&&!TestEnemy->GetCharacterMovement()->IsComponentTickEnabled()&&TestEnemy->GetController()==nullptr,TEXT("death disables animation ticking movement and controller"));
        Check(TestEnemy->GetCapsuleComponent()->GetCollisionEnabled()==ECollisionEnabled::NoCollision,TEXT("dead capsule stops blocking"));
        for(int32 I=0;I<TestEnemy->Parts.Num();++I)
        {
            const UStaticMeshComponent* Part=TestEnemy->Parts[I];
            Check(IsValid(Part)&&Part->IsSimulatingPhysics()&&Part->IsGravityEnabled(),*FString::Printf(TEXT("ragdoll body %d simulates with gravity"),I));
        }
        bool Connected=TestEnemy->RagdollJoints.Num()==6;TSet<UPrimitiveComponent*> Bodies;
        for(UPhysicsConstraintComponent* Joint:TestEnemy->RagdollJoints)
        {
            if(!IsValid(Joint)||!Joint->ConstraintInstance.IsValidConstraintInstance()){Connected=false;continue;}
            UPrimitiveComponent* A=nullptr;UPrimitiveComponent* B=nullptr;FName BoneA,BoneB;
            Joint->GetConstrainedComponents(A,BoneA,B,BoneB);
            if(!A||!B||A==B)Connected=false;else{Bodies.Add(A);Bodies.Add(B);}
        }
        Check(Connected&&Bodies.Num()==7,TEXT("six initialized physics joints connect all seven corpse bodies"));
    },[this,Ragdoll](ALWCharacter& P)
    {
        ALWZombie* Corpse=Ragdoll->Corpse.Get();
        if(!RequireV2(IsValid(Corpse)&&Corpse->Parts.Num()==7&&IsValid(Corpse->Parts[0]),TEXT("corpse remains valid after gravity interval")))return;
        Check(Corpse->Parts[0]->GetComponentLocation().Z<Ragdoll->TorsoZ-40,TEXT("ragdoll actually falls after more than one second"));
        bool Together=true;
        for(UPhysicsConstraintComponent* Joint:Corpse->RagdollJoints)
        {
            if(!IsValid(Joint)||!Joint->ConstraintInstance.IsValidConstraintInstance()||Joint->IsBroken()){Together=false;continue;}
            UPrimitiveComponent* A=nullptr;UPrimitiveComponent* B=nullptr;FName BoneA,BoneB;
            Joint->GetConstrainedComponents(A,BoneA,B,BoneB);
            if(!IsValid(A)||!IsValid(B)||FVector::Dist(A->GetComponentLocation(),B->GetComponentLocation())>180)Together=false;
        }
        Check(Together,TEXT("falling corpse joints remain initialized and segments stay together"));
        const FTransform Before=Corpse->Parts[0]->GetComponentTransform();Corpse->Tick(.1f);
        Check(Before.Equals(Corpse->Parts[0]->GetComponentTransform()),TEXT("dead Tick cannot animate or rotate simulated pieces"));
        P.Controller->SetControlRotation((Corpse->Parts[0]->GetCenterOfMass()-P.GetPawnViewLocation()).Rotation());
    },[this,Ragdoll](ALWCharacter&){return GetWorld()->GetTimeSeconds()-Ragdoll->GameTime>=1.1f;});
    Add(TEXT("ragdoll screenshot"),.5,[](ALWCharacter&){},[this](ALWCharacter&){CaptureV2(TEXT("40_Ragdoll"));});

    struct FTradeCase
    {
        TWeakObjectPtr<ALWWorldObject> Trader;
        FGuid ItemId;
        FName RecordId;
        int64 Cost=0,Money=0;
        TArray<FLWItemInstance> StockBefore,InventoryBefore;
        FVector PatrolStart=FVector::ZeroVector;
    };
    auto Trade=MakeShared<FTradeCase>();
    Add(TEXT("wandering trader encounter"),2,[this,Trade](ALWCharacter& P)
    {
        P.ClosePanels();P.bAim=false;
        ALWWorldObject* Trader=LWV2Object(P,ELWObjectKind::Trader);
        if(!RequireV2(IsValid(Trader),TEXT("street trader is available for patrol check")))return;
        Trade->Trader=Trader;Trade->PatrolStart=Trader->GetActorLocation();
        LWV2Teleport(P,Trade->PatrolStart+Trader->GetActorForwardVector()*520+FVector(0,0,100));
        P.Controller->SetControlRotation((Trade->PatrolStart+FVector(0,0,95)-P.GetPawnViewLocation()).Rotation());
    },[this,Trade](ALWCharacter& P)
    {
        Check(Trade->Trader.IsValid()&&FVector::Dist(Trade->PatrolStart,Trade->Trader->GetActorLocation())>20,TEXT("trader moves along its street patrol during live play"));
        if(Trade->Trader.IsValid())P.Controller->SetControlRotation((Trade->Trader->GetActorLocation()+FVector(0,0,95)-P.GetPawnViewLocation()).Rotation());
        CaptureV2(TEXT("40A_TraderEncounter"));
    });
    Add(TEXT("generated trader purchase and stock transaction"),1,[this,Trade](ALWCharacter& P)
    {
        ALWWorldObject* Trader=LWV2Object(P,ELWObjectKind::Trader);
        if(!RequireV2(IsValid(Trader),TEXT("generated trader actor exists")))return;
        Trade->Trader=Trader;Trade->RecordId=Trader->RecordId;
        LWV2Teleport(P,Trader->GetActorLocation()+FVector(0,-220,100));Trader->Use(&P);
        auto* Stock=P.ItemsFor(2);
        if(!RequireV2(Stock!=nullptr&&!Stock->IsEmpty(),TEXT("nearby trader exposes generated stock")))return;
        int64 Best=MAX_int64;
        for(const auto& Item:*Stock)
        {
            const auto& D=LWItems::Def(Item.Definition);const int64 Cost=int64(D.Price)*Item.Count;
            if(Cost>0&&Cost<=P.Money&&Cost<Best&&D.Width<=2&&D.Height<=2){Best=Cost;Trade->ItemId=Item.Id;}
        }
        if(!RequireV2(Trade->ItemId.IsValid(),TEXT("starting credits can buy an actual small stock item")))return;
        Trade->Cost=Best;Trade->Money=P.Money;Trade->StockBefore=*Stock;Trade->InventoryBefore=P.Inventory;
        const auto InvBytes=LWV2Snapshot(P.Inventory),StockBytes=LWV2Snapshot(*Stock);
        P.Money=0;
        Check(!LWV2MoveAuto(P,2,0,Trade->ItemId),TEXT("unfunded trader purchase is rejected"));
        Check(P.Money==0&&InvBytes==LWV2Snapshot(P.Inventory)&&StockBytes==LWV2Snapshot(*Stock),TEXT("failed purchase preserves money inventory and stock"));
        P.Money=Trade->Money;
        Check(!P.MoveItem(2,0,Trade->ItemId,40,40,false),TEXT("purchase rejects invalid inventory placement"));
        Check(P.Money==Trade->Money&&InvBytes==LWV2Snapshot(P.Inventory)&&StockBytes==LWV2Snapshot(*Stock),TEXT("placement failure never charges the buyer"));
        if(!RequireV2(LWV2MoveAuto(P,2,0,Trade->ItemId),TEXT("trader buy uses real inventory transfer API")))return;
        Check(P.Money==Trade->Money-Trade->Cost,TEXT("successful buy debits exact price times stack count"));
        Stock=P.ItemsFor(2);
        Check(P.FindItem(Trade->ItemId)&&Stock&&!Stock->ContainsByPredicate([&](const auto& I){return I.Id==Trade->ItemId;}),TEXT("purchased identity moves from stock to player exactly once"));
        Check(Stock&&Stock->Num()==Trade->StockBefore.Num()-1&&P.Inventory.Num()==Trade->InventoryBefore.Num()+1,TEXT("trade transfers exactly one complete item stack"));
        const auto* Original=Trade->StockBefore.FindByPredicate([&](const auto& I){return I.Id==Trade->ItemId;});
        const auto* Bought=P.FindItem(Trade->ItemId);
        Check(Original&&Bought&&LWV2Snapshot({*Original},true)==LWV2Snapshot({*Bought},true),TEXT("purchase preserves payload including ammo and durability"));
        AuditV2(P,TEXT("trader purchase"));
    },[this](ALWCharacter&){CaptureV2(TEXT("41_Trader"));});

    struct FPersistenceCase
    {
        TArray<uint8> Inventory,Stash,Stock;
        TMap<FName,int64> Ammo;
        int64 Money=0;float Health=0;
    };
    auto Persistence=MakeShared<FPersistenceCase>();
    Add(TEXT("version two save round trip"),1,[this,Persistence,Trade](ALWCharacter& P)
    {
        P.ClosePanels();P.CancelReload();P.StopAttack();P.Health=87;
        Persistence->Inventory=LWV2Snapshot(P.Inventory);Persistence->Stash=LWV2Snapshot(P.Stash);
        Persistence->Ammo=LWV2Ammo(P);Persistence->Money=P.Money;Persistence->Health=P.Health;
        const auto* Stock=P.World->Containers.Find(Trade->RecordId);
        if(!RequireV2(Stock!=nullptr,TEXT("purchased trader record available for persistence")))return;
        Persistence->Stock=LWV2Snapshot(Stock->Items);P.SaveProgress();
        auto* Saved=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromSlot(LWV2SaveSlot,0));
        if(!RequireV2(IsValid(Saved),TEXT("automation save can be read from disk")))return;
        Check(Saved->Version==2,TEXT("new SaveProgress explicitly writes version two"));
        Check(LWV2Snapshot(Saved->Inventory)==Persistence->Inventory&&LWV2Snapshot(Saved->Stash)==Persistence->Stash,TEXT("disk save stores carried and secured inventories in full"));
        P.Money=987654;P.Health=31;P.Inventory.Empty();P.Stash.Empty();P.ActiveWeaponId.Invalidate();
        P.LoadProgress();
        Check(P.Money==Persistence->Money&&P.Health==Persistence->Health,TEXT("save restores exact credits and health"));
        Check(LWV2Snapshot(P.Inventory)==Persistence->Inventory&&LWV2Snapshot(P.Stash)==Persistence->Stash,TEXT("load restores all item identities placement and ammunition fields"));
        Check(LWV2SameAmmo(Persistence->Ammo,LWV2Ammo(P)),TEXT("save load conserves all loose magazine chamber and cylinder ammo"));
        Check(P.World->KilledZombies.Contains(LWV2EnemyId),TEXT("save restores defeated zombie identity"));
        const auto* RestoredStock=P.World->Containers.Find(Trade->RecordId);
        Check(RestoredStock&&LWV2Snapshot(RestoredStock->Items)==Persistence->Stock,TEXT("trader purchase stock persists across world reset"));
        AuditV2(P,TEXT("save round trip"));
    });

    Add(TEXT("distant negative-coordinate streaming and return"),1,[this,Persistence](ALWCharacter& P)
    {
        const FVector Far(-2560000,2560000,800);
        LWV2Teleport(P,Far);P.World->Stream(Far,true);
        const int32 Ring=2*FMath::Clamp(P.World->RenderRadius,1,4)+1;
        Check(P.World->Chunks.Num()<=Ring*Ring,TEXT("streaming evicts distant chunks within configured ring"));
        Check(P.World->Chunks.Contains(LWGen::ChunkAt(FVector2D(Far))),TEXT("distant negative-coordinate chunk created"));
        P.LoadProgress();
        Check(LWV2Snapshot(P.Inventory)==Persistence->Inventory&&LWV2Snapshot(P.Stash)==Persistence->Stash,TEXT("stream eviction and return cannot reset inventory"));
        Check(P.World->Chunks.Contains(LWGen::ChunkAt(FVector2D(P.GetActorLocation()))),TEXT("save return streams the restored player chunk"));
    },FLWV2Action(),FLWV2Ready(),45);

    Add(TEXT("legacy relief interaction and indoor detection"),1.2,[this](ALWCharacter& P)
    {
        ALWInteractable* Terminal=nullptr;double Best=TNumericLimits<double>::Max();
        for(TActorIterator<ALWInteractable> It(GetWorld());It;++It)
        {
            const double D=FVector::DistSquared(It->GetActorLocation(),FVector(1400,4200,0));
            if(D<Best){Best=D;Terminal=*It;}
        }
        if(!RequireV2(IsValid(Terminal),TEXT("origin relief terminal exists")))return;
        P.Health=30;P.Hunger=5;P.Thirst=4;Terminal->Use(&P);
        Check(P.Health==100&&P.Hunger==100&&P.Thirst==100,TEXT("relief interaction restores health hunger and thirst"));
        const auto AmmoBefore=LWV2Ammo(P);Terminal->Use(&P);
        Check(LWV2SameAmmo(AmmoBefore,LWV2Ammo(P)),TEXT("relief cooldown cannot manufacture inventory ammunition"));
        LWV2Teleport(P,Terminal->GetActorLocation()+FVector(270,-60,80));P.Controller->SetControlRotation(FRotator(-5,180,0));
    },[this](ALWCharacter& P)
    {Check(P.bIndoors,TEXT("roof detection enters indoor acoustic state"));CaptureV2(TEXT("42_Interior"));});

    // Save/load during an unfinished magazine reload must retain the last committed ownership.
    struct FInterruptedCase {TArray<uint8> Inventory;TMap<FName,int64> Ammo;};
    auto Interrupted=MakeShared<FInterruptedCase>();
    Add(TEXT("reload interruption persistence"),.55,[this,Interrupted](ALWCharacter& P)
    {
        P.ClosePanels();LWV2Teleport(P,FVector(2000,0,115));P.bSafehouse=false;
        const FGuid Rifle=LWV2FindId(P.Inventory,TEXT("rifle"));
        if(!RequireV2(Rifle.IsValid()&&P.MoveItem(0,0,Rifle,0,0,false,TEXT("Primary")),TEXT("rifle equipped for interrupted reload")))return;
        P.Equip(6);Interrupted->Ammo=LWV2Ammo(P);P.Reload();
        Check(P.bReloading,TEXT("reload starts before persistence interruption"));
    },[this,Interrupted](ALWCharacter& P)
    {
        Check(P.bReloading,TEXT("reload is still active at save interruption"));
        Interrupted->Inventory=LWV2Snapshot(P.Inventory);P.SaveProgress();P.LoadProgress();
        Check(!P.bReloading&&LWV2Snapshot(P.Inventory)==Interrupted->Inventory,TEXT("loading unfinished reload retains committed magazine ownership without replay"));
        Check(LWV2SameAmmo(Interrupted->Ammo,LWV2Ammo(P)),TEXT("interrupted reload save cannot create or discard rounds"));
        AuditV2(P,TEXT("interrupted reload save"));
    });

    struct FDeathCase
    {
        TArray<FLWItemInstance> Inventory;
        TArray<uint8> Stash;
        TMap<FName,int64> Ammo;
        FName Bag;
        int32 BagsBefore=0;
    };
    auto Death=MakeShared<FDeathCase>();
    Add(TEXT("player death drops every carried identity"),1,[this,Death](ALWCharacter& P)
    {
        P.ClosePanels();P.CancelReload();P.StopAttack();P.LeaveSafehouse();LWV2Teleport(P,FVector(2000,0,115));
        if(!RequireV2(!P.Inventory.IsEmpty()&&!P.Stash.IsEmpty()&&!P.bSafehouse&&P.CanAct(),TEXT("death fixture has carried gear secured gear and outside controls")))return;
        Death->Inventory=P.Inventory;Death->Stash=LWV2Snapshot(P.Stash);Death->Ammo=LWV2Ammo(P.Inventory);Death->BagsBefore=LWV2DroppedCount(*P.World);
        // The lethal hit legitimately wears equipped armor before HandleDeath packages the gear.
        for(auto& Item:Death->Inventory)if(Item.Slot==TEXT("Armor")&&Item.Durability>0)
        {Item.Durability=FMath::Max(0,Item.Durability-5000);break;}
        UGameplayStatics::ApplyDamage(&P,10000,nullptr,this,nullptr);Death->Bag=P.LastDeathBag;
        Check(P.Health==0&&P.bDeadSaved&&P.Inventory.IsEmpty()&&!P.ActiveWeaponId.IsValid(),TEXT("death clears all carried equipment and active weapon"));
        Check(Death->Stash==LWV2Snapshot(P.Stash),TEXT("death preserves secured stash in full"));
        const auto* Bag=P.World->Containers.Find(Death->Bag);
        if(!RequireV2(!Death->Bag.IsNone()&&Bag!=nullptr,TEXT("death creates persistent world loot bag")))return;
        Check(Bag->bDropped&&Bag->Items.Num()==Death->Inventory.Num(),TEXT("death bag receives every carried record including loaded magazines"));
        Check(LWV2Snapshot(Death->Inventory,true)==LWV2Snapshot(Bag->Items,true),TEXT("death drop preserves every item payload identity chamber cylinder and magazine link"));
        Check(LWV2SameAmmo(Death->Ammo,LWV2Ammo(Bag->Items)),TEXT("death conserves ammunition into world loot"));
        UGameplayStatics::ApplyDamage(&P,10000,nullptr,this,nullptr);P.HandleDeath();
        Check(P.LastDeathBag==Death->Bag&&LWV2DroppedCount(*P.World)==Death->BagsBefore+1,TEXT("repeated death cannot duplicate gear bags"));
        auto* Saved=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromSlot(LWV2SaveSlot,0));
        Check(IsValid(Saved)&&Saved->Version==2&&Saved->Health==0&&Saved->Inventory.IsEmpty()&&Saved->LastDeathBag==Death->Bag,TEXT("death state with empty inventory is committed to disk"));
        Check(IsValid(Saved)&&LWV2Snapshot(Saved->Stash)==Death->Stash,TEXT("death save preserves secured stash"));
    },[this](ALWCharacter&){CaptureV2(TEXT("43_Death"));});
    Add(TEXT("respawn without replacement gear"),1,[this,Death](ALWCharacter& P)
    {
        P.Respawn();
        Check(P.Health==100&&P.bSafehouse&&!P.bDeadSaved&&P.Inventory.IsEmpty(),TEXT("respawn is healthy in bunker with empty carried inventory"));
        Check(LWV2Snapshot(P.Stash)==Death->Stash,TEXT("respawn preserves stash and does not reinitialize it"));
        const auto* Bag=P.World->Containers.Find(Death->Bag);
        Check(Bag&&LWV2Snapshot(Bag->Items,true)==LWV2Snapshot(Death->Inventory,true),TEXT("respawn leaves all dropped gear in original bag"));
        Check(P.bWaypoint,TEXT("respawn points player toward recovery location"));
    },[this](ALWCharacter& P)
    {Check(!P.ActiveWeaponId.IsValid()&&!P.WeaponRoot->IsVisible(),TEXT("unarmed respawn has no resurrected active weapon"));CaptureV2(TEXT("44_Respawn"));});
    Add(TEXT("reload persisted respawn without gear resurrection"),1,[this,Death](ALWCharacter& P)
    {
        P.LoadProgress();
        Check(P.bSafehouse&&P.Health>0&&P.Inventory.IsEmpty()&&!P.ActiveWeaponId.IsValid(),TEXT("save reload cannot resurrect carried gear after death"));
        Check(LWV2Snapshot(P.Stash)==Death->Stash,TEXT("stash persists through death respawn and disk reload"));
        const auto* Bag=P.World->Containers.Find(Death->Bag);
        Check(Bag&&LWV2Snapshot(Bag->Items,true)==LWV2Snapshot(Death->Inventory,true),TEXT("dropped gear persists through disk reload without loss or duplication"));
        Check(Bag&&LWV2SameAmmo(Death->Ammo,LWV2Ammo(Bag->Items)),TEXT("persisted death bag preserves complete ammunition ledger"));
        Check(LWV2DroppedCount(*P.World)==Death->BagsBefore+1,TEXT("reload retains exactly one new death bag"));
        AuditV2(P,TEXT("respawn disk reload"));P.ToggleInventory();
    },[this](ALWCharacter&){CaptureV2(TEXT("45_EmptyRespawnInventory"));});

    Add(TEXT("final survival soak and screenshot verification"),2,[](ALWCharacter& P){P.ClosePanels();},
    [this,Ragdoll,Death](ALWCharacter& P)
    {
        Check(P.bSafehouse&&P.Health==100&&P.Hunger==100&&P.Thirst==100,TEXT("respawn bunker remains safe during extended live soak"));
        Check(P.Inventory.IsEmpty()&&LWV2Snapshot(P.Stash)==Death->Stash,TEXT("empty respawn and secured stash remain stable across live ticks"));
        Check(!Ragdoll->Corpse.IsValid(),TEXT("ragdoll actor and owned joints expire after corpse lifespan"));
        if(V2->bAudioSmoke&&!FParse::Param(FCommandLine::Get(),TEXT("NoSound"))){
            Check(IsValid(P.World->Wind.Get())&&P.World->Wind->IsPlaying(),TEXT("wind loop survives mute and the complete gameplay session"));
            Check(IsValid(P.World->Drone.Get())&&P.World->Drone->IsPlaying(),TEXT("drone loop survives mute and the complete gameplay session"));
            Check(IsValid(P.World->Music.Get())&&P.World->Music->IsPlaying(),TEXT("music loop survives state transitions and mute"));
        }
        Check(V2->Checks>=55,TEXT("V2 includes at least the original 55 checks"));
        for(const FString& Filename:V2->Screenshots)
            Check(IFileManager::Get().FileSize(*Filename)>0,*FString::Printf(TEXT("screenshot written: %s"),*FPaths::GetCleanFilename(Filename)));
        UE_LOG(LogTemp,Display,TEXT("LW_V2_NOTE loot determinism remains in separate LethalWorld.Loot automation tests"));
    },[this,Ragdoll](ALWCharacter&){return FPlatformTime::Seconds()-V2->StartedAt>=95&&!Ragdoll->Corpse.IsValid();},120);
}

void ALWGameMode::BuildV3Smoke(ALWCharacter& InitialPlayer)
{
    V2->OriginalSensitivity=InitialPlayer.Sensitivity;V2->OriginalVolume=InitialPlayer.MasterVolume;V2->bOriginalCrust=InitialPlayer.bCrust;
    V2->OriginalPendingResolution=InitialPlayer.PendingResolution;V2->OriginalPendingWindow=InitialPlayer.PendingWindowMode;V2->bSettingsSaved=true;
    auto Add=[this](const TCHAR* Name,double Wait,FLWV2Action Begin,FLWV2Action End=FLWV2Action()){
        V2->Steps.Add({Name,Wait,25,MoveTemp(Begin),MoveTemp(End),FLWV2Ready()});};
    Add(TEXT("v3 world and asset load"),3,[this](ALWCharacter& P){P.NewGame();},[this](ALWCharacter& P){
        for(const auto& Pair:P.World->Meshes)Check(IsValid(Pair.Value),*FString::Printf(TEXT("mesh %s exists"),*Pair.Key.ToString()));
        for(FName Name:ULWAudioCatalog::GetDefaultSlotNames())Check(IsValid(P.World->Sounds.FindRef(Name).Get()),*FString::Printf(TEXT("sound %s exists"),*Name.ToString()));
        TSet<int32> Kinds;for(int I=0;I<4;I++){auto* Enemy=GetWorld()->SpawnActor<ALWZombie>(FVector(5000+I*200,1000,500),FRotator::ZeroRotator);if(Enemy){Enemy->ConfigureKind(static_cast<ELWEnemyKind>(I));Kinds.Add(int32(Enemy->Kind));Check(Enemy->Parts.Num()==7&&IsValid(Enemy->Parts[0]->GetStaticMesh()),TEXT("enemy kind has loaded body meshes"));Enemy->Destroy();}}Check(Kinds.Num()==4,TEXT("all four enemy kinds can spawn independently of local density"));
    });
    TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Gather(FVector2D::ZeroVector,InitialPlayer.World->Seed,Roads,Sites);
    for(int Type=0;Type<5;Type++){
        const LWGen::FSite* Found=nullptr;for(const auto& S:Sites)if(S.Type==Type&&(!Found||S.Position.SizeSquared()<Found->Position.SizeSquared()))Found=&S;
        if(!Found){Check(false,TEXT("all POI types available"));continue;}const auto S=*Found;
        Add(TEXT("POI exterior"),2,[S](ALWCharacter& P){
            P.ClosePanels();FVector At=FVector(S.Position,115)+FRotator(0,S.Yaw,0).RotateVector(FVector(S.Type==0?1000:350,-S.Size.Y*.5-1750,0));
            P.World->Stream(At,true);P.SetActorLocation(At,false,nullptr,ETeleportType::TeleportPhysics);P.Controller->SetControlRotation((FVector(S.Position,280)-At).Rotation());
        },[this,S](ALWCharacter& P){CaptureV2(*FString::Printf(TEXT("POI_%d_Exterior"),S.Type));
            int Doors=0,Windows=0,Signs=0;for(TActorIterator<ALWWorldObject> I(GetWorld());I;++I)if(FVector2D::Distance(FVector2D(I->GetActorLocation()),S.Position)<S.Size.X){Doors+=I->Kind==ELWObjectKind::Door;Windows+=I->Kind==ELWObjectKind::Window;Signs+=I->Kind==ELWObjectKind::Sign;}
            Check(Doors>=2&&Windows>=2&&Signs>=1,TEXT("POI has interactive doors windows and mounted sign"));
        });
        Add(TEXT("POI interior"),2,[S](ALWCharacter& P){
            FVector At=FVector(S.Position,125)+FRotator(0,S.Yaw,0).RotateVector(FVector(-S.Size.X*.5+170,-S.Size.Y*.5+170,0));P.SetActorLocation(At,false,nullptr,ETeleportType::TeleportPhysics);P.Controller->SetControlRotation(FRotator(-5,S.Yaw+35,0));
            for(TActorIterator<ALWWorldObject> I(P.GetWorld());I;++I)if(I->Kind==ELWObjectKind::Door&&FVector2D::Distance(FVector2D(I->GetActorLocation()),S.Position)<S.Size.X&&!I->bChanged)I->Use(&P);
        },[this,S](ALWCharacter& P){CaptureV2(*FString::Printf(TEXT("POI_%d_Interior"),S.Type));});
    }
    Add(TEXT("prop combat persistence and vehicle inventory"),1,[this](ALWCharacter& P){
        P.World->Stream(FVector(2000,0,110),true);P.SetActorLocation(FVector(2000,0,110));P.Controller->SetControlRotation(FRotator::ZeroRotator);
        auto* Door=P.World->SpawnObject(ELWObjectKind::Door,TEXT("test_door"),FVector(2200,-80,25));Door->Use(&P);Door->Tick(1);
        Check(Door->bChanged&&Door->DoorAngle>90,TEXT("door swings about its hinge"));Door->Destroy();
        Door=P.World->SpawnObject(ELWObjectKind::Door,TEXT("test_door"),FVector(2200,-80,25));Check(Door->bChanged,TEXT("door state survives actor recreation"));Door->Destroy();
        auto* Glass=P.World->SpawnObject(ELWObjectKind::Window,TEXT("test_glass"),FVector(2300,0,120));Glass->SetActorScale3D(FVector(.04,3,2));
        FHitResult Hit;FCollisionQueryParams Q(NAME_None,false,&P);
        Check(GetWorld()->LineTraceSingleByChannel(Hit,FVector(2200,0,120),FVector(2400,0,120),ECC_Visibility,Q)&&Hit.GetActor()==Glass,TEXT("intact pane blocks bullets"));
        UGameplayStatics::ApplyDamage(Glass,20,nullptr,&P,nullptr);
        Check(Glass->bChanged&&!Glass->Body->IsCollisionEnabled(),TEXT("damage breaks pane and removes collision"));Glass->Destroy();
        Glass=P.World->SpawnObject(ELWObjectKind::Window,TEXT("test_glass"),FVector(2300,0,120));Check(!Glass->Body->IsVisible(),TEXT("broken window survives recreation"));Glass->Destroy();
        auto* Car=P.World->SpawnObject(ELWObjectKind::Car,TEXT("test_car"),FVector(2150,300,12));Cast<ALWVehicle>(Car)->Record()->Unlocked=true;P.ObjectFocus=Car;P.VehicleGlove();
        Check(P.OpenObject&&P.OpenObject->RecordId==Car->RecordId&&P.ItemsFor(2)!=nullptr,TEXT("vehicle cargo opens grid inventory"));P.ClosePanels();
        auto& Items=P.World->Containers.FindChecked(TEXT("test_car")).Items;Items.Empty();Car->Destroy();
        Car=P.World->SpawnObject(ELWObjectKind::Car,TEXT("test_car"),FVector(2150,300,12));Check(P.World->Containers.FindChecked(TEXT("test_car")).Items.IsEmpty(),TEXT("vehicle loot does not reroll"));Car->Destroy();
        P.SaveProgress();P.LoadProgress();Check(P.World->PropStates.FindRef(TEXT("test_glass"))==1,TEXT("destroyed prop state survives save load"));
    });
    Add(TEXT("pump chain explosion"),2,[this](ALWCharacter& P){
        P.SetActorLocation(FVector(2000,0,110));P.Controller->SetControlRotation(FRotator::ZeroRotator);
        float Ground=FMath::Max(P.World->HeightAt(FVector2D(3700,-200)),P.World->HeightAt(FVector2D(3700,200)))+30;auto* A=P.World->SpawnObject(ELWObjectKind::FuelPump,TEXT("test_pump_a"),FVector(3700,-200,Ground));
        auto* B=P.World->SpawnObject(ELWObjectKind::FuelPump,TEXT("test_pump_b"),FVector(3700,200,Ground));
        UGameplayStatics::ApplyDamage(A,30,nullptr,&P,nullptr);Check(A->bChanged&&B->bChanged,TEXT("shooting pump ignites adjacent pump"));
        Check(UGameplayStatics::ApplyDamage(A,30,nullptr,&P,nullptr)==0,TEXT("spent pump cannot explode twice"));CaptureV2(TEXT("PumpExplosion"));
    });
    Add(TEXT("live shotgun versus fuel pump"),1,[this](ALWCharacter& P){
        P.Health=1000;P.SetActorLocation(FVector(2000,0,110));P.Controller->SetControlRotation(FRotator(-5,0,0));P.Equip(2);P.bAim=true;
        P.World->SpawnObject(ELWObjectKind::FuelPump,TEXT("test_live_pump"),FVector(2450,0,20));
    },[this](ALWCharacter& P){
        P.Attack();P.StopAttack();Check(P.World->PropStates.FindRef(TEXT("test_live_pump"))==1,TEXT("player shotgun actually detonates pump"));Check(P.Health>900&&P.Health<1000,TEXT("own pump explosion damages shooter with distance falloff"));P.bAim=false;P.Health=100;
        CaptureV2(TEXT("LiveShotgunExplosion"));
    });
    for(int K=1;K<=3;K++){
        Add(TEXT("new enemy presentation"),2,[this,K](ALWCharacter& P){
            for(TActorIterator<ALWWorldObject> I(GetWorld());I;++I)if(I->RecordId==TEXT("test_live_pump"))I->Destroy();
            if(TestEnemy)TestEnemy->Destroy();P.ClosePanels();P.Health=100;P.SetActorLocation(FVector(2000,0,110));P.Controller->SetControlRotation(FRotator::ZeroRotator);
            TestEnemy=GetWorld()->SpawnActor<ALWZombie>(FVector(2550,0,K==2?70:110),FRotator(0,180,0));
            TestEnemy->PersistentId=0xf0300000+K;TestEnemy->ConfigureKind(static_cast<ELWEnemyKind>(K));TestEnemy->Home=TestEnemy->GetActorLocation();P.World->ZombieCount++;
            for(UStaticMeshComponent* Part:TestEnemy->Parts)Check(IsValid(Part->GetStaticMesh()),TEXT("enemy part mesh loaded"));
            if(K==3){Check(TestEnemy->IsObserved(&P),TEXT("mannequin detects player gaze"));Check(!TestEnemy->bAggressive,TEXT("distant mannequin initially passive"));}
        },[this,K](ALWCharacter& P){if(K==1)Check(TestEnemy->RaiderRounds<12,TEXT("raider fires live burst"));if(K==2)Check(P.Health<100,TEXT("dog bites player"));CaptureV2(*FString::Printf(TEXT("Enemy_%d"),K));});
        if(K==3){
            Add(TEXT("mannequin watched freeze"),1,[this](ALWCharacter& P){TestEnemy->Home=TestEnemy->GetActorLocation();},[this](ALWCharacter& P){Check(FVector::Dist2D(TestEnemy->Home,TestEnemy->GetActorLocation())<1,TEXT("watched mannequin stays still"));P.Controller->SetControlRotation(FRotator(0,180,0));});
            Add(TEXT("mannequin unobserved approach"),1,[](ALWCharacter& P){},[this](ALWCharacter& P){Check(!TestEnemy->IsObserved(&P)&&FVector::Dist2D(TestEnemy->Home,TestEnemy->GetActorLocation())>40,TEXT("unobserved mannequin advances"));UGameplayStatics::ApplyDamage(TestEnemy,1,nullptr,&P,nullptr);Check(TestEnemy->bAggressive,TEXT("attacked mannequin becomes hostile"));P.Controller->SetControlRotation(FRotator::ZeroRotator);});
        }
        Add(TEXT("new enemy ragdoll"),2,[this](ALWCharacter& P){UGameplayStatics::ApplyDamage(TestEnemy,1000,nullptr,&P,nullptr);},[this,K](ALWCharacter& P){
            Check(TestEnemy->bDead&&TestEnemy->RagdollJoints.Num()==6,TEXT("new enemy uses connected ragdoll"));for(UStaticMeshComponent* Part:TestEnemy->Parts)Check(Part->IsSimulatingPhysics(),TEXT("dead enemy part simulates physics"));CaptureV2(*FString::Printf(TEXT("Enemy_%d_Ragdoll"),K));
        });
    }
    Add(TEXT("capture flush"),2,[](ALWCharacter& P){});
}


#include "LWCreatorSmoke35.inl"
#include "LWTradingCards36Smoke.inl"

#include "LWRecovery37Smoke.inl"
#include "LWRevision38Smoke.inl"

#include "LWWorkbench39Smoke.inl"

#include "LWGameplay40Smoke.inl"

#include "LWVehicle42Smoke.inl"
#include "LWSave43Smoke.inl"
#include "LWNPCAudio44Smoke.inl"
#include "LWBunker45Smoke.inl"
#include "LWUI46Smoke.inl"

#include "LWConsole47Smoke.inl"

#include "LWBoss48Smoke.inl"

#include "LWVehicle49Smoke.inl"
