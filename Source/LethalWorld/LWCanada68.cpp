#include "LWGeography84.h"
#include "LWCampaign76.h"
#include "LWCurrency70.h"
#include "LWCanada68.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWResident.h"
#include "LWVehicle.h"
#include "LWBorderGuard51.h"
#include "LWWeaponMods.h"
#include "Components/SceneComponent.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace {
bool Controlled(const FLWItemInstance& I){const auto& D=LWItems::Def(I.Definition);return D.Category==TEXT("Weapon")||D.Category==TEXT("Ammo")||D.Category==TEXT("Magazine")||I.Slot==TEXT("Loaded");}
void Store(ALWCharacter* P,TArray<FLWItemInstance>& Items,FName Source){
 TSet<FGuid> Ids;for(const auto& I:Items)if(Controlled(I)){Ids.Add(I.Id);if(I.LoadedMagazine.IsValid())Ids.Add(I.LoadedMagazine);}
 for(const auto& I:Items)if(Ids.Contains(I.Id)){FLWCustomsItem68 E;E.Item=I;E.Source=Source;P->RPG.Canada68.Escrow.Add(E);}
 Items.RemoveAll([&](const auto& I){return Ids.Contains(I.Id);});
}
bool AtCheckpoint(ALWCharacter* P){return P&&P->World&&(LWGeography84::NearCheckpoint(FVector2D(P->GetActorLocation()))||(P->RPG.Campaign76.Started&&P->RPG.Campaign76.Values.FindRef(TEXT("humanitarian_permit"))&&FVector::Dist2D(P->GetActorLocation(),LWCampaign76::Frame(TEXT("crossing")).GetLocation())<1800));}
void Radio(ALWWorld* W,ALWCharacter* P,const FString& Text,float Duration){W->Sound(TEXT("RadioStatic"),P->GetActorLocation(),.85f);P->Notify(TEXT("CANADIAN AIR CONTROL: ")+Text,Duration);}
}
bool LWCanada68::Authorized(const ALWCharacter* P){return P&&(P->RPG.Canada68.Passport||P->RPG.Campaign76.Values.FindRef(TEXT("humanitarian_permit")))&&P->RPG.Canada68.Cleared;}
bool LWCanada68::BuyPassport(ALWCharacter* P){
 if(!AtCheckpoint(P))return false;auto& S=P->RPG.Canada68;if(S.Passport)return true;
 if(P->Money<PassportPrice){P->Notify(TEXT("PASSPORT COST: $1,000,000"));return false;}
 P->Money-=PassportPrice;S.Passport=true;P->PersistWorldChange();P->Notify(TEXT("CANADIAN PASSPORT ISSUED"));return true;
}
bool LWCanada68::Deposit(ALWCharacter* P){
 if(!AtCheckpoint(P)||(!P->RPG.Canada68.Passport&&!P->RPG.Campaign76.Values.FindRef(TEXT("humanitarian_permit"))))return false;
 P->CancelReload();Store(P,P->Inventory,NAME_None);
 // Search every compartment of nearby vehicles, including independently named RV storage.
 TArray<FName> Cars;for(TActorIterator<ALWVehicle> V(P->GetWorld());V;++V)if((V->Driver==P||V->ConvoyOwner==P||(V->Record()&&(V->Record()->LastDriven||V->Record()->Owned45)))&&FVector::DistSquared(V->GetActorLocation(),P->GetActorLocation())<FMath::Square(5000.))Cars.Add(V->RecordId);
 for(auto& Pair:P->World->Containers)for(FName Car:Cars)if(Pair.Key==Car||Pair.Key==FName(*(TEXT("glove_")+Car.ToString()))||Pair.Key.ToString().StartsWith(Car.ToString()+TEXT("_"))){Store(P,Pair.Value.Items,Pair.Key);break;}
 P->ActiveWeaponId.Invalidate();P->WeaponRoot->SetVisibility(false,true);P->bTrigger=false;
 P->RPG.Canada68.Cleared=true;P->SyncAmmoHUD();P->PersistWorldChange();P->Notify(TEXT("CUSTOMS CLEARED / WEAPONS HELD FOR YOUR RETURN"),6);return true;
}
void LWCanada68::Reclaim(ALWCharacter* P){
 if(!AtCheckpoint(P)||LWGeography84::Canada(FVector2D(P->GetActorLocation()))){if(P)P->Notify(TEXT("COLLECT WEAPONS ON THE AMERICAN SIDE"));return;}
 auto& S=P->RPG.Canada68;S.Cleared=false;S.Entered=false;
 // Restore each weapon and its linked magazine atomically. Keep excess in escrow, never drop it.
 for(int Index=0;Index<S.Escrow.Num();){
  const auto E=S.Escrow[Index];if(E.Item.Slot==TEXT("Loaded")){++Index;continue;}
  TArray<FLWItemInstance>* Destination=&P->Inventory;int W=12,H=LWItems::InventoryHeight(P->Inventory);
  if(!E.Source.IsNone()){auto* C=P->World->Containers.Find(E.Source);if(!C){++Index;continue;}Destination=&C->Items;W=C->Width;H=C->Height;}
  TArray<FLWItemInstance> From;From.Add(E.Item);for(const auto& Other:S.Escrow)if(Other.Item.Id==E.Item.LoadedMagazine)From.Add(Other.Item);
  const bool Fit=LWItems::QuickTransfer(From,*Destination,E.Item.Id,W,H);
  if(!Fit){++Index;continue;}
  if(E.Source.IsNone()&&!E.Item.Slot.IsNone()&&LWItems::CanEquip(E.Item,E.Item.Slot))LWItems::Equip(*Destination,E.Item.Id,E.Item.Slot);
  S.Escrow.RemoveAll([&](const auto& I){return I.Item.Id==E.Item.Id||I.Item.Id==E.Item.LoadedMagazine;});
 }
 P->PersistWorldChange();P->SyncAmmoHUD();P->Notify(S.Escrow.IsEmpty()?TEXT("WEAPONS RETURNED / SAFE TRAVELS"):TEXT("MAKE ROOM / REMAINING WEAPONS ARE SAFE AT CUSTOMS"),6);
}
bool LWCanada68::Dialogue(ALWCharacter* P){
 if(!P||!P->Speaker)return false;auto* N=P->Speaker.Get();const bool Guard=N->NpcRole==TEXT("border68"),Merchant=N->NpcRole==TEXT("canada_shop68");const bool Exchange=N->NpcRole==TEXT("exchange70");if(!Guard&&!Merchant&&!Exchange)return false;
 auto Choice=[&](const TCHAR* Text,const TCHAR* A){P->DialogueChoices.Add(Text);P->DialogueActions.Add(FName(A));};
 if(Exchange){
  P->DialogueText=TEXT("Exchange credits and Canadian dollars here. The displayed live rate includes our spread.");
  Choice(TEXT("Exchange 100 credits for CAD."),TEXT("cad100"));Choice(TEXT("Exchange 1,000 credits for CAD."),TEXT("cad1000"));Choice(TEXT("Exchange all credits for CAD."),TEXT("cadall"));
  Choice(TEXT("Exchange CAD $100 for credits."),TEXT("cr100"));Choice(TEXT("Exchange CAD $1,000 for credits."),TEXT("cr1000"));Choice(TEXT("Exchange all CAD for credits."),TEXT("crall"));
 }else if(Guard){
  if(auto* G=Cast<ALWBorderGuard51>(N);G&&G->Hostile51>0){P->DialogueText=TEXT("Stand back. This checkpoint is under alert.");}
  else{
   P->DialogueText=TEXT("Welcome to the Canadian checkpoint. Stay this side of the red line behind us. A permanent passport costs one million credits. Check your weapons and ammunition here before each visit; vehicle storage is inspected too. Collect everything on your return. Follow the signed lakeshore route to Toronto.");
   if(!P->RPG.Canada68.Passport)Choice(TEXT(""),TEXT(""));
   if(!P->RPG.Canada68.Passport){P->DialogueChoices.Last()=TEXT("Buy a permanent passport. [$1,000,000]");P->DialogueActions.Last()=TEXT("ca_buy");}
   else Choice(TEXT("Check weapons and clear customs."),TEXT("ca_deposit"));
   if(!P->RPG.Canada68.Escrow.IsEmpty())Choice(TEXT("Collect my stored weapons."),TEXT("ca_reclaim"));
   Choice(TEXT("Mark Toronto on my route."),TEXT("ca_route"));
   Choice(TEXT("Where can I exchange currency?"),TEXT("ca_exchange"));
  }
 }else{P->DialogueText=TEXT("Welcome to Toronto. Our northern supplies are made here, and we pay a premium for goods from the south. We buy and sell only in Canadian dollars. Visit Toronto Currency Exchange to convert your credits.");Choice(TEXT("Browse northern supplies. [CAD]"),TEXT("ca_trade"));Choice(TEXT("Mark the currency exchange."),TEXT("ca_exchange"));}
 Choice(TEXT("Goodbye."),TEXT("bye"));N->Say(P->DialogueText);return true;
}
bool LWCanada68::Action(ALWCharacter* P,FName A){
 if(!P||!P->Speaker)return false;auto* N=P->Speaker.Get();
 if(N->NpcRole==TEXT("exchange70")){
  const bool ToCAD=A==TEXT("cad100")||A==TEXT("cad1000")||A==TEXT("cadall");
  if(ToCAD||A==TEXT("cr100")||A==TEXT("cr1000")||A==TEXT("crall")){int64 Amount=(A==TEXT("cadall")?P->Money:A==TEXT("crall")?P->RPG.Canada68.CanadianDollars:A.ToString().EndsWith(TEXT("1000"))?1000:100);LWCurrency70::Exchange(P,Amount,ToCAD);return true;}
 }
 if(A==TEXT("ca_exchange")&&(N->NpcRole==TEXT("border68")||N->NpcRole==TEXT("canada_shop68"))){P->SetWaypoint(LWGen::CanadaCity68()+FVector2D(13500,-13500));P->Notify(TEXT("TORONTO CURRENCY EXCHANGE MARKED"));P->ClosePanels();return true;}
 if(N->NpcRole==TEXT("border68")){
  if(auto* G=Cast<ALWBorderGuard51>(N);G&&G->Hostile51>0)return false;
  if(A==TEXT("ca_buy")){BuyPassport(P);P->BuildDialogue(TEXT("root"));return true;}
  if(A==TEXT("ca_deposit")){Deposit(P);P->ClosePanels();return true;}
  if(A==TEXT("ca_reclaim")){Reclaim(P);P->BuildDialogue(TEXT("root"));return true;}
  if(A==TEXT("ca_route")){P->SetWaypoint(LWGen::CanadaCity68());P->Notify(TEXT("TORONTO / LAKESHORE ROUTE MARKED"),8);P->ClosePanels();return true;}
 }
 if(N->NpcRole==TEXT("canada_shop68")&&A==TEXT("ca_trade")){
  const FName Id(*(TEXT("canada_shop_")+N->ResidentId.ToString()));
  if(!P->World->Containers.Contains(Id)){
   FLWContainerRecord R;R.Id=Id;R.Context=TEXT("canada68");R.bTrader=true;R.Position=N->GetActorLocation();
   for(FName Item:{FName(TEXT("ca_trauma")),FName(TEXT("ca_meal")),FName(TEXT("ca_tonic")),FName(TEXT("medkit")),FName(TEXT("water")),FName(TEXT("food")),FName(TEXT("lockpick")),FName(TEXT("scrap"))}){auto I=LWItems::Make(Item,FMath::Min(5,LWItems::Def(Item).MaxStack));LWItems::Place(R.Items,I,12,12);}P->World->Containers.Add(Id,R);
  }
  if(!IsValid(N->Shop)){N->Shop=P->World->SpawnObject(ELWObjectKind::Trader,Id,N->GetActorLocation());if(!N->Shop)return true;N->Shop->SetActorHiddenInGame(true);N->Shop->SetActorEnableCollision(false);N->Shop->SetActorTickEnabled(false);}
  N->Shop->SetActorLocation(N->GetActorLocation());P->OpenContainer(N->Shop);return true;
 }
 return false;
}
int LWCanada68::TradePrice(const ALWCharacter* P,const FLWItemInstance& I,bool Buy){
 const int Base=LWMods::Price(I);const auto* C=P&&P->World&&IsValid(P->OpenObject)?P->World->Containers.Find(P->OpenObject->RecordId):nullptr;
 if(LWCurrency70::CanadianTrade(P)){const int Sell=Base*2;return Buy?FMath::Max(Sell+FMath::Max(1,Base/4),FMath::RoundToInt(Base*3*FMath::Max(.3f,1-P->Stat(TEXT("barter"))))):Sell;}
 return Buy?FMath::RoundToInt(Base*FMath::Max(.3f,P?1-P->Stat(TEXT("barter")):1.f)):Base/2;
}
void LWCanada68::Tick(ALWWorld* W,ALWCharacter* P,float Dt){
 if(!W||!P||!P->bStarted||P->bMenu||P->Health<=0||P->IsUIOpen())return;
 auto& S=P->RPG.Canada68;const bool North=LWGeography84::Canada(FVector2D(P->GetActorLocation()));
 auto* V=W->BorderAircraft68.IsValid()?W->BorderAircraft68.Get():P->Vehicle.Get();bool Flight=V&&(V->IsHelicopter57()||V->IsAircraft84())&&LWGeography84::Canada(FVector2D(V->GetActorLocation()));
 if(Flight&&!Authorized(P)){
  if(W->BorderAircraft68.Get()!=V){W->BorderAircraft68=V;W->BorderCountdown68=AirGrace;Radio(W,P,TEXT("UNAUTHORIZED AIRCRAFT. LEAVE CANADIAN AIRSPACE IMMEDIATELY. YOU HAVE 20 SECONDS."),8);}
  float Before=W->BorderCountdown68;W->BorderCountdown68-=Dt;
  if(Before>10&&W->BorderCountdown68<=10)Radio(W,P,TEXT("FINAL WARNING. TEN SECONDS TO LEAVE OUR AIRSPACE."),6);
  if(W->BorderCountdown68<=0){Radio(W,P,TEXT("AIRSPACE VIOLATION. INTERCEPT AUTHORIZED."),4);V->Explode();W->BorderAircraft68.Reset();}
 }else if(W->BorderAircraft68.IsValid()){Radio(W,P,TEXT("AIRSPACE CLEAR. LAND AT CUSTOMS FOR ENTRY."),5);W->BorderAircraft68.Reset();W->BorderCountdown68=0;}
 if(North&&Authorized(P)&&!S.Entered){S.Entered=true;P->Notify(TEXT("CANADA / WELCOME NORTH"),5);P->RequestSave40();}
 if(S.Entered&&!North&&!LWGeography84::Canada(FVector2D(P->GetActorLocation()))){S.Entered=false;S.Cleared=false;P->Notify(TEXT("RETURN TO CUSTOMS TO COLLECT YOUR WEAPONS"),6);P->RequestSave40();}
 // Loaded saves and pursuits cannot import an American enemy population into Canada.
 W->CanadaSafetyClock68-=Dt;if(W->CanadaSafetyClock68<=0){W->CanadaSafetyClock68=1;
  for(TActorIterator<ALWZombie> E(W->GetWorld());E;++E)if(!Cast<ALWResident>(*E)&&LWGeography84::Canada(FVector2D(E->GetActorLocation())))E->Destroy();
  if(North&&Authorized(P)){
   const int Before=S.Escrow.Num();Store(P,P->Inventory,NAME_None);
   if(P->Vehicle){const FName Id=P->Vehicle->RecordId;for(auto& C:W->Containers)if(C.Key==Id||C.Key==P->Vehicle->GloveId()||C.Key.ToString().StartsWith(Id.ToString()+TEXT("_")))Store(P,C.Value.Items,C.Key);}
   if(S.Escrow.Num()!=Before){P->ActiveWeaponId.Invalidate();P->WeaponRoot->SetVisibility(false,true);P->Notify(TEXT("WEAPONS HELD AT CUSTOMS"));P->PersistWorldChange();}
  }
 }
}
