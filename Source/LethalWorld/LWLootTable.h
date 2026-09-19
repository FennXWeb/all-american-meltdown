#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LWInventory.h"
#include "LWLootTable.generated.h"

UENUM(BlueprintType)
enum class ELWLootTier : uint8
{
    Common,
    Uncommon,
    Rare,
    Epic
};

USTRUCT(BlueprintType)
struct LETHALWORLD_API FLWLootEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot")
    FName ItemId;

    // Zero, negative and non-finite weights never participate in a draw.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot", meta=(ClampMin="0"))
    float Weight = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot")
    ELWLootTier Tier = ELWLootTier::Common;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot", meta=(ClampMin="1", ClampMax="10000"))
    int32 MinCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot", meta=(ClampMin="1", ClampMax="10000"))
    int32 MaxCount = 1;

    // Prevent another selection of this ID, including from guaranteed groups.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot")
    bool bUnique = false;

    // Total units of this ID across the entire container; zero is unlimited.
    // When an ID occurs in multiple rows, its strictest positive cap applies.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot", meta=(ClampMin="0", ClampMax="10000"))
    int32 MaxPerContainer = 0;
};

USTRUCT(BlueprintType)
struct LETHALWORLD_API FLWLootGroup
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot")
    FName Name;

    // Draws before regular rolls, subject to empty chance, limits and valid weights.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot", meta=(ClampMin="0", ClampMax="128"))
    int32 Rolls = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot", meta=(TitleProperty="ItemId"))
    TArray<FLWLootEntry> Entries;
};

USTRUCT(BlueprintType)
struct LETHALWORLD_API FLWLootPreset
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot")
    FName Context;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot", meta=(TitleProperty="ItemId"))
    TArray<FLWLootEntry> Entries;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot", meta=(ClampMin="0", ClampMax="128"))
    int32 MinRolls = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot", meta=(ClampMin="0", ClampMax="128"))
    int32 MaxRolls = 3;

    // Probability that the WHOLE container is empty, evaluated before guarantees.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot", meta=(ClampMin="0", ClampMax="1"))
    float EmptyChance = 0.15f;

    // Never select an item ID twice; a selected ammo/supply stack may contain >1 unit.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot")
    bool bUniqueItems = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot", meta=(ClampMin="0", ClampMax="128"))
    int32 MaxItems = 16;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot", meta=(TitleProperty="Name"))
    TArray<FLWLootGroup> GuaranteedGroups;

    // Optional context overrides, multiplied by the table's tier multipliers.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot")
    TMap<ELWLootTier, float> RarityMultipliers;

    // Quality and normalized Hazard favor higher tiers and larger stack counts.
    // Tier weight factor: (1 + Quality*QualityRarityBoost + Hazard*HazardRarityBoost)^Tier.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scaling", meta=(ClampMin="0", ClampMax="1"))
    float Quality = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scaling", meta=(ClampMin="0", ClampMax="10"))
    float QualityRarityBoost = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scaling", meta=(ClampMin="0", ClampMax="10"))
    float HazardRarityBoost = 2.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scaling", meta=(ClampMin="0", ClampMax="128"))
    int32 HazardExtraRolls = 2;

    // Reduces EmptyChance by this fraction at Hazard=1; EmptyChance=1 stays certain.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scaling", meta=(ClampMin="0", ClampMax="1"))
    float HazardEmptyReduction = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scaling", meta=(ClampMin="0", ClampMax="10"))
    float QualityCountBonus = 0.25f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scaling", meta=(ClampMin="0", ClampMax="10"))
    float HazardCountBonus = 0.5f;
};

// New assets start with native presets. Serialized edits override those defaults.
UCLASS(BlueprintType)
class LETHALWORLD_API ULWLootTable : public UDataAsset
{
    GENERATED_BODY()

public:
    ULWLootTable();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot", meta=(TitleProperty="Context"))
    TArray<FLWLootPreset> Presets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot")
    TMap<ELWLootTier, float> RarityMultipliers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loot")
    FName DefaultContext = FName(TEXT("road"));

    // Local random stream only. Same table, catalog, context, seed and hazard => same loot,
    // including instance GUIDs. Supply a distinct seed per container / trader restock.
    // Stack counts are scaled then capped by the catalog's MaxStack and MaxPerContainer.
    // Unknown contexts use DefaultContext; missing fallback/explicitly empty presets yield nothing.
    UFUNCTION(BlueprintCallable, Category="Loot")
    TArray<FLWItemInstance> Roll(FName Context, int32 Seed, float Hazard = 0.f) const;

    // Only call deliberately: replaces all designer changes with the native defaults.
    UFUNCTION(BlueprintCallable, CallInEditor, Category="Loot")
    void ResetToNativeDefaults();
};

namespace LWLoot
{
    // Main/game thread: load the editable asset if present, otherwise use native defaults.
    LETHALWORLD_API const ULWLootTable* Get();
    LETHALWORLD_API TArray<FLWItemInstance> Roll(FName Context, int32 Seed, float Hazard = 0.f);
}
