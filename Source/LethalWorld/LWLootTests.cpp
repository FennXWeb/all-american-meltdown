#include "LWLootTable.h"
#include "Misc/AutomationTest.h"
#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"

#if WITH_DEV_AUTOMATION_TESTS
#include <limits>

namespace
{
    // Tests use native item rules even when a designer has a local catalog asset installed.
    struct FNativeCatalogScope
    {
        FNativeCatalogScope()
        {
            ULWItemCatalog* Catalog = NewObject<ULWItemCatalog>();
            Catalog->Items = LWItems::Defaults();
            LWItems::SetCatalog(Catalog);
        }
        ~FNativeCatalogScope() { LWItems::SetCatalog(nullptr); }
    };

    bool Identical(const TArray<FLWItemInstance>& A, const TArray<FLWItemInstance>& B)
    {
        if (A.Num() != B.Num())
        {
            return false;
        }
        for (int32 I = 0; I < A.Num(); ++I)
        {
            if (!FLWItemInstance::StaticStruct()->CompareScriptStruct(&A[I], &B[I], 0))
            {
                return false;
            }
        }
        return true;
    }

    FLWLootEntry TestEntry(const TCHAR* Id, float Weight = 1.f, int32 Count = 1)
    {
        FLWLootEntry Entry;
        Entry.ItemId = FName(Id);
        Entry.Weight = Weight;
        Entry.MinCount = Entry.MaxCount = Count;
        return Entry;
    }

    ULWLootTable* SinglePreset(TArray<FLWLootEntry> Entries)
    {
        ULWLootTable* Table = NewObject<ULWLootTable>();
        Table->Presets.Reset();
        Table->RarityMultipliers.Reset();
        FLWLootPreset& Preset = Table->Presets.AddDefaulted_GetRef();
        Preset.Context = FName(TEXT("test"));
        Preset.Entries = MoveTemp(Entries);
        Preset.MinRolls = Preset.MaxRolls = 1;
        Preset.EmptyChance = 0.f;
        Preset.HazardExtraRolls = 0;
        Preset.HazardEmptyReduction = 0.f;
        Preset.HazardCountBonus = Preset.QualityCountBonus = 0.f;
        return Table;
    }

    int32 Units(const TArray<FLWItemInstance>& Items, FName Definition)
    {
        int32 Count = 0;
        for (const FLWItemInstance& Item : Items)
        {
            if (Item.Definition == Definition)
            {
                Count += Item.Count;
            }
        }
        return Count;
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWLootDeterminismTest, "LethalWorld.Loot.DeterminismAndNativeDefaults",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWLootDeterminismTest::RunTest(const FString& Parameters)
{
    FNativeCatalogScope Catalog;
    ULWLootTable* Table = NewObject<ULWLootTable>();
    TestEqual(TEXT("Eight native contexts"), Table->Presets.Num(), 8);
    for (const FLWLootPreset& Preset : Table->Presets)
    {
        for (int32 Seed : {0, 42, -97, MIN_int32, MAX_int32})
        {
            const auto A = Table->Roll(Preset.Context, Seed, 0.6f);
            Table->Roll(FName(TEXT("trader")), Seed + (Seed == MAX_int32 ? -1 : 1));
            const auto B = Table->Roll(Preset.Context, Seed, 0.6f);
            TestTrue(TEXT("Every saved field, GUID and output order are reproducible"), Identical(A, B));
            TSet<FName> Definitions;
            TSet<FGuid> Identities;
            for (const FLWItemInstance& Item : A)
            {
                TestTrue(TEXT("Valid item identity"), Item.Id.IsValid());
                TestFalse(TEXT("No duplicate unique definitions"), Definitions.Contains(Item.Definition));
                TestFalse(TEXT("No duplicate identities"), Identities.Contains(Item.Id));
                TestTrue(TEXT("Counts fit inventory stacks"), Item.Count > 0 && Item.Count <= LWItems::Def(Item.Definition).MaxStack);
                if(Item.Definition==TEXT("gas_can")){TestEqual(TEXT("Gas cans contain their declared fuel capacity"),Item.Rounds,LWItems::Def(Item.Definition).Capacity);TestEqual(TEXT("Fuel is not chambered ammunition"),Item.Chamber,0);}else TestEqual(TEXT("No unaccounted loaded ammunition"), Item.Rounds + Item.Chamber, 0);
                TestFalse(TEXT("No phantom loaded magazines"), Item.LoadedMagazine.IsValid());
                TestFalse(TEXT("Cylinder starts empty"), Item.Cylinder.ContainsByPredicate([](int32 State) { return State != 0; }));
                Definitions.Add(Item.Definition);
                Identities.Add(Item.Id);
            }
        }
        auto CheckEntries = [&](const TArray<FLWLootEntry>& Entries)
        {
            for (const auto& Entry : Entries)
            {
                TestTrue(TEXT("Native weights are nonnegative and finite"), FMath::IsFinite(Entry.Weight) && Entry.Weight >= 0.f);
                TestTrue(TEXT("Native counts valid"), Entry.MinCount > 0 && Entry.MaxCount >= Entry.MinCount);
                TestEqual(TEXT("Native loot definition exists"), LWItems::Def(Entry.ItemId).Id, Entry.ItemId);
            }
        };
        CheckEntries(Preset.Entries);
        for (const auto& Group : Preset.GuaranteedGroups) { CheckEntries(Group.Entries); }
    }
    TestTrue(TEXT("Unknown contexts deterministically use road"),
        Identical(Table->Roll(FName(TEXT("missing")), 123), Table->Roll(FName(TEXT("road")), 123)));
    TestTrue(TEXT("FName casing does not alter loot identity"),
        Identical(Table->Roll(FName(TEXT("TRADER")), 9), Table->Roll(FName(TEXT("trader")), 9)));
    TestFalse(TEXT("Different restock seed creates different identities"),
        Identical(Table->Roll(FName(TEXT("trader")), 9), Table->Roll(FName(TEXT("trader")), 10)));
    Table->Presets.Reset();
    TestEqual(TEXT("Missing fallback yields no loot"), Table->Roll(NAME_None, 1).Num(), 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWLootWeightsTest, "LethalWorld.Loot.WeightsAndInvalidConfiguration",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWLootWeightsTest::RunTest(const FString& Parameters)
{
    FNativeCatalogScope Catalog;
    ULWLootTable* Table = SinglePreset({TestEntry(TEXT("food")), TestEntry(TEXT("water"), 3.f)});
    int32 FoodDraws = 0;
    for (int32 Seed = 0; Seed < 4096; ++Seed)
    {
        const auto Items = Table->Roll(FName(TEXT("test")), Seed);
        if (!TestEqual(TEXT("Exactly one weighted draw"), Items.Num(), 1)) { return false; }
        FoodDraws += Items[0].Definition == FName(TEXT("food")) ? 1 : 0;
    }
    TestTrue(TEXT("Weights 1:3 select first item approximately 25% of the time"), FoodDraws > 819 && FoodDraws < 1229);
    auto& Preset = Table->Presets[0];
    Preset.Entries = {TestEntry(TEXT("food"), -1.f), TestEntry(TEXT("water"), 0.f),
        TestEntry(TEXT("medkit"), std::numeric_limits<float>::quiet_NaN()),
        TestEntry(TEXT("battery"), std::numeric_limits<float>::infinity()),
        TestEntry(TEXT("unknown_item"), 1000.f), TestEntry(TEXT("scrap"), 1.f, 0)};
    TestEqual(TEXT("Disabled and invalid rows cannot produce loot"), Table->Roll(FName(TEXT("test")), 1).Num(), 0);
    Preset.Entries = {TestEntry(TEXT("food"))};
    Table->RarityMultipliers.Add(ELWLootTier::Common, -1.f);
    TestEqual(TEXT("Negative table rarity multiplier disables tier"), Table->Roll(FName(TEXT("test")), 1).Num(), 0);
    Table->RarityMultipliers.Reset();
    Preset.RarityMultipliers.Add(ELWLootTier::Common, 0.f);
    TestEqual(TEXT("Zero context rarity multiplier disables tier"), Table->Roll(FName(TEXT("test")), 1).Num(), 0);
    Preset.RarityMultipliers.Reset();
    Preset.Entries[0].MinCount = 4;
    Preset.Entries[0].MaxCount = 2;
    Preset.MinRolls = 4;
    Preset.MaxRolls = 2;
    const auto Reversed = Table->Roll(FName(TEXT("test")), 7);
    TestEqual(TEXT("Reversed ranges remain usable with uniqueness"), Reversed.Num(), 1);
    if (!Reversed.IsEmpty())
    {
        TestTrue(TEXT("Reversed count bounds normalize and respect MaxStack"),
            Reversed[0].Count >= FMath::Min(2, LWItems::Def(FName(TEXT("food"))).MaxStack)
            && Reversed[0].Count <= 4);
    }
    Preset.MaxItems = 0;
    TestEqual(TEXT("Zero capacity yields no loot"), Table->Roll(FName(TEXT("test")), 1).Num(), 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWLootLimitsTest, "LethalWorld.Loot.GuaranteesUniquenessAndCaps",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWLootLimitsTest::RunTest(const FString& Parameters)
{
    FNativeCatalogScope Catalog;
    ULWLootTable* Table = SinglePreset({TestEntry(TEXT("ammo_12g"), 1.f, 4)});
    auto& Preset = Table->Presets[0];
    Preset.MinRolls = Preset.MaxRolls = 128;
    Preset.bUniqueItems = false;
    Preset.Entries[0].MaxPerContainer = 10;
    const auto Capped = Table->Roll(FName(TEXT("test")), 3);
    TestEqual(TEXT("Repeated stack counts stop exactly at cap"), Units(Capped, FName(TEXT("ammo_12g"))), 10);
    TestEqual(TEXT("Final draw receives remaining units only"), Capped.Num(), 3);
    Preset.bUniqueItems = true;
    TestEqual(TEXT("Unique context exhausts one ID after one draw"), Table->Roll(FName(TEXT("test")), 3).Num(), 1);
    Preset.bUniqueItems = false;
    Preset.Entries[0].bUnique = true;
    Preset.Entries.Add(TestEntry(TEXT("ammo_12g"), 100.f, 4));
    TestEqual(TEXT("Unique row constrains duplicate definitions in other rows"), Table->Roll(FName(TEXT("test")), 3).Num(), 1);
    Preset.Entries[0].bUnique = false;
    FLWLootGroup Group;
    Group.Name = FName(TEXT("guarantee"));
    Group.Entries = {TestEntry(TEXT("ammo_12g"), 1.f, 2)};
    Group.Entries[0].MaxPerContainer = 5;
    Preset.GuaranteedGroups = {Group};
    TestEqual(TEXT("Strictest quantity cap spans all pools"),
        Units(Table->Roll(FName(TEXT("test")), 3), FName(TEXT("ammo_12g"))), 5);
    Preset.MaxItems = 1;
    const auto First = Table->Roll(FName(TEXT("test")), 3);
    TestEqual(TEXT("Guarantees consume container capacity"), First.Num(), 1);
    if (!First.IsEmpty()) { TestEqual(TEXT("Guaranteed pool draws first"), First[0].Count, 2); }
    Preset.MinRolls = Preset.MaxRolls = 0;
    TestEqual(TEXT("Guarantees work with zero regular rolls"), Table->Roll(FName(TEXT("test")), 3).Num(), 1);
    Preset.Entries.Reset();
    Preset.GuaranteedGroups[0].Entries[0].Weight = 0.f;
    TestEqual(TEXT("Impossible guarantees terminate without fabricating loot"), Table->Roll(FName(TEXT("test")), 3).Num(), 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWLootScalingTest, "LethalWorld.Loot.EmptyChanceQualityAndHazard",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWLootScalingTest::RunTest(const FString& Parameters)
{
    FNativeCatalogScope Catalog;
    ULWLootTable* Table = SinglePreset({TestEntry(TEXT("ammo_12g"), 1.f, 4)});
    auto& Preset = Table->Presets[0];
    FLWLootGroup Group;
    Group.Entries = {TestEntry(TEXT("food"))};
    Preset.GuaranteedGroups = {Group};
    Preset.EmptyChance = 1.f;
    Preset.HazardEmptyReduction = 1.f;
    for (int32 Seed = 0; Seed < 64; ++Seed)
    {
        TestEqual(TEXT("Certain empty overrides guarantees even at full hazard"), Table->Roll(FName(TEXT("test")), Seed, 1.f).Num(), 0);
    }
    Preset.EmptyChance = 0.f;
    TestEqual(TEXT("Zero empty chance keeps both draws"), Table->Roll(FName(TEXT("test")), 1).Num(), 2);
    Preset.GuaranteedGroups.Reset();
    Preset.EmptyChance = 0.5f;
    int32 Empty = 0;
    for (int32 Seed = 0; Seed < 4096; ++Seed)
    {
        Empty += Table->Roll(FName(TEXT("test")), Seed).IsEmpty() ? 1 : 0;
    }
    TestTrue(TEXT("Half empty chance has expected frequency"), Empty > 1638 && Empty < 2458);
    Preset.EmptyChance = 0.f;
    Preset.HazardCountBonus = 1.f;
    TestEqual(TEXT("Hazard scales quantities"), Units(Table->Roll(FName(TEXT("test")), 1, 1.f), FName(TEXT("ammo_12g"))), 8);
    TestTrue(TEXT("Negative hazard clamps to zero"), Identical(Table->Roll(FName(TEXT("test")), 1, -20.f), Table->Roll(FName(TEXT("test")), 1)));
    TestTrue(TEXT("Large hazard clamps to one"), Identical(Table->Roll(FName(TEXT("test")), 1, 99.f), Table->Roll(FName(TEXT("test")), 1, 1.f)));
    TestTrue(TEXT("Non-finite hazard acts as zero"), Identical(Table->Roll(FName(TEXT("test")), 1,
        std::numeric_limits<float>::quiet_NaN()), Table->Roll(FName(TEXT("test")), 1)));
    Preset.bUniqueItems = false;
    Preset.HazardExtraRolls = 2;
    TestEqual(TEXT("Hazard adds configured rolls"), Table->Roll(FName(TEXT("test")), 1, 1.f).Num(), 3);
    Preset.HazardExtraRolls = 0;
    Preset.Quality = 1.f;
    Preset.QualityCountBonus = 1.f;
    TestEqual(TEXT("Quality scales quantities"), Units(Table->Roll(FName(TEXT("test")), 1), FName(TEXT("ammo_12g"))), 8);

    Preset.Entries = {TestEntry(TEXT("food")), TestEntry(TEXT("sniper"))};
    Preset.Entries[1].Tier = ELWLootTier::Epic;
    Preset.Quality = 0.f;
    int32 LowRare = 0, HazardRare = 0, QualityRare = 0;
    for (int32 Seed = 0; Seed < 1024; ++Seed)
    {
        Preset.Quality = 0.f;
        LowRare += Units(Table->Roll(FName(TEXT("test")), Seed), FName(TEXT("sniper")));
        HazardRare += Units(Table->Roll(FName(TEXT("test")), Seed, 1.f), FName(TEXT("sniper")));
        Preset.Quality = 1.f;
        QualityRare += Units(Table->Roll(FName(TEXT("test")), Seed), FName(TEXT("sniper")));
    }
    TestTrue(TEXT("Hazard favors higher tiers"), HazardRare > LowRare + 200);
    TestTrue(TEXT("Quality favors higher tiers"), QualityRare > LowRare + 200);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWLootTraderTest, "LethalWorld.Loot.ContextPresetsAndTraderStock",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWLootTraderTest::RunTest(const FString& Parameters)
{
    FNativeCatalogScope Catalog;
    ULWLootTable* Table = NewObject<ULWLootTable>();
    int32 MilitaryAmmo = 0, DinerAmmo = 0, MilitaryGuns = 0, DinerGuns = 0;
    for (int32 Seed = 0; Seed < 128; ++Seed)
    {
        const auto Stock = Table->Roll(FName(TEXT("trader")), Seed);
        TestTrue(TEXT("Trader always has substantial stock"), Stock.Num() >= 19);
        for (const TCHAR* Id : {TEXT("ammo_12g"), TEXT("ammo_357"), TEXT("ammo_762"),
             TEXT("ammo_9mm"), TEXT("ammo_556"), TEXT("medkit"), TEXT("food"), TEXT("water")})
        {
            TestTrue(TEXT("Trader essential is guaranteed"), Units(Stock, FName(Id)) > 0);
        }
        for (const TCHAR* Context : {TEXT("military"), TEXT("diner")})
        {
            const bool bMilitary = FName(Context) == FName(TEXT("military"));
            for (const auto& Item : Table->Roll(FName(Context), Seed))
            {
                if (Item.Definition.ToString().StartsWith(TEXT("ammo_")))
                {
                    (bMilitary ? MilitaryAmmo : DinerAmmo) += Item.Count;
                }
                const int32 WeaponIndex = LWItems::Def(Item.Definition).WeaponIndex;
                if (WeaponIndex >= 2) { (bMilitary ? MilitaryGuns : DinerGuns)++; }
            }
        }
    }
    TestTrue(TEXT("Military presets yield more ammunition than diners"), MilitaryAmmo > DinerAmmo);
    TestTrue(TEXT("Military presets yield more firearms than diners"), MilitaryGuns > DinerGuns);
    return true;
}
#endif
