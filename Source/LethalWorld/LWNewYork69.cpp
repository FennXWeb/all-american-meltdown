#include "LWAircraft84.h"
#include "LWGeography84.h"
#include "LWCampaign76Corridor.h"
#include "LWSyracuse73.h"
#include "LWNewYork69.h"
#include "LWGeneration.h"

namespace LWNY69 {
FVector2D Project(double Lat,double Lon){return LWSyracuse73::Warp({(Lat-42.8864)*1111000.,(Lon+78.8784)*813000.});}
float WaterDepth(FVector2D P){if(const float D=LWSyracuse73::LakeDepth(P);D>0)return D;return LWGeography84::WaterDepth(P);}
const TArray<FTown>& Towns(){static const TArray<FTown> T={
 {TEXT("BUFFALO"),42.8864,-78.8784,6},{TEXT("NIAGARA FALLS"),43.0962,-79.0377,4},
 {TEXT("ROCHESTER"),43.1566,-77.6088,6},{TEXT("SYRACUSE"),43.0481,-76.1474,6},
 {TEXT("ORCHARD PARK"),42.7676,-78.7439,2},{TEXT("WILLIAMSVILLE"),42.9639,-78.7378,2},
 {TEXT("TONAWANDA"),43.0203,-78.8803,2},{TEXT("LOCKPORT"),43.1706,-78.6903,3},
 {TEXT("BATAVIA"),42.9981,-78.1875,3},{TEXT("LE ROY"),42.9784,-77.9842,2},
 {TEXT("BROCKPORT"),43.2137,-77.9392,2},{TEXT("GENESEO"),42.7959,-77.8169,2},
 {TEXT("CANANDAIGUA"),42.8742,-77.2880,3},{TEXT("VICTOR"),42.9826,-77.4092,2},
 {TEXT("GENEVA"),42.8689,-76.9777,3},{TEXT("WATERLOO"),42.9048,-76.8627,2},
 {TEXT("SENECA FALLS"),42.9106,-76.7966,2},{TEXT("AUBURN"),42.9317,-76.5661,3},
 {TEXT("SKANEATELES"),42.9470,-76.4291,2},{TEXT("BALDWINSVILLE"),43.1587,-76.3327,2},
 {TEXT("LIVERPOOL"),43.1065,-76.2177,2},
 {TEXT("MEDINA"),43.2201,-78.3869,2},{TEXT("ALBION"),43.2464,-78.1936,2},
 {TEXT("PALMYRA"),43.0639,-77.2333,2},{TEXT("NEWARK"),43.0468,-77.0953,2},{TEXT("LYONS"),43.0642,-76.9903,2},
 {TEXT("WEEDSPORT"),43.0487,-76.5627,2},{TEXT("CAMILLUS"),43.0392,-76.3041,2},
 {TEXT("ROME"),43.2128,-75.4557,4},{TEXT("CAMDEN"),43.3348,-75.7474,2},{TEXT("PULASKI"),43.567,-76.1277,2},
 {TEXT("WATERTOWN"),43.9748,-75.9108,4},{TEXT("CLAYTON"),44.2395,-76.0858,2},
 {TEXT("ALEXANDRIA BAY"),44.3359,-75.9177,2},{TEXT("LOWVILLE"),43.7867,-75.4919,2},{TEXT("OLD FORGE"),43.7101,-74.9743,2}};return T;}
namespace {
FIntPoint RegionAt(FVector2D P){return {LWGen::FloorDiv(P.X+25600,51200),LWGen::FloorDiv(P.Y+25600,51200)};}
struct FAtlas {
 TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;
 TMap<FIntPoint,TArray<int>> RoadIndex,SiteIndex;
 TMap<FIntPoint,TPair<FVector2D,FString>> Settlements;
 FAtlas(){
  using namespace LWGen;
  auto Road=[&](FVector2D A,FVector2D B,float Width=780,bool Highway=false){if((B-A).Size()>1)Roads.Add({A,B,Width,Highway});};
  auto Route=[&](std::initializer_list<FVector2D> Points,float Width,bool Highway){bool First=true;FVector2D Last;for(auto P:Points){if(!First)Road(Last,P,Width,Highway);Last=P;First=false;}};
  auto G=[](double Lat,double Lon){return Project(Lat,Lon);};
  LWGeography84::Roads(Roads);
  auto Landmark=[&](int Type,FVector2D Position,const TCHAR* Name){FSite S;S.Type=Type;S.Size=LWPlaces::Size(Type);S.Position=Position;S.Id=0xEA690000u+Sites.Num();S.PlaceName69=Name;S.Floors=Type==69?4:1;S.AccessibleFloors=S.Floors;Sites.Add(S);};
  Landmark(68,G(42.774,-78.792),TEXT("HIGHMARK STADIUM"));
  Landmark(69,G(43.071,-76.171),TEXT("DESTINY USA"));
  Landmark(32,G(42.9405,-78.7322),TEXT("BUFFALO NIAGARA INTERNATIONAL AIRPORT"));
  Landmark(32,G(43.1189,-77.6724),TEXT("GREATER ROCHESTER INTERNATIONAL AIRPORT"));
  Landmark(32,G(43.1133,-76.1112),TEXT("SYRACUSE HANCOCK INTERNATIONAL AIRPORT"));
  Landmark(21,G(43.087,-79.015),TEXT("NIAGARA CASINO RESORT"));
  Landmark(59,G(42.931,-76.56)+FVector2D(30000,0),TEXT("AUBURN CORRECTIONAL FACILITY"));
  Landmark(58,G(43.20,-77.68),TEXT("GREECE SHOPPING CENTER"));
  // Preserve every existing dungeon and unique destination as a fixed rural stop.
  for(int I=0;I<30;I++)Landmark(I<10?22+I:33+I-10,FVector2D(-190000-(I%3)*62000,120000+(I/3)*235000),LWPlaces::Name(I<10?22+I:33+I-10));
  LWSyracuse73::Landmarks(Sites);
  LWCampaign76::AddCorridor(Sites,Roads);
  for(auto& Site:Sites)if(Site.Type==32){Site.Size=Site.Id==0xEA690004u?FVector2D(340000,320000):FVector2D(165000,165000);if(Site.Id!=0xEA690004u)if(const auto* F=LWAviation84::Closest(Site.Position)){auto D=F->Direction();Site.Position=FVector2D(F->Apron+FVector(-D.Y,D.X,0)*14000);}}
  for(const auto& A:LWAviation84::Runways())if(A.Id==TEXT("ART")||A.Id==TEXT("RME")||A.Id==TEXT("YYZ")){FSite S;S.Type=32;S.Size=FVector2D(A.Length()+20000);auto D=A.Direction();S.Position=FVector2D(A.Apron+FVector(-D.Y,D.X,0)*14000);S.Id=0xEA840000u+GetTypeHash(A.Id.ToString());S.PlaceName69=A.Name;S.Friendly=true;Sites.Add(S);}
  TArray<FSite> Reserved=Sites;Reserved.RemoveAll([](const auto& S){return S.Type>=70&&S.Type!=74;});
  FSite Bunker;Bunker.Position=FVector2D(-1700,1700);Bunker.Size=FVector2D(7500);Reserved.Add(Bunker);
  // Named communities occupy town-edge parcels beside the mapped road network.
  for(int T:{8,10,12,17,23,24,28,29,30,31,32,33,34,35}){if(!Towns().IsValidIndex(T))continue;const auto& Town=Towns()[T];FVector2D Center=G(Town.Latitude,Town.Longitude);
   FVector2D H=Center+FVector2D(0,24000);double Best=DBL_MAX;FVector2D Join=Center;
   for(const auto& R:Roads){FVector2D Q;const double D=DistanceToSegment(H,R,&Q);if(D<Best&&!R.Highway){Best=D;Join=Q;}}
   FVector2D Away=(H-Join).GetSafeNormal();if(Away.IsNearlyZero())Away=FVector2D(0,1);H=Join+Away*17000;
   if(WaterDepth(H)>0||LWGeography84::Canada(H)||LWSyracuse73::Contains(H)||Reserved.ContainsByPredicate([&](const auto& S){return (S.Position-H).Size()<S.Size.Size()*.5+14000;}))continue;
   FSite Reserve;Reserve.Position=H;Reserve.Size=FVector2D(28000,32000);Reserved.Add(Reserve);Settlements.Add(RegionAt(H),{H+FVector2D(0,5000),Town.Name});Road(Join,H+FVector2D(0,-11500),650);
   Road(H+FVector2D(0,-11500),H+FVector2D(0,11500),650);Road(H+FVector2D(-11500,0),H+FVector2D(11500,0),650);
  }
  LWSyracuse73::Streets(Roads);
  // Overlapping reservation margins must be routed as a single obstacle. A
  // later independent bypass otherwise pushes roads into the preceding site.
  TArray<FSite> RoadReserves=Reserved;bool Merged=true;
  while(Merged){Merged=false;for(int I=0;I<RoadReserves.Num()&&!Merged;I++)for(int J=I+1;J<RoadReserves.Num();J++){
   auto& A=RoadReserves[I];const auto B=RoadReserves[J];const auto D=(A.Position-B.Position).GetAbs(),H=(A.Size+B.Size)*.5+FVector2D(4404);
   if(D.X>H.X||D.Y>H.Y)continue;const FVector2D Lo(FMath::Min(A.Position.X-A.Size.X*.5,B.Position.X-B.Size.X*.5),FMath::Min(A.Position.Y-A.Size.Y*.5,B.Position.Y-B.Size.Y*.5)),Hi(FMath::Max(A.Position.X+A.Size.X*.5,B.Position.X+B.Size.X*.5),FMath::Max(A.Position.Y+A.Size.Y*.5,B.Position.Y+B.Size.Y*.5));A.Position=(Lo+Hi)*.5;A.Size=Hi-Lo;RoadReserves.RemoveAt(J);Merged=true;break;
  }}
  BypassSpecials(Roads,RoadReserves);
  LWCampaign76::FinishApproach78(Sites,Roads);
  // Restore public approaches as well as the internal cross after reservation bypasses.
  const int PerimeterRoads=Roads.Num();
  for(const auto& Pair:Settlements){const auto H=Pair.Value.Key-FVector2D(0,5000);
   for(FVector2D Offset:{FVector2D(0,-11500),FVector2D(0,11500),FVector2D(-11500,0),FVector2D(11500,0)}){const FVector2D End=H+Offset;FVector2D Join=End;double Best=20000;
    for(int I=0;I<PerimeterRoads;I++){FVector2D Q;const double D=DistanceToSegment(End,Roads[I],&Q);if(D>=Best)continue;const FRoad Link{End,Q,650,false};if(Reserved.ContainsByPredicate([&](const auto& O){return !O.Position.Equals(H,1)&&RoadOverlaps(O,Link);}))continue;bool Dry=true;for(int J=0;J<=16;J++)Dry&=WaterDepth(FMath::Lerp(End,Q,J/16.))==0;if(Dry){Best=D;Join=Q;}}
    if(Best<20000)Road(End,Join,650);
   }
   Road(H+FVector2D(0,-11500),H+FVector2D(0,11500),650);Road(H+FVector2D(-11500,0),H+FVector2D(11500,0),650);
  }
  // Visible 1.93 m junction closure at the Baldwinsville survey/extract seam.
  Road(FVector2D(751410.8,1517743.9),FVector2D(751286.7099,1517891.6753),650);

  // Attach landmark entrances after reservations have moved the through roads.
  for(const auto& S:Sites){if(S.Type>=70)continue;const FVector2D Door=Entrance(S);FVector2D Join=Door;double Best=DBL_MAX;for(const auto& R:Roads){FVector2D Q;const double D=DistanceToSegment(Door,R,&Q);if(D<Best&&!Reserved.ContainsByPredicate([&](const auto& O){return RoadOverlaps(O,{Q,Door,400,false});})){Best=D;Join=Q;}}if(Best<DBL_MAX)Road(Join,Door,400);}
  {const FVector2D Entry(-1700,1700);FVector2D Join=Entry;double Best=DBL_MAX;for(const auto& R:Roads){FVector2D Q;double D=DistanceToSegment(Entry,R,&Q);if(D<Best){Best=D;Join=Q;}}Road(Join,Entry,400);}
  for(const auto& Pair:Settlements){const auto H=Pair.Value.Key-FVector2D(0,5000);for(int Side:{-1,1})for(int Row=0;Row<4;Row++){
   const int Types[]={67,2,19,9};FSite S;S.Position=H+FVector2D(Side*6800,-11250+Row*7500);S.Type=Types[Row];S.Size=LWPlaces::Size(S.Type);S.Yaw=Side<0?-90:90;S.Id=0xB6900000u+Sites.Num();S.Friendly=true;S.SettlementBuilding=true;S.PlaceName69=Pair.Value.Value+TEXT(" / ")+LWPlaces::Name(S.Type);
   if(!Roads.ContainsByPredicate([&](const auto& R){return RoadOverlaps(S,R);}))Sites.Add(S);
  }}
  LWGeography84::FillParcels(Sites,Roads);
  LWSyracuse73::Buildings(Sites,Roads);
  // Roads are indexed as short chords, so a region query never scans the entire atlas.
  TArray<FRoad> Split;for(const auto& R:Roads){int Steps=FMath::Max(1,FMath::CeilToInt((R.B-R.A).Size()/8000));for(int I=0;I<Steps;I++)Split.Add({FMath::Lerp(R.A,R.B,double(I)/Steps),FMath::Lerp(R.A,R.B,double(I+1)/Steps),R.Width,R.Highway});}Roads=MoveTemp(Split);
  for(int I=0;I<Roads.Num();I++){auto A=RegionAt(Roads[I].A),B=RegionAt(Roads[I].B);for(int X=FMath::Min(A.X,B.X);X<=FMath::Max(A.X,B.X);X++)for(int Y=FMath::Min(A.Y,B.Y);Y<=FMath::Max(A.Y,B.Y);Y++)RoadIndex.FindOrAdd({X,Y}).Add(I);}
  for(int I=0;I<Sites.Num();I++){const auto& S=Sites[I];if(S.Size.GetMax()>12800){auto A=RegionAt(S.Position-S.Size*.5),B=RegionAt(S.Position+S.Size*.5);for(int X=A.X;X<=B.X;X++)for(int Y=A.Y;Y<=B.Y;Y++)SiteIndex.FindOrAdd({X,Y}).Add(I);}else SiteIndex.FindOrAdd(RegionAt(S.Position)).Add(I);}
 }
};
const FAtlas& Atlas(){static const FAtlas Data;return Data;}
}
const TArray<LWGen::FRoad>& Roads(){return Atlas().Roads;}
const TArray<LWGen::FSite>& Sites(){return Atlas().Sites;}
void Region(FIntPoint R,TArray<LWGen::FRoad>& OutRoads,TArray<LWGen::FSite>& OutSites){const auto& A=Atlas();if(const auto* I=A.RoadIndex.Find(R))for(int K:*I)OutRoads.Add(A.Roads[K]);if(const auto* I=A.SiteIndex.Find(R))for(int K:*I)OutSites.Add(A.Sites[K]);}
bool Settlement(FIntPoint R,FVector2D* Hub,FString* Name){if(const auto* S=Atlas().Settlements.Find(R)){if(Hub)*Hub=S->Key;if(Name)*Name=S->Value;return true;}return false;}
FVector2D RoadHub(FIntPoint R){FVector2D Hub;if(Settlement(R,&Hub))return Hub;const FVector2D P(R.X*51200.,R.Y*51200.);Hub=P;double Best=DBL_MAX;if(const auto* Indices=Atlas().RoadIndex.Find(R))for(int I:*Indices){FVector2D Q;double D=LWGen::DistanceToSegment(P,Atlas().Roads[I],&Q);if(D<Best){Best=D;Hub=Q;}}return Hub;}
}
