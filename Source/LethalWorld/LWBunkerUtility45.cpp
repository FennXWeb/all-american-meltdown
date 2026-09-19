#include "LWBunker45.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWLootTable.h"
void ALWBunker45::TickUtilities(float Dt){
 auto& S=Player->Bunker45;if(S.Assignments.IsEmpty())return;S.WorkSeconds+=Dt;if(S.WorkSeconds<300)return;S.WorkSeconds-=300;++S.WorkCycles;
 TMap<FName,int> Workers;for(const auto& A:S.Assignments){const auto* C=Player->RPG.Crew.FindByPredicate([&](const auto& X){return X.Id==A.Key;});if(C&&!C->Following&&C->Station.IsNone()&&S.Utilities.Contains(A.Value))Workers.FindOrAdd(A.Value)++;}
 for(auto& W:Workers)W.Value=FMath::Min(W.Value,8);Player->Money+=Workers.FindRef(TEXT("trade"))*35;
 auto& Chest=World->Containers.FindOrAdd(TEXT("bunker_work45"));Chest.Id=TEXT("bunker_work45");Chest.Context=TEXT("bunker");Chest.Width=12;Chest.Height=14;Chest.Position=Center()+FVector(-1700,-130,24);
 for(int I=0;I<Workers.FindRef(TEXT("salvage"));++I)for(auto Item:LWLoot::Roll(TEXT("road"),World->Seed+S.WorkCycles*53+I))LWItems::Place(Chest.Items,Item,12,14);
 if(Workers.FindRef(TEXT("farm")))for(FName Food:{FName(TEXT("food")),FName(TEXT("water"))}){auto Item=LWItems::Make(Food,Workers.FindRef(TEXT("farm")));LWItems::Place(Chest.Items,Item,12,14);}
 if(Workers.FindRef(TEXT("medical"))&&Player->bSafehouse)Player->Health=FMath::Min(Player->MaxHealth(),Player->Health+Workers[TEXT("medical")]*10);
 if(Workers.FindRef(TEXT("recruit"))&&S.WorkCycles%4==0&&Player->RPG.Crew.Num()<S.Bedrooms()){
  FLWCrewRecord C;C.Id=FName(*(TEXT("crew45_")+FGuid::NewGuid().ToString(EGuidFormats::Digits)));const TCHAR* Names[]={TEXT("Della Marr"),TEXT("Evan Pike"),TEXT("Mina Vale"),TEXT("Owen Reed"),TEXT("Tess Cole"),TEXT("Alex Stone")};C.Name=FString::Printf(TEXT("%s %d"),Names[S.WorkCycles%6],S.WorkCycles/4);C.Bedroom=Player->RPG.Crew.Num();C.Following=false;Player->RPG.Crew.Add(C);Player->Notify(C.Name+TEXT(" joined your bunker."));
 }
 if(Workers.FindRef(TEXT("contracts"))&&S.WorkCycles%2==0)for(const auto& Q:ULWRPGCatalog::Get()->Quests)if(!Player->RPG.Quests.ContainsByPredicate([&](const auto& X){return X.Id==Q.Id;})&&Player->AcceptQuest(Q.Id))break;
 Player->RequestSave40();
}

