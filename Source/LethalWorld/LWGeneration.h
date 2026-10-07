#pragma once
#include "CoreMinimal.h"
#include "LWBorder51.h"
#include "LWGeography84.h"
#include "LWAircraft84.h"
#include "Math/RandomStream.h"
#include "LWPOITypes.h"
#include "LWStoryState.h"
#include "LWNewYork69.h"

// Global generation is pure: same seed + coordinate yields the same graph, regardless of load order.
namespace LWGen
{
    inline thread_local int32 TownDensity=43,ParcelLevel=1;
    inline thread_local float Ruggedness=1;
    constexpr double ChunkSize = 12800.0;
    constexpr double RegionSize = 51200.0;
    inline int32 FloorDiv(double Value, double Divisor) { return FMath::FloorToInt(Value / Divisor); }
    inline FIntPoint ChunkAt(FVector2D P) { return FIntPoint(FloorDiv(P.X, ChunkSize), FloorDiv(P.Y, ChunkSize)); }
    inline uint32 Hash(int32 X, int32 Y, int32 Seed, uint32 Salt=0)
    {
        uint32 H = uint32(X)*0x8da6b343u ^ uint32(Y)*0xd8163841u ^ uint32(Seed)*0xcb1ab31fu ^ Salt;
        H ^= H >> 16; H *= 0x7feb352du; H ^= H >> 15; H *= 0x846ca68bu; return H ^ (H >> 16);
    }
    struct FRoad { FVector2D A, B; float Width=780; bool Highway=false; };
    struct FSite
    {
        FVector2D Position; float Yaw=0; int32 Type=0; uint32 Id=0;
        FVector2D Size=FVector2D(1400,1000);
        FString PlaceName69;
        bool Friendly=false;
        bool Canadian=false;
        bool SettlementBuilding=false;
        int32 District=0; // 0 countryside, 1 residential, 2 commercial, 3 industrial, 4 downtown
        int32 Floors=1, AccessibleFloors=1;
    };
    LETHALWORLD_API FVector2D CanadaCity68();
    LETHALWORLD_API double CanadaRoadY68(double X,double Y);
    LETHALWORLD_API void CanadaRegion68(FIntPoint R,int Seed,TArray<FRoad>& Roads,TArray<FSite>& Sites);
    inline FVector2D Hub(FIntPoint R, int32 Seed)
    {
        return LWNY69::RoadHub(R);
    }
    inline float DistanceToSegment(FVector2D P, const FRoad& R, FVector2D* Closest=nullptr)
    {
        const FVector2D D=R.B-R.A;
        const double T=FMath::Clamp(FVector2D::DotProduct(P-R.A,D)/FMath::Max(1.,D.SizeSquared()),0.,1.);
        const FVector2D Q=R.A+D*T; if(Closest) *Closest=Q;
        return (P-Q).Size();
    }
    inline FVector2D Entrance(const FSite& S){return S.Position+FVector2D(0,-S.Size.Y*.5-400).GetRotated(S.Yaw);}
    inline bool RoadOverlaps(const FSite& S,const FRoad& R){
        if(S.Type==70||S.Type==78)return false; // Open fairgrounds and lakeshore paths may contain roads.
        const FVector2D A=(R.A-S.Position).GetRotated(-S.Yaw),B=(R.B-S.Position).GetRotated(-S.Yaw),D=B-A,E=S.Size*.5+FVector2D(R.Width*.5+100);double Lo=0,Hi=1;for(int I=0;I<2;I++){double P=I?A.Y:A.X,V=I?D.Y:D.X,H=I?E.Y:E.X;if(FMath::Abs(V)<.0001){if(FMath::Abs(P)>H)return false;}else{double L=(-H-P)/V,U=(H-P)/V;if(L>U)Swap(L,U);Lo=FMath::Max(Lo,L);Hi=FMath::Min(Hi,U);if(Lo>Hi)return false;}}return true;
    }
    inline FIntPoint UniqueRegion(int I,int Seed){int X=(I%5-2)*3,Y=(I/5-2)*3;if(X==0&&Y==0)Y=8;const int Rotation=Hash(0,0,Seed,28000)%4;for(int K=0;K<Rotation;K++){int Old=X;X=-Y;Y=Old;}return {X,Y};}
    inline int UniqueIndex(FIntPoint R,int Seed){for(int I=0;I<20;I++)if(UniqueRegion(I,Seed)==R)return I;return INDEX_NONE;}
    inline bool HasTown(FIntPoint R,int32 Seed){return LWNY69::Settlement(R);}
    inline bool IsCity(FIntPoint R,int32 Seed){return R!=FIntPoint::ZeroValue && HasTown(R,Seed) && Hash(R.X,R.Y,Seed,20001)%100<36;}
    inline int DistrictPOI(int District,FRandomStream& Rand){
        static const int Housing[]={7,7,8,9,9,6};static const int Shops[]={4,5,2,14,18,17,65,67};static const int Industry[]={3,13,13,0,16,66};
        return District==1?Housing[Rand.RandRange(0,5)]:District==3?Industry[Rand.RandRange(0,5)]:Shops[Rand.RandRange(0,7)];
    }
    inline bool AirportHere(FIntPoint R,int Seed){return UniqueIndex(R,Seed)==INDEX_NONE&&!HasTown(R,Seed)&&Hash(R.X,R.Y,Seed,28032)%1000<3;}
    // Reserve major destinations before laying out roads and ordinary parcels.
    inline int MajorType53(FIntPoint R,int Seed){
        if(R==FIntPoint::ZeroValue||UniqueIndex(R,Seed)!=INDEX_NONE||R.X*RegionSize>LWBorder51::Strip-30000)return INDEX_NONE;
        const FIntPoint Cell(FloorDiv(R.X,4),FloorDiv(R.Y,4));const int Slot=(R.Y-Cell.Y*4)*4+R.X-Cell.X*4;
        const uint32 H=Hash(Cell.X,Cell.Y,Seed,53001);const int Start=H%16;
        if(Slot==(Start+2)%16&&H%100<55)return 64;
        if(Slot==(Start+7)%16)return 65;if(Slot==(Start+9)%16)return 67;
        if(Slot==Start)return 58;if(Slot==(Start+5)%16)return 59;
        if(Slot==(Start+10)%16&&Hash(Cell.X,Cell.Y,Seed,53002)%100<45)return 21;
        if(Slot==(Start+13)%16)return (H>>8)%2?63:22+(H>>12)%10;
        return INDEX_NONE;
    }
    inline bool SpecialSite(FIntPoint R,int Seed,FSite& S){const int I=UniqueIndex(R,Seed),Major=MajorType53(R,Seed);if(I==INDEX_NONE&&!AirportHere(R,Seed)&&Major==INDEX_NONE)return false;
        S.Type=I!=INDEX_NONE?33+I:AirportHere(R,Seed)?32:Major;S.Size=LWPlaces::Size(S.Type);S.Position=FVector2D(R.X*RegionSize,R.Y*RegionSize)+(I==INDEX_NONE?FVector2D::ZeroVector:FVector2D(14000,-14000));
        if(I==INDEX_NONE&&S.Type!=32&&LWStory::Reserved(S.Position,S.Size.Size()*.5+1800))return false;
        S.Id=I!=INDEX_NONE?Hash(I,0,Seed,28034):Hash(R.X,R.Y,Seed,S.Type==32?28033:53003);S.Friendly=S.Type==21;S.Floors=S.Type==32?3:S.Type==21?5:LWDungeons::IsDungeon(S.Type)?LWDungeons::Profile(S.Type).Floors:1;S.AccessibleFloors=S.Floors;return true;}
    inline TArray<FSite> SpecialNear(FIntPoint R,int Seed){TArray<FSite> Out;for(int Y=-1;Y<=1;Y++)for(int X=-1;X<=1;X++){FSite S;if(SpecialSite(R+FIntPoint(X,Y),Seed,S))Out.Add(S);}return Out;}
    inline bool CutsBox(FVector2D A,FVector2D B,FVector2D Center,FVector2D Half){A-=Center;B-=Center;const FVector2D D=B-A;double Lo=0,Hi=1;for(int K=0;K<2;K++){const double P=K?A.Y:A.X,V=K?D.Y:D.X,H=K?Half.Y:Half.X;if(FMath::Abs(V)<1.e-6){if(FMath::Abs(P)>=H)return false;}else{double L=(-H-P)/V,U=(H-P)/V;if(L>U)Swap(L,U);Lo=FMath::Max(Lo,L);Hi=FMath::Min(Hi,U);if(Lo>=Hi)return false;}}return true;}
    inline void BypassSpecials(TArray<FRoad>& Roads,const TArray<FSite>& Reserved){for(const auto& S:Reserved){TArray<FRoad> Out;for(auto Road:Roads){const FVector2D H=S.Size*.5+FVector2D(2200);if(!CutsBox(Road.A,Road.B,S.Position,H)){Out.Add(Road);continue;}
      auto Outside=[&](FVector2D P){auto Q=P-S.Position;if(FMath::Abs(Q.X)<H.X&&FMath::Abs(Q.Y)<H.Y){if(H.X-FMath::Abs(Q.X)<H.Y-FMath::Abs(Q.Y))Q.X=(Q.X>=0?1:-1)*(H.X+2);else Q.Y=(Q.Y>=0?1:-1)*(H.Y+2);}return Q+S.Position;};
      TArray<FVector2D> N{Outside(Road.A),Outside(Road.B)};for(auto V:{FVector2D(-1,-1),FVector2D(1,-1),FVector2D(1,1),FVector2D(-1,1)})N.Add(S.Position+V*(H+FVector2D(2)));
      double Cost[6]={0,DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX,DBL_MAX};int Prev[6]={-1,-1,-1,-1,-1,-1};bool Done[6]={};for(int Step=0;Step<6;Step++){int A=-1;for(int I=0;I<6;I++)if(!Done[I]&&(A<0||Cost[I]<Cost[A]))A=I;if(A<0||Cost[A]==DBL_MAX)break;Done[A]=true;for(int B=0;B<6;B++)if(B!=A&&!CutsBox(N[A],N[B],S.Position,H)){double C=Cost[A]+FVector2D::Distance(N[A],N[B]);if(C<Cost[B]){Cost[B]=C;Prev[B]=A;}}}
      if(Prev[1]>=0){int B=1;while(Prev[B]>=0){int A=Prev[B];if(FVector2D::Distance(N[A],N[B])>1)Out.Add({N[A],N[B],Road.Width,Road.Highway});B=A;}}
     }Roads=MoveTemp(Out);}}
    inline void RegionRaw(FIntPoint R,int32 Seed,TArray<FRoad>& Roads,TArray<FSite>& Sites){
        if(R.X*RegionSize-RegionSize>LWBorder51::North)return;
        const int RoadStart=Roads.Num(),SiteStart=Sites.Num();
        const FVector2D H=Hub(R,Seed);const bool Town=HasTown(R,Seed);
        auto Parcel=[&](FVector2D P,FVector2D Tangent,int Type,uint32 Id,int Sign,float Width){
            FSite S;S.Type=Type;S.Friendly=Type==21;S.Size=LWPlaces::Size(Type);S.Id=Id;
            S.Yaw=FMath::RadiansToDegrees(FMath::Atan2(Tangent.Y*Sign,Tangent.X*Sign));
            FVector2D Side(-Tangent.Y,Tangent.X);S.Position=P+Side*Sign*(Width*.5+S.Size.Y*.5+1150+(Type==0?900:0));
            Sites.Add(S);Roads.Add({P,Entrance(S),400,false});
        };
        // Every row is connected; periodic north/south spines connect all rows. Other links
        // branch unpredictably, producing T-junctions and long stretches without development.
        for(int Axis=0;Axis<3;Axis++){
            if(Axis==1&&R.X%3!=0&&Hash(R.X,R.Y,Seed,78)%100>28)continue;
            if(Axis==2&&(R.X%3==0||Hash(R.X,R.Y,Seed,79)%100>18))continue;
            FIntPoint Other=R+(Axis==0?FIntPoint(1,0):Axis==1?FIntPoint(0,1):FIntPoint(1,1));
            FVector2D E=Hub(Other,Seed),D=E-H,N=FVector2D(-D.Y,D.X).GetSafeNormal();
            bool Highway=Axis==0&&R.Y%3==0;float Width=Highway?(Hash(0,R.Y,Seed,33100)%3==0?2440.f:1680.f):Axis==2?580:780;
            FRandomStream Rand(Hash(R.X,R.Y,Seed,121+Axis));float Bend=Rand.FRandRange(-5500,5500);
            auto Point=[&](float T){return H+D*T+N*(FMath::Sin(T*PI)*Bend+FMath::Sin(T*2*PI)*Bend*.24);};
            constexpr int Segments=24;for(int I=0;I<Segments;I++)Roads.Add({Point(I/float(Segments)),Point((I+1)/float(Segments)),Width,Highway});
            int Parcels=Town?Rand.RandRange(ParcelLevel,2+ParcelLevel*2):Rand.RandRange(0,ParcelLevel);
            for(int I=0;I<Parcels;I++){
                // Attach to the actual polyline, never to its continuous approximation.
                float T=FMath::Clamp((I+1.f)/(Parcels+1)+Rand.FRandRange(-.06,.06),.16f,.84f);int K=FMath::Min(Segments-1,int(T*Segments));
                FVector2D A=Point(K/float(Segments)),B=Point((K+1)/float(Segments));FVector2D P=FMath::Lerp(A,B,T*Segments-K),Dir=(B-A).GetSafeNormal();
                int Type=Highway?(Rand.FRand()<.5f?0:1):DistrictPOI(Axis==2?3:1,Rand);
                static const int Fringe[]={0,1,10,11,12,15,13,3,16};if(Town&&Axis==0&&T>.4f)Type=Fringe[Rand.RandRange(0,8)];

                if(!Town&&Rand.FRand()<.22f)Type=53+Rand.RandRange(0,2);
                Parcel(P,Dir,Type,Hash(R.X,R.Y,Seed,1000+Axis*10+I),Rand.RandRange(0,1)*2-1,Width);
            }
            // Small residential lanes leave the arterial at a real segment junction.
            // Their curved ends create hamlets instead of extending a uniform grid.
            if(R!=FIntPoint::ZeroValue&&ParcelLevel>0&&Rand.FRand()<(Town?.6f:.25f)){
                const int K=Rand.RandRange(8,16),Side=Rand.RandRange(0,1)*2-1;
                const FVector2D Start=Point(K/float(Segments)),Along=(Point((K+1)/float(Segments))-Start).GetSafeNormal();
                const FVector2D Out(-Along.Y*Side,Along.X*Side);
                const float Length=Rand.FRandRange(7000,11000),Curve=Rand.FRandRange(-1800,1800);
                auto Lane=[&](float T){return Start+Out*(Length*T)+Along*(Curve*T*T);};
                for(int J=0;J<4;J++)Roads.Add({Lane(J*.25f),Lane((J+1)*.25f),520,false});
                for(int J=0;J<ParcelLevel+1;J++){
                    const float T=.5f+J*.15f;const int Segment=FMath::Min(3,int(T*4));
                    const FVector2D A=Lane(Segment*.25f),B=Lane((Segment+1)*.25f);
                    Parcel(FMath::Lerp(A,B,T*4-Segment),(B-A).GetSafeNormal(),7+Rand.RandRange(0,2),Hash(R.X,R.Y,Seed,5000+Axis*20+J),J%2?1:-1,520);
                }
            }
        }
        if(Town){
            const FVector2D Camp=H+FVector2D(0,-5000);Roads.Add({H,Camp,640,false});Roads.Add({Camp+FVector2D(-3100,0),Camp+FVector2D(3100,0),3600,false});
            int SettlementSize=Hash(R.X,R.Y,Seed,1870)%3;for(int I=0;I<(SettlementSize==0?1:3);I++){FSite S;S.Position=Camp+(I==1?FVector2D(0,-3300):FVector2D(I==0?-3200:3200,3200));S.Yaw=I==1?180:0;S.Type=I==0?4:I==1?2:1;S.Size=LWPlaces::Size(S.Type);S.Id=Hash(R.X,R.Y,Seed,7000+I);S.Friendly=true;S.SettlementBuilding=true;Sites.Add(S);Roads.Add({Camp+FVector2D(I==1?0:I==0?-3100:3100,0),Entrance(S),400,false});}

            if(SettlementSize==2)for(int Side:{-1,1}){FSite S;S.Position=Camp+FVector2D(Side*6200,-3200);S.Yaw=180;S.Type=9;S.Size=LWPlaces::Size(9);S.Id=Hash(R.X,R.Y,Seed,7100+Side);S.Friendly=S.SettlementBuilding=true;Sites.Add(S);Roads.Add({Camp+FVector2D(Side*3100,0),Entrance(S),500,false});}
            const FVector2D Junction=H+FVector2D(0,2800);Roads.Add({H,Junction,640,false});
            FVector2D A=H+FVector2D(-8500,2800),B=H+FVector2D(8500,2800);Roads.Add({A,B,640,false});
            FRandomStream TownRand(Hash(R.X,R.Y,Seed,933));int Count=R==FIntPoint::ZeroValue?4:TownRand.RandRange(2,4);
            for(int I=0;I<Count;I++){int Type=R==FIntPoint::ZeroValue?(I==2?2:I):DistrictPOI(2,TownRand);Parcel(H+FVector2D(-6300+I*4200,2800),FVector2D(1,0),Type,Hash(R.X,R.Y,Seed,2000+I),1,640);}
        }
        // Planned neighborhoods have connected local streets and deliberately zoned blocks.
        // Development stays within 250 meters of the hub; regional arterials remain the backbone.
        if(Town && ParcelLevel>0){
            const bool City=IsCity(R,Seed);FRandomStream Urban(Hash(R.X,R.Y,Seed,20002));
            const FVector2D Center=H+FVector2D(0,15500);const int N=City?4:3;const double Block=5600;
            const FVector2D Corner=Center-FVector2D(N*Block*.5);
            Roads.Add({H,Corner+FVector2D(N*Block*.5,0),780,false});
            for(int Y=0;Y<=N;Y++)for(int X=0;X<N;X++)Roads.Add({Corner+FVector2D(X*Block,Y*Block),Corner+FVector2D((X+1)*Block,Y*Block),Y==0?1000.f:720.f,false});
            for(int X=0;X<=N;X++)for(int Y=0;Y<N;Y++)Roads.Add({Corner+FVector2D(X*Block,Y*Block),Corner+FVector2D(X*Block,(Y+1)*Block),X==N/2?1000.f:720.f,false});
            for(int Y=0;Y<N;Y++)for(int X=0;X<N;X++){
                FSite S;S.Id=Hash(R.X,R.Y,Seed,21000+Y*N+X);S.Position=Corner+FVector2D((X+.5)*Block,(Y+.5)*Block);
                S.District=City&&X>0&&X<N-1?4:Y==N-1?1:X==N-1?3:2;
                S.Type=S.District==4?(S.Id%3==0?56:S.Id%3==1?57:20):DistrictPOI(S.District,Urban);
                // An apartment estate at the edge of each city, with repeated residential footprints.
                if(City&&Y==N-1){S.Type=6;S.District=1;}
                S.Size=LWPlaces::Size(S.Type);S.Yaw=0;
                if(LWPlaces::IsTower(S.Type)){S.Floors=Urban.RandRange(12,22);const int Access=S.Id%10;S.AccessibleFloors=Access<3?0:Access<8?Urban.RandRange(2,5):S.Floors;}
                Sites.Add(S);Roads.Add({Corner+FVector2D((X+.5)*Block,Y*Block),Entrance(S),400,false});
            }
            // Quieter residential lane beyond the commercial center, separated by open land.
            FVector2D Start=H+FVector2D(-15500,3500),End=Start+FVector2D(0,14000);
            Roads.Add({H,Start,580,false});Roads.Add({Start,End,520,false});
            for(int I=0;I<4;I++)for(int Side:{-1,1})Parcel(Start+FVector2D(0,2200+I*3100),FVector2D(0,1),I%3==0?9:7+I%2,Hash(R.X,R.Y,Seed,22000+I*2+(Side+1)/2),Side,520);
        }
        auto Reserved=SpecialNear(R,Seed);TArray<FRoad> OwnRoads;for(int I=RoadStart;I<Roads.Num();I++)OwnRoads.Add(Roads[I]);BypassSpecials(OwnRoads,Reserved);Roads.SetNum(RoadStart);Roads.Append(OwnRoads);
        // Exclusive landmark reservations take priority over repeatable parcels.
        for(int I=Sites.Num()-1;I>=SiteStart;I--){const auto S=Sites[I];if(Reserved.ContainsByPredicate([&](const auto& U){FVector2D D=S.Position-U.Position;return FMath::Abs(D.X)<S.Size.Size()*.5+U.Size.X*.5+500&&FMath::Abs(D.Y)<S.Size.Size()*.5+U.Size.Y*.5+500;}))Sites.RemoveAt(I);}
        FSite Special;if(SpecialSite(R,Seed,Special)){const FVector2D Door=Entrance(Special),Approach=Door+FVector2D(0,-3000);FVector2D Access=Approach;double Best=DBL_MAX;for(const auto& Road:OwnRoads){FVector2D Q;double D=DistanceToSegment(Approach,Road,&Q);if(D<Best){Best=D;Access=Q;}}TArray<FRoad> Drive{{Access,Approach,400,false}};BypassSpecials(Drive,Reserved);Roads.Append(Drive);Roads.Add({Approach,Door,400,false});Sites.Add(Special);}
        if(R==FIntPoint::ZeroValue)Roads.Add({FVector2D(-1700,2800),FVector2D(-1700,1700),400,false});
    }
    inline bool ParcelsOverlap(const FSite& A,const FSite& B){
        FVector2D AX=FVector2D(1,0).GetRotated(A.Yaw),AY=FVector2D(-AX.Y,AX.X),BX=FVector2D(1,0).GetRotated(B.Yaw),BY=FVector2D(-BX.Y,BX.X),D=B.Position-A.Position;
        for(auto Axis:{AX,AY,BX,BY}){double RA=FMath::Abs(FVector2D::DotProduct(AX,Axis))*A.Size.X*.5+FMath::Abs(FVector2D::DotProduct(AY,Axis))*A.Size.Y*.5;double RB=FMath::Abs(FVector2D::DotProduct(BX,Axis))*B.Size.X*.5+FMath::Abs(FVector2D::DotProduct(BY,Axis))*B.Size.Y*.5;if(FMath::Abs(FVector2D::DotProduct(D,Axis))>=RA+RB+150)return false;}return true;
    }
    inline void Region(FIntPoint R,int32 Seed,TArray<FRoad>& Roads,TArray<FSite>& Sites){
        LWNY69::Region(R,Roads,Sites);
        CanadaRegion68(R,Seed,Roads,Sites);
    }
    struct FNeighborhood38 {
        FIntPoint Region; int32 Seed=0,Towns=0,Parcels=0;float Ruggedness=0;uint64 Used=0;
        TArray<FRoad> Roads;TArray<FSite> Sites;
    };
    inline thread_local TArray<FNeighborhood38> NeighborhoodCache68;
    inline thread_local uint64 NeighborhoodClock68=0;
    inline void PublishNeighborhood68(FNeighborhood38 Entry){
        auto& Cache=NeighborhoodCache68;for(const auto& E:Cache)if(E.Region==Entry.Region&&E.Seed==Entry.Seed&&E.Towns==Entry.Towns&&E.Parcels==Entry.Parcels&&E.Ruggedness==Entry.Ruggedness)return;
        Entry.Used=++NeighborhoodClock68;if(Cache.Num()>=16){int Old=0;for(int I=1;I<Cache.Num();I++)if(Cache[I].Used<Cache[Old].Used)Old=I;Cache.RemoveAt(Old);}Cache.Add(MoveTemp(Entry));
    }
    // Each planning worker owns a cache; completed neighborhoods are published to gameplay.
    inline const FNeighborhood38& Neighborhood38(FVector2D Position,int32 Seed) {
        const FIntPoint R(FloorDiv(Position.X+RegionSize*.5,RegionSize),FloorDiv(Position.Y+RegionSize*.5,RegionSize));
        auto& Cache=NeighborhoodCache68;auto& Clock=NeighborhoodClock68;++Clock;
        if(auto* Hit=Cache.FindByPredicate([&](const auto& E){return E.Region==R&&E.Seed==Seed&&E.Towns==TownDensity&&E.Parcels==ParcelLevel&&E.Ruggedness==Ruggedness;})){Hit->Used=Clock;return *Hit;}
        FNeighborhood38 Entry;Entry.Region=R;Entry.Seed=Seed;Entry.Towns=TownDensity;Entry.Parcels=ParcelLevel;Entry.Ruggedness=Ruggedness;Entry.Used=Clock;
        for(int Y=-1;Y<=1;++Y)for(int X=-1;X<=1;++X)Region(R+FIntPoint(X,Y),Seed,Entry.Roads,Entry.Sites);
        TSet<uint32> SeenSites73;Entry.Sites.RemoveAll([&](const FSite& S){if(SeenSites73.Contains(S.Id))return true;SeenSites73.Add(S.Id);return false;});
        Entry.Sites.RemoveAll([&](const FSite& S){return Entry.Roads.ContainsByPredicate([&](const FRoad& Road){return RoadOverlaps(S,Road);});});
        if(Cache.Num()>=8){int Oldest=0;for(int I=1;I<Cache.Num();++I)if(Cache[I].Used<Cache[Oldest].Used)Oldest=I;Cache.RemoveAt(Oldest);}
        Cache.Add(MoveTemp(Entry));return Cache.Last();
    }
    inline void Gather(FVector2D Position,int32 Seed,TArray<FRoad>& Roads,TArray<FSite>& Sites) {
        const bool Append=!Roads.IsEmpty()||!Sites.IsEmpty();
        const auto& N=Neighborhood38(Position,Seed);Roads.Append(N.Roads);Sites.Append(N.Sites);
        if(Append)Sites.RemoveAll([&](const FSite& S){return Roads.ContainsByPredicate([&](const FRoad& Road){return RoadOverlaps(S,Road);});});
    }
    inline float Height(FVector2D P, const TArray<FRoad>& Roads,const TArray<FSite>& Sites)
    {
        const float Water=LWNY69::WaterDepth(P);
        if(Water>0&&!LWStory::Reserved(P,6000)&&!Sites.ContainsByPredicate([&](const auto& S){return (P-S.Position).Size()<S.Size.Size()*.5+1200;}))return -Water;
        if(LWGeography84::Canada(P)){float Blend=1.f;Blend=Blend*Blend*(3-2*Blend);for(const auto& R:Roads)Blend=FMath::Min(Blend,FMath::Clamp((DistanceToSegment(P,R)-R.Width*.5f-800)/2200.f,0.f,1.f));for(const auto& S:Sites){auto D=P-S.Position;Blend=FMath::Min(Blend,FMath::Clamp(float(FMath::Max(FMath::Abs(D.X)-S.Size.X*.5-1000,FMath::Abs(D.Y)-S.Size.Y*.5-1000)/2200),0.f,1.f));}return LWAviation84::Surface(P,Blend*(300+FMath::PerlinNoise2D(P*.00010)*350+FMath::PerlinNoise2D(P*.000025)*850));}
        // The permanent bunker parcel uses global coordinates so every chunk agrees.
        const FVector2D ShelterOffset=P-FVector2D(-1700,1700);
        const double ShelterDistance=FMath::Max(FMath::Abs(ShelterOffset.X)-650.,FMath::Abs(ShelterOffset.Y)-650.);
        float Blend=FMath::Clamp(float(ShelterDistance/900.),0.f,1.f);
        for(const FRoad& R:Roads) Blend=FMath::Min(Blend,FMath::Clamp((DistanceToSegment(P,R)-R.Width*.5f-110)/900.f,0.f,1.f));
        for(const FSite& S:Sites)
        {
            const FVector2D Q=(P-S.Position).GetRotated(-S.Yaw);
            // Cover a full terrain-cell diagonal beyond the developed footprint, so
            // interpolated triangles cannot cross up through floors or forecourts.
            const float Pad=FMath::Max(700.f,ChunkSize/24.f*1.5f);
            const float DX=FMath::Abs(Q.X)-S.Size.X*.5-Pad;
            const float MinY=S.Type==21?FMath::Min(-9400.f,float(-S.Size.Y*.5)):-S.Size.Y*.5;
            const float DY=FMath::Max(float(Q.Y-S.Size.Y*.5),float(MinY-Q.Y))-Pad;
            const float D=FMath::Max(DX,DY);
            Blend=FMath::Min(Blend,FMath::Clamp(D/900,0.f,1.f));
        }
        Blend=Blend*Blend*(3-2*Blend);
        return LWAviation84::Surface(P,Blend*Ruggedness*(180+FMath::PerlinNoise2D(P*.000075)*420+FMath::PerlinNoise2D(P*.00039)*120));
    }
}
