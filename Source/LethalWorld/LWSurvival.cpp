#include "LWGeography84.h"
#include "LWSettlement82.h"
#include "LWCampaign76.h"
#include "LWMainMenu81.h"
#include "LWNewGame72.h"
#include "LWCurrency70.h"
#include "LWBunker45.h"
#include "LWSaveSlots62.h"
#include "LWArsenal62.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "LWLoading45.h"
#include "LWWeaponMods.h"
#include "LWVehicle.h"
#include "EngineUtils.h"
#include "LWCharacter.h"
#include "LWCanada68.h"
#include "LWStory.h"
#include "LWWorld.h"
#include "LWWorldObject.h"
#include "LWSaveGame.h"
#include "LWNavigation.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

namespace {
FString SaveSlot(){if(FParse::Param(FCommandLine::Get(),TEXT("LWV17Smoke")))return TEXT("AllAmericanMeltdown_AutomationV17");if(FParse::Param(FCommandLine::Get(),TEXT("LWV16Smoke")))return TEXT("AllAmericanMeltdown_AutomationV16");if(FParse::Param(FCommandLine::Get(),TEXT("LWV15Smoke")))return TEXT("AllAmericanMeltdown_AutomationV15");if(FParse::Param(FCommandLine::Get(),TEXT("LWV14Smoke")))return TEXT("LethalWorld_AutomationV14");if(FParse::Param(FCommandLine::Get(),TEXT("LWV11Smoke")))return TEXT("LethalWorld_AutomationV11");if(FParse::Param(FCommandLine::Get(),TEXT("LWV10Smoke")))return TEXT("LethalWorld_AutomationV10");if(FParse::Param(FCommandLine::Get(),TEXT("LWV9Smoke")))return TEXT("LethalWorld_AutomationV9");if(FParse::Param(FCommandLine::Get(),TEXT("LWV7Smoke")))return TEXT("LethalWorld_AutomationV7");if(FParse::Param(FCommandLine::Get(),TEXT("LWV6Smoke")))return TEXT("LethalWorld_AutomationV6");if(FParse::Param(FCommandLine::Get(),TEXT("LWV5Smoke")))return TEXT("LethalWorld_AutomationV5");if(FParse::Param(FCommandLine::Get(),TEXT("LWV4Smoke")))return TEXT("LethalWorld_AutomationV4");if(FParse::Param(FCommandLine::Get(),TEXT("LWV3Smoke")))return TEXT("LethalWorld_AutomationV3");return FParse::Param(FCommandLine::Get(),TEXT("LWSmoke"))||FParse::Param(FCommandLine::Get(),TEXT("LWV2Smoke"))?TEXT("LethalWorld_AutomationV2"):TEXT("LethalWorld_Survivor");}
bool Pack(TArray<FLWItemInstance>& Items,FLWItemInstance Item,int W=12,int H=-1){return LWItems::Place(Items,Item,W,H<0?LWItems::InventoryHeight(Items):H);}
int32 Price(const FLWItemInstance& I){return LWMods::Price(I);}
}
FString ALWCharacter::SaveSlot40()const{return SaveSlot()==TEXT("LethalWorld_Survivor")?LWSaves62::Auto():SaveSlot();}
FLWItemInstance* ALWCharacter::FindItem(FGuid Id){return Inventory.FindByPredicate([Id](const auto& I){return I.Id==Id;});}
const FLWItemInstance* ALWCharacter::FindItem(FGuid Id)const{return Inventory.FindByPredicate([Id](const auto& I){return I.Id==Id;});}
FLWItemInstance* ALWCharacter::ActiveGun(){return FindItem(ActiveWeaponId);}const FLWItemInstance* ALWCharacter::ActiveGun()const{return FindItem(ActiveWeaponId);}
void ALWCharacter::InitializeInventory()
{
    Inventory.Empty();ActiveWeaponId.Invalidate();
    auto Gear=[&](FName Id,FName Slot){auto I=LWItems::Make(Id);I.Slot=Slot;Inventory.Add(I);return I.Id;};
    Gear(TEXT("crowbar"),TEXT("Melee"));
    FGuid Rev=Gear(TEXT("revolver"),TEXT("Sidearm"));FindItem(Rev)->Cylinder.Init(1,6);
    // Six rounds in the revolver; no spare ammunition or starter supplies.
}
void ALWCharacter::InitializeStash()
{
    Stash.Empty();
}

bool ALWCharacter::GiveItem(FName Definition,int32 Count)
{
    if(LWItems::Def(Definition).Category==TEXT("WeaponPart"))Definition=TEXT("scrap");
    const int32 Max=LWItems::Def(Definition).MaxStack;if(Count<=0||LWItems::Def(Definition).Id.IsNone())return false;
    auto Next=Inventory;
    for(auto& I:Next)if(I.Definition==Definition&&I.Slot.IsNone()&&I.Count<Max){const int N=FMath::Min(Count,Max-I.Count);I.Count+=N;Count-=N;}
    while(Count>0){const int N=FMath::Min(Count,Max);if(!Pack(Next,LWItems::Make(Definition,N)))return false;Count-=N;}
    Inventory=MoveTemp(Next);return true;
}
int32 ALWCharacter::AmmoCount(FName Ammo)const{int N=0;for(const auto& I:Inventory)if(I.Definition==Ammo)N+=I.Count;return N;}
int32 ALWCharacter::TakeAmmo(FName Ammo,int32 Count)
{
    int Taken=0;for(auto& I:Inventory)if(I.Definition==Ammo){int N=FMath::Min(Count-Taken,I.Count);I.Count-=N;Taken+=N;if(Taken==Count)break;}
    Inventory.RemoveAll([](const auto& I){return I.Count<=0;});return Taken;
}
TArray<FLWItemInstance>* ALWCharacter::ItemsFor(int32 Panel)
{
    if(Panel==0)return &Inventory;
    if(IsLocked(OpenObject))return nullptr;
    if(!IsValid(OpenObject)||!bInventory||FVector::Dist(GetActorLocation(),OpenObject->GetActorLocation())>550)return nullptr;
    if(Panel==1&&bSafehouse&&OpenObject->Kind==ELWObjectKind::Stash)return &Stash;
    if(Panel==2)if(auto* R=World->Containers.Find(OpenObject->RecordId))return &R->Items;
    return nullptr;
}
bool ALWCharacter::MoveItem(int32 From,int32 To,FGuid Id,int32 X,int32 Y,bool Rotate,FName Slot)
{
    auto* A=ItemsFor(From);auto* B=ItemsFor(To);if(!A||!B)return false;
    const auto* Source=A->FindByPredicate([Id](const auto& I){return I.Id==Id;});if(!Source)return false;
    const bool Trade=IsValid(OpenObject)&&OpenObject->Kind==ELWObjectKind::Trader;
    const int Cost=Trade&&From==2&&To==0?LWCanada68::TradePrice(this,*Source,true):0,Gain=Trade&&From==0&&To==2?LWCanada68::TradePrice(this,*Source,false):0;
    if(Cost>LWCurrency70::Balance(this)){Notify(FString(TEXT("INSUFFICIENT "))+LWCurrency70::Unit(this));return false;}
    if(Gain>MAX_int64-LWCurrency70::Balance(this))return false;
    if(Trade&&((From!=0&&From!=2)||(To!=0&&To!=2)))return false;
    const int W=To==2?World->Containers.FindChecked(OpenObject->RecordId).Width:12,H=To==0?LWItems::InventoryHeight(Inventory):To==1?14:World->Containers.FindChecked(OpenObject->RecordId).Height;
    const auto BeforeA=*A,BeforeB=*B;
    bool Result=false;
    if(!Slot.IsNone()){
        if(To!=0||!LWItems::CanEquip(*Source,Slot))return false;
        auto AC=*A,BC=*B;
        if(From==To){
            FLWItemInstance Displaced;bool HasDisplaced=false;
            if(auto* Old=AC.FindByPredicate([&](const auto& I){return I.Slot==Slot&&I.Id!=Id;})){Displaced=*Old;HasDisplaced=true;AC.RemoveAll([&](const auto& I){return I.Id==Displaced.Id;});}
            Result=LWItems::Equip(AC,Id,Slot);
            if(Result&&HasDisplaced){Displaced.Slot=NAME_None;Result=Pack(AC,Displaced);}
            if(Result)*A=MoveTemp(AC);
        }else{
            if(BC.ContainsByPredicate([&](const auto& I){return I.Slot==Slot;}))return false;
            // A vacant equipment slot does not require spare backpack space. Stage outside the grid,
            // preserving Transfer's linked-magazine validation, then equip before committing either array.
            Result=LWItems::Transfer(AC,BC,Id,0,32,Rotate,64,96)&&LWItems::Equip(BC,Id,Slot);
            if(Result){*A=MoveTemp(AC);*B=MoveTemp(BC);}
        }
    }else Result=From==To?LWItems::Move(*A,Id,X,Y,Rotate,W,H):LWItems::Transfer(*A,*B,Id,X,Y,Rotate,W,H);
    if(Result&&!LWItems::ValidateEquipment(Inventory)){*A=BeforeA;*B=BeforeB;Notify(TEXT("EMPTY EXTRA BACKPACK ROWS / RIG WEAPON SLOT FIRST"));return false;}
    if(!Result){Notify(TEXT("DOES NOT FIT // ROTATE WITH [R]"),2);return false;}
    LWCurrency70::Wallet(this)+=Gain-Cost;RecordSettlementTransfer(From,To,FMath::Max(Cost,Gain),Trade);
    if(!FindItem(ActiveWeaponId)){ActiveWeaponId.Invalidate();WeaponMesh->SetVisibility(false);}
    else if(auto* Gun=ActiveGun();Gun&&Gun->Slot.IsNone()){ActiveWeaponId.Invalidate();WeaponRoot->SetVisibility(false,true);}
    World->Sound(Trade&&(Cost||Gain)?TEXT("Trade"):(From==2&&To==0?TEXT("LootPickup"):TEXT("InventoryMove")),GetActorLocation(),.7f);SyncAmmoHUD();PersistWorldChange();return true;
}
bool ALWCharacter::LoadAmmoOnto(int32 From,FGuid AmmoId,int32 To,FGuid TargetId)
{
    if(IsValid(OpenObject)&&OpenObject->Kind==ELWObjectKind::Trader&&(From==2||To==2)){Notify(TEXT("BUY ITEMS BEFORE MODIFYING THEM"));return false;}
    auto* A=ItemsFor(From);auto* B=ItemsFor(To);if(!A||!B||AmmoId==TargetId)return false;
    auto* Ammo=A->FindByPredicate([&](const auto& I){return I.Id==AmmoId;});auto* Mag=B->FindByPredicate([&](const auto& I){return I.Id==TargetId;});if(!Ammo||!Mag)return false;
    if(LWItems::Def(Ammo->Definition).Category==TEXT("Attachment")){
        const FName Mount=LWMods::Mount(Ammo->Definition);
        if(!Mag->Parts39.IsEmpty()){Notify(TEXT("EDIT CUSTOM WEAPONS AT A WORKBENCH"));return false;}
        if(!LWMods::Compatible(Mag->Definition,Ammo->Definition)||Mag->Attachments.Contains(Mount)){Notify(TEXT("INCOMPATIBLE OR MOUNT OCCUPIED"));return false;}
        Mag->Attachments.Add(Mount,Ammo->Definition);Ammo->Count--;A->RemoveAll([](const auto& I){return I.Count<=0;});
        ConfigureAttachments();RecordSettlementTransfer(From,To,0,false);PersistWorldChange();return true;
    }
    int N=LWItems::LoadMagazine(*Mag,*Ammo);
    if(N<=0&&LWItems::StackCompatible(*Ammo,*Mag)){N=FMath::Min(Ammo->Count,LWItems::Def(Mag->Definition).MaxStack-Mag->Count);Mag->Count+=N;Ammo->Count-=N;}
    if(N<=0){Notify(TEXT("INCOMPATIBLE AMMO OR MAGAZINE FULL"),2);return false;}
    A->RemoveAll([](const auto& I){return I.Count<=0;});World->Sound(TEXT("Reload"),GetActorLocation(),.5f);RecordSettlementTransfer(From,To,0,false);SyncAmmoHUD();PersistWorldChange();return true;
}
bool ALWCharacter::EquipItem(FGuid Id)
{
    auto* Item=FindItem(Id);if(!Item)return false;const auto Slots=LWItems::Def(Item->Definition).EquipSlots;
    if(!Item->Slot.IsNone()&&Item->Slot!=TEXT("Loaded")){if(LWItems::Def(Item->Definition).WeaponIndex>=0){ActiveWeaponId=Id;Equip(LWItems::Def(Item->Definition).WeaponIndex);}return true;}
    for(FName Slot:Slots)if(!Inventory.ContainsByPredicate([&](const auto& I){return I.Slot==Slot;})){
        if(LWItems::Equip(Inventory,Id,Slot)){if(LWItems::Def(Item->Definition).WeaponIndex>=0){ActiveWeaponId=Id;Equip(LWItems::Def(Item->Definition).WeaponIndex);}PersistWorldChange();return true;}
    }
    Notify(TEXT("EQUIPMENT SLOT OCCUPIED // DRAG TO SWAP"));return false;
}
bool ALWCharacter::UseItem(FGuid Id)
{
    auto* I=FindItem(Id);if(!I)return false;const auto& D=LWItems::Def(I->Definition);
    if(I->Definition==TEXT("settlement_flag"))return ALWSettlement82::Ensure(this)->BeginClaim();
    if(D.Category!=TEXT("Consumable"))return EquipItem(Id);
    if(I->Definition==TEXT("ca_trauma")){Health=MaxHealth();Stamina=MaxStamina();}
    else if(I->Definition==TEXT("ca_meal")){Hunger=100;Thirst=FMath::Min(100.f,Thirst+30);}
    else if(I->Definition==TEXT("ca_tonic")){Thirst=100;Stamina=MaxStamina();Health=FMath::Min(MaxHealth(),Health+25);}
    else if(I->Definition==TEXT("medkit"))Health=FMath::Min(MaxHealth(),Health+65*(1+Stat(TEXT("healing"))));
    else if(I->Definition==TEXT("food"))Hunger=FMath::Min(100.f,Hunger+45);
    else if(I->Definition==TEXT("water"))Thirst=FMath::Min(100.f,Thirst+55);else return false;
    I->Count--;Inventory.RemoveAll([](const auto& Item){return Item.Count<=0;});World->Sound(TEXT("LootPickup"),GetActorLocation());PersistWorldChange();return true;
}
bool ALWCharacter::UnloadItem(FGuid Id)
{
    auto* I=FindItem(Id);if(!I||I->Slot==TEXT("Loaded"))return false;
    CancelReload();const auto& Def=LWItems::Def(I->Definition);auto Next=Inventory;
    auto* Gun=Next.FindByPredicate([&](const auto& It){return It.Id==Id;});
    if(Gun->LoadedMagazine.IsValid()){
        const FGuid Mid=Gun->LoadedMagazine;auto* M=Next.FindByPredicate([&](const auto& It){return It.Id==Mid;});if(!M)return false;
        auto Mag=*M;Mag.Slot=NAME_None;Next.RemoveAll([&](const auto& It){return It.Id==Mid;});
        Next.FindByPredicate([&](const auto& It){return It.Id==Id;})->LoadedMagazine.Invalidate();
        if(!Pack(Next,Mag)){Notify(TEXT("MAKE SPACE FOR THE MAGAZINE"));return false;}
        Inventory=MoveTemp(Next);SyncAmmoHUD();PersistWorldChange();return true;
    }
    int Count=Gun->Rounds+(Gun->Chamber==1?1:0);for(int R:Gun->Cylinder)if(R==1)Count++;
    if(Count<=0)return false;Gun->Rounds=0;Gun->Chamber=0;for(int& R:Gun->Cylinder)R=0;
    auto Original=Inventory;Inventory=MoveTemp(Next);if(!GiveItem(Def.AmmoType,Count)){Inventory=MoveTemp(Original);Notify(TEXT("MAKE SPACE FOR LOOSE AMMO"));return false;}
    SyncAmmoHUD();PersistWorldChange();return true;
}
bool ALWCharacter::DropItem(FGuid Id)
{
    if(bSafehouse){Notify(TEXT("USE SECURED STORAGE INSIDE THE BUNKER"));return false;}
    const auto* Item=FindItem(Id);if(!Item||Item->Slot==TEXT("Loaded"))return false;const FGuid Mag=Item->LoadedMagazine;
    TArray<FLWItemInstance> Drop;Drop.Add(*Item);if(auto* M=FindItem(Mag))Drop.Add(*M);
    auto Next=Inventory;Next.RemoveAll([&](const auto& I){return I.Id==Id||I.Id==Mag;});
    if(!LWItems::ValidateEquipment(Next)){Notify(TEXT("EMPTY EXTRA BACKPACK ROWS / RIG WEAPON SLOT FIRST"));return false;}
    World->DropGear(GetActorLocation()+GetActorForwardVector()*130,Drop);Inventory=MoveTemp(Next);
    if(Id==ActiveWeaponId){ActiveWeaponId.Invalidate();WeaponRoot->SetVisibility(false,true);}PersistWorldChange();return true;
}
void ALWCharacter::PersistWorldChange(){if(bStarted)RequestSave40();}
void ALWCharacter::EnterSafehouse()
{
    if(Vehicle)Vehicle->Exit(true);ClosePanels();CancelReload();bSafehouse=true;bAim=false;bSprint=false;GetCharacterMovement()->StopMovementImmediately();
    SetActorLocation(ALWWorld::BunkerSpawn(),false,nullptr,ETeleportType::TeleportPhysics);Controller->SetControlRotation(FRotator(0,20,0));bIndoors=true;World->Sound(TEXT("BunkerDoor"),GetActorLocation());Notify(TEXT("SHELTER 01 // YOU AND YOUR STASH ARE SAFE"),6);SaveProgress();
}
void ALWCharacter::LeaveSafehouse()
{
    ClosePanels();SetActorLocation(ALWWorld::BunkerDoorPosition()+FVector(0,250,110),false,nullptr,ETeleportType::TeleportPhysics);Controller->SetControlRotation(FRotator(0,90,0));bSafehouse=false;bIndoors=false;GetCharacterMovement()->StopMovementImmediately();World->Sound(TEXT("BunkerDoor"),GetActorLocation());Notify(TEXT("AIRLOCK OPEN // CARRIED GEAR IS AT RISK"));SaveProgress();
}
void ALWCharacter::HandleDeath()
{
    StandFromChair(true);
    Traversing54=Sliding54=false;
    if(bDeadSaved)return;Cards=FLWCardGame();if(Vehicle)Vehicle->Exit(true);ClosePanels();CancelReload();LastDeathBag=World->DropGear(GetActorLocation(),Inventory);Inventory.Empty();ActiveWeaponId.Invalidate();
    Health=0;bDeadSaved=true;bSprint=false;bAim=false;bTrigger=false;GetCharacterMovement()->DisableMovement();SetMenuInput(true);World->Sound(TEXT("DeathDrop"),GetActorLocation());Notify(TEXT("CARRIED GEAR DROPPED // STASH SECURED"),99);SaveProgress();
}
void ALWCharacter::Respawn()
{
    if(Health<=0&&HasMissionRecovery37()){Notify(TEXT("CHOOSE HOW TO CONTINUE YOUR MISSION"),5);SetMenuInput(true);return;}
    if(RPG.Story.Enabled&&RPG.Story.GearHeld&&RPG.Story.Stage>=21&&RPG.Story.Stage<=23){Health=MaxHealth();bDeadSaved=false;bMenu=false;ClosePanels();ALWStoryDirector::Ensure(this)->RestorePrison();SetMenuInput(false);SaveProgress();return;}
    Prone54=Sliding54=Traversing54=false;GetCharacterMovement()->SetCrouchedHalfHeight(52);UnCrouch();
    Health=MaxHealth();Stamina=MaxStamina();Hunger=Thirst=100;bDeadSaved=false;bStarted=true;bMenu=false;ClosePanels();GetCharacterMovement()->SetMovementMode(MOVE_Walking);if(!RestoreRespawn())EnterSafehouse();
    if(auto* R=World->Containers.Find(LastDeathBag))SetWaypoint(FVector2D(R->Position));Notify(TEXT("RECOVER YOUR GEAR BAG OR EQUIP FROM THE STASH"),8);SaveProgress();
}
void ALWCharacter::SaveProgress()
{
    if(OpeningMode||bWorldSetup||bLoadingSave||!bStarted||!World||(Health<=0&&!bDeadSaved))return;
    RequestSave40();SaveUrgent43=true;
}
ULWSaveGame* ALWCharacter::MakeProgressSnapshot37()
{
    if(Campaign76)Campaign76->Sync();
    auto* S=Cast<ULWSaveGame>(UGameplayStatics::CreateSaveGameObject(ULWSaveGame::StaticClass()));
    S->Version=2;S->Bunker45=Bunker45;S->RPG=RPG;S->Identity=Identity;S->Vehicles=World->Vehicles;S->SeatedVehicle=Vehicle?Vehicle->RecordId:NAME_None;S->SeatedSeat84=Vehicle?Vehicle->PlayerSeat:-1;S->CabinEye84=Vehicle?Vehicle->CabinEye:FVector::ZeroVector;S->Geography84=84;
    S->MenuLocation81=bSafehouse?TEXT("Bunker"):World->LocationName;S->MenuMission81=LWMenu81::Mission(RPG);
    World->SnapshotEncounters();S->Encounters=World->Encounters;
    S->TownSetting=World->TownSetting;S->POISetting=World->POISetting;S->TerrainSetting=World->TerrainSetting;S->Difficulty=World->Difficulty;S->SurfaceWetness=World->SurfaceWetness;
    S->Seed=World->Seed;S->PlayerStance54=Vehicle||SittingChair||Traversing54?0:Prone54?2:bIsCrouched?1:0;S->Position=SittingChair?SitReturn:Traversing54?TraversalStart54:GetActorLocation();S->View=GetControlRotation();S->Health=Health;S->Stamina=Stamina;S->Hunger=Hunger;S->Thirst=Thirst;S->Money=Money;S->Cards=Cards;
    if(Vehicle){FVector At=Vehicle->GetActorLocation()+Vehicle->GetActorRightVector()*185;At.Z=World->HeightAt(FVector2D(At))+110;S->Position=At;}
    S->Inventory=Inventory;S->Stash=Stash;S->WorldContainers=World->Containers;S->PropStates=World->PropStates;S->ActiveWeaponId=ActiveWeaponId;S->LastDeathBag=LastDeathBag;
    S->Weapon=Weapon;S->Shells=Shells;S->ReserveShells=ReserveShells;S->Kills=Kills;S->TimeOfDay=World->TimeOfDay;S->DayNumber=World->DayNumber;S->Killed=World->KilledZombies.Array();
    S->Sensitivity=Sensitivity;S->MasterVolume=MasterVolume;S->Crust=bCrust;S->HasWaypoint=bWaypoint;S->Waypoint=Waypoint;
    return S;
}
bool ALWCharacter::HasSavedGame() const{return !LWSaves62::Latest().IsEmpty()||UGameplayStatics::DoesSaveGameExist(SaveSlot(),0);}
void ALWCharacter::LoadProgress()
{
    if(OpeningMode||bWorldSetup||!World)return;FLWLoadingScope45 Loading(TEXT("Restoring survivor"));DrainSave40();SaveDirty40=false;auto* S=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromSlot(LWSaves62::Latest().IsEmpty()?SaveSlot():LWSaves62::Latest(),0));if(!S||S->Version>2)return;
    ApplyProgressSnapshot37(S);
}
void ALWCharacter::ApplyProgressSnapshot37(ULWSaveGame* S)
{
    DrainSave40();SaveDirty40=SaveUrgent43=false;
    if(!S||!World)return;LWGeography84::Migrate(S);Prone54=Sliding54=Traversing54=false;GetCharacterMovement()->SetCrouchedHalfHeight(52);UnCrouch();bIsCrouched=false;GetCapsuleComponent()->SetCapsuleSize(32,88,false);if(World->BunkerParts.IsEmpty())World->CreateBunker();Bunker45=S->Bunker45;MissionRecovery37=S->MissionRecovery37;
    if(Campaign76){Campaign76->Destroy();Campaign76=nullptr;}if(Story){Story->Destroy();Story=nullptr;}bStoryLocked=false;
    StandFromChair(true);if(Settlement82)Settlement82->Reset();
    TGuardValue<bool> LoadingGuard(bLoadingSave,true);
    UGameplayStatics::SetGamePaused(this,false);if(Vehicle)Vehicle->Exit(true);ClosePanels();CancelReload();World->Reset();World->Encounters=S->Encounters;World->Vehicles=S->Vehicles;World->Seed=S->Seed;World->TownSetting=S->TownSetting;World->POISetting=S->POISetting;World->TerrainSetting=S->TerrainSetting;World->Difficulty=S->Difficulty;World->SurfaceWetness=S->SurfaceWetness;World->ApplyGenerationSettings();World->KilledZombies.Empty();for(uint32 Id:S->Killed)World->KilledZombies.Add(Id);
    Identity=S->Identity;ApplyIdentity();RPG=S->RPG;if(RPG.Attributes.Num()!=7)RPG.Attributes.Init(1,7);World->PropStates=S->PropStates;for(AActor* Actor:World->BunkerParts)if(auto* O=Cast<ALWWorldObject>(Actor))if(O->Kind==ELWObjectKind::Door)O->ConfigureProp();World->TimeOfDay=S->TimeOfDay;World->DayNumber=S->DayNumber;World->Containers=S->Version>=2?S->WorldContainers:TMap<FName,FLWContainerRecord>();
    if(S->Version>=2){Inventory=S->Inventory;Stash=S->Stash;ActiveWeaponId=S->ActiveWeaponId;LastDeathBag=S->LastDeathBag;}else{InitializeInventory();InitializeStash();}
    TArray<FLWItemInstance> ReturnedGear72;LWNewGame72::RetireStory(RPG,Inventory,Stash,ReturnedGear72);MissionRecovery37.Remove(TEXT("chapter1"));ChapterJournal=false;
    Health=S->Health;Stamina=S->Stamina;Hunger=S->Hunger;Thirst=S->Thirst;Money=S->Money;Cards=S->Cards;Kills=S->Kills;bDeadSaved=Health<=0;
    const uint8 Stance54=Health>0?FMath::Min<uint8>(S->PlayerStance54,2):0;const FVector At=Health<=0?ALWWorld::BunkerSpawn():((S->RPG.Story.Enabled||S->RPG.Story.Stage>0||S->RPG.Story.GearHeld)&&LWNewGame72::LegacySite(S->Position))?FVector(LWNewGame72::StartXY(),World->HeightAt(LWNewGame72::StartXY())+110):S->Position+FVector(0,0,4+(Stance54==2?56:Stance54==1?36:0));SetActorLocation(At,false,nullptr,ETeleportType::TeleportPhysics);LWArsenal62::Migrate(Inventory);LWArsenal62::Migrate(Stash);for(auto& Town:RPG.Settlements)for(auto& Listing:Town.Value.Listings)LWArsenal62::Migrate(Listing.Items);for(auto& Pair:World->Containers)LWArsenal62::Migrate(Pair.Value.Items);MigrateDirectAmmo();World->Stream(At,true);if(!ReturnedGear72.IsEmpty())World->DropGear(At,ReturnedGear72);if(Controller)Controller->SetControlRotation(S->View);
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);GetCharacterMovement()->StopMovementImmediately();bStarted=true;bMenu=false;bSettings=false;bSafehouse=ALWWorld::IsSafePosition(At);
    if(Stance54){Prone54=Stance54==2;GetCharacterMovement()->SetCrouchedHalfHeight(Prone54?32:52);Crouch();GetCharacterMovement()->Crouch(false);CameraOffset54=0;Camera->SetRelativeLocation(FVector(0,0,Prone54?22:34));}
    if(ActiveGun())Equip(LWItems::Def(ActiveGun()->Definition).WeaponIndex);else WeaponRoot->SetVisibility(false,true);
    bWaypoint=S->HasWaypoint&&!S->RPG.Story.Enabled;Waypoint=S->Waypoint;RouteTimer=0;Route.Empty();RouteGround.Empty();bRouteComplete=false;LastRouteFrom=FVector2D::ZeroVector;
    for(AActor* A:World->BunkerParts)if(auto* F=Cast<ALWWorldObject>(A))if(F->Kind==ELWObjectKind::Furniture)F->SetFurniture(F->UseType);
    World->RestoreBunkerContainers();if(BunkerManager45)BunkerManager45->Rebuild();ApplySettings();SetMenuInput(false);if(RPG.Campaign76.Started)ALWCampaign76::Ensure(this)->Begin(false);if(Health<=0){if(HasMissionRecovery37()){GetCharacterMovement()->DisableMovement();SetMenuInput(true);}else Respawn();}else {if(!S->SeatedVehicle.IsNone())for(TActorIterator<ALWVehicle> I(GetWorld());I;++I)if(I->RecordId==S->SeatedVehicle){const float SavedSpeed=I->Speed;I->Speed=0;I->Enter(this,S->SeatedSeat84);I->Speed=SavedSpeed;if(S->SeatedSeat84==-2){I->CabinEye=S->CabinEye84;TickVehicleSeat();}break;}Notify(TEXT("SURVIVOR RECORD RESTORED"));}
}
void ALWCharacter::SetWaypoint(FVector2D P){Waypoint=P;bWaypoint=true;RouteTimer=0;Route.Empty();RouteGround.Empty();Notify(TEXT("WAYPOINT SET // FOLLOW THE AMBER ROUTE"));}
void ALWCharacter::ClearWaypoint(){bWaypoint=false;Route.Empty();RouteGround.Empty();}
void ALWCharacter::RouteToBunker(){if(bStarted&&Health>0)SetWaypoint(FVector2D(ALWWorld::BunkerDoorPosition()));}
void ALWCharacter::TickRoute(float Dt)
{
    if(!bWaypoint||bSafehouse)return;const FVector2D Here(GetActorLocation());
    if((Here-Waypoint).Size()<180){Notify(TEXT("WAYPOINT REACHED"));ClearWaypoint();return;}
    RouteTimer-=Dt;if(RouteTimer<=0){
        RouteTimer=2;
        double Nearest=DBL_MAX;int32 Segment=INDEX_NONE;FVector2D Projection=Here;
        for(int32 I=1;I<Route.Num();++I){
            const FVector2D A=Route[I-1],D=Route[I]-A;
            const FVector2D On=A+D*FMath::Clamp(FVector2D::DotProduct(Here-A,D)/FMath::Max(1.,D.SizeSquared()),0.,1.);
            const double Distance=(On-Here).SizeSquared();
            if(Distance<Nearest){Nearest=Distance;Segment=I;Projection=On;}
        }
        const bool Reroute=Route.Num()<2||Nearest>FMath::Square(900.)||(!bRouteComplete&&(Here-Route.Last()).Size()<1400);
        if(Reroute)Route=LWNavigation::FindPath(Here,Waypoint,World->Seed,&bRouteComplete);
        else if(Segment!=INDEX_NONE){Route.RemoveAt(0,Segment);Route.Insert(Projection,0);}
        if(Reroute||(Here-LastRouteFrom).Size()>=1000){
        LastRouteFrom=Here;RouteGround.Empty();
        auto Ground=[&](FVector2D XY){
            float Z=World->HeightAt(XY);FHitResult Hit;FCollisionQueryParams Q(NAME_None,false,this);
            if(GetWorld()->LineTraceSingleByObjectType(Hit,FVector(XY,Z+200),FVector(XY,Z-100),FCollisionObjectQueryParams(ECC_WorldStatic),Q))Z=Hit.ImpactPoint.Z;
            return FVector(XY,Z+22);
        };
        for(int I=1;I<Route.Num();I++){
            const FVector2D A=Route[I-1],B=Route[I],D=B-A;if((A-Here).Size()>22000&&(B-Here).Size()>22000)continue;
            const int Steps=FMath::Clamp(FMath::CeilToInt(D.Size()/250),1,160);
            for(int J=0;J<Steps&&RouteGround.Num()<480;J++){const FVector2D L=A+D*(double(J)/Steps),R=A+D*(double(J+1)/Steps);if((L-Here).Size()>22000)continue;RouteGround.Add(Ground(L));RouteGround.Add(Ground(R));}
        }
    }
    }
    // Depth-tested route segments are sampled onto the generated ground; never draw through walls.
    if(!bMap&&!bInventory)for(int I=1;I<RouteGround.Num();I+=2)DrawDebugLine(GetWorld(),RouteGround[I-1],RouteGround[I],FColor(238,167,59),false,0,0,4);
}
