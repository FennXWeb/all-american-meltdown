#include "LWWeaponMods.h"
#include "LWHUD.h"
#include "LWCharacter.h"
#include "LWInventory.h"
#include "LWFuel.h"
#include "LWWorld.h"
#include "LWWorldObject.h"
#include "CoreGlobals.h"
#include "Engine/Canvas.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include <initializer_list>

// All interaction lives in InventoryScreen. Persistent state is declared by the owning HUD header.
namespace LWInventoryHUD
{
    const FLinearColor InvBone(.88f, .91f, .86f);
    const FLinearColor InvMuted(.56f, .64f, .65f);
    const FLinearColor InvAmber(.90f, .57f, .25f);
    const FLinearColor InvGreen(.43f, .72f, .38f);
    const FLinearColor InvRed(.86f, .28f, .18f);
    const FLinearColor InvPanel(.035f, .046f, .036f, .98f);
    const FLinearColor InvEdge(.23f, .28f, .21f);

    struct FArea
    {
        float X = 0, Y = 0, W = 0, H = 0;
        float Right() const { return X + W; }
        float Bottom() const { return Y + H; }
        bool Contains(FVector2D P) const { return P.X >= X && P.Y >= Y && P.X < Right() && P.Y < Bottom(); }
        bool Intersect(const FArea& Other, FArea& Out) const
        {
            Out.X = FMath::Max(X, Other.X); Out.Y = FMath::Max(Y, Other.Y);
            Out.W = FMath::Min(Right(), Other.Right()) - Out.X;
            Out.H = FMath::Min(Bottom(), Other.Bottom()) - Out.Y;
            return Out.W > 0 && Out.H > 0;
        }
    };

    struct FGrid
    {
        int32 PanelId = INDEX_NONE, Width = 12, Height = 10, FirstRow = 0, Rows = 10;
        float X = 328, Y = 144, Cell = 32;
        const TArray<FLWItemInstance>* Items = nullptr;
        FArea Bounds() const { return { X, Y, Width * Cell, Rows * Cell }; }
        FIntPoint CellAt(FVector2D Mouse) const
        {
            return { FMath::FloorToInt32((Mouse.X - X) / Cell), FirstRow + FMath::FloorToInt32((Mouse.Y - Y) / Cell) };
        }
    };

    struct FEquipmentBox { FName Slot; FArea Area; };

    struct FHit
    {
        int32 PanelId = INDEX_NONE;
        FGuid Id;
        FName Slot;
        const FGrid* Grid = nullptr;
        const FEquipmentBox* Equipment = nullptr;
    };

    const FLWItemInstance* Find(const TArray<FLWItemInstance>* Items, FGuid Id)
    {
        return Items && Id.IsValid() ? Items->FindByPredicate([Id](const FLWItemInstance& I) { return I.Id == Id; }) : nullptr;
    }

    int32 Price(const FLWItemInstance& Item)
    {
        return LWMods::Price(Item);
    }

    FString FitText(FString Value, float Width, float Size = .8f)
    {
        const int32 Limit = FMath::Max(0, FMath::FloorToInt(Width / (9.f * FMath::Max(Size, .8f))));
        if (Value.Len() > Limit) Value = Limit > 3 ? Value.Left(Limit - 3) + TEXT("...") : Value.Left(Limit);
        return Value;
    }

    FString Name(const FLWItemDefinition& D)
    {
        return (D.DisplayName.IsEmpty() ? D.Id.ToString() : D.DisplayName.ToString()).ToUpper();
    }

    FString ShortName(FName Id)
    {
        static const TMap<FName, FString> Names = {
            {TEXT("crowbar"),TEXT("BAR")}, {TEXT("bat"),TEXT("BAT")},
            {TEXT("ammo_12g"),TEXT("12G")}, {TEXT("ammo_357"),TEXT(".357")},
            {TEXT("ammo_762"),TEXT("7.62")}, {TEXT("ammo_762belt"),TEXT("7.62")},
            {TEXT("ammo_9mm"),TEXT("9MM")}, {TEXT("ammo_556"),TEXT("5.56")},
            {TEXT("ammo_50ae"),TEXT(".50 AE")}, {TEXT("ammo_rocket"),TEXT("MISSILE")},
            {TEXT("ammo_fuel"),TEXT("FUEL")},
            {TEXT("mag_sniper5"),TEXT("762")}, {TEXT("mag_smg30"),TEXT("9MM")},
            {TEXT("mag_rifle30"),TEXT("556")}, {TEXT("mag_lmg100"),TEXT("BELT BOX")},
            {TEXT("food"),TEXT("CAN")}, {TEXT("water"),TEXT("H2O")},
            {TEXT("medkit"),TEXT("MEDKIT")}, {TEXT("battery"),TEXT("BAT")}, {TEXT("scrap"),TEXT("SCR")}
        };
        if (const FString* Label = Names.Find(Id)) return *Label;
        return Name(LWItems::Def(Id));
    }

    FLinearColor Tint(const FLWItemDefinition& D)
    {
        if (D.Category == TEXT("Weapon")) return FLinearColor(.22f, .24f, .17f);
        if (D.Category == TEXT("Ammo") || D.Category == TEXT("Magazine")) return FLinearColor(.29f, .24f, .14f);
        if (D.Id == TEXT("medkit")) return FLinearColor(.29f, .15f, .13f);
        if (D.Id == TEXT("water")) return FLinearColor(.14f, .23f, .25f);
        if (D.Category == TEXT("Armor") || D.Category == TEXT("Container")) return FLinearColor(.20f, .25f, .19f);
        return FLinearColor(.22f, .25f, .20f);
    }

    FString RoundLabel(const FLWItemInstance& I, const TArray<FLWItemInstance>* Items)
    {
        const FLWItemDefinition& D = LWItems::Def(I.Definition);
        if (D.Category == TEXT("Magazine")) return FString::Printf(TEXT("%d/%d"), I.Rounds, D.Capacity);
        if (!I.Cylinder.IsEmpty())
        {
            int32 Live = 0; for (int32 State : I.Cylinder) Live += State == 1 ? 1 : 0;
            return FString::Printf(TEXT("%d/%d"), Live, I.Cylinder.Num());
        }
        if (D.Category == TEXT("Weapon") && !D.AmmoType.IsNone())
        {
            const FLWItemInstance* Mag = Find(Items, I.LoadedMagazine);
            if (!D.MagazineType.IsNone() && !Mag)
                return FString::Printf(TEXT("NO MAG +%d"), I.Chamber == 1 ? 1 : 0);
            if (D.WeaponIndex == 9 || D.WeaponIndex == 14)
                return FString::Printf(TEXT("%d/%d"), I.Rounds, D.WeaponIndex == 14 ? 10 : 1);
            return FString::Printf(TEXT("%d+%d"), Mag ? Mag->Rounds : I.Rounds, I.Chamber == 1 ? 1 : 0);
        }
        return I.Count > 1 ? FString::Printf(TEXT("x%d"), I.Count) : FString();
    }

    // Clip icon lines to occupied cells and the visible row window, including partially visible items.
    bool ClipSegment(FVector2D& A, FVector2D& B, const FArea& Box)
    {
        const FVector2D Delta = B - A;
        double Low = 0, High = 1;
        auto Boundary = [&](double P, double Q)
        {
            if (FMath::Abs(P) < .000001) return Q >= 0;
            const double T = Q / P;
            if (P < 0) { if (T > High) return false; Low = FMath::Max(Low, T); }
            else { if (T < Low) return false; High = FMath::Min(High, T); }
            return true;
        };
        if (!Boundary(-Delta.X, A.X - Box.X) || !Boundary(Delta.X, Box.Right() - A.X) ||
            !Boundary(-Delta.Y, A.Y - Box.Y) || !Boundary(Delta.Y, Box.Bottom() - A.Y)) return false;
        B = A + Delta * High; A += Delta * Low;
        return true;
    }

    // Original normalized line art; no asset loading or scene-capture dependency in DrawHUD.
    void Silhouette(const FLWItemDefinition& D, TFunctionRef<void(FVector2D, FVector2D, float)> Stroke)
    {
        auto L = [&](float X, float Y, float X2, float Y2, float Weight = 1.5f) { Stroke({X,Y}, {X2,Y2}, Weight); };
        auto Box = [&](float X, float Y, float W, float H) { L(X,Y,X+W,Y); L(X+W,Y,X+W,Y+H); L(X+W,Y+H,X,Y+H); L(X,Y+H,X,Y); };
        auto Path = [&](std::initializer_list<FVector2D> Points, float Weight = 1.5f)
        {
            bool First = true; FVector2D Last = FVector2D::ZeroVector;
            for (FVector2D Point : Points) { if (!First) Stroke(Last, Point, Weight); First = false; Last = Point; }
        };
        if (D.Id == TEXT("crowbar"))
        {
            Path({{.42,.92},{.42,.24},{.52,.12},{.72,.12},{.79,.22}}, 3);
            L(.42,.81,.25,.91,3); L(.29,.91,.20,.91);
        }
        else if (D.Id == TEXT("bat"))
        {
            Path({{.44,.93},{.44,.70},{.32,.27},{.34,.12},{.46,.07},{.59,.10},{.66,.24},{.56,.70},{.56,.93},{.44,.93}},2);
            L(.42,.80,.58,.80); L(.41,.91,.59,.91,3);
        }
        else if (D.Id == TEXT("revolver"))
        {
            Path({{.10,.35},{.64,.35},{.64,.31},{.91,.31},{.91,.44},{.65,.44},{.61,.56},{.48,.56},{.40,.82},{.22,.76},{.31,.49},{.10,.49},{.10,.35}},2);
            Box(.43,.36,.19,.15); L(.48,.36,.48,.51); L(.55,.36,.55,.51);
            Path({{.45,.54},{.55,.59},{.61,.55},{.60,.49}}); L(.23,.32,.30,.27);
        }
        else if (D.Category == TEXT("Weapon"))
        {
            const bool Lmg = D.Id == TEXT("lmg"), Smg = D.Id == TEXT("smg");
            Path({{.06,.47},{.19,.39},{.30,.45},{.69,.45},{.69,.57},{.32,.57},{.20,.69},{.06,.69},{.06,.47}},2);
            Box(.69,.47,.25,.05); L(.94,.46,.94,.55,2);
            if (D.Id == TEXT("shotgun")) { Box(.43,.58,.24,.08); L(.51,.60,.51,.65); L(.57,.60,.57,.65); }
            else
            {
                Path({{.37,.58},{.32,.78},{.40,.80},{.46,.60}},2);
                if (Lmg) Box(.50,.57,.19,.24);
                else Path({{.55,.57},{.53,.82},{.61,.85},{.66,.59}},2);
            }
            if (D.Id == TEXT("sniper")) { Box(.34,.28,.24,.09); L(.39,.37,.39,.45); L(.53,.37,.53,.45); }
            if (Lmg) { L(.78,.54,.74,.85); L(.78,.54,.90,.85); L(.35,.40,.35,.31); L(.35,.31,.51,.31); }
            if (Smg) { L(.08,.54,.25,.51); L(.23,.41,.23,.34); L(.68,.44,.68,.36); }
            if (D.Id == TEXT("rifle")) { L(.56,.42,.56,.32); L(.56,.32,.65,.32); L(.65,.32,.65,.44); }
        }
        else if (D.Category == TEXT("Ammo"))
        {
            for (float X : {.22f,.48f,.74f})
            {
                Path({{X-.07,.79},{X-.07,.35},{X,.20},{X+.07,.35},{X+.07,.79},{X-.07,.79}},1.5f);
                L(X-.07,.65,X+.07,.65); L(X-.08,.76,X+.08,.76);
            }
        }
        else if (D.Category == TEXT("Magazine"))
        {
            if (D.Id == TEXT("mag_lmg100")) { Box(.14,.27,.72,.50); Box(.35,.16,.32,.11); L(.22,.44,.78,.44); }
            else { Path({{.30,.16},{.64,.16},{.65,.52},{.77,.80},{.39,.85},{.29,.54},{.30,.16}},2); L(.40,.28,.45,.69); L(.53,.28,.59,.68); }
        }
        else if (D.Id == TEXT("helmet"))
        {
            Path({{.12,.64},{.18,.33},{.33,.18},{.64,.18},{.82,.35},{.88,.65},{.65,.67},{.59,.79},{.37,.79},{.29,.65},{.12,.64}},2);
            L(.21,.57,.80,.57); L(.35,.67,.39,.85); L(.65,.67,.60,.85);
        }
        else if (D.Id == TEXT("armor") || D.Id == TEXT("rig"))
        {
            Path({{.13,.25},{.34,.14},{.39,.30},{.62,.30},{.68,.14},{.87,.25},{.78,.85},{.22,.85},{.13,.25}},2);
            Box(.31,.39,.38,.33); L(.26,.77,.74,.77);
            if (D.Id == TEXT("rig")) { Box(.24,.49,.15,.26); Box(.43,.49,.15,.26); Box(.62,.49,.15,.26); }
        }
        else if (LWItems::BackpackRows(D.Id)>0)
        {
            Path({{.26,.22},{.37,.13},{.64,.13},{.76,.23},{.81,.81},{.72,.89},{.25,.89},{.18,.79},{.26,.22}},2);
            Box(.29,.51,.40,.28); L(.28,.34,.73,.34); L(.37,.14,.37,.06); L(.37,.06,.63,.06); L(.63,.06,.63,.14);
        }
        else if(D.Id==TEXT("night_vision")){Box(.12,.36,.30,.37);Box(.58,.36,.30,.37);Box(.17,.43,.20,.22);Box(.63,.43,.20,.22);L(.3,.3,.7,.3,3);L(.5,.3,.5,.12,3);}
        else if (D.Id == TEXT("water"))
        {
            Box(.40,.10,.20,.13); Path({{.40,.23},{.28,.36},{.28,.84},{.72,.84},{.72,.36},{.60,.23}},2);
            Box(.30,.47,.40,.21); L(.36,.36,.36,.44);
        }
        else if (D.Id == TEXT("food"))
        {
            Box(.24,.28,.52,.54); Path({{.24,.28},{.34,.18},{.65,.18},{.76,.28}}); L(.24,.37,.76,.37); L(.24,.74,.76,.74); Box(.43,.22,.15,.06);
        }
        else if (D.Id == TEXT("car_key")) { Box(.18,.15,.38,.32); L(.44,.47,.77,.82,3); L(.65,.69,.54,.80,3); L(.73,.76,.65,.86,3); }
        else if (D.Id == TEXT("lockpick")) { Path({{.20,.84},{.63,.25},{.79,.22},{.84,.13}},2); Path({{.39,.87},{.79,.37},{.70,.31}},2); }
        else if (D.Id == TEXT("medkit"))
        {
            Box(.14,.28,.72,.52); Box(.35,.16,.30,.12); L(.50,.39,.50,.68,4); L(.35,.54,.65,.54,4);
        }
        else if (D.Id == TEXT("battery"))
        {
            Box(.25,.24,.50,.62); Box(.40,.13,.20,.11); L(.38,.45,.62,.45); L(.50,.34,.50,.56); L(.39,.72,.61,.72);
        }
        else if (D.Category == TEXT("Tool"))
        {
            Path({{.40,.86},{.32,.78},{.57,.39},{.42,.25},{.50,.12},{.54,.29},{.66,.32},{.75,.18},{.83,.31},{.69,.47},{.46,.84},{.40,.86}},2);
        }
        else
        {
            Path({{.17,.63},{.38,.27},{.52,.38},{.35,.78},{.17,.63}}); Path({{.52,.62},{.62,.19},{.80,.30},{.75,.80},{.52,.62}});
            L(.25,.58,.38,.41); L(.62,.65,.70,.37);
        }
    }

    // Mirrors the character transaction on copies, so previews account for linked magazines and slot swaps.
    bool PreviewMove(const TArray<FLWItemInstance>& Source, const TArray<FLWItemInstance>& Target,
        bool SamePanel, FGuid Id, int32 X, int32 Y, bool Rotated, int32 W, int32 H, FName Slot)
    {
        TArray<FLWItemInstance> A = Source, B = Target;
        if (Slot.IsNone())
        {
            const FLWItemInstance* Item = Find(&A, Id);
            if (!Item || !LWItems::Fits(Target, *Item, X, Y, Rotated, W, H, SamePanel ? Id : FGuid())) return false;
            return SamePanel ? LWItems::Move(A, Id, X, Y, Rotated, W, H) : LWItems::Transfer(A, B, Id, X, Y, Rotated, W, H);
        }
        const FLWItemInstance* Item = Find(&A, Id);
        if (!Item || !LWItems::CanEquip(*Item, Slot)) return false;
        if (SamePanel)
        {
            const FLWItemInstance* Occupant = A.FindByPredicate([&](const FLWItemInstance& I) { return I.Slot == Slot && I.Id != Id; });
            FLWItemInstance Displaced;
            const bool HasDisplaced = Occupant != nullptr;
            if (Occupant)
            {
                Displaced = *Occupant;
                A.RemoveAll([&](const FLWItemInstance& I) { return I.Id == Displaced.Id; });
            }
            if (!LWItems::Equip(A, Id, Slot)) return false;
            if (!HasDisplaced) return true;
            Displaced.Slot = NAME_None;
            return LWItems::Place(A, Displaced, 12, LWItems::InventoryHeight(A)) && LWItems::ValidateEquipment(A);
        }
        if (B.ContainsByPredicate([Slot](const FLWItemInstance& I) { return I.Slot == Slot; })) return false;
        return LWItems::Transfer(A, B, Id, 0, 32, Rotated, 64, 96) && LWItems::Equip(B, Id, Slot);
    }
}

void ALWHUD::InventoryScreen(ALWCharacter* P)
{
    using namespace LWInventoryHUD;
    auto CancelDrag = [&] { DragId.Invalidate(); DragPanel = 0; DragOffset = FVector2D::ZeroVector; };
    if (!IsValid(P) || !Canvas || !PlayerOwner || !P->bInventory || Scale <= 0)
    {
        CancelDrag(); MouseWasDown = false; InventoryLastFrame = 0;
        return;
    }

    const float OriginX = FMath::Max(0.f, (Canvas->ClipX / Scale - 1280.f) * .5f);
    const float OriginY = 0;
    auto Fill = [&](FArea A, FLinearColor C) { if (A.W > 0 && A.H > 0) Rect(OriginX+A.X, OriginY+A.Y, A.W, A.H, C); };
    auto Stroke = [&](FVector2D A, FVector2D B, FLinearColor C, float Weight = 1.f)
    {
        Line(OriginX+float(A.X), OriginY+float(A.Y), OriginX+float(B.X), OriginY+float(B.Y), C, Weight);
    };
    auto Label = [&](const FString& S, float X, float Y, float Size, FLinearColor C)
    {
        Text(S, OriginX+X, OriginY+Y, Size, C);
    };
    auto Border = [&](FArea A, FLinearColor C, float Weight = 1.f)
    {
        Stroke({A.X,A.Y},{A.Right(),A.Y},C,Weight); Stroke({A.Right(),A.Y},{A.Right(),A.Bottom()},C,Weight);
        Stroke({A.Right(),A.Bottom()},{A.X,A.Bottom()},C,Weight); Stroke({A.X,A.Bottom()},{A.X,A.Y},C,Weight);
    };

    float RawX = 0, RawY = 0;
    const bool HasMouse = Mouse55(RawX, RawY);
    const FVector2D Mouse = HasMouse ? FVector2D(RawX/Scale-OriginX, RawY/Scale-OriginY) : FVector2D(-10000,-10000);
    const bool MouseDown = PlayerOwner->IsInputKeyDown(EKeys::LeftMouseButton);
    const bool Interrupted = InventoryLastFrame == 0 || GFrameCounter > InventoryLastFrame + 1;
    InventoryLastFrame = GFrameCounter;
    FName Context;
    if (IsValid(P->OpenObject)) Context = FName(*FString::Printf(TEXT("%d:%s"), int32(P->OpenObject->Kind), *P->OpenObject->RecordId.ToString()));
    if (Interrupted || InventoryContext != Context)
    {
        CancelDrag(); MouseWasDown = MouseDown;
        if (InventoryContext != Context) { InventoryScroll = 0; if (SelectedPanel != 0) SelectedId.Invalidate(); }
        InventoryContext = Context;
    }
    if (!HasMouse) CancelDrag();
    const bool Pressed = HasMouse && !Interrupted && MouseDown &&
        (!MouseWasDown || PlayerOwner->WasInputKeyJustPressed(EKeys::LeftMouseButton));
    const bool Released = HasMouse && !MouseDown &&
        (MouseWasDown || PlayerOwner->WasInputKeyJustReleased(EKeys::LeftMouseButton));

    FGrid PlayerGrid;
    PlayerGrid.PanelId = 0;
    PlayerGrid.Items = P->ItemsFor(0);
    PlayerGrid.Height=LWItems::InventoryHeight(P->Inventory);
    if(PlayerGrid.Bounds().Contains(Mouse)){
        if(PlayerOwner->WasInputKeyJustPressed(EKeys::MouseScrollUp))--PlayerInventoryScroll;
        if(PlayerOwner->WasInputKeyJustPressed(EKeys::MouseScrollDown))++PlayerInventoryScroll;
    }
    PlayerInventoryScroll=FMath::Clamp(PlayerInventoryScroll,0,PlayerGrid.Height-PlayerGrid.Rows);PlayerGrid.FirstRow=PlayerInventoryScroll;
    FGrid OtherGrid;
    OtherGrid.X = 804;
    OtherGrid.PanelId = IsValid(P->OpenObject) && P->OpenObject->Kind == ELWObjectKind::Stash ? 1 : 2;
    OtherGrid.Items = P->ItemsFor(OtherGrid.PanelId);
    const bool Trader = IsValid(P->OpenObject) && P->OpenObject->Kind == ELWObjectKind::Trader;
    FString OtherTitle = TEXT("FIELD STORAGE");
    if (OtherGrid.PanelId == 1)
    {
        OtherGrid.Width = 12; OtherGrid.Height = 14; OtherTitle = TEXT("SECURED STASH");
    }
    else if (IsValid(P->OpenObject) && P->World)
    {
        if (const FLWContainerRecord* Record = P->World->Containers.Find(P->OpenObject->RecordId))
        {
            OtherGrid.Width = FMath::Max(1, Record->Width);
            OtherGrid.Height = FMath::Max(1, Record->Height);
            OtherTitle = Trader ? TEXT("TRADER STOCK") : Record->bDropped ? TEXT("RECOVERY BAG") : Record->Context.ToString().Replace(TEXT("_"),TEXT(" ")).ToUpper();
            if (OtherTitle.IsEmpty() || OtherTitle == TEXT("NONE")) OtherTitle = TEXT("FIELD CACHE");
        }
    }
    OtherGrid.Cell = FMath::Min(32.f, 384.f / OtherGrid.Width);
    OtherGrid.Rows = FMath::Min(OtherGrid.Height, FMath::Max(1, FMath::FloorToInt(448.f / OtherGrid.Cell)));
    const int32 MaxScroll = FMath::Max(0, OtherGrid.Height - OtherGrid.Rows);
    InventoryScroll = FMath::Clamp(InventoryScroll, 0, MaxScroll);
    const FArea OtherPanel{784,90,472,568};
    const FArea CloseButton{1125,28,115,30};
    const float ScrollX = OtherGrid.Bounds().Right() + 12;
    const FArea ScrollUp{ScrollX,144,20,22};
    const FArea ScrollDown{ScrollX,OtherGrid.Bounds().Bottom()-22,20,22};
    const FArea ScrollTrack{ScrollX+3,171,14,FMath::Max(1.f,OtherGrid.Bounds().H-54)};
    bool ConsumedPress = false;
    const FArea PlayerUp{730,144,20,22},PlayerDown{730,442,20,22},PlayerTrack{734,173,12,240};
    if(PlayerGrid.Height>PlayerGrid.Rows){
        if(Pressed&&PlayerUp.Contains(Mouse)){--PlayerInventoryScroll;ConsumedPress=true;}
        if(Pressed&&PlayerDown.Contains(Mouse)){++PlayerInventoryScroll;ConsumedPress=true;}
        if(MouseDown&&!DragId.IsValid()&&PlayerTrack.Contains(Mouse)){PlayerInventoryScroll=FMath::RoundToInt((Mouse.Y-PlayerTrack.Y)/PlayerTrack.H*(PlayerGrid.Height-PlayerGrid.Rows));ConsumedPress=true;}
        PlayerInventoryScroll=FMath::Clamp(PlayerInventoryScroll,0,PlayerGrid.Height-PlayerGrid.Rows);PlayerGrid.FirstRow=PlayerInventoryScroll;
    }
    if(Pressed && Mouse.X>=620 && Mouse.X<760 && Mouse.Y>=97 && Mouse.Y<136){CancelDrag();ConsumedPress=true;}
    if(Pressed && OtherGrid.Items && !Trader && Mouse.X>=1090 && Mouse.X<1248 && Mouse.Y>=97 && Mouse.Y<136){CancelDrag();ConsumedPress=true;}
    if(Pressed && OtherGrid.Items && !Trader && Mouse.X>=790 && Mouse.X<958 && Mouse.Y>=616 && Mouse.Y<652){CancelDrag();ConsumedPress=true;}
    if (Pressed && CloseButton.Contains(Mouse))
    {
        CancelDrag(); MouseWasDown = MouseDown; P->ClosePanels(); InventoryLastFrame = 0;
        return;
    }
    if (OtherGrid.Items && MaxScroll > 0)
    {
        if (OtherPanel.Contains(Mouse))
        {
            if (PlayerOwner->WasInputKeyJustPressed(EKeys::MouseScrollUp)) --InventoryScroll;
            if (PlayerOwner->WasInputKeyJustPressed(EKeys::MouseScrollDown)) ++InventoryScroll;
        }
        if (PlayerOwner->WasInputKeyJustPressed(EKeys::PageUp)) InventoryScroll -= OtherGrid.Rows;
        if (PlayerOwner->WasInputKeyJustPressed(EKeys::PageDown)) InventoryScroll += OtherGrid.Rows;
        if (Pressed && ScrollUp.Contains(Mouse)) { --InventoryScroll; ConsumedPress = true; }
        if (Pressed && ScrollDown.Contains(Mouse)) { ++InventoryScroll; ConsumedPress = true; }
        if (MouseDown && !DragId.IsValid() && ScrollTrack.Contains(Mouse))
        {
            InventoryScroll = FMath::RoundToInt32((Mouse.Y-ScrollTrack.Y)/ScrollTrack.H*MaxScroll);
            ConsumedPress = true;
        }
        InventoryScroll = FMath::Clamp(InventoryScroll, 0, MaxScroll);
    }
    OtherGrid.FirstRow = InventoryScroll;

    const TArray<FEquipmentBox> Equipment = {
        {TEXT("Primary"),{36,135,236,52}}, {TEXT("Secondary"),{36,193,236,52}},
        {TEXT("RigPrimary"),{36,251,236,52}},
        {TEXT("Sidearm"),{36,309,114,52}}, {TEXT("Melee"),{158,309,114,52}},
        {TEXT("Armor"),{36,367,114,52}}, {TEXT("Helmet"),{158,367,114,52}},
        {TEXT("Rig"),{36,425,114,52}}, {TEXT("Backpack"),{158,425,114,52}},
        {TEXT("Tool"),{36,483,236,52}}
    };
    auto HitTest = [&]() -> FHit
    {
        for (const FEquipmentBox& Box : Equipment)
        {
            if (!Box.Area.Contains(Mouse)) continue;
            FHit Hit; Hit.PanelId = 0; Hit.Slot = Box.Slot; Hit.Equipment = &Box;
            if (PlayerGrid.Items) if (const FLWItemInstance* I = PlayerGrid.Items->FindByPredicate([&](const FLWItemInstance& V) { return V.Slot == Box.Slot && V.Count > 0; })) Hit.Id = I->Id;
            return Hit;
        }
        for (const FGrid* Grid : {&PlayerGrid, &OtherGrid})
        {
            if (!Grid->Items || !Grid->Bounds().Contains(Mouse)) continue;
            FHit Hit; Hit.PanelId = Grid->PanelId; Hit.Grid = Grid;
            const FIntPoint Cell = Grid->CellAt(Mouse);
            for (const FLWItemInstance& I : *Grid->Items)
            {
                if (!I.Slot.IsNone() || I.Count <= 0) continue;
                if (LWItems::Footprint(I.Definition, I.bRotated).Contains(Cell-FIntPoint(I.X,I.Y))) { Hit.Id = I.Id; break; }
            }
            return Hit;
        }
        return {};
    };
    auto RefreshArrays = [&]
    {
        PlayerGrid.Items = P->ItemsFor(0);
        OtherGrid.Items = P->ItemsFor(OtherGrid.PanelId);
    };
    auto EquipmentArt = [](const FLWItemInstance& I, const FEquipmentBox& Box, float& Cell, FVector2D& Origin)
    {
        const FLWItemDefinition& D = LWItems::Def(I.Definition);
        const int32 W = FMath::Max(1, I.bRotated ? D.Height : D.Width), H = FMath::Max(1, I.bRotated ? D.Width : D.Height);
        Cell = FMath::Min((Box.Area.W-18)/W, (Box.Area.H-24)/H);
        Origin = {Box.Area.X+(Box.Area.W-W*Cell)*.5f, Box.Area.Y+19+(Box.Area.H-23-H*Cell)*.5f};
    };
    FHit Hover = HitTest();
    if (DragId.IsValid() && !Find(P->ItemsFor(DragPanel), DragId)) CancelDrag();
    if (SelectedId.IsValid() && !Find(P->ItemsFor(SelectedPanel), SelectedId)) SelectedId.Invalidate();
    if(Pressed&&!ConsumedPress&&Hover.Id.IsValid()&&P->OpenObject&&(PlayerOwner->IsInputKeyDown(EKeys::LeftShift)||PlayerOwner->IsInputKeyDown(EKeys::RightShift))){const FGuid Id=Hover.Id;CancelDrag();P->QuickTransferItem(Hover.PanelId,Id);RefreshArrays();Hover=HitTest();ConsumedPress=true;}
    if (Pressed && !ConsumedPress)
    {
        SelectedId = Hover.Id; SelectedPanel = Hover.PanelId;
        if (const FLWItemInstance* I = Find(P->ItemsFor(Hover.PanelId), Hover.Id))
        {
            if (I->Slot != TEXT("Loaded"))
            {
                DragId = I->Id; DragPanel = Hover.PanelId; bDragRotated = I->bRotated;
                if (Hover.Grid)
                {
                    DragOffset = {(Mouse.X-Hover.Grid->X)/Hover.Grid->Cell-I->X,
                        (Mouse.Y-Hover.Grid->Y)/Hover.Grid->Cell+Hover.Grid->FirstRow-I->Y};
                }
                else if (Hover.Equipment)
                {
                    float Cell; FVector2D Origin; EquipmentArt(*I, *Hover.Equipment, Cell, Origin);
                    DragOffset = (Mouse-Origin)/FMath::Max(Cell,1.f);
                    const TArray<FIntPoint> Cells = LWItems::Footprint(I->Definition,I->bRotated);
                    const FIntPoint GrabCell(FMath::FloorToInt32(DragOffset.X),FMath::FloorToInt32(DragOffset.Y));
                    if (!Cells.Contains(GrabCell))
                    {
                        double BestDistance = TNumericLimits<double>::Max();
                        FVector2D Nearest = FVector2D::ZeroVector;
                        for (FIntPoint C : Cells)
                        {
                            const FVector2D Center(C.X+.5,C.Y+.5);
                            const double Distance = (Center-DragOffset).SizeSquared();
                            if (Distance < BestDistance) { BestDistance = Distance; Nearest = Center; }
                        }
                        DragOffset = Nearest;
                    }
                }
            }
        }
    }

    auto Transaction = [&](TFunctionRef<bool()> Action, const TCHAR* Failure)
    {
        const FString BeforeMessage = P->Message;
        const float BeforeTime = P->MessageTime;
        const bool Result = Action();
        if (!Result && P->Message == BeforeMessage && P->MessageTime <= BeforeTime) P->Notify(Failure,2.5f);
        RefreshArrays();
        return Result;
    };
    const bool RightPressed = PlayerOwner->WasInputKeyJustPressed(EKeys::RightMouseButton);
    if (RightPressed && DragId.IsValid()) CancelDrag();
    else if (!DragId.IsValid())
    {
        if (RightPressed && Hover.Id.IsValid()) { SelectedId = Hover.Id; SelectedPanel = Hover.PanelId; }
        const bool Unload = PlayerOwner->WasInputKeyJustPressed(EKeys::U);
        const bool Drop = PlayerOwner->WasInputKeyJustPressed(EKeys::Delete);
        if ((RightPressed || Unload || Drop) && SelectedId.IsValid())
        {
            if (SelectedPanel != 0) P->Notify(TEXT("MOVE OR BUY THIS ITEM INTO YOUR INVENTORY FIRST"),2.5f);
            else if (RightPressed) Transaction([&] { return P->UseItem(SelectedId); },TEXT("ITEM CANNOT BE USED OR EQUIPPED"));
            else if (Unload) Transaction([&] { return P->UnloadItem(SelectedId); },TEXT("NOTHING TO UNLOAD OR NO FREE SPACE"));
            else if (Drop) Transaction([&] { return P->DropItem(SelectedId); },TEXT("THIS ITEM CANNOT BE DROPPED HERE"));
        }
    }

    // I and Escape are already bound to character panel actions; polling them here would close on the opening keypress.
    FLWItemInstance Dragged;
    bool HasDrag = false, ValidDrop = false, Combine = false;
    int32 DropX = 0, DropY = 0;
    FString DropHint = TEXT("DRAG TO AN OPEN GRID OR EQUIPMENT SLOT");
    Hover = HitTest();
    if (const FLWItemInstance* I = Find(P->ItemsFor(DragPanel), DragId))
    {
        Dragged = *I; HasDrag = true;
        const FLWItemDefinition& D = LWItems::Def(Dragged.Definition);
        if (PlayerOwner->WasInputKeyJustPressed(EKeys::R))
        {
            // Transform the continuous grab point together with the clockwise footprint rotation.
            DragOffset = bDragRotated ? FVector2D(DragOffset.Y,D.Height-DragOffset.X) : FVector2D(D.Height-DragOffset.Y,DragOffset.X);
            bDragRotated = !bDragRotated;
        }
        if (Hover.Grid)
        {
            DropX = FMath::FloorToInt32((Mouse.X-Hover.Grid->X)/Hover.Grid->Cell-DragOffset.X+.0001);
            DropY = FMath::FloorToInt32((Mouse.Y-Hover.Grid->Y)/Hover.Grid->Cell+Hover.Grid->FirstRow-DragOffset.Y+.0001);
        }
        const TArray<FLWItemInstance>* Source = P->ItemsFor(DragPanel);
        const TArray<FLWItemInstance>* Target = P->ItemsFor(Hover.PanelId);
        const FLWItemInstance* TargetItem = Find(Target,Hover.Id);
        Combine = TargetItem && TargetItem->Id != DragId &&
            (D.Category == TEXT("Attachment") || D.Category == TEXT("Ammo") || (D.MaxStack > 1 && TargetItem->Definition == Dragged.Definition));
        if (Combine)
        {
            FLWItemInstance Ammo = Dragged, Mag = *TargetItem;
            ValidDrop = !(Trader && (DragPanel == 2 || Hover.PanelId == 2)) &&
                (LWMods::Compatible(Mag.Definition,Ammo.Definition) || LWItems::LoadMagazine(Mag,Ammo) > 0 || (Ammo.Definition == Mag.Definition && D.MaxStack > Mag.Count && Ammo.Count > 0));
            DropHint = ValidDrop ? (D.Category==TEXT("Attachment")?TEXT("RELEASE TO INSTALL ATTACHMENT"):D.Category == TEXT("Ammo") && LWItems::Def(Mag.Definition).Category == TEXT("Magazine") ? TEXT("RELEASE TO LOAD MAGAZINE") : TEXT("RELEASE TO COMBINE STACKS")) : TEXT("INCOMPATIBLE / FULL / BUY BEFORE MODIFYING");
        }
        else if (Source && Target && (Hover.Grid || !Hover.Slot.IsNone()))
        {
            ValidDrop = PreviewMove(*Source,*Target,DragPanel==Hover.PanelId,DragId,DropX,DropY,bDragRotated,
                Hover.Grid ? Hover.Grid->Width : 12,Hover.Grid ? Hover.Grid->Height : 10,Hover.Slot);
            DropHint = ValidDrop ? (Hover.Slot.IsNone() ? TEXT("RELEASE TO MOVE") : TEXT("RELEASE TO EQUIP / SWAP")) : TEXT("BLOCKED // CHECK SPACE, SHAPE OR SLOT");
            if (Trader && DragPanel == 2 && Hover.PanelId == 0)
            {
                if (Price(Dragged) > P->Money) { ValidDrop = false; DropHint = TEXT("INSUFFICIENT CREDITS"); }
                else if (ValidDrop) DropHint = FString::Printf(TEXT("RELEASE TO BUY // $%d"),Price(Dragged));
            }
            else if (Trader && DragPanel == 0 && Hover.PanelId == 2 && ValidDrop)
                DropHint = FString::Printf(TEXT("RELEASE TO SELL // $%d"),Price(Dragged)/2);
        }
        if (Released)
        {
            const FGuid MovingId = DragId;
            const int32 FromPanel = DragPanel;
            const bool NoChange = Hover.PanelId == FromPanel && !Combine &&
                ((!Hover.Slot.IsNone() && Hover.Slot == Dragged.Slot) ||
                    (Hover.Grid && Dragged.Slot.IsNone() && Dragged.X == DropX && Dragged.Y == DropY && Dragged.bRotated == bDragRotated));
            if (!NoChange && Target && (Hover.Grid || !Hover.Slot.IsNone()))
            {
                const bool Result = Combine
                    ? Transaction([&] { return P->LoadAmmoOnto(FromPanel,MovingId,Hover.PanelId,Hover.Id); },TEXT("ITEMS CANNOT BE COMBINED"))
                    : Transaction([&] { return P->MoveItem(FromPanel,Hover.PanelId,MovingId,DropX,DropY,bDragRotated,Hover.Slot); },TEXT("MOVE FAILED // ITEM REMAINS IN ITS SOURCE"));
                if (Result) { SelectedId = Combine ? Hover.Id : MovingId; SelectedPanel = Hover.PanelId; }
            }
            else if (!NoChange) P->Notify(TEXT("DRAG CANCELLED // ITEM REMAINS IN ITS SOURCE"),1.5f);
            CancelDrag(); HasDrag = false;
            Hover = HitTest();
        }
    }
    MouseWasDown = MouseDown;
    RefreshArrays();
    if (SelectedId.IsValid() && !Find(P->ItemsFor(SelectedPanel),SelectedId)) SelectedId.Invalidate();

    // Rendering follows input and reacquires arrays after every transaction; no stale item pointer survives a mutation.
    Rect(0,0,Canvas->ClipX/Scale,Canvas->ClipY/Scale,FLinearColor(.008f,.014f,.010f,.91f));
    Fill({16,16,1248,690},FLinearColor(.020f,.028f,.022f,.97f));
    for (int32 Y = 20; Y < 704; Y += 4) Fill({20,float(Y),1240,1},FLinearColor(.22f,.28f,.18f,.045f));
    Stroke({24,77},{1256,77},InvEdge);
    Label(FitText(FString::Printf(TEXT("$%lld"),P->Money),200),1040,64,.9f,InvAmber);
    Label(FString::Printf(TEXT("CREDIT  $%06lld"),P->Money),790,36,1.1f,InvAmber);
    Fill(CloseButton,CloseButton.Contains(Mouse) ? FLinearColor(.29f,.19f,.10f) : FLinearColor(.08f,.10f,.07f));
    Border(CloseButton,CloseButton.Contains(Mouse) ? InvAmber : InvEdge);
    Label(TEXT("[I] CLOSE"),CloseButton.X+10,CloseButton.Y+7,.9f,InvBone);

    const FArea EquipmentPanel{24,90,260,568}, InventoryPanel{304,90,460,568};
    for (FArea A : {EquipmentPanel,InventoryPanel,OtherPanel}) { Fill(A,InvPanel); Border(A,InvEdge); Fill({A.X,A.Y,3,26},InvAmber); }
    Label(TEXT("EQUIPMENT"),36,103,1.f,InvBone);
    Label(TEXT("ITEMS"),320,103,1.f,InvBone);
    Control55(TEXT("inv_sort55"),TEXT("AUTO SORT"),OriginX+620,OriginY+97,140,34,false,true);
    if(OtherGrid.Items&&!Trader){Control55(TEXT("inv_all55"),TEXT("TAKE ALL"),OriginX+790,OriginY+616,168,34,false,true);}
    if(OtherGrid.Items&&!Trader){Control55(TEXT("inv_other55"),TEXT("AUTO SORT"),OriginX+1090,OriginY+97,158,34,false,true);}
    Label(FitText(TEXT("")+OtherTitle,436,1.f),800,103,1.f,InvBone);

    auto DrawItem = [&](const FLWItemInstance& I, const TArray<FLWItemInstance>* Items,
        FVector2D Origin, float Cell, bool Rotated, FArea Clip, bool Selected, bool Ghost, bool Allowed, bool Compact)
    {
        const FLWItemDefinition& D = LWItems::Def(I.Definition);
        const TArray<FIntPoint> Cells = LWItems::Footprint(I.Definition,Rotated);
        if (Cells.IsEmpty()) return;
        TSet<FIntPoint> Occupied;
        for (FIntPoint C : Cells) Occupied.Add(C);
        const bool SourceOfDrag = !Ghost && I.Id == DragId;
        const float Alpha = Ghost ? .76f : SourceOfDrag ? .35f : 1.f;
        FLinearColor Background = Ghost ? (Allowed ? FLinearColor(.16f,.40f,.14f,.66f) : FLinearColor(.43f,.12f,.08f,.66f)) : Tint(D);
        Background.A *= Alpha;
        FLinearColor Outline = Ghost ? (Allowed ? InvGreen : InvRed) : (D.Category==TEXT("Weapon")||D.Category==TEXT("WeaponPart")) ? LWMods::TierColor(I.WeaponTier) : Selected ? InvAmber : FLinearColor(.39f,.43f,.32f);
        Outline.A = Alpha;
        TArray<FArea> VisibleCells;
        int32 FirstVisible = MAX_int32, LastVisible = MIN_int32;
        for (FIntPoint C : Cells)
        {
            const FArea Tile{float(Origin.X)+C.X*Cell,float(Origin.Y)+C.Y*Cell,Cell,Cell};
            FArea Visible;
            if (!Tile.Intersect(Clip,Visible)) continue;
            VisibleCells.Add(Visible);
            FirstVisible = FMath::Min(FirstVisible,C.Y); LastVisible = FMath::Max(LastVisible,C.Y);
            Fill({Visible.X+.6f,Visible.Y+.6f,FMath::Max(0.f,Visible.W-1.2f),FMath::Max(0.f,Visible.H-1.2f)},Background);
            auto EdgeLine = [&](FVector2D A,FVector2D B) { if (ClipSegment(A,B,Clip)) Stroke(A,B,Outline,Selected||Ghost ? 1.5f : 1.f); };
            if (!Occupied.Contains(C+FIntPoint(-1,0))) EdgeLine({Tile.X+1,Tile.Y+1},{Tile.X+1,Tile.Bottom()-1});
            if (!Occupied.Contains(C+FIntPoint(1,0))) EdgeLine({Tile.Right()-1,Tile.Y+1},{Tile.Right()-1,Tile.Bottom()-1});
            if (!Occupied.Contains(C+FIntPoint(0,-1))) EdgeLine({Tile.X+1,Tile.Y+1},{Tile.Right()-1,Tile.Y+1});
            if (!Occupied.Contains(C+FIntPoint(0,1))) EdgeLine({Tile.X+1,Tile.Bottom()-1},{Tile.Right()-1,Tile.Bottom()-1});
        }
        if (VisibleCells.IsEmpty()) return;
        auto Transform = [&](FVector2D Point)
        {
            Point.X *= D.Width; Point.Y *= D.Height;
            if (Rotated) Point = {D.Height-Point.Y,Point.X};
            return Origin+Point*Cell;
        };
        Silhouette(D,[&](FVector2D A,FVector2D B,float Weight)
        {
            A = Transform(A); B = Transform(B);
            for (const FArea& Visible : VisibleCells)
            {
                FVector2D ClippedA = A, ClippedB = B;
                const FArea Inset{Visible.X+1,Visible.Y+1,FMath::Max(0.f,Visible.W-2),FMath::Max(0.f,Visible.H-2)};
                if (Inset.W > 0 && Inset.H > 0 && ClipSegment(ClippedA,ClippedB,Inset))
                    Stroke(ClippedA,ClippedB,FLinearColor(.70f,.73f,.58f,Alpha*.9f),FMath::Max(.7f,Weight*FMath::Min(1.f,Cell/28.f)));
            }
        });
        if (Compact || Cell < 22) return;
        auto WidestRun = [&](int32 Row) -> FArea
        {
            const int32 W = Rotated ? D.Height : D.Width;
            int32 Start = 0, BestStart = 0, Length = 0, BestLength = 0;
            for (int32 Col = 0; Col <= W; ++Col)
            {
                if (Occupied.Contains(FIntPoint(Col,Row))) { if (Length == 0) Start = Col; ++Length; }
                else { if (Length > BestLength) { BestStart = Start; BestLength = Length; } Length = 0; }
            }
            return {float(Origin.X)+BestStart*Cell,float(Origin.Y)+Row*Cell,BestLength*Cell,Cell};
        };
        auto TileLabel = [&](FString S, FArea Run, float Y, bool AlignRight, FLinearColor Color)
        {
            S = FitText(S,Run.W-6,.8f);
            const float W = S.Len()*7.2f;
            const float X = AlignRight ? Run.Right()-W-3 : Run.X+3;
            if (S.IsEmpty() || X < Clip.X || X+W > Clip.Right() || Y < Clip.Y || Y+15 > Clip.Bottom()) return;
            Fill({X-1,Y,W+2,14},FLinearColor(.02f,.027f,.018f,.72f*Alpha));
            Color.A = Alpha; Label(S,X,Y,.8f,Color);
        };
        const FArea Top = WidestRun(FirstVisible), Bottom = WidestRun(LastVisible);
        TileLabel(!I.CustomName39.IsEmpty()?I.CustomName39:Top.W < 96 ? ShortName(I.Definition) : Name(D),Top,Top.Y+2,false,(D.Category==TEXT("Weapon")||D.Category==TEXT("WeaponPart"))?LWMods::TierColor(I.WeaponTier):InvBone);
        FString Status = RoundLabel(I,Items);
        if (Bottom.W < 48 && D.Category == TEXT("Magazine")) Status = FString::FromInt(I.Rounds);
        TileLabel(Status,Bottom,Bottom.Bottom()-16,true,InvBone);
        if (D.MaxDurability > 0 && D.Category != TEXT("Consumable") && Bottom.Bottom() <= Clip.Bottom())
        {
            const float Fraction = FMath::Clamp(float(I.Durability)/D.MaxDurability,0.f,1.f);
            Fill({Bottom.X+2,Bottom.Bottom()-3,(Bottom.W-4)*Fraction,1},Fraction < .3f ? InvRed : InvMuted);
        }
    };

    auto DrawGrid = [&](const FGrid& Grid)
    {
        const FArea Bounds = Grid.Bounds();
        Fill(Bounds,FLinearColor(.025f,.034f,.026f));
        for (int32 Row = 0; Row < Grid.Rows; ++Row)
        {
            for (int32 Col = 0; Col < Grid.Width; ++Col)
            {
                Fill({Grid.X+Col*Grid.Cell+1,Grid.Y+Row*Grid.Cell+1,Grid.Cell-2,Grid.Cell-2},
                    (Row+Col)%2 ? FLinearColor(.035f,.051f,.057f) : FLinearColor(.027f,.042f,.048f));
            }
        }
        Border(Bounds,InvEdge);
        if (!Grid.Items) return;
        for (const FLWItemInstance& Item : *Grid.Items)
        {
            if (!Item.Slot.IsNone() || Item.Count <= 0) continue;
            DrawItem(Item,Grid.Items,{Grid.X+Item.X*Grid.Cell,Grid.Y+(Item.Y-Grid.FirstRow)*Grid.Cell},Grid.Cell,Item.bRotated,Bounds,
                (SelectedId == Item.Id && SelectedPanel == Grid.PanelId) || (Hover.Id == Item.Id && Hover.PanelId == Grid.PanelId),false,false,false);
        }
    };
    DrawGrid(PlayerGrid);
    Label(FString::Printf(TEXT("%d SLOTS"),12*PlayerGrid.Height),330,476,.7f,InvMuted);
    if(PlayerGrid.Height>PlayerGrid.Rows){
        Label(TEXT("^"),734,147,.8f,InvBone);Label(TEXT("v"),734,445,.8f,InvBone);Fill(PlayerTrack,InvEdge);
        const float Thumb=PlayerTrack.H*PlayerGrid.Rows/PlayerGrid.Height;
        Fill({PlayerTrack.X,PlayerTrack.Y+(PlayerTrack.H-Thumb)*PlayerInventoryScroll/(PlayerGrid.Height-PlayerGrid.Rows),PlayerTrack.W,Thumb},InvMuted);
    }
    int32 CarriedCells = 0, CarriedStacks = 0;
    if (PlayerGrid.Items) for (const FLWItemInstance& I : *PlayerGrid.Items) if (I.Slot.IsNone() && I.Count > 0) { CarriedCells += LWItems::Footprint(I.Definition,I.bRotated).Num(); ++CarriedStacks; }

    for (const FEquipmentBox& Box : Equipment)
    {
        const FLWItemInstance* Item = PlayerGrid.Items ? PlayerGrid.Items->FindByPredicate([&](const FLWItemInstance& I) { return I.Slot == Box.Slot && I.Count > 0; }) : nullptr;
        const bool TargetSlot = HasDrag && Hover.Slot == Box.Slot;
        const bool Selected = Item && SelectedPanel == 0 && SelectedId == Item->Id;
        Fill(Box.Area,TargetSlot ? (ValidDrop ? FLinearColor(.10f,.22f,.085f) : FLinearColor(.26f,.09f,.06f)) : FLinearColor(.062f,.081f,.056f));
        Border(Box.Area,TargetSlot ? (ValidDrop ? InvGreen : InvRed) : Selected ? InvAmber : InvEdge,TargetSlot ? 2.f : 1.f);
        Label(Box.Slot==TEXT("RigPrimary")?TEXT("RIG PRIMARY [5]"):Box.Slot.ToString().ToUpper(),Box.Area.X+7,Box.Area.Y+4,.8f,Item ? InvBone : InvMuted);
        if (Item)
        {
            float Cell; FVector2D Origin; EquipmentArt(*Item,Box,Cell,Origin);
            DrawItem(*Item,PlayerGrid.Items,Origin,Cell,Item->bRotated,{Box.Area.X+3,Box.Area.Y+19,Box.Area.W-6,Box.Area.H-22},false,false,false,true);
            const FString Stats = RoundLabel(*Item,PlayerGrid.Items);
            const float StatsWidth = FMath::Min(float(Stats.Len())*7.2f,Box.Area.W*.45f);
            const FString ItemName = FitText(Name(LWItems::Def(Item->Definition)),Box.Area.W-StatsWidth-21);
            Fill({Box.Area.X+5,Box.Area.Bottom()-17,ItemName.Len()*7.2f+3,15},FLinearColor(.023f,.030f,.021f,.85f));
            Label(ItemName,Box.Area.X+6,Box.Area.Bottom()-16,.8f,LWItems::Def(Item->Definition).Category==TEXT("Weapon")?LWMods::TierColor(Item->WeaponTier):InvBone);
            if (!Stats.IsEmpty()) Label(FitText(Stats,Box.Area.W*.45f),Box.Area.Right()-FMath::Min(float(Stats.Len())*7.2f,Box.Area.W*.45f)-7,Box.Area.Bottom()-16,.8f,InvBone);
            if (Item->Id == P->ActiveWeaponId) Fill({Box.Area.Right()-9,Box.Area.Y+7,4,4},InvAmber);
        }
        else
        {
            Label(TEXT("- - -"),Box.Area.X+Box.Area.W*.5f-18,Box.Area.Y+30,.8f,InvMuted);
        }
    }
    Label(TEXT("SURVIVOR CONDITION"),36,570,.8f,InvMuted);
    auto Vital = [&](const TCHAR* Title,float Value,float Y,FLinearColor Color,float Maximum=100.f)
    {
        Label(Title,36,Y,.8f,InvMuted); Fill({96,Y+5,119,5},FLinearColor(.11f,.14f,.095f));
        Fill({96,Y+5,119*FMath::Clamp(Value/FMath::Max(1.f,Maximum),0.f,1.f),5},Value < Maximum*.25f ? InvRed : Color);
        Label(FString::Printf(TEXT("%03d"),FMath::CeilToInt(Value)),234,Y,.8f,InvBone);
    };
    Vital(TEXT("HEALTH"),P->Health,590,InvBone,P->MaxHealth()); Vital(TEXT("FOOD"),P->Hunger,611,InvMuted); Vital(TEXT("WATER"),P->Thirst,632,InvMuted);

    if (OtherGrid.Items)
    {
        DrawGrid(OtherGrid);
        if (MaxScroll > 0)
        {
            Fill(ScrollUp,ScrollUp.Contains(Mouse) ? InvEdge : FLinearColor(.08f,.10f,.07f));
            Fill(ScrollDown,ScrollDown.Contains(Mouse) ? InvEdge : FLinearColor(.08f,.10f,.07f));
            Label(TEXT("^"),ScrollUp.X+6,ScrollUp.Y+3,.8f,InventoryScroll > 0 ? InvBone : InvMuted);
            Label(TEXT("v"),ScrollDown.X+6,ScrollDown.Y+3,.8f,InventoryScroll < MaxScroll ? InvBone : InvMuted);
            Fill(ScrollTrack,FLinearColor(.10f,.13f,.085f));
            const float Thumb = FMath::Max(18.f,ScrollTrack.H*OtherGrid.Rows/OtherGrid.Height);
            Fill({ScrollTrack.X,ScrollTrack.Y+(ScrollTrack.H-Thumb)*InventoryScroll/MaxScroll,ScrollTrack.W,Thumb},InvMuted);
        }
    }
    else
    {
        Border({985,295,64,46},InvEdge,2);Stroke({985,307},{1049,307},InvEdge,2);Fill({1010,303,14,8},InvEdge);
        Label(TEXT("NO CONTAINER OPEN"),935,366,.95f,InvMuted);
        
    }

    const FLWItemInstance* Inspect = HasDrag ? &Dragged : Find(P->ItemsFor(Hover.PanelId),Hover.Id);
    int32 InspectPanel = HasDrag ? DragPanel : Hover.PanelId;
    if (!Inspect) { InspectPanel = SelectedPanel; Inspect = Find(P->ItemsFor(SelectedPanel),SelectedId); }
    const FArea Details{320,503,428,144};
    Fill(Details,FLinearColor(.057f,.073f,.050f));
    Stroke({320,503},{748,503},InvEdge);
    if (Inspect)
    {
        const FLWItemDefinition& D = LWItems::Def(Inspect->Definition);
        const bool InspectRotated = HasDrag ? bDragRotated : Inspect->bRotated;
        Label(FitText(((D.Category==TEXT("Weapon")||D.Category==TEXT("WeaponPart"))?FString(LWMods::TierName(Inspect->WeaponTier))+TEXT(" "):FString())+(Inspect->CustomName39.IsEmpty()?Name(D):Inspect->CustomName39),408,1.05f),330,511,1.05f,(D.Category==TEXT("Weapon")||D.Category==TEXT("WeaponPart"))?LWMods::TierColor(Inspect->WeaponTier):InvAmber);
        Label(FitText(FString::Printf(TEXT("%s / %dx%d / STACK %d OF %d"),*D.Category.ToString().ToUpper(),
            InspectRotated ? D.Height : D.Width,InspectRotated ? D.Width : D.Height,Inspect->Count,D.MaxStack),408),330,534,.8f,InvMuted);
        FString DetailA, DetailB;
        if (!Inspect->Cylinder.IsEmpty())
        {
            int32 Live = 0, Spent = 0;
            for (int32 State : Inspect->Cylinder) { Live += State == 1 ? 1 : 0; Spent += State == 2 ? 1 : 0; }
            DetailA = FString::Printf(TEXT("%s / %s %d LIVE / %d SPENT"),*ShortName(D.AmmoType),(D.WeaponIndex==8||D.WeaponIndex==11)?TEXT("BARRELS"):TEXT("CYLINDER"),Live,Spent);
            FString States;
            for (int32 Index = 0; Index < Inspect->Cylinder.Num(); ++Index)
                States += FString::Printf(TEXT("%s%s "),Index == Inspect->CylinderIndex ? TEXT(">") : TEXT(""),Inspect->Cylinder[Index] == 1 ? TEXT("O") : Inspect->Cylinder[Index] == 2 ? TEXT("X") : TEXT("-"));
            DetailB = States+TEXT(" O LIVE  X SPENT  - EMPTY");
        }
        else if (D.Category == TEXT("Weapon") && !D.AmmoType.IsNone())
        {
            const FLWItemInstance* Mag = Find(P->ItemsFor(InspectPanel),Inspect->LoadedMagazine);
            const TCHAR* Chamber = Inspect->Chamber == 1 ? TEXT("LIVE") : Inspect->Chamber == 2 ? TEXT("SPENT") : TEXT("EMPTY");
            DetailA = FString::Printf(TEXT("AMMO %s / CHAMBER %s"),*ShortName(D.AmmoType),Chamber);
            if (!D.MagazineType.IsNone())
                DetailB = Mag ? FString::Printf(TEXT("%s: %d/%d"),*Name(LWItems::Def(Mag->Definition)),Mag->Rounds,LWItems::Def(Mag->Definition).Capacity) : FString::Printf(TEXT("NO MAG / TAKES %s"),*Name(LWItems::Def(D.MagazineType)));
            else if(D.WeaponIndex==9){DetailA=TEXT("LOAD ONE LOOSE MISSILE WITH [R]");DetailB=FString::Printf(TEXT("MISSILE %d / 1"),Inspect->Rounds);}
            else if(D.WeaponIndex==14){DetailA=TEXT("ONE BATTERY SUPPLIES TEN SHOTS / [R] REPLACE");DetailB=FString::Printf(TEXT("CHARGE %d / 10"),Inspect->Rounds);}
            else DetailB = FString::Printf(TEXT("INTEGRAL TUBE %d/%d + CHAMBER"),Inspect->Rounds,D.WeaponIndex==2?5:D.Capacity);
        }
        else if (D.Category == TEXT("Magazine"))
        {
            DetailA = FString::Printf(TEXT("%s / %d OF %d ROUNDS"),*Name(LWItems::Def(D.AmmoType)),Inspect->Rounds,D.Capacity);
            DetailB = TEXT("DRAG COMPATIBLE LOOSE AMMO ONTO MAGAZINE");
        }
        else if (D.Category == TEXT("Ammo"))
        {
            DetailA = FString::Printf(TEXT("%d LOOSE ROUNDS / STACK LIMIT %d"),Inspect->Count,D.MaxStack);
            DetailB = D.Id==TEXT("ammo_rocket")?TEXT("[R] LOAD DIRECTLY INTO MISSILE LAUNCHER"):
                (D.Id==TEXT("ammo_12g")||D.Id==TEXT("ammo_357"))?TEXT("[R] LOAD LOOSE ROUNDS INTO EQUIPPED WEAPON"):
                TEXT("DROP ON A MAGAZINE TO LOAD / SAME AMMO TO STACK");
        }
        else if(D.Id==TEXT("night_vision")){DetailA=TEXT("HELMET SLOT / NIGHT VISION");DetailB=TEXT("[N] LOWER / RAISE GOGGLES");}
        else if(D.Id==TEXT("rig")){DetailA=TEXT("ADDITIONAL PRIMARY WEAPON SLOT");DetailB=TEXT("[5] EQUIP RIG WEAPON");}
        else if(LWItems::BackpackRows(D.Id)>0){DetailA=FString::Printf(TEXT("+%d INVENTORY SLOTS WHEN EQUIPPED"),12*LWItems::BackpackRows(D.Id));DetailB=TEXT("SCROLL OVER CARRIED GRID TO REACH EXTRA ROWS");}
        else if (D.ContainerWidth > 0)
        {
            DetailA = FString::Printf(TEXT("CONTAINER GRID %d x %d"),D.ContainerWidth,D.ContainerHeight);
            DetailB = TEXT("DRAG INTO ITS MATCHING EQUIPMENT SLOT");
        }
        else if (D.Id == TEXT("gas_can")) {DetailA=FString::Printf(TEXT("FUEL %d/%d L"),Inspect->Rounds,LWFuel::CanCapacity);DetailB=TEXT("PARK AND SWITCH OFF THE ENGINE TO REFUEL");}
        else if (D.Id == TEXT("car_key")) {DetailA=TEXT("MATCHES VIN ")+Inspect->VehicleId.ToString(EGuidFormats::Digits);DetailB=TEXT("KEEP IN CARRIED INVENTORY TO UNLOCK AND START THAT CAR");}
        else if (D.Id == TEXT("lockpick")) {DetailA=TEXT("PICK DOOR AND CONTAINER LOCKS / E AT A LOCK");DetailB=TEXT("EXCESS TORQUE DAMAGES THE CURRENT PICK");}
        else if (D.Id == TEXT("medkit")) { DetailA = FString::Printf(TEXT("RESTORES UP TO %d HEALTH"),FMath::RoundToInt(65*(1+P->Stat(TEXT("healing"))))); DetailB = TEXT("RIGHT CLICK TO USE ONE MEDICAL KIT"); }
        else if (D.Id == TEXT("food")) { DetailA = TEXT("RESTORES UP TO 45 FOOD"); DetailB = TEXT("RIGHT CLICK TO EAT ONE PORTION"); }
        else if (D.Id == TEXT("water")) { DetailA = TEXT("RESTORES UP TO 55 WATER"); DetailB = TEXT("RIGHT CLICK TO DRINK ONE BOTTLE"); }
        else { DetailA = D.EquipSlots.IsEmpty() ? TEXT("SUPPLY / TRADE ITEM") : TEXT("RIGHT CLICK TO EQUIP OR SELECT WEAPON"); DetailB = TEXT("DRAG TO MOVE / R ROTATES WHILE DRAGGING"); }
        if(D.Category==TEXT("WeaponPart")){const auto* Part=LWParts39::Find(D.Id);DetailA=Part?Part->Group.ToString().ToUpper()+TEXT(" / WEAPON WORKBENCH"):FString();DetailB=LWMods::Modifier(Inspect).IsNone()?TEXT("BUILD OR MODIFY WEAPONS AT A WORKBENCH"):LWMods::Modifier(Inspect).ToString().ToUpper();}
        if(D.Category==TEXT("Attachment")){DetailA=TEXT("MOUNT: ")+LWMods::Mount(D.Id).ToString();DetailB=TEXT("DRAG ONTO A COMPATIBLE WEAPON");}
        if(D.Category==TEXT("Weapon")){
            if(InspectPanel==0&&!HasDrag&&Inspect->Parts39.IsEmpty()){const FName Mounts[]={TEXT("Optic"),TEXT("Light"),TEXT("Laser"),TEXT("Grip")};const FKey Keys[]={EKeys::F1,EKeys::F2,EKeys::F3,EKeys::F4};for(int M=0;M<4;M++)if(PlayerOwner->WasInputKeyJustPressed(Keys[M])){const FGuid Id=Inspect->Id;if(LWMods::Detach(P->Inventory,Id,Mounts[M])){P->ConfigureAttachments();P->PersistWorldChange();}else P->Notify(TEXT("MOUNT EMPTY OR INVENTORY FULL"));return;}}
        }
        Label(FitText(DetailA,408),330,551,.8f,InvBone);
        Label(FitText(DetailB,408),330,567,.8f,InvMuted);
        if(D.Category==TEXT("Weapon"))
        {
            FString Attachments;
            const FName Mounts[]={TEXT("Optic"),TEXT("Light"),TEXT("Laser"),TEXT("Grip")};
            for(int M=0;M<4;++M){const FName A=Inspect->Attachments.FindRef(Mounts[M]);Attachments+=FString::Printf(TEXT("F%d:%s "),M+1,A.IsNone()?TEXT("-"):*A.ToString().Replace(TEXT("att_"),TEXT("")));}
            if(!Inspect->Parts39.IsEmpty())Attachments=FString::Printf(TEXT("CUSTOM BUILD / %d PARTS / EDIT AT WORKBENCH"),Inspect->Parts39.Num());
            Label(FitText(Attachments,408),330,583,.8f,InvMuted);
            FString Finish=FString::Printf(TEXT("DMG +%d%%"),LWMods::Tier(Inspect)*12);
            if(!LWMods::Modifier(Inspect).IsNone())Finish+=TEXT(" / ")+LWMods::Modifier(Inspect).ToString();
            if(!LWMods::SkinName(Inspect).IsEmpty())Finish+=TEXT(" / ")+LWMods::SkinName(Inspect);
            Label(FitText(Finish,408),330,599,.8f,LWMods::TierColor(Inspect->WeaponTier));
        }
        const FString Condition = D.MaxDurability > 0 ? FString::Printf(TEXT("COND %d/%d  "),Inspect->Durability,D.MaxDurability) : FString();
        Label(FitText(Condition+FString::Printf(TEXT("BUY $%d / SELL $%d"),Price(*Inspect),Price(*Inspect)/2),408),330,615,.8f,Trader ? InvAmber : InvBone);
        Label(InspectPanel == 0 ? TEXT("[RMB] USE/EQUIP   [U] UNLOAD   [DEL] DROP") : Trader ? TEXT("DRAG INTO CARRIED ITEMS TO BUY") : TEXT("DRAG INTO CARRIED ITEMS TO USE"),330,632,.8f,InvMuted);
    }
    else
    {
        Label(TEXT("SELECT AN ITEM TO INSPECT"),332,524,1.f,InvMuted);
    }

    if (HasDrag)
    {
        const float GhostCell = Hover.Grid ? Hover.Grid->Cell : 32.f;
        FVector2D GhostOrigin = Mouse-DragOffset*GhostCell;
        FArea GhostClip{18,80,1244,580};
        if (Hover.Grid)
        {
            GhostOrigin = {Hover.Grid->X+DropX*GhostCell,Hover.Grid->Y+(DropY-Hover.Grid->FirstRow)*GhostCell};
            GhostClip = Hover.Grid->Bounds();
        }
        DrawItem(Dragged,P->ItemsFor(DragPanel),GhostOrigin,GhostCell,bDragRotated,GhostClip,false,true,ValidDrop,true);
        if (Combine && Hover.Grid)
        {
            if (const FLWItemInstance* Target = Find(Hover.Grid->Items,Hover.Id))
            {
                for (FIntPoint C : LWItems::Footprint(Target->Definition,Target->bRotated))
                {
                    FArea Visible;
                    const FArea Cell{Hover.Grid->X+(Target->X+C.X)*GhostCell,Hover.Grid->Y+(Target->Y+C.Y-Hover.Grid->FirstRow)*GhostCell,GhostCell,GhostCell};
                    if (Cell.Intersect(Hover.Grid->Bounds(),Visible)) Border(Visible,ValidDrop ? InvGreen : InvRed,2);
                }
            }
        }
        Fill({306,664,944,34},FLinearColor(.025f,.034f,.021f,.98f));
        Label(FitText(DropHint+TEXT("   [R] ROTATE / [RMB] CANCEL"),930,.9f),316,673,.9f,ValidDrop ? InvGreen : InvRed);
    }
    else
    {
        Stroke({24,665},{1256,665},InvEdge);
        Label(TEXT("LMB DRAG   /   RMB USE OR EQUIP   /   U UNLOAD   /   DEL DROP   /   I OR ESC CLOSE"),34,680,.8f,InvMuted);
        if (P->MessageTime > 0)
        {
            Fill({307,664,944,31},FLinearColor(.065f,.070f,.041f,.98f));
            Label(FitText(P->Message.ToUpper(),924,.85f),317,672,.85f,InvAmber);
        }
    }
    if (HasMouse)
    {
        const FLinearColor Cursor = HasDrag ? (ValidDrop ? InvGreen : InvRed) : InvBone;
        Stroke(Mouse+FVector2D(-5,0),Mouse+FVector2D(5,0),Cursor);
        Stroke(Mouse+FVector2D(0,-5),Mouse+FVector2D(0,5),Cursor);
    }
}
