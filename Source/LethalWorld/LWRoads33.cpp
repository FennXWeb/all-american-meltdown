#include "LWRoads33.h"
#include "LWVehicle.h"
#include "LWWorld.h"
#include "Engine/World.h"
namespace LWRoads33 {
int Lanes(const LWGen::FRoad& R){return R.Highway?(R.Width>2100?6:4):R.Width>3000?0:R.Width>=700?2:1;}
TArray<FJunction> Junctions(const TArray<LWGen::FRoad>& All,int Seed){
 TArray<const LWGen::FRoad*> Roads;for(const auto& R:All)if(R.Width>=520&&R.Width<3000&&(R.B-R.A).SizeSquared()>1)Roads.Add(&R);
 TArray<FVector2D> Points;auto Add=[&](FVector2D P){if(!Points.ContainsByPredicate([&](auto Q){return P.Equals(Q,2);}))Points.Add(P);};
 for(int I=0;I<Roads.Num();I++)for(int K=I+1;K<Roads.Num();K++){auto& A=*Roads[I];auto& B=*Roads[K];FVector2D D=A.B-A.A,E=B.B-B.A;double Cross=D.X*E.Y-D.Y*E.X;if(FMath::Abs(Cross)<.01)continue;auto V=B.A-A.A;double T=(V.X*E.Y-V.Y*E.X)/Cross,U=(V.X*D.Y-V.Y*D.X)/Cross;if(T>=-.00001&&T<=1.00001&&U>=-.00001&&U<=1.00001)Add(A.A+D*FMath::Clamp(T,0.,1.));}
 TArray<FJunction> Out;for(auto P:Points){FJunction J;J.Position=P;J.Id=LWGen::Hash(FMath::RoundToInt(P.X),FMath::RoundToInt(P.Y),Seed,33001);float Widest=0;
  for(auto* R:Roads)if(LWGen::DistanceToSegment(P,*R)<2){for(auto End:{R->A,R->B})if(FVector2D::Distance(End,P)>20){auto D=(End-P).GetSafeNormal();if(!J.Arms.ContainsByPredicate([&](auto A){return FVector2D::DotProduct(A.Out,D)>.96;}))J.Arms.Add({D,R->Width});}if(R->Width>Widest){Widest=R->Width;J.Main=(R->B-R->A).GetSafeNormal();}}
  if(J.Arms.Num()<3)continue;J.Radius=Widest*.5f+160;J.Signals=Widest>=1000&&J.Arms.Num()>=3&&J.Arms.Num()<=4;for(const auto& A:J.Arms){double Alignment=FMath::Abs(FVector2D::DotProduct(A.Out,J.Main));if(Alignment>.259&&Alignment<.965)J.Signals=false;}
  // Very close crossings share a protected junction footprint; no conflicting signal installations.
  if(Out.ContainsByPredicate([&](const auto& O){return FVector2D::Distance(P,O.Position)<FMath::Max(J.Radius,O.Radius)*1.3;}))continue;
  Out.Add(J);
 }return Out;
}
int Phase(const FJunction& J,FVector2D A,double Seconds){const bool Main=FMath::Abs(FVector2D::DotProduct(A.GetSafeNormal(),J.Main))>.72;double T=FMath::Fmod(FMath::Max(0.,Seconds)+J.Id%68,68.);if(!Main)T=FMath::Fmod(T+34,68.);return T<28?2:T<32?1:0;}
bool NeedsStop(const FJunction& J,FVector2D Approach){
 if(J.Signals)return false;
 // Prefer the widest continuous road. At T junctions the stem must yield even
 // when its segment happens to be first in the generator's road array.
 FVector2D Priority=J.Main;float Best=-1;
 for(const auto& A:J.Arms)for(const auto& B:J.Arms)if(FVector2D::DotProduct(A.Out,B.Out)<-.90){
  float Width=FMath::Min(A.Width,B.Width);if(Width>Best+.1f){Best=Width;Priority=A.Out;}
 }
 return FMath::Abs(FVector2D::DotProduct(Approach.GetSafeNormal(),Priority))<.90;
}
bool InJunction(FVector2D P,const TArray<FJunction>& J,float Extra){return J.ContainsByPredicate([&](const auto& X){return FVector2D::Distance(P,X.Position)<X.Radius+Extra;});}
float TrafficSpeed(ALWVehicle* V,float Desired,float Dt){
 if(!V||!V->World)return Desired;
 struct FState{uint32 Stop=0;float Wait=0;uint32 Cleared=0;FVector2D Center;};static TMap<TWeakObjectPtr<ALWVehicle>,FState> States;
 for(auto I=States.CreateIterator();I;++I)if(!I.Key().IsValid())I.RemoveCurrent();auto& State=States.FindOrAdd(V);
 FVector2D Here(V->GetActorLocation()),Forward(V->GetActorForwardVector());if(State.Cleared&&FVector2D::Distance(Here,State.Center)>3500)State.Cleared=0;
 float Result=Desired;for(auto& Pair:V->World->Chunks)if(Pair.Value)for(const auto& J:Pair.Value->RoadJunctions){
  FVector2D Delta=Here-J.Position;double Distance=Delta.Size();if(Distance>5000||FVector2D::DotProduct(-Delta.GetSafeNormal(),Forward)<.65)continue;
  const FApproach* Arm=nullptr;double Dot=.7;for(const auto& A:J.Arms){double D=FVector2D::DotProduct(A.Out,Delta.GetSafeNormal());if(D>Dot){Dot=D;Arm=&A;}}if(!Arm)continue;
  const double Gap=FVector2D::DotProduct(Delta,Arm->Out)-J.Radius-V->Spec().HalfLength-80;if(Gap < -100)continue;
  bool Stop=J.Signals?Phase(J,Arm->Out,V->GetWorld()->GetTimeSeconds())!=2:NeedsStop(J,Arm->Out)&&State.Cleared!=J.Id;
  if(!J.Signals&&Stop){if(State.Stop!=J.Id){State.Stop=J.Id;State.Wait=0;}if(Gap<180&&FMath::Abs(V->Speed)<20){State.Wait+=Dt;if(State.Wait>=1.5){State.Cleared=J.Id;State.Center=J.Position;Stop=false;}}}
  if(Stop)Result=FMath::Min(Result,float(FMath::Sqrt(2*V->Spec().Brake*.65*FMath::Max(0.,Gap-30))));
 }return Result;
}
}
