#include "LWResident.h"
#include "LWWorldObject.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

struct FLWCompanionRoute {
 struct FNode{FVector At;float G,F;int Parent;bool Closed=false;};
 TArray<FNode> Nodes;TArray<int> Open;TMap<FIntVector,int> Known;
 TArray<FVector> Waypoints;int Step=0,Best=0;
 FVector Goal=FVector::ZeroVector,RequestedGoal=FVector::ZeroVector,Origin=FVector::ZeroVector,LastPosition=FVector::ZeroVector;
 double Started=0,DebugTime=0;float Retry=0,Stalled=0,Blocked=0,ProgressClock=0;bool Searching=false,Complete=false;
};
namespace {
 constexpr float Cell=70;
 FCollisionQueryParams NavQuery(ALWResident* N,bool Doors){
  FCollisionQueryParams Q(NAME_None,false,N);
  for(TActorIterator<APawn> It(N->GetWorld());It;++It)Q.AddIgnoredActor(*It);
  if(Doors)for(TActorIterator<ALWWorldObject> It(N->GetWorld());It;++It)if(It->Kind==ELWObjectKind::Door&&It->CanCompanionOpenDoor()&&It->DoorAngle<88&&N->GetWorld()->GetTimeSeconds()>=It->ManualDoorUntil)Q.AddIgnoredActor(*It);
  return Q;
 }
 bool FloorPoint(ALWResident* N,FVector Guess,FVector& Out,const FCollisionQueryParams& Q){
  const float Half=N->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),Radius=N->GetCapsuleComponent()->GetScaledCapsuleRadius()+2;
  const float Step=N->GetCharacterMovement()->MaxStepHeight;
  FHitResult H;FVector Foot=Guess-FVector(0,0,Half);
  if(!N->GetWorld()->LineTraceSingleByChannel(H,Foot+FVector(0,0,Step+6),Foot-FVector(0,0,95),ECC_Pawn,Q)||H.ImpactNormal.Z<N->GetCharacterMovement()->GetWalkableFloorZ())return false;
  Out=H.ImpactPoint+FVector(0,0,Half+3);
  return !N->GetWorld()->OverlapBlockingTestByChannel(Out+FVector(0,0,Step*.5f),FQuat::Identity,ECC_Pawn,FCollisionShape::MakeCapsule(Radius,Half-Step*.5f),Q);
 }
 bool WalkEdge(ALWResident* N,FVector A,FVector Desired,FVector& End,const FCollisionQueryParams& Q){
  const float Half=N->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),Radius=N->GetCapsuleComponent()->GetScaledCapsuleRadius()+2,Step=N->GetCharacterMovement()->MaxStepHeight;
  int Samples=FMath::Max(1,FMath::CeilToInt(FVector::Dist2D(A,Desired)/30));if(Samples>220)return false;
  FVector Previous=A;
  for(int I=1;I<=Samples;I++){FVector Guess=FMath::Lerp(A,Desired,float(I)/Samples);Guess.Z=Previous.Z;FVector Ground;
   if(!FloorPoint(N,Guess,Ground,Q)||Ground.Z-Previous.Z>Step+5||Previous.Z-Ground.Z>75)return false;
   FHitResult H;const FVector Raise(0,0,Step*.5f);
   if(N->GetWorld()->SweepSingleByChannel(H,Previous+Raise,Ground+Raise,FQuat::Identity,ECC_Pawn,FCollisionShape::MakeCapsule(Radius,Half-Step*.5f),Q))return false;
   Previous=Ground;
  }
  End=Previous;return true;
 }
 FIntVector Key(FVector At,FVector Origin){return FIntVector(FMath::RoundToInt((At.X-Origin.X)/Cell),FMath::RoundToInt((At.Y-Origin.Y)/Cell),FMath::RoundToInt((At.Z-Origin.Z)/20));}
 float Heuristic(FVector A,FVector B){return FVector::Dist(A,B);}
 void Finish(FLWCompanionRoute& R,int Node,bool Complete){R.Waypoints.Empty();for(int I=Node;I>0;I=R.Nodes[I].Parent)R.Waypoints.Insert(R.Nodes[I].At,0);R.Step=0;R.Searching=false;R.Complete=Complete;R.Retry=Complete?1.5f:2.5f;}
}
void ALWResident::ResetCompanionNavigation(){CompanionRoute.Reset();Path.Empty();}
bool ALWResident::NavigateCompanion(FVector Goal,float Speed,float Dt){
 if(!World||Riding||DownTime>0)return false;
 if(!CompanionRoute)CompanionRoute=MakeShared<FLWCompanionRoute>();auto& R=*CompanionRoute;
 const FVector Here=GetActorLocation();R.Retry-=Dt;
 static const bool Debug=FParse::Param(FCommandLine::Get(),TEXT("LWCompanionNavDebug"));
 if(Debug&&GetWorld()->GetTimeSeconds()>=R.DebugTime){
  R.DebugTime=GetWorld()->GetTimeSeconds()+1;
  UE_LOG(LogTemp,Display,TEXT("LW_NAV_DEBUG %s at=%s goal=%s searching=%d nodes=%d open=%d step=%d/%d next=%s blocked=%.2f stalled=%.2f retry=%.2f speed=%.1f"),
   *GetName(),*Here.ToString(),*Goal.ToString(),R.Searching,R.Nodes.Num(),R.Open.Num(),R.Step,R.Waypoints.Num(),
   *(R.Waypoints.IsValidIndex(R.Step)?R.Waypoints[R.Step]:FVector::ZeroVector).ToString(),R.Blocked,R.Stalled,R.Retry,GetVelocity().Size());
 }
 R.ProgressClock+=Dt;
 if(R.ProgressClock>=.35f){
  const float Moved=FVector::Dist(Here,R.LastPosition);R.LastPosition=Here;
  R.Stalled=(Moved<8&&R.Step<R.Waypoints.Num()&&!R.Searching)?R.Stalled+R.ProgressClock:0;
  R.ProgressClock=0;
 }
 FVector Arrival;auto ArrivalQ=NavQuery(this,false);if(FVector::Dist2D(Here,Goal)<105&&FMath::Abs(Here.Z-Goal.Z)<65&&WalkEdge(this,Here,Goal,Arrival,ArrivalQ)){R.Searching=false;R.Waypoints.Empty();return false;}
 // Compare the requested destination, not its floor projection/fallback. Otherwise a
 // formation point over a stairwell can restart the fallback search every frame.
 const bool GoalMoved=FVector::Dist2D(R.RequestedGoal,Goal)>140||FMath::Abs(R.RequestedGoal.Z-Goal.Z)>90;
 if(R.Nodes.IsEmpty()||(R.Retry<=0&&(GoalMoved||(!R.Searching&&R.Step>=R.Waypoints.Num())||R.Stalled>1.5f||R.Blocked>.6f))){
  R=FLWCompanionRoute();R.Origin=Here;R.Goal=R.RequestedGoal=Goal;R.LastPosition=Here;R.Started=GetWorld()->GetTimeSeconds();
  auto Q=NavQuery(this,true);FVector Ground;
  if(!FloorPoint(this,Goal,Ground,Q)){auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));if(!P||FVector::Dist2D(Goal,P->GetActorLocation())>600||!P->RPG.Crew.ContainsByPredicate([&](const auto& C){return C.Id==ResidentId&&C.Following;})||!FloorPoint(this,P->GetActorLocation(),Ground,Q)){R.Retry=2;R.Nodes.Add({Here,0,0,-1});return false;}}
  R.Goal=Ground;FVector Direct;
  if(FVector::Dist2D(Here,Ground)<1600&&WalkEdge(this,Here,Ground,Direct,Q)&&FMath::Abs(Direct.Z-Ground.Z)<30){R.Waypoints={Direct};R.Nodes.Add({Here,0,0,-1});R.Complete=true;R.Retry=1.5f;}
  else{R.Nodes.Add({Here,0,Heuristic(Here,Ground),-1});R.Open.Add(0);R.Known.Add(Key(Here,Here),0);R.Searching=true;}
 }
 if(R.Searching){
  // Persistent FIFO service breaks actor-tick ordering: a late resident gets first
  // use of the next frame after earlier residents consumed the shared time slice.
  struct FWaitingSearch{TWeakObjectPtr<ALWResident> Resident;uint64 RequestedFrame;};
  static TArray<FWaitingSearch> Waiting;
  static uint64 Frame=MAX_uint64;static int Remaining=0;static double SearchSeconds=0;
  if(Frame!=GFrameCounter){Frame=GFrameCounter;Remaining=96;SearchSeconds=0;}
  Waiting.RemoveAll([](const FWaitingSearch& W){const auto* N=W.Resident.Get();
   return !N||!N->CompanionRoute||!N->CompanionRoute->Searching||N->Riding||N->DownTime>0||GFrameCounter-W.RequestedFrame>2;
  });
  auto* Request=Waiting.FindByPredicate([this](const FWaitingSearch& W){return W.Resident.Get()==this;});
  if(Request)Request->RequestedFrame=GFrameCounter;else Waiting.Add({this,GFrameCounter});
  if(Waiting[0].Resident.Get()==this&&Remaining>0&&SearchSeconds<.003){
  const double SearchStart=FPlatformTime::Seconds();
  auto Q=NavQuery(this,true);const int Budget=FMath::Min(Remaining,32);int Used=0;
  for(int Iter=0;Iter<Budget&&R.Open.Num()&&R.Nodes.Num()<6500&&(Iter==0||SearchSeconds+FPlatformTime::Seconds()-SearchStart<.003);Iter++){
   ++Used;
   int BestIndex=0;for(int I=1;I<R.Open.Num();I++)if(R.Nodes[R.Open[I]].F<R.Nodes[R.Open[BestIndex]].F)BestIndex=I;
   int Id=R.Open[BestIndex];R.Open.RemoveAtSwap(BestIndex);if(R.Nodes[Id].Closed)continue;R.Nodes[Id].Closed=true;const auto Node=R.Nodes[Id];
   if(Heuristic(Node.At,R.Goal)<Heuristic(R.Nodes[R.Best].At,R.Goal))R.Best=Id;
   FVector FinalPoint;if(FVector::Dist2D(Node.At,R.Goal)<95&&FMath::Abs(Node.At.Z-R.Goal.Z)<35&&WalkEdge(this,Node.At,R.Goal,FinalPoint,Q)){Finish(R,Id,true);R.Waypoints.Add(FinalPoint);break;}
   for(int X=-1;X<=1;X++)for(int Y=-1;Y<=1;Y++)if(X||Y){FVector Want=Node.At+FVector(X*Cell,Y*Cell,0),Next;if(FVector::Dist2D(Want,R.Origin)>6500||!WalkEdge(this,Node.At,Want,Next,Q))continue;
    FIntVector K=Key(Next,R.Origin);float G=Node.G+FVector::Dist(Node.At,Next);int* Old=R.Known.Find(K);if(Old){auto& V=R.Nodes[*Old];if(V.Closed||G>=V.G)continue;V.G=G;V.F=G+Heuristic(Next,R.Goal);V.Parent=Id;R.Open.Add(*Old);}else{int Added=R.Nodes.Add({Next,G,G+Heuristic(Next,R.Goal),Id});R.Known.Add(K,Added);R.Open.Add(Added);}
   }
  }
  Remaining-=Used; // Charge actual work, not 32 nodes when the time slice ended early.
  SearchSeconds+=FPlatformTime::Seconds()-SearchStart;
  // Rejoin on the next request, behind every resident still waiting this frame.
  Waiting.RemoveAt(0);
  }
  // A stacked-floor goal can require a long detour away from the goal before any
  // node improves on the origin's heuristic. A time slice is not search failure:
  // retain that frontier instead of repeating the same first four seconds forever.
  const bool UsefulPartial=FVector::Dist2D(R.Origin,R.Nodes[R.Best].At)>150;
  const bool Exhausted=R.Open.IsEmpty()||R.Nodes.Num()>=6500;
  if(R.Searching&&(Exhausted||(UsefulPartial&&GetWorld()->GetTimeSeconds()-R.Started>4))){
   UE_LOG(LogTemp,Verbose,TEXT("LW_COMPANION_SEARCH %s nodes=%d open=%d age=%.2f partial=%d exhausted=%d origin=%s goal=%s"),
    *GetName(),R.Nodes.Num(),R.Open.Num(),GetWorld()->GetTimeSeconds()-R.Started,UsefulPartial,Exhausted,*R.Origin.ToString(),*R.Goal.ToString());
   if(UsefulPartial)Finish(R,R.Best,false);else {R.Searching=false;R.Retry=2.5f;}
  }
 }
 if(R.Step>=R.Waypoints.Num())return false;
 while(R.Step<R.Waypoints.Num()&&FVector::Dist2D(Here,R.Waypoints[R.Step])<40&&FMath::Abs(Here.Z-R.Waypoints[R.Step].Z)<45){
  // Reaching the waypoint's acceptance radius does not mean the corner is clear.
  // Validate the shortcut from the actual capsule position before dropping this
  // waypoint; otherwise a valid stair approach clips the wall and replans in place.
  if(R.Step+1<R.Waypoints.Num()&&FVector::Dist2D(Here,R.Waypoints[R.Step])>8){
   FVector CornerEnd;const FVector After=R.Waypoints[R.Step+1];
   if(!WalkEdge(this,Here,After,CornerEnd,ArrivalQ)||FMath::Abs(CornerEnd.Z-After.Z)>30)break;
  }
  R.Step++;
 }
 if(R.Step>=R.Waypoints.Num())return false;
 FVector Next=R.Waypoints[R.Step];auto Q=NavQuery(this,false);FHitResult Hit;
 FVector Direction=(Next-Here).GetSafeNormal2D();FVector Look=Here+Direction*FMath::Min(180.,FVector::Dist2D(Here,Next));
 if(GetWorld()->SweepSingleByChannel(Hit,Here+FVector(0,0,15),Look+FVector(0,0,15),FQuat::Identity,ECC_Pawn,FCollisionShape::MakeCapsule(32,55),Q)){
  if(auto* Door=Cast<ALWWorldObject>(Hit.GetActor());Door&&Door->Kind==ELWObjectKind::Door){if(!Door->RequestCompanionDoor(this)){
   // Allow a usable door to swing; a newly locked or manually held door must eventually replan.
   R.Stalled=0;R.Blocked+=Dt;if(!Door->CanCompanionOpenDoor()||R.Blocked>3){R.Retry=0;}return false;
  }}
 }
 // Reject newly blocked paths, cliffs and incorrect floors before applying movement.
 FVector End;auto WalkQ=NavQuery(this,false);FVector Probe=Here+Direction*FMath::Min(60.,FVector::Dist2D(Here,Next));if(!WalkEdge(this,Here,Probe,End,WalkQ)){R.Retry=0;R.Blocked+=Dt;return false;}
 R.Blocked=0;
 FVector Avoid=Direction;for(TActorIterator<ALWResident> It(GetWorld());It;++It)if(*It!=this&&!It->Riding){FVector Apart=Here-It->GetActorLocation();if(FMath::Abs(Apart.Z)<90&&Apart.Size2D()<125){Avoid+=Apart.GetSafeNormal2D()*FMath::Max(0.,1-Apart.Size2D()/100)*.7;if(FVector::DotProduct(-Apart.GetSafeNormal2D(),Direction)>.4)Avoid+=FVector(-Direction.Y,Direction.X,0)*.9;}}
 if(!Avoid.IsNearlyZero()&&WalkEdge(this,Here,Here+Avoid.GetSafeNormal2D()*50,End,WalkQ))Direction=Avoid.GetSafeNormal2D();
 GetCharacterMovement()->MaxWalkSpeed=FMath::Min(Speed,float(FVector::Dist2D(Here,Next)/FMath::Max(.016f,Dt)));
 AddMovementInput(Direction,1,true);SetActorRotation(FMath::RInterpTo(GetActorRotation(),Direction.Rotation(),Dt,4));return true;
}
