#include "LWInventory.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UObject/Class.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"

namespace
{
    // No disk assets are required, and no test leaves its temporary overrides installed.
    struct FLWInventoryTestCatalog
    {
        TStrongObjectPtr<ULWItemCatalog> Catalog;

        FLWInventoryTestCatalog() : Catalog(NewObject<ULWItemCatalog>())
        {
            LWItems::SetCatalog(Catalog.Get());
        }

        ~FLWInventoryTestCatalog()
        {
            LWItems::SetCatalog(nullptr);
        }

        void Add(const FLWItemDefinition& Definition)
        {
            Catalog->Items.Add(Definition);
            LWItems::SetCatalog(Catalog.Get());
        }
    };

    TArray<uint8> LWInventorySnapshot(const TArray<FLWItemInstance>& Items)
    {
        TArray<uint8> Bytes;
        FMemoryWriter Writer(Bytes, true);
        FObjectAndNameAsStringProxyArchive Archive(Writer, false);
        Archive.ArIsSaveGame = true;
        int32 Count = Items.Num();
        Archive << Count;
        for (FLWItemInstance Copy : Items)
        {
            FLWItemInstance::StaticStruct()->SerializeItem(Archive, &Copy, nullptr);
        }
        return Bytes;
    }

    FLWItemInstance LWTestItem(FName Definition, int32 X = 0, int32 Y = 0)
    {
        FLWItemInstance Item = LWItems::Make(Definition);
        Item.X = X;
        Item.Y = Y;
        return Item;
    }

    FLWItemDefinition LWTestLShape()
    {
        FLWItemDefinition Shape;
        Shape.Id = TEXT("test_l_shape");
        Shape.Width = 2;
        Shape.Height = 3;
        Shape.Cells = { FIntPoint(0, 0), FIntPoint(0, 1), FIntPoint(0, 2), FIntPoint(1, 2) };
        return Shape;
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWInventoryCatalogTest, "LethalWorld.Inventory.CatalogAndFactories",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWInventoryCatalogTest::RunTest(const FString& Parameters)
{
    (void)Parameters;
    FLWInventoryTestCatalog Scope;
    TestEqual(TEXT("New editable catalog is seeded with defaults"), Scope.Catalog->Items.Num(), LWItems::Defaults().Num());
    const TArray<FName> WeaponIds = {
        TEXT("crowbar"), TEXT("bat"), TEXT("shotgun"), TEXT("revolver"),
        TEXT("sniper"), TEXT("smg"), TEXT("rifle"), TEXT("lmg")
    };
    for (int32 Index = 0; Index < WeaponIds.Num(); ++Index)
    {
        TestEqual(TEXT("Stable weapon index"), LWItems::Def(WeaponIds[Index]).WeaponIndex, Index);
        const FLWItemInstance Weapon = LWItems::Make(WeaponIds[Index]);
        TestTrue(TEXT("New weapon has valid identity"), Weapon.Id.IsValid());
        TestEqual(TEXT("New weapon does not create tube rounds"), Weapon.Rounds, 0);
        TestEqual(TEXT("New weapon has an empty chamber"), Weapon.Chamber, 0);
        TestFalse(TEXT("New weapon does not invent a magazine"), Weapon.LoadedMagazine.IsValid());
    }
    TSet<FName> Seen;
    for (const FLWItemDefinition& D : LWItems::Defaults())
    {
        TestFalse(TEXT("Default IDs are unique"), Seen.Contains(D.Id));
        Seen.Add(D.Id);
        TestEqual(TEXT("Default IDs use lower case"), D.Id.ToString(), D.Id.ToString().ToLower());
        TestTrue(TEXT("Defaults have usable footprints"), !LWItems::Footprint(D.Id).IsEmpty());
        TestTrue(TEXT("Default prices are nonnegative"), D.Price >= 0);
    }
    TestEqual(TEXT("LMG shares sniper loose ammunition"), LWItems::Def(TEXT("lmg")).AmmoType, LWItems::Def(TEXT("sniper")).AmmoType);
    TestEqual(TEXT("LMG uses detachable belt box"), LWItems::Def(TEXT("lmg")).MagazineType, FName(TEXT("mag_lmg100")));
    TestEqual(TEXT("Bag has requested internal width"), LWItems::Def(TEXT("backpack")).ContainerWidth, 12);
    TestEqual(TEXT("Bag has requested internal height"), LWItems::Def(TEXT("backpack")).ContainerHeight, 24);
    TestTrue(TEXT("Scrap default is available to loot"), LWItems::Make(TEXT("scrap")).Id.IsValid());
    TestTrue(TEXT("Battery default is available to loot"), LWItems::Make(TEXT("battery")).Id.IsValid());

    const FLWItemInstance Revolver = LWItems::Make(TEXT("revolver"));
    TestEqual(TEXT("Revolver has six chambers"), Revolver.Cylinder.Num(), 6);
    TestEqual(TEXT("Initial cylinder index"), Revolver.CylinderIndex, 0);
    for (int32 State : Revolver.Cylinder)
    {
        TestEqual(TEXT("All revolver chambers start empty"), State, 0);
    }
    TestTrue(TEXT("Each Make has a distinct identity"), Revolver.Id != LWItems::Make(TEXT("revolver")).Id);
    TestTrue(TEXT("Unknown definition is inert"), LWItems::Def(TEXT("missing_item")).Id.IsNone());
    TestFalse(TEXT("Unknown items cannot be made"), LWItems::Make(TEXT("missing_item")).Id.IsValid());
    TestFalse(TEXT("Zero count cannot be made"), LWItems::Make(TEXT("food"), 0).Id.IsValid());
    TestFalse(TEXT("Negative count cannot be made"), LWItems::Make(TEXT("food"), -1).Id.IsValid());
    TestFalse(TEXT("Oversized request is rejected instead of losing excess"), LWItems::Make(TEXT("food"), 6).Id.IsValid());

    FLWItemDefinition Override = LWItems::Def(TEXT("rifle"));
    Override.Price = 777;
    Scope.Add(Override);
    TestEqual(TEXT("Valid catalog override wins"), LWItems::Def(TEXT("rifle")).Price, 777);
    Scope.Catalog->Items.Last().Price = 999;
    TestEqual(TEXT("Catalog contents are copied, not kept as dangling references"), LWItems::Def(TEXT("rifle")).Price, 777);
    TestTrue(TEXT("Overrides retain absent defaults"), !LWItems::Def(TEXT("water")).Id.IsNone());

    FLWItemDefinition Invalid = LWTestLShape();
    Invalid.Id = TEXT("duplicate_cells");
    const FIntPoint DuplicateCell = Invalid.Cells[0];
    Invalid.Cells.Add(DuplicateCell);
    Scope.Add(Invalid);
    TestTrue(TEXT("Duplicate occupied cells invalidate a definition"), LWItems::Def(Invalid.Id).Id.IsNone());
    Invalid = LWTestLShape();
    Invalid.Id = TEXT("outside_cells");
    Invalid.Cells.Add(FIntPoint(2, 0));
    Scope.Add(Invalid);
    TestTrue(TEXT("Out-of-bounds cells invalidate a definition"), LWItems::Def(Invalid.Id).Id.IsNone());
    Invalid = LWTestLShape();
    Invalid.Id = TEXT("invalid_slot");
    Invalid.EquipSlots.Add(TEXT("Loaded"));
    Scope.Add(Invalid);
    TestTrue(TEXT("Reserved loaded slot is not an equipment slot"), LWItems::Def(Invalid.Id).Id.IsNone());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWInventoryShapesTest, "LethalWorld.Inventory.ShapesRotationAndEdges",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWInventoryShapesTest::RunTest(const FString& Parameters)
{
    (void)Parameters;
    FLWInventoryTestCatalog Scope;
    Scope.Add(LWTestLShape());
    FLWItemInstance Shape = LWTestItem(TEXT("test_l_shape"));
    TArray<FLWItemInstance> Empty;
    TestTrue(TEXT("L fits exact unrotated edge"), LWItems::Fits(Empty, Shape, 0, 0, false, 2, 3));
    TestFalse(TEXT("Unrotated L exceeds short grid"), LWItems::Fits(Empty, Shape, 0, 0, false, 3, 2));
    TestTrue(TEXT("Rotated L fits exact edge"), LWItems::Fits(Empty, Shape, 0, 0, true, 3, 2));
    TestFalse(TEXT("Rotated L crossing right edge is rejected"), LWItems::Fits(Empty, Shape, 1, 0, true, 3, 2));
    TestFalse(TEXT("Negative origin is rejected"), LWItems::Fits(Empty, Shape, -1, 0, false, 4, 4));
    TestFalse(TEXT("Zero width is rejected"), LWItems::Fits(Empty, Shape, 0, 0, false, 0, 4));
    TestFalse(TEXT("Extreme coordinate cannot wrap around bounds"), LWItems::Fits(Empty, Shape, MAX_int32 - 1, 0, false, MAX_int32, 3));

    const TArray<FIntPoint> Rotated = LWItems::Footprint(Shape.Definition, true);
    TestEqual(TEXT("Rotation preserves occupied cell count"), Rotated.Num(), 4);
    for (const FIntPoint Expected : { FIntPoint(2, 0), FIntPoint(1, 0), FIntPoint(0, 0), FIntPoint(0, 1) })
    {
        TestTrue(TEXT("Clockwise occupied cell is present"), Rotated.Contains(Expected));
    }
    TArray<FLWItemInstance> Items = { Shape };
    const FLWItemInstance Dot = LWItems::Make(TEXT("food"));
    TestTrue(TEXT("Bounding boxes may overlap in an unoccupied notch"), LWItems::Fits(Items, Dot, 1, 0, false, 2, 3));
    TestFalse(TEXT("Actual occupied cells collide"), LWItems::Fits(Items, Dot, 1, 2, false, 2, 3));
    TestFalse(TEXT("Self collides unless explicitly ignored"), LWItems::Fits(Items, Shape, 0, 0, false, 2, 3));
    TestTrue(TEXT("Ignore excludes the specified identity"), LWItems::Fits(Items, Shape, 0, 0, false, 2, 3, Shape.Id));
    Items[0].bRotated = true;
    TestTrue(TEXT("Rotated notch remains available"), LWItems::Fits(Items, Dot, 2, 1, false, 3, 2));
    TestFalse(TEXT("Rotated occupied tip collides"), LWItems::Fits(Items, Dot, 2, 0, false, 3, 2));

    FLWItemInstance Equipped = LWItems::Make(TEXT("rifle"));
    Equipped.Slot = TEXT("Primary");
    FLWItemInstance Loaded = LWItems::Make(TEXT("mag_rifle30"));
    Loaded.Slot = TEXT("Loaded");
    Items = { Equipped, Loaded };
    TestTrue(TEXT("Equipped and loaded items occupy no grid cells"), LWItems::Fits(Items, Dot, 0, 0, false, 1, 1));
    TestFalse(TEXT("Loaded magazine cannot itself fit a grid target"), LWItems::Fits(Empty, Loaded, 0, 0, false, 12, 12));
    FLWItemInstance Exhausted = LWTestItem(TEXT("ammo_556"));
    Exhausted.Count = 0;
    Items = { Exhausted };
    TestTrue(TEXT("Exhausted ammo releases its cell before stack cleanup"), LWItems::Fits(Items, Dot, 0, 0, false, 1, 1));
    Items = { LWTestItem(TEXT("food")) };
    Items[0].Slot = FName(TEXT("none"));
    TestFalse(TEXT("Text none is a grid item and still collides"), LWItems::Fits(Items, Dot, 0, 0, false, 1, 1));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWInventoryPlacementTest, "LethalWorld.Inventory.PlaceMoveAndIdentity",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWInventoryPlacementTest::RunTest(const FString& Parameters)
{
    (void)Parameters;
    FLWInventoryTestCatalog Scope;
    TArray<FLWItemInstance> Items;
    FLWItemInstance Water = LWItems::Make(TEXT("water"), 3);
    const FGuid WaterId = Water.Id;
    TestTrue(TEXT("Auto-placement tries the alternate orientation"), LWItems::Place(Items, Water, 2, 1));
    TestTrue(TEXT("Auto-placement reports rotation"), Water.bRotated);
    TestTrue(TEXT("Auto-placement preserves identity"), Water.Id == WaterId);
    TestEqual(TEXT("Auto-placement preserves stack"), Water.Count, 3);
    TestEqual(TEXT("Auto-placement inserts one record"), Items.Num(), 1);
    TestTrue(TEXT("Existing item may move while ignoring its old cells"), LWItems::Move(Items, Water.Id, 0, 0, true, 2, 1));

    FLWItemInstance Food = LWItems::Make(TEXT("food"));
    Food.Durability = 43;
    const TArray<uint8> FoodBefore = LWInventorySnapshot({ Food });
    const TArray<uint8> Before = LWInventorySnapshot(Items);
    TestFalse(TEXT("Full grid refuses another item"), LWItems::Place(Items, Food, 2, 1));
    TestTrue(TEXT("Failed auto-placement preserves external instance"), FoodBefore == LWInventorySnapshot({ Food }));
    TestTrue(TEXT("Failed auto-placement preserves inventory"), Before == LWInventorySnapshot(Items));
    TestFalse(TEXT("Failed move does not rotate an item across an edge"), LWItems::Move(Items, Water.Id, 0, 0, false, 2, 1));
    TestTrue(TEXT("Failed move preserves all saved fields"), Before == LWInventorySnapshot(Items));
    TestFalse(TEXT("Duplicate placement via an array-element reference is safe"), LWItems::Place(Items, Items[0], 2, 1));
    TestFalse(TEXT("Missing identity cannot move"), LWItems::Move(Items, FGuid::NewGuid(), 0, 0, true, 2, 1));
    const FLWItemInstance Duplicate = Items[0];
    Items.Add(Duplicate);
    const TArray<uint8> Duplicated = LWInventorySnapshot(Items);
    TestFalse(TEXT("Ambiguous duplicated identity cannot move"), LWItems::Move(Items, Water.Id, 0, 0, true, 2, 1));
    TestTrue(TEXT("Ambiguous identity failure preserves data"), Duplicated == LWInventorySnapshot(Items));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWInventoryTransferTest, "LethalWorld.Inventory.AtomicTransfer",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWInventoryTransferTest::RunTest(const FString& Parameters)
{
    (void)Parameters;
    FLWInventoryTestCatalog Scope;
    FLWItemInstance Revolver = LWTestItem(TEXT("revolver"));
    Revolver.Cylinder = { 1, 2, 0, 1, 2, 0 };
    Revolver.CylinderIndex = 5;
    Revolver.Durability = 37;
    TArray<FLWItemInstance> From = { Revolver, LWTestItem(TEXT("food"), 3, 0) };
    TArray<FLWItemInstance> To = { LWTestItem(TEXT("food")) };
    const TArray<uint8> FromBefore = LWInventorySnapshot(From);
    const TArray<uint8> ToBefore = LWInventorySnapshot(To);
    TestFalse(TEXT("Occupied destination rejects transfer"), LWItems::Transfer(From, To, Revolver.Id, 0, 0, true, 6, 6));
    TestTrue(TEXT("Failure preserves entire source"), FromBefore == LWInventorySnapshot(From));
    TestTrue(TEXT("Failure preserves entire destination"), ToBefore == LWInventorySnapshot(To));
    TestFalse(TEXT("Off-grid destination rejects transfer"), LWItems::Transfer(From, To, Revolver.Id, 5, 5, false, 6, 6));
    TestTrue(TEXT("Bounds failure also preserves source"), FromBefore == LWInventorySnapshot(From));

    To.Add(Revolver);
    const TArray<uint8> DuplicateBefore = LWInventorySnapshot(To);
    TestFalse(TEXT("Destination duplicate identity rejects transfer"), LWItems::Transfer(From, To, Revolver.Id, 2, 0, false, 6, 6));
    TestTrue(TEXT("Duplicate failure preserves destination"), DuplicateBefore == LWInventorySnapshot(To));
    To.RemoveAt(To.Num() - 1);

    TestTrue(TEXT("Valid transfer succeeds"), LWItems::Transfer(From, To, Revolver.Id, 2, 0, true, 6, 6));
    TestEqual(TEXT("Source loses exactly one record"), From.Num(), 1);
    TestEqual(TEXT("Destination gains exactly one record"), To.Num(), 2);
    FLWItemInstance Expected = Revolver;
    Expected.X = 2;
    Expected.Y = 0;
    Expected.bRotated = true;
    if (To.Num() == 2)
    {
        TestTrue(TEXT("Success preserves all cylinder and durability state"), LWInventorySnapshot({ Expected }) == LWInventorySnapshot({ To[1] }));
    }
    TestTrue(TEXT("Same-array transfer is a move"), LWItems::Transfer(To, To, Revolver.Id, 3, 2, false, 6, 6));
    TestEqual(TEXT("Same-array transfer never duplicates/removes records"), To.Num(), 2);
    const TArray<uint8> SameBefore = LWInventorySnapshot(To);
    TestFalse(TEXT("Invalid same-array transfer is atomic"), LWItems::Transfer(To, To, Revolver.Id, 5, 5, false, 6, 6));
    TestTrue(TEXT("Same-array failure preserves data"), SameBefore == LWInventorySnapshot(To));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWInventoryLoadedTransferTest, "LethalWorld.Inventory.LoadedMagazineTransfer",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWInventoryLoadedTransferTest::RunTest(const FString& Parameters)
{
    (void)Parameters;
    FLWInventoryTestCatalog Scope;
    for (bool MagazineFirst : { false, true })
    {
        FLWItemInstance Gun = LWTestItem(TEXT("rifle"));
        FLWItemInstance Mag = LWItems::Make(TEXT("mag_rifle30"));
        Mag.Slot = TEXT("Loaded");
        Mag.Rounds = 23;
        Mag.Durability = 61;
        Gun.LoadedMagazine = Mag.Id;
        Gun.Chamber = 1;
        TArray<FLWItemInstance> From = MagazineFirst ? TArray<FLWItemInstance>{ Mag, Gun } : TArray<FLWItemInstance>{ Gun, Mag };
        From.Add(LWTestItem(TEXT("food"), 6, 0));
        TArray<FLWItemInstance> To = { LWTestItem(TEXT("food")) };
        const TArray<uint8> FromBefore = LWInventorySnapshot(From);
        const TArray<uint8> ToBefore = LWInventorySnapshot(To);
        TestFalse(TEXT("Direct loaded-magazine move fails"), LWItems::Move(From, Mag.Id, 0, 3, false, 12, 12));
        TestFalse(TEXT("Direct loaded-magazine equip fails"), LWItems::Equip(From, Mag.Id, TEXT("Tool")));
        TestFalse(TEXT("Direct loaded-magazine transfer fails"), LWItems::Transfer(From, To, Mag.Id, 1, 0, false, 12, 12));
        FLWItemInstance ExternalMag = Mag;
        ExternalMag.Id = FGuid::NewGuid();
        TestFalse(TEXT("Loaded magazine cannot be auto-placed"), LWItems::Place(To, ExternalMag, 12, 12));
        TestTrue(TEXT("Rejected magazine operations preserve source"), FromBefore == LWInventorySnapshot(From));
        TestTrue(TEXT("Rejected magazine operations preserve destination"), ToBefore == LWInventorySnapshot(To));
        TestFalse(TEXT("Blocked gun transfer leaves its magazine behind with it"), LWItems::Transfer(From, To, Gun.Id, 0, 0, false, 6, 2));
        TestTrue(TEXT("Blocked pair transfer is atomic"), FromBefore == LWInventorySnapshot(From) && ToBefore == LWInventorySnapshot(To));

        To.Add(Mag);
        const TArray<uint8> CollisionBefore = LWInventorySnapshot(To);
        TestFalse(TEXT("Loaded-magazine destination identity collision rejects whole pair"), LWItems::Transfer(From, To, Gun.Id, 1, 0, false, 6, 2));
        TestTrue(TEXT("Magazine collision preserves both arrays"), FromBefore == LWInventorySnapshot(From) && CollisionBefore == LWInventorySnapshot(To));
        To.RemoveAt(To.Num() - 1);

        TestTrue(TEXT("Gun and loaded magazine transfer together"), LWItems::Transfer(From, To, Gun.Id, 1, 0, false, 6, 2));
        TestEqual(TEXT("Only unrelated source item remains"), From.Num(), 1);
        TestEqual(TEXT("Both records reach destination"), To.Num(), 3);
        const FLWItemInstance* MovedGun = To.FindByPredicate([&Gun](const FLWItemInstance& I) { return I.Id == Gun.Id; });
        const FLWItemInstance* MovedMag = To.FindByPredicate([&Mag](const FLWItemInstance& I) { return I.Id == Mag.Id; });
        if (TestNotNull(TEXT("Gun identity preserved"), MovedGun) && TestNotNull(TEXT("Magazine identity preserved"), MovedMag))
        {
            TestTrue(TEXT("Link still points to colocated magazine"), MovedGun->LoadedMagazine == MovedMag->Id);
            TestTrue(TEXT("Loaded record is unchanged in full"), LWInventorySnapshot({ *MovedMag }) == LWInventorySnapshot({ Mag }));
            TestEqual(TEXT("Chamber plus magazine rounds conserved"), MovedGun->Chamber + MovedMag->Rounds, 24);
            TestEqual(TEXT("Gun does not duplicate magazine rounds"), MovedGun->Rounds, 0);
        }

        TArray<FLWItemInstance> Broken = { Gun };
        TArray<FLWItemInstance> Empty;
        const TArray<uint8> BrokenBefore = LWInventorySnapshot(Broken);
        TestFalse(TEXT("Missing linked magazine rejects transfer"), LWItems::Transfer(Broken, Empty, Gun.Id, 0, 0, false, 12, 12));
        TestTrue(TEXT("Missing link failure is atomic"), BrokenBefore == LWInventorySnapshot(Broken) && Empty.IsEmpty());
        FLWItemInstance OtherGun = Gun;
        OtherGun.Id = FGuid::NewGuid();
        OtherGun.Y = 3;
        Broken = { Gun, Mag, OtherGun };
        const TArray<uint8> SharedBefore = LWInventorySnapshot(Broken);
        TestFalse(TEXT("Two guns cannot claim the same loaded magazine"), LWItems::Transfer(Broken, Empty, Gun.Id, 0, 0, false, 12, 12));
        TestTrue(TEXT("Shared magazine failure is atomic"), SharedBefore == LWInventorySnapshot(Broken) && Empty.IsEmpty());
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWInventoryMagazineTest, "LethalWorld.Inventory.MagazineCompatibilityCapacityConservation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWInventoryMagazineTest::RunTest(const FString& Parameters)
{
    (void)Parameters;
    FLWInventoryTestCatalog Scope;
    const TArray<FName> MagIds = { TEXT("mag_sniper5"), TEXT("mag_smg30"), TEXT("mag_rifle30"), TEXT("mag_lmg100") };
    const TArray<FName> AmmoIds = { TEXT("ammo_12g"), TEXT("ammo_357"), TEXT("ammo_762"), TEXT("ammo_9mm"), TEXT("ammo_556"), TEXT("ammo_762belt") };
    const int32 Capacities[] = { 5, 30, 30, 100 };
    for (int32 M = 0; M < MagIds.Num(); ++M)
    {
        TestEqual(TEXT("Requested capacity"), LWItems::Def(MagIds[M]).Capacity, Capacities[M]);
        for (int32 A = 0; A < AmmoIds.Num(); ++A)
        {
            FLWItemInstance Mag = LWItems::Make(MagIds[M]);
            TestEqual(TEXT("Factory magazine starts empty"), Mag.Rounds, 0);
            Mag.Rounds = Capacities[M] - 2;
            FLWItemInstance Ammo = LWItems::Make(AmmoIds[A], 4);
            const bool Compatible = ((M == 0 || M == 3) && (A == 2 || A == 5)) || (M == 1 && A == 3) || (M == 2 && A == 4);
            const int32 Before = Mag.Rounds + Ammo.Count;
            const TArray<uint8> Snapshot = LWInventorySnapshot({ Mag, Ammo });
            TestEqual(TEXT("Compatibility matrix controls round movement"), LWItems::LoadMagazine(Mag, Ammo), Compatible ? 2 : 0);
            TestEqual(TEXT("Every load conserves rounds"), Mag.Rounds + Ammo.Count, Before);
            TestTrue(TEXT("Capacity is never exceeded"), Mag.Rounds <= Capacities[M]);
            if (!Compatible)
            {
                TestTrue(TEXT("Incompatible load changes no saved fields"), Snapshot == LWInventorySnapshot({ Mag, Ammo }));
            }
        }
    }

    FLWItemInstance Box = LWItems::Make(TEXT("mag_lmg100"));
    FLWItemInstance First = LWItems::Make(TEXT("ammo_762"), 60);
    FLWItemInstance Second = LWItems::Make(TEXT("ammo_762"), 60);
    TestEqual(TEXT("Box takes first loose stack"), LWItems::LoadMagazine(Box, First), 60);
    TestEqual(TEXT("Empty source reports zero"), LWItems::LoadMagazine(Box, First), 0);
    TestEqual(TEXT("Box stops at capacity using partial second stack"), LWItems::LoadMagazine(Box, Second), 40);
    TestEqual(TEXT("Remaining loose rounds are retained"), Second.Count, 20);
    TestEqual(TEXT("Multiple-stack total is conserved"), Box.Rounds + First.Count + Second.Count, 120);
    TestEqual(TEXT("Full magazine cannot consume more"), LWItems::LoadMagazine(Box, Second), 0);
    for (int32 BadRounds : { -1, 101 })
    {
        Box.Rounds = BadRounds;
        const TArray<uint8> Before = LWInventorySnapshot({ Box, Second });
        TestEqual(TEXT("Malformed magazine is rejected without normalizing away rounds"), LWItems::LoadMagazine(Box, Second), 0);
        TestTrue(TEXT("Malformed load is unchanged"), Before == LWInventorySnapshot({ Box, Second }));
    }
    Box.Rounds = 0;
    Box.Count = 2;
    TestEqual(TEXT("Stacked magazines cannot multiply loaded ammunition"), LWItems::LoadMagazine(Box, Second), 0);
    Box.Count = 1;
    FLWItemInstance AliasedIdentity = Second;
    AliasedIdentity.Id = Box.Id;
    TestEqual(TEXT("Two records with the same identity cannot exchange rounds"), LWItems::LoadMagazine(Box, AliasedIdentity), 0);
    const TArray<uint8> BeforeSelf = LWInventorySnapshot({ Box });
    TestEqual(TEXT("An instance cannot load itself"), LWItems::LoadMagazine(Box, Box), 0);
    TestTrue(TEXT("Self-load preserves data"), BeforeSelf == LWInventorySnapshot({ Box }));
    FLWItemInstance Gun = LWItems::Make(TEXT("sniper"));
    TestEqual(TEXT("Weapon cannot impersonate a detachable magazine"), LWItems::LoadMagazine(Gun, Second), 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWInventoryEquipmentTest, "LethalWorld.Inventory.EquipmentRestrictions",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWInventoryEquipmentTest::RunTest(const FString& Parameters)
{
    (void)Parameters;
    FLWInventoryTestCatalog Scope;
    const TArray<FName> Slots = { TEXT("Primary"), TEXT("Secondary"), TEXT("Sidearm"), TEXT("Melee"), TEXT("Armor"), TEXT("Helmet"), TEXT("Rig"), TEXT("Backpack"), TEXT("Tool") };
    const TArray<FName> Equipment = { TEXT("rifle"), TEXT("shotgun"), TEXT("revolver"), TEXT("crowbar"), TEXT("armor"), TEXT("helmet"), TEXT("rig"), TEXT("backpack"), TEXT("tool") };
    for (int32 ItemIndex = 0; ItemIndex < Equipment.Num(); ++ItemIndex)
    {
        const FLWItemInstance Item = LWItems::Make(Equipment[ItemIndex]);
        for (int32 SlotIndex = 0; SlotIndex < Slots.Num(); ++SlotIndex)
        {
            const bool Allowed = ItemIndex < 2 ? SlotIndex < 2 : ItemIndex == SlotIndex;
            TestEqual(TEXT("Equipment compatibility matrix"), LWItems::CanEquip(Item, Slots[SlotIndex]), Allowed);
        }
        TestFalse(TEXT("None is a grid target, not equipment"), LWItems::CanEquip(Item, NAME_None));
        TestFalse(TEXT("Unknown equipment slot rejected"), LWItems::CanEquip(Item, TEXT("Belt")));
        TestFalse(TEXT("Loaded is reserved, not equipment"), LWItems::CanEquip(Item, TEXT("Loaded")));
    }
    for (FName Definition : { FName(TEXT("food")), FName(TEXT("water")), FName(TEXT("medkit")), FName(TEXT("ammo_556")), FName(TEXT("mag_rifle30")) })
    {
        for (FName Slot : Slots)
        {
            TestFalse(TEXT("Ordinary storage items cannot equip"), LWItems::CanEquip(LWItems::Make(Definition), Slot));
        }
    }
    FLWItemInstance Rifle = LWTestItem(TEXT("rifle"));
    FLWItemInstance Shotgun = LWTestItem(TEXT("shotgun"), 0, 3);
    TArray<FLWItemInstance> Items = { Rifle, Shotgun };
    TestTrue(TEXT("Compatible item equips"), LWItems::Equip(Items, Rifle.Id, TEXT("Primary")));
    TestEqual(TEXT("Equipped item clears grid origin"), Items[0].X, -1);
    const TArray<uint8> Before = LWInventorySnapshot(Items);
    TestFalse(TEXT("Occupied equipment slot does not swap implicitly"), LWItems::Equip(Items, Shotgun.Id, TEXT("Primary")));
    TestTrue(TEXT("Failed equipment mutation is atomic"), Before == LWInventorySnapshot(Items));
    TestTrue(TEXT("Second long gun fits Secondary"), LWItems::Equip(Items, Shotgun.Id, TEXT("Secondary")));
    TestTrue(TEXT("Re-equipping the same slot is harmless"), LWItems::Equip(Items, Rifle.Id, TEXT("Primary")));
    Items.Add(LWTestItem(TEXT("food")));
    const TArray<uint8> EquippedBefore = LWInventorySnapshot(Items);
    TestFalse(TEXT("Unequip fails if requested grid cells are occupied"), LWItems::Move(Items, Rifle.Id, 0, 0, false, 6, 2));
    TestTrue(TEXT("Failed unequip keeps equipment slot"), EquippedBefore == LWInventorySnapshot(Items));
    TestTrue(TEXT("Move to free grid cells unequips"), LWItems::Move(Items, Rifle.Id, 1, 0, false, 6, 2));
    TestTrue(TEXT("Successful unequip clears slot"), Items[0].Slot.IsNone());
    TestTrue(TEXT("Vacated equipment slot is reusable"), LWItems::Equip(Items, Shotgun.Id, TEXT("Primary")));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWInventorySerializationTest, "LethalWorld.Inventory.SaveGameRoundTrip",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWInventorySerializationTest::RunTest(const FString& Parameters)
{
    (void)Parameters;
    FLWInventoryTestCatalog Scope;
    FLWItemInstance Gun = LWItems::Make(TEXT("revolver"));
    Gun.Count = 3;
    Gun.X = 7;
    Gun.Y = 9;
    Gun.bRotated = true;
    Gun.Slot = TEXT("Sidearm");
    Gun.Rounds = 4;
    Gun.Chamber = 2;
    Gun.Cylinder = { 0, 1, 2, 1, 2, 0 };
    Gun.CylinderIndex = 5;
    Gun.LoadedMagazine = FGuid::NewGuid();
    Gun.Durability = 39;
    // Deliberately distinctive field values test serialization independently from gameplay validation.
    TArray<uint8> Bytes = LWInventorySnapshot({ Gun });
    FMemoryReader Reader(Bytes, true);
    FObjectAndNameAsStringProxyArchive Archive(Reader, false);
    Archive.ArIsSaveGame = true;
    int32 Count = 0;
    Archive << Count;
    FLWItemInstance Restored;
    FLWItemInstance::StaticStruct()->SerializeItem(Archive, &Restored, nullptr);
    TestFalse(TEXT("SaveGame archive loads without errors"), Archive.IsError());
    TestEqual(TEXT("Array record count persists"), Count, 1);
    TestTrue(TEXT("Every saved field round-trips, including live/spent chambers and linked GUID"), Bytes == LWInventorySnapshot({ Restored }));
    TestTrue(TEXT("Stable item identity persists"), Gun.Id == Restored.Id);
    TestEqual(TEXT("Definition ID persists"), Restored.Definition, Gun.Definition);
    TestEqual(TEXT("Stack count persists"), Restored.Count, Gun.Count);
    TestEqual(TEXT("X coordinate persists"), Restored.X, Gun.X);
    TestEqual(TEXT("Y coordinate persists"), Restored.Y, Gun.Y);
    TestEqual(TEXT("Rotation persists"), Restored.bRotated, Gun.bRotated);
    TestEqual(TEXT("Equipment slot persists"), Restored.Slot, Gun.Slot);
    TestEqual(TEXT("Integral rounds persist"), Restored.Rounds, Gun.Rounds);
    TestEqual(TEXT("Chamber state persists"), Restored.Chamber, Gun.Chamber);
    TestEqual(TEXT("Durability persists"), Restored.Durability, Gun.Durability);
    TestTrue(TEXT("Live/spent cylinder states persist"), Gun.Cylinder == Restored.Cylinder);
    TestEqual(TEXT("Cylinder index persists"), Restored.CylinderIndex, 5);
    TestTrue(TEXT("Loaded magazine link persists"), Gun.LoadedMagazine == Restored.LoadedMagazine);
    return true;
}
#endif
