#include "LWArsenal62.h"
#include "LWInventory.h"
#include "LWWeaponParts39.h"
#include "LWFuel.h"

#include "Engine/StaticMesh.h"
#include "Misc/PackageName.h"
#include "UObject/UObjectGlobals.h"

namespace
{
    bool LWIsEquipmentSlot(FName Slot)
    {
        static const TArray<FName> Slots = {
            TEXT("Primary"), TEXT("Secondary"), TEXT("Sidearm"), TEXT("Melee"),
            TEXT("Armor"), TEXT("Helmet"), TEXT("Rig"), TEXT("Backpack"), TEXT("Tool"), TEXT("RigPrimary")
        };
        return Slots.Contains(Slot);
    }

    bool LWIsLoaded(const FLWItemInstance& Item)
    {
        return Item.Slot == FName(TEXT("Loaded"));
    }

    FName LWCanonicalAmmo(FName Id)
    {
        // Old belt-ammunition loot remains usable as loose 7.62; belt boxes hold their own Rounds.
        return Id == FName(TEXT("ammo_762belt")) ? FName(TEXT("ammo_762")) : Id;
    }

    bool LWValidDefinition(const FLWItemDefinition& D)
    {
        if (D.Id.IsNone() || D.Width < 1 || D.Width > 64 || D.Height < 1 || D.Height > 64 ||
            D.MaxStack < 1 || D.Capacity < 0 || D.Price < 0 || D.MaxDurability < 0 ||
            D.ContainerWidth < 0 || D.ContainerHeight < 0 ||
            ((D.ContainerWidth == 0) != (D.ContainerHeight == 0)))
        {
            return false;
        }
        TSet<FIntPoint> Seen;
        for (const FIntPoint Cell : D.Cells)
        {
            if (Cell.X < 0 || Cell.Y < 0 || Cell.X >= D.Width || Cell.Y >= D.Height || Seen.Contains(Cell))
            {
                return false;
            }
            Seen.Add(Cell);
        }
        for (FName Slot : D.EquipSlots)
        {
            if (!LWIsEquipmentSlot(Slot))
            {
                return false;
            }
        }
        return true;
    }

    struct FLWCatalogState
    {
        bool bInitialized = false;
        TMap<FName, FLWItemDefinition> Definitions;
    };

    FLWCatalogState& LWCatalogState()
    {
        static FLWCatalogState State;
        return State;
    }

    void LWCopyCatalog(FLWCatalogState& State, const ULWItemCatalog* Catalog)
    {
        State.Definitions.Empty();
        for (const FLWItemDefinition& D : LWItems::Defaults())
        {
            State.Definitions.Add(D.Id, D);
        }
        if (Catalog)
        {
            for (const FLWItemDefinition& D : Catalog->Items)
            {
                if (LWValidDefinition(D))
                {
                    State.Definitions.Add(D.Id, D);
                }
            }
        }
    for(FName Direct:{FName(TEXT("missile_launcher")),FName(TEXT("taser"))})if(auto* D=State.Definitions.Find(Direct)){D->MagazineType=NAME_None;D->AmmoType=Direct==TEXT("taser")?TEXT("battery"):TEXT("ammo_rocket");D->Capacity=Direct==TEXT("taser")?10:1;}
        State.bInitialized = true;
    }

    void LWEnsureCatalog()
    {
        FLWCatalogState& State = LWCatalogState();
        if (!State.bInitialized)
        {
            // Initialize before loading, so an asset load cannot recurse into an uninitialized catalog.
            LWCopyCatalog(State, nullptr);
            if (FPackageName::DoesPackageExist(TEXT("/Game/Data/DA_ItemCatalog")))
            {
                const ULWItemCatalog* Catalog = LoadObject<ULWItemCatalog>(nullptr,
                    TEXT("/Game/Data/DA_ItemCatalog.DA_ItemCatalog"), nullptr, LOAD_NoWarn | LOAD_Quiet);
                if (Catalog)
                {
                    LWCopyCatalog(State, Catalog);
                }
            }
        }
    }

    bool LWValidInstance(const FLWItemInstance& Item)
    {
        const FLWItemDefinition& D = LWItems::Def(Item.Definition);
        return Item.Id.IsValid() && !D.Id.IsNone() && Item.Count > 0 && Item.Count <= D.MaxStack;
    }

    int32 LWUniqueIndex(const TArray<FLWItemInstance>& Items, FGuid Id)
    {
        if (!Id.IsValid())
        {
            return INDEX_NONE;
        }
        int32 Result = INDEX_NONE;
        for (int32 Index = 0; Index < Items.Num(); ++Index)
        {
            if (Items[Index].Id == Id)
            {
                if (Result != INDEX_NONE)
                {
                    return INDEX_NONE;
                }
                Result = Index;
            }
        }
        return Result;
    }

    bool LWContainsId(const TArray<FLWItemInstance>& Items, FGuid Id)
    {
        return Items.ContainsByPredicate([Id](const FLWItemInstance& Item) { return Item.Id == Id; });
    }

    bool LWIsReferencedMagazine(const TArray<FLWItemInstance>& Items, FGuid Id)
    {
        return Id.IsValid() && Items.ContainsByPredicate(
            [Id](const FLWItemInstance& Item) { return Item.LoadedMagazine == Id; });
    }

    void LWSetGridPosition(FLWItemInstance& Item, int32 X, int32 Y, bool Rotated)
    {
        Item.X = X;
        Item.Y = Y;
        Item.bRotated = Rotated;
        Item.Slot = NAME_None;
    }

    bool LWLinkedMagazineIndex(const TArray<FLWItemInstance>& Items, const FLWItemInstance& Gun, int32& Index)
    {
        Index = INDEX_NONE;
        if (!Gun.LoadedMagazine.IsValid())
        {
            return true;
        }
        Index = LWUniqueIndex(Items, Gun.LoadedMagazine);
        if (Index == INDEX_NONE || Gun.Id == Gun.LoadedMagazine)
        {
            return false;
        }
        const FLWItemInstance& Mag = Items[Index];
        const FLWItemDefinition& GunDef = LWItems::Def(Gun.Definition);
        const FLWItemDefinition& MagDef = LWItems::Def(Mag.Definition);
        if (!LWValidInstance(Mag) || Mag.Count != 1 || !LWIsLoaded(Mag) || Mag.LoadedMagazine.IsValid() ||
            GunDef.Category != FName(TEXT("Weapon")) || GunDef.MagazineType != Mag.Definition ||
            MagDef.Category != FName(TEXT("Magazine")) || GunDef.AmmoType.IsNone() ||
            LWCanonicalAmmo(GunDef.AmmoType) != LWCanonicalAmmo(MagDef.AmmoType) ||
            Mag.Rounds < 0 || Mag.Rounds > MagDef.Capacity)
        {
            return false;
        }
        // A magazine cannot be carried by two guns, even in malformed saved data.
        for (const FLWItemInstance& Other : Items)
        {
            if (Other.Id != Gun.Id && Other.LoadedMagazine == Mag.Id)
            {
                return false;
            }
        }
        return true;
    }
}

ULWItemCatalog::ULWItemCatalog()
{
    Items = LWItems::Defaults();
}

TArray<FLWItemDefinition> LWItems::Defaults()
{
    TArray<FLWItemDefinition> Result;
    auto Add = [&Result](const TCHAR* Id, const TCHAR* Label, const TCHAR* Category,
        int32 Width, int32 Height, int32 Price) -> FLWItemDefinition&
    {
        FLWItemDefinition& D = Result.AddDefaulted_GetRef();
        D.Id = FName(Id);
        D.DisplayName = FText::FromString(Label);
        D.Category = FName(Category);
        D.Width = Width;
        D.Height = Height;
        D.Price = Price;
        return D;
    };
    auto Weapon = [&Add](const TCHAR* Id, const TCHAR* Label, int32 Index, int32 W, int32 H,
        int32 Price, const TCHAR* Ammo, const TCHAR* Magazine, int32 Capacity, const TCHAR* Slot,
        const TCHAR* MeshName)
    {
        FLWItemDefinition& D = Add(Id, Label, TEXT("Weapon"), W, H, Price);
        D.WeaponIndex = Index;
        D.AmmoType = FName(Ammo);
        D.MagazineType = FName(Magazine);
        D.Capacity = Capacity;
        D.EquipSlots.Add(FName(Slot));
        if (D.EquipSlots.Contains(FName(TEXT("Primary"))))
        {
            D.EquipSlots.Add(TEXT("Secondary")); D.EquipSlots.Add(TEXT("RigPrimary"));
        }
        if (MeshName)
        {
            D.Mesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(FString::Printf(
                TEXT("/Game/Art/Meshes/SM_%s.SM_%s"), MeshName, MeshName)));
        }
    };

    Weapon(TEXT("crowbar"), TEXT("Crowbar"), 0, 1, 4, 45, TEXT("None"), TEXT("None"), 0, TEXT("Melee"), TEXT("Crowbar"));
    Weapon(TEXT("bat"), TEXT("Baseball Bat"), 1, 1, 4, 40, TEXT("None"), TEXT("None"), 0, TEXT("Melee"), TEXT("Bat"));
    Weapon(TEXT("shotgun"), TEXT("Pump Shotgun"), 2, 5, 2, 450, TEXT("ammo_12g"), TEXT("None"), 6, TEXT("Primary"), TEXT("Shotgun"));
    Weapon(TEXT("doublebarrel"), TEXT("Double Barrel Shotgun"), 8, 5, 2, 620, TEXT("ammo_12g"), TEXT("None"), 2, TEXT("Primary"), TEXT("DoubleBarrelV18"));
    Weapon(TEXT("revolver"), TEXT(".357 Revolver"), 3, 2, 2, 600, TEXT("ammo_357"), TEXT("None"), 6, TEXT("Sidearm"), nullptr);
    Weapon(TEXT("sniper"), TEXT("Sniper Rifle"), 4, 6, 2, 1200, TEXT("ammo_762"), TEXT("mag_sniper5"), 5, TEXT("Primary"), nullptr);
    Weapon(TEXT("smg"), TEXT("Submachine Gun"), 5, 3, 2, 900, TEXT("ammo_9mm"), TEXT("mag_smg30"), 30, TEXT("Primary"), nullptr);
    Weapon(TEXT("rifle"), TEXT("Assault Rifle"), 6, 5, 2, 1400, TEXT("ammo_556"), TEXT("mag_rifle30"), 30, TEXT("Primary"), nullptr);
    Weapon(TEXT("lmg"), TEXT("Light Machine Gun"), 7, 6, 3, 2400, TEXT("ammo_762"), TEXT("mag_lmg100"), 100, TEXT("Primary"), nullptr);

    Weapon(TEXT("missile_launcher"),TEXT("Missile Launcher"),9,6,3,4200,TEXT("ammo_rocket"),TEXT(""),1,TEXT("Primary"),TEXT("MissileLauncher24"));
    Weapon(TEXT("minigun"),TEXT("Minigun"),10,6,3,5500,TEXT("ammo_762"),TEXT("minigun_box"),150,TEXT("Primary"),TEXT("Minigun24"));
    Weapon(TEXT("sawedoff"),TEXT("Sawed-off Shotgun"),11,3,2,700,TEXT("ammo_12g"),TEXT("None"),2,TEXT("Primary"),TEXT("SawedOff24"));
    Weapon(TEXT("desert_eagle"),TEXT("Desert Eagle"),12,2,2,1500,TEXT("ammo_50ae"),TEXT("mag_deagle7"),7,TEXT("Sidearm"),TEXT("DesertEagle24"));
    Weapon(TEXT("m4"),TEXT("M4 Carbine"),13,5,2,1900,TEXT("ammo_556"),TEXT("mag_rifle30"),30,TEXT("Primary"),TEXT("M424"));
    Weapon(TEXT("taser"),TEXT("Taser"),14,2,2,650,TEXT("battery"),TEXT(""),10,TEXT("Sidearm"),TEXT("Taser24"));
    Weapon(TEXT("flamethrower"),TEXT("Flamethrower"),15,6,3,3300,TEXT("ammo_fuel"),TEXT("fuel_tank"),100,TEXT("Primary"),TEXT("Flamethrower24"));

    Weapon(TEXT("tenbarrel"),TEXT("10-Barrel Shotgun"),16,6,3,4200,TEXT("ammo_12g"),TEXT(""),10,TEXT("Primary"),TEXT("TenBarrel50"));
    Weapon(TEXT("giant_glock"),TEXT("Giant Glock"),17,5,3,2200,TEXT("ammo_50ae"),TEXT("mag_giant50"),12,TEXT("Primary"),TEXT("GiantGlock50"));
    Weapon(TEXT("questionable_ak"),TEXT("Questionable AK"),18,5,3,2700,TEXT("ammo_762"),TEXT("mag_ak75"),75,TEXT("Primary"),TEXT("QuestionableAK50"));
    Weapon(TEXT("finger_guns"),TEXT("Finger Guns"),19,1,1,250,TEXT(""),TEXT(""),0,TEXT("Sidearm"),TEXT("FingerGuns50"));
    Weapon(TEXT("budget_cut"),TEXT("Budget Cut AR-15"),20,5,2,850,TEXT("ammo_556"),TEXT("mag_rifle30"),30,TEXT("Primary"),TEXT("BudgetCut50"));

    for(const TCHAR* Id:{TEXT("att_light"),TEXT("att_laser"),TEXT("att_reflex"),TEXT("att_holo"),TEXT("att_scope4"),TEXT("att_scope8"),TEXT("att_vertical"),TEXT("att_angled")}) {
        auto& D=Add(Id,Id,TEXT("Attachment"),1,1,180);
        D.DisplayName=FText::FromString(FString(Id).Replace(TEXT("att_"),TEXT("")).Replace(TEXT("scope"),TEXT("Scope x")));
    }
    auto Ammo = [&Add](const TCHAR* Id, const TCHAR* Label, int32 Stack, int32 Price)
    {
        FLWItemDefinition& D = Add(Id, Label, TEXT("Ammo"), 1, 1, Price);
        D.MaxStack = Stack;
        D.MaxDurability = 0;
    };
    Ammo(TEXT("ammo_12g"), TEXT("12 Gauge Shells"), 20, 8);
    Ammo(TEXT("ammo_357"), TEXT(".357 Rounds"), 60, 5);
    Ammo(TEXT("ammo_762"), TEXT("7.62 Rounds"), 60, 6);
    Ammo(TEXT("ammo_9mm"), TEXT("9mm Rounds"), 60, 3);
    Ammo(TEXT("ammo_556"), TEXT("5.56 Rounds"), 60, 5);
    Ammo(TEXT("ammo_762belt"), TEXT("7.62 Rounds (Legacy Belt)"), 60, 6);

    auto Magazine = [&Add](const TCHAR* Id, const TCHAR* Label, const TCHAR* AmmoId,
        int32 Capacity, int32 W, int32 H, int32 Price)
    {
        FLWItemDefinition& D = Add(Id, Label, TEXT("Magazine"), W, H, Price);
        D.AmmoType = FName(AmmoId);
        D.Capacity = Capacity;
    };
    Magazine(TEXT("mag_sniper5"), TEXT("7.62 Magazine (5)"), TEXT("ammo_762"), 5, 1, 2, 55);
    Magazine(TEXT("mag_smg30"), TEXT("9mm Magazine (30)"), TEXT("ammo_9mm"), 30, 1, 2, 75);
    Magazine(TEXT("mag_rifle30"), TEXT("5.56 Magazine (30)"), TEXT("ammo_556"), 30, 1, 2, 90);
    Magazine(TEXT("mag_lmg100"), TEXT("Detachable 7.62 Belt Box (100)"), TEXT("ammo_762"), 100, 2, 2, 160);

    Ammo(TEXT("ammo_rocket"),TEXT("Missile"),4,220);
    Ammo(TEXT("ammo_30mm"),TEXT("30mm Turret Shell"),24,45);
    Ammo(TEXT("ammo_50ae"),TEXT(".50 AE"),35,12);
    Ammo(TEXT("ammo_dart"),TEXT("Taser Dart Pair"),8,35);
    Ammo(TEXT("ammo_fuel"),TEXT("Flamethrower Fuel"),100,4);
    Magazine(TEXT("rocket_tube"),TEXT("Missile Canister"),TEXT("ammo_rocket"),1,1,4,90);
    Magazine(TEXT("minigun_box"),TEXT("7.62 Feed Box (150)"),TEXT("ammo_762"),150,3,2,260);
    Magazine(TEXT("mag_giant50"),TEXT("Giant .50 AE Magazine (12)"),TEXT("ammo_50ae"),12,2,3,170);
    Magazine(TEXT("mag_ak75"),TEXT("AK Extended Magazine (75)"),TEXT("ammo_762"),75,2,3,210);
    Magazine(TEXT("mag_deagle7"),TEXT(".50 AE Magazine (7)"),TEXT("ammo_50ae"),7,1,2,110);
    Magazine(TEXT("taser_cartridge"),TEXT("Taser Cartridge"),TEXT("ammo_dart"),1,1,1,35);
    Magazine(TEXT("fuel_tank"),TEXT("Fuel Tank (100)"),TEXT("ammo_fuel"),100,2,3,160);
    Add(TEXT("armor"), TEXT("Body Armor"), TEXT("Armor"), 3, 3, 650).EquipSlots.Add(TEXT("Armor"));
    Add(TEXT("helmet"), TEXT("Helmet"), TEXT("Armor"), 2, 2, 250).EquipSlots.Add(TEXT("Helmet"));
    FLWItemDefinition& Rig = Add(TEXT("rig"), TEXT("Chest Rig"), TEXT("Container"), 3, 3, 200);
    Rig.EquipSlots.Add(TEXT("Rig"));
    Rig.ContainerWidth = 6;
    Rig.ContainerHeight = 4;
    FLWItemDefinition& Backpack = Add(TEXT("backpack"), TEXT("Backpack"), TEXT("Container"), 4, 4, 300);
    Backpack.EquipSlots.Add(TEXT("Backpack"));
    Backpack.ContainerWidth = 12;
    Backpack.ContainerHeight = 24;
    Backpack.Mesh=TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Art/Meshes/SM_Backpack25.SM_Backpack25")));
    for(int K=0;K<4;K++){
        const TCHAR* Ids[]={TEXT("sling_pack"),TEXT("hiking_pack"),TEXT("military_pack"),TEXT("expedition_pack")};
        const TCHAR* Names[]={TEXT("Sling Pack"),TEXT("Hiking Backpack"),TEXT("Military Rucksack"),TEXT("Expedition Pack")};
        const int Rows[]={2,6,8,10};
        auto& D=Add(Ids[K],Names[K],TEXT("Container"),K==0?2:4,K==0?3:K+3,150+K*450);
        D.EquipSlots.Add(TEXT("Backpack"));D.ContainerWidth=12;D.ContainerHeight=Rows[K];
        const TCHAR* Meshes[]={TEXT("SlingPack25"),TEXT("HikingPack25"),TEXT("MilitaryPack25"),TEXT("ExpeditionPack25")};
        D.Mesh=TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(FString::Printf(TEXT("/Game/Art/Meshes/SM_%s.SM_%s"),Meshes[K],Meshes[K])));
    }
    auto& NV=Add(TEXT("night_vision"),TEXT("Night Vision Goggles"),TEXT("Equipment"),2,2,2200);NV.EquipSlots.Add(TEXT("Helmet"));NV.Mesh=TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Art/Meshes/SM_NightVision25.SM_NightVision25")));
    Add(TEXT("tool"), TEXT("Utility Tool"), TEXT("Tool"), 1, 2, 60).EquipSlots.Add(TEXT("Tool"));
    Add(TEXT("food"), TEXT("Canned Food"), TEXT("Consumable"), 1, 1, 20).MaxStack = 5;
    Add(TEXT("water"), TEXT("Drinking Water"), TEXT("Consumable"), 1, 2, 15).MaxStack = 3;
    Add(TEXT("medkit"), TEXT("Medical Kit"), TEXT("Consumable"), 2, 2, 120).MaxStack = 3;
    Add(TEXT("ca_trauma"),TEXT("Northern Trauma Kit"),TEXT("Consumable"),2,2,350).MaxStack=3;
    Add(TEXT("ca_meal"),TEXT("Toronto Trail Meal"),TEXT("Consumable"),1,1,95).MaxStack=5;
    Add(TEXT("ca_tonic"),TEXT("Alpine Recovery Tonic"),TEXT("Consumable"),1,2,130).MaxStack=3;
    Add(TEXT("scrap"), TEXT("Scrap Metal"), TEXT("Material"), 1, 1, 2).MaxStack = 240;
    Add(TEXT("settlement_flag"),TEXT("Settlement Flag"),TEXT("Tool"),2,2,650).MaxStack=1;
    Add(TEXT("lockpick"), TEXT("Lockpicks"), TEXT("Tool"), 1, 2, 12).MaxStack=20;
    Add(TEXT("car_key"), TEXT("Vehicle Key"), TEXT("Tool"), 1, 1, 15);
    Result.Add(LWFuel::GasCanDefinition());
    Add(TEXT("battery"), TEXT("Battery"), TEXT("Consumable"), 1, 1, 12).MaxStack = 10;
    for(const auto& A:LWArsenal62::Attachments()){auto& D=Add(*A.Id.ToString(),*A.Name,TEXT("Attachment"),2,1,280);}
    Result.Append(LWParts39::Items()); // Legacy definitions remain readable for migration; never generated.
    return Result;
}

void LWItems::SetCatalog(const ULWItemCatalog* Catalog)
{
    FLWCatalogState& State = LWCatalogState();
    if (Catalog)
    {
        LWCopyCatalog(State, Catalog);
    }
    else
    {
        State.Definitions.Empty();
        State.bInitialized = false;
    }
}

const FLWItemDefinition& LWItems::Def(FName Id)
{
    LWEnsureCatalog();

    if (const FLWItemDefinition* Definition = LWCatalogState().Definitions.Find(Id))
    {
        return *Definition;
    }
    static const FLWItemDefinition Unknown = []
    {
        FLWItemDefinition D;
        D.DisplayName = FText::FromString(TEXT("Unknown Item"));
        D.Width = D.Height = D.MaxStack = D.MaxDurability = 0;
        return D;
    }();
    return Unknown;
}

FLWItemInstance LWItems::Make(FName Definition, int32 Count)
{
    const FLWItemDefinition& D = Def(Definition);
    FLWItemInstance Item;
    if (D.Id.IsNone() || Count < 1 || Count > D.MaxStack)
    {
        Item.Count = 0;
        return Item;
    }
    Item.Id = FGuid::NewGuid();
    Item.Definition = D.Id;
    Item.Count = Count;
    Item.Durability = D.MaxDurability;
    if(D.Id==TEXT("gas_can"))Item.Rounds=LWFuel::CanCapacity;
    if ((D.Id == FName(TEXT("revolver")) || D.Id == FName(TEXT("doublebarrel")) || D.Id == FName(TEXT("sawedoff"))))
    {
        Item.Cylinder.Init(0, D.Capacity);
    }
    return Item;
}

TArray<FIntPoint> LWItems::Footprint(FName Definition, bool Rotated)
{
    const FLWItemDefinition& D = Def(Definition);
    TArray<FIntPoint> Result = D.Cells;
    if (Result.IsEmpty())
    {
        Result.Reserve(D.Width * D.Height);
        for (int32 Y = 0; Y < D.Height; ++Y)
        {
            for (int32 X = 0; X < D.Width; ++X)
            {
                Result.Emplace(X, Y);
            }
        }
    }
    if (Rotated)
    {
        for (FIntPoint& Cell : Result)
        {
            Cell = FIntPoint(D.Height - 1 - Cell.Y, Cell.X);
        }
    }
    return Result;
}

bool LWItems::Fits(const TArray<FLWItemInstance>& Items, const FLWItemInstance& Item,
    int32 X, int32 Y, bool Rotated, int32 Width, int32 Height, FGuid Ignore)
{
    if (!LWValidInstance(Item) || LWIsLoaded(Item) || Width <= 0 || Height <= 0 ||
        X < 0 || Y < 0 || X >= Width || Y >= Height)
    {
        return false;
    }
    TSet<FIntPoint> CandidateCells;
    for (const FIntPoint Cell : Footprint(Item.Definition, Rotated))
    {
        // Subtraction after validating the anchor avoids overflow at extreme int32 coordinates.
        if (Cell.X >= Width - X || Cell.Y >= Height - Y)
        {
            return false;
        }
        CandidateCells.Add(FIntPoint(X + Cell.X, Y + Cell.Y));
    }
    if (CandidateCells.IsEmpty())
    {
        return false;
    }
    for (const FLWItemInstance& Other : Items)
    {
        if (!Other.Slot.IsNone() || (Ignore.IsValid() && Other.Id == Ignore))
        {
            continue;
        }
        // A stack exhausted by LoadMagazine no longer occupies cells, even before the caller removes it.
        if (Other.Count == 0 && Def(Other.Definition).Category == FName(TEXT("Ammo")))
        {
            continue;
        }
        if (!LWValidInstance(Other) || Other.X < 0 || Other.Y < 0 || Other.X >= Width || Other.Y >= Height)
        {
            return false;
        }
        for (const FIntPoint Cell : Footprint(Other.Definition, Other.bRotated))
        {
            if (Cell.X >= Width - Other.X || Cell.Y >= Height - Other.Y ||
                CandidateCells.Contains(FIntPoint(Other.X + Cell.X, Other.Y + Cell.Y)))
            {
                return false;
            }
        }
    }
    return true;
}

bool LWItems::Place(TArray<FLWItemInstance>& Items, FLWItemInstance& Item, int32 Width, int32 Height)
{
    if (!LWValidInstance(Item) || LWIsLoaded(Item) || LWContainsId(Items, Item.Id) ||
        LWIsReferencedMagazine(Items, Item.Id) || Width <= 0 || Height <= 0)
    {
        return false;
    }
    for (int32 Orientation = 0; Orientation < 2; ++Orientation)
    {
        const bool Rotated = Orientation == 0 ? Item.bRotated : !Item.bRotated;
        for (int32 Y = 0; Y < Height; ++Y)
        {
            for (int32 X = 0; X < Width; ++X)
            {
                if (Fits(Items, Item, X, Y, Rotated, Width, Height))
                {
                    FLWItemInstance Placed = Item;
                    LWSetGridPosition(Placed, X, Y, Rotated);
                    Item = Placed;
                    Items.Add(Placed);
                    return true;
                }
            }
        }
    }
    return false;
}

bool LWItems::Move(TArray<FLWItemInstance>& Items, FGuid Id,
    int32 X, int32 Y, bool Rotated, int32 Width, int32 Height)
{
    const int32 Index = LWUniqueIndex(Items, Id);
    if (Index == INDEX_NONE || LWIsReferencedMagazine(Items, Id) ||
        !Fits(Items, Items[Index], X, Y, Rotated, Width, Height, Id))
    {
        return false;
    }
    LWSetGridPosition(Items[Index], X, Y, Rotated);
    return true;
}

bool LWItems::Transfer(TArray<FLWItemInstance>& From, TArray<FLWItemInstance>& To, FGuid Id,
    int32 X, int32 Y, bool Rotated, int32 Width, int32 Height)
{
    if (&From == &To)
    {
        return Move(From, Id, X, Y, Rotated, Width, Height);
    }
    const int32 Index = LWUniqueIndex(From, Id);
    if (Index == INDEX_NONE || LWContainsId(To, Id) || LWIsReferencedMagazine(From, Id))
    {
        return false;
    }
    const FLWItemInstance& Item = From[Index];
    int32 MagazineIndex = INDEX_NONE;
    if (!Fits(To, Item, X, Y, Rotated, Width, Height) || !LWLinkedMagazineIndex(From, Item, MagazineIndex))
    {
        return false;
    }
    if (MagazineIndex != INDEX_NONE &&
        (LWContainsId(To, From[MagazineIndex].Id) || LWIsReferencedMagazine(To, From[MagazineIndex].Id)))
    {
        return false;
    }

    // All validation precedes both arrays' mutations. Copies preserve every ammunition/save field.
    FLWItemInstance Moved = Item;
    LWSetGridPosition(Moved, X, Y, Rotated);
    To.Add(Moved);
    if (MagazineIndex != INDEX_NONE)
    {
        To.Add(From[MagazineIndex]);
        // Remove in descending order so either source ordering (magazine first or gun first) works.
        From.RemoveAt(FMath::Max(Index, MagazineIndex));
        From.RemoveAt(FMath::Min(Index, MagazineIndex));
    }
    else
    {
        From.RemoveAt(Index);
    }
    return true;
}

bool LWItems::CanEquip(const FLWItemInstance& Item, FName Slot)
{
    return LWValidInstance(Item) && Item.Count == 1 && !LWIsLoaded(Item) &&
        LWIsEquipmentSlot(Slot) && (Def(Item.Definition).EquipSlots.Contains(Slot) || (Slot==TEXT("RigPrimary")&&Def(Item.Definition).EquipSlots.Contains(TEXT("Primary"))));
}

bool LWItems::Equip(TArray<FLWItemInstance>& Items, FGuid Id, FName Slot)
{
    if(Slot==TEXT("RigPrimary")&&!Items.ContainsByPredicate([](const auto& I){return I.Slot==TEXT("Rig")&&I.Definition==TEXT("rig");}))return false;
    const int32 Index = LWUniqueIndex(Items, Id);
    if (Index == INDEX_NONE || LWIsReferencedMagazine(Items, Id) || !CanEquip(Items[Index], Slot))
    {
        return false;
    }
    for (int32 OtherIndex = 0; OtherIndex < Items.Num(); ++OtherIndex)
    {
        if (OtherIndex != Index && Items[OtherIndex].Slot == Slot)
        {
            return false;
        }
    }
    Items[Index].Slot = Slot;
    Items[Index].X = Items[Index].Y = -1;
    Items[Index].bRotated = false;
    return true;
}

int32 LWItems::LoadMagazine(FLWItemInstance& Mag, FLWItemInstance& Ammo)
{
    if (&Mag == &Ammo || Mag.Id == Ammo.Id || !LWValidInstance(Mag) || !LWValidInstance(Ammo))
    {
        return 0;
    }
    const FLWItemDefinition& MagDef = Def(Mag.Definition);
    const FLWItemDefinition& AmmoDef = Def(Ammo.Definition);
    if (MagDef.Category != FName(TEXT("Magazine")) || AmmoDef.Category != FName(TEXT("Ammo")) ||
        Mag.Count != 1 || MagDef.AmmoType.IsNone() || MagDef.Capacity <= 0 ||
        LWCanonicalAmmo(MagDef.AmmoType) != LWCanonicalAmmo(Ammo.Definition) ||
        Mag.Rounds < 0 || Mag.Rounds > MagDef.Capacity)
    {
        return 0;
    }
    const int32 Loaded = FMath::Min(MagDef.Capacity - Mag.Rounds, Ammo.Count);
    Mag.Rounds += Loaded;
    Ammo.Count -= Loaded;
    return Loaded;
}

TArray<FLWItemDefinition> LWItems::All47(){LWEnsureCatalog();TArray<FLWItemDefinition> Result;LWCatalogState().Definitions.GenerateValueArray(Result);Result.RemoveAll([](const auto& D){return D.Category==TEXT("WeaponPart");});Result.Sort([](const auto& A,const auto& B){return A.Id.LexicalLess(B.Id);});return Result;}
