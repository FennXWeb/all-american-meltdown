#include "LWWeaponEffect.h"
#include "LWVehicle.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "LWWeaponMods.h"
#include "LWVehicle.h"
#include "LWWorldObject.h"
#include "LWCharacter.h"
#include "Misc/ScopeExit.h"
#include "LWWorld.h"
#include "LWZombie.h"
#include "Camera/CameraComponent.h"
#include "LWAudioCatalog.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Inventory is the only ammunition store. This translation unit owns transient animation
// bookkeeping, not copies of weapons, magazines or rounds. Weak keys do not retain actors.
namespace LWWeapons
{
    constexpr int32 TubeCapacity = 5;
    constexpr int32 CylinderSize = 6;
    const FName LoadedSlot(TEXT("Loaded"));

    struct FSpec
    {
        const TCHAR* Name;
        const TCHAR* Mesh;
        const TCHAR* FireSound;
        float Damage, Interval, Range, HipSpread, AimSpread, FalloffStart, FalloffSpan;
        float MinimumDamage, HeadMultiplier, Recoil, YawRecoil, Noise;
        int32 Pellets;
        FVector Muzzle;
    };

    // Entries 0..2 preserve the original combat tuning exactly (see melee handling too).
    const FSpec Specs[] = {
        {TEXT("CROWBAR"), TEXT("Crowbar"), TEXT("Swing"), 36,.55f,185,0,0,0,1,1,1,0,0,700,1,FVector(60,0,0)},
        {TEXT("BASEBALL BAT"), TEXT("Bat"), TEXT("Swing"), 63,.85f,225,0,0,0,1,1,1,0,0,700,1,FVector(83,0,0)},
        {TEXT("PUMP SHOTGUN"), TEXT("Shotgun"), TEXT("Shotgun"), 16,.9f,9000,4.8f,2.1f,600,8000,.3f,1.8f,1.15f,.2f,16000,10,FVector(79,0,2.6)},
        {TEXT(".357 REVOLVER"), TEXT("Revolver"), TEXT("RevolverFire"), 68,.42f,14000,2.8f,.45f,1500,10000,.45f,2,1.1f,.17f,15000,1,FVector(34.5,0,4.5)},
        {TEXT("SNIPER RIFLE"), TEXT("Sniper"), TEXT("SniperFire"), 115,.9f,45000,5,.09f,9000,30000,.60f,2.4f,1.6f,.13f,25000,1,FVector(92.3,0,4.3)},
        {TEXT("SUBMACHINE GUN"), TEXT("SMG"), TEXT("SMGFire"), 22,.085f,10000,3.4f,1.1f,1000,7000,.35f,1.8f,.24f,.14f,11000,1,FVector(37.3,0,1.9)},
        {TEXT("COMBAT RIFLE"), TEXT("Rifle"), TEXT("RifleFire"), 34,.11f,20000,2.9f,.55f,2500,14000,.40f,2,.48f,.15f,19000,1,FVector(64.4,0,2.7)},
        {TEXT("LIGHT MACHINE GUN"), TEXT("LMG"), TEXT("LMGFire"), 38,.10f,22000,4.4f,1.35f,2800,16000,.40f,1.8f,.65f,.24f,23000,1,FVector(80.5,0,2.8)},
        {TEXT("DOUBLE BARREL SHOTGUN"),TEXT("DoubleBarrelV18"),TEXT("DoubleBarrelFire"),18,.32f,8500,5.2f,2.5f,500,7000,.28f,1.7f,1.7f,.3f,19000,11,FVector(78,0,3)},
        {TEXT("MISSILE LAUNCHER"),TEXT("MissileLauncher24"),TEXT("MissileFire24"),420,1.3f,30000,1,.2f,0,30000,1,1,2,.1f,25000,1,FVector(85,0,3)},
        {TEXT("MINIGUN"),TEXT("Minigun24"),TEXT("MinigunFire24"),28,.045f,22000,4,1.8f,2500,18000,.4f,1.8f,.22f,.12f,26000,1,FVector(86,0,0)},
        {TEXT("SAWED-OFF SHOTGUN"),TEXT("SawedOff24"),TEXT("SawedOffFire24"),20,.26f,5500,8,5,350,4200,.2f,1.6f,2,.35f,20000,12,FVector(42,0,3)},
        {TEXT("DESERT EAGLE"),TEXT("DesertEagle24"),TEXT("DeagleFire24"),85,.36f,15000,3.2f,.5f,1600,12000,.45f,2,1.8f,.25f,19000,1,FVector(29,0,4)},
        {TEXT("M4 CARBINE"),TEXT("M424"),TEXT("M4Fire24"),36,.08f,24000,2.5f,.4f,3000,18000,.45f,2,.38f,.1f,19000,1,FVector(64,0,3)},
        {TEXT("TASER"),TEXT("Taser24"),TEXT("TaserFire24"),3,.8f,850,1,.15f,0,850,1,1,.1f,0,1800,1,FVector(23,0,3)},
        {TEXT("FLAMETHROWER"),TEXT("Flamethrower24"),TEXT("FlameFire24"),7,.11f,850,8,6,0,850,1,1,.04f,0,10000,1,FVector(74,0,0)},
        {TEXT("10-BARREL SHOTGUN"),TEXT("TenBarrel50"),TEXT("DoubleBarrelFire"),16,3.f,9000,8,5,600,8000,.3f,1.8f,3,.3f,35000,100,FVector(82,0,3)},
        {TEXT("GIANT GLOCK"),TEXT("GiantGlock50"),TEXT("DeagleFire24"),100,.5f,18000,3,.5f,1800,13000,.45f,2,2.4f,.2f,22000,1,FVector(66,0,8)},
        {TEXT("QUESTIONABLE AK"),TEXT("QuestionableAK50"),TEXT("RifleFire"),37,.1f,22000,3.5f,.8f,2500,16000,.4f,2,.6f,.2f,20000,1,FVector(68,0,3)},
        {TEXT("FINGER GUNS"),TEXT("FingerGuns50"),TEXT("Click"),6,.3f,7000,2,.4f,1000,6000,.5f,1.5f,.2f,0,1800,1,FVector(34,0,3)},
        {TEXT("BUDGET CUT AR-15"),TEXT("BudgetCut50"),TEXT("M4Fire24"),27,.14f,17000,4,1,2000,12000,.35f,1.8f,.5f,.2f,18000,1,FVector(65,0,3)}
    };

    struct FRuntime
    {
        FGuid VisualGun, ReloadGun, CycleGun;
        float CycleElapsed = 0;
        FVector2D Kick54=FVector2D::ZeroVector;float Buffered54=0,Obstruction54=0,ObstructionClock54=0;
        int32 CycleStage = 0;
        float FlashUntil = 0;
        float Spin=0,Heat=0,BarrelAngle=0; bool Overheated=false;
        bool bChargeOnly = false;
        bool bResumeTubeAfterCharge = false;
        int32 RevolverInsertions = 0;
        TWeakObjectPtr<UStaticMeshComponent> FeedBox;
    };

    FRuntime& State(ALWCharacter& P)
    {
        static TMap<TWeakObjectPtr<ALWCharacter>, FRuntime> States;
        for (auto It = States.CreateIterator(); It; ++It)
            if (!It.Key().IsValid()) It.RemoveCurrent();
        return States.FindOrAdd(TWeakObjectPtr<ALWCharacter>(&P));
    }

    bool Direct(int Index){return Index==9||Index==14||Index==16;}
    bool ValidIndex(int32 Index) { return Index >= 0 && Index < UE_ARRAY_COUNT(Specs); }
    bool Automatic(const ALWCharacter& P) { return P.Weapon == 18 || P.Weapon == 20 || P.Weapon == 7 || P.Weapon==10 || P.Weapon==15 || ((P.Weapon == 5 || P.Weapon == 6 || P.Weapon==13) && P.FireMode == 1); }
    FName CanonicalAmmo(FName Id) { return Id == TEXT("ammo_762belt") ? FName(TEXT("ammo_762")) : Id; }

    FLWItemInstance* Find(TArray<FLWItemInstance>& Items, FGuid Id)
    {
        return Id.IsValid() ? Items.FindByPredicate([Id](const FLWItemInstance& I) { return I.Id == Id; }) : nullptr;
    }
    const FLWItemInstance* Find(const TArray<FLWItemInstance>& Items, FGuid Id)
    {
        return Id.IsValid() ? Items.FindByPredicate([Id](const FLWItemInstance& I) { return I.Id == Id; }) : nullptr;
    }
    bool Equipped(const FLWItemInstance& I)
    {
        const auto& D = LWItems::Def(I.Definition);
        return D.Category == TEXT("Weapon") && ValidIndex(D.WeaponIndex) &&
            !I.Slot.IsNone() && I.Slot != LoadedSlot && LWItems::CanEquip(I, I.Slot);
    }
    const FLWItemInstance* Gun(const ALWCharacter& P)
    {
        const auto* I = P.ActiveGun();
        return I && Equipped(*I) && LWItems::Def(I->Definition).WeaponIndex == P.Weapon ? I : nullptr;
    }
    bool Compatible(const FLWItemInstance& Weapon, const FLWItemInstance& Mag)
    {
        const auto& W = LWItems::Def(Weapon.Definition);
        const auto& M = LWItems::Def(Mag.Definition);
        return Mag.Id.IsValid() && Mag.Count == 1 && M.Category == TEXT("Magazine") &&
            !W.MagazineType.IsNone() && Mag.Definition == W.MagazineType &&
            CanonicalAmmo(W.AmmoType) == CanonicalAmmo(M.AmmoType) &&
            Mag.Rounds >= 0 && Mag.Rounds <= M.Capacity && !Mag.LoadedMagazine.IsValid();
    }
    bool Referenced(const TArray<FLWItemInstance>& Items, FGuid Magazine, FGuid Except = FGuid())
    {
        return Items.ContainsByPredicate([&](const FLWItemInstance& I) { return I.Id != Except && I.LoadedMagazine == Magazine; });
    }
    bool Unique(const TArray<FLWItemInstance>& Items, FGuid Id)
    {
        int32 Count = 0;
        for (const auto& I : Items) if (I.Id == Id) ++Count;
        return Id.IsValid() && Count == 1;
    }
    const FLWItemInstance* Magazine(const TArray<FLWItemInstance>& Items, const FLWItemInstance& Weapon)
    {
        const auto* M = Find(Items, Weapon.LoadedMagazine);
        return M && Unique(Items,M->Id) && Unique(Items,Weapon.Id) && M->Slot == LoadedSlot && Compatible(Weapon, *M) &&
            !Referenced(Items, M->Id, Weapon.Id) ? M : nullptr;
    }
    bool ValidCylinder(const FLWItemInstance& I)
    {
        if (I.Cylinder.Num() != (LWItems::Def(I.Definition).Capacity) || I.CylinderIndex < 0 || I.CylinderIndex >= I.Cylinder.Num()) return false;
        for (int32 Round : I.Cylinder) if (Round < 0 || Round > 2) return false;
        return true;
    }

    // The old magazine remains associated with the gun while it is in the loading hand.
    // Seating is the atomic ownership commit: spare -> Loaded; old -> spare's grid cells.
    // Cancelling extraction simply reseats the original. There are never unlinked Loaded
    // magazines, off-grid items, or save-only "in-hand" items. This also makes CancelReload
    // pointer-stable for LWSurvival::UnloadItem, which retains an item pointer across it.
    bool SwapMagazine(TArray<FLWItemInstance>& Items, FGuid GunId, FGuid SpareId)
    {
        const auto* G = Find(Items, GunId);
        const auto* S = Find(Items, SpareId);
        if (!G || !Unique(Items,GunId) || !Unique(Items,SpareId) || !Equipped(*G) || !S || !S->Slot.IsNone() || !Compatible(*G, *S) ||
            Referenced(Items, SpareId) || !LWItems::Fits(Items, *S, S->X, S->Y, S->bRotated, 12, LWItems::InventoryHeight(Items), S->Id)) return false;
        const auto* Old = Magazine(Items, *G);
        if (G->LoadedMagazine.IsValid() && !Old) return false;
        const int32 X = S->X, Y = S->Y;
        const bool Rotated = S->bRotated;
        const FGuid OldId = Old ? Old->Id : FGuid();
        auto Next = Items;
        auto* NewGun = Find(Next, GunId);
        auto* Spare = Find(Next, SpareId);
        NewGun->LoadedMagazine.Invalidate();
        Spare->Slot = LoadedSlot;
        Spare->X = Spare->Y = -1;
        Spare->bRotated = false;
        if (OldId.IsValid())
        {
            auto* Removed = Find(Next, OldId);
            Removed->Slot = NAME_None;
            if (LWItems::Fits(Next, *Removed, X, Y, Rotated, 12, LWItems::InventoryHeight(Next), OldId))
            {
                Removed->X = X; Removed->Y = Y; Removed->bRotated = Rotated;
            }
            else
            {
                auto ToPlace = *Removed;
                Next.RemoveAll([OldId](const FLWItemInstance& I) { return I.Id == OldId; });
                if (!LWItems::Place(Next, ToPlace, 12, LWItems::InventoryHeight(Next))) return false;
            }
        }
        // Place rejects a still-referenced magazine. Reacquire after the possible RemoveAll/Add.
        Find(Next,GunId)->LoadedMagazine = SpareId;
        Items = MoveTemp(Next);
        return true;
    }

    FGuid BestSpare(const TArray<FLWItemInstance>& Items, const FLWItemInstance& G)
    {
        FGuid Result;
        int32 Rounds = 0;
        for (const auto& I : Items)
            if (I.Slot.IsNone() && Compatible(G, I) && !Referenced(Items, I.Id) && I.Rounds > Rounds)
            { Result = I.Id; Rounds = I.Rounds; }
        return Result;
    }

    bool FeedChamber(TArray<FLWItemInstance>& Items, FGuid GunId)
    {
        auto* G = Find(Items, GunId);
        if (!G || !Equipped(*G) || G->Chamber == 1) return false; // Tactical reload never ejects a live round.
        const int32 Index = LWItems::Def(G->Definition).WeaponIndex;
        if (Index < 2 || Index == 3 || G->Chamber < 0 || G->Chamber > 2) return false;
        G->Chamber = 0; // A spent case is not ammunition.
        if (Index == 2)
        {
            if (G->Rounds <= 0) return false;
            --G->Rounds; G->Chamber = 1; return true;
        }
        const auto* Loaded = Magazine(Items, *G);
        if (!Loaded || Loaded->Rounds <= 0) return false;
        auto* M = Find(Items, Loaded->Id);
        --M->Rounds; G->Chamber = 1;
        return true;
    }

    bool InsertLooseRound(TArray<FLWItemInstance>& Items, FGuid GunId)
    {
        auto* G = Find(Items, GunId);
        if (!G || !Equipped(*G)) return false;
        const auto& D = LWItems::Def(G->Definition);
        const int32 Index = D.WeaponIndex;
        int32 Empty = INDEX_NONE;
        if (Index == 2)
        {
            if (G->Rounds < 0 || G->Rounds >= TubeCapacity) return false;
        }
        else if ((Index == 3 || (Index==8 || Index==11)) && ValidCylinder(*G)) Empty = G->Cylinder.IndexOfByKey(0);
        else return false;
        if ((Index == 3 || (Index==8 || Index==11)) && Empty == INDEX_NONE) return false;
        auto* Loose = Items.FindByPredicate([&](const FLWItemInstance& I)
        {
            return I.Slot.IsNone() && I.Count > 0 && LWItems::Def(I.Definition).Category == TEXT("Ammo") &&
                CanonicalAmmo(I.Definition) == CanonicalAmmo(D.AmmoType);
        });
        if (!Loose) return false;
        --Loose->Count;
        if (Index == 2) ++G->Rounds; else G->Cylinder[Empty] = 1;
        Items.RemoveAll([](const FLWItemInstance& I) { return I.Count == 0 && LWItems::Def(I.Definition).Category == TEXT("Ammo"); });
        return true;
    }

    bool FireCylinder(FLWItemInstance& G)
    {
        if (!ValidCylinder(G)) return false;
        const int32 Index = G.CylinderIndex;
        const bool Live = G.Cylinder[Index] == 1;
        if (Live) G.Cylinder[Index] = 2;
        G.CylinderIndex = (Index + 1) % G.Cylinder.Num(); // Empty chambers also advance on a trigger pull.
        return Live;
    }

    void EjectSpent(FLWItemInstance& G)
    {
        for (int32& Round : G.Cylinder) if (Round == 2) Round = 0;
    }
    void Sound(ALWCharacter& P, FName Name, float Volume = .65f, float Pitch = 1.f)
    {
        if (P.World) P.World->Sound(Name, P.Camera ? P.Camera->GetComponentLocation() : P.GetActorLocation(), Volume, Pitch);
    }
    const TCHAR* ChargeSound(int32 Index)
    {
        switch (Index)
        {
        case 2: return TEXT("Pump");
        case 4: return TEXT("SniperBolt");
        case 5: return TEXT("SMGCharge");
        case 6: return TEXT("RifleCharge");
        default: return TEXT("LMGClose"); // Catalog's heavy latch event also supplies the LMG charging clack.
        }
    }
    const TCHAR* MagOutSound(int32 Index)
    {
        return Index == 4 ? TEXT("SniperMagOut") : Index == 5 ? TEXT("SMGMagOut") : TEXT("RifleMagOut");
    }
    const TCHAR* MagInSound(int32 Index)
    {
        return Index == 4 ? TEXT("SniperMagIn") : Index == 5 ? TEXT("SMGMagIn") : TEXT("RifleMagIn");
    }
    float Smooth(float A, float B, float T)
    {
        const float X = FMath::Clamp((T - A) / FMath::Max(.001f, B - A), 0.f, 1.f);
        return X * X * (3 - 2 * X);
    }
    float Pulse(float A, float Peak, float B, float T) { return Smooth(A, Peak, T) * (1 - Smooth(Peak, B, T)); }
    FQuat Axis(FVector V, float Degrees) { return FQuat(V, FMath::DegreesToRadians(Degrees)); }
    FVector MovingRest(int32 Index)
    {
        if(Index==16)return FVector(10,0,0);if(Index==17)return FVector(-7,0,-12);if(Index==18)return FVector(10,0,-5);if(Index==20)return FVector(24,0,3);
        if(Index==11)return FVector(10,0,0);if(Index==10)return FVector(20,0,0);if(Index==12)return FVector(-3,0,-6);if(Index==13)return FVector(9.7,0,-5.2);if(Index==14)return FVector(13,0,3);if(Index==15)return FVector(6,-9,-8);if(Index==9)return FVector(6,0,3);
        switch (Index)
        {
        case 3: return FVector(8,0,4.5);
        case 4: return FVector(2,0,4);
        case 5: return FVector(8.4,0,-4.7);
        case 6: return FVector(9.7,0,-5.2);
        case 7: return FVector(17.3,0,6.8);
        case 8: return FVector(10,0,0);
        default: return FVector::ZeroVector;
        }
    }
    FVector FeedRest(int32 Index) { return Index==10?FVector(2,12,-10):Index==12?FVector::ZeroVector:Index == 4 ? FVector(7,0,-4.7) : FVector(6.3,-6.5,2.1); }
    FVector LeftRest(int32 Index)
    {
        if(Index==16)Index=8;if(Index==17)Index=3;if(Index==18||Index==20)Index=6;
        if(Index==11)Index=8;if(Index==13)Index=6;if(Index==12||Index==14)Index=3;
        switch (Index)
        {
        case 8: return FVector(35,-3,-6);
        case 2: return FVector(39,-3,-6);
        case 3: return FVector(-6.5,-7,-18.3);
        case 4: return FVector(32,-3.5,-9);
        case 5: return FVector(20.5,-4,-6.5);
        case 6: return FVector(29.9,-3.5,-11);
        default: return FVector(30,-4.7,-11.1);
        }
    }
    FVector RightRest(int32 Index)
    {
        if(Index==16)Index=8;if(Index==17)Index=3;if(Index==18||Index==20)Index=6;
        if(Index==11)Index=8;if(Index==13)Index=6;if(Index==12||Index==14)Index=3;
        switch (Index)
        {
        case 8: return FVector(-6,2,-11);
        case 2: return FVector(3,2,-11);
        case 3: return FVector(-9.5,1.5,-16.5);
        case 4: return FVector(-15,2,-12);
        case 5: return FVector(-12,1.7,-16);
        case 6: return FVector(-12,1.5,-16);
        default: return FVector(-13,1.4,-16);
        }
    }
    FRotator SupportRotation() { return FRotator(8,0,55); }
    FVector ViewRest(int32 Index, bool Aiming)
    {
        if(Index==16)return Aiming?FVector(62,0,-15):FVector(62,13,-20);if(Index==17)return Aiming?FVector(65,0,-18):FVector(56,16,-23);if(Index==19)return FVector(40,9,-15);if(Index==18||Index==20)Index=6;
        // Camera-relative centimetres. Keep the receiver ahead of the near plane;
        // the original 15 cm ADS offset put stocks and wrists through the camera.
        if(Index==13)Index=6;if(Index==12||Index==14)Index=3;
        static const FVector Hip[] = {
            FVector(22,21,-22), FVector(22,21,-22), FVector(54,10,-14), FVector(40,8,-13),
            FVector(64,10,-17), FVector(52,10,-14), FVector(60,11,-16), FVector(70,12,-18)
        };
        static const FVector Aim[] = {
            FVector(22,21,-22), FVector(22,21,-22), FVector(52,0,-6.3), FVector(40,0,-11.3),
            FVector(120,3,-24), FVector(55,0,-9.8), FVector(65,0,-11.3), FVector(80,0,-10.5)
        };
        // The sniper's glass is an opaque static surface, not a rendered optic.
        // Keep it below the zoomed reticle instead of looking into its solid cap.
        if((Index==8 || Index==11))return Aiming?FVector(55,0,-6):FVector(58,11,-16);
        return (Aiming ? Aim : Hip)[FMath::Clamp(Index,0,7)];
    }
    struct FPresentationPose { FVector Location; FRotator Rotation; };
    FPresentationPose PresentationPose(int32 Index)
    {
        // At 1280x720 / 86 degrees, the working parts occupy the clear space
        // above the bottom HUD. Positive roll turns mag extraction toward screen left.
        if(Index==16)Index=8;if(Index==17)Index=3;if(Index==18||Index==20)Index=6;
        if(Index==11)Index=8;if(Index==13)Index=6;if(Index==12||Index==14)Index=3;
        switch (Index)
        {
        case 8: return {FVector(68,4,-5),FRotator(0,-25,25)};
        case 2: return {FVector(67,4,-7),FRotator(0,-24,50)};
        case 3: return {FVector(45,5,-10),FRotator(6,-20,18)};
        case 4: return {FVector(85,5,-10),FRotator(-2,-24,42)};
        case 5: return {FVector(68,5,-7),FRotator(0,-24,55)};
        case 6: return {FVector(78,5,-7),FRotator(0,-26,55)};
        default: return {FVector(86,6,-16),FRotator(0,-22,24)};
        }
    }
    void StartCharge(ALWCharacter& P, bool ResumeTube = false)
    {
        P.CancelReload();
        auto& S = State(P);
        S.ReloadGun = P.ActiveWeaponId;
        S.bChargeOnly = true;
        S.bResumeTubeAfterCharge = ResumeTube;
        P.bReloading = true;
        P.ReloadPhase = 0;
        P.ReloadElapsed = 0;
        P.ReloadDuration = P.ReloadTimer = .9f;
        P.bAim = false; P.bSprint = false;
        Sound(P, ChargeSound(P.Weapon), .65f, P.Weapon == 7 ? .75f : 1.f);
    }
    bool InShelter(const ALWCharacter& P)
    {
        return P.bSafehouse || ALWWorld::IsSafePosition(P.GetActorLocation()) ||
            (P.Camera && ALWWorld::IsSafePosition(P.Camera->GetComponentLocation()));
    }
}

FString ALWCharacter::WeaponName() const
{
    const auto* G = LWWeapons::Gun(*this);
    return G ? (LWMods::Modifier(G).IsNone()?FString():LWMods::Modifier(G).ToString()+TEXT(" "))+FString(LWMods::TierName(G->WeaponTier))+TEXT(" ")+(G->CustomName39.IsEmpty()?LWWeapons::Specs[Weapon].Name:G->CustomName39) : TEXT("UNARMED");
}

void ALWCharacter::Equip(int32 Index)
{
    using namespace LWWeapons;
    if (!ValidIndex(Index)) return;
    FGuid Selected;
    if (const auto* Current = ActiveGun(); Current && Equipped(*Current) && LWItems::Def(Current->Definition).WeaponIndex == Index)
        Selected = Current->Id;
    if (!Selected.IsValid())
        for (const auto& I : Inventory)
            if (Equipped(I) && LWItems::Def(I.Definition).WeaponIndex == Index) { Selected = I.Id; break; }
    if (!Selected.IsValid())
    {
        // EquipItem calls back into Equip only after assigning a legitimate equipment slot.
        // Return immediately here; never continue using the pre-EquipItem inventory pointer.
        for (const auto& I : Inventory)
            if (I.Slot.IsNone() && LWItems::Def(I.Definition).WeaponIndex == Index)
            { const FGuid Id = I.Id; EquipItem(Id); return; }
        Notify(TEXT("WEAPON NOT EQUIPPED OR OWNED"), 2);
        return;
    }
    auto& S = State(*this);
    if (S.VisualGun == Selected && ActiveWeaponId == Selected && Weapon == Index)
    { SyncAmmoHUD(); return; } // Repeated number-key input cannot reset the firing cooldown.
    CancelReload();
    StopAttack();
    S.CycleGun.Invalidate(); S.CycleElapsed = 0; S.CycleStage = 0;
    AttackTimer = AttackDuration = 0;
    ActiveWeaponId = Selected; Weapon = Index;
    FireMode = (Index == 5 || Index == 6 || Index == 7 || Index==13) ? 1 : 0;
    S.VisualGun = Selected;
    ConfigureWeaponParts();
    SyncAmmoHUD();
}

void ALWCharacter::EquipCrowbar() { if(Vehicle){Vehicle->ChangeSeat(-1);return;} if(SecurityMode==2){ChooseWire(0);return;} EquipSlot(TEXT("Primary")); }
void ALWCharacter::EquipBat() { if(Vehicle){Vehicle->ChangeSeat(0);return;} if(SecurityMode==2){ChooseWire(1);return;} EquipSlot(TEXT("Secondary")); }
void ALWCharacter::EquipShotgun() { if(Vehicle){Vehicle->ChangeSeat(1);return;} if(SecurityMode==2){ChooseWire(2);return;} EquipSlot(TEXT("Sidearm")); }
void ALWCharacter::EquipRevolver() { if(Vehicle){Vehicle->ChangeSeat(2);return;} if(SecurityMode==2){ChooseWire(3);return;} EquipSlot(TEXT("Melee")); }
void ALWCharacter::EquipSniper() { if(Vehicle){Vehicle->ChangeSeat(3);return;} if(SecurityMode==2){ChooseWire(4);return;} EquipSlot(TEXT("RigPrimary")); }
void ALWCharacter::EquipSMG() { if(Vehicle){Vehicle->ChangeSeat(4);return;} if(SecurityMode==2){ChooseWire(5);return;} }
void ALWCharacter::EquipRifle() { if(Vehicle){Vehicle->ChangeSeat(5);return;}}
void ALWCharacter::EquipLMG() { if(Vehicle){Vehicle->ChangeSeat(6);return;}}
void ALWCharacter::NextWeapon()
{
    if(QuickContainer54()){QuickLootStep54(-1);return;}
    if(Vehicle){Vehicle->ChangeSeat((Vehicle->PlayerSeat+1+1+Vehicle->Spec().Seats)%Vehicle->Spec().Seats-1);return;}
    if (!CanAct()) return;
    for (int32 Step = 1; Step <= UE_ARRAY_COUNT(LWWeapons::Specs); ++Step)
    {
        const int32 Index = (FMath::Clamp(Weapon,0,int(UE_ARRAY_COUNT(LWWeapons::Specs))-1) + Step) % UE_ARRAY_COUNT(LWWeapons::Specs);
        if (Inventory.ContainsByPredicate([Index](const FLWItemInstance& I) { return LWWeapons::Equipped(I) && LWItems::Def(I.Definition).WeaponIndex == Index; }))
        { Equip(Index); return; }
    }
}
void ALWCharacter::PreviousWeapon()
{
    if(QuickContainer54()){QuickLootStep54(1);return;}
    if(Vehicle){Vehicle->ChangeSeat((Vehicle->PlayerSeat+1+-1+Vehicle->Spec().Seats)%Vehicle->Spec().Seats-1);return;}
    if (!CanAct()) return;
    for (int32 Step = 1; Step <= UE_ARRAY_COUNT(LWWeapons::Specs); ++Step)
    {
        const int32 Index = (FMath::Clamp(Weapon,0,int(UE_ARRAY_COUNT(LWWeapons::Specs))-1) + UE_ARRAY_COUNT(LWWeapons::Specs) - Step) % UE_ARRAY_COUNT(LWWeapons::Specs);
        if (Inventory.ContainsByPredicate([Index](const FLWItemInstance& I) { return LWWeapons::Equipped(I) && LWItems::Def(I.Definition).WeaponIndex == Index; }))
        { Equip(Index); return; }
    }
}

void ALWCharacter::SyncAmmoHUD()
{
    using namespace LWWeapons;
    Shells = ReserveShells = 0;
    const auto* G = Gun(*this);
    if (!G || Weapon < 2) return;
    if ((Weapon == 3 || (Weapon==8 || Weapon==11)))
    {
        for (int32 Round : G->Cylinder) if (Round == 1) ++Shells;
    }
    else
    {
        Shells = G->Chamber == 1 ? 1 : 0;
        if(Direct(Weapon))Shells=G->Rounds;
        else if (Weapon == 2) Shells += FMath::Max(0,G->Rounds);
        else if (const auto* M = Magazine(Inventory,*G)) Shells += M->Rounds;
    }
    const FName Ammo = LWItems::Def(G->Definition).AmmoType;
    ReserveShells = AmmoCount(Ammo);
    if (CanonicalAmmo(Ammo) == TEXT("ammo_762")) ReserveShells += AmmoCount(TEXT("ammo_762belt"));
}

FString ALWCharacter::AmmoStatus() const
{
    using namespace LWWeapons;
    const auto* G = Gun(*this);
    if (!G) return TEXT("PUNCH // 8 STAMINA");
    if(Weapon==19)return TEXT("NO AMMO REQUIRED");
    if(Weapon==16)return FString::Printf(TEXT("BARRELS %d/10 // %d SHELLS"),G->Rounds,AmmoCount(TEXT("ammo_12g")));
    if (Weapon < 2) return Weapon == 0 ? TEXT("MELEE // 16 STAMINA") : TEXT("MELEE // 26 STAMINA");
    if ((Weapon == 3 || (Weapon==8 || Weapon==11)))
    {
        FString Chambers;
        for (int32 I = 0; I < G->Cylinder.Num(); ++I)
        {
            if (I == G->CylinderIndex) Chambers += TEXT("[");
            Chambers += G->Cylinder[I] == 1 ? TEXT("O") : G->Cylinder[I] == 2 ? TEXT("X") : TEXT("-");
            Chambers += I == G->CylinderIndex ? TEXT("] ") : TEXT(" ");
        }
        return ((Weapon==8 || Weapon==11)?TEXT("BARRELS "):TEXT("CYL ")) + Chambers + FString::Printf(TEXT("// %d LOOSE"), AmmoCount(LWItems::Def(G->Definition).AmmoType));
    }
    const TCHAR* Chamber = G->Chamber == 1 ? TEXT("LIVE") : G->Chamber == 2 ? TEXT("SPENT") : TEXT("EMPTY");
    if (Weapon == 2) return FString::Printf(TEXT("TUBE %d/5 + %s // %d SHELLS"), G->Rounds, Chamber, AmmoCount(TEXT("ammo_12g")));
    if(Weapon==9)return FString::Printf(TEXT("MISSILE %d/1 // %d MISSILES"),G->Rounds,AmmoCount(TEXT("ammo_rocket")));
    if(Weapon==14)return FString::Printf(TEXT("CHARGE %d/10 // %d BATTERIES"),G->Rounds,AmmoCount(TEXT("battery")));
    int32 Spares = 0, SpareRounds = 0;
    for (const auto& I : Inventory)
        if (I.Slot.IsNone() && Compatible(*G,I) && !Referenced(Inventory,I.Id)) { ++Spares; SpareRounds += I.Rounds; }
    const auto* M = Magazine(Inventory,*G);
    return FString::Printf(TEXT("%s %d/%d + %s // %d MAGS (%d) // %s"),
        Weapon == 7 ? TEXT("BELT") : TEXT("MAG"), M ? M->Rounds : 0,
        LWItems::Def(G->Definition).Capacity, Chamber, Spares, SpareRounds,
        Automatic(*this) ? TEXT("AUTO") : TEXT("SEMI"));
}

void ALWCharacter::ToggleFireMode()
{
    if(Vehicle){if(!IsUIOpen())VehicleWipers();return;}
    if (!CanAct() || !LWWeapons::Gun(*this) || (Weapon != 5 && Weapon != 6 && Weapon!=13)) return;
    bTrigger = false;
    FireMode = FireMode == 1 ? 0 : 1;
    LWWeapons::Sound(*this,TEXT("Click"),.45f);
    Notify(FireMode == 1 ? TEXT("FIRE SELECTOR // AUTOMATIC") : TEXT("FIRE SELECTOR // SEMI-AUTOMATIC"),1.5f);
}

void ALWCharacter::ConsumeUIAttack(){bUIAttackHeld=true;LastUIAttackFrame=GFrameCounter;StopAttack();}
void ALWCharacter::ReleaseAttackInput(){StopAttack();bUIAttackHeld=false;}

void ALWCharacter::StopAttack()
{
    bTrigger = false;if(Vehicle)Vehicle->TurretTrigger57=false;
    LWWeapons::State(*this).Buffered54=0;
    LWWeapons::State(*this).FlashUntil = 0;
    if (Muzzle) Muzzle->SetIntensity(0);
}

void ALWCharacter::ChamberRound()
{
    // During extraction the old magazine is still owned by the gun, but cannot feed.
    if (bReloading && Weapon >= 4 && Weapon != 8 && Weapon != 11 && !LWWeapons::State(*this).bChargeOnly && ReloadPhase < (Weapon == 7 ? 3 : 2)) return;
    if (LWWeapons::Gun(*this)) LWWeapons::FeedChamber(Inventory,ActiveWeaponId);
    SyncAmmoHUD();
}

void ALWCharacter::Attack()
{
    using namespace LWWeapons;
    if(IsUIOpen()){ConsumeUIAttack();return;}
    if(bUIAttackHeld||LastUIAttackFrame==GFrameCounter){StopAttack();return;}
    if(Vehicle&&Vehicle->HasTurret57()&&Vehicle->PlayerSeat==0){Vehicle->TurretTrigger57=true;return;}
    if (!CanAct() || !World || !Camera) { StopAttack(); return; }
    if (!Gun(*this)) { Punch(); return; }
    if (InShelter(*this)) { StopAttack(); Notify(TEXT("SAFEHOUSE // WEAPONS SAFE"),1.5f); return; }
    bSprint=false;
    const bool WasHeld = bTrigger;
    bTrigger = true;
    if (WasHeld && !Automatic(*this)) return;
    if (AttackTimer > 0){if(!WasHeld&&AttackTimer<.14f)State(*this).Buffered54=.14f;return;}
    if (bReloading) { CancelReload(); return; } // One press interrupts; another deliberately fires.
    auto& SpinState=State(*this);
    if(Weapon==10&&(SpinState.Spin<.98f||SpinState.Overheated))return;
    if(Weapon==20&&BudgetBrace50<.99f)return;
    const FSpec& S = Specs[Weapon];
    auto* G = ActiveGun();
    const FVector Start = Camera->GetComponentLocation(), ForwardVector = Camera->GetForwardVector();
    FCollisionQueryParams Params(SCENE_QUERY_STAT(LWWeaponTrace),true,this);
    if (Weapon < 2)
    {
        const float Cost = Weapon == 0 ? 16.f : 26.f;
        if (Stamina < Cost) { bTrigger = false; Notify(TEXT("EXHAUSTED // CATCH YOUR BREATH"),1); return; }
        Stamina -= Cost;
        AttackDuration = AttackTimer = S.Interval*LWMods::Interval(ActiveGun());
        Sound(*this,TEXT("Swing"),.7f,Weapon == 0 ? 1.15f : .8f);
        World->Noise(Start,700);
        FHitResult Hit;
        if (GetWorld()->SweepSingleByChannel(Hit,Start,Start+ForwardVector*S.Range,FQuat::Identity,
            ECC_Visibility,FCollisionShape::MakeSphere(26),Params))
        {
            if(Cast<ALWWorldObject>(Hit.GetActor()))UGameplayStatics::ApplyPointDamage(Hit.GetActor(),S.Damage*LWMods::Damage(ActiveGun())*CombatMultiplier(),ForwardVector,Hit,Controller,this,nullptr);
            auto* Z = Cast<ALWZombie>(Hit.GetActor());
            World->Sound(Z ? TEXT("FleshHit") : TEXT("MetalHit"),Hit.ImpactPoint,.8f);
            World->Noise(Hit.ImpactPoint,1500);
            if (Z && !ALWWorld::IsSafePosition(Z->GetActorLocation()))
            {
                ALWWeaponEffect::Impact(this,Hit,ForwardVector,S.Damage*LWMods::Damage(ActiveGun())*CombatMultiplier()*LWMods::Shots(ActiveGun()),LWMods::Modifier(ActiveGun()),Weapon);
                Z->Stagger = Weapon == 0 ? .25f : .65f;
                HitMarker = .18f;
            }
        }
        return;
    }
    bool Fired = false;
    if(Weapon==19)Fired=true;
    else if(Weapon==16){if(G->Rounds==10){G->Rounds=0;Fired=true;}}
    else if(Direct(Weapon)){if(G->Rounds>0){--G->Rounds;Fired=true;}}
    else if ((Weapon == 3 || (Weapon==8 || Weapon==11)))
    {
        if (!ValidCylinder(*G)) { bTrigger = false; Notify(TEXT("INVALID CYLINDER STATE")); return; }
        Fired = FireCylinder(*G);
    }
    else if (G->Chamber == 1)
    {
        G->Chamber = 2;
        Fired = true;
        if (Weapon != 2 && Weapon != 4) FeedChamber(Inventory,ActiveWeaponId);
    }
    else
    {
        const auto* M = Magazine(Inventory,*G);
        if (G->Chamber == 2 || (Weapon == 2 ? G->Rounds > 0 : M && M->Rounds > 0))
        { StartCharge(*this); return; }
    }
    if (!Fired)
    {
        Sound(*this,TEXT("Click"),.6f);
        AttackDuration = AttackTimer = Weapon == 3 ? S.Interval : .2f;
        bTrigger = false; // No automatic dry-click loop.
        SyncAmmoHUD();
        Notify(TEXT("EMPTY // [R] RELOAD"),1.5f);
        return;
    }
    AttackDuration = AttackTimer = S.Interval*LWMods::Interval(ActiveGun());
    if(Weapon==10){SpinState.Heat=FMath::Min(1.f,SpinState.Heat+.018f);if(SpinState.Heat>=1){SpinState.Overheated=true;Notify(TEXT("BARRELS OVERHEATED"),1.5f);}}
    auto& Runtime = State(*this);
    Runtime.FlashUntil = GetWorld()->GetRealTimeSeconds()+.07f;
    if (Muzzle) { Muzzle->SetVisibility(true); Muzzle->SetIntensity(40000); }
    if (Weapon == 2 || Weapon == 4)
    {
        Runtime.CycleGun = ActiveWeaponId; Runtime.CycleElapsed = 0; Runtime.CycleStage = 0;
    }
    const FVector Bore = WeaponMesh ? WeaponMesh->GetComponentTransform().TransformPosition(S.Muzzle) : Start;
    // Local reports use listener-relative playback from the outset: no distance,
    // panning, occlusion or random pitch changes as the muzzle/aim pose moves.
    // Other listeners still hear a positional report at the actual muzzle.
    ULWAudioCatalog::PlaySlot(this,S.FireSound,Bore,LWMods::Has(ActiveGun(),TEXT("att_suppressor62"))?.5f:1.f,1,true,
        World->Attenuation,World->LoudAttenuation,IsLocallyControlled());
    World->Noise(Bore,S.Noise*(LWMods::Has(ActiveGun(),TEXT("att_suppressor62"))?.35f:1.f));
    RecoilBloom54=FMath::Min(.85f,RecoilBloom54+.085f);
    ALWZombie::SuppressAlong54(this,Start,Start+ForwardVector*S.Range);
    Runtime.Kick54+=FVector2D(-S.Recoil*LWMods::Recoil(ActiveGun())*FMath::Max(.3f,1-Stat(TEXT("recoil"))),FMath::FRandRange(-S.YawRecoil,S.YawRecoil)*LWMods::Recoil(ActiveGun()));
    if(const auto* Custom=ActiveGun();Custom&&!Custom->Parts39.IsEmpty()){
        // One action consumes one cartridge. Every mounted barrel shares that discharge.
        // Local orientation is intentional, including backwards/sideways barrels.
        const auto Barrels=LWParts39::Barrels(*Custom);
        for(const auto& T:Barrels){
            const FVector Origin=WeaponMesh->GetComponentTransform().TransformPosition(T.GetLocation());
            const FVector Heading=WeaponMesh->GetComponentTransform().TransformVectorNoScale(T.GetRotation().GetForwardVector());
            FHitResult Block;const bool Covered=GetWorld()->LineTraceSingleByChannel(Block,Start,Origin,ECC_Visibility,Params);
            for(int Pellet=0;Pellet<S.Pellets*LWMods::Shots(Custom);++Pellet){
                const FVector Dir=FMath::VRandCone(Heading,FMath::DegreesToRadians((bAim?S.AimSpread:S.HipSpread)*Spread54()*LWMods::Spread(Custom,bAim)*FMath::Max(.3f,1-Stat(TEXT("spread")))));
                if(!Covered&&(Weapon==9||Weapon==15)){auto* FX=GetWorld()->SpawnActor<ALWWeaponEffect>(Origin,Dir.Rotation());if(FX)FX->Initialize(this,Dir,Weapon==9?0:1,S.Damage*LWMods::Damage(Custom)*CombatMultiplier(),LWMods::Modifier(Custom));continue;}
                FHitResult Hit=Block;if(!Covered)GetWorld()->LineTraceSingleByChannel(Hit,Origin,Origin+Dir*S.Range,ECC_Visibility,Params);
                if(Pellet==0)ALWWeaponEffect::Gunfire(this,World,Origin,Hit.bBlockingHit?Hit.ImpactPoint:Origin+Dir*S.Range,false);
                if(Hit.bBlockingHit){auto* Z=Cast<ALWZombie>(Hit.GetActor());const bool Head=Z&&Hit.ImpactPoint.Z>Z->GetActorLocation().Z+49;const float Falloff=FMath::Clamp(1-(float(Hit.Distance)/(1+Stat(TEXT("range")))-S.FalloffStart)/S.FalloffSpan,S.MinimumDamage,1.f);ALWWeaponEffect::Impact(this,Hit,Dir,S.Damage*LWMods::Damage(Custom)*CombatMultiplier(Head)*Falloff*(Head?S.HeadMultiplier:1),LWMods::Modifier(Custom),Weapon);}
            }
        }SyncAmmoHUD();if(Weapon==16)StartRecoil50(-ForwardVector*2600+FVector(0,0,1000));return;
    }
    FHitResult BarrelBlock;
    const bool BlockedBarrel = GetWorld()->LineTraceSingleByChannel(BarrelBlock,Start,Bore,ECC_Visibility,Params) &&
        !Cast<ALWZombie>(BarrelBlock.GetActor());
    if((Weapon==9||Weapon==15)&&!BlockedBarrel){
        for(int N=0;N<LWMods::Shots(ActiveGun());N++){
            FHitResult Target;GetWorld()->LineTraceSingleByChannel(Target,Start,Start+ForwardVector*S.Range,ECC_Visibility,Params);
            FVector Aim=(Target.bBlockingHit?Target.ImpactPoint:Start+ForwardVector*S.Range)-Bore;
            FVector Dir=FMath::VRandCone(Aim.GetSafeNormal(),FMath::DegreesToRadians(Weapon==15?S.HipSpread:N>0?1.f:0.f));
            auto* Projectile=GetWorld()->SpawnActor<ALWWeaponEffect>(Bore,Dir.Rotation());
            if(Projectile)Projectile->Initialize(this,Dir,Weapon==9?0:1,S.Damage*LWMods::Damage(ActiveGun())*CombatMultiplier(),LWMods::Modifier(ActiveGun()));
        }SyncAmmoHUD();return;
    }
    for (int32 Pellet = 0; Pellet < S.Pellets*LWMods::Shots(ActiveGun()); ++Pellet)
    {
        if (BlockedBarrel){if(Pellet==0)UGameplayStatics::ApplyPointDamage(BarrelBlock.GetActor(),S.Damage*LWMods::Damage(ActiveGun())*CombatMultiplier(),ForwardVector,BarrelBlock,Controller,this,nullptr);break;} // Gun poking through cover cannot fire through it.
        const FVector Dir = FMath::VRandCone(ForwardVector,FMath::DegreesToRadians((bAim ? S.AimSpread : S.HipSpread)*Spread54()*LWMods::Spread(ActiveGun(),bAim)*FMath::Max(.3f,1-Stat(TEXT("spread")))));
        FHitResult Hit;
        if (!GetWorld()->LineTraceSingleByChannel(Hit,Start,Start+Dir*S.Range,ECC_Visibility,Params)) {
            FHitResult MuzzleHit;const FVector End=Start+Dir*S.Range;
            GetWorld()->LineTraceSingleByChannel(MuzzleHit,Bore,End,ECC_Visibility,Params);
            if(Pellet==0)ALWWeaponEffect::Gunfire(this,World,Bore,MuzzleHit.bBlockingHit?MuzzleHit.ImpactPoint:End,false);
            continue;
        }
        // The camera and muzzle must both have an unobstructed path to the same impact.
        FHitResult Obstruction;
        if (GetWorld()->LineTraceSingleByChannel(Obstruction,Bore,Hit.ImpactPoint,ECC_Visibility,Params) &&
            Obstruction.GetActor() != Hit.GetActor()) {
            if(Pellet==0)ALWWeaponEffect::Gunfire(this,World,Bore,Obstruction.ImpactPoint,false);
            continue;
        }
        if(Pellet==0)ALWWeaponEffect::Gunfire(this,World,Bore,Hit.ImpactPoint,false);
        auto* Z=Cast<ALWZombie>(Hit.GetActor());
        const float Falloff=FMath::Clamp(1-(float(Hit.Distance)/(1+Stat(TEXT("range")))-S.FalloffStart)/S.FalloffSpan,S.MinimumDamage,1.f);
        const bool Head=Z&&Hit.ImpactPoint.Z>Z->GetActorLocation().Z+49;
        ALWWeaponEffect::Impact(this,Hit,Dir,S.Damage*LWMods::Damage(ActiveGun())*CombatMultiplier(Head)*Falloff*(Head?S.HeadMultiplier:1.f),LWMods::Modifier(ActiveGun()),Weapon);
    }
    SyncAmmoHUD();
    if(Weapon==16)StartRecoil50(-ForwardVector*2600+FVector(0,0,1000));
}

void ALWCharacter::Reload()
{
    if(Vehicle){if(Vehicle->HasTurret57()&&Vehicle->PlayerSeat==0)Vehicle->ReloadTurret57();else Vehicle->Ignition();return;}
    using namespace LWWeapons;
    if (!CanAct() || !World || !Gun(*this) || Weapon < 2 || bReloading || AttackTimer > 0) return;
    auto* G = ActiveGun();
    const auto& D = LWItems::Def(G->Definition);
    auto& S = State(*this);
    S.RevolverInsertions = 0;
    ReloadMagazineId.Invalidate();
    if(Weapon==19){ReloadDuration=1.65f;}
    else if(Weapon==18&&Magazine(Inventory,*G)&&!BestSpare(Inventory,*G).IsValid()){ReloadDuration=2.8f;ReloadMagazineId=G->LoadedMagazine;}
    else if(Direct(Weapon)){if(G->Rounds>0||AmmoCount(D.AmmoType)<(Weapon==16?10:1)){Notify(TEXT("ALREADY LOADED OR NO AMMUNITION"));return;}ReloadDuration=Weapon==16?4.f:Weapon==9?3.f:1.4f;}
    else if (Weapon == 2)
    {
        if (G->Chamber != 1 && (G->Rounds > 0 || G->Chamber == 2)) { StartCharge(*this,true); return; }
        if (G->Rounds >= TubeCapacity || AmmoCount(D.AmmoType) <= 0) { Notify(TEXT("TUBE FULL OR NO LOOSE SHELLS"),1.5f); return; }
        ReloadDuration = .6f;
    }
    else if ((Weapon == 3 || (Weapon==8 || Weapon==11)))
    {
        if (!ValidCylinder(*G)) { Notify(TEXT("INVALID CYLINDER STATE")); return; }
        int32 Empty = 0, Spent = 0;
        for (int32 Round : G->Cylinder) { if (Round == 0) ++Empty; else if (Round == 2) ++Spent; }
        S.RevolverInsertions = FMath::Min(Empty+Spent,AmmoCount(D.AmmoType));
        if (Spent == 0 && S.RevolverInsertions == 0) { Notify(TEXT("CYLINDER FULL OR NO LOOSE ROUNDS"),1.5f); return; }
        ReloadDuration = (Weapon==8 || Weapon==11)?1.5f+.55f*S.RevolverInsertions:1.15f+.40f*S.RevolverInsertions;
    }
    else
    {
        const FGuid Spare = BestSpare(Inventory,*G);
        if (!Spare.IsValid())
        {
            const auto* M = Magazine(Inventory,*G);
            if (G->Chamber != 1 && (G->Chamber == 2 || (M && M->Rounds > 0))) { StartCharge(*this); return; }
            Notify(TEXT("NO LOADED SPARE MAGAZINE // LOAD ONE IN [I]"),2.5f); return;
        }
        auto Preview = Inventory;
        if (!SwapMagazine(Preview,ActiveWeaponId,Spare)) { Notify(TEXT("CANNOT STOW MAGAZINE // CHECK GRID AND MAGAZINE LINK")); return; }
        ReloadMagazineId = Spare;
        ReloadDuration = Weapon == 7 ? 6.f : Weapon == 4 ? 2.65f : Weapon == 5 ? 1.9f : 2.35f;
        if (G->Chamber == 1 && Weapon != 7) ReloadDuration = Weapon == 4 ? 1.75f : Weapon == 5 ? 1.3f : 1.65f;
    }
    if(Weapon==18)ReloadDuration=2.8f;
    bReloading = true; bTrigger = false; bAim = false; bSprint = false;
    ReloadElapsed = 0; ReloadPhase = 0; ReloadTimer = ReloadDuration;
    S.ReloadGun = ActiveWeaponId; S.bChargeOnly = false; S.bResumeTubeAfterCharge = false;
    S.CycleGun.Invalidate(); S.FlashUntil = 0;
    if (Muzzle) Muzzle->SetIntensity(0);
    if ((Weapon == 3 || (Weapon==8 || Weapon==11))) Sound(*this,(Weapon==8 || Weapon==11)?TEXT("DoubleBarrelOpen"):TEXT("RevolverOpen"));
    else if (Weapon == 7) Sound(*this,TEXT("LMGCover"));
    else if (Weapon >= 4) Sound(*this,MagOutSound(Weapon));
}

void ALWCharacter::CancelReload()
{
    using namespace LWWeapons;
    auto& S = State(*this);
    const bool WasReloading = bReloading;
    const auto* ReloadGun = FindItem(S.ReloadGun);
    const int32 ReloadWeapon = ReloadGun ? LWItems::Def(ReloadGun->Definition).WeaponIndex : INDEX_NONE;
    // Never roll ammunition back. Inserted rounds and a seated magazine stay committed.
    // No TArray mutations here: survival item operations retain pointers across this call.
    bReloading = false; bTrigger = false;
    ReloadElapsed = ReloadDuration = ReloadTimer = 0; ReloadPhase = 0;
    ReloadMagazineId.Invalidate();
    S.ReloadGun.Invalidate(); S.bChargeOnly = false; S.bResumeTubeAfterCharge = false; S.RevolverInsertions = 0; S.FlashUntil = 0;
    if (Muzzle) Muzzle->SetIntensity(0);
    if (WasReloading && Health > 0)
    {
        if ((ReloadWeapon == 3 || (ReloadWeapon==8 || ReloadWeapon==11))) Sound(*this,(ReloadWeapon==8 || ReloadWeapon==11)?TEXT("DoubleBarrelClose"):TEXT("RevolverClose"),.55f);
        else if (ReloadWeapon == 7) Sound(*this,TEXT("LMGClose"),.45f);
    }
    if (MovingPart)
    {
        MovingPart->SetRelativeLocation(MovingRest(Weapon));
        MovingPart->SetRelativeRotation(FRotator::ZeroRotator);
    }
    if (SecondPart) { SecondPart->SetRelativeLocation(FeedRest(Weapon)); SecondPart->SetRelativeRotation(FRotator::ZeroRotator); }
    if (LoadingHand) { LoadingHand->SetRelativeLocation(LeftRest(Weapon)); LoadingHand->SetRelativeRotation(SupportRotation()); }
    SyncAmmoHUD();
}

void ALWCharacter::UpdateReload(float Dt)
{
    using namespace LWWeapons;
    if (!bReloading) return;
    auto& S = State(*this);
    if (!Gun(*this) || S.ReloadGun != ActiveWeaponId || !CanAct()) { CancelReload(); return; }
    if (!FMath::IsFinite(Dt) || Dt < 0) return;
    ReloadElapsed += Dt*(1+Stat(TEXT("reload")));
    ReloadTimer = FMath::Max(0.f,ReloadDuration-ReloadElapsed);
    bool Changed = false;
    auto Complete = [&]()
    {
        // Completed closure sounds are scheduled below; do not play them twice through cancellation.
        bReloading = false;
        CancelReload();
        SyncAmmoHUD();
        PersistWorldChange();
    };
    if(Weapon==19){if(ReloadElapsed>=ReloadDuration){Sound(*this,TEXT("Click"),.5f,1.5f);Complete();}return;}
    if(Weapon==18&&ActiveGun()->LoadedMagazine==ReloadMagazineId){
        if(ReloadElapsed>=ReloadDuration){ChamberRound();Sound(*this,TEXT("RifleMagIn"));Complete();}return;
    }
    if(Direct(Weapon)){
        if(ReloadElapsed>=ReloadDuration){const FName Ammo=LWItems::Def(ActiveGun()->Definition).AmmoType;if(ConsumeSupply(Ammo,Weapon==16?10:1)){auto* Loaded=ActiveGun();Loaded->Rounds=Weapon==16?10:Weapon==14?10:1;Loaded->Chamber=0;Sound(*this,TEXT("LockOpen"));}Complete();}return;
    }
    if (S.bChargeOnly)
    {
        if (ReloadPhase == 0 && ReloadElapsed >= .32f)
        {
            if (auto* G = ActiveGun(); G && G->Chamber == 2) { G->Chamber = 0; Changed = true; }
            ReloadPhase = 1;
        }
        if (ReloadPhase == 1 && ReloadElapsed >= .78f) { ChamberRound(); ReloadPhase = 2; Changed = true; }
        if (ReloadElapsed >= ReloadDuration)
        {
            const bool ResumeTube = S.bResumeTubeAfterCharge;
            const float Remaining = ReloadElapsed-ReloadDuration;
            Complete();
            const auto* Current = Gun(*this);
            if (ResumeTube && Current && Weapon == 2 && Current->Rounds < TubeCapacity &&
                AmmoCount(LWItems::Def(Current->Definition).AmmoType) > 0)
            {
                Reload();
                if (Remaining > 0 && bReloading) UpdateReload(Remaining);
            }
            return;
        }
    }
    else if (Weapon == 2)
    {
        // First insertion .60 s; subsequent insertions .62 s, exactly as the original shotgun.
        while (ReloadElapsed >= ReloadDuration && bReloading)
        {
            if (!InsertLooseRound(Inventory,ActiveWeaponId)) { Complete(); return; }
            Sound(*this,TEXT("Reload"),.5f);
            ++ReloadPhase; Changed = true;
            const auto* G = ActiveGun();
            if (G->Chamber != 1)
            {
                // An empty gun first gets a shell, then a deliberate pump feeds it; never create 6 in the tube.
                const float Remaining = ReloadElapsed-ReloadDuration;
                SyncAmmoHUD(); PersistWorldChange(); StartCharge(*this,true);
                if (Remaining > 0) UpdateReload(Remaining);
                return;
            }
            if (G->Rounds >= TubeCapacity || AmmoCount(LWItems::Def(G->Definition).AmmoType) <= 0) { Complete(); return; }
            ReloadDuration += .62f;
        }
        ReloadTimer = FMath::Max(0.f,ReloadDuration-ReloadElapsed);
    }
    else if ((Weapon==8 || Weapon==11))
    {
        if(ReloadPhase==0&&ReloadElapsed>=.65f){EjectSpent(*ActiveGun());Sound(*this,TEXT("DoubleBarrelEject"));ReloadPhase=1;Changed=true;}
        while(ReloadPhase>=1&&ReloadPhase<=S.RevolverInsertions&&ReloadElapsed>=.85f+.55f*ReloadPhase){if(InsertLooseRound(Inventory,ActiveWeaponId)){Sound(*this,TEXT("DoubleBarrelInsert"));Changed=true;}ReloadPhase++;}
        if(ReloadElapsed>=ReloadDuration){Sound(*this,TEXT("DoubleBarrelClose"));Complete();return;}
    }
    else if (Weapon == 3)
    {
        if (ReloadPhase == 0 && ReloadElapsed >= .40f) ReloadPhase = 1;
        if (ReloadPhase == 1 && ReloadElapsed >= .75f)
        {
            EjectSpent(*ActiveGun()); // Live rounds survive both the ejector stroke and interruption.
            Sound(*this,TEXT("RevolverEject"));
            ReloadPhase = 2; Changed = true;
        }
        while (ReloadPhase >= 2 && ReloadPhase-2 < S.RevolverInsertions &&
            ReloadElapsed >= .75f+.40f*(ReloadPhase-1))
        {
            if (!InsertLooseRound(Inventory,ActiveWeaponId))
            {
                S.RevolverInsertions = ReloadPhase-2;
                ReloadDuration = FMath::Min(ReloadDuration,ReloadElapsed+.40f);
                break;
            }
            Sound(*this,TEXT("RevolverInsert"),.55f,1.f+.02f*(ReloadPhase-2));
            ++ReloadPhase; Changed = true;
        }
        if (ReloadElapsed >= ReloadDuration)
        {
            Sound(*this,TEXT("RevolverClose")); Complete(); return;
        }
    }
    else
    {
        const bool Belt = Weapon == 7;
        const float Out = Belt ? 1.05f : Weapon == 4 ? .40f : Weapon == 5 ? .28f : .40f;
        const float In = Belt ? 3.60f : Weapon == 4 ? 1.35f : Weapon == 5 ? .95f : 1.25f;
        if (ReloadPhase == 0 && ReloadElapsed >= Out) ReloadPhase = 1;
        if (Belt && ReloadPhase == 1 && ReloadElapsed >= 1.85f)
        { Sound(*this,TEXT("LMGBelt"),.7f,.8f); ReloadPhase = 2; }
        const int32 SeatPhase = Belt ? 2 : 1;
        if (ReloadPhase == SeatPhase && ReloadElapsed >= In)
        {
            if (!SwapMagazine(Inventory,ActiveWeaponId,ReloadMagazineId))
            { Notify(TEXT("RELOAD INTERRUPTED // MAGAZINE MOVED OR NO SPACE")); CancelReload(); return; }
            Sound(*this,Belt ? TEXT("LMGBelt") : MagInSound(Weapon),.75f);
            ++ReloadPhase; Changed = true;
        }
        if (Belt && ReloadPhase == 3 && ReloadElapsed >= 4.55f)
        { Sound(*this,TEXT("LMGClose"),.85f); ReloadPhase = 4; }
        const int32 ChargePhase = Belt ? 4 : 2;
        const float ChargeStart = Belt ? 5.10f : In+.22f;
        if (ReloadPhase == ChargePhase && ReloadElapsed >= ChargeStart)
        {
            if (ActiveGun()->Chamber != 1) Sound(*this,ChargeSound(Weapon),.65f,Belt ? .72f : 1.f);
            ++ReloadPhase;
        }
        if (ReloadElapsed >= ReloadDuration)
        {
            // The magazine must actually have been seated before a bolt can feed from it.
            if (ReloadPhase >= SeatPhase+1) ChamberRound();
            Complete(); return;
        }
    }
    if (Changed) { SyncAmmoHUD(); PersistWorldChange(); }
}

void ALWCharacter::ConfigureWeaponParts()
{
    using namespace LWWeapons;
    if (!World || !WeaponMesh || !WeaponRoot) return;
    const auto* G = Gun(*this);
    if (!G)
    {
        ConfigureCustom39();ConfigureAttachments();
        State(*this).VisualGun.Invalidate();
        WeaponRoot->SetVisibility(false,true);
        if (Muzzle) Muzzle->SetIntensity(0);
        return;
    }
    const auto& D = LWItems::Def(G->Definition);
    UStaticMesh* Base = D.Mesh.IsNull() ? nullptr : D.Mesh.LoadSynchronous();
    if (!Base) Base = World->Mesh(Weapon==4?TEXT("SniperBareV14"):Specs[Weapon].Mesh);
    WeaponMesh->SetStaticMesh(Base);WeaponMesh->SetLightingChannels(false,true,false);
    WeaponMesh->SetRelativeTransform(D.MeshTransform);
    if (Weapon < 2) WeaponMesh->SetRelativeRotation(FRotator(45,-12,0));
    auto Setup = [&](UStaticMeshComponent* Part, const TCHAR* Name, FVector Position, FRotator Rotation = FRotator::ZeroRotator)
    {
        if (!Part) return;
        Part->AttachToComponent(WeaponMesh,FAttachmentTransformRules::KeepRelativeTransform);
        Part->SetStaticMesh(Name ? World->Mesh(Name) : nullptr);
        Part->SetRelativeLocationAndRotation(Position,Rotation);
        Part->SetRelativeScale3D(FVector::OneVector);
        Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Part->SetCastShadow(false);Part->SetLightingChannels(false,true,false);
        Part->SetVisibility(false);
    };
    Setup(MovingPart,Weapon==16?TEXT("TenBarrels50"):Weapon==17?TEXT("GiantMag50"):Weapon==18?TEXT("AKMag50"):Weapon==19?nullptr:Weapon==20?TEXT("BudgetBarrel50"):(Weapon==8 || Weapon==11) ? (Weapon==11?TEXT("SawedOffBarrels24"):TEXT("DoubleBarrelBarrelsV18")) : Weapon == 3 ? TEXT("RevolverCylinder") : Weapon == 4 ? TEXT("SniperBolt") :
        Weapon == 5 ? TEXT("SMGMag") : Weapon == 6 ? TEXT("RifleMag") : Weapon == 7 ? TEXT("LMGCover") : Weapon>=9 ? (Weapon==10?TEXT("MinigunBarrels24"):Weapon==12?TEXT("DeagleMag24"):Weapon==13?TEXT("RifleMag"):Weapon==14?TEXT("TaserCartridge24"):Weapon==15?TEXT("FuelTank24"):TEXT("Missile24")) : nullptr,MovingRest(Weapon));
    Setup(SecondPart,Weapon==10?TEXT("MinigunBox24"):Weapon==12?TEXT("DeagleSlide24"):Weapon == 4 ? TEXT("SniperMag") : Weapon == 7 ? TEXT("LMGBelt") : nullptr,FeedRest(Weapon));
    Setup(Arms,TEXT("RightHand35"),Weapon < 2 ? FVector(16,10,-5) : RightRest(Weapon),
        Weapon < 2 ? FRotator::ZeroRotator : FRotator(48,-4,-12));
    Setup(LoadingHand,Weapon==19?TEXT("FingerGuns50"):TEXT("LeftHand35"),Weapon < 2 ? FVector(16,-10,-5) : LeftRest(Weapon),
        Weapon < 2 ? FRotator::ZeroRotator : SupportRotation());
    for (auto* HandMesh : {Arms.Get(),LoadingHand.Get()}) if (HandMesh)
    {
        HandMesh->SetRelativeScale3D(Weapon < 2 ? FVector(.65f,.85f,.85f) : FVector(.82f));
        if (Weapon < 2) HandMesh->AttachToComponent(WeaponRoot,FAttachmentTransformRules::KeepRelativeTransform);
    }
    if(Weapon==19)LoadingHand->SetRelativeScale3D(FVector(1,-1,1));
    auto& S = State(*this);
    if (Weapon == 7 && !S.FeedBox.IsValid())
    {
        auto* Box = NewObject<UStaticMeshComponent>(this,TEXT("LWWeaponFeedBox"));
        AddInstanceComponent(Box);
        Box->SetupAttachment(WeaponMesh);
        Box->RegisterComponent();
        S.FeedBox = Box;
    }
    if (S.FeedBox.IsValid()) Setup(S.FeedBox.Get(),Weapon == 7 ? TEXT("AmmoBox") : nullptr,FVector(6,-14,-22));
    if (Muzzle)
    {
        Muzzle->AttachToComponent(WeaponMesh,FAttachmentTransformRules::KeepRelativeTransform);
        Muzzle->SetRelativeLocation(Specs[Weapon].Muzzle);
        Muzzle->SetIntensity(0);
    }
    for(auto* Part:{WeaponMesh.Get(),MovingPart.Get(),SecondPart.Get()})if(Part){Part->EmptyOverrideMaterials();if(G->Camo62==0&&!LWMods::SkinName(G).IsEmpty()){
        auto* Finish=World->Material(FName(*FString::Printf(TEXT("WeaponSkin24_%d_%02d"),LWMods::Tier(G),G->WeaponSkin)));
        if(Finish)for(int I=0;I<Part->GetNumMaterials();I++){FString Name=Part->GetMaterial(I)?Part->GetMaterial(I)->GetName():FString();if(!Name.Contains(TEXT("Rubber"))&&!Name.Contains(TEXT("Glass"))&&!Name.Contains(TEXT("Glow")))Part->SetMaterial(I,Finish);}
    }}
    ConfigureCustom39();ConfigureAttachments();ApplyIdentity();
    S.VisualGun = ActiveWeaponId;
    // Visibility is evaluated from inventory state, not assumed from a non-null mesh.
    UpdateWeapon(0);
}

void ALWCharacter::UpdateWeapon(float Dt)
{
    using namespace LWWeapons;
    if (!FMath::IsFinite(Dt) || Dt < 0 || !World || !WeaponRoot) return;
    auto& S = State(*this);
    const auto* G = Gun(*this);
    ON_SCOPE_EXIT { UpdateCustom39(); if(Gun(*this)&&!Gun(*this)->Parts39.IsEmpty()&&S.FeedBox.IsValid())S.FeedBox->SetVisibility(false); };
    if(Traversing54){WeaponRoot->SetVisibility(false,true);WeaponRoot->SetVisibility(true,false);Arms->SetVisibility(true);LoadingHand->SetVisibility(true);float Reach=FMath::Sin(FMath::Clamp(TraversalTime54/TraversalDuration54,0.f,1.f)*PI);WeaponRoot->SetRelativeLocation(FVector(22,21,-22));WeaponRoot->SetRelativeRotation(FRotator(-Reach*18,0,0));LoadingHand->SetRelativeLocation(FVector(25+Reach*15,-25,Reach*14));LoadingHand->SetRelativeRotation(FRotator(0,0,80));return;}
    const bool Show = G && CanAct();
    // SetMenuInput propagates visibility to all children; explicitly restore each optional part.
    WeaponRoot->SetVisibility(Show,false);
    if (!Show)
    {
        bTrigger = false; S.FlashUntil = 0;S.Kick54=FVector2D::ZeroVector;
        if (bReloading) CancelReload();
    }
    if (Muzzle)
    {
        const bool Flash = Show && !InShelter(*this) && GetWorld() && GetWorld()->GetRealTimeSeconds() < S.FlashUntil;
        Muzzle->SetIntensity(Flash ? 40000.f : 0.f);
        Muzzle->SetVisibility(Flash);
    }
    const bool ScopeView=bAim&&(LWMods::Scope(G));
    auto Visibility = [Show,ScopeView](UStaticMeshComponent* PartMesh, bool Enabled)
    { if (PartMesh) PartMesh->SetVisibility(Show && !ScopeView && Enabled && PartMesh->GetStaticMesh() != nullptr,false); };
    for(auto& A:AttachmentParts)Visibility(A,G!=nullptr);
    Visibility(WeaponMesh,G != nullptr);
    Visibility(Arms,G != nullptr);
    Visibility(LoadingHand,G != nullptr);
    if (!G)
    {
        Visibility(MovingPart,false); Visibility(SecondPart,false); Visibility(S.FeedBox.Get(),false);
        S.CycleGun.Invalidate(); TickUnarmed(Dt);
        return;
    }
    if(CanAct()){const FVector2D Kick=S.Kick54*(1-FMath::Exp(-35.f*Dt));S.Kick54-=Kick;AddControllerPitchInput(Kick.X);AddControllerYawInput(Kick.Y);}else S.Kick54=FVector2D::ZeroVector;
    const float PreviousAttack = AttackTimer;
    AttackTimer = FMath::Max(0.f,AttackTimer-Dt);
    // A interrupted/manual-action weapon keeps a spent chamber until it is actually cycled.
    if (S.CycleGun.IsValid())
    {
        if (S.CycleGun != ActiveWeaponId || (Weapon != 2 && Weapon != 4)) S.CycleGun.Invalidate();
        else
        {
            S.CycleElapsed += Dt;
            if (S.CycleStage == 0 && S.CycleElapsed >= (Weapon == 2 ? .37f : .18f))
            { Sound(*this,ChargeSound(Weapon),.55f); S.CycleStage = 1; }
            if (S.CycleStage == 1 && S.CycleElapsed >= .46f)
            {
                if (ActiveGun()->Chamber == 2) ActiveGun()->Chamber = 0;
                S.CycleStage = 2; SyncAmmoHUD();
            }
            if (S.CycleStage == 2 && S.CycleElapsed >= .78f) { ChamberRound(); S.CycleStage = 3; }
            if (S.CycleElapsed >= .9f) S.CycleGun.Invalidate();
        }
    }
    UpdateReload(Dt);
    if(S.Buffered54>0){S.Buffered54=FMath::Max(0.f,S.Buffered54-Dt);if(AttackTimer<=0&&bTrigger&&CanAct()&&!bReloading&&!Automatic(*this)){S.Buffered54=0;bTrigger=false;Attack();}}
    // A bounded catch-up keeps held automatic fire close to its intended rate on slow frames.
    float Overdue = FMath::Max(0.f,Dt-PreviousAttack);
    for (int32 Shots = 0; Shots < 4 && bTrigger && Automatic(*this) && CanAct() && !bReloading && AttackTimer <= 0; ++Shots)
    {
        Attack();
        if (!bTrigger || AttackTimer <= 0) break;
        const float Carry = FMath::Min(Overdue,AttackTimer);
        AttackTimer -= Carry; Overdue -= Carry;
    }
    G = Gun(*this); // Reload insertion replaces the array; never retain a pointer across it.
    if (!G) return;
    S.Spin=FMath::FInterpConstantTo(S.Spin,Weapon==10&&bTrigger&&!bReloading&&!S.Overheated?1.f:0.f,Dt,1.5f);
    S.Heat=FMath::Max(0.f,S.Heat-Dt*(bTrigger&&!S.Overheated?.02f:.24f));if(S.Overheated&&S.Heat<.25f)S.Overheated=false;
    S.BarrelAngle=FMath::Fmod(S.BarrelAngle+Dt*S.Spin*1800,360.f);
    BudgetBrace50=FMath::FInterpConstantTo(BudgetBrace50,Weapon==20&&bTrigger&&!bReloading&&Show?1.f:0.f,Dt,4.f);
    const bool HasMag = Magazine(Inventory,*G) != nullptr;
    bool VisibleFeed = HasMag;
    const float T = ReloadElapsed;
    if (bReloading && !S.bChargeOnly)
    {
        if (Weapon == 7) VisibleFeed = T < 1.85f ? HasMag : T >= 2.95f && (ReloadMagazineId.IsValid() || HasMag);
        else if (Weapon >= 4)
        {
            const float Hide = Weapon == 4 ? .68f : Weapon == 5 ? .43f : .62f;
            const float Return = Weapon == 4 ? .95f : Weapon == 5 ? .66f : .86f;
            VisibleFeed = T < Hide ? HasMag : T >= Return && (ReloadMagazineId.IsValid() || HasMag);
        }
    }
    Visibility(MovingPart,Weapon==16||Weapon==20||(Direct(Weapon)&&(G->Rounds>0||bReloading)) || Weapon==10 || (Weapon>=9 && !Direct(Weapon) && Weapon!=11 && VisibleFeed) || (Weapon==8 || Weapon==11) || Weapon == 3 || Weapon == 4 || Weapon == 7 || ((Weapon == 5 || Weapon == 6) && VisibleFeed));
    Visibility(SecondPart,Weapon==12 || (Weapon == 4 || Weapon == 7 || Weapon==10) && VisibleFeed);
    Visibility(S.FeedBox.Get(),Weapon == 7 && VisibleFeed);
    if (!Show) return;

    const float Time = GetWorld()->GetTimeSeconds();
    const bool Moving = GetVelocity().Size2D() > 30;
    const bool Running = bSprint && Moving && Stamina > 1 && !bIsCrouched && !bAim;
    const float Speed = Moving ? (Running ? 12.f : 8.f) : 1.4f;
    FVector Rest = ViewRest(Weapon,bAim && Weapon >= 2);
    Rest.Z += FMath::Sin(Time*Speed)*(Moving ? 1.1f : .16f);
    FRotator Rotation(FMath::Clamp(-LookY*.16f,-3.f,3.f),FMath::Clamp(-LookX*.2f,-4.f,4.f),Moving ? FMath::Sin(Time*Speed*.5f)*.8f : 0);
    const float Progress = AttackDuration > 0 ? FMath::Clamp(1-AttackTimer/AttackDuration,0.f,1.f) : 1;
    if (AttackTimer > 0 && (Weapon<2||!LWMods::Effect(G,TEXT("Recoil Free"))))
    {
        const float Swing = FMath::Sin(Progress*PI);
        if (Weapon < 2) { Rotation.Yaw -= Swing*75; Rotation.Pitch -= Swing*60; Rest.X += Swing*17; }
        else if (Weapon == 2) { Rest.X -= Swing*9; Rotation.Pitch += Swing*10; }
        else
        {
            const float Kick = Pulse(0,.12f,1,Progress)*(LWMods::Effect(G,TEXT("Recoil Free"))?0.f:1.f);
            Rest.X -= Kick*(Weapon == 4 ? 9.f : Weapon == 3 ? 5.5f : Weapon == 7 ? 3.2f : 2.3f);
            Rotation.Pitch += Kick*(Weapon == 3 ? 16.f : Weapon == 4 ? 8.f : 3.5f);
            Rotation.Roll += Kick*(Weapon == 3 ? -5.f : 1.2f);
        }
    }
    FVector Part = MovingRest(Weapon), Feed = FeedRest(Weapon), Hand = LeftRest(Weapon), Box(6,-14,-22);
    FQuat PartRotation = FQuat::Identity, FeedRotation = FQuat::Identity;
    FRotator HandRotation = SupportRotation();
    float Mechanism = S.CycleGun.IsValid() ? S.CycleElapsed : -1;
    if (bReloading && S.bChargeOnly) Mechanism = T;
    const float Presentation = bReloading ? Smooth(0,.18f,T) :
        Mechanism >= 0 ? Smooth(.08f,.24f,Mechanism)*(1-Smooth(.78f,.9f,Mechanism)) : 0;
    if (Presentation > 0 && Weapon >= 2)
    {
        auto Pose = PresentationPose(Weapon);
        // A bolt/pump can cycle while aiming, and FOV eases out after Reload().
        // Compensate distance during that zoom so the working hand stays in frame.
        const float Fov = Camera ? FMath::Clamp(Camera->FieldOfView,24.f,100.f) : 86.f;
        Pose.Location.X *= FMath::Max(1.f,FMath::Tan(FMath::DegreesToRadians(43.f))/FMath::Tan(FMath::DegreesToRadians(Fov*.5f)));
        Rest = FMath::Lerp(Rest,Pose.Location,Presentation);
        Rotation = FMath::Lerp(Rotation,Pose.Rotation,Presentation);
    }
    if(Weapon==10){PartRotation=Axis(FVector::XAxisVector,S.BarrelAngle);}
    if (Weapon == 3) PartRotation = Axis(FVector::XAxisVector,60.f*G->CylinderIndex);
    if (bReloading && !S.bChargeOnly)
    {
        if (Weapon == 2)
        {
            const float Begin = ReloadPhase == 0 ? 0.f : .6f+(ReloadPhase-1)*.62f;
            const float P = Smooth(Begin,ReloadDuration,T);
            const FVector Loading = FMath::Lerp(FVector(10,-6,-16),FVector(15,-1,-7),Pulse(0,.78f,1,P));
            Hand = FMath::Lerp(LeftRest(2),Loading,Presentation);
            HandRotation = FMath::Lerp(SupportRotation(),FRotator(28,-15,80),Presentation);
        }
        else if ((Weapon==8 || Weapon==11))
        {
            const float Open=Smooth(0,.5f,T)*(1-Smooth(ReloadDuration-.4f,ReloadDuration,T));
            PartRotation=Axis(FVector::YAxisVector,48*Open);
            Hand=FMath::Lerp(LeftRest(8),Part+FVector(-5,-6,8),Open);Hand.Z-=FMath::Sin(FMath::Clamp((T-.85f)/.55f,0.f,2.f)*PI)*12;HandRotation=FRotator(20,-15,70);
        }
        else if (Weapon == 3)
        {
            const float Open = Smooth(0,.40f,T)*(1-Smooth(ReloadDuration-.40f,ReloadDuration,T));
            const FVector Crane(8,-3.6,-.5);
            const FQuat Swing = Axis(FVector::XAxisVector,95*Open);
            Part = Crane+Swing.RotateVector(MovingRest(3)-Crane);
            PartRotation = Swing*Axis(FVector::XAxisVector,60.f*G->CylinderIndex);
            const float Eject = Pulse(.40f,.57f,.75f,T);
            Part.X -= Eject*.8f;
            const float Insert = T >= .75f && T < ReloadDuration-.40f ? FMath::Sin(FMath::Frac((T-.75f)/.40f)*PI) : 0;
            Hand = FMath::Lerp(LeftRest(3),Part+FVector(-7,-4,-5),Open);
            Hand.X += Eject*4+Insert*4; Hand.Z -= Insert*1.5f;
            HandRotation = FMath::Lerp(SupportRotation(),FRotator(20,-18,75),Open);
        }
        else if (Weapon == 7)
        {
            const float Open = Smooth(0,1.05f,T)*(1-Smooth(3.9f,4.55f,T));
            PartRotation = Axis(FVector::YAxisVector,75*Open);
            const float Out = Smooth(1.05f,1.85f,T)*(1-Smooth(2.95f,3.60f,T));
            Feed += FVector(0,-8*Out,4*Out);
            FeedRotation = Axis(FVector::XAxisVector,20*Out);
            Box += FVector(4,-9*Out,2*Out);
            const FVector LidGrip = Part+PartRotation.RotateVector(FVector(-23,-2,0))+FVector(-5,-3,-4);
            const FVector BoxGrip = Box+FVector(-3,-2,8);
            Hand = T < 1.05f ? FMath::Lerp(LeftRest(7),LidGrip,Smooth(0,.25f,T)) :
                T < 3.6f ? FMath::Lerp(LidGrip,BoxGrip,Smooth(1.05f,1.40f,T)) :
                T < 4.55f ? FMath::Lerp(BoxGrip,LidGrip,Smooth(3.6f,3.9f,T)) :
                FMath::Lerp(LidGrip,LeftRest(7),Smooth(4.55f,5.1f,T));
            HandRotation = FMath::Lerp(SupportRotation(),FRotator(22,-10,75),Presentation);
            if (G->Chamber != 1 && T >= 5.10f) Mechanism = (T-5.10f)/.90f*.9f;
        }
        else
        {
            const float In = Weapon == 4 ? 1.35f : Weapon == 5 ? .95f : 1.25f;
            const float OutEnd = Weapon == 4 ? .68f : Weapon == 5 ? .43f : .62f;
            const float Return = Weapon == 4 ? .95f : Weapon == 5 ? .66f : .86f;
            const float Withdraw = T < Return ? Smooth(.05f,OutEnd,T) : 1-Smooth(Return,In,T);
            FVector Offset = FVector(Weapon == 6 ? 3.5f : 0,-1,-(Weapon == 4 ? 11 : 15))*Withdraw;
            const FQuat Tilt = Axis(FVector::YAxisVector,(Weapon == 6 ? -18 : 10)*Withdraw);
            if (Weapon == 4 || Weapon==10) { Feed += Offset; FeedRotation = Tilt; }
            else { Part += Offset; PartRotation = Tilt; }
            const float Grip = Smooth(0,.20f,T)*(1-Smooth(In,In+.22f,T));
            Hand = FMath::Lerp(LeftRest(Weapon),((Weapon == 4||Weapon==10) ? Feed : Part)+FVector(-5,-2,-8),Grip);
            HandRotation = FMath::Lerp(SupportRotation(),FRotator(38,-8,75),Grip);
            if (G->Chamber != 1 && T >= In+.22f && ReloadDuration > In+.6f)
                Mechanism = .9f*Smooth(In+.22f,ReloadDuration,T);
        }
    }
    if(bReloading&&!S.bChargeOnly&&(Weapon==9||Weapon==14||Weapon==15)){
        float Pull=T<.86f?Smooth(.05f,.62f,T):1-Smooth(.86f,1.25f,T);
        Part=MovingRest(Weapon)+(Weapon==9?FVector(-55,0,0):Weapon==14?FVector(16,0,0):FVector(0,-18,-8))*Pull;
        Hand=Part+FVector(-4,-3,-5);PartRotation=Axis(FVector::YAxisVector,Weapon==15?Pull*20:0);
    }
    if(Weapon==12){const float Slide=G->Chamber==0?1.f:AttackTimer>0?Pulse(0,.15f,.55f,Progress):0.f;Feed.X-=Slide*3.2f;}
    if(Weapon==10){Part=FVector(20,0,0);PartRotation=Axis(FVector::XAxisVector,S.BarrelAngle);}
    if (Mechanism >= 0)
    {
        const float Pull = Pulse(.14f,.44f,.78f,Mechanism);
        if (Weapon == 4)
        {
            Part.X -= Pull*9;
            PartRotation = Axis(FVector::XAxisVector,60*Smooth(0,.18f,Mechanism)*(1-Smooth(.78f,.9f,Mechanism)));
            const float Grip = Smooth(.02f,.18f,Mechanism)*(1-Smooth(.78f,.9f,Mechanism));
            const FVector BoltGrip = Part+PartRotation.RotateVector(FVector(-7,8,-3))+FVector(-5,0,-4);
            Hand = FMath::Lerp(Hand,BoltGrip,Grip);
            HandRotation = FMath::Lerp(HandRotation,FRotator(25,-15,-55),Grip);
        }
        else if (Weapon == 2)
        {
            // The v1 Shotgun contains a baked-in fore-end, with no separate pump FBX.
            // Hand travel and receiver counter-motion show the rack without duplicating that geometry.
            Hand.X -= Pull*9;
            Rest.X += Pull*1.5f; Rotation.Roll += Pull*2;
        }
        else
        {
            const float Grip = Smooth(.02f,.18f,Mechanism)*(1-Smooth(.78f,.9f,Mechanism));
            const FVector ChargeGrip(Weapon == 7 ? 4-12*Pull : -10-7*Pull,Weapon == 7 ? 8 : -7,3);
            Hand = FMath::Lerp(Hand,ChargeGrip,Grip);
            HandRotation = FMath::Lerp(HandRotation,FRotator(25,-15,Weapon == 7 ? -65 : 70),Grip);
            Rest.X += Pull; Rotation.Pitch -= Pull*2;
        }
    }
    if(GunBashTimer>0){const float Bash=FMath::Sin(GunBashTimer/.55f*PI);Rest.X+=Bash*14;Rotation.Yaw+=Bash*28;Rotation.Roll-=Bash*18;}
    if (Weapon == 7 && !bReloading && HasMag)
    {
        Feed.Y += (1-Progress)*1.35f;
        Feed.Z += FMath::Sin(Progress*PI*2)*.35f*(AttackTimer > 0 ? 1 : 0);
    }
    WeaponMesh->SetRelativeTransform(LWItems::Def(G->Definition).MeshTransform);
    if(Weapon==16&&bReloading){PartRotation=Axis(FVector(0,1,0),50*FMath::Sin(PI*T/ReloadDuration));Hand=Part+FVector(5,-12,-6);}
    if(Weapon==19){Arms->SetVisibility(false);Hand=FVector(0,-22,0);HandRotation=FRotator::ZeroRotator;if(bReloading){Rotation.Roll+=360*Smooth(.1f,1.4f,T);Rest.Z-=FMath::Sin(T/ReloadDuration*PI)*10;Rotation.Yaw+=FMath::Sin(T*13)*12;}}
    if(Weapon==18&&bReloading){
        // The magazine stays in the support hand while the receiver is tossed and replaced.
        const float Toss=Smooth(.35f,.95f,T),Return=Smooth(1.25f,2.4f,T);
        FVector Offset=T<1.25f?FVector(85,65,45)*Toss:FVector(-65,35,-45)*(1-Return);
        WeaponMesh->AddLocalOffset(Offset);WeaponMesh->AddLocalRotation(FRotator(T<1.25f?Toss*110:(1-Return)*-65,T<1.25f?Toss*100:0,0));
        WeaponMesh->SetVisibility(Show&&(T<.95f||T>1.25f));
        MovingPart->SetVisibility(Show&&HasMag);MovingPart->SetWorldTransform(FTransform(WeaponRoot->GetComponentQuat(),WeaponRoot->GetComponentTransform().TransformPosition(FVector(10,0,-5))));
        LoadingHand->SetWorldLocation(WeaponRoot->GetComponentTransform().TransformPosition(FVector(10,-5,-18)));
    }
    if(Weapon==20){Part=MovingRest(20);const float Loose=1-BudgetBrace50;PartRotation=FRotator(-Loose*(35+(Moving?FMath::Sin(Time*7)*18:3)),Loose*FMath::Sin(Time*5)*(Moving?22:5),0).Quaternion();Hand=FMath::Lerp(LeftRest(20),FVector(42,-3,0),BudgetBrace50);}
    if (MovingPart&&!(Weapon==18&&bReloading)) MovingPart->SetRelativeLocationAndRotation(Part,PartRotation);
    if (SecondPart) SecondPart->SetRelativeLocationAndRotation(Feed,FeedRotation);
    if (LoadingHand&&!(Weapon==18&&bReloading)) LoadingHand->SetRelativeLocationAndRotation(Weapon < 2 ? FVector(16,-10,-5) : Hand,Weapon < 2 ? FRotator::ZeroRotator : HandRotation);
    if (S.FeedBox.IsValid()) S.FeedBox->SetRelativeLocation(Box);
    if (bAim && LWMods::OpenSight(G) && !bReloading && Presentation <= 0 && GunBashTimer <= 0)
    {
        // Put the actual aperture on the camera/fire ray, including mesh catalog transforms.
        LWMods::AlignOpenSight(G,Weapon,WeaponMesh->GetRelativeTransform(),Rest,Rotation);
    }
    S.ObstructionClock54-=Dt;if(S.ObstructionClock54<=0){S.ObstructionClock54=.08f;FHitResult H;FCollisionQueryParams Q(SCENE_QUERY_STAT(LWWeaponNearWall54),false,this);const FVector Eye=Camera->GetComponentLocation();bool Block=GetWorld()->SweepSingleByChannel(H,Eye,Eye+Camera->GetForwardVector()*85,FQuat::Identity,ECC_Visibility,FCollisionShape::MakeSphere(5),Q);S.Obstruction54=Block&&!Cast<ALWZombie>(H.GetActor())?1.f-H.Time:0.f;}
    if(!bReloading){Rest.X-=S.Obstruction54*10;Rotation.Pitch-=S.Obstruction54*28;}
    WeaponMotion54(Rest,Rotation,Dt);
    WeaponRoot->SetRelativeLocation(FMath::VInterpTo(WeaponRoot->GetRelativeLocation(),Rest,Dt,18));
    WeaponRoot->SetRelativeRotation(FMath::RInterpTo(WeaponRoot->GetRelativeRotation(),Rotation,Dt,18));
}

#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "UObject/StrongObjectPtr.h"

namespace LWWeaponTests
{
    struct FCatalog
    {
        TStrongObjectPtr<ULWItemCatalog> Catalog;
        FCatalog() : Catalog(NewObject<ULWItemCatalog>()) { LWItems::SetCatalog(Catalog.Get()); }
        ~FCatalog() { LWItems::SetCatalog(nullptr); }
    };
    FLWItemInstance EquippedWeapon(FName Definition)
    {
        auto G = LWItems::Make(Definition);
        G.Slot = LWItems::Def(Definition).EquipSlots[0];
        return G;
    }
    int32 LiveRounds(const TArray<FLWItemInstance>& Items)
    {
        int32 Count = 0;
        for (const auto& I : Items)
        {
            if (LWItems::Def(I.Definition).Category == TEXT("Ammo")) Count += I.Count;
            Count += I.Rounds+(I.Chamber == 1 ? 1 : 0);
            for (int32 Round : I.Cylinder) if (Round == 1) ++Count;
        }
        return Count;
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWWeaponMagazineSwapTest,"LethalWorld.Weapons.FullGridMagazineSwap",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWWeaponMagazineSwapTest::RunTest(const FString& Parameters)
{
    using namespace LWWeapons;
    LWWeaponTests::FCatalog Scope;
    auto G = LWWeaponTests::EquippedWeapon(TEXT("rifle")); G.Chamber = 1;
    auto Old = LWItems::Make(TEXT("mag_rifle30")); Old.Slot = LoadedSlot; Old.Rounds = 7;
    auto Spare = LWItems::Make(TEXT("mag_rifle30")); Spare.X = 3; Spare.Y = 4; Spare.bRotated = true; Spare.Rounds = 30;
    G.LoadedMagazine = Old.Id;
    TArray<FLWItemInstance> Items = {G,Old,Spare};
    for (int32 Y = 0; Y < 10; ++Y) for (int32 X = 0; X < 12; ++X)
    {
        auto Filler = LWItems::Make(TEXT("ammo_9mm"));
        if (LWItems::Fits(Items,Filler,X,Y,false,12,10)) { Filler.X = X; Filler.Y = Y; Items.Add(Filler); }
    }
    const int32 Before = LWWeaponTests::LiveRounds(Items), ItemCount = Items.Num();
    TestTrue(TEXT("Full-grid swap succeeds by using the spare footprint"),SwapMagazine(Items,G.Id,Spare.Id));
    TestEqual(TEXT("Same number of item identities"),Items.Num(),ItemCount);
    TestEqual(TEXT("All live ammunition conserved"),LWWeaponTests::LiveRounds(Items),Before);
    TestTrue(TEXT("New link is the individual spare"),Find(Items,G.Id)->LoadedMagazine == Spare.Id);
    TestEqual(TEXT("Tactical chamber retained"),Find(Items,G.Id)->Chamber,1);
    TestEqual(TEXT("Removed rounds retained"),Find(Items,Old.Id)->Rounds,7);
    TestEqual(TEXT("Removed mag uses spare X"),Find(Items,Old.Id)->X,3);
    TestEqual(TEXT("Removed mag uses spare Y"),Find(Items,Old.Id)->Y,4);
    TestTrue(TEXT("Removed mag retains swapped rotation"),Find(Items,Old.Id)->bRotated);
    TestFalse(TEXT("A loaded chamber is not fed again"),FeedChamber(Items,G.Id));
    TestEqual(TEXT("Magazine rounds are not duplicated into the gun"),Find(Items,G.Id)->Rounds,0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWWeaponMagazineFailureTest,"LethalWorld.Weapons.MagazineIdentityAndFailure",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWWeaponMagazineFailureTest::RunTest(const FString& Parameters)
{
    using namespace LWWeapons;
    LWWeaponTests::FCatalog Scope;
    auto G = LWWeaponTests::EquippedWeapon(TEXT("smg"));
    auto Small = LWItems::Make(TEXT("mag_smg30")); Small.X=0; Small.Y=0; Small.Rounds=9;
    auto Full = LWItems::Make(TEXT("mag_smg30")); Full.X=2; Full.Y=0; Full.Rounds=30;
    auto Wrong = LWItems::Make(TEXT("mag_rifle30")); Wrong.X=4; Wrong.Y=0; Wrong.Rounds=30;
    TArray<FLWItemInstance> Items={G,Small,Full,Wrong};
    TestTrue(TEXT("Select highest compatible spare"),BestSpare(Items,G)==Full.Id);
    TestFalse(TEXT("Wrong magazine fails without changing ownership"),SwapMagazine(Items,G.Id,Wrong.Id));
    TestFalse(TEXT("Failure leaves gun unloaded"),Find(Items,G.Id)->LoadedMagazine.IsValid());
    TestTrue(TEXT("Empty gun accepts a spare"),SwapMagazine(Items,G.Id,Full.Id));
    const int32 Before=LWWeaponTests::LiveRounds(Items);
    TestTrue(TEXT("Charging moves exactly one round"),FeedChamber(Items,G.Id));
    TestEqual(TEXT("Feed conserves live ammunition"),LWWeaponTests::LiveRounds(Items),Before);
    TestEqual(TEXT("Magazine is authoritative"),Find(Items,Full.Id)->Rounds,29);
    auto Other=LWWeaponTests::EquippedWeapon(TEXT("smg")); Other.Slot=TEXT("Secondary"); Other.LoadedMagazine=Full.Id; Items.Add(Other);
    TestNull(TEXT("A magazine shared by two guns is rejected"),Magazine(Items,*Find(Items,G.Id)));
    TestFalse(TEXT("Malformed link cannot be overwritten"),SwapMagazine(Items,G.Id,Small.Id));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWWeaponTubeTest,"LethalWorld.Weapons.ShotgunFivePlusOne",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWWeaponTubeTest::RunTest(const FString& Parameters)
{
    using namespace LWWeapons;
    LWWeaponTests::FCatalog Scope;
    auto G=LWWeaponTests::EquippedWeapon(TEXT("shotgun"));
    auto Ammo=LWItems::Make(TEXT("ammo_12g"),8); Ammo.X=0; Ammo.Y=0;
    TArray<FLWItemInstance> Items={G,Ammo};
    for(int32 I=0;I<5;++I) TestTrue(TEXT("One loose shell per insertion"),InsertLooseRound(Items,G.Id));
    TestFalse(TEXT("Sixth tube shell prohibited"),InsertLooseRound(Items,G.Id));
    TestTrue(TEXT("Pump feeds one shell"),FeedChamber(Items,G.Id));
    TestTrue(TEXT("Top off to five plus one"),InsertLooseRound(Items,G.Id));
    TestEqual(TEXT("Tube remains five"),Find(Items,G.Id)->Rounds,5);
    TestEqual(TEXT("Chamber is live"),Find(Items,G.Id)->Chamber,1);
    TestEqual(TEXT("Reload preserves all eight rounds"),LWWeaponTests::LiveRounds(Items),8);
    Find(Items,G.Id)->Chamber=2; // One shot.
    TestTrue(TEXT("Spent chamber can be cycled"),FeedChamber(Items,G.Id));
    TestEqual(TEXT("Only fired shell is lost"),LWWeaponTests::LiveRounds(Items),7);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWWeaponCylinderTest,"LethalWorld.Weapons.PersistentRevolverCylinder",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWWeaponCylinderTest::RunTest(const FString& Parameters)
{
    using namespace LWWeapons;
    LWWeaponTests::FCatalog Scope;
    auto G=LWWeaponTests::EquippedWeapon(TEXT("revolver")); G.Cylinder={1,2,0,1,2,0}; G.CylinderIndex=2;
    auto Ammo=LWItems::Make(TEXT("ammo_357"),4); Ammo.X=0; Ammo.Y=0;
    TArray<FLWItemInstance> Items={G,Ammo};
    TestFalse(TEXT("Empty next chamber does not search ahead for a live round"),FireCylinder(*Find(Items,G.Id)));
    TestEqual(TEXT("Dry fire advances exactly one chamber"),Find(Items,G.Id)->CylinderIndex,3);
    TestTrue(TEXT("Next live chamber fires"),FireCylinder(*Find(Items,G.Id)));
    TestEqual(TEXT("Fired chamber is spent"),Find(Items,G.Id)->Cylinder[3],2);
    const int32 Before=LWWeaponTests::LiveRounds(Items);
    EjectSpent(*Find(Items,G.Id));
    TestEqual(TEXT("Ejection retains the unspent round"),Find(Items,G.Id)->Cylinder[0],1);
    TestEqual(TEXT("Ejection does not return spent cases as ammo"),LWWeaponTests::LiveRounds(Items),Before);
    TestTrue(TEXT("Insert exactly one live round"),InsertLooseRound(Items,G.Id));
    // Cancellation closes the animation without modifying this already-committed inventory state.
    TestEqual(TEXT("Partially reloaded live round is retained"),Find(Items,G.Id)->Cylinder[1],1);
    TestEqual(TEXT("Cylinder index remains persistent"),Find(Items,G.Id)->CylinderIndex,4);
    TestEqual(TEXT("Single insertion conserves ammo"),LWWeaponTests::LiveRounds(Items),Before);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWWeaponOwnershipTest,"LethalWorld.Weapons.OwnershipAndLegacyTuning",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FLWWeaponOwnershipTest::RunTest(const FString& Parameters)
{
    using namespace LWWeapons;
    LWWeaponTests::FCatalog Scope;
    auto G=LWItems::Make(TEXT("rifle"));
    TestFalse(TEXT("Grid weapon cannot attack as equipped"),Equipped(G));
    G.Slot=LoadedSlot; TestFalse(TEXT("Loaded is not a weapon equipment slot"),Equipped(G));
    G.Slot=TEXT("Sidearm"); TestFalse(TEXT("Incompatible equipment slot rejected"),Equipped(G));
    G.Slot=TEXT("Primary"); TestTrue(TEXT("Owned compatible equipped weapon accepted"),Equipped(G));
    TestEqual(TEXT("Crowbar damage preserved"),Specs[0].Damage,36.f);
    TestEqual(TEXT("Crowbar interval preserved"),Specs[0].Interval,.55f);
    TestEqual(TEXT("Bat damage preserved"),Specs[1].Damage,63.f);
    TestEqual(TEXT("Bat interval preserved"),Specs[1].Interval,.85f);
    TestEqual(TEXT("Shotgun pellets preserved"),Specs[2].Pellets,10);
    TestEqual(TEXT("Shotgun pellet damage preserved"),Specs[2].Damage,16.f);
    TestEqual(TEXT("Shotgun hip spread preserved"),Specs[2].HipSpread,4.8f);
    TestEqual(TEXT("Shotgun aim spread preserved"),Specs[2].AimSpread,2.1f);
    return true;
}
#endif
