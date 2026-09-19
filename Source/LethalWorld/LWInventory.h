#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UObject/SoftObjectPtr.h"
#include "LWInventory.generated.h"

class UStaticMesh;

/** Editable item rules. Width/Height are the unrotated bounding box (1..64 cells). */
USTRUCT(BlueprintType)
struct LETHALWORLD_API FLWItemDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FName Id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FText DisplayName;

    /** Built-in categories: Weapon, Ammo, Magazine, Armor, Container, Tool, Consumable, Material. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
    FName Category;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Footprint", meta=(ClampMin="1", ClampMax="64"))
    int32 Width = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Footprint", meta=(ClampMin="1", ClampMax="64"))
    int32 Height = 1;

    /** Empty means a filled rectangle. Otherwise only these unique local cells are occupied. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Footprint")
    TArray<FIntPoint> Cells;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Equipment")
    TArray<FName> EquipSlots;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item", meta=(ClampMin="1"))
    int32 MaxStack = 1;

    /** Loose ammunition ID accepted by this weapon/magazine. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
    FName AmmoType;

    /** Detachable magazine definition; None for melee, tube-fed weapons and revolvers. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
    FName MagazineType;

    /** Magazine/tube capacity, or cylinder size. Chamber is tracked separately. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon", meta=(ClampMin="0"))
    int32 Capacity = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
    int32 WeaponIndex = INDEX_NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item", meta=(ClampMin="0"))
    int32 Price = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visual")
    TSoftObjectPtr<UStaticMesh> Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visual")
    FTransform MeshTransform = FTransform::Identity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Container", meta=(ClampMin="0"))
    int32 ContainerWidth = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Container", meta=(ClampMin="0"))
    int32 ContainerHeight = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item", meta=(ClampMin="0"))
    int32 MaxDurability = 100;
};

/** Optional /Game/Data/DA_ItemCatalog. Entries override defaults by Id; invalid entries are ignored. */
UCLASS(BlueprintType)
class LETHALWORLD_API ULWItemCatalog : public UDataAsset
{
    GENERATED_BODY()

public:
    ULWItemCatalog();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Items", meta=(TitleProperty="Id"))
    TArray<FLWItemDefinition> Items;
};

USTRUCT(BlueprintType)
struct LETHALWORLD_API FLWAssemblyPart39 {
 GENERATED_BODY()
 UPROPERTY(EditAnywhere,BlueprintReadWrite,SaveGame) FGuid Id;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,SaveGame) FName Definition;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,SaveGame) FTransform Transform=FTransform::Identity;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,SaveGame) int32 Tier=0;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,SaveGame) FName Modifier;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,SaveGame) int32 Skin=-1;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,SaveGame) int32 Durability=100;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,SaveGame) int32 Rounds=0;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,SaveGame) int32 Chamber=0;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,SaveGame) TArray<int32> Cylinder;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,SaveGame) int32 CylinderIndex=0;
};

/** SaveGame properties preserve identity, placement and ammunition state without object pointers. */
USTRUCT(BlueprintType)
struct LETHALWORLD_API FLWItemInstance
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere,BlueprintReadWrite,SaveGame,Category="Workbench") TArray<FLWAssemblyPart39> Parts39;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,SaveGame,Category="Workbench") FGuid Action39;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,SaveGame) FTransform PartTransform39=FTransform::Identity;
    UPROPERTY(EditAnywhere,BlueprintReadWrite,SaveGame,Category="Workbench") FString CustomName39;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Item")
    FGuid VehicleId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Weapon") int32 WeaponTier=0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Weapon") FName LegendaryModifier;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Weapon") int32 WeaponSkin=-1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Weapon") TMap<FName,FName> Attachments;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Item")
    FGuid Id;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Item")
    FName Definition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Item")
    int32 Count = 1;

    /** -1/-1 when unplaced or equipped. Grid coordinates refer to the footprint's origin. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Placement")
    int32 X = -1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Placement")
    int32 Y = -1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Placement")
    bool bRotated = false;

    /** NAME_None (including FName("none")) means grid storage; Loaded is reserved for linked magazines. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Placement")
    FName Slot;

    /** Live rounds in a magazine or integral tube. Never mirror a detachable magazine here. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Ammunition")
    int32 Rounds = 0;

    /** 0 = empty, 1 = live, 2 = spent. Revolvers use Cylinder instead. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Ammunition")
    int32 Chamber = 0;

    /** One entry per revolver chamber: 0 = empty, 1 = live, 2 = spent. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Ammunition")
    TArray<int32> Cylinder;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Ammunition", meta=(ClampMin="0", ClampMax="5"))
    int32 CylinderIndex = 0;

    /** Identity of the actual magazine instance, whose Rounds remain authoritative. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Ammunition")
    FGuid LoadedMagazine;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category="Item")
    int32 Durability = 100;
};

/** Game-thread inventory operations. Failure never mutates any argument. No implicit stacking/swapping. */
namespace LWItems
{
    LETHALWORLD_API int32 BackpackRows(FName Definition);
    LETHALWORLD_API int32 InventoryHeight(const TArray<FLWItemInstance>& Items);
    LETHALWORLD_API bool ValidateEquipment(const TArray<FLWItemInstance>& Items);
    LETHALWORLD_API bool StackCompatible(const FLWItemInstance& A,const FLWItemInstance& B);
    LETHALWORLD_API bool AutoSort(TArray<FLWItemInstance>& Items,int32 Width,int32 Height);
    LETHALWORLD_API bool QuickTransfer(TArray<FLWItemInstance>& From,TArray<FLWItemInstance>& To,FGuid Id,int32 Width,int32 Height);
    /** Unknown IDs return an inert definition with Id=None. References last until SetCatalog. */
    LETHALWORLD_API const FLWItemDefinition& Def(FName Id);
    LETHALWORLD_API TArray<FLWItemDefinition> Defaults();
    LETHALWORLD_API TArray<FLWItemDefinition> All47();

    /** Copies valid overrides onto defaults. nullptr resets lazy runtime asset lookup. */
    LETHALWORLD_API void SetCatalog(const ULWItemCatalog* Catalog);

    /** Unknown IDs or counts outside [1, MaxStack] return an invalid Id; no counts are silently lost. */
    LETHALWORLD_API FLWItemInstance Make(FName Definition, int32 Count = 1);

    /** Clockwise 90-degree rotation inside the definition's bounding box. Unknown IDs return no cells. */
    LETHALWORLD_API TArray<FIntPoint> Footprint(FName Definition, bool Rotated = false);

    /** Only occupied cells collide. Equipped items do not occupy grid cells; Ignore skips one identity. */
    LETHALWORLD_API bool Fits(const TArray<FLWItemInstance>& Items, const FLWItemInstance& Item,
        int32 X, int32 Y, bool Rotated, int32 Width, int32 Height, FGuid Ignore = FGuid());

    /** Adds a new identity, scanning rows in its current orientation, then the other. Loaded is rejected. */
    LETHALWORLD_API bool Place(TArray<FLWItemInstance>& Items, FLWItemInstance& Item,
        int32 Width, int32 Height);

    /** Moves an existing identity to the grid, clearing its equipment slot. Loaded magazines are rejected. */
    LETHALWORLD_API bool Move(TArray<FLWItemInstance>& Items, FGuid Id,
        int32 X, int32 Y, bool Rotated, int32 Width, int32 Height);

    /** Atomic grid transfer, including a gun's linked Loaded magazine. Same-array transfer is a Move. */
    LETHALWORLD_API bool Transfer(TArray<FLWItemInstance>& From, TArray<FLWItemInstance>& To, FGuid Id,
        int32 X, int32 Y, bool Rotated, int32 Width, int32 Height);

    /** Slots: Primary, Secondary, Sidearm, Melee, Armor, Helmet, Rig, Backpack, Tool. */
    LETHALWORLD_API bool CanEquip(const FLWItemInstance& Item, FName Slot);

    /** Requires an existing identity and vacant compatible slot. Use Move to unequip. */
    LETHALWORLD_API bool Equip(TArray<FLWItemInstance>& Items, FGuid Id, FName Slot);

    /** Returns rounds moved. Ammo.Count may become zero; caller may then remove that exhausted stack. */
    LETHALWORLD_API int32 LoadMagazine(FLWItemInstance& Mag, FLWItemInstance& Ammo);
}
