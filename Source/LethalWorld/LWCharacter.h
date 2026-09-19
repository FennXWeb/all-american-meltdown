#pragma once
#include "CoreMinimal.h"
#include "Async/Future.h"
#include "GameFramework/Character.h"
#include "LWInventory.h"
#include "LWRPG.h"
#include "LWOpening.h"
#include "LWCardGame.h"
#include "LWMissionRecovery37.h"
#include "LWBunker45State.h"
#include "LWCharacter.generated.h"
UCLASS()
class LETHALWORLD_API ALWCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    UPROPERTY() FLWBunker45State Bunker45;
    UPROPERTY() TObjectPtr<class ALWBunker45> BunkerManager45;
    bool BaseUI45=false,BuildMode45=false;
    UPROPERTY() TObjectPtr<class USceneComponent> LeanPivot40;
    bool LeanLeft40=false,LeanRight40=false,SaveDirty40=false;
    float LeanAmount40=0,SaveQuiet40=0,SaveAge40=0;int32 AutoSaves40=0;
    TFuture<bool> SaveWrite40;
    UPROPERTY() TArray<TObjectPtr<class ULWSaveGame>> CheckpointSnapshots43;
    TArray<TSharedPtr<FLWCheckpointJob43>> CheckpointJobs43;
    void FinishCheckpoints43(bool Wait=false);
    // Immutable value-only snapshot; keep alive until background serialization completes.
    UPROPERTY() TObjectPtr<class ULWSaveGame> SaveSnapshot43;
    bool SaveUrgent43=false;
    double LastSaveCaptureMs43=0;
    void StartSave43();bool FinishSave43();
    FString SaveSlot40()const;void RequestSave40();void TickSave40(float Dt);void DrainSave40();
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    void TickLean40(float Dt);void LeanLeftStart40();void LeanLeftStop40();void LeanRightStart40();void LeanRightStop40();void QuickHeal40();
    UPROPERTY() TObjectPtr<class ALWWorkbench39> Workbench39;
    UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> CustomMeshes39;
    void OpenWorkbench39(class ALWWorldObject* Station);void CloseWorkbench39();
    void ConfigureCustom39();void UpdateCustom39();
    UPROPERTY() TMap<FName,FLWMissionRecovery37> MissionRecovery37;
    bool bRestoringMission37=false;
    FName ActiveMission37()const;
    bool HasMissionRecovery37()const;
    void CaptureMission37(FName Key=NAME_None,bool Force=false);
    bool RecoverMission37(int32 Choice);
    class ULWSaveGame* MakeProgressSnapshot37();
    void ApplyProgressSnapshot37(class ULWSaveGame* Save);
    void TogglePlayerMenu();void SwitchTab(int32 Tab);void EquipSlot(FName Slot);bool QuickTransferItem(int32 From,FGuid Id);bool SortInventory(int32 Panel=0);
    UPROPERTY() TObjectPtr<class ALWVehicle> Vehicle;
    UPROPERTY() TObjectPtr<class ALWWorldObject> SecurityTarget;
    int32 SecurityMode=0,SecurityTier=0,WireStage=0;
    FGuid SecurityPick;
    float PickAngle=0,LockTurn=0,PickHealth=100,LockSecret=0,WireHeat=0,WireProgress=0,WireClock=0;
    float SeatYaw=0,SeatPitch=0;
    TArray<int32> WireOrder;
    bool HasKey(FGuid VIN)const;bool IsLocked(class ALWWorldObject* O)const;
    bool StartLockpick(class ALWWorldObject* O);bool StartHotwire();void TickSecurity(float Dt);void CancelSecurity();void FinishSecurity();void ChooseWire(int32 Index);
    void TickVehicleSeat();void VehicleGlove();void VehicleLights();void VehicleRadio();void VehicleWipers();void VehicleLeft();void VehicleRight();
    UPROPERTY() FLWRPGState RPG;
    UPROPERTY() TObjectPtr<class ALWStoryDirector> Story;
    bool bStoryLocked=false;
    UPROPERTY() TObjectPtr<class ALWResident> ResidentFocus;
    UPROPERTY() TObjectPtr<class ALWEncounterScene> EncounterSpeaker;
    UPROPERTY() FLWCardGame Cards;
    UPROPERTY() TObjectPtr<ALWCardTable> CardTable;
    int32 CollectionPage36=0,CollectionSelection36=0;bool CollectionOwned36=false;int32 CollectionRarity36=-1;
    int32 CardPage=0;bool CardRules=false;
    double CardBusyUntil=0;
    void CardAction(FName Action,int32 Value=0);
    bool EncounterJournal=false;bool ChapterJournal=true;
    UPROPERTY() TObjectPtr<class ALWResident> Speaker;
    int32 RPGPanel=0,SkillCategory=0,SkillPage=0,JournalPage=0;
    FName DialogueNode=TEXT("root");
    FString DialogueText;
    TArray<FString> DialogueChoices;
    TArray<FName> DialogueActions;
    float RPGClock=0;
    float Stat(FName Effect)const{return LWRPG::Stat(RPG,Effect);}
    float MaxHealth()const{return 100+Stat(TEXT("health"));}
    float MaxStamina()const{return 100+Stat(TEXT("stamina"));}
    int32 CompanionLimit()const{return FMath::Clamp(1+int32(Stat(TEXT("companions"))),1,10);}
    void GainXP(int32 Amount);bool BuyPerk(FName Id);bool RaiseAttribute(int32 Category);
    void ToggleSkills();void ToggleJournal();void ToggleCrew();
    void TickRPG(float Dt);void QuestEvent(FName Event,FName Target=NAME_None,int32 Count=1);
    bool AcceptQuest(FName Id);bool TurnInQuest(FName Id);bool TrackQuest(FName Id);
    int32 CountSupply(FName Item)const;bool ConsumeSupply(FName Item,int32 Count);
    void Talk(class ALWResident* NPC);void BuildDialogue(FName Node);void ChooseDialogue(int32 Choice);
    bool Recruit(class ALWResident* NPC);bool SetCompanion(FName Id,bool Following);
    float CombatMultiplier(bool Headshot=false)const;
    // Traversal is collision driven; these transient states never enter saved inventories.
    bool Prone54=false,CrouchHeld54=false,Sliding54=false,Traversing54=false,WasGrounded54=true;
    float CrouchHold56=0;bool CrouchHoldUsed56=false;
    float SlideTime54=0,SlideCooldown54=0,TraversalTime54=0,TraversalDuration54=.65f;
    float GroundGrace54=0,JumpBuffer54=0,CameraOffset54=0,Landing54=0,LastFallSpeed54=0;
    float Handling54=0,RecoilBloom54=0,Rumble54=0,RumbleTime54=0,InputForward54=0,InputRight54=0;
    FVector SlideDirection54=FVector::ForwardVector,TraversalStart54,TraversalLift54,TraversalEnd54;
    FRotator Sway54=FRotator::ZeroRotator;
    virtual void OnStartCrouch(float HeightAdjust,float ScaledHeightAdjust) override;
    virtual void OnEndCrouch(float HeightAdjust,float ScaledHeightAdjust) override;
    void ToggleProne54(); bool Stand54(); bool TryTraverse54(); void EndTraverse54();
    void TickMovement54(float Dt); float Spread54()const;
    void WeaponMotion54(FVector& Position,FRotator& Rotation,float Dt);
    void GroundImpact54(FVector Where,float Strength,float Radius);
    class ALWWorldObject* QuickContainer54()const;
    int QuickIndex54=0;FName QuickRecord54;
    void QuickLootStep54(int Direction); void QuickLootOpen54(); void QuickLootAll54();
    int TakeAll54(); bool TakeQuick54(bool All=false);
    ALWCharacter();
    virtual void BeginPlay() override;
    virtual void Tick(float Dt) override;
    virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
    virtual float TakeDamage(float Damage,const FDamageEvent& Event,AController* Instigator,AActor* Causer) override;
    UPROPERTY(VisibleAnywhere) TObjectPtr<class UCameraComponent> Camera;
    UPROPERTY() TObjectPtr<class USceneComponent> WeaponRoot;
    UPROPERTY() TObjectPtr<class UStaticMeshComponent> WeaponMesh;
    UPROPERTY() TObjectPtr<class UStaticMeshComponent> Arms;
    UPROPERTY() TObjectPtr<class USpotLightComponent> Flashlight;
    UPROPERTY() TObjectPtr<class UPointLightComponent> Muzzle;
    UPROPERTY() TObjectPtr<class UMaterialInstanceDynamic> Filter;
    UPROPERTY() TObjectPtr<class ALWWorld> World;
    UPROPERTY() TObjectPtr<class ALWInteractable> Focus;
    UPROPERTY() TObjectPtr<class ALWWorldObject> ObjectFocus;
    UPROPERTY() TObjectPtr<class ALWWorldObject> OpenObject;
    UPROPERTY() TArray<FLWItemInstance> Inventory;
    UPROPERTY() TArray<FLWItemInstance> Stash;
    UPROPERTY() TObjectPtr<class UStaticMeshComponent> MovingPart;
    UPROPERTY() TObjectPtr<class UStaticMeshComponent> SecondPart;
    UPROPERTY() TObjectPtr<class UStaticMeshComponent> LoadingHand;
    FGuid ActiveWeaponId,ReloadMagazineId;
    bool bInventory=false,bSafehouse=false,bTrigger=false,bDeadSaved=false,bWaypoint=false,bRouteComplete=true;
    FVector2D Waypoint=FVector2D::ZeroVector;
    TArray<FVector2D> Route;
    TArray<FVector> RouteGround;
    FVector2D LastRouteFrom=FVector2D::ZeroVector;
    FName LastDeathBag;
    float RouteTimer=0,ReloadElapsed=0,ReloadDuration=0,VideoConfirmTimer=0;
    int32 ReloadPhase=0,FireMode=0;
    FIntPoint PendingResolution=FIntPoint(1600,900);
    int32 PendingWindowMode=2;
    bool bVideoConfirm=false,bLoadingSave=false;
    float Health=100,Stamina=100,Hunger=100,Thirst=100;
    FString DungeonStatus;
    int64 Money=150;
    int32 Weapon=0,Shells=6,ReserveShells=24,Kills=0;
    bool bStarted=false,bMenu=true,bMap=false,bSettings=false,bIndoors=false,bSprint=false,bAim=false,bReloading=false,bCrust=true;
    float AttackTimer=0,AttackDuration=0,ReloadTimer=0,HurtFlash=0,HitMarker=0,MessageTime=0;
    float Sensitivity=.3f,MasterVolume=.8f;
    FString Message;
    float StepClock=0,AutoSaveClock=0,RoofClock=0,LookX=0,LookY=0;
    UPROPERTY() TObjectPtr<class ALWWorldObject> SittingChair;
    FVector SitReturn;
    void SitOnChair(class ALWWorldObject* Chair);void StandFromChair(bool Force=false);bool SleepInBed(float Hours=4);
    bool bUIInputActive=false,bUIAttackHeld=false;
    uint64 LastUIAttackFrame=MAX_uint64;
    bool bConsole47=false,bGod47=false;
    bool IsUIOpen() const {return bConsole47|| BaseUI45||BuildMode45||Workbench39||bStoryLocked||bUIInputActive||bMenu||bSettings||bVideoConfirm||bInventory||bMap||RPGPanel||SecurityMode||OpeningMode||bWorldSetup||Speaker||EncounterSpeaker||CardTable;}
    float BudgetBrace50=0;
    bool Recoiling50=false;
    void StartRecoil50(FVector Impulse);bool TickRecoil50(float Dt);
    bool CanAct() const {return !Traversing54&&!Recoiling50&&bStarted&&!IsUIOpen()&&!Vehicle&&!SittingChair&&Health>0;}
    void ConsumeUIAttack();void ReleaseAttackInput();
    void Notify(FString Text,float Seconds=3);
    void StartGame(bool Continue=true);
    void NewGame();
    UPROPERTY() FLWIdentity Identity;
 UPROPERTY() TObjectPtr<class USceneComponent> SurvivorRoot;
 UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> SurvivorParts;
 void BuildSurvivorBody();void TickSurvivorBody(float Dt);
    UPROPERTY() FLWIdentity DraftIdentity;
    UPROPERTY() TObjectPtr<ALWOpeningScene> OpeningScene;
    int32 OpeningMode=0,OpeningShot=0;
    double OpeningSince=0;
    float PortraitYaw=0;
    int32 CreatorTab35=0,CreatorPage35=0,TattooZone35=0;
    float CreatorZoom35=0;
    bool CreatorDirty35=false;double CreatorPreviewTime35=0;int CreatorPreset35=1;
    bool OpeningPaused=false,NameEditing=false;
    TArray<int32> StartingAttributes={4,4,4,4,4,4,4};
    FString CreationMessage;
    void BeginOpening();void UpdateOpening();void OpeningClick(FName Name);void EndOpening(bool Commit);
    void ApplyIdentity();void BindIdentityInput(UInputComponent* Input);

    bool bWorldSetup=false,bSeedEdit=false;
    FString SeedText=TEXT("198706");
    int32 SetupTowns=1,SetupPOIs=1,SetupTerrain=1,SetupDifficulty=1;
    void BeginWorldSetup();void WorldSetupClick(FName Name);void BindSeedInput(class UInputComponent* Input);
    void ToggleMenu();
    void ToggleMap();
    void ToggleSettings();
    void ToggleCrust();
    void CycleVolume();
    void CycleSensitivity();
    void LowerSensitivity();
    void CycleResolution(); void CycleWindowMode(); void ApplyVideo(); void ConfirmVideo(); void RevertVideo();
    FString WindowModeName() const;
    void ApplySettings();
    void Forward(float V); void Right(float V); void Turn(float V); void Look(float V);
    void StartSprint(); void StopSprint(); void StartCrouch(); void StopCrouch();
    void StartAim(); void StopAim(); void DoJump();
    void Attack(); void Reload(); void Interact(); void ToggleFlashlight();
    bool DiscoverPlace(uint32 Id,FVector Entrance,const FString& Name); void TickDiscovery(float Dt); bool FastTravel();
    float DiscoveryClock=0,DiscoveryAlert=0; FString DiscoveryTitle; double LastCombatTime=-1000;
    void ToggleNightVision(); void TickNightVision(); bool bNightVision=false;
    UPROPERTY() TObjectPtr<class USpotLightComponent> NightVisionLight;
    void StopAttack();void ToggleFireMode();
    void EquipRevolver();void EquipSniper();void EquipSMG();void EquipRifle();void EquipLMG();
    void Equip(int32 Index); void EquipCrowbar(); void EquipBat(); void EquipShotgun(); void NextWeapon(); void PreviousWeapon();
    bool HasSavedGame() const;
    void Reward(int32 Amount); void SaveProgress(); void LoadProgress();
    FString WeaponName() const;
    void SetMenuInput(bool Enabled);
    double WorldHour()const;
    FLWSettlementRecord& EnsureSettlement(FIntPoint Region);
    FString SettlerName(FName Town,FName Resident);
    FName NearestSettlement(FVector Position)const;
    bool SettlementHostile(FName Town)const;
    void ChangeReputation(FName Town,int Delta,bool Aggro=false);
    void RecordSettlementTransfer(int From,int To,int Value,bool Trade);
    void TickSettlements();
    bool BuildSettlementDialogue(FName Node);
    bool SettlementAction(FName Action);
    bool ConsignItem(FName Town,FGuid Item);
    void SetBedSpawn(ALWWorldObject* Bed=nullptr);
    bool RestoreRespawn();
    void OpenBedMenu(ALWWorldObject* Bed);
    UPROPERTY() TObjectPtr<ALWWorldObject> RestBed;
    int32 SettlementPage=0;

    void ToggleInventory(); void ClosePanels(); void OpenContainer(class ALWWorldObject* Object);
    void EnterSafehouse();void LeaveSafehouse();void Respawn();void HandleDeath();
    void InitializeInventory();void InitializeStash();
    FLWItemInstance* ActiveGun(); const FLWItemInstance* ActiveGun()const;
    FLWItemInstance* FindItem(FGuid Id);const FLWItemInstance* FindItem(FGuid Id)const;
    TArray<FLWItemInstance>* ItemsFor(int32 Panel);
    bool MoveItem(int32 From,int32 To,FGuid Id,int32 X,int32 Y,bool Rotate,FName EquipSlot=NAME_None);
    bool UseItem(FGuid Id);bool EquipItem(FGuid Id);bool UnloadItem(FGuid Id);bool DropItem(FGuid Id);
    bool LoadAmmoOnto(int32 From,FGuid AmmoId,int32 To,FGuid TargetId);
    int32 AmmoCount(FName Ammo)const;int32 TakeAmmo(FName Ammo,int32 Count);
    bool GiveItem(FName Definition,int32 Count=1);void SyncAmmoHUD();
    void UpdateWeapon(float Dt);void UpdateReload(float Dt);void CancelReload();void ChamberRound();
    void ConfigureAttachments();void TickAttachments();
    UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> AttachmentParts;
    UPROPERTY() TObjectPtr<class USpotLightComponent> GunLight;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> LaserDot;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> LaserBeam;
    bool PunchLeft=false;void Punch();
    void GunBash();void RefillCanisters(class ALWWorldObject* Pump);void MigrateDirectAmmo();float GunBashTimer=0;
    UPROPERTY() TObjectPtr<class ALWZombie> CombatTarget;float CombatTargetTime=0;
    UPROPERTY() TObjectPtr<class ALWZombie> CorpseFocus;void TickUnarmed(float Dt);
    void ConfigureWeaponParts();void PersistWorldChange();
    void SetWaypoint(FVector2D P);void ClearWaypoint();void RouteToBunker();void TickRoute(float Dt);
    FString AmmoStatus()const;FString FocusPrompt()const;
};
