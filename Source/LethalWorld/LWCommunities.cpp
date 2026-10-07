#include "LWWorld.h"
#include "LWCommunities84.h"
#include "LWResident.h"
#include "LWCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "LWWorldTextComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
void ALWWorld::SpawnSettlement(ALWChunk* Chunk,FIntPoint R){
 if(LWCommunities84::Spawn(this,Chunk,R))return;
 if(!LWGen::HasTown(R,Seed))return;
 const FVector2D Hub=LWGen::Hub(R,Seed)+FVector2D(0,-5000);if(LWGen::ChunkAt(Hub)!=Chunk->Coordinate||LWStory::Reserved(Hub,5500))return;
 const FVector Origin=Chunk->GetActorLocation();
 auto* Player=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));if(!Player)return;if(!Player->World)Player->World=this;
 auto& Town=Player->EnsureSettlement(R);const FName TownId(*FString::Printf(TEXT("town_hostile_%d_%d"),R.X,R.Y));
 const int TownSize=Town.Size;
 TArray<LWGen::FRoad> Roads,AllRoads;TArray<LWGen::FSite> Sites,FinalSites;LWGen::Region(R,Seed,Roads,Sites);LWGen::Gather(Hub,Seed,AllRoads,FinalSites);
 Sites.RemoveAll([&](const auto& S){return !FinalSites.ContainsByPredicate([&](const auto& O){return O.Id==S.Id;});});
 auto B=[&](FVector P,FVector Size,FName Mat){
  const bool Fence=Size.Z>250&&((Size.X<40&&Size.Y>1000)||(Size.Y<40&&Size.X>1000));
  if(!Fence){Chunk->Box(this,Mat,FVector(Hub,12)+P-Origin,Size);return;}
  const bool AlongX=Size.X>Size.Y;const float Length=AlongX?Size.X:Size.Y;const int Pieces=FMath::CeilToInt(Length/200);const float Span=Length/Pieces;
  for(int I=0;I<Pieces;I++){FVector At=P+(AlongX?FVector(1,0,0):FVector(0,1,0))*(-Length*.5+(I+.5)*Span);FVector2D World=Hub+FVector2D(At);
   if(AllRoads.ContainsByPredicate([&](const auto& Road){return LWGen::DistanceToSegment(World,Road)<Road.Width*.5+180+Span*.5;}))continue;
   FVector Piece=Size;if(AlongX)Piece.X=Span;else Piece.Y=Span;Chunk->Box(this,Mat,FVector(Hub,12)+At-Origin,Piece);
  }
 };
 for(const auto& S:Sites)if(S.SettlementBuilding){int First=Chunk->Residents.Num();Chunk->Building(this,S);for(int I=First;I<Chunk->Residents.Num();I++){if(auto* N=Cast<ALWResident>(Chunk->Residents[I])){N->SettlementId=TownId;N->DisplayName=Player->SettlerName(TownId,N->ResidentId);}if(auto* O=Cast<ALWWorldObject>(Chunk->Residents[I]))if(auto* Storage=Containers.Find(O->RecordId))Storage->Settlement=TownId;}}
 // A gated perimeter for communities; the central road stays open.
 if(TownSize==1){for(int Side:{-1,1}){B(FVector(Side*5000,0,160),FVector(30,10400,320),TEXT("CorrugatedV7"));if(Side<0)B(FVector(0,-5200,160),FVector(10000,30,320),TEXT("CorrugatedV7"));else for(int E:{-1,1})B(FVector(E*2850,5200,160),FVector(4300,30,320),TEXT("CorrugatedV7"));} /* entrance cut into north wall */
}
 if(TownSize==0){for(int I=0;I<4;I++){FVector At(-1800+I*1100,-1800,12);B(At+FVector(0,0,100),FVector(260,220,200),TEXT("Cloth"));B(At+FVector(0,-120,170),FVector(300,25,35),TEXT("Wood"));}}
 auto* Sign=NewObject<ULWWorldTextComponent>(Chunk);Sign->SetupAttachment(Chunk->GetRootComponent());Sign->SetRelativeLocation(FVector(Hub+FVector2D(0,1500),350)-Origin);Sign->SetRelativeRotation(FRotator(0,90,0));Sign->SetText(FText::FromString(Town.Name));Sign->SetWorldSize(65);Sign->SetHorizontalAlignment(EHTA_Center);Sign->RegisterComponent();
 // Open settlement boundaries preserve access from the generated roads.
 const TCHAR* Roles[]={TEXT("merchant"),TEXT("warden"),TEXT("medic"),TEXT("recruit"),TEXT("recruit")};const TCHAR* Names[]={TEXT("MARA / QUARTERMASTER"),TEXT("WARDEN VALE"),TEXT("DOC ORREN"),TEXT("ASH"),TEXT("ROOK")};
 auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
 for(int I=0;I<5;I++){
 FName Id(*FString::Printf(TEXT("resident_%d_%d_%d"),R.X,R.Y,I));if(P&&P->RPG.Crew.ContainsByPredicate([&](const auto& C){return C.Id==Id;}))continue;
 bool Exists=false;for(TActorIterator<ALWResident> N(GetWorld());N;++N)if(N->ResidentId==Id){Exists=true;break;}if(Exists)continue;
 FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
 auto* N=GetWorld()->SpawnActor<ALWResident>(FVector(Hub+FVector2D(-1500+I*700,-300),110),FRotator(0,-90,0),Params);
 if(N){N->ConfigureResident(Id,Roles[I],Names[I],I);N->SettlementId=TownId;N->DisplayName=Player->SettlerName(TownId,N->ResidentId);N->ActivitySpots={N->Home,N->Home,FVector(Hub+FVector2D(-1900,-520),110),FVector(Hub+FVector2D(1600,900),110),FVector(Hub+FVector2D(-600,-900),110)};N->ActivityTime=6+I*3;Chunk->Residents.Add(N);}
 }
 // Market awnings, benches, repair bays and light poles give the routines physical destinations.
 auto Model=[&](FName Mesh,FVector At,float Yaw=0){Chunk->Add(this,Mesh,NAME_None,FVector(Hub,12)+At-Origin,FVector(1),FRotator(0,Yaw,0));};
 for(int Side:{-1,1}){
  for(int X:{-1,1})B(FVector(Side*1900+X*280,-700,135),FVector(14,14,270),TEXT("Steel"));B(FVector(Side*1900,-700,278),FVector(650,600,18),Side<0?TEXT("Cloth"):TEXT("Rust"));
  Model(TEXT("Desk"),FVector(Side*1900,-700,24));Model(TEXT("Crate"),FVector(Side*2100,-950,24));Model(TEXT("Crate"),FVector(Side*1800,-950,24));
  Model(TEXT("ChairV3"),FVector(Side*1400,900,24),Side<0?90:-90);Model(TEXT("Desk"),FVector(Side*1600,1100,24));Model(TEXT("Generator"),FVector(Side*2600,1100,12));
  auto* Lamp=GetWorld()->SpawnActor<AActor>(FVector(Hub+FVector2D(Side*2900,-100),340),FRotator::ZeroRotator);auto* L=NewObject<UPointLightComponent>(Lamp);Lamp->SetRootComponent(L);L->SetWorldLocation(FVector(Hub+FVector2D(Side*2900,-100),340));L->SetIntensity(9000);L->SetAttenuationRadius(1900);L->SetLightColor(FLinearColor(1,.72f,.4f));L->SetCastShadows(false);L->RegisterComponent();Chunk->Residents.Add(Lamp);B(FVector(Side*2900,-100,170),FVector(18,18,340),TEXT("Steel"));
 }
 const TCHAR* CivNames[]={TEXT("JUNE"),TEXT("ELI"),TEXT("NESS"),TEXT("OTTO"),TEXT("SABLE"),TEXT("FINN")};
 for(int I=0;I<(TownSize==0?3:TownSize==1?9:18);I++){FName Id(*FString::Printf(TEXT("citizen_%d_%d_%d"),R.X,R.Y,I));bool Exists=false;for(TActorIterator<ALWResident> N(GetWorld());N;++N)if(N->ResidentId==Id)Exists=true;if(Exists)continue;
  auto* N=GetWorld()->SpawnActor<ALWResident>(FVector(Hub+FVector2D(-2100+(I%6)*800,350+(I/6)*300),110),FRotator::ZeroRotator);if(N){N->ConfigureResident(Id,TEXT("civilian"),CivNames[I%6],I);N->SettlementId=TownId;N->DisplayName=Player->SettlerName(TownId,N->ResidentId);N->ActivitySpots={N->Home,FVector(Hub+FVector2D(2100,-200),110),FVector(Hub+FVector2D(-1900,-520),110),FVector(Hub+FVector2D(1400,650),110),FVector(Hub+FVector2D(0,-900),110)};N->Activity=I%5;N->ActivityTime=1+I*2;N->ChatterTime=5+I*4;Chunk->Residents.Add(N);}
 }
 // Settlement notice board and player-facing services sit in the central courtyard.
 for(int I=0;I<3;I++){auto* F=SpawnObject(ELWObjectKind::Furniture,FName(*FString::Printf(TEXT("camp_%d_%d_%d"),R.X,R.Y,I)),FVector(Hub+FVector2D(-600+I*600,-1100),25));F->SetFurniture(I==0?TEXT("water"):I==1?TEXT("radio"):TEXT("workbench"));Chunk->Residents.Add(F);}
}
