#include "LWSettlement82.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWResident.h"
#include "LWVehicle.h"
#include "LWLootTable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "Engine/World.h"

FName ALWSettlement82::HomeClaim(FName Id)const{for(const auto& C:Player->RPG.Claims82)if(C.Value.Residents.ContainsByPredicate([&](const auto& N){return N.Id==Id;}))return C.Key;return NAME_None;}
FVector ALWSettlement82::Home(FName Id)const{
 for(const auto& Pair:Player->RPG.Claims82){const auto& C=Pair.Value;const auto* N=C.Residents.FindByPredicate([&](const auto& R){return R.Id==Id;});if(!N)continue;
  bool Night=World->TimeOfDay<6||World->TimeOfDay>=22;const auto* P=C.Pieces.FindByPredicate([&](const auto& X){return X.Id==(Night?N->Bed:N->Station);});
  if(P){const auto* D=LWBuilding82::Find(P->Catalog);if(D)return P->Transform.TransformPosition({0,-D->Size.Y*.5-90,92});}
  float Angle=(GetTypeHash(Id)%360)*PI/180.f;FVector Spot=C.Center+FVector(FMath::Cos(Angle)*330,FMath::Sin(Angle)*330,0);Spot.Z=World->HeightAt(FVector2D(Spot))+92;return Spot;
 }return Player->GetActorLocation();
}
bool ALWSettlement82::Recruit(FName Id){auto* C=Player->RPG.Claims82.Find(Id);if(!C||!C->Broadcast||C->Residents.Num()>=LWBuilding82::Beds(*C)||!C->Pieces.ContainsByPredicate([](const auto& P){return P.Catalog==TEXT("ham");}))return false;
 FLWSettler82 N;N.Id=FName(*(Id.ToString()+FString::Printf(TEXT("_resident_%d"),++C->RecruitSerial)));N.Name=Player->SettlerName(Id,N.Id);N.Voice=GetTypeHash(N.Id)%3;
 for(const auto& B:C->Pieces)if(const auto* D=LWBuilding82::Find(B.Catalog);D&&D->Use==TEXT("bed")&&!C->Residents.ContainsByPredicate([&](const auto& R){return R.Bed==B.Id;})){N.Bed=B.Id;break;}
 if(N.Bed.IsNone())return false;C->Residents.Add(N);C->RecruitmentSeconds=0;Player->Notify(N.Name+TEXT(" has arrived at ")+C->Name,6);Player->RequestSave40();return true;
}
bool ALWSettlement82::Assign(FName Id,FName Station){auto* C=State();if(!C)return false;auto* N=C->Residents.FindByPredicate([&](const auto& R){return R.Id==Id;});if(!N)return false;
 if(!Station.IsNone()){const auto* P=C->Pieces.FindByPredicate([&](const auto& X){return X.Id==Station;});const auto* D=P?LWBuilding82::Find(P->Catalog):nullptr;if(!D||!D->Job())return false;
  if(C->Residents.ContainsByPredicate([&](const auto& R){return R.Id!=Id&&R.Station==Station;})){Message=TEXT("This station already has a worker.");return false;}}
 N->Station=Station;Message=Station.IsNone()?TEXT("Assignment cleared."):TEXT("Assignment saved. Crew members work when they return home.");Player->RequestSave40();return true;
}
bool ALWSettlement82::Crew(FName Id,bool Follow){auto* C=State();if(!C)return false;const auto* N=C->Residents.FindByPredicate([&](const auto& R){return R.Id==Id;});if(!N)return false;
 auto* Existing=Player->RPG.Crew.FindByPredicate([&](const auto& R){return R.Id==Id;});int Active=0;for(const auto& R:Player->RPG.Crew)Active+=R.Following;
 if(Follow&&(!Existing||!Existing->Following)&&Active>=Player->CompanionLimit()){Message=TEXT("Active crew is full. Increase your companion limit with perks.");return false;}
 if(!Existing){FLWCrewRecord R;R.Id=N->Id;R.Name=N->Name;R.Voice=N->Voice;R.Bedroom=0;Player->RPG.Crew.Add(R);Existing=&Player->RPG.Crew.Last();Player->QuestEvent(TEXT("recruit"));}
 Existing->Following=Follow;Existing->Station=Follow?NAME_None:C->Id;Existing->HomeVehicle66=NAME_None;Player->Bunker45.Assignments.Remove(Id);
 for(TActorIterator<ALWResident> R(GetWorld());R;++R)if(R->ResidentId==Id){if(!Follow&&R->Riding)R->LeaveVehicle();R->SettlementId=Follow?NAME_None:C->Id;R->ResetCompanionNavigation();R->Home=Home(Id);}
 Player->RequestSave40();Message=Follow?N->Name+TEXT(" is joining your crew."):N->Name+TEXT(" is returning to ")+C->Name;return true;
}
bool ALWSettlement82::ResidentTick(ALWResident* N,float Dt){const FName Id=HomeClaim(N->ResidentId);if(Id.IsNone())return false;const auto* CrewRecord=Player->RPG.Crew.FindByPredicate([&](const auto& R){return R.Id==N->ResidentId;});if(CrewRecord&&CrewRecord->Following)return false;
 N->SettlementId=Id;if(N->Riding)N->LeaveVehicle();N->ThreatScanClock-=Dt;
 if(N->ThreatScanClock<=0){N->ThreatScanClock=.35f+(N->PersistentId%7)*.02f;if(N->DefendSettlement(Player)){N->ThreatScanClock=0;return true;}}
 N->Home=Home(N->ResidentId);const auto& C=Player->RPG.Claims82.FindChecked(Id);const auto* Resident=C.Residents.FindByPredicate([&](const auto& R){return R.Id==N->ResidentId;});bool Working=Resident&&!Resident->Station.IsNone()&&World->TimeOfDay>=6&&World->TimeOfDay<22;
 const auto* Assigned=Resident?C.Pieces.FindByPredicate([&](const auto& X){return X.Id==Resident->Station;}):nullptr;const bool Guard=Working&&Assigned&&Assigned->Catalog==TEXT("guard");
 N->ActivityTime-=Dt;if(N->ActivityTime<=0){N->ActivityTime=FMath::FRandRange(12.f,24.f);N->FollowTarget=N->Home;if(!Working||Guard){const float A=FMath::FRand()*2*PI;N->FollowTarget+=FVector(FMath::Cos(A)*(Guard?550:140),FMath::Sin(A)*(Guard?550:140),0);if(Guard){if(FVector::Dist2D(N->FollowTarget,C.Center)>C.Radius-100)N->FollowTarget=N->Home;N->FollowTarget.Z=World->HeightAt(FVector2D(N->FollowTarget))+92;}}}
 if(FVector::DistSquared(N->FollowTarget,N->Home)>FMath::Square(Guard?700.:280.))N->FollowTarget=N->Home;
 bool Walking=N->NavigateCompanion(N->FollowTarget,170,Dt);if(!Walking){N->GetCharacterMovement()->StopMovementImmediately();if(Working)if(const auto* P=C.Pieces.FindByPredicate([&](const auto& X){return X.Id==Resident->Station;}))N->SetActorRotation((P->Transform.GetLocation()-N->GetActorLocation()).Rotation());}
 N->FollowCheck45+=Dt;if(N->FollowCheck45>2){if(Walking&&FVector::DistSquared(N->GetActorLocation(),N->FollowPosition45)<400)N->FollowStuck45+=N->FollowCheck45;else N->FollowStuck45=0;N->FollowPosition45=N->GetActorLocation();N->FollowCheck45=0;
  if(N->FollowStuck45>12||FVector::Dist2D(N->GetActorLocation(),C.Center)>C.Radius+500){FVector Spot=N->Home;FRotator Rot=N->GetActorRotation();if(GetWorld()->FindTeleportSpot(N,Spot,Rot)){N->SetActorLocation(Spot,false,nullptr,ETeleportType::TeleportPhysics);N->ResetCompanionNavigation();N->FollowStuck45=0;}}}
 if(N->Gun)N->Gun->SetVisibility(Guard);return true;
}
void ALWSettlement82::Economy(float Dt){
 bool Changed=false;TMap<FName,ALWResident*> Live;for(TActorIterator<ALWResident> N(GetWorld());N;++N)Live.Add(N->ResidentId,*N);bool Spawned=false;
 for(auto& Pair:Player->RPG.Claims82){auto& C=Pair.Value;TMap<FName,int> Workers;
  // Repair bed reservations after a spare bed was removed or an old save migrated.
  TSet<FName> UsedBeds;for(auto& N:C.Residents){if(!C.Pieces.ContainsByPredicate([&](const auto& P){return P.Id==N.Bed;} )||UsedBeds.Contains(N.Bed))N.Bed=NAME_None;if(!N.Bed.IsNone())UsedBeds.Add(N.Bed);}
  for(auto& N:C.Residents){if(N.Bed.IsNone())for(const auto& B:C.Pieces)if(const auto* D=LWBuilding82::Find(B.Catalog);D&&D->Use==TEXT("bed")&&!UsedBeds.Contains(B.Id)){N.Bed=B.Id;UsedBeds.Add(B.Id);Changed=true;break;}
   const auto* CrewRecord=Player->RPG.Crew.FindByPredicate([&](const auto& R){return R.Id==N.Id;});const bool Away=CrewRecord&&CrewRecord->Following;
   if(!Away)if(const auto* Station=C.Pieces.FindByPredicate([&](const auto& P){return P.Id==N.Station;}))if(const auto* D=LWBuilding82::Find(Station->Catalog);D&&D->Job())Workers.FindOrAdd(D->Use)++;
   ALWResident* NPC=Live.FindRef(N.Id);
   if(!Away&&!Loaded.Contains(C.Id)){if(IsValid(NPC)&&Player->Speaker!=NPC){NPC->Destroy();People.Remove(N.Id);}continue;}
   if(!NPC&&!Spawned&&Loaded.Contains(C.Id)&&Queue.IsEmpty()){FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;NPC=GetWorld()->SpawnActor<ALWResident>(Home(N.Id),FRotator::ZeroRotator,Params);if(NPC){NPC->ConfigureResident(N.Id,TEXT("recruit"),N.Name,N.Voice);NPC->SettlementId=Away?NAME_None:C.Id;NPC->Home=Home(N.Id);People.Add(N.Id,NPC);Spawned=true;}}
   if(NPC)People.Add(N.Id,NPC);
  }
  const bool Radio=C.Broadcast&&C.Pieces.ContainsByPredicate([](const auto& P){return P.Catalog==TEXT("ham");});
  if(Radio&&C.Residents.Num()<LWBuilding82::Beds(C)){C.RecruitmentSeconds+=Dt;if(C.RecruitmentSeconds>=480.f/FMath::Min(3,1+Workers.FindRef(TEXT("recruit"))))Changed|=Recruit(C.Id);}
  C.WorkSeconds+=Dt;if(C.WorkSeconds<300)continue;C.WorkSeconds=FMath::Fmod(C.WorkSeconds,300.);C.WorkCycle++;Changed=true;
  const FName Store(*(C.Id.ToString()+TEXT("_supplies")));auto& Chest=World->Containers.FindOrAdd(Store);Chest.Id=Store;Chest.Context=TEXT("settlement82");Chest.Width=12;Chest.Height=24;Chest.Position=C.Center;Chest.Unlocked=true;
  auto Put=[&](FName Item,int Count){if(Count<=0)return;auto Trial=Chest.Items;int Left=Count;for(auto& I:Trial)if(I.Definition==Item&&I.Slot.IsNone()){int Add=FMath::Min(Left,LWItems::Def(Item).MaxStack-I.Count);I.Count+=Add;Left-=Add;}while(Left>0){int Add=FMath::Min(Left,LWItems::Def(Item).MaxStack);auto I=LWItems::Make(Item,Add);if(!LWItems::Place(Trial,I,12,24))return;Left-=Add;}Chest.Items=MoveTemp(Trial);};
  C.Scrap=FMath::Min(1000000,C.Scrap+Workers.FindRef(TEXT("scavenge"))*20);C.Treasury+=Workers.FindRef(TEXT("sell"))*40;
  Put(TEXT("food"),Workers.FindRef(TEXT("farm"))*3);Put(TEXT("water"),Workers.FindRef(TEXT("water"))*4);Put(TEXT("medkit"),Workers.FindRef(TEXT("medical")));
  for(int I=0;I<FMath::Min(Workers.FindRef(TEXT("loot")),8);I++){auto Pack=Chest.Items;bool Fits=true;for(auto Item:LWLoot::Roll(TEXT("depot"),GetTypeHash(C.Id)+C.WorkCycle*199+I))if(Item.Slot==TEXT("Loaded"))Pack.Add(Item);else if(!LWItems::Place(Pack,Item,12,24)){Fits=false;break;}if(Fits)Chest.Items=MoveTemp(Pack);}
  if(Workers.FindRef(TEXT("contracts"))&&C.Contracts.Num()<6)for(const auto& Q:ULWRPGCatalog::Get()->Quests)if(!Q.Id.ToString().StartsWith(TEXT("landmark_"))&&!C.Contracts.Contains(Q.Id)&&!Player->RPG.Quests.ContainsByPredicate([&](const auto& X){return X.Id==Q.Id;})&&(Q.Prerequisite.IsNone()||Player->RPG.Quests.ContainsByPredicate([&](const auto& X){return X.Id==Q.Prerequisite&&X.Rewarded;}))){C.Contracts.Add(Q.Id);break;}
 }
 if(Changed)Player->RequestSave40();
}
