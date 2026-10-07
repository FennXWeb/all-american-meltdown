#include "LWCampaign76Corridor.h"
#include "LWGeneration.h"

void LWCampaign76::FinishApproach78(const TArray<LWGen::FSite>& Sites,TArray<LWGen::FRoad>& Roads){
 TArray<LWGen::FSite> Lots;FVector2D Center;
 for(const auto& S:Sites)if(S.Id==0xEC760000u||(S.Id>=0xEC78A000u&&S.Id<0xEC78A004u)){Lots.Add(S);if(S.Id==0xEC760000u)Center=S.Position;}
 if(Lots.Num()!=5)return;
 const FVector2D A=Center+FVector2D(-7500,-2100),B=Center+FVector2D(7500,-2100);
 // Restore the small public street after the coarse landmark bypass pass.
 Roads.Add({A,B,600,false});for(const auto& S:Lots){const auto Door=LWGen::Entrance(S);Roads.Add({Door,{Door.X,Center.Y-2100},320,false});}
 TArray<LWGen::FRoad> Out;Out.Reserve(Roads.Num()+20);
 for(const auto& Road:Roads){
  if(!Lots.ContainsByPredicate([&](const auto& S){return LWGen::RoadOverlaps(S,Road);})){Out.Add(Road);continue;}
  const double Margin=Road.Width*.5+135;
  auto Clear=[&](FVector2D X,FVector2D Y){return !Lots.ContainsByPredicate([&](const auto& S){return LWGen::CutsBox(X,Y,S.Position,S.Size*.5+FVector2D(Margin));});};
  auto Outside=[&](FVector2D P){for(const auto& S:Lots){auto Q=P-S.Position;const auto H=S.Size*.5+FVector2D(Margin);if(FMath::Abs(Q.X)<H.X&&FMath::Abs(Q.Y)<H.Y){if(H.X-FMath::Abs(Q.X)<H.Y-FMath::Abs(Q.Y))Q.X=(Q.X>=0?1:-1)*(H.X+2);else Q.Y=(Q.Y>=0?1:-1)*(H.Y+2);P=S.Position+Q;}}return P;};
  TArray<FVector2D> Nodes{Outside(Road.A),Outside(Road.B)};
  for(const auto& S:Lots)for(auto Corner:{FVector2D(-1,-1),FVector2D(1,-1),FVector2D(1,1),FVector2D(-1,1)})Nodes.Add(S.Position+Corner*(S.Size*.5+FVector2D(Margin+2)));
  TArray<double> Cost;Cost.Init(DBL_MAX,Nodes.Num());Cost[0]=0;TArray<int> Previous;Previous.Init(-1,Nodes.Num());TArray<bool> Used;Used.Init(false,Nodes.Num());
  for(int Step=0;Step<Nodes.Num();Step++){int From=-1;for(int I=0;I<Nodes.Num();I++)if(!Used[I]&&(From<0||Cost[I]<Cost[From]))From=I;if(From<0||Cost[From]==DBL_MAX)break;Used[From]=true;if(From==1)break;
   for(int To=0;To<Nodes.Num();To++)if(To!=From&&Clear(Nodes[From],Nodes[To])){double Next=Cost[From]+FVector2D::Distance(Nodes[From],Nodes[To]);if(Next<Cost[To]){Cost[To]=Next;Previous[To]=From;}}
  }
  for(int To=1;Previous[To]>=0;){const int From=Previous[To];if(FVector2D::Distance(Nodes[From],Nodes[To])>1)Out.Add({Nodes[From],Nodes[To],Road.Width,Road.Highway});To=From;}
 }
 Roads=MoveTemp(Out);
}
