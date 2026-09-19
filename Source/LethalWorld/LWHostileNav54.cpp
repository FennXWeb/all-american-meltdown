#include "LWZombie.h"
#include "LWWorldObject.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "HAL/PlatformTime.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"

struct FLWHostileRoute54{
 struct FNode{FVector At;float G,F;int Parent;bool Closed=false;};
 TArray<FNode> Nodes;TArray<int> Open;TMap<FIntVector,int> Known;
 FVector Goal,Origin;double Retry=0,Requested=0;int Best=0;bool Searching=false;
 TArray<TWeakObjectPtr<ALWWorldObject>> Doors;
};
void ALWZombie::FindRoute(FVector Goal){
 if(bDead||!World||CrossesSafehouse(Goal,Goal))return;
 if(!HostileRoute54)HostileRoute54=MakeShared<FLWHostileRoute54>();auto& R=*HostileRoute54;
 const FVector Here=GetActorLocation();const double Now=GetWorld()->GetTimeSeconds();
 if(!R.Searching&&Now>=R.Retry&&(Path.IsEmpty()||FVector::DistSquared(R.Goal,Goal)>FMath::Square(180.f))){
  R=FLWHostileRoute54();R.Goal=Goal;R.Origin=Here;R.Nodes.Add({Here,0,float(FVector::Dist(Here,Goal)),-1});R.Open.Add(0);R.Known.Add(FIntVector::ZeroValue,0);R.Searching=true;
  if(Kind==ELWEnemyKind::Raider||Kind==ELWEnemyKind::Zombie||Kind==ELWEnemyKind::Mannequin)for(TActorIterator<ALWWorldObject> It(GetWorld());It;++It)if(It->Kind==ELWObjectKind::Door&&It->CanCompanionOpenDoor()&&FVector::DistSquared(Here,It->GetActorLocation())<FMath::Square(3200.f))R.Doors.Add(*It);
 }
 if(!R.Searching)return;R.Requested=Now;
 static TArray<TWeakObjectPtr<ALWZombie>> Queue;
 Queue.RemoveAll([Now](const auto& N){return !N.IsValid()||N->bDead||!N->HostileRoute54||!N->HostileRoute54->Searching||Now-N->HostileRoute54->Requested>.2;});Queue.AddUnique(this);
 if(Queue[0].Get()!=this)return;
 // One shared millisecond budget prevents synchronized crowds from causing search spikes.
 static uint64 Frame=0;static double Used=0;static int NodesThisFrame=0;if(Frame!=GFrameCounter){Frame=GFrameCounter;Used=0;NodesThisFrame=0;}
 if(Used>.0012||NodesThisFrame>=48)return;const double Begin=FPlatformTime::Seconds();Queue.RemoveAt(0);
 FCollisionQueryParams Q(SCENE_QUERY_STAT(LWHostileNav54),false,this);
 for(TActorIterator<APawn> It(GetWorld());It;++It)Q.AddIgnoredActor(*It);
 for(const auto& Door:R.Doors)if(Door.IsValid()&&Door->CanCompanionOpenDoor())Q.AddIgnoredActor(Door.Get());
 const float Half=GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),Radius=GetCapsuleComponent()->GetScaledCapsuleRadius(),Step=GetCharacterMovement()->MaxStepHeight;
 auto Finish=[&](int Index){Path.Empty();for(int N=Index;N>0;N=R.Nodes[N].Parent)Path.Insert(R.Nodes[N].At,0);R.Searching=false;R.Retry=Now+1.4;};
 for(int Work=0;Work<8&&R.Open.Num()&&NodesThisFrame<48&&Used+(FPlatformTime::Seconds()-Begin)<.0012;++Work){
  ++NodesThisFrame;int Best=0;for(int I=1;I<R.Open.Num();++I)if(R.Nodes[R.Open[I]].F<R.Nodes[R.Open[Best]].F)Best=I;
  int Id=R.Open[Best];R.Open.RemoveAtSwap(Best);auto Node=R.Nodes[Id];if(Node.Closed)continue;R.Nodes[Id].Closed=true;
  if(FVector::Dist(Node.At,Goal)<105){Finish(Id);break;}
  if(FVector::DistSquared(Node.At,Goal)<FVector::DistSquared(R.Nodes[R.Best].At,Goal))R.Best=Id;
  for(int Y=-1;Y<=1;++Y)for(int X=-1;X<=1;++X){if(!X&&!Y)continue;
   FVector End=Node.At+FVector(X*90,Y*90,0),Prev=Node.At;bool Clear=true;FHitResult H;
   for(int Sample=1;Sample<=4&&Clear;++Sample){FVector Guess=FMath::Lerp(Node.At,End,Sample*.25f);Guess.Z=Prev.Z;
    if(!GetWorld()->LineTraceSingleByChannel(H,Guess+FVector(0,0,Step+5-Half),Guess-FVector(0,0,Half+65),ECC_Pawn,Q)||H.ImpactNormal.Z<GetCharacterMovement()->GetWalkableFloorZ()){Clear=false;break;}
    FVector Ground=H.ImpactPoint+FVector(0,0,Half+2),Raise(0,0,Step*.5f);
    if(CrossesSafehouse(Prev,Ground)||GetWorld()->SweepSingleByChannel(H,Prev+Raise,Ground+Raise,FQuat::Identity,ECC_Pawn,FCollisionShape::MakeCapsule(Radius+1,FMath::Max(Radius+1,Half-Step*.5f)),Q)){Clear=false;break;}Prev=Ground;
   }
   if(!Clear||FVector::Dist2D(Prev,R.Origin)>3000)continue;
   FIntVector Key(FMath::RoundToInt((Prev.X-R.Origin.X)/90),FMath::RoundToInt((Prev.Y-R.Origin.Y)/90),FMath::RoundToInt((Prev.Z-R.Origin.Z)/20));
   float G=Node.G+FVector::Dist(Node.At,Prev),F=G+FVector::Dist(Prev,Goal);
   if(int* Existing=R.Known.Find(Key)){auto& Old=R.Nodes[*Existing];if(G<Old.G&&!Old.Closed){Old.G=G;Old.F=F;Old.Parent=Id;}}
   else if(R.Nodes.Num()<1800){int N=R.Nodes.Add({Prev,G,F,Id});R.Known.Add(Key,N);R.Open.Add(N);}
  }
 }
 if(R.Searching&&(R.Open.IsEmpty()||R.Nodes.Num()>=1800))Finish(R.Best);
 Used+=FPlatformTime::Seconds()-Begin;
 if(PersistentId==54002&&FParse::Param(FCommandLine::Get(),TEXT("LWUpdate54Smoke"))){static int Last=-1;if(Last!=int(Now)){Last=int(Now);UE_LOG(LogTemp,Display,TEXT("NAV54 at=%s goal=%s nodes=%d open=%d path=%d searching=%d best=%s"),*Here.ToString(),*Goal.ToString(),R.Nodes.Num(),R.Open.Num(),Path.Num(),R.Searching,*R.Nodes[R.Best].At.ToString());}}

}
