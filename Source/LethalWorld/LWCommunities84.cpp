#include "LWCommunities84.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWResident.h"
#include "LWStreaming68.h"
#include "LWNewYork69.h"
#include "LWWorldTextComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
bool LWCommunities84::Spawn(ALWWorld* W,ALWChunk* C,FIntPoint Region){FVector2D Hub;FString Name;if(!LWNY69::Settlement(Region,&Hub,&Name))return false;Hub-=FVector2D(0,5000);if(LWGen::ChunkAt(Hub)!=C->Coordinate)return true;auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(W,0));if(!P)return true;
 auto& Town=P->EnsureSettlement(Region);Town.Center=FVector(Hub,110);const FName TownId(*FString::Printf(TEXT("town_hostile_%d_%d"),Region.X,Region.Y));C->MajorBounds+=FBox2D(Hub-FVector2D(16000),Hub+FVector2D(16000));
 TArray<LWGen::FSite> Plots;for(const auto& S:LWNY69::Sites())if(S.SettlementBuilding&&FVector2D::Distance(S.Position,Hub)<17000)Plots.Add(S);
 auto Geometry=[&](TFunction<void()> Fn){if(C->Plan68)C->Plan68->Geometry.Add(MoveTemp(Fn));else Fn();};auto Population=[&](TFunction<void()> Fn){if(C->Plan68)C->Plan68->Population.Add(MoveTemp(Fn));else Fn();};
 for(const auto& S:Plots)Geometry([C,W,S,TownId](){const int First=C->Residents.Num();C->Building(W,S);for(int I=First;I<C->Residents.Num();I++)if(auto* O=Cast<ALWWorldObject>(C->Residents[I]))if(auto* Storage=W->Containers.Find(O->RecordId))Storage->Settlement=TownId;});
 auto Spot=[&](int Type,int Variant,FVector Local=FVector::ZeroVector){TArray<LWGen::FSite> Candidates=Plots.FilterByPredicate([&](const auto& S){return S.Type==Type;});if(Candidates.IsEmpty())return FVector(Hub+FVector2D(2500+Variant*140,1700),110);const auto& S=Candidates[Variant%Candidates.Num()];return FVector(S.Position,110)+FRotator(0,S.Yaw,0).RotateVector(Local);};
 const FVector Market=Spot(67,0),Clinic=Spot(2,0),Bar=Spot(19,0),Home=Spot(9,0),Workshop(Hub+FVector2D(3500,-1800),110),Garden(Hub+FVector2D(-3100,1800),110),Well(Hub+FVector2D(2400,2500),110);
 // Every work destination has a corresponding furnishing and a walkable approach.
 Geometry([C,W,Hub,Workshop,Garden,Well](){const FVector O=C->GetActorLocation();for(int Side:{-1,1})for(int Y=-2;Y<=2;Y++)C->StreetLight(W,FVector(Hub+FVector2D(Side*900,Y*5200),12)-O,FRotator::ZeroRotator);
  C->Add(W,TEXT("WeaponBench39"),NAME_None,Workshop-FVector(0,0,86)-O);C->Add(W,TEXT("Generator"),NAME_None,Workshop+FVector(280,180,-86)-O);for(int Row=0;Row<5;Row++)for(int Col=0;Col<3;Col++)C->Add(W,TEXT("Planter65"),NAME_None,Garden+FVector(Row*130,Col*140,-86)-O);
  for(int I=0;I<4;I++){C->Add(W,TEXT("WaitingBench65"),NAME_None,FVector(Hub+FVector2D((I%2?1:-1)*1400,(I/2?1:-1)*1800),24)-O);C->Add(W,TEXT("WasteBinV13"),NAME_None,FVector(Hub+FVector2D((I%2?1:-1)*1600,(I/2?1:-1)*1800),24)-O);}
  if(auto* O2=W->SpawnObject(ELWObjectKind::Furniture,FName(*FString::Printf(TEXT("community84_water_%d_%d"),C->Coordinate.X,C->Coordinate.Y)),Well-FVector(0,0,86))){O2->SetFurniture(TEXT("water"));C->Residents.Add(O2);}
 });
 // Reuse established resident IDs, names, reputation and recruitment records.
 const TCHAR* Roles[]={TEXT("merchant"),TEXT("warden"),TEXT("medic"),TEXT("recruit"),TEXT("recruit"),TEXT("merchant"),TEXT("recruit")};
 TArray<FVector> Important={Market,Workshop,Clinic,Bar+FVector(80,160,0),Bar+FVector(-120,160,0),Spot(67,1),Spot(19,1)};
 const int Civilians=32+LWGen::Hash(Region.X,Region.Y,W->Seed,8401)%17;
 for(int I=0;I<Civilians+7;I++){const bool Civil=I>=7;int Index=Civil?I-7:I;const FName Id(*FString::Printf(TEXT("%s_%d_%d_%d"),Civil?TEXT("citizen"):TEXT("resident"),Region.X,Region.Y,Index));FVector Start=Civil?FVector(Hub+FVector2D((Index%2?1:-1)*(1200+(Index%5)*180),-10000+(Index/2)*1150),110):Important[I];FName Role=Civil?TEXT("civilian"):Roles[I];
  TArray<FVector> Routine;if(!Civil){Routine={Start,Start+FVector(0,80,0),Start-FVector(0,80,0),Start};}else{const FVector Job=Index%5==0?Garden:Index%5==1?Workshop:Index%5==2?Well:Index%5==3?Market:Clinic;Routine={Spot(9,Index/16)+FVector(-300+(Index%4)*180,-400+((Index/4)%4)*190,0),Job+FVector((Index%5)*110,(Index/5)*115,0),Start,Spot(19,Index/16)+FVector(-300+(Index%4)*180,-350+((Index/4)%4)*175,0)};}
  Population([C,W,P,Id,TownId,Start,Role,Routine,I,Civil](){if(!IsValid(C)||!IsValid(P)||P->RPG.Crew.ContainsByPredicate([&](const auto& Crew){return Crew.Id==Id;}))return;for(TActorIterator<ALWResident> N(W->GetWorld());N;++N)if(N->ResidentId==Id)return;FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;auto* N=W->GetWorld()->SpawnActor<ALWResident>(Start,FRotator::ZeroRotator,Params);if(!N)return;N->ConfigureResident(Id,Role,P->SettlerName(TownId,Id),I);N->SettlementId=TownId;N->ActivitySpots=Routine;N->CommunityRoutine84=Civil;N->Activity=2;N->ActivityTime=3+I*.7f;N->ChatterTime=15+I*1.7f;C->Residents.Add(N);});
 }
 Geometry([C,W,Hub,Name](){FVector At=FVector(Hub+FVector2D(-1100,-13200),170)-C->GetActorLocation();C->Box(W,TEXT("Wood"),At,FVector(30,900,240));for(float Side:{-1.f,1.f})C->Box(W,TEXT("Steel"),At+FVector(0,Side*390,-90),FVector(25,25,270));auto* Label=NewObject<ULWWorldTextComponent>(C);Label->SetupAttachment(C->GetRootComponent());Label->SetRelativeLocation(At+FVector(-18,0,0));Label->SetRelativeRotation(FRotator(0,180,0));Label->SetWorldSize(52);Label->SetHorizontalAlignment(EHTA_Center);Label->SetText(FText::FromString(Name));Label->RegisterComponent();});return true;
}
