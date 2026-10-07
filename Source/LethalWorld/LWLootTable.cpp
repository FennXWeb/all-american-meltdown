#include "LWWeaponMods.h"
#include "Misc/Crc.h"
#include "LWWeaponMods.h"
#include "LWLootTable.h"

#include "Math/RandomStream.h"
#include "Misc/PackageName.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/UObjectGlobals.h"

namespace
{
    constexpr int32 MaxDraws = 128;
    constexpr int32 MaxUnits = 10000;

    float FiniteClamp(float Value, float Low, float High)
    {
        return FMath::IsFinite(Value) ? FMath::Clamp(Value, Low, High) : Low;
    }

    double Multiplier(const TMap<ELWLootTier, float>& Values, ELWLootTier Tier)
    {
        const float* Value = Values.Find(Tier);
        return Value ? FiniteClamp(*Value, 0.f, 1000000.f) : 1.0;
    }

    double WeightOf(const FLWLootEntry& Entry, const ULWLootTable& Table,
                    const FLWLootPreset& Preset, float Hazard)
    {
        if (Entry.ItemId.IsNone() || Entry.MaxCount <= 0 || uint8(Entry.Tier) > uint8(ELWLootTier::Epic))
        {
            return 0.0;
        }
        const FLWItemDefinition& Definition = LWItems::Def(Entry.ItemId);
        if (Definition.Id != Entry.ItemId || Definition.MaxStack <= 0)
        {
            return 0.0;
        }
        double Weight = FiniteClamp(Entry.Weight, 0.f, 1000000.f)
            * Multiplier(Table.RarityMultipliers, Entry.Tier) * Multiplier(Preset.RarityMultipliers, Entry.Tier);
        const double Boost = 1.0 + FiniteClamp(Preset.Quality, 0.f, 1.f)
            * FiniteClamp(Preset.QualityRarityBoost, 0.f, 10.f)
            + Hazard * FiniteClamp(Preset.HazardRarityBoost, 0.f, 10.f);
        for (uint8 Tier = 0; Tier < uint8(Entry.Tier); ++Tier)
        {
            Weight *= Boost;
        }
        return Weight;
    }

    FLWLootEntry Entry(const TCHAR* Id, float Weight, int32 Min = 1, int32 Max = 1,
                      ELWLootTier Tier = ELWLootTier::Common, int32 Cap = 0)
    {
        FLWLootEntry Result;
        Result.ItemId = FName(Id);
        Result.Weight = Weight;
        Result.MinCount = Min;
        Result.MaxCount = Max;
        Result.Tier = Tier;
        Result.bUnique = Cap == 1;
        Result.MaxPerContainer = Cap;
        return Result;
    }

    FLWLootGroup Group(const TCHAR* Name, int32 Rolls, TArray<FLWLootEntry> Entries)
    {
        FLWLootGroup Result;
        Result.Name = FName(Name);
        Result.Rolls = Rolls;
        Result.Entries = MoveTemp(Entries);
        return Result;
    }
}

ULWLootTable::ULWLootTable()
{
    ResetToNativeDefaults();
}

void ULWLootTable::ResetToNativeDefaults()
{
    Presets.Reset();
    DefaultContext = FName(TEXT("road"));
    RarityMultipliers = {{ELWLootTier::Common, 1.f}, {ELWLootTier::Uncommon, 0.8f},
                         {ELWLootTier::Rare, 0.45f}, {ELWLootTier::Epic, 0.18f}};
    using Tier = ELWLootTier;
    const auto Crowbar = Entry(TEXT("crowbar"), 8, 1, 1, Tier::Common, 1);
    const auto Bat = Entry(TEXT("bat"), 8, 1, 1, Tier::Common, 1);
    const auto Shotgun = Entry(TEXT("shotgun"), 4, 1, 1, Tier::Uncommon, 1);
    const auto Revolver = Entry(TEXT("revolver"), 4, 1, 1, Tier::Uncommon, 1);
    const auto Sniper = Entry(TEXT("sniper"), 1, 1, 1, Tier::Epic, 1);
    const auto SMG = Entry(TEXT("smg"), 3, 1, 1, Tier::Rare, 1);
    const auto Rifle = Entry(TEXT("rifle"), 3, 1, 1, Tier::Rare, 1);
    const auto LMG = Entry(TEXT("lmg"), 1, 1, 1, Tier::Epic, 1);
    const auto Shells = Entry(TEXT("ammo_12g"), 14, 4, 12, Tier::Common, 36);
    const auto Magnum = Entry(TEXT("ammo_357"), 12, 6, 18, Tier::Common, 48);
    const auto NATO = Entry(TEXT("ammo_762"), 8, 8, 20, Tier::Uncommon, 60);
    const auto NineMM = Entry(TEXT("ammo_9mm"), 14, 12, 30, Tier::Common, 90);
    const auto FiveFiveSix = Entry(TEXT("ammo_556"), 10, 12, 30, Tier::Uncommon, 90);
    const auto SniperMag = Entry(TEXT("mag_sniper5"), 4, 1, 1, Tier::Rare, 1);
    const auto SMGMag = Entry(TEXT("mag_smg30"), 6, 1, 1, Tier::Uncommon, 1);
    const auto RifleMag = Entry(TEXT("mag_rifle30"), 6, 1, 1, Tier::Uncommon, 1);
    const auto LMGMag = Entry(TEXT("mag_lmg100"), 2, 1, 1, Tier::Rare, 1);
    const auto Armor = Entry(TEXT("armor"), 4, 1, 1, Tier::Rare, 1);
    const auto Helmet = Entry(TEXT("helmet"), 6, 1, 1, Tier::Uncommon, 1);
    const auto Rig = Entry(TEXT("rig"), 5, 1, 1, Tier::Uncommon, 1);
    const auto Backpack = Entry(TEXT("backpack"), 6, 1, 1, Tier::Uncommon, 1);
    const auto Medkit = Entry(TEXT("medkit"), 12, 1, 2, Tier::Uncommon, 5);
    const auto Food = Entry(TEXT("food"), 24, 1, 3, Tier::Common, 8);
    const auto Water = Entry(TEXT("water"), 24, 1, 3, Tier::Common, 8);
    const auto Scrap = Entry(TEXT("scrap"), 40, 8, 20, Tier::Common, 100);
    const auto Battery = Entry(TEXT("battery"), 14, 1, 2, Tier::Common, 6);

    auto Add = [this](const TCHAR* Context, int32 Min, int32 Max, float Empty,
                     TArray<FLWLootEntry> Entries) -> FLWLootPreset&
    {
        FLWLootPreset& Preset = Presets.AddDefaulted_GetRef();
        Preset.Context = FName(Context);
        Preset.MinRolls = Min;
        Preset.MaxRolls = Max;
        Preset.EmptyChance = Empty;
        Preset.Entries = MoveTemp(Entries);
        return Preset;
    };

    Add(TEXT("gas"), 2, 4, 0.12f, {Food, Water, Scrap, Battery, Crowbar, Shells, Magnum, Shotgun, Revolver});
    Add(TEXT("motel"), 2, 4, 0.18f, {Food, Water, Medkit, Battery, Bat, Backpack, Magnum, Revolver, NineMM});
    auto& Clinic = Add(TEXT("clinic"), 1, 3, 0.08f, {Medkit, Water, Food, Battery, Scrap});
    Clinic.GuaranteedGroups.Add(Group(TEXT("medical"), 1, {Medkit}));
    auto& Depot = Add(TEXT("depot"), 2, 5, 0.12f,
        {Scrap, Battery, Crowbar, Rig, Backpack, Shells, NATO, NineMM, FiveFiveSix,
         SMGMag, RifleMag, SniperMag, LMGMag, Shotgun, SMG, Rifle, Sniper, LMG});
    Depot.Quality = 0.15f;
    Depot.GuaranteedGroups.Add(Group(TEXT("workshop"), 1, {Scrap, Battery}));
    auto& Diner = Add(TEXT("diner"), 1, 3, 0.08f, {Food, Water, Battery, Scrap, Medkit, Bat});
    Diner.GuaranteedGroups.Add(Group(TEXT("provisions"), 1, {Food, Water}));
    Add(TEXT("road"), 1, 3, 0.30f, {Scrap, Battery, Water, Food, Bat, Crowbar, Shells, Magnum, Medkit});
    auto& Military = Add(TEXT("military"), 3, 6, 0.05f,
        {Shells, Magnum, NATO, NineMM, FiveFiveSix, SniperMag, SMGMag, RifleMag, LMGMag,
         Armor, Helmet, Rig, Backpack, Medkit, Shotgun, Revolver, SMG, Rifle, Sniper, LMG});
    Military.Quality = 0.5f;
    Military.GuaranteedGroups.Add(Group(TEXT("ammunition"), 1, {NATO, NineMM, FiveFiveSix}));
    Military.GuaranteedGroups.Add(Group(TEXT("field_supplies"), 1, {Medkit, Armor, Helmet, Rig}));
    auto& Trader = Add(TEXT("trader"), 4, 8, 0.f,
        {Crowbar, Bat, Shotgun, Revolver, Sniper, SMG, Rifle, LMG, Shells, Magnum, NATO, NineMM,
         FiveFiveSix, SniperMag, SMGMag, RifleMag, LMGMag, Armor, Helmet, Rig, Backpack,
         Medkit, Food, Water, Scrap, Battery});
    for(const TCHAR* Id:{TEXT("att_light"),TEXT("att_laser"),TEXT("att_reflex"),TEXT("att_holo"),TEXT("att_scope4"),TEXT("att_scope8"),TEXT("att_vertical"),TEXT("att_angled")})for(auto& P:Presets)if(P.Context==TEXT("depot")||P.Context==TEXT("military")||P.Context==TEXT("trader"))P.Entries.Add(Entry(Id,4,1,1,Tier::Uncommon,1));
    for(auto& P:Presets)if(P.Context==TEXT("gas")||P.Context==TEXT("depot")||P.Context==TEXT("trader")||P.Context==TEXT("motel"))P.Entries.Add(Entry(TEXT("doublebarrel"),3,1,1,Tier::Uncommon,1));
    for(auto& P:Presets)if(P.Context==TEXT("military")||P.Context==TEXT("depot")||P.Context==TEXT("trader")){
        for(const TCHAR* Id:{TEXT("missile_launcher"),TEXT("minigun"),TEXT("sawedoff"),TEXT("desert_eagle"),TEXT("m4"),TEXT("taser"),TEXT("flamethrower"),TEXT("tenbarrel"),TEXT("giant_glock"),TEXT("questionable_ak"),TEXT("finger_guns"),TEXT("budget_cut")})P.Entries.Add(Entry(Id,2,1,1,Tier::Rare,1));
        for(const TCHAR* Id:{TEXT("ammo_rocket"),TEXT("ammo_30mm"),TEXT("ammo_50ae"),TEXT("battery"),TEXT("ammo_fuel")})P.Entries.Add(Entry(Id,8,1,4,Tier::Common,1));
        for(const TCHAR* Id:{TEXT("minigun_box"),TEXT("mag_deagle7"),TEXT("fuel_tank"),TEXT("mag_giant50"),TEXT("mag_ak75")})P.Entries.Add(Entry(Id,5,1,1,Tier::Uncommon,1));
    }
    for(auto& P:Presets){
        if(P.Context==TEXT("motel")||P.Context==TEXT("road")||P.Context==TEXT("depot")||P.Context==TEXT("trader")){P.Entries.Add(Entry(TEXT("sling_pack"),8,1,1,Tier::Common,1));P.Entries.Add(Entry(TEXT("hiking_pack"),4,1,1,Tier::Uncommon,1));}
        if(P.Context==TEXT("military")||P.Context==TEXT("depot")||P.Context==TEXT("trader")){P.Entries.Add(Entry(TEXT("military_pack"),4,1,1,Tier::Rare,1));P.Entries.Add(Entry(TEXT("expedition_pack"),2,1,1,Tier::Rare,1));P.Entries.Add(Entry(TEXT("night_vision"),2,1,1,Tier::Rare,1));}
    }
    for(auto& P:Presets)if(P.Context==TEXT("gas")||P.Context==TEXT("depot")||P.Context==TEXT("trader"))P.Entries.Add(Entry(TEXT("gas_can"),P.Context==TEXT("gas")?18:8,1,1,Tier::Common,1));
    for(auto& P:Presets)if(P.Context==TEXT("road")||P.Context==TEXT("depot")||P.Context==TEXT("military")||P.Context==TEXT("motel"))P.Entries.Add(Entry(TEXT("settlement_flag"),3,1,1,Tier::Uncommon,1));
    Trader.GuaranteedGroups.Add(Group(TEXT("settlement82"),1,{Entry(TEXT("settlement_flag"),1,1,1,Tier::Common,1)}));
    Trader.GuaranteedGroups.Add(Group(TEXT("building82"),1,{Entry(TEXT("scrap"),1,40,80,Tier::Common,160)}));
    Trader.MaxItems = 28;
    Trader.Quality = 0.75f;
    Trader.HazardExtraRolls = 0;
    Trader.GuaranteedGroups.Add(Group(TEXT("essentials"), 8,
        {Shells, Magnum, NATO, NineMM, FiveFiveSix, Medkit, Food, Water}));
    Trader.GuaranteedGroups.Add(Group(TEXT("melee"), 1, {Crowbar, Bat}));
    Trader.GuaranteedGroups.Add(Group(TEXT("firearms"), 2, {Shotgun, Revolver, SMG, Rifle, Sniper, LMG}));
    Trader.GuaranteedGroups.Add(Group(TEXT("equipment"), 2, {Armor, Helmet, Rig, Backpack}));
    Trader.GuaranteedGroups.Add(Group(TEXT("magazines"), 2, {SniperMag, SMGMag, RifleMag, LMGMag}));
}

TArray<FLWItemInstance> ULWLootTable::Roll(FName Context, int32 Seed, float Hazard) const
{
    TArray<FLWItemInstance> Result;
    const FLWLootPreset* Preset = Presets.FindByPredicate(
        [Context](const FLWLootPreset& Value) { return Value.Context == Context; });
    if (!Preset)
    {
        Preset = Presets.FindByPredicate(
            [this](const FLWLootPreset& Value) { return Value.Context == DefaultContext; });
    }
    if (!Preset || Preset->MaxItems <= 0)
    {
        return Result;
    }
    Hazard = FiniteClamp(Hazard, 0.f, 1.f);
    FRandomStream Random(Seed);
    const float EmptyChance = FiniteClamp(Preset->EmptyChance, 0.f, 1.f);
    const float EffectiveEmpty = EmptyChance * (1.f - Hazard * FiniteClamp(Preset->HazardEmptyReduction, 0.f, 1.f));
    if (EmptyChance >= 1.f || Random.FRand() < EffectiveEmpty)
    {
        return Result;
    }

    // Maps are lookup-only: selection and output order always follow the authored arrays.
    TMap<FName, int32> UsedUnits;
    TMap<FName, int32> UnitCaps;
    TSet<FName> UniqueIds;
    auto CollectLimits = [&](const TArray<FLWLootEntry>& Entries)
    {
        for (const FLWLootEntry& Value : Entries)
        {
            if (WeightOf(Value, *this, *Preset, Hazard) <= 0.0)
            {
                continue;
            }
            if (Value.bUnique)
            {
                UniqueIds.Add(Value.ItemId);
            }
            if (Value.MaxPerContainer > 0)
            {
                int32& Cap = UnitCaps.FindOrAdd(Value.ItemId, MaxUnits);
                Cap = FMath::Min(Cap, FMath::Clamp(Value.MaxPerContainer, 1, MaxUnits));
            }
        }
    };
    CollectLimits(Preset->Entries);
    for (const FLWLootGroup& Guaranteed : Preset->GuaranteedGroups)
    {
        if (Guaranteed.Rolls > 0)
        {
            CollectLimits(Guaranteed.Entries);
        }
    }

    const int32 Capacity = FMath::Clamp(Preset->MaxItems, 0, MaxDraws);
    const float CountScale = 1.f + FiniteClamp(Preset->Quality, 0.f, 1.f)
        * FiniteClamp(Preset->QualityCountBonus, 0.f, 10.f)
        + Hazard * FiniteClamp(Preset->HazardCountBonus, 0.f, 10.f);
    auto Remaining = [&](const FLWLootEntry& Value)
    {
        if(LWItems::Def(Value.ItemId).Category==TEXT("WeaponPart"))return 0;
        const int32 Used = UsedUnits.FindRef(Value.ItemId);
        if (Used > 0 && (Preset->bUniqueItems || UniqueIds.Contains(Value.ItemId)))
        {
            return 0;
        }
        const int32* Cap = UnitCaps.Find(Value.ItemId);
        return FMath::Max(0, (Cap ? *Cap : MaxUnits) - Used);
    };
    auto Draw = [&](const TArray<FLWLootEntry>& Entries) -> bool
    {
        if (Result.Num() >= Capacity)
        {
            return false;
        }
        double Total = 0.0;
        for (const FLWLootEntry& Value : Entries)
        {
            if (Remaining(Value) > 0)
            {
                Total += WeightOf(Value, *this, *Preset, Hazard);
            }
        }
        if (Total <= 0.0)
        {
            return false;
        }
        double Ticket = double(Random.FRand()) * Total;
        const FLWLootEntry* Selected = nullptr;
        for (const FLWLootEntry& Value : Entries)
        {
            const double Weight = Remaining(Value) > 0 ? WeightOf(Value, *this, *Preset, Hazard) : 0.0;
            if (Weight <= 0.0)
            {
                continue;
            }
            Selected = &Value; // Safe final positive candidate if subtraction rounds at the boundary.
            if (Ticket < Weight)
            {
                break;
            }
            Ticket -= Weight;
        }
        if (!Selected)
        {
            return false;
        }
        const int32 A = FMath::Clamp(Selected->MinCount, 1, MaxUnits);
        const int32 B = FMath::Clamp(Selected->MaxCount, 1, MaxUnits);
        const int32 BaseCount = Random.RandRange(FMath::Min(A, B), FMath::Max(A, B));
        const int32 Limit = FMath::Min(Remaining(*Selected), LWItems::Def(Selected->ItemId).MaxStack);
        const int32 Count = FMath::Clamp(FMath::RoundToInt(BaseCount * CountScale), 1, Limit);
        FName SpawnId=Selected->ItemId; if(SpawnId==TEXT("rocket_tube"))SpawnId=TEXT("ammo_rocket");if(SpawnId==TEXT("taser_cartridge")||SpawnId==TEXT("ammo_dart"))SpawnId=TEXT("battery");
        FLWItemInstance Item = LWItems::Make(SpawnId, FMath::Min(Count,LWItems::Def(SpawnId).MaxStack));
        if(LWItems::Def(Item.Definition).Category==TEXT("Weapon")||LWItems::Def(Item.Definition).Category==TEXT("WeaponPart")){FRandomStream QR(Seed^FCrc::StrCrc32(*Selected->ItemId.ToString()));float Q=QR.FRand()+Hazard*.12f;Item.WeaponTier=Q>.995?4:Q>.94?3:Q>.80?2:Q>.55?1:0;LWMods::RollFinish(Item,Random);}
        if (!Item.Id.IsValid())
        {
            return false;
        }
        // Make assigns random identities. Replace them so save/reload and repeated generation
        // reproduce the ENTIRE instance. Callers must supply a distinct seed per container/restock.
        const FString Identity = FString::Printf(TEXT("LWLoot/v2/%s/%s/%d"),
            *Preset->Context.ToString().ToLower(), *Selected->ItemId.ToString().ToLower(), Result.Num());
        Item.Id = FGuid::NewDeterministicGuid(Identity, uint64(uint32(Seed)));
        Result.Add(MoveTemp(Item));
        UsedUnits.FindOrAdd(Selected->ItemId) += Count;
        return true;
    };

    for (const FLWLootGroup& Guaranteed : Preset->GuaranteedGroups)
    {
        for (int32 I = 0, Num = FMath::Clamp(Guaranteed.Rolls, 0, MaxDraws); I < Num; ++I)
        {
            if (!Draw(Guaranteed.Entries))
            {
                break;
            }
        }
    }
    TArray<FLWLootEntry> Pool62=Preset->Entries;
    if(Context==TEXT("depot")||Context==TEXT("military")||Context==TEXT("trader"))for(const auto& D:LWArsenal62::Attachments())Pool62.Add(Entry(*D.Id.ToString(),2.f,1,1,ELWLootTier::Uncommon,1));
    const int32 A = FMath::Clamp(Preset->MinRolls, 0, MaxDraws);
    const int32 B = FMath::Clamp(Preset->MaxRolls, 0, MaxDraws);
    const int32 NumRolls = FMath::Min(MaxDraws, Random.RandRange(FMath::Min(A, B), FMath::Max(A, B))
        + FMath::FloorToInt(Hazard * FMath::Clamp(Preset->HazardExtraRolls, 0, MaxDraws)));
    for (int32 I = 0; I < NumRolls; ++I)
    {
        if (!Draw(Pool62))
        {
            break;
        }
    }
    return Result;
}

const ULWLootTable* LWLoot::Get()
{
    static TWeakObjectPtr<ULWLootTable> Cached;
    if (!Cached.IsValid())
    {
        // ResolveObject avoids repeated loads; the existence check avoids warnings when unseeded.
        const FSoftObjectPath Path(TEXT("/Game/Data/DA_LootTable.DA_LootTable"));
        Cached = Cast<ULWLootTable>(Path.ResolveObject());
        if (!Cached.IsValid() && FPackageName::DoesPackageExist(Path.GetLongPackageName()))
        {
            Cached = Cast<ULWLootTable>(Path.TryLoad());
        }
    }
    return Cached.IsValid() ? Cached.Get() : GetDefault<ULWLootTable>();
}

TArray<FLWItemInstance> LWLoot::Roll(FName Context, int32 Seed, float Hazard)
{
    return Get()->Roll(Context, Seed, Hazard);
}