#include "LWSettlement82.h"
#include "LWCampaign76.h"
#include "LWHUD.h"
#include "Engine/GameViewportClient.h"
#include "Framework/Application/SlateApplication.h"
#include "LWNewGame72.h"
#include "LWDebugMenu67.h"
#include "LWConsole47.h"
#include "LWBunker45.h"
#include "LWLoading45.h"
#include "LWGraphics33.h"
#include "LWLighting.h"
#include "LWWeaponMods.h"
#include "LWVehicle.h"
#include "LWResident.h"
#include "InputCoreTypes.h"
#include "LWCharacter.h"
#include "LWStory.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "LWWorld.h"
#include "LWInteractable.h"
#include "LWWorldObject.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "AudioDevice.h"
#include "Misc/ConfigCacheIni.h"

ALWCharacter::ALWCharacter()
{
    PrimaryActorTick.bCanEverTick=true;PrimaryActorTick.bTickEvenWhenPaused=true;
    GetCapsuleComponent()->InitCapsuleSize(32,88);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
    GetCharacterMovement()->MaxWalkSpeed=340;GetCharacterMovement()->JumpZVelocity=420;
    GetCharacterMovement()->AirControl=.25f;GetCharacterMovement()->BrakingDecelerationWalking=2000;
    GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch=true;GetCharacterMovement()->SetCrouchedHalfHeight(52);
    LeanPivot40=CreateDefaultSubobject<USceneComponent>(TEXT("LeanPivot"));LeanPivot40->SetupAttachment(RootComponent);
    Camera=CreateDefaultSubobject<UCameraComponent>(TEXT("Eyes"));Camera->SetupAttachment(LeanPivot40);
    Camera->SetRelativeLocation(FVector(0,0,66));Camera->bUsePawnControlRotation=true;Camera->FieldOfView=86;
    WeaponRoot=CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRig"));WeaponRoot->SetupAttachment(Camera);
    auto ViewMesh=[&](const TCHAR* Name){auto* M=CreateDefaultSubobject<UStaticMeshComponent>(Name);M->SetupAttachment(WeaponRoot);M->SetCollisionEnabled(ECollisionEnabled::NoCollision);M->CastShadow=false;return M;};
    WeaponMesh=ViewMesh(TEXT("Weapon"));Arms=ViewMesh(TEXT("Arms"));MovingPart=ViewMesh(TEXT("Mechanism"));SecondPart=ViewMesh(TEXT("Feed"));LoadingHand=ViewMesh(TEXT("LoadingHand"));
    Flashlight=CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));Flashlight->SetupAttachment(Camera);
    Flashlight->SetRelativeLocation(FVector(10,8,-6));Flashlight->SetIntensity(180000);Flashlight->SetAttenuationRadius(14000);
    Flashlight->SetInnerConeAngle(12);Flashlight->SetOuterConeAngle(29);Flashlight->SetLightColor(FLinearColor(.92,.96,.75));
    NightVisionLight=CreateDefaultSubobject<USpotLightComponent>(TEXT("NightVisionIlluminator"));NightVisionLight->SetupAttachment(Camera);
    NightVisionLight->SetIntensity(14000);NightVisionLight->SetAttenuationRadius(6500);NightVisionLight->SetInnerConeAngle(45);NightVisionLight->SetOuterConeAngle(65);NightVisionLight->SetVisibility(false);NightVisionLight->SetCastShadows(true);
    Muzzle=CreateDefaultSubobject<UPointLightComponent>(TEXT("MuzzleFlash"));Muzzle->SetupAttachment(WeaponRoot);
    Muzzle->SetRelativeLocation(FVector(80,0,0));Muzzle->SetIntensity(0);Muzzle->SetAttenuationRadius(1200);Muzzle->SetLightColor(FLinearColor(1,.49,.1));Muzzle->SetCastShadows(false);
}
void ALWCharacter::BeginPlay()
{
    Super::BeginPlay();LWLighting::Load();LWGraphics33::Load();World=ALWWorld::Get(this);if(!World)World=GetWorld()->SpawnActor<ALWWorld>();
    Arms->SetStaticMesh(World->Mesh(TEXT("Arms")));InitializeInventory();InitializeStash();Equip(0);WeaponRoot->SetRelativeLocation(FVector(22,21,-22));
    auto& P=Camera->PostProcessSettings;
    P.bOverride_AutoExposureMethod=true;P.AutoExposureMethod=EAutoExposureMethod::AEM_Manual;P.bOverride_AutoExposureBias=true;P.AutoExposureBias=0;
    P.bOverride_AutoExposureApplyPhysicalCameraExposure=true;P.AutoExposureApplyPhysicalCameraExposure=false;
    P.bOverride_ColorSaturation=true;P.ColorSaturation=FVector4(.69,.72,.62,1);P.bOverride_ColorContrast=true;P.ColorContrast=FVector4(1.08,1.08,1.08,1);
    P.bOverride_FilmGrainIntensity=true;P.FilmGrainIntensity=.38f;P.bOverride_VignetteIntensity=true;P.VignetteIntensity=.48f;
    P.bOverride_BloomIntensity=true;P.BloomIntensity=.25f;P.bOverride_SceneFringeIntensity=true;P.SceneFringeIntensity=.8f;
    if(UMaterialInterface* M=World->Material(TEXT("Crust"))){Filter=UMaterialInstanceDynamic::Create(M,this);Camera->AddOrUpdateBlendable(Filter,1);}
    SetActorLocation(FVector(1400,2300,180));if(Controller)Controller->SetControlRotation(FRotator(-2,98,0));
    if(GConfig){GConfig->GetFloat(TEXT("LethalWorld.Settings"),TEXT("Sensitivity"),Sensitivity,GGameUserSettingsIni);GConfig->GetFloat(TEXT("LethalWorld.Settings"),TEXT("Volume"),MasterVolume,GGameUserSettingsIni);GConfig->GetBool(TEXT("LethalWorld.Settings"),TEXT("Crust"),bCrust,GGameUserSettingsIni);}
    if(auto* S=UGameUserSettings::GetGameUserSettings()){PendingResolution=S->GetScreenResolution();PendingWindowMode=int32(S->GetFullscreenMode());}
    ULWConsole47::Install(this);ApplySettings();GetCharacterMovement()->DisableMovement();SetMenuInput(true);Notify(TEXT("SHELTER 01 IS YOUR LAST LINE OF DEFENSE."),8);
}
void ALWCharacter::SetupPlayerInputComponent(UInputComponent* I)
{
    Super::SetupPlayerInputComponent(I);I->BindKey(EKeys::MiddleMouseButton,IE_Pressed,this,&ALWCharacter::GunBash);I->BindKey(EKeys::N,IE_Pressed,this,&ALWCharacter::ToggleNightVision);BindSeedInput(I);BindIdentityInput(I);
    I->BindKey(EKeys::G,IE_Pressed,this,&ALWCharacter::VehicleGlove);I->BindKey(EKeys::T,IE_Pressed,this,&ALWCharacter::VehicleRadio);I->BindKey(EKeys::Z,IE_Pressed,this,&ALWCharacter::VehicleLeft);I->BindKey(EKeys::X,IE_Pressed,this,&ALWCharacter::VehicleRight);
    I->BindKey(EKeys::K,IE_Pressed,this,&ALWCharacter::ToggleSkills);I->BindKey(EKeys::J,IE_Pressed,this,&ALWCharacter::ToggleJournal);I->BindKey(EKeys::O,IE_Pressed,this,&ALWCharacter::ToggleCrew);
    I->BindAxis(TEXT("Forward"),this,&ALWCharacter::Forward);I->BindAxis(TEXT("Right"),this,&ALWCharacter::Right);I->BindAxis(TEXT("Turn"),this,&ALWCharacter::Turn);I->BindAxis(TEXT("Look"),this,&ALWCharacter::Look);
    I->BindKey(EKeys::P,IE_Pressed,this,&ALWCharacter::ToggleProne54);
    I->BindKey(EKeys::Y,IE_Pressed,this,&ALWCharacter::QuickLootAll54);
    I->BindKey(EKeys::L,IE_Pressed,this,&ALWCharacter::QuickLootOpen54);
    I->BindAction(TEXT("Jump"),IE_Pressed,this,&ALWCharacter::DoJump);
    I->BindAction(TEXT("Sprint"),IE_Pressed,this,&ALWCharacter::StartSprint);I->BindAction(TEXT("Sprint"),IE_Released,this,&ALWCharacter::StopSprint);
    I->BindAction(TEXT("Crouch"),IE_Pressed,this,&ALWCharacter::StartCrouch);I->BindAction(TEXT("Crouch"),IE_Released,this,&ALWCharacter::StopCrouch);
    I->BindAction(TEXT("LeanLeft"),IE_Pressed,this,&ALWCharacter::LeanLeftStart40);I->BindAction(TEXT("LeanLeft"),IE_Released,this,&ALWCharacter::LeanLeftStop40).bExecuteWhenPaused=true;
    I->BindAction(TEXT("LeanRight"),IE_Pressed,this,&ALWCharacter::LeanRightStart40);I->BindAction(TEXT("LeanRight"),IE_Released,this,&ALWCharacter::LeanRightStop40).bExecuteWhenPaused=true;
    I->BindAction(TEXT("QuickHeal"),IE_Pressed,this,&ALWCharacter::QuickHeal40);
    I->BindAction(TEXT("Aim"),IE_Pressed,this,&ALWCharacter::StartAim);I->BindAction(TEXT("Aim"),IE_Released,this,&ALWCharacter::StopAim);
    I->BindAction(TEXT("Attack"),IE_Pressed,this,&ALWCharacter::Attack);I->BindAction(TEXT("Attack"),IE_Released,this,&ALWCharacter::ReleaseAttackInput).bExecuteWhenPaused=true;
    I->BindAction(TEXT("Reload"),IE_Pressed,this,&ALWCharacter::Reload);I->BindAction(TEXT("FireMode"),IE_Pressed,this,&ALWCharacter::ToggleFireMode);
    I->BindAction(TEXT("Interact"),IE_Pressed,this,&ALWCharacter::Interact);I->BindAction(TEXT("Flashlight"),IE_Pressed,this,&ALWCharacter::ToggleFlashlight);
    I->BindAction(TEXT("Crowbar"),IE_Pressed,this,&ALWCharacter::EquipCrowbar);I->BindAction(TEXT("Bat"),IE_Pressed,this,&ALWCharacter::EquipBat);I->BindAction(TEXT("Shotgun"),IE_Pressed,this,&ALWCharacter::EquipShotgun);
    I->BindAction(TEXT("Revolver"),IE_Pressed,this,&ALWCharacter::EquipRevolver);I->BindAction(TEXT("Sniper"),IE_Pressed,this,&ALWCharacter::EquipSniper);I->BindAction(TEXT("SMG"),IE_Pressed,this,&ALWCharacter::EquipSMG);I->BindAction(TEXT("Rifle"),IE_Pressed,this,&ALWCharacter::EquipRifle);I->BindAction(TEXT("LMG"),IE_Pressed,this,&ALWCharacter::EquipLMG);
    I->BindAction(TEXT("NextWeapon"),IE_Pressed,this,&ALWCharacter::NextWeapon);I->BindAction(TEXT("PreviousWeapon"),IE_Pressed,this,&ALWCharacter::PreviousWeapon);
    I->BindAction(TEXT("Pause"),IE_Pressed,this,&ALWCharacter::ToggleMenu).bExecuteWhenPaused=true;I->BindAction(TEXT("Start"),IE_Pressed,this,&ALWCharacter::Interact).bExecuteWhenPaused=true;
    I->BindAction(TEXT("Map"),IE_Pressed,this,&ALWCharacter::ToggleMap);I->BindAction(TEXT("Inventory"),IE_Pressed,this,&ALWCharacter::ToggleInventory);
    I->BindAction(TEXT("Save"),IE_Pressed,this,&ALWCharacter::SaveProgress);I->BindAction(TEXT("Load"),IE_Pressed,this,&ALWCharacter::LoadProgress);I->BindAction(TEXT("BunkerWaypoint"),IE_Pressed,this,&ALWCharacter::RouteToBunker);
}
void ALWCharacter::Forward(float V){InputForward54=V;if(Vehicle){if(Vehicle->PlayerSeat==-2)Vehicle->CabinMove.X=V;else if(Vehicle->PlayerSeat==-1){if(Vehicle->SelfDriving74&&FMath::Abs(V)>.2f)Vehicle->RequestDriverStop();else if(!Vehicle->SelfDriving74)Vehicle->Throttle=V;}return;}if(CanAct()&&!Sliding54)AddMovementInput(GetActorForwardVector(),V);}void ALWCharacter::Right(float V){InputRight54=V;if(Vehicle){if(Vehicle->PlayerSeat==-2)Vehicle->CabinMove.Y=V;else if(Vehicle->PlayerSeat==-1){if(Vehicle->SelfDriving74&&FMath::Abs(V)>.2f)Vehicle->RequestDriverStop();else if(!Vehicle->SelfDriving74)Vehicle->Steer=V;}return;}if(CanAct()&&!Sliding54)AddMovementInput(GetActorRightVector(),V);}
void ALWCharacter::Turn(float V){if(SecurityMode==1){PickAngle=FMath::Clamp(PickAngle+V*Sensitivity*4,-90.f,90.f);return;}if(Vehicle&&!bMenu&&!bInventory&&!bMap&&!RPGPanel&&!SecurityMode){SeatYaw=Vehicle->PlayerSeat!=-1?FMath::UnwindDegrees(SeatYaw+V*Sensitivity*2):FMath::Clamp(SeatYaw+V*Sensitivity*2,-115.f,115.f);return;}if(CanAct()){AddControllerYawInput(V*Sensitivity);LookX=V;}}void ALWCharacter::Look(float V){if(Vehicle&&!bMenu&&!bInventory&&!bMap&&!RPGPanel&&!SecurityMode){SeatPitch=FMath::Clamp(SeatPitch-V*Sensitivity*2,-65.f,55.f);return;}if(CanAct()){AddControllerPitchInput(V*Sensitivity);LookY=V;}}
void ALWCharacter::StartSprint(){if(CanAct())bSprint=true;}void ALWCharacter::StopSprint(){bSprint=false;}
void ALWCharacter::StartCrouch(){if(!CanAct())return;CrouchHeld54=true;CrouchHold56=0;CrouchHoldUsed56=false;if(Prone54){Stand54();return;}if(bSprint&&GetCharacterMovement()->IsMovingOnGround()&&GetVelocity().Size2D()>430&&Stamina>=18&&SlideCooldown54<=0){Sliding54=true;SlideTime54=0;SlideDirection54=GetVelocity().GetSafeNormal2D();Stamina-=18;bSprint=bAim=false;World->Sound(TEXT("Slide54"),GetActorLocation(),.7f);Crouch();return;}if(bIsCrouched)Stand54();else Crouch();}
void ALWCharacter::StopCrouch(){CrouchHeld54=false;CrouchHold56=0;}void ALWCharacter::StartAim(){if(CanAct())bAim=true;}void ALWCharacter::StopAim(){bAim=false;}
void ALWCharacter::DoJump(){if(bStoryLocked)return;if(Vehicle){Vehicle->StandInCamper();return;}if(SittingChair){StandFromChair();return;}if(CanAct()&&Stamina>12){if(Prone54){Stand54();return;}if(TryTraverse54())return;if(Sliding54){Sliding54=false;SlideCooldown54=.8f;UnCrouch();}if(GetCharacterMovement()->IsMovingOnGround()){Jump();Stamina-=10;GroundGrace54=0;}else if(GroundGrace54>0){LaunchCharacter(FVector(0,0,420),false,true);Stamina-=10;GroundGrace54=0;}else JumpBuffer54=.14f;}}
void ALWCharacter::ToggleFlashlight(){if(Vehicle){VehicleLights();return;}if(CanAct()){Flashlight->ToggleVisibility();World->Sound(TEXT("Click"),GetActorLocation());}}
void ALWCharacter::Notify(FString Text,float Seconds){Message=Text;MessageTime=Seconds;}
void ALWCharacter::ClosePanels(){if(Settlement82&&SettlementBuild82)Settlement82->Close();if(Campaign76&&Campaign76->SceneOpen){Campaign76->Pause();RPG.Campaign76.Paused=true;}if(bDebug67&&DebugMenu67)DebugMenu67->Close();SavePanel62=0;if((BaseUI45||BuildMode45)&&BunkerManager45)BunkerManager45->Close();CloseWorkbench39();CancelSecurity();CardTable=nullptr;RestBed=nullptr;Service66=nullptr;RPGPanel=0;Speaker=nullptr;EncounterSpeaker=nullptr;bInventory=false;bMap=false;OpenObject=nullptr;bTrigger=false;SetMenuInput(bMenu);}
void ALWCharacter::ToggleMap(){if(Workbench39)return;if(!bStarted||bMenu||Health<=0)return;if(bMap)ClosePanels();else SwitchTab(1);}
void ALWCharacter::ToggleInventory(){if(Workbench39)return;if(!bStarted||bMenu||Health<=0)return;if(bInventory)ClosePanels();else SwitchTab(0);}
void ALWCharacter::OpenContainer(ALWWorldObject* O){if(IsLocked(O)){StartLockpick(O);return;}ClosePanels();bInventory=true;OpenObject=O;CancelReload();bAim=false;bSprint=false;SetMenuInput(true);}
FString ALWCharacter::FocusPrompt()const{if(QuickContainer54())return FString();if(CorpseFocus)return TEXT("[E] SEARCH BODY");if(SittingChair)return TEXT("[E / SPACE] STAND UP");if(Vehicle)return Vehicle->CabinPrompt();return ResidentFocus?(ResidentFocus->NpcRole==TEXT("civilian")?FString():TEXT("[E] TALK ")+ResidentFocus->DisplayName):ObjectFocus?ObjectFocus->Prompt():Focus?Focus->Prompt():FString();}
void ALWCharacter::Interact(){if(bDebug67)return;if(BaseUI45||BuildMode45||SettlementBuild82)return;if(Workbench39)return;if(bStoryLocked)return;if(OpeningMode){if(OpeningMode==2&&NameEditing){NameEditing=false;return;}OpeningClick(OpeningMode==1?TEXT("intro_pause"):TEXT("creator_done"));return;}if(bWorldSetup)return;if(SittingChair){StandFromChair();return;}if(!bStarted){StartGame(true);return;}if(Health<=0){Respawn();return;}if(bMenu){ToggleMenu();return;}if(Workbench39||bInventory||bMap||RPGPanel||SecurityMode){ClosePanels();return;}if(Vehicle){FName Action=Vehicle->FocusControl();if(Action.IsNone()&&Vehicle->PlayerSeat!=-2)Vehicle->Exit();else Vehicle->Control(Action);return;}if(QuickContainer54()){TakeQuick54();return;}if(CorpseFocus)CorpseFocus->LootBody(this);else if(ResidentFocus)Talk(ResidentFocus);else if(ObjectFocus)ObjectFocus->Use(this);else if(Focus)Focus->Use(this);}
void ALWCharacter::SetMenuInput(bool Enabled)
{
    // A scene owns input until it has actually closed, including load/creation handoffs.
    Enabled=Enabled||(Campaign76&&Campaign76->SceneOpen);
    bUIInputActive=Enabled;
    if(Enabled){StopAttack();bAim=false;bSprint=false;}
    if(auto* PC=Cast<APlayerController>(Controller))if(PC->IsInputKeyDown(EKeys::LeftMouseButton))ConsumeUIAttack();
    WeaponRoot->SetVisibility(!Enabled&&!Vehicle&&!SittingChair&&ActiveGun()!=nullptr,true);
    if(auto* PC=Cast<APlayerController>(Controller)){PC->bShowMouseCursor=Enabled;if(Enabled){FInputModeGameAndUI Mode;Mode.SetHideCursorDuringCapture(false);Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);PC->SetInputMode(Mode);
      if(FSlateApplication::IsInitialized())FSlateApplication::Get().ReleaseAllPointerCapture();
    }else{FInputModeGameOnly Mode;PC->SetInputMode(Mode);}
    // Canvas HUD buttons need the first press forwarded by FSceneViewport.
    // NoCapture skips IE_Pressed in a game viewport (but still sends double-clicks).
    // Temporary capture forwards the press and releases on mouse-up; the cursor stays visible.
    if(auto* Viewport=GetWorld()->GetGameViewport()){Viewport->SetMouseCaptureMode(Enabled?EMouseCaptureMode::CaptureDuringMouseDown:EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown);Viewport->SetMouseLockMode(Enabled?EMouseLockMode::DoNotLock:EMouseLockMode::LockOnCapture);}}
}
void ALWCharacter::StartGame(bool Continue)
{
    if(bStarted){ToggleMenu();return;}if(!Continue||!HasSavedGame()){BeginWorldSetup();return;}bStarted=true;if(Continue)LoadProgress();bMenu=false;bSettings=false;SetMenuInput(false);UGameplayStatics::SetGamePaused(this,false);Notify(TEXT("[I] GEAR   [B] ROUTE TO SAFEHOUSE   [TAB] FIELD MAP"),8);
}
void ALWCharacter::NewGame()
{
    Prone54=Sliding54=Traversing54=false;GetCharacterMovement()->SetCrouchedHalfHeight(52);UnCrouch();
    DrainSave40();SaveDirty40=SaveUrgent43=false;
    FLWLoadingScope45 Loading(TEXT("Preparing new survivor"));if(World->BunkerParts.IsEmpty())World->CreateBunker();Bunker45=FLWBunker45State();
    MissionRecovery37.Empty();
    if(Campaign76){Campaign76->Destroy();Campaign76=nullptr;}if(Story){Story->Destroy();Story=nullptr;}bStoryLocked=false;
    Identity=FLWIdentity();
    if(Vehicle)Vehicle->Exit(true);
    StandFromChair(true);World->Seed=198706;World->TownSetting=World->POISetting=World->TerrainSetting=1;World->ApplyGenerationSettings();World->SurfaceWetness=0;if(Settlement82)Settlement82->Reset();RPG=FLWRPGState();
    UGameplayStatics::SetGamePaused(this,false);ClosePanels();World->KilledZombies.Empty();World->Containers.Empty();World->PropStates.Empty();World->Reset();World->Encounters=FLWEncounterState();Cards=FLWCardGame();World->Vehicles.Empty();
    Health=Stamina=Hunger=Thirst=100;Money=0;Kills=0;bDeadSaved=false;LastDeathBag=NAME_None;World->TimeOfDay=6.f;World->DayNumber=1;InitializeInventory();InitializeStash();World->RestoreBunkerContainers();
    const FVector Start(LWNewGame72::StartXY(),World->HeightAt(LWNewGame72::StartXY())+110);SetActorLocation(Start);World->Stream(Start,true);GetCharacterMovement()->SetMovementMode(MOVE_Walking);GetCharacterMovement()->StopMovementImmediately();if(Controller)Controller->SetControlRotation(FRotator(-2,98,0));
    for(AActor* A:World->BunkerParts)if(auto* F=Cast<ALWWorldObject>(A))if(F->Kind==ELWObjectKind::Furniture)F->SetFurniture(F->UseType);
    bStarted=true;bMenu=false;bSettings=false;bSafehouse=false;bWaypoint=false;CancelReload();Equip(0);SetMenuInput(false);ApplyIdentity();if(BunkerManager45)BunkerManager45->Rebuild();RPG.Story.Enabled=false;SaveProgress();
}
void ALWCharacter::ToggleMenu(){if(SettlementBuild82&&Settlement82){Settlement82->Input(EKeys::Escape,true);return;}if(!bStarted)if(auto* PC=Cast<APlayerController>(Controller))if(auto* H=Cast<ALWHUD>(PC->GetHUD());H&&H->News81){H->News81=false;ConsumeUIAttack();return;}if(Campaign76&&Campaign76->SceneOpen){ClosePanels();return;}if(bDebug67&&DebugMenu67){DebugMenu67->Close();return;}if(SavePanel62){SavePanel62=0;return;}if((BaseUI45||BuildMode45)&&BunkerManager45){BunkerManager45->Close();return;}if(OpeningMode){OpeningClick(OpeningMode==1?TEXT("intro_pause"):TEXT("creator_back"));return;}if(bWorldSetup){bWorldSetup=false;bSeedEdit=false;return;}if(!bStarted)return;if(Workbench39||bInventory||bMap||RPGPanel||SecurityMode){ClosePanels();return;}if(bVideoConfirm)RevertVideo();bMenu=!bMenu;bSettings=false;bSprint=false;bAim=false;bTrigger=false;UGameplayStatics::SetGamePaused(this,bMenu);SetMenuInput(bMenu);if(bMenu&&Health>0)RequestSave40();}
void ALWCharacter::ToggleSettings(){if(bVideoConfirm)RevertVideo();bSettings=!bSettings;}void ALWCharacter::ToggleCrust(){bCrust=!bCrust;ApplySettings();}
void ALWCharacter::CycleVolume(){MasterVolume=MasterVolume>=.99f?0:FMath::Min(1.f,MasterVolume+.1f);ApplySettings();}
void ALWCharacter::CycleSensitivity(){Sensitivity=FMath::Min(5.f,Sensitivity+(Sensitivity<.5f?.05f:Sensitivity<1?.1f:.25f));ApplySettings();}
void ALWCharacter::LowerSensitivity(){Sensitivity=FMath::Max(.025f,Sensitivity-(Sensitivity<=.5f?.05f:Sensitivity<=1?.1f:.25f));ApplySettings();}
void ALWCharacter::ApplySettings()
{
    Sensitivity=FMath::Clamp(Sensitivity,.025f,5.f);MasterVolume=FMath::Clamp(MasterVolume,0.f,1.f);if(Filter)Filter->SetScalarParameterValue(TEXT("Strength"),bCrust?1:0);Camera->PostProcessSettings.FilmGrainIntensity=bCrust?.38f:0;
    if(FAudioDeviceHandle Device=GetWorld()->GetAudioDevice())Device->SetTransientPrimaryVolume(MasterVolume);
    if(GConfig){GConfig->SetFloat(TEXT("LethalWorld.Settings"),TEXT("Sensitivity"),Sensitivity,GGameUserSettingsIni);GConfig->SetFloat(TEXT("LethalWorld.Settings"),TEXT("Volume"),MasterVolume,GGameUserSettingsIni);GConfig->SetBool(TEXT("LethalWorld.Settings"),TEXT("Crust"),bCrust,GGameUserSettingsIni);GConfig->Flush(false,GGameUserSettingsIni);}
}
void ALWCharacter::CycleResolution(){const FIntPoint Sizes[]={{1280,720},{1600,900},{1920,1080},{2560,1440},{3840,2160}};int32 I=0;for(;I<5;I++)if(Sizes[I]==PendingResolution)break;PendingResolution=Sizes[(I+1)%5];}
void ALWCharacter::CycleWindowMode(){PendingWindowMode=(PendingWindowMode+1)%3;}
FString ALWCharacter::WindowModeName()const{return PendingWindowMode==0?TEXT("FULLSCREEN"):PendingWindowMode==1?TEXT("BORDERLESS"):TEXT("WINDOWED");}
void ALWCharacter::ApplyVideo(){if(auto* S=UGameUserSettings::GetGameUserSettings()){S->SetScreenResolution(PendingResolution);S->SetFullscreenMode(EWindowMode::Type(PendingWindowMode));S->ApplyResolutionSettings(false);bVideoConfirm=true;VideoConfirmTimer=GetWorld()->GetRealTimeSeconds()+15;}}
void ALWCharacter::ConfirmVideo(){if(auto* S=UGameUserSettings::GetGameUserSettings()){S->ConfirmVideoMode();S->SaveSettings();}bVideoConfirm=false;}
void ALWCharacter::RevertVideo(){if(auto* S=UGameUserSettings::GetGameUserSettings()){S->RevertVideoMode();S->ApplyResolutionSettings(false);PendingResolution=S->GetScreenResolution();PendingWindowMode=int32(S->GetFullscreenMode());}bVideoConfirm=false;}
float ALWCharacter::TakeDamage(float D,const FDamageEvent& E,AController* I,AActor* C)
{
    if((Story&&GetWorld()->GetTimeSeconds()<Story->CombatGrace52)||bGod47||bStoryLocked||!bStarted||bMenu||Health<=0||bSafehouse||ALWWorld::IsSafePosition(GetActorLocation()))return 0;if(E.IsOfType(FRadialDamageEvent::ClassID))D=Super::TakeDamage(D,E,I,C);float Applied=FMath::Max(0.f,D)*(World->Difficulty==0?.65f:World->Difficulty==2?1.45f:1.f)*FMath::Max(.3f,1-Stat(TEXT("armor")));
    for(auto& Item:Inventory)if(Item.Slot==TEXT("Armor")&&Item.Durability>0){Applied*=.6f;Item.Durability=FMath::Max(0,Item.Durability-FMath::CeilToInt(D*.5f));break;}
    if(Applied>0){LastCombatTime=GetWorld()->GetTimeSeconds();if(auto* Enemy=Cast<ALWZombie>(C)){CombatTarget=Enemy;CombatTargetTime=8;}}Health=FMath::Max(0.f,Health-Applied);HurtFlash=.65f;World->Sound(TEXT("Hurt"),GetActorLocation(),.8f);if(Health<=0)HandleDeath();return Applied;
}
void ALWCharacter::Reward(int32 Amount){Money+=FMath::RoundToInt(FMath::Max(0,Amount)*(1+Stat(TEXT("credits"))));Kills++;GainXP(40);QuestEvent(TEXT("kill"));Notify(FString::Printf(TEXT("CONTAINMENT CREDIT +$%d"),Amount),3);World->Sound(TEXT("Credit"),GetActorLocation(),.5f);}
void ALWCharacter::Tick(float Dt)
{
    Super::Tick(Dt);if(bConsole47)return;if(TickRecoil50(Dt))return;TickSave40(Dt);TickLean40(Dt);TickMovement54(Dt);TickNightVision();TickSurvivorBody(Dt);if(!World)return;if(bVideoConfirm&&GetWorld()->GetRealTimeSeconds()>VideoConfirmTimer)RevertVideo();if(!bStarted||bMenu)return;ALWBunker45::Ensure(this);if(!RPG.Claims82.IsEmpty())ALWSettlement82::Ensure(this);
    if(bStoryLocked){bTrigger=bSprint=bAim=false;GetCharacterMovement()->StopMovementImmediately();return;}
    HurtFlash=FMath::Max(0.f,HurtFlash-Dt);HitMarker=FMath::Max(0.f,HitMarker-Dt);MessageTime-=Dt;bSafehouse=ALWWorld::IsSafePosition(GetActorLocation());
    if(Health<=0){if(!bDeadSaved)HandleDeath();return;}if(bInventory||bMap||SecurityMode){bTrigger=false;GetCharacterMovement()->StopMovementImmediately();}
    CaptureMission37();TickDiscovery(Dt);TickAttachments();TickVehicleSeat();TickSecurity(Dt);TickRPG(Dt);if(!Vehicle)UpdateWeapon(Dt);TickRoute(Dt);const bool Moving=GetVelocity().Size2D()>30,Running=bSprint&&Moving&&Stamina>1&&!bIsCrouched&&!bAim&&!Prone54&&!Sliding54&&!Traversing54;
    Stamina=FMath::Clamp(Stamina+Dt*(Running?-19.f:AttackTimer>0?0.f:12.f*(1+Stat(TEXT("recovery")))),0.f,MaxStamina());if(Stamina<1)bSprint=false;if(!Sliding54)GetCharacterMovement()->MaxWalkSpeed=Prone54?95:Running?610*(1+Stat(TEXT("sprint"))):bAim?220:340*(1+Stat(TEXT("speed")));
    if(!bSafehouse){Hunger=FMath::Max(0.f,Hunger-Dt*.021f*(World->Difficulty==0?.65f:World->Difficulty==2?1.35f:1.f)*FMath::Max(.2f,1-Stat(TEXT("hunger"))));Thirst=FMath::Max(0.f,Thirst-Dt*(World->Difficulty==0?.65f:World->Difficulty==2?1.35f:1.f)*(Running?.057f:.032f)*FMath::Max(.2f,1-Stat(TEXT("thirst"))));if(!bGod47&&(Hunger<=0||Thirst<=0)){Health=FMath::Max(0.f,Health-Dt*.65f);if(Health<=0){HandleDeath();return;}}}
    const float AimFov=LWMods::Fov(ActiveGun());Camera->SetFieldOfView(FMath::FInterpTo(Camera->FieldOfView,bAim&&Weapon>=2?AimFov:Running?92:86,Dt,8));
    StepClock+=Dt;if(Moving&&!Sliding54&&!Traversing54&&!GetCharacterMovement()->IsFalling()&&StepClock>(Prone54?.85f:Running?.29f:.48f)){
        StepClock=0;FHitResult Hit;FCollisionQueryParams Q(NAME_None,false,this);GetWorld()->LineTraceSingleByChannel(Hit,GetActorLocation(),GetActorLocation()-FVector(0,0,180),ECC_Visibility,Q);
        World->Sound(bIndoors?TEXT("StepIndoor"):Hit.ImpactPoint.Z<22?TEXT("StepRoad"):TEXT("StepEarth"),GetActorLocation()-FVector(0,0,70),bIsCrouched?.22f:Running?1.f:.65f,FMath::FRandRange(.88f,1.12f));if(!bSafehouse)World->Noise(GetActorLocation(),bIsCrouched?170:Running?2200:700);
    }
    GunBashTimer=FMath::Max(0.f,GunBashTimer-Dt);CombatTargetTime=FMath::Max(0.f,CombatTargetTime-Dt);if(CombatTargetTime<=0||!IsValid(CombatTarget))CombatTarget=nullptr;RoofClock+=Dt;if(RoofClock>.16f){RoofClock=0;FHitResult Hit;FCollisionQueryParams Q(NAME_None,false,this);const FVector C=Camera->GetComponentLocation();bIndoors=bSafehouse||GetWorld()->LineTraceSingleByChannel(Hit,C,C+FVector(0,0,1000),ECC_Visibility,Q);Focus=nullptr;ObjectFocus=nullptr;ResidentFocus=nullptr;CorpseFocus=nullptr;if(GetWorld()->LineTraceSingleByChannel(Hit,C,C+Camera->GetForwardVector()*330,ECC_Visibility,Q)){if(auto* Enemy=Cast<ALWZombie>(Hit.GetActor());Enemy&&Enemy->bDead)CorpseFocus=Enemy;ResidentFocus=Cast<ALWResident>(Hit.GetActor());Focus=Cast<ALWInteractable>(Hit.GetActor());ObjectFocus=Cast<ALWWorldObject>(Hit.GetActor());}}
    AutoSaveClock+=Dt;if(AutoSaveClock>60){AutoSaveClock=0;RequestSave40();}
}

void ALWCharacter::ToggleNightVision(){
 if(!bStarted||bMenu||bInventory||bMap||RPGPanel||SecurityMode||Health<=0)return;
 if(!Inventory.ContainsByPredicate([](const auto& I){return I.Definition==TEXT("night_vision")&&I.Slot==TEXT("Helmet");})){Notify(TEXT("EQUIP NIGHT VISION IN THE HELMET SLOT"));return;}
 bNightVision=!bNightVision;TickNightVision();World->Sound(TEXT("Click"),GetActorLocation());Notify(bNightVision?TEXT("NIGHT VISION ON"):TEXT("NIGHT VISION OFF"));
}
void ALWCharacter::TickNightVision(){
 if(!Inventory.ContainsByPredicate([](const auto& I){return I.Definition==TEXT("night_vision")&&I.Slot==TEXT("Helmet");})||Health<=0||!bStarted)bNightVision=false;
 NightVisionLight->SetVisibility(bNightVision);auto& P=Camera->PostProcessSettings;
 P.bOverride_ColorGain=true;P.ColorGain=bNightVision?FVector4(.22f,1.25f,.3f,1):FVector4(1,1,1,1);
 P.AutoExposureBias=bNightVision?2.5f:0;P.ColorSaturation=bNightVision?FVector4(0,0,0,1):FVector4(.69,.72,.62,1);
 P.VignetteIntensity=bNightVision?.72f:.48f;P.FilmGrainIntensity=bNightVision?.65f:bCrust?.38f:0;
}
