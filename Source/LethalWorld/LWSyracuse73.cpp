#include "LWSyracuse73.h"
#include "LWGeneration.h"
namespace LWSyracuse73 {
namespace {
const FVector2D Center((43.0481-42.8864)*1111000.,(-76.1474+78.8784)*813000.);
// Plain aggregates keep the large offline tables in read-only data with no startup constructors.
struct FPoint73 {double X,Y;operator FVector2D()const{return FVector2D(X,Y);}};
struct FRoad73 {FPoint73 A,B;float Width;bool Highway;int Name;};
struct FLandmark73 {int Type;FPoint73 P,Size;float Yaw;const TCHAR* Name;};
struct FBlock73 {uint32 Id;FPoint73 P,Size;float Yaw;int District,Floors;const TCHAR* Name;};
struct FFill73 {uint32 Id;FPoint73 P;int Type;float Yaw;const TCHAR* Name;};
#include "LWSyracuseData73.inl"
}
FVector2D Warp(FVector2D P){const auto D=P-Center;const double R=D.Size();if(R<.001)return P;const double F=R<=90000?R*10:R<1100000?900000+(R-90000)*200000/1010000:R;return Center+D*(F/R);}
FVector2D Unwarp(FVector2D P){const auto D=P-Center;const double R=D.Size();if(R<.001)return P;const double F=R<=900000?R/10:R<1100000?90000+(R-900000)*1010000/200000:R;return Center+D*(F/R);}
bool Contains(FVector2D P){const auto Q=Unwarp(P);const double Lat=Q.X/1111000+42.8864,Lon=Q.Y/813000-78.8784;return Lat>42.995&&Lat<43.145&&Lon>-76.265&&Lon<-76.065;}
float LakeDepth(FVector2D P){
 static const FBox2D Bounds=[](){FBox2D B(ForceInit);for(auto V:Lake73)B+=V;return B;}();if(!Bounds.IsInside(P))return 0;
 bool In=false;float Edge=FLT_MAX;constexpr int Count=UE_ARRAY_COUNT(Lake73);
 for(int I=0,J=Count-1;I<Count;J=I++){const auto A=Lake73[I],B=Lake73[J];if((A.Y>P.Y)!=(B.Y>P.Y)&&P.X<(B.X-A.X)*(P.Y-A.Y)/(B.Y-A.Y)+A.X)In=!In;Edge=FMath::Min(Edge,LWGen::DistanceToSegment(P,{A,B,0,false}));}
 return In?FMath::Min(2200.f,Edge*.18f):0.f;
}
void Landmarks(TArray<LWGen::FSite>& Sites){for(const auto& L:RawLandmarks73){LWGen::FSite S;S.Type=L.Type;S.Id=0xEA730000u+L.Type;S.Position=L.P;S.Size=L.Size;S.Yaw=L.Yaw;S.Floors=L.Type==72?2:L.Type==76?3:1;S.AccessibleFloors=S.Floors;S.PlaceName69=L.Name;Sites.Add(S);}}
void Streets(TArray<LWGen::FRoad>& Roads){
 TArray<LWGen::FRoad> Outside;TArray<FVector2D> Joins;
 // Cut only obsolete local alignments. Boundary endpoints remain connected to the state network.
 for(const auto& R:Roads){const int N=FMath::Max(1,FMath::CeilToInt((R.B-R.A).Size()/4000));for(int I=0;I<N;I++){
  const auto A=FMath::Lerp(R.A,R.B,double(I)/N),B=FMath::Lerp(R.A,R.B,double(I+1)/N);const bool IA=Contains(A),IB=Contains(B);
  if(!IA&&!IB){Outside.Add({A,B,R.Width,R.Highway});continue;}
  if(IA==IB)continue;
  FVector2D Out=IA?B:A,In=IA?A:B;for(int J=0;J<32;J++){const auto Mid=(Out+In)*.5;if(Contains(Mid))In=Mid;else Out=Mid;}
  const auto Edge=(Out+In)*.5;Outside.Add({IA?B:A,Edge,R.Width,R.Highway});Joins.Add(Edge);
 }}
 Roads=MoveTemp(Outside);for(const auto& R:RawRoads73)Roads.Add({R.A,R.B,R.Width,R.Highway});
 for(auto P:Joins){double Best=DBL_MAX;FVector2D Join=P;for(const auto& R:RawRoads73){FVector2D Q;double D=LWGen::DistanceToSegment(P,{R.A,R.B,R.Width,R.Highway},&Q);if(D<Best){Best=D;Join=Q;}}if(Best<200000)Roads.Add({P,Join,700,false});}
}
void Buildings(TArray<LWGen::FSite>& Sites,const TArray<LWGen::FRoad>& Roads){
 // Data was checked offline against the street widths; recheck only nearby changed access roads.
 TMap<FIntPoint,TArray<int>> Index;for(int I=0;I<Roads.Num();I++){auto A=Roads[I].A/16000.,B=Roads[I].B/16000.;for(int X=FMath::FloorToInt(FMath::Min(A.X,B.X))-1;X<=FMath::FloorToInt(FMath::Max(A.X,B.X))+1;X++)for(int Y=FMath::FloorToInt(FMath::Min(A.Y,B.Y))-1;Y<=FMath::FloorToInt(FMath::Max(A.Y,B.Y))+1;Y++)Index.FindOrAdd({X,Y}).Add(I);}
 auto Clear=[&](const LWGen::FSite& S){if(LakeDepth(S.Position)>0)return false;const auto K=FIntPoint(FMath::FloorToInt(S.Position.X/16000),FMath::FloorToInt(S.Position.Y/16000));if(const auto* Rows=Index.Find(K))for(int I:*Rows)if(LWGen::RoadOverlaps(S,Roads[I]))return false;return true;};
 for(const auto& B:RawBlocks73){LWGen::FSite S;S.Type=79;S.Id=0xD0000000u+(B.Id&0x0fffffffu);S.Position=B.P;S.Size=B.Size;S.Yaw=B.Yaw;S.District=B.District;S.Floors=B.Floors;S.AccessibleFloors=0;S.PlaceName69=B.Name;if(Clear(S))Sites.Add(S);}
 for(const auto& B:RawFill73){LWGen::FSite S;S.Id=B.Id;S.Type=B.Type;S.Position=B.P;S.Size=LWPlaces::Size(S.Type);S.Yaw=B.Yaw;S.District=(B.Type==7||B.Type==8||B.Type==9)?1:2;S.PlaceName69=FString(B.Name)+TEXT(" / ")+LWPlaces::Name(S.Type);if(Clear(S))Sites.Add(S);}
}
TArrayView<const FFair73> FairBuildings(){return MakeArrayView(RawFair73);}
FVector2D Start(){return LWNY69::Project(43.0509,-76.15291)+FVector2D(-2200,-3400);}
const TCHAR* StreetAt(FVector2D P){double Best=2500;const TCHAR* Name=nullptr;for(const auto& R:RawRoads73){double D=LWGen::DistanceToSegment(P,{R.A,R.B,R.Width,R.Highway});if(D<Best){Best=D;Name=StreetNames73[R.Name];}}return Name;}
}
