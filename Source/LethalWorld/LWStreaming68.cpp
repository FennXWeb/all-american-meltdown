#include "LWAircraft84.h"
#include "LWGeography84.h"
#include "LWStreaming68.h"
#include "LWDungeon.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWVehicle.h"
#include "LWGraphics33.h"
#include "Async/Async.h"
#include "ProceduralMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "LWStreamAssets68.inl"

namespace {
void HoldPopulation(ALWChunk* C){
 for(auto& Ref:C->Residents)if(auto* A=Ref.Get();IsValid(A)&&(Cast<ACharacter>(A)||Cast<ALWVehicle>(A)||Cast<ALWDungeon>(A))){
  if(!C->Ready68){
   if(A->ActorHasTag(TEXT("ChunkHeld68")))continue;A->Tags.Add(TEXT("ChunkHeld68"));
   if(A->IsActorTickEnabled())A->Tags.Add(TEXT("ChunkTick68"));if(A->GetActorEnableCollision())A->Tags.Add(TEXT("ChunkCollision68"));if(A->IsHidden())A->Tags.Add(TEXT("ChunkHidden68"));
   A->SetActorTickEnabled(false);A->SetActorEnableCollision(false);A->SetActorHiddenInGame(true);
   if(auto* N=Cast<ACharacter>(A))if(N->GetCharacterMovement()->IsComponentTickEnabled()){A->Tags.Add(TEXT("ChunkMove68"));N->GetCharacterMovement()->SetComponentTickEnabled(false);}
  }else if(A->ActorHasTag(TEXT("ChunkHeld68"))){
   A->SetActorTickEnabled(A->ActorHasTag(TEXT("ChunkTick68")));A->SetActorEnableCollision(A->ActorHasTag(TEXT("ChunkCollision68")));A->SetActorHiddenInGame(A->ActorHasTag(TEXT("ChunkHidden68")));
   if(auto* N=Cast<ACharacter>(A))if(A->ActorHasTag(TEXT("ChunkMove68")))N->GetCharacterMovement()->SetComponentTickEnabled(true);
   for(FName Tag:{FName(TEXT("ChunkHeld68")),FName(TEXT("ChunkTick68")),FName(TEXT("ChunkCollision68")),FName(TEXT("ChunkHidden68")),FName(TEXT("ChunkMove68"))})A->Tags.Remove(Tag);
  }
 }
}
}

TSharedPtr<FLWChunkPlan68> LWStreaming68::Plan(FIntPoint C,int Seed,int Town,int Parcel,float Ruggedness){
 // Only value data enters the worker. Its deterministic caches and settings are thread local.
 LWGen::TownDensity=Town;LWGen::ParcelLevel=Parcel;LWGen::Ruggedness=Ruggedness;
 auto P=MakeShared<FLWChunkPlan68>();const FVector2D O(C.X*LWGen::ChunkSize,C.Y*LWGen::ChunkSize);
 P->Neighborhood=LWGen::Neighborhood38(O+FVector2D(LWGen::ChunkSize*.5),Seed);P->Roads=P->Neighborhood.Roads;P->Sites=P->Neighborhood.Sites;
 constexpr int Grid=24;P->Vertices.Reserve(625);P->Normals.Reserve(625);P->UV.Reserve(625);P->Triangles.Reserve(3456);
 for(int Y=0;Y<=Grid;Y++)for(int X=0;X<=Grid;X++){
  FVector2D Local(X*LWGen::ChunkSize/Grid,Y*LWGen::ChunkSize/Grid),At=O+Local;
  P->Vertices.Add(FVector(Local,LWGen::Height(At,P->Roads,P->Sites)));P->UV.Add(At/450.);
  float DX=LWGen::Height(At+FVector2D(40,0),P->Roads,P->Sites)-LWGen::Height(At-FVector2D(40,0),P->Roads,P->Sites);
  float DY=LWGen::Height(At+FVector2D(0,40),P->Roads,P->Sites)-LWGen::Height(At-FVector2D(0,40),P->Roads,P->Sites);
  P->Normals.Add(FVector(-DX,-DY,80).GetSafeNormal());
  if(X<Grid&&Y<Grid){int I=Y*(Grid+1)+X;P->Triangles.Append({I,I+Grid+1,I+1,I+1,I+Grid+1,I+Grid+2});}
 }
 return P;
}
bool ALWChunk::FlushInstances68(){
 if(PendingInstances68.IsEmpty())return true;
 auto It=PendingInstances68.CreateIterator();auto* B=Batches.FindRef(It.Key()).Get();
 if(B){B->AddInstances(It.Value(),false,false,false);if(!B->IsRegistered())B->RegisterComponent();}
 It.RemoveCurrent();return PendingInstances68.IsEmpty();
}
bool ALWChunk::BuildStep68(ALWWorld* W){
 if(!Plan68)return true;const auto& Roads=Plan68->Roads;const auto& Sites=Plan68->Sites;const FVector Origin=GetActorLocation();
 BuildingSurfaces=true;BufferInstances68=true;
 if(Stage68>=4&&!Plan68->Geometry.IsEmpty()){auto Work=MoveTemp(Plan68->Geometry[0]);Plan68->Geometry.RemoveAt(0,EAllowShrinking::No);Work();return false;}
 if(Stage68==0){auto& P=*Plan68;Terrain->CreateMeshSection(0,P.Vertices,P.Triangles,P.Normals,P.UV,TArray<FColor>(),TArray<FProcMeshTangent>(),true);Terrain->SetMaterial(0,W->Material(LWGeography84::Canada(FVector2D(Origin))?TEXT("CanadaGrass51"):TEXT("Earth")));Stage68++;return false;}
 if(Plan68->Warm&&!Plan68->Warm->HasLoadCompleted()){if(SyncCollision68)Plan68->Warm->WaitUntilComplete();else return false;}
 if(Stage68==1){if(Plan68->Vertices.ContainsByPredicate([](const FVector& V){return V.Z<-100;}))Box(W,TEXT("RV66_Water"),FVector(LWGen::ChunkSize*.5,LWGen::ChunkSize*.5,-75),FVector(LWGen::ChunkSize,LWGen::ChunkSize,6),FRotator::ZeroRotator,false);BuildRoads33(W,Roads,Sites);LWAviation84::Ground(this,W);Stage68++;return false;}
 if(Stage68==2){
  auto* P84=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(W,0));if(P84&&P84->Vehicle&&P84->Vehicle->IsAircraft84()&&P84->GetActorLocation().Z>65000){HighFlight84=true;Stage68=6;return false;}
  if(!Plan68->Geometry.IsEmpty()){auto Work=MoveTemp(Plan68->Geometry[0]);Plan68->Geometry.RemoveAt(0,EAllowShrinking::No);Work();return false;}
  while(SiteIndex68<Sites.Num()){auto S=Sites[SiteIndex68++];if(S.SettlementBuilding||LWGen::ChunkAt(S.Position)!=Coordinate)continue;
   if(S.Canadian)BuildingCanada68(W,S);else Building(W,S);return false;
  }Stage68++;SiteIndex68=0;return false;
 }
 if(Stage68==3){
  LWBorder51::Build(this,W);if(LWGeography84::Canada(FVector2D(Origin))){Stage68++;return false;}
  while(SiteIndex68<Sites.Num()){const auto S=Sites[SiteIndex68++];if(LWGen::ChunkAt(S.Position)!=Coordinate)continue;
   FRotator Facing(0,S.Yaw,0);FVector Front=FVector(S.Position,0)-Origin+Facing.RotateVector(FVector(0,-S.Size.Y*.5-150,0));
   if(!LWPlaces::IsTower(S.Type)){StreetLight(W,Front+Facing.RotateVector(FVector(S.Size.X*.5-100,0,0)),Facing);if(S.District==2||S.Type==5||S.Type==19)StreetLight(W,Front+FVector(0,0,310),Facing,true);}
   for(int Side:{-1,1})Add(W,TEXT("WasteBinV13"),NAME_None,Front+Facing.RotateVector(FVector(Side*(S.Size.X*.5-280),0,0)),FVector(1),Facing);return false;
  }Stage68++;return false;
 }
 if(Stage68==4){W->SpawnRecords(this);Stage68++;return false;}
 if(Stage68==5){
  if(!LWGeography84::Canada(FVector2D(Origin))){FRandomStream Rand(LWGen::Hash(Coordinate.X,Coordinate.Y,W->Seed,31));for(int I=0;I<42;I++){
   FVector2D Local(Rand.FRandRange(0,LWGen::ChunkSize),Rand.FRandRange(0,LWGen::ChunkSize)),P=FVector2D(Origin)+Local;
   if(LWNY69::WaterDepth(P)>0||LWStory::Reserved(P)||Roads.ContainsByPredicate([&](const auto& R){return LWGen::DistanceToSegment(P,R)<R.Width*.5+450;})||Sites.ContainsByPredicate([&](const auto& S){return (P-S.Position).Size()<S.Size.Size()*.5+250;}))continue;
   Add(W,I%4?TEXT("DeadTree"):TEXT("Rubble"),NAME_None,FVector(Local,LWGen::Height(P,Roads,Sites)),FVector(Rand.FRandRange(.55,1.6)),FRotator(0,Rand.FRandRange(0,360),0),I%4!=0);
  }Wilderness78(W,Roads,Sites);}Stage68++;return false;
 }
 if(Stage68==6){if(!Plan68->Furnishing.IsEmpty()){if(Plan68->Furnishing[0]())Plan68->Furnishing.RemoveAt(0,EAllowShrinking::No);return false;}if(!FlushSurface68())return false;Stage68++;return false;}
 if(Stage68==7){if(!FlushInstances68())return false;Stage68++;return false;}
 if(Stage68==8){
  if(HighFlight84){Stage68=9;return false;}
  if(!SyncCollision68&&!Terrain->BodyInstance.IsValidBodyInstance())return false;
  for(const auto& Pair:SurfaceMeshes)if(auto* M=Pair.Value.Get();M&&M->GetCollisionEnabled()!=ECollisionEnabled::NoCollision&&M->GetProcMeshSection(0)&&!M->BodyInstance.IsValidBodyInstance())return false;
  if(!Plan68->Population.IsEmpty()){auto Work=MoveTemp(Plan68->Population[0]);Plan68->Population.RemoveAt(0,EAllowShrinking::No);Plan68->RunningPopulation=true;Work();Plan68->RunningPopulation=false;return false;}
  if(!Plan68->WildernessQueued68){Plan68->WildernessQueued68=true;Plan68->RunningPopulation=true;W->SpawnEnemies(this,nullptr);W->SpawnRoamingBoss(this);Plan68->RunningPopulation=false;return false;}Stage68++;return false;
 }
 BuildingSurfaces=false;BufferInstances68=false;Plan68.Reset();Ready68=true;return true;
}
void ALWWorld::Stream68(FVector P,bool Immediate){
 const FIntPoint C=LWGen::ChunkAt(FVector2D(P));auto* Player=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
 const FVector Velocity=Player&&Player->Vehicle?Player->Vehicle->GetVelocity():Player?Player->GetVelocity():FVector::ZeroVector;
 FVector Prediction=Velocity*4; // Four seconds of flight lookahead, capped to contain memory.
 if(Player&&Player->Vehicle)Prediction=Player->Vehicle->IsHelicopter57()?Player->Vehicle->Velocity57*4:Player->Vehicle->GetActorForwardVector()*Player->Vehicle->Speed*4;
 Prediction=Prediction.GetClampedToMaxSize(LWGen::ChunkSize*2);
 const FIntPoint Ahead=LWGen::ChunkAt(FVector2D(P+Prediction));
 const bool HighFlight84=Player&&Player->Vehicle&&Player->Vehicle->IsAircraft84()&&P.Z>65000;
 const int WantedRadius=HighFlight84?1:LWGraphics33::Radius58();
 if(C!=LastCenter||Ahead!=LastAhead68||WantedRadius!=RenderRadius||Immediate){
  LastCenter=C;LastAhead68=Ahead;RenderRadius=WantedRadius;Queue.Empty();RequiredChunks68.Empty();
  for(int Y=-RenderRadius;Y<=RenderRadius;Y++)for(int X=-RenderRadius;X<=RenderRadius;X++){Queue.AddUnique(C+FIntPoint(X,Y));Queue.AddUnique(Ahead+FIntPoint(X,Y));}
  for(const auto& K:Queue)RequiredChunks68.Add(K);
  Queue.RemoveAll([&](const auto& K){return Chunks.Contains(K)||Jobs68.Contains(K);});
  Queue.Sort([C,Ahead](const auto& A,const auto& B){return (A-C).SizeSquared()+.3*(A-Ahead).SizeSquared()<(B-C).SizeSquared()+.3*(B-Ahead).SizeSquared();});
  TArray<FIntPoint> Remove;for(const auto& Pair:Chunks)if((Pair.Value->HighFlight84&&!HighFlight84)||(FMath::Max(FMath::Abs(Pair.Key.X-C.X),FMath::Abs(Pair.Key.Y-C.Y))>RenderRadius+2&&FMath::Max(FMath::Abs(Pair.Key.X-Ahead.X),FMath::Abs(Pair.Key.Y-Ahead.Y))>RenderRadius+1&&!Pair.Value->MajorBounds.IsInside(FVector2D(P))))Remove.Add(Pair.Key);
  for(auto K:Remove){auto* Old=Chunks[K].Get();Old->SetActorTickEnabled(false);Old->SetActorHiddenInGame(true);Old->SetActorEnableCollision(false);Retiring68.Add(Old);Chunks.Remove(K);if(!HighFlight84&&Old->HighFlight84)Queue.AddUnique(K);} // Destruction is handled in small batches below.
 }
 if(Immediate)for(auto& Pair:Chunks)if(auto* Chunk=Pair.Value.Get();Chunk->Plan68&&!Chunk->SyncCollision68){
  Chunk->SyncCollision68=true;Chunk->Terrain->bUseAsyncCooking=false;
  if(Chunk->Stage68>0){const auto& Q=*Chunk->Plan68;Chunk->Terrain->CreateMeshSection(0,Q.Vertices,Q.Triangles,Q.Normals,Q.UV,TArray<FColor>(),TArray<FProcMeshTangent>(),true);}
  for(auto& Surface:Chunk->SurfaceMeshes){Surface.Value->bUseAsyncCooking=false;Chunk->DirtySurfaces68.Add(Surface.Key);}
  if(Chunk->Stage68>6)Chunk->Stage68=6;
 }
 const double Start=FPlatformTime::Seconds();
 do{
  while(!Queue.IsEmpty()&&Jobs68.Num()<3){const auto K=Queue[0];Queue.RemoveAt(0,EAllowShrinking::No);if(Chunks.Contains(K)||Jobs68.Contains(K))continue;auto J=MakeShared<FLWChunkJob68>();const int JobSeed=Seed,Town=LWGen::TownDensity,Parcel=LWGen::ParcelLevel;const float Rugged=LWGen::Ruggedness;
   J->Future=Async(EAsyncExecution::ThreadPool,[K,JobSeed,Town,Parcel,Rugged](){return LWStreaming68::Plan(K,JobSeed,Town,Parcel,Rugged);});Jobs68.Add(K,J);
  }
  TArray<FIntPoint> Done;for(auto& Pair:Jobs68)if(Pair.Value->Future.IsReady())Done.Add(Pair.Key);
  for(auto K:Done){auto Plan=Jobs68[K]->Future.Get();Jobs68.Remove(K);LWGen::PublishNeighborhood68(Plan->Neighborhood);
   for(const auto& S:Plan->Sites){const FVector2D D=FVector2D(P)-S.Position;if(S.Size.GetMax()>LWGen::ChunkSize&&FMath::Abs(D.X)<S.Size.X*.5+4000&&FMath::Abs(D.Y)<S.Size.Y*.5+4000){auto SiteChunk=LWGen::ChunkAt(S.Position);RequiredChunks68.Add(SiteChunk);if(SiteChunk!=K&&!Chunks.Contains(SiteChunk)&&!Jobs68.Contains(SiteChunk))Queue.AddUnique(SiteChunk);}}
   if(!RequiredChunks68.Contains(K)&&FMath::Max(FMath::Abs(K.X-C.X),FMath::Abs(K.Y-C.Y))>RenderRadius+2&&FMath::Max(FMath::Abs(K.X-Ahead.X),FMath::Abs(K.Y-Ahead.Y))>RenderRadius+1)continue;
   TSet<FSoftObjectPath> Paths;AddWarmBundle68(0,Paths);if(K.X*LWGen::ChunkSize>=LWBorder51::Strip&&K.X*LWGen::ChunkSize<LWBorder51::North)AddWarmBundle68(2,Paths);
   for(const auto& S:Plan->Sites)if(LWGen::ChunkAt(S.Position)==K){int Group=S.Canadian?2:S.Type>=68?69:S.Type>=64?64:S.Type>=58?58:S.Type>=53?53:S.Type>=33?33:S.Type==32?32:S.Type>=22?22:S.Type==21?21:LWPlaces::IsTower(S.Type)?20:(S.Type==5||S.Type==11||S.Type==17)?5:1;AddWarmBundle68(Group,Paths);}
   Plan->Warm=UAssetManager::GetStreamableManager().RequestAsyncLoad(Paths.Array());
   auto* Chunk=GetWorld()->SpawnActor<ALWChunk>(FVector(K.X*LWGen::ChunkSize,K.Y*LWGen::ChunkSize,0),FRotator::ZeroRotator);if(Chunk){Chunk->Coordinate=K;Chunk->SyncCollision68=Immediate;Chunk->Terrain->bUseAsyncCooking=!Immediate;Chunk->Plan68=Plan;Chunks.Add(K,Chunk);}
  }
  ALWChunk* Next=nullptr;double Best=DBL_MAX;
  for(auto& Pair:Chunks)if(auto* Chunk=Pair.Value.Get();Chunk->Plan68){if(!Immediate&&Chunk->Stage68>0&&Chunk->Plan68->Warm&&!Chunk->Plan68->Warm->HasLoadCompleted())continue;double Score=(Pair.Key-C).SizeSquared()+.3*(Pair.Key-Ahead).SizeSquared()-(Chunk->Stage68==0?1000:Chunk->Stage68==1?500:0);if(Score<Best){Best=Score;Next=Chunk;}}
  if(Next){const double T=FPlatformTime::Seconds();Next->BuildStep68(this);HoldPopulation(Next);const double Ms=(FPlatformTime::Seconds()-T)*1000;StreamMaxStep68=FMath::Max(StreamMaxStep68,Ms);if(Ms>20)UE_LOG(LogTemp,Verbose,TEXT("STREAM68 stage=%d chunk=%s %.1f ms"),Next->Stage68,*Next->Coordinate.ToString(),Ms);}
  else if(Immediate&&Jobs68.Num()){Jobs68.CreateIterator().Value()->Future.Wait();continue;}
  if(!Next&&(!Immediate||(Queue.IsEmpty()&&Jobs68.IsEmpty())))break;
 }while(Immediate||FPlatformTime::Seconds()-Start<.003);
 // Unload residents/components over frames; do not destroy a town in a single tick.
 if(!Retiring68.IsEmpty()){
  auto* Old=Retiring68[0].Get();if(!IsValid(Old)){Retiring68.RemoveAt(0);return;}
  const double Deadline=FPlatformTime::Seconds()+.001;
  while(!Old->Residents.IsEmpty()&&FPlatformTime::Seconds()<Deadline){auto* A=Old->Residents.Pop(EAllowShrinking::No).Get();if(auto* Child=Cast<ALWChunk>(A)){Child->SetActorHiddenInGame(true);Child->SetActorEnableCollision(false);Child->SetActorTickEnabled(false);Retiring68.Add(Child);}else if(IsValid(A))A->Destroy();}
  if(Old->Residents.IsEmpty()){
   TArray<UActorComponent*> Components;Old->GetComponents(Components);
   int Count=0;for(auto* Component:Components)if(Component&&Component!=Old->Terrain&&Count++<4)Component->DestroyComponent();
   if(Count<=4){Old->Destroy();Retiring68.RemoveAt(0);}
  }
 }
}
