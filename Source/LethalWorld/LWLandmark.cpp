#include "LWLandmark.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWZombie.h"
#include "LWWeaponMods.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
ALWLandmark::ALWLandmark(){PrimaryActorTick.bCanEverTick=true;PrimaryActorTick.TickInterval=.4f;}
FName ALWLandmark::Key(const TCHAR* S)const{return FName(*FString::Printf(TEXT("landmark_%d_%s"),Site.Type-LWLandmarks::First,S));}
int ALWLandmark::Stage()const{return World?World->PropStates.FindRef(Key(TEXT("stage"))):0;}
int ALWLandmark::GuardCount()const{int M=LWLandmarks::Get(Site.Type).Mechanic;return M==7?9:M==6?1:M==4?5:0;}
uint32 ALWLandmark::GuardId(int I)const{return LWGen::Hash(Site.Type,I,World->Seed,28500);}
void ALWLandmark::Tick(float Dt){Super::Tick(Dt);TInlineComponentArray<UPointLightComponent*> Lights;GetComponents(Lights);for(auto* Light:Lights)Light->SetVisibility(Stage()>=3);if(!World||!Chunk||Stage()!=2||!GuardCount())return;auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));if(!P||!P->bStarted||P->bMenu||P->Health<=0||FVector::Dist2D(P->GetActorLocation(),FVector(Site.Position,0))>6500)return;
 const int Count=GuardCount();Guards.SetNum(Count);int Wave=0;if(Count==9){while(Wave<2&&World->KilledZombies.Contains(GuardId(Wave*3))&&World->KilledZombies.Contains(GuardId(Wave*3+1))&&World->KilledZombies.Contains(GuardId(Wave*3+2)))Wave++;}
 for(int I=0;I<Count;I++){if(IsValid(Guards[I])||World->KilledZombies.Contains(GuardId(I))||World->ZombieCount>=130||(Count==9&&I/3!=Wave))continue;
 FVector V=FVector(Site.Position,Chunk->GetActorLocation().Z)+FRotator(0,Site.Yaw,0).RotateVector(FVector(-650+(I%3)*650,1000+(I/3)*220,140));
 if(FVector::DistSquared(V,P->GetActorLocation())<FMath::Square(350.f))continue;FActorSpawnParameters A;A.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
 auto* Z=GetWorld()->SpawnActor<ALWZombie>(V,FRotator::ZeroRotator,A);if(!Z)continue;Z->PersistentId=GuardId(I);Z->ConfigureKind(Count==1?ELWEnemyKind(Site.Type==48?5:6):ELWEnemyKind(Count==9&&I>=6?5:I%2));Z->Home=Z->GetActorLocation();Guards[I]=Z;Chunk->Residents.Add(Z);World->ZombieCount++;
 }
}
void ALWLandmark::MakeReward(){FName Id=Key(TEXT("reserve"));if(World->Containers.Contains(Id))return;FLWContainerRecord C;C.Id=Id;C.Width=12;C.Height=12;C.Unlocked=true;C.Position=FVector(Site.Position,0);C.Context=TEXT("landmark");FRandomStream R(Site.Id);int Serial=0;
 auto Add=[&](FName Name,int Count,bool Signature=false){const auto& D=LWItems::Def(Name);if(D.Id.IsNone())return;while(Count>0){int N=FMath::Min(Count,D.MaxStack);Count-=N;auto Item=LWItems::Make(Name,N);Item.Id=FGuid::NewDeterministicGuid(Id.ToString()+FString::FromInt(Serial++));if(Signature){Item.WeaponTier=4;LWMods::RollFinish(Item,R);}if(D.Category==TEXT("Magazine"))Item.Rounds=D.Capacity;LWItems::Place(C.Items,Item,C.Width,C.Height);}};
 const auto& D=LWLandmarks::Get(Site.Type);Add(D.Weapon,1,true);const auto& Gun=LWItems::Def(D.Weapon);if(!Gun.MagazineType.IsNone()&&!LWItems::Def(Gun.MagazineType).Id.IsNone())Add(Gun.MagazineType,2);if(!Gun.AmmoType.IsNone())Add(Gun.AmmoType,FMath::Max(12,Gun.Capacity*3));Add(TEXT("medkit"),4);Add(TEXT("lockpick"),6);Add(TEXT("food"),6);World->Containers.Add(Id,C);
}
bool ALWLandmark::Activate(int Index,int Choice,ALWCharacter* P){if(!World||!P||P->Health<=0)return false;const auto& D=LWLandmarks::Get(Site.Type);const FName Quest(*FString::Printf(TEXT("landmark_%d"),Site.Type-LWLandmarks::First));auto* Q=P->RPG.Quests.FindByPredicate([&](const auto& V){return V.Id==Quest;});if(!Q){FLWQuestState V;V.Id=Quest;V.Stage=Stage();V.Rewarded=Stage()==3;P->RPG.Quests.Add(V);Q=&P->RPG.Quests.Last();}P->RPG.TrackedQuest=Quest;
 int S=Stage();if(Index==2&&S==3)return true;if(Index!=S){P->Notify(S<3?FString(TEXT("Next: "))+D.Steps[S]:TEXT("This reserve has already been claimed."),7);return false;}
 if(S==0){P->Notify(D.Story,10);}
 if(S==1){
  if(D.Mechanic==1||D.Mechanic==5){FName Supply=D.Mechanic==1?TEXT("water"):TEXT("scrap");if(P->CountSupply(Supply)<3){P->Notify(FString::Printf(TEXT("Requires 3 %s."),*Supply.ToString()));return false;}P->ConsumeSupply(Supply,3);}
  if(D.Mechanic==2){int& T=World->PropStates.FindOrAdd(Key(TEXT("sequence")));const int Pattern[3]={(Site.Type+1)%3,Site.Type%3,(Site.Type+2)%3};if(Choice!=Pattern[T%3]){T=0;P->PersistWorldChange();P->Notify(TEXT("Sequence reset. Read the numbered inscription at the entrance."),6);return false;}T++;if(T<3){P->Notify(FString::Printf(TEXT("Signal %d / 3 aligned."),T));P->PersistWorldChange();return false;}}
  if(D.Mechanic==3){if(P->CountSupply(TEXT("lockpick"))<1){P->Notify(TEXT("A lockpick is needed to release the damaged seal."));return false;}P->ConsumeSupply(TEXT("lockpick"),1);}
  if(GuardCount()){World->Noise(FVector(Site.Position,0),6000);P->Notify(D.Mechanic==7?TEXT("Arena active. Survive three waves."):TEXT("Something heard that. Clear the threats before opening the reserve."),8);}
 }
 if(S==2){int Left=0;for(int I=0;I<GuardCount();I++)Left+=!World->KilledZombies.Contains(GuardId(I));if(Left){P->Notify(FString::Printf(TEXT("%d threats remain."),Left));return false;}
  MakeReward();World->PropStates.Add(Key(TEXT("choice")),Choice);World->PropStates.Add(Key(TEXT("stage")),3);Q->Stage=3;Q->Rewarded=true;Q->Progress=0;
  if(D.Mechanic==8){World->PropStates.Add(TEXT("weather_crown_type"),Choice?4:0);World->PropStates.Add(TEXT("weather_crown_until"),FMath::CeilToInt(((World->DayNumber-1)*24.+World->TimeOfDay+3)*60));}
  if(D.Mechanic==9&&Choice==0)for(auto& Town:P->RPG.Settlements)Town.Value.Reputation=FMath::Min(100,Town.Value.Reputation+5);
  P->Money+=D.Mechanic==9&&Choice?3000:1500;P->GainXP(600);P->Notify(TEXT("Landmark complete. The reserve is yours."),7);World->Sound(TEXT("LevelUp"),P->GetActorLocation());P->PersistWorldChange();return true;
 }
 World->Sound(TEXT("LockOpen"),P->GetActorLocation(),.35f);World->PropStates.Add(Key(TEXT("stage")),S+1);Q->Stage=S+1;Q->Progress=0;P->PersistWorldChange();return false;
}
FString ALWLandmarkDevice::Prompt()const{if(!Landmark)return TEXT("");const auto& D=LWLandmarks::Get(Landmark->Site.Type);if(Index==2&&Landmark->Stage()==3)return TEXT("[E] OPEN RESERVE");if(Index==1&&D.Mechanic==2)return FString::Printf(TEXT("[E] CHANNEL %d"),Choice+1);if(Index==2&&D.Mechanic==8)return Choice?TEXT("[E] CALL STORM (3 HOURS)"):TEXT("[E] CLEAR SKIES (3 HOURS)");if(Index==2&&D.Mechanic==9)return Choice?TEXT("[E] SUPPRESS EVIDENCE (+3000 CREDITS)"):TEXT("[E] PUBLISH EVIDENCE (+REPUTATION)");return FString(TEXT("[E] "))+D.Steps[FMath::Clamp(Index,0,2)];}
void ALWLandmarkDevice::Use(ALWCharacter* P){if(Landmark&&Landmark->Activate(Index,Choice,P)&&Index==2)P->OpenContainer(this);}
