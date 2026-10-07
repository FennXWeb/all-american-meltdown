#include "LWGeography84.h"
#include "LWGeneration.h"
#include "LWSyracuse73.h"
namespace {
struct FPoint84 {double X,Y;operator FVector2D()const{return {X,Y};}};
struct FMapRoad84 {FPoint84 A,B;float Width;bool Highway;int Name;bool OSM;};
struct FPolygon84 {const TCHAR* Name;const FPoint84* Points;int Count;};
#include "LWGeographyData84.inl"
struct FBoundary84 {FBox2D Bounds;const FPolygon84* Poly;};
TArray<FBoundary84> Prepare84(const FPolygon84* Data,int Count){TArray<FBoundary84> Out;for(int I=0;I<Count;I++){FBox2D B(ForceInit);for(int J=0;J<Data[I].Count;J++)B+=FVector2D(Data[I].Points[J]);Out.Add({B,&Data[I]});}return Out;}
bool Inside84(const FBoundary84& B,FVector2D P){if(!B.Bounds.IsInside(P))return false;bool Inside=false;const auto& Poly=*B.Poly;for(int I=0,J=Poly.Count-1;I<Poly.Count;J=I++){const FVector2D A=Poly.Points[I],C=Poly.Points[J];if((A.Y>P.Y)!=(C.Y>P.Y)&&P.X<(C.X-A.X)*(P.Y-A.Y)/(C.Y-A.Y)+A.X)Inside=!Inside;}return Inside;}
}
float LWGeography84::WaterDepth(FVector2D P){
 static const auto Polys=Prepare84(WaterPolygons84,UE_ARRAY_COUNT(WaterPolygons84));
 for(const auto& Poly:Polys)if(Inside84(Poly,P)){float Edge=FLT_MAX;for(int I=1;I<Poly.Poly->Count;I++)Edge=FMath::Min(Edge,LWGen::DistanceToSegment(P,{Poly.Poly->Points[I-1],Poly.Poly->Points[I],0,false}));return FMath::Min(12000.f,Edge*.25f);}return 0;
}
bool LWGeography84::Canada(FVector2D P){static const auto Polys=Prepare84(CanadaPolygons84,UE_ARRAY_COUNT(CanadaPolygons84));for(const auto& B:Polys)if(Inside84(B,P))return true;return false;}
const TArray<LWGen::FRoad>& LWGeography84::Border(){static const auto Data=[](){TArray<LWGen::FRoad> R;for(const auto& L:BorderLines84)R.Add({L.A,L.B,0,false});return R;}();return Data;}
FVector2D LWGeography84::BorderNearest(FVector2D P){FVector2D Best=P;double Dist=DBL_MAX;for(const auto& R:Border()){FVector2D Q;const double D=LWGen::DistanceToSegment(P,R,&Q);if(D<Dist){Dist=D;Best=Q;}}return Best;}
const TArray<LWGeography84::FCheckpoint>& LWGeography84::Checkpoints(){static const TArray<FCheckpoint> Data=[](){TArray<FCheckpoint> R;for(auto G:{FVector2D(42.9086,-78.9077),FVector2D(43.1514,-79.0430),FVector2D(44.3447,-75.9803)}){auto P=BorderNearest(LWNY69::Project(G.X,G.Y));float Yaw=0;for(float A:{0.f,45.f,90.f,135.f,180.f,225.f,270.f,315.f})if(Canada(P+FVector2D(4000,0).GetRotated(A))&&!Canada(P-FVector2D(4000,0).GetRotated(A))){Yaw=A;break;}R.Add({P,Yaw,R.Num()==0?TEXT("PEACE BRIDGE"):R.Num()==1?TEXT("LEWISTON / QUEENSTON"):TEXT("THOUSAND ISLANDS")});}return R;}();return Data;}
bool LWGeography84::NearCheckpoint(FVector2D P,float Distance){for(const auto& C:Checkpoints())if((C.Position-P).Size()<Distance)return true;return false;}
bool LWGeography84::Restricted(FVector2D P){if(!Canada(P))return false;for(const auto& C:Checkpoints()){const auto Local=(P-C.Position).GetRotated(-C.Yaw);if(FMath::Abs(Local.Y)<1800&&Local.X<=300&&Local.X>-7000)return false;}return true;}
void LWGeography84::Roads(TArray<LWGen::FRoad>& Out){
 // Both regional and town surveys use the same projection. Keeping the junction
 // vertices prevents the road graph diverging from the visible road surfaces.
 for(const auto& R:MapRoads84){const FVector2D A=R.A,B=R.B;Out.Add({A,B,R.Width,R.Highway});}
 // Connect the town extracts to the generalized regional survey at their edges.
 const int Count=Out.Num();TMap<FIntPoint,TArray<int>> Index;auto Key=[](FVector2D P){return FIntPoint(FMath::FloorToInt(P.X/16000),FMath::FloorToInt(P.Y/16000));};
 for(int I=0;I<Count;I++)if(!MapRoads84[I].OSM){auto A=Key(Out[I].A),B=Key(Out[I].B);for(int X=FMath::Min(A.X,B.X);X<=FMath::Max(A.X,B.X);X++)for(int Y=FMath::Min(A.Y,B.Y);Y<=FMath::Max(A.Y,B.Y);Y++)Index.FindOrAdd({X,Y}).Add(I);}
 TSet<FIntPoint> Joined;
 for(const auto& Post:Checkpoints())for(int Side:{-1,1}){const FVector2D P=Post.Position+FVector2D(Side*7000,0).GetRotated(Post.Yaw);float Best=30000;FVector2D Join=P;for(int I=0;I<Count;I++){FVector2D Q;float D=LWGen::DistanceToSegment(P,Out[I],&Q);if(D<Best&&Canada(Q)==(Side>0)){Best=D;Join=Q;}}if(Best<30000&&Best>80)Out.Add({P,Join,1100,false});Out.Add({Post.Position,P,1100,false});}
 for(int I=0;I<Count;I++)if(MapRoads84[I].OSM&&Out[I].Width>=750){const FVector2D P=Out[I].A;auto K=Key(P);if(Joined.Contains(K))continue;float Best=8000;FVector2D Join=P;
  for(int X=-1;X<=1;X++)for(int Y=-1;Y<=1;Y++)if(const auto* Ids=Index.Find(K+FIntPoint(X,Y)))for(int J:*Ids){FVector2D Q;float D=LWGen::DistanceToSegment(P,Out[J],&Q);if(D<Best&&WaterDepth((P+Q)*.5)==0){Best=D;Join=Q;}}
  if(Best<8000&&Best>80){Out.Add({P,Join,700,false});Joined.Add(K);}
 }
}
void LWGeography84::FillParcels(TArray<LWGen::FSite>& Sites,TArray<LWGen::FRoad>& Roads){
 using namespace LWGen;
 TMap<FIntPoint,TArray<int>> RoadIndex,SiteIndex;auto Key=[](FVector2D P){return FIntPoint(FMath::FloorToInt(P.X/16000),FMath::FloorToInt(P.Y/16000));};
 auto IndexSite=[&](int I){const auto& S=Sites[I];auto A=Key(S.Position-FVector2D(S.Size.Size()*.5+500)),B=Key(S.Position+FVector2D(S.Size.Size()*.5+500));for(int X=A.X;X<=B.X;X++)for(int Y=A.Y;Y<=B.Y;Y++)SiteIndex.FindOrAdd({X,Y}).Add(I);};
 for(int I=0;I<Sites.Num();I++)IndexSite(I);
 auto IndexRoad=[&](int I){const auto& R=Roads[I];auto A=Key(FVector2D(FMath::Min(R.A.X,R.B.X),FMath::Min(R.A.Y,R.B.Y))-FVector2D(2000)),B=Key(FVector2D(FMath::Max(R.A.X,R.B.X),FMath::Max(R.A.Y,R.B.Y))+FVector2D(2000));for(int X=A.X;X<=B.X;X++)for(int Y=A.Y;Y<=B.Y;Y++)RoadIndex.FindOrAdd({X,Y}).Add(I);};
 const int RoadCount=Roads.Num();for(int I=0;I<RoadCount;I++)IndexRoad(I);
 int Added=0;
 for(int I=0;I<RoadCount;I++){
  const auto R=Roads[I];const double Length=(R.B-R.A).Size();if(Length<1600||R.Width<500)continue;
  const auto Mid=(R.A+R.B)*.5;if(LWSyracuse73::Contains(Mid)||Canada(Mid))continue;
  const LWNY69::FTown* Town=nullptr;double TownDist=DBL_MAX;for(const auto& T:LWNY69::Towns()){const double D=(LWNY69::Project(T.Latitude,T.Longitude)-Mid).Size();if(D<TownDist){TownDist=D;Town=&T;}}
  const bool Urban=Town&&TownDist<((Town->Blocks>=4)?52000:26000);if(!Urban&&(R.Highway||I%7!=0))continue;
  const FVector2D Forward=(R.B-R.A).GetSafeNormal(),Normal(-Forward.Y,Forward.X);
  const int Count=FMath::Clamp(int(Length/(Urban?4300:14000)),1,8);
  for(int N=0;N<Count;N++)for(int Sign:{-1,1}){
   const FVector2D At=FMath::Lerp(R.A,R.B,(N+.5)/Count);FRandomStream Random(Hash(FMath::RoundToInt(At.X/10),FMath::RoundToInt(At.Y/10),84,Sign+2));
   FSite S;const int Residential[]={7,8,9,6},Commercial[]={4,5,10,11,12,14,18,60,61,62,67},Rural[]={0,1,7,8,13,16};
   S.Type=Urban?(R.Highway||Random.FRand()<.30?Commercial[Random.RandRange(0,10)]:Residential[Random.RandRange(0,3)]):Rural[Random.RandRange(0,5)];S.Size=LWPlaces::Size(S.Type);S.Yaw=FMath::RadiansToDegrees(FMath::Atan2(Forward.Y*Sign,Forward.X*Sign));S.Position=At+Normal*Sign*(R.Width*.5+S.Size.Y*.5+500);S.Id=0x84000000u+(Hash(FMath::RoundToInt(At.X/10),FMath::RoundToInt(At.Y/10),84,Sign+3)&0xFFFFFF);S.District=Urban?S.Type>=7&&S.Type<=9?1:2:0;
   S.PlaceName69=(Town?FString(Town->Name):TEXT("UPSTATE"))+TEXT(" / ")+LWPlaces::Name(S.Type)+FString::Printf(TEXT(" %03d"),int(S.Id%1000));
   bool Wet=WaterDepth(S.Position)>0||Canada(S.Position);for(int X:{-1,1})for(int Y:{-1,1}){FVector2D Corner=S.Position+FVector2D(S.Size.X*.5*X,S.Size.Y*.5*Y).GetRotated(S.Yaw);Wet|=LWNY69::WaterDepth(Corner)>0||Canada(Corner);}if(Wet)continue;auto K=Key(S.Position);bool Blocked=false;
   for(int X=-1;X<=1&&!Blocked;X++)for(int Y=-1;Y<=1&&!Blocked;Y++){
    if(const auto* Ids=SiteIndex.Find(K+FIntPoint(X,Y)))for(int J:*Ids)if(ParcelsOverlap(S,Sites[J])){Blocked=true;break;}
    if(const auto* Ids=RoadIndex.Find(K+FIntPoint(X,Y)))for(int J:*Ids)if(RoadOverlaps(S,Roads[J])){Blocked=true;break;}
   }
   if(Blocked)continue;Sites.Add(S);IndexSite(Sites.Num()-1);Roads.Add({At,Entrance(S),350,false});IndexRoad(Roads.Num()-1);Added++;
  }
 }
}
