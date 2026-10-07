#include "LWGeography84.h"
#include "LWSiteIdentity.h"
#include "Engine/Texture2D.h"
#include "LWVehicle.h"
#include "LWHUD.h"
#include "LWCharacter.h"
#include "LWGeneration.h"
#include "LWWorld.h"
#include "LWResident.h"
#include "LWWorldObject.h"
#include "CoreGlobals.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Templates/UnrealTemplate.h"

// MapPan is a world-XY offset from the player; MapZoom is virtual pixels per
// world unit. Per-HUD drag state and bounded geometry caches stay private to this UI.
namespace LWMapHUD
{
namespace
{
    const FLinearColor Paper(.020f, .040f, .051f, 1.f);
    const FLinearColor Ink(.013f, .019f, .013f, 1.f);
    const FLinearColor Bone(.87f, .91f, .90f, 1.f);
    const FLinearColor Muted(.39f, .45f, .33f, 1.f);
    const FLinearColor Grid(.07f, .12f, .15f, 1.f);
    const FLinearColor RoadColor(.32f, .45f, .48f, 1.f);
    const FLinearColor Highway(.48f, .37f, .20f, 1.f);
    const FLinearColor Amber(.98f, .59f, .22f, 1.f);
    const FLinearColor Shelter(.40f, .78f, .67f, 1.f);
    const FLinearColor Loss(.89f, .33f, .24f, 1.f);
    // At minimum zoom a 1216 x 482 survey, plus owner-region padding, needs
    // at most 6 x 10 regions. Zooming/panning never generates an unbounded world.
    constexpr float MinZoom = .0003f;
    constexpr float MaxZoom = .04f;
    constexpr float MiniZoom = .014f;

    struct FRect
    {
        double X, Y, W, H;
        double Right() const { return X + W; }
        double Bottom() const { return Y + H; }
        FVector2D Center() const { return FVector2D(X + W * .5, Y + H * .5); }
        bool Contains(FVector2D P, double Margin = 0.0) const
        {
            return P.X >= X + Margin && P.X <= Right() - Margin &&
                P.Y >= Y + Margin && P.Y <= Bottom() - Margin;
        }
    };

    bool Finite(FVector2D P)
    {
        return FMath::IsFinite(P.X) && FMath::IsFinite(P.Y);
    }

    struct FView
    {
        FRect Bounds;
        FVector2D Center;
        double Zoom;

        FVector2D Plot(FVector2D World) const
        {
            const FVector2D Delta = World - Center;
            return Bounds.Center() + FVector2D(Delta.Y, -Delta.X) * Zoom;
        }
        FVector2D Unplot(FVector2D Screen) const
        {
            const FVector2D Delta = (Screen - Bounds.Center()) / Zoom;
            return Center + FVector2D(-Delta.Y, Delta.X);
        }
    };

    // Liang-Barsky clips crossings even when BOTH original endpoints are outside.
    // Inset by half the stroke width so thick roads cannot bleed over the frame.
    bool Clip(FVector2D& A, FVector2D& B, const FRect& R, double Inset = 0.0)
    {
        if (!Finite(A) || !Finite(B) || R.W <= 2 * Inset || R.H <= 2 * Inset)
        {
            return false;
        }
        const FVector2D D = B - A;
        double Enter = 0.0, Exit = 1.0;
        const double P[] = {-D.X, D.X, -D.Y, D.Y};
        const double Q[] = {A.X - R.X - Inset, R.Right() - Inset - A.X,
            A.Y - R.Y - Inset, R.Bottom() - Inset - A.Y};
        for (int32 I = 0; I < 4; ++I)
        {
            if (FMath::Abs(P[I]) < 1.e-12)
            {
                if (Q[I] < 0.0) return false;
                continue;
            }
            const double T = Q[I] / P[I];
            if (P[I] < 0.0) Enter = FMath::Max(Enter, T);
            else Exit = FMath::Min(Exit, T);
            if (Enter > Exit) return false;
        }
        B = A + D * Exit;
        A += D * Enter;
        return true;
    }

    void Stroke(ALWHUD& HUD, const FRect& Bounds, FVector2D A, FVector2D B,
        FLinearColor Color, float Width = 1.f)
    {
        if (Clip(A, B, Bounds, Width * .5))
        {
            HUD.Line(A.X, A.Y, B.X, B.Y, Color, Width);
        }
    }

    void Fill(ALWHUD& HUD, const FRect& Bounds, const FRect& R, FLinearColor Color)
    {
        const double X = FMath::Max(Bounds.X, R.X), Y = FMath::Max(Bounds.Y, R.Y);
        const double Right = FMath::Min(Bounds.Right(), R.Right());
        const double Bottom = FMath::Min(Bounds.Bottom(), R.Bottom());
        if (Right > X && Bottom > Y) HUD.Rect(X, Y, Right - X, Bottom - Y, Color);
    }

    void Border(ALWHUD& HUD, const FRect& R, FLinearColor Color)
    {
        HUD.Line(R.X, R.Y, R.Right(), R.Y, Color);
        HUD.Line(R.Right(), R.Y, R.Right(), R.Bottom(), Color);
        HUD.Line(R.Right(), R.Bottom(), R.X, R.Bottom(), Color);
        HUD.Line(R.X, R.Bottom(), R.X, R.Y, Color);
    }

    FString Distance(double WorldUnits)
    {
        return WorldUnits >= 100000.0
            ? FString::Printf(TEXT("%.1f KM"), WorldUnits / 100000.0)
            : FString::Printf(TEXT("%.0f M"), WorldUnits / 100.0);
    }

    struct FGeometry
    {
        FIntPoint Min = FIntPoint::ZeroValue, Max = FIntPoint::ZeroValue;
        int32 Seed = 0;
        bool Valid = false;
        TArray<LWGen::FRoad> Roads;
        TArray<LWGen::FSite> Sites;

        void Update(const FView& View, int32 InSeed)
        {
            const FVector2D A = View.Unplot(FVector2D(View.Bounds.X, View.Bounds.Y));
            const FVector2D B = View.Unplot(FVector2D(View.Bounds.Right(), View.Bounds.Bottom()));
            constexpr double Limit = double(MAX_int32 / 2 - 32) * LWGen::RegionSize;
            if (!Finite(A) || !Finite(B) || FMath::Abs(A.X) > Limit || FMath::Abs(A.Y) > Limit ||
                FMath::Abs(B.X) > Limit || FMath::Abs(B.Y) > Limit)
            {
                Valid = false; Roads.Reset(); Sites.Reset(); return;
            }
            const FIntPoint NewMin(
                LWGen::FloorDiv(FMath::Min(A.X, B.X) + LWGen::RegionSize * .5, LWGen::RegionSize) - 1,
                LWGen::FloorDiv(FMath::Min(A.Y, B.Y) + LWGen::RegionSize * .5, LWGen::RegionSize) - 1);
            const FIntPoint NewMax(
                LWGen::FloorDiv(FMath::Max(A.X, B.X) + LWGen::RegionSize * .5, LWGen::RegionSize) + 1,
                LWGen::FloorDiv(FMath::Max(A.Y, B.Y) + LWGen::RegionSize * .5, LWGen::RegionSize) + 1);
            if (Valid && Min == NewMin && Max == NewMax && Seed == InSeed) return;
            Min = NewMin; Max = NewMax; Seed = InSeed;
            Roads.Reset(); Sites.Reset();
            const int64 Width = int64(Max.X) - Min.X + 1, Height = int64(Max.Y) - Min.Y + 1;
            Valid = Width > 0 && Height > 0 && Width <= 12 && Height <= 12 && Width * Height <= 64;
            if(View.Zoom<.0035){Valid=true;Roads=LWNY69::Roads();Roads.RemoveAll([](const auto& R){return R.Width<850;});Sites=LWNY69::Sites();TArray<LWGen::FRoad> CR;TArray<LWGen::FSite> CS;LWGen::Gather(LWGen::CanadaCity68(),Seed,CR,CS);Roads.Append(CR);Sites.Append(CS);return;}
            if (!Valid) return;
            Roads.Reserve(int32(Width * Height) * 28);
            Sites.Reserve(int32(Width * Height) * 10);
            for (int32 Y = Min.Y; Y <= Max.Y; ++Y)
            {
                for (int32 X = Min.X; X <= Max.X; ++X)
                {
                    LWGen::Region(FIntPoint(X, Y), Seed, Roads, Sites);
                    if(!LWGen::HasTown(FIntPoint(X,Y),Seed))continue;LWGen::FSite Camp;Camp.Id=LWGen::Hash(X,Y,Seed,26001);Camp.Position=LWGen::Hub(FIntPoint(X,Y),Seed)+FVector2D(0,-3400);Camp.Friendly=true;Sites.Add(Camp);
                }
            }
        }
    };

    struct FHUDState
    {
        FGeometry Survey, Mini;
        FVector2D LastMouse = FVector2D::ZeroVector;
        uint64 LastMapFrame = MAX_uint64, InputFrame = MAX_uint64;
        bool Dragging = false;
    };

    FHUDState& StateFor(ALWHUD& HUD)
    {
        // Weak keys neither retain destroyed HUDs nor share drag/zoom caches across players.
        static TMap<TWeakObjectPtr<ALWHUD>, FHUDState> States;
        for (auto It = States.CreateIterator(); It; ++It)
        {
            if (!It.Key().IsValid()) It.RemoveCurrent();
        }
        return States.FindOrAdd(TWeakObjectPtr<ALWHUD>(&HUD));
    }

    void DrawPaper(ALWHUD& HUD, const FView& View, bool Mini)
    {
        const FRect& R = View.Bounds;
        HUD.Rect(R.X, R.Y, R.W, R.H, Paper);
        for(double Y=R.Y;Y<R.Bottom();Y+=20)for(double X=R.X;X<R.Right();X+=20){if(LWNY69::WaterDepth(View.Unplot(FVector2D(X+10,Y+10)))>0)Fill(HUD,R,{X,Y,20,20},FLinearColor(.035,.10,.13,1));}
        for (int32 I = 0; I < (Mini ? 18 : 90); ++I)
        {
            const uint32 H = LWGen::Hash(I, 17, 4101);
            const double X = R.X + (H % 1000) * R.W / 1000.0;
            const double Y = R.Y + ((H >> 12) % 1000) * R.H / 1000.0;
            Stroke(HUD, R, FVector2D(X, Y), FVector2D(X + 3 + (H % 29), Y + 1),
                FLinearColor(.12f, .14f, .08f, .22f));
        }
        double Step = LWGen::ChunkSize;
        while (Step * View.Zoom < (Mini ? 30.0 : 48.0)) Step *= 2;
        while (Step * View.Zoom > (Mini ? 60.0 : 96.0)) Step *= .5;
        const FVector2D NW = View.Unplot(FVector2D(R.X, R.Y));
        const FVector2D SE = View.Unplot(FVector2D(R.Right(), R.Bottom()));
        double X = FMath::CeilToDouble(SE.X / Step) * Step;
        for (int32 I = 0; I < 64 && X <= NW.X; ++I, X += Step)
        {
            Stroke(HUD, R, View.Plot(FVector2D(X, NW.Y)), View.Plot(FVector2D(X, SE.Y)), Grid);
        }
        double Y = FMath::CeilToDouble(NW.Y / Step) * Step;
        for (int32 I = 0; I < 64 && Y <= SE.Y; ++I, Y += Step)
        {
            Stroke(HUD, R, View.Plot(FVector2D(NW.X, Y)), View.Plot(FVector2D(SE.X, Y)), Grid);
        }
    }

    void DrawRoads(ALWHUD& HUD, const FView& View, const FGeometry& Geometry, bool Mini)
    {
        for (int32 Pass = 0; Pass < 2; ++Pass)
        {
            for (const LWGen::FRoad& Road : Geometry.Roads)
            {
                const float Width = FMath::Clamp(float(Road.Width * View.Zoom), Mini ? .8f : 1.f, Mini ? 4.f : 10.f);
                Stroke(HUD, View.Bounds, View.Plot(Road.A), View.Plot(Road.B),
                    Pass == 0 ? Ink : Road.Highway ? Highway : RoadColor, Width + (Pass == 0 ? 2.f : 0.f));
            }
        }
    }

    void DrawSites(ALWHUD& HUD, const FView& View, const FGeometry& Geometry,
        bool Mini, FVector2D Mouse, bool HasMouse, FString& Hover, const ALWCharacter& Player)
    {
        if(!HUD.POIAtlas)HUD.POIAtlas=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Art/Textures/T_POIAtlas30.T_POIAtlas30"));
        for (const LWGen::FSite& Site : Geometry.Sites)
        {
            if(Site.SettlementBuilding||!Player.RPG.KnownPlaces.Contains(int64(Site.Id)))continue;
            const FVector2D C=View.Plot(Site.Position);
            const float IconSize=Mini?18.f:30.f;
            if(!View.Bounds.Contains(C,IconSize*.5f+1))continue;
            const FLinearColor Color=Site.Friendly?Shelter:Site.Type==21?Amber:Bone;
            if(!Mini&&View.Zoom>=.009){
                FVector2D Corners[4];const FVector2D Signs[]={FVector2D(-1,-1),FVector2D(1,-1),FVector2D(1,1),FVector2D(-1,1)};
                for(int I=0;I<4;I++)Corners[I]=View.Plot(Site.Position+(Signs[I]*Site.Size*.5).GetRotated(Site.Yaw));
                for(int I=0;I<4;I++)Stroke(HUD,View.Bounds,Corners[I],Corners[(I+1)%4],Muted*.6f);
            }
            Fill(HUD,View.Bounds,{C.X-IconSize*.5,C.Y-IconSize*.5,IconSize,IconSize},Ink);
            const int IconType=Site.Type==68?15:Site.Type==69?58:Site.Type>=70?(Site.Type==74||Site.Type==76?15:Site.Type==72?5:Site.Type==78?16:60):Site.Type;
            if(HUD.POIAtlas&&IconType>=0&&IconType<64){
                const float U=(IconType%8)/8.f,V=(IconType/8)/8.f;
                HUD.DrawTexture(HUD.POIAtlas,(C.X-IconSize*.5)*HUD.Scale,(C.Y-IconSize*.5)*HUD.Scale,IconSize*HUD.Scale,IconSize*HUD.Scale,U+.001f,V+.001f,.123f,.123f,Color,BLEND_Translucent);
            }else Fill(HUD,View.Bounds,{C.X-3,C.Y-3,6,6},Color);
            const FString Label=LWSites::Label(Site);
            if(!Mini&&View.Zoom>=.022&&View.Bounds.Contains(C+FVector2D(16+Label.Len()*5,12)))HUD.Text(Label,C.X+18,C.Y-5,.65f,Color);
            if(HasMouse&&(Mouse-C).SizeSquared()<FMath::Square(IconSize*.6f))Hover=LWSites::Name(Site);
        }
    }

    enum class EMarker { Trader, Bunker, Bag, Waypoint, Vehicle };

    FLinearColor MarkerColor(EMarker Kind)
    {
        return Kind == EMarker::Bunker ? Shelter : Kind == EMarker::Bag ? Loss : Amber;
    }

    FVector2D PinToEdge(const FView& View, FVector2D Screen, bool& Outside)
    {
        constexpr double Margin = 12.0;
        Outside = !View.Bounds.Contains(Screen, Margin);
        if (!Outside) return Screen;
        const FVector2D D = Screen - View.Bounds.Center();
        double T = 1.0;
        if (FMath::Abs(D.X) > 1.e-9) T = FMath::Min(T, (View.Bounds.W * .5 - Margin) / FMath::Abs(D.X));
        if (FMath::Abs(D.Y) > 1.e-9) T = FMath::Min(T, (View.Bounds.H * .5 - Margin) / FMath::Abs(D.Y));
        return View.Bounds.Center() + D * T;
    }

    void Glyph(ALWHUD& HUD, const FRect& Bounds, FVector2D C, EMarker Kind, bool Mini)
    {
        const FLinearColor Color = MarkerColor(Kind);
        // A waypoint can be the bunker or the death bag: its larger, hollow ring
        // preserves the destination's own icon instead of painting over it.
        const double R = Kind == EMarker::Waypoint ? (Mini ? 7.0 : 10.0) : (Mini ? 4.0 : 6.0);
        if (Kind != EMarker::Waypoint)
        {
            Fill(HUD, Bounds, {C.X - R - 2, C.Y - R - 2, R * 2 + 4, R * 2 + 4}, Ink);
        }
        auto Edge = [&](double X, double Y, double X2, double Y2)
        {
            Stroke(HUD, Bounds, C + FVector2D(X, Y), C + FVector2D(X2, Y2), Color, Mini ? 1.3f : 1.8f);
        };
        if(Kind==EMarker::Vehicle){
            Edge(-R,-R*.5,R,-R*.5);Edge(R,-R*.5,R,R*.5);Edge(R,R*.5,-R,R*.5);Edge(-R,R*.5,-R,-R*.5);
            Edge(-R*.5,-R*.5,-R*.25,-R);Edge(-R*.25,-R,R*.5,-R);Edge(R*.5,-R,R*.8,-R*.5);
            Edge(-R*.6,R*.5,-R*.6,R);Edge(R*.6,R*.5,R*.6,R);
        }
        else if (Kind == EMarker::Bunker)
        {
            Edge(-R, 0, 0, -R); Edge(0, -R, R, 0);
            Edge(-R + 1, 0, -R + 1, R); Edge(-R + 1, R, R - 1, R); Edge(R - 1, R, R - 1, 0);
            Edge(0, R, 0, 1);
        }
        else if (Kind == EMarker::Bag)
        {
            Edge(-R, -R + 2, R, -R + 2); Edge(R, -R + 2, R, R); Edge(R, R, -R, R); Edge(-R, R, -R, -R + 2);
            Edge(-2, -R + 2, -2, -R); Edge(-2, -R, 2, -R); Edge(2, -R, 2, -R + 2);
        }
        else
        {
            Edge(0, -R, R, 0); Edge(R, 0, 0, R); Edge(0, R, -R, 0); Edge(-R, 0, 0, -R);
            if (Kind == EMarker::Trader)
            {
                Edge(-2, -2, 2, -2); Edge(-2, -2, -2, 0); Edge(-2, 0, 2, 0); Edge(2, 0, 2, 2); Edge(2, 2, -2, 2);
            }
        }
    }

    void DrawMarker(ALWHUD& HUD, const FView& View, FVector2D Position, EMarker Kind,
        bool Mini, bool EdgeIndicator, FVector2D Mouse, bool HasMouse, const FString& Label, FString& Hover)
    {
        if (!Finite(Position)) return;
        const FVector2D Actual = View.Plot(Position);
        if (!EdgeIndicator && !View.Bounds.Contains(Actual, 10.0)) return;
        bool Outside = false;
        const FVector2D C = PinToEdge(View, Actual, Outside);
        Glyph(HUD, View.Bounds, C, Kind, Mini);
        if (Outside)
        {
            const FVector2D D = (Actual - View.Bounds.Center()).GetSafeNormal();
            const FVector2D Side(-D.Y, D.X), Tip = C + D * 10;
            Stroke(HUD, View.Bounds, Tip, Tip - D * 4 + Side * 3, MarkerColor(Kind), 1.5f);
            Stroke(HUD, View.Bounds, Tip, Tip - D * 4 - Side * 3, MarkerColor(Kind), 1.5f);
        }
        if (HasMouse && (Mouse - C).SizeSquared() < 144.0)
        {
            Hover = Label + (Outside ? TEXT(" / OFF MAP") : TEXT(""));
        }
    }

    void DrawContacts(ALWHUD& HUD, const FView& View, ALWCharacter& P, bool Mini,
        FVector2D Mouse, bool HasMouse, FString& Hover)
    {
        for(const auto& E:P.World->Encounters.Records)if(E.Value.Seen&&E.Value.Stage<2){const auto* D=ULWEncounterCatalog::Get()->Find(E.Value.Type);DrawMarker(HUD,View,FVector2D(E.Value.Position),EMarker::Waypoint,Mini,false,Mouse,HasMouse,D?D->Title:TEXT("ENCOUNTER"),Hover);}
        const FName LastVehicle=ALWVehicle::LastDrivenId(P.World);
        if(const auto* Record=P.World->Vehicles.Find(LastVehicle)){
            FVector Position=Record->Position;
            for(TActorIterator<ALWVehicle> V(P.GetWorld());V;++V)if(V->RecordId==LastVehicle){Position=V->GetActorLocation();break;}
            const FString Label=FString(Record->Exploded?TEXT("LAST VEHICLE (WRECK) / "):TEXT("LAST DRIVEN / "))+LWTraffic::Get(Record->Model).Name;
            DrawMarker(HUD,View,FVector2D(Position),EMarker::Vehicle,Mini,true,Mouse,HasMouse,Label,Hover);
        }
        for(const auto& Pair:P.World->Vehicles)if(Pair.Value.Owned45&&Pair.Key!=LastVehicle)DrawMarker(HUD,View,FVector2D(Pair.Value.Position),EMarker::Vehicle,Mini,true,Mouse,HasMouse,FString(Pair.Value.Stored45?TEXT("GARAGE / "):TEXT("OWNED / "))+LWTraffic::Get(Pair.Value.Model).Name,Hover);
        for(const auto& C:P.RPG.Claims82)DrawMarker(HUD,View,FVector2D(C.Value.Center),EMarker::Bunker,Mini,true,Mouse,HasMouse,C.Value.Name,Hover);
        TSet<FName> LiveTraders;
        FVector2D BagPosition = FVector2D::ZeroVector;
        bool HasBag = false;
        if (!P.LastDeathBag.IsNone())
        {
            if (const FLWContainerRecord* Record = P.World->Containers.Find(P.LastDeathBag))
            {
                HasBag = Record->bDropped;
                BagPosition = FVector2D(Record->Position);
            }
        }
        // Moving actors supersede saved spawn positions. Records still show known
        // exchanges and the last death bag when their chunks have streamed out.
        for (TActorIterator<ALWWorldObject> It(P.World->GetWorld()); It; ++It)
        {
            ALWWorldObject* Object = *It;
            if (!IsValid(Object) || Object->World != P.World) continue;
            if (Object->Kind == ELWObjectKind::Trader)
            {
                LiveTraders.Add(Object->RecordId);if(FVector::Dist2D(Object->GetActorLocation(),P.GetActorLocation())>900&&!P.RPG.Discoveries.Contains(Object->RecordId))continue;
                DrawMarker(HUD, View, FVector2D(Object->GetActorLocation()), EMarker::Trader,
                    Mini, false, Mouse, HasMouse, TEXT("WANDERING EXCHANGE / LIVE"), Hover);
            }
            else if (!P.LastDeathBag.IsNone() && Object->RecordId == P.LastDeathBag)
            {
                HasBag = true;
                BagPosition = FVector2D(Object->GetActorLocation());
            }
        }
        for (const auto& Pair : P.World->Containers)
        {
            if (Pair.Value.bTrader && P.RPG.Discoveries.Contains(Pair.Key) && !LiveTraders.Contains(Pair.Key))
            {
                DrawMarker(HUD, View, FVector2D(Pair.Value.Position), EMarker::Trader,
                    Mini, false, Mouse, HasMouse, TEXT("EXCHANGE / LAST KNOWN POSITION"), Hover);
            }
        }
        DrawMarker(HUD, View, FVector2D(ALWWorld::BunkerDoorPosition()), EMarker::Bunker,
            Mini, true, Mouse, HasMouse, TEXT("SHELTER 01 / SAFEHOUSE"), Hover);
        if (HasBag)
        {
            DrawMarker(HUD, View, BagPosition, EMarker::Bag, Mini, true,
                Mouse, HasMouse, TEXT("LAST GEAR BAG"), Hover);
        }
        if (P.bWaypoint)
        {
            DrawMarker(HUD, View, P.Waypoint, EMarker::Waypoint, Mini, true,
                Mouse, HasMouse, TEXT("ACTIVE WAYPOINT"), Hover);
        }
    }

    void DrawRoute(ALWHUD& HUD, const FView& View, const ALWCharacter& P, bool Mini)
    {
        if (!P.bWaypoint) return;
        for (int32 Pass = 0; Pass < 2; ++Pass)
        {
            for (int32 I = 1; I < P.Route.Num(); ++I)
            {
                Stroke(HUD, View.Bounds, View.Plot(P.Route[I - 1]), View.Plot(P.Route[I]),
                    Pass == 0 ? Ink : Amber, Pass == 0 ? (Mini ? 4.f : 6.f) : (Mini ? 1.8f : 2.8f));
            }
        }
        // A cropped route stops at its real intermediate hub. Never draw a false
        // straight connector from that hub to the remote final waypoint.
        if (!P.bRouteComplete && P.Route.Num() > 1)
        {
            const FVector2D End = View.Plot(P.Route.Last());
            if (View.Bounds.Contains(End, 7.0))
            {
                int32 Previous = P.Route.Num() - 2;
                while (Previous > 0 && (P.Route.Last() - P.Route[Previous]).SizeSquared() < 1.0) --Previous;
                const FVector2D D = (End - View.Plot(P.Route[Previous])).GetSafeNormal();
                const FVector2D Side(-D.Y, D.X);
                for (int32 I = 0; I < 2; ++I)
                {
                    const FVector2D C = End - D * I * 5;
                    Stroke(HUD, View.Bounds, C - D * 4 + Side * 3, C, Amber, 1.5f);
                    Stroke(HUD, View.Bounds, C - D * 4 - Side * 3, C, Amber, 1.5f);
                }
            }
        }
    }

    void DrawPlayer(ALWHUD& HUD, const FView& View, const ALWCharacter& P, bool Mini)
    {
        bool Outside = false;
        const FVector2D C = PinToEdge(View, View.Plot(FVector2D(P.GetActorLocation())), Outside);
        const FVector Forward = FRotator(0, P.GetControlRotation().Yaw, 0).Vector();
        const FVector2D D(Forward.Y, -Forward.X), Side(-D.Y, D.X);
        const double Size = Mini ? 6.0 : 9.0;
        const FVector2D Points[] = {C + D * Size, C - D * Size * .6 + Side * Size * .65,
            C - D * Size * .3, C - D * Size * .6 - Side * Size * .65};
        for (int32 Pass = 0; Pass < 2; ++Pass)
        {
            for (int32 I = 0; I < 4; ++I)
            {
                Stroke(HUD, View.Bounds, Points[I], Points[(I + 1) % 4],
                    Pass == 0 ? Ink : Bone, Pass == 0 ? 4.f : 1.8f);
            }
        }
        if (Outside && !Mini)
        {
            const double X = FMath::Clamp(C.X - 11.0, View.Bounds.X + 3, View.Bounds.Right() - 27);
            const double Y = FMath::Clamp(C.Y + 12.0, View.Bounds.Y + 3, View.Bounds.Bottom() - 18);
            HUD.Text(TEXT("YOU"), X, Y, .8f, Bone);
        }
    }

    void DrawScale(ALWHUD& HUD, const FView& View, bool Mini)
    {
        const double Target = (Mini ? 54.0 : 100.0) / View.Zoom;
        double Units = 100.0;
        while (Units * 10 <= Target) Units *= 10;
        if (Units * 5 <= Target) Units *= 5;
        else if (Units * 2 <= Target) Units *= 2;
        const double Pixels = Units * View.Zoom;
        const double X = View.Bounds.X + 10, Y = View.Bounds.Bottom() - 10;
        Fill(HUD, View.Bounds, {X - 4, Y - 22, FMath::Max(Pixels + 8, Mini ? 60.0 : 110.0), 28}, Ink);
        HUD.Text(Distance(Units), X, Y - 22, .8f, Muted);
        Stroke(HUD, View.Bounds, FVector2D(X, Y), FVector2D(X + Pixels, Y), Muted);
        Stroke(HUD, View.Bounds, FVector2D(X, Y - 3), FVector2D(X, Y + 3), Muted);
        Stroke(HUD, View.Bounds, FVector2D(X + Pixels, Y - 3), FVector2D(X + Pixels, Y + 3), Muted);
    }

    void DrawCompass(ALWHUD& HUD, const FView& View, bool Mini)
    {
        const FVector2D C(View.Bounds.Right() - (Mini ? 14 : 28), View.Bounds.Y + (Mini ? 29 : 37));
        Fill(HUD, View.Bounds, {C.X - 10, C.Y - 27, Mini ? 20.0 : 35.0, 39}, Ink);
        HUD.Text(TEXT("N"), C.X - 3.6, C.Y - 27, .8f, Bone);
        Stroke(HUD, View.Bounds, C + FVector2D(0, 7), C - FVector2D(0, 10), Bone, 1.5f);
        Stroke(HUD, View.Bounds, C - FVector2D(0, 10), C + FVector2D(-3, -5), Bone, 1.5f);
        Stroke(HUD, View.Bounds, C - FVector2D(0, 10), C + FVector2D(3, -5), Bone, 1.5f);
        if (!Mini)
        {
            Stroke(HUD, View.Bounds, C, C + FVector2D(12, 0), Muted);
            HUD.Text(TEXT("E"), C.X + 14, C.Y - 7, .8f, Muted);
        }
    }

    FString Paint(ALWHUD& HUD, const FView& View, FGeometry& Geometry, ALWCharacter& P,
        bool Mini, FVector2D Mouse = FVector2D::ZeroVector, bool HasMouse = false)
    {
        Geometry.Update(View, P.World->Seed);
        FString Hover;
        DrawPaper(HUD, View, Mini);
        DrawRoads(HUD, View, Geometry, Mini);
        DrawSites(HUD, View, Geometry, Mini, Mouse, HasMouse, Hover, P);
        if(!Mini){auto Q=View.Plot(LWGen::CanadaCity68());if(View.Bounds.Contains(Q,24))HUD.Text(TEXT("TORONTO"),Q.X-40,Q.Y-22,.7f,Bone);Q=View.Plot(LWNY69::Project(43.75,-77.85));if(View.Bounds.Contains(Q,95))HUD.Text(TEXT("LAKE ONTARIO"),Q.X-60,Q.Y,.7f,Bone);}
        if(!Mini)for(const auto& T:LWNY69::Towns()){const auto Q=View.Plot(LWNY69::Project(T.Latitude,T.Longitude));if(View.Bounds.Contains(Q,95)&&(View.Zoom>.001||T.Blocks>=3))HUD.Text(T.Name,Q.X-45,Q.Y-22,.7f,Bone);}
        for(const auto& Segment:LWGeography84::Border())Stroke(HUD,View.Bounds,View.Plot(Segment.A),View.Plot(Segment.B),FLinearColor(.8f,.22f,.18f),Mini?1.f:2.f);
        for(int StoryIndex=0;StoryIndex<9;StoryIndex++){int64 Id=0xEF320000u+StoryIndex;if(!P.RPG.KnownPlaces.Contains(Id))continue;FVector2D Point=View.Plot(LWStory::Site(StoryIndex));if(!View.Bounds.Contains(Point,10))continue;HUD.Rect(Point.X-5,Point.Y-5,10,10,Amber);if(!Mini&&HasMouse&&(Point-Mouse).Size()<20)Hover=LWStory::SiteName(StoryIndex);}
        DrawRoute(HUD, View, P, Mini);
        DrawScale(HUD, View, Mini);
        DrawCompass(HUD, View, Mini);
        DrawContacts(HUD, View, P, Mini, Mouse, HasMouse, Hover);
        for(TActorIterator<ALWResident> It(P.GetWorld());It;++It)if(P.RPG.Crew.ContainsByPredicate([&](const auto& C){return C.Following&&C.Id==It->ResidentId;})){
 bool Outside=false;const FVector2D At=PinToEdge(View,View.Plot(FVector2D(It->GetActorLocation())),Outside);const FLinearColor CrewColor(.25f,.85f,.85f);
 Stroke(HUD,View.Bounds,At-FVector2D(4,0),At+FVector2D(4,0),CrewColor,2);Stroke(HUD,View.Bounds,At-FVector2D(0,4),At+FVector2D(0,4),CrewColor,2);
 if(!Mini&&View.Bounds.Contains(At+FVector2D(140,15)))HUD.Text(It->DisplayName,At.X+8,At.Y-7,.7f,CrewColor);
 }
 DrawPlayer(HUD, View, P, Mini);
        Border(HUD, View.Bounds, Muted);
        return Hover;
    }

    void Control(ALWHUD& HUD, const FRect& R, const TCHAR* Label, FVector2D Mouse,
        bool HasMouse, bool Enabled = true)
    {
        HUD.Control55(FName(Label),Label,R.X,R.Y,R.W,R.H,false,false);
        if(!Enabled)HUD.Rect(R.X,R.Y,R.W,R.H,FLinearColor(0,0,0,.35f));

    }

    FString RouteStatus(const ALWCharacter& P)
    {
        if (!P.bWaypoint) return TEXT("NO DESTINATION // MARK A ROAD OR A PLACE TO GO");
        const FString Target = TEXT("WAYPOINT ") + Distance((P.Waypoint - FVector2D(P.GetActorLocation())).Size());
        if (P.Route.Num() < 2)
        {
            return Target + (P.bSafehouse ? TEXT(" // ROUTE RESUMES OUTSIDE") : TEXT(" // AWAITING ROUTE"));
        }
        double Length = 0;
        for (int32 I = 1; I < P.Route.Num(); ++I) Length += (P.Route[I] - P.Route[I - 1]).Size();
        return Target + (P.bRouteComplete ? TEXT(" // ROUTE ") : TEXT(" // NEXT LEG ")) + Distance(Length)
            + (P.bRouteComplete ? TEXT("") : TEXT(" // CONTINUES"));
    }
}
}

void ALWHUD::Map(ALWCharacter* P)
{
    if (!Canvas || !PlayerOwner || !IsValid(P) || !IsValid(P->World) || !P->bMap || P->bMenu || P->Health <= 0) return;
    if (Canvas->ClipX <= 0 || Canvas->ClipY <= 0) return;
    // Fit the 1280x720 survey even on narrower displays; restore DrawHUD's scale on exit.
    const TGuardValue<float> SurveyScale(Scale, FMath::Min(Canvas->ClipX / 1280.f, Canvas->ClipY / 720.f));
    const double Width = Canvas->ClipX / Scale, Height = Canvas->ClipY / Scale;
    const double Left = 0, Top = 0;
    const LWMapHUD::FRect Area{Left + 32, Top + 108, Width - 64, 482};
    const LWMapHUD::FRect BunkerButton{Left + 32, Top + 62, 264, 34};
    const LWMapHUD::FRect CenterButton{Left + 308, Top + 62, 176, 34};
    const LWMapHUD::FRect GoalButton{Left + 496, Top + 62, 196, 34};
    const LWMapHUD::FRect CasinoButton{Left + 704, Top + 62, 210, 34};
    const LWMapHUD::FRect MinusButton{Width - 184, Top + 62, 54, 34};
    const LWMapHUD::FRect PlusButton{Width - 118, Top + 62, 86, 34};
    const FVector2D Player(P->GetActorLocation());
    MapZoom = FMath::IsFinite(MapZoom) ? FMath::Clamp(MapZoom, LWMapHUD::MinZoom, LWMapHUD::MaxZoom) : .005f;
    if (!LWMapHUD::Finite(MapPan)) MapPan = FVector2D::ZeroVector;
    LWMapHUD::FView View{Area, Player + MapPan, MapZoom};
    float MX = 0, MY = 0;
    const bool HasMouse = Mouse55(MX, MY);
    const FVector2D Mouse(MX / Scale, MY / Scale);
    const bool OverMap = HasMouse && Area.Contains(Mouse);
    LWMapHUD::FHUDState& State = LWMapHUD::StateFor(*this);
    if (State.LastMapFrame == MAX_uint64 || (State.LastMapFrame != GFrameCounter && State.LastMapFrame + 1 != GFrameCounter))
    {
        State.Dragging = false;
    }
    State.LastMapFrame = GFrameCounter;

    if (State.InputFrame != GFrameCounter)
    {
        State.InputFrame = GFrameCounter;
        const bool LeftClick = HasMouse && PlayerOwner->WasInputKeyJustPressed(EKeys::LeftMouseButton);
        auto Clicked = [&](const LWMapHUD::FRect& R) { return LeftClick && R.Contains(Mouse); };
        if (!HasMouse || !PlayerOwner->IsInputKeyDown(EKeys::MiddleMouseButton)) State.Dragging = false;
        if (OverMap && PlayerOwner->WasInputKeyJustPressed(EKeys::MiddleMouseButton))
        {
            State.Dragging = true;
            State.LastMouse = Mouse;
        }
        if (State.Dragging)
        {
            const FVector2D Delta = Mouse - State.LastMouse;
            MapPan += FVector2D(Delta.Y, -Delta.X) / MapZoom;
        }
        State.LastMouse = Mouse;
        if (Clicked(CenterButton) || PlayerOwner->WasInputKeyJustPressed(EKeys::Home)) MapPan = FVector2D::ZeroVector;
        if (P->bWaypoint && (Clicked(GoalButton) || PlayerOwner->WasInputKeyJustPressed(EKeys::End))) MapPan = P->Waypoint - Player;

        const FVector2D Bunker(ALWWorld::BunkerDoorPosition());
        const bool BPressed = PlayerOwner->WasInputKeyJustPressed(EKeys::B);
        // The character may already have handled B this frame. Avoid clearing its
        // newly cached route a second time; the on-screen button always works alone.
        if (Clicked(BunkerButton) || (BPressed && (!P->bWaypoint || !P->Waypoint.Equals(Bunker, .1)))) P->RouteToBunker();
        if(Clicked(CasinoButton)){P->FastTravel();return;}
        if (OverMap && PlayerOwner->WasInputKeyJustPressed(EKeys::RightMouseButton)) P->ClearWaypoint();

        double Steps = 0;
        if (OverMap)
        {
            Steps = PlayerOwner->GetInputAnalogKeyState(EKeys::MouseWheelAxis);
            if (FMath::Abs(Steps) < .01)
            {
                if (PlayerOwner->WasInputKeyJustPressed(EKeys::MouseScrollUp)) Steps += 1;
                if (PlayerOwner->WasInputKeyJustPressed(EKeys::MouseScrollDown)) Steps -= 1;
            }
        }
        if (Clicked(PlusButton) || PlayerOwner->WasInputKeyJustPressed(EKeys::Add) || PlayerOwner->WasInputKeyJustPressed(EKeys::Equals)) Steps += 1;
        if (Clicked(MinusButton) || PlayerOwner->WasInputKeyJustPressed(EKeys::Subtract) || PlayerOwner->WasInputKeyJustPressed(EKeys::Hyphen)) Steps -= 1;
        View.Center = Player + MapPan;
        if (Steps != 0)
        {
            const FVector2D Anchor = OverMap ? Mouse : Area.Center();
            const FVector2D Before = View.Unplot(Anchor);
            MapZoom = FMath::Clamp(float(MapZoom * FMath::Pow(1.25, FMath::Clamp(Steps, -4.0, 4.0))), LWMapHUD::MinZoom, LWMapHUD::MaxZoom);
            View.Zoom = MapZoom;
            MapPan += Before - View.Unplot(Anchor);
            View.Center = Player + MapPan;
        }
        if (OverMap && LeftClick && !State.Dragging){
            FVector2D Goal=View.Unplot(Mouse);TArray<LWGen::FRoad> R;TArray<LWGen::FSite> S;LWGen::Gather(Goal,P->World->Seed,R,S);
            for(int StoryIndex=0;StoryIndex<9;StoryIndex++)if(P->RPG.KnownPlaces.Contains(int64(0xEF320000u+StoryIndex))&&(View.Plot(LWStory::Site(StoryIndex))-Mouse).Size()<18)Goal=FVector2D(P->RPG.KnownPlaces[int64(0xEF320000u+StoryIndex)]);
            for(const auto& Site:S)if(P->RPG.KnownPlaces.Contains(int64(Site.Id))&&(View.Plot(Site.Position)-Mouse).Size()<18){Goal=FVector2D(P->RPG.KnownPlaces[int64(Site.Id)]);break;}
            P->SetWaypoint(Goal);
        }
    }
    View.Center = Player + MapPan;
    View.Zoom = MapZoom;

    Rect(0, 0, Width, Height, LWMapHUD::Ink);
    Rect(Left + 16, Top + 14, Width - 32, 692, FLinearColor(.030f, .038f, .025f));
    Text(TEXT("MAP"), Left + 32, Top + 21, 1.65f, LWMapHUD::Bone);
    Text(TEXT(""), Left + 790, Top + 29, 1.f, LWMapHUD::Muted);
    Line(Left + 32, Top + 52, Width - 32, Top + 52, LWMapHUD::Muted);
    LWMapHUD::Control(*this, BunkerButton, TEXT("[B] ROUTE TO BUNKER"), Mouse, HasMouse);
    LWMapHUD::Control(*this, CenterButton, TEXT("[HOME] CENTER"), Mouse, HasMouse);
    LWMapHUD::Control(*this, GoalButton, TEXT("[END] WAYPOINT"), Mouse, HasMouse, P->bWaypoint);
    LWMapHUD::Control(*this, CasinoButton, TEXT("FAST TRAVEL"), Mouse, HasMouse);
    LWMapHUD::Control(*this, MinusButton, TEXT(" - "), Mouse, HasMouse);
    LWMapHUD::Control(*this, PlusButton, TEXT(" + ZOOM"), Mouse, HasMouse);
    Text(FString::Printf(TEXT("ZOOM %.1fX"), MapZoom / .005f), Width - 344, Top + 72, .9f, LWMapHUD::Muted);

    const FString Hover = LWMapHUD::Paint(*this, View, State.Survey, *P, false, Mouse, OverMap);
    Text(LWMapHUD::RouteStatus(*P), Left + 36, Top + 602, .9f, P->bWaypoint ? LWMapHUD::Amber : LWMapHUD::Muted);
    const FVector2D Readout = OverMap ? View.Unplot(Mouse) : View.Center;
    const FString Coordinates = FString::Printf(TEXT("N %+.0f M  /  E %+.0f M"), Readout.X / 100.0, Readout.Y / 100.0);
    Text(Hover.IsEmpty() ? TEXT("") : Hover, Left + 36, Top + 622, .8f, LWMapHUD::Bone);
    Text(Coordinates, Width - 38 - Coordinates.Len() * 7.2, Top + 622, .8f, LWMapHUD::Muted);
    Line(Left + 32, Top + 646, Width - 32, Top + 646, LWMapHUD::Muted * .5f);
    const LWMapHUD::FRect Legend{Left + 32, Top + 648, Width - 64, 28};
    Rect(Left + 43, Top + 657, 5, 5, LWMapHUD::Muted); Text(TEXT("SITE"), Left + 58, Top + 652, .85f, LWMapHUD::Muted);
    Text(TEXT("+ RELIEF"), Left + 186, Top + 652, .85f, LWMapHUD::Bone);
    LWMapHUD::Glyph(*this, Legend, FVector2D(Left + 359, Top + 662), LWMapHUD::EMarker::Trader, false);
    Text(TEXT("EXCHANGE"), Left + 375, Top + 652, .85f, LWMapHUD::Amber);
    LWMapHUD::Glyph(*this, Legend, FVector2D(Left + 555, Top + 662), LWMapHUD::EMarker::Bunker, false);
    Text(TEXT("SAFEHOUSE"), Left + 571, Top + 652, .85f, LWMapHUD::Shelter);
    LWMapHUD::Glyph(*this, Legend, FVector2D(Left + 771, Top + 662), LWMapHUD::EMarker::Bag, false);
    Text(TEXT("GEAR BAG"), Left + 787, Top + 652, .85f, LWMapHUD::Loss);
    LWMapHUD::Glyph(*this, Legend, FVector2D(Left + 981, Top + 662), LWMapHUD::EMarker::Waypoint, false);
    Text(TEXT("WAYPOINT"), Left + 997, Top + 652, .85f, LWMapHUD::Amber);
    Text(TEXT("LMB WAYPOINT   RMB CLEAR   MMB PAN   WHEEL / +/- ZOOM   [TAB] CLOSE"), Left + 36, Top + 686, .85f, LWMapHUD::Muted);
}

void ALWHUD::MiniMap(ALWCharacter* P)
{
    if (!Canvas || !IsValid(P) || !IsValid(P->World) || !P->bStarted || P->bMenu ||
        P->bMap || P->bInventory || P->Health <= 0 || Scale <= 0 || Canvas->ClipX <= 0) return;
    const double Width = Canvas->ClipX / Scale;
    const LWMapHUD::FRect Panel{Width - 194, 58, 174, 154};
    const LWMapHUD::FView View{{Panel.X + 6, Panel.Y + 26, 162, 100}, FVector2D(P->GetActorLocation()), LWMapHUD::MiniZoom};
    LWMapHUD::FHUDState& State = LWMapHUD::StateFor(*this);
    State.Dragging = false;
    Rect(Panel.X, Panel.Y, Panel.W, Panel.H, LWMapHUD::Ink);
    LWMapHUD::Border(*this, Panel, LWMapHUD::Muted * .65f);
    Text(TEXT("N"), Panel.X + 8, Panel.Y + 5, .8f, LWMapHUD::Bone);
    Text(TEXT("[TAB]"), Panel.Right() - 44, Panel.Y + 5, .8f, LWMapHUD::Muted);
    LWMapHUD::Paint(*this, View, State.Mini, *P, true);
    FString Status = P->bSafehouse ? TEXT("SHELTER 01 / SECURE") : TEXT("[B] ROUTE TO BUNKER");
    if (P->bWaypoint)
    {
        Status = TEXT("GOAL ") + LWMapHUD::Distance((P->Waypoint - FVector2D(P->GetActorLocation())).Size());
        if (!P->bRouteComplete && P->Route.Num() > 1) Status += TEXT(" >>");
    }
    Text(Status, Panel.X + 8, Panel.Y + 133, .8f, P->bWaypoint ? LWMapHUD::Amber : LWMapHUD::Muted);
}
