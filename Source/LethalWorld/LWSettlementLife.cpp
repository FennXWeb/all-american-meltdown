#include "LWCharacter.h"
#include "LWWeaponMods.h"
#include "LWWorld.h"
#include "LWResident.h"
#include "LWVehicle.h"
#include "Misc/Crc.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

double ALWCharacter::WorldHour()const{return World?(World->DayNumber-1)*24.+World->TimeOfDay:0;}
FLWSettlementRecord& ALWCharacter::EnsureSettlement(FIntPoint R){
 FName Id(*FString::Printf(TEXT("town_hostile_%d_%d"),R.X,R.Y));auto& T=RPG.Settlements.FindOrAdd(Id);
 if(T.Name.IsEmpty()){
  static const TCHAR* A[]={TEXT("Cedar"),TEXT("Ash"),TEXT("Copper"),TEXT("Liberty"),TEXT("Mercy"),TEXT("Briar"),TEXT("Pine"),TEXT("Iron"),TEXT("Last"),TEXT("Coyote"),TEXT("Juniper"),TEXT("Hollow"),TEXT("Redwater"),TEXT("Silver"),TEXT("Dust"),TEXT("Prospect")};
  static const TCHAR* B[]={TEXT("Crossing"),TEXT("Haven"),TEXT("Creek"),TEXT("Grove"),TEXT("Landing"),TEXT("Junction"),TEXT("Heights"),TEXT("Springs"),TEXT("Ridge"),TEXT("Falls"),TEXT("Watch"),TEXT("Point"),TEXT("Valley"),TEXT("Rest"),TEXT("Hold"),TEXT("Reach")};
  uint32 H=LWGen::Hash(R.X,R.Y,World->Seed,1871);T.Size=LWGen::Hash(R.X,R.Y,World->Seed,1870)%3;
  T.Name=FString(A[H%16])+TEXT(" ")+B[(H/16)%16];FString Base=T.Name;int N=2;
  bool Duplicate=true;while(Duplicate){Duplicate=false;for(const auto& Pair:RPG.Settlements)if(Pair.Key!=Id&&Pair.Value.Name==T.Name){Duplicate=true;break;}if(Duplicate)T.Name=Base+FString::Printf(TEXT(" %d"),N++);}
  T.Center=FVector(LWGen::Hub(R,World->Seed)+FVector2D(0,-5000),110);T.LastEconomyHour=WorldHour();
  // Migrate old permanent hostility to a temporary incident.
  if(World->PropStates.FindRef(Id)){T.Reputation=-30;T.HostileUntil=WorldHour()+12;World->PropStates.Remove(Id);}
 }
 return T;
}
FString ALWCharacter::SettlerName(FName Town,FName Id){
 auto* T=RPG.Settlements.Find(Town);if(!T)return Id.ToString();if(auto* N=T->Names.Find(Id))return *N;
 static const TCHAR* First[]={TEXT("Mara"),TEXT("Elias"),TEXT("June"),TEXT("Otis"),TEXT("Nadia"),TEXT("Ruth"),TEXT("Tobias"),TEXT("Clara"),TEXT("Jasper"),TEXT("Sofia"),TEXT("Caleb"),TEXT("Wren"),TEXT("Hector"),TEXT("Iris"),TEXT("Dante"),TEXT("Mae"),TEXT("Nolan"),TEXT("Vera"),TEXT("Simon"),TEXT("Alma"),TEXT("Felix"),TEXT("Ada"),TEXT("Owen"),TEXT("Zara"),TEXT("Emmett"),TEXT("Tessa"),TEXT("Malik"),TEXT("Nell"),TEXT("Silas"),TEXT("Rosa"),TEXT("Victor"),TEXT("Lena")};
 static const TCHAR* Last[]={TEXT("Mercer"),TEXT("Holloway"),TEXT("Reyes"),TEXT("Bennett"),TEXT("Chen"),TEXT("Alvarez"),TEXT("Ward"),TEXT("Okafor"),TEXT("Sinclair"),TEXT("Patel"),TEXT("Brooks"),TEXT("Fletcher"),TEXT("Quinn"),TEXT("Navarro"),TEXT("Carter"),TEXT("Voss"),TEXT("Morrow"),TEXT("Kim"),TEXT("Sawyer"),TEXT("Hayes"),TEXT("Dalton"),TEXT("Flores"),TEXT("Turner"),TEXT("Reed"),TEXT("Lawson"),TEXT("Santos"),TEXT("Hale"),TEXT("Price"),TEXT("Dawson"),TEXT("Ellis"),TEXT("Sutton"),TEXT("Grant")};
 uint32 H=FCrc::StrCrc32(*Id.ToString());FString Name;int Attempt=0;bool Used;
 do{Name=FString(First[H%32])+TEXT(" ");if(Attempt)Name+=FString::Chr('A'+(H/1024+Attempt)%26)+TEXT(". ");Name+=Last[(H/32)%32];if(Attempt>26)Name+=FString::Printf(TEXT(" %d"),Attempt);Used=false;for(const auto& S:RPG.Settlements)for(const auto& N:S.Value.Names)Used|=N.Value==Name;Attempt++;}while(Used);
 T->Names.Add(Id,Name);return Name;
}
FName ALWCharacter::NearestSettlement(FVector At)const{FName Id;double Best=DBL_MAX;for(const auto& Pair:RPG.Settlements){double D=FVector::Dist2D(At,Pair.Value.Center);if(D<Best&&D<(Pair.Value.Size==2?8500:6500)&&FMath::Abs(At.Z-Pair.Value.Center.Z)<900){Best=D;Id=Pair.Key;}}return Id;}
bool ALWCharacter::SettlementHostile(FName Id)const{const auto* T=RPG.Settlements.Find(Id);return T&&(T->Reputation<=-100||T->HostileUntil>WorldHour());}
void ALWCharacter::ChangeReputation(FName Id,int Delta,bool Aggro){auto* T=RPG.Settlements.Find(Id);if(!T)return;T->Reputation=FMath::Clamp(T->Reputation+Delta,-100,100);if(Aggro)T->HostileUntil=WorldHour()+12;if(T->Reputation<40)T->Member=false;if(T->Reputation<75)T->Leader=false;}
void ALWCharacter::RecordSettlementTransfer(int From,int To,int Value,bool Trade){
 if(!OpenObject||From==To)return;auto* R=World->Containers.Find(OpenObject->RecordId);FName Id=R?R->Settlement:NAME_None;if(Id.IsNone()&&Trade)Id=NearestSettlement(OpenObject->GetActorLocation());auto* T=RPG.Settlements.Find(Id);if(!T)return;
 if(Trade&&Value>0){if(WorldHour()-T->LastTradeHour>=24){T->TradeRep=0;T->LastTradeHour=WorldHour();}if(T->TradeRep<5){ChangeReputation(Id,1);T->TradeRep++;}}
 else if(!Trade&&From==2&&To==0&&!T->Leader){ChangeReputation(Id,-5,true);Notify(TEXT("STOLEN GOODS // SETTLEMENT REPUTATION -5"));}
}
void ALWCharacter::TickSettlements(){
 double Now=WorldHour();for(auto& Pair:RPG.Settlements){auto& T=Pair.Value;double Elapsed=FMath::Max(0.,Now-T.LastEconomyHour);int Hours=FMath::Min(168,FMath::FloorToInt(Elapsed));if(Hours>0){if(T.Leader&&!SettlementHostile(Pair.Key))T.Treasury+=Hours*(2+T.Size);T.LastEconomyHour=Now;}
 for(int I=T.Listings.Num()-1;I>=0;I--)if(T.Listings[I].SaleHour<=Now){T.Treasury+=T.Listings[I].Payout;T.Listings.RemoveAt(I);}
 }
}
bool ALWCharacter::ConsignItem(FName Town,FGuid Id){
 auto* T=RPG.Settlements.Find(Town);auto* I=FindItem(Id);if(!T||!T->Leader||SettlementHostile(Town)||T->Listings.Num()>=12||!I||!I->Slot.IsNone())return false;
 FLWConsignment Listing;Listing.Name=LWItems::Def(I->Definition).DisplayName.ToString();Listing.Payout=FMath::Max(1,FMath::RoundToInt(LWMods::Price(*I)*FMath::FRandRange(1.35f,1.9f)));Listing.SaleHour=WorldHour()+FMath::FRandRange(18.f,96.f);
 auto Next=Inventory;if(!LWItems::Transfer(Next,Listing.Items,Id,0,0,false,64,96))return false;
 Inventory=MoveTemp(Next);T->Listings.Add(Listing);RequestSave40();return true;
}
bool ALWCharacter::BuildSettlementDialogue(FName Node){
 if(!Speaker)return false;FString Key=Node.ToString();if(!Key.StartsWith(TEXT("town")))return false;FName Id=Speaker->SettlementId;auto* T=RPG.Settlements.Find(Id);if(!T)return false;
 auto Choice=[&](FString Text,FName Action){DialogueChoices.Add(Text);DialogueActions.Add(Action);};
 DialogueText=FString::Printf(TEXT("%s. Reputation: %d / 100. %s. Treasury: %lld credits."),*T->Name,T->Reputation,T->Leader?TEXT("Settlement leader"):T->Member?TEXT("Member"):TEXT("Visitor"),T->Treasury);
 if(Node==TEXT("town")){Choice(TEXT("Collect income and shop earnings."),TEXT("town_collect"));if(!T->Member)Choice(TEXT("Apply for membership. [40 reputation]"),TEXT("town_join"));if(!T->Leader)Choice(TEXT("Take leadership. [100 reputation]"),TEXT("town_lead"));if(T->Leader){Choice(TEXT("Use settlement as respawn point."),TEXT("town_spawn"));Choice(TEXT("Station companions here."),TEXT("town_crew"));Choice(TEXT("Sell my items through the shop."),TEXT("town_shop"));}Choice(TEXT("Back."),TEXT("root"));}
 else if(Node==TEXT("town_crew")){int Start=SettlementPage*4;for(int I=Start;I<FMath::Min(Start+4,RPG.Crew.Num());I++)Choice(RPG.Crew[I].Name,FName(*(TEXT("town_station:")+RPG.Crew[I].Id.ToString())));Choice(TEXT("Next companions."),TEXT("town_crew_next"));Choice(TEXT("Back."),TEXT("town"));}
 else if(Node==TEXT("town_shop")){DialogueText+=TEXT(" Listed goods sell after 18-96 game hours at a premium. Up to 12 listings; collect proceeds here.");TArray<FGuid> Goods;for(const auto& I:Inventory)if(I.Slot.IsNone())Goods.Add(I.Id);SettlementPage=FMath::Clamp(SettlementPage,0,FMath::Max(0,(Goods.Num()-1)/4));for(int I=SettlementPage*4;I<FMath::Min(SettlementPage*4+4,Goods.Num());I++){auto* Item=FindItem(Goods[I]);Choice(TEXT("List ")+LWItems::Def(Item->Definition).DisplayName.ToString()+FString::Printf(TEXT(" x%d"),Item->Count),FName(*(TEXT("town_list:")+Item->Id.ToString())));}Choice(TEXT("Next items."),TEXT("town_shop_next"));Choice(TEXT("Back."),TEXT("town"));}
 Speaker->Say(DialogueText);return true;
}
bool ALWCharacter::SettlementAction(FName Action){
 FString A=Action.ToString();if(!A.StartsWith(TEXT("town"))||!Speaker)return false;FName Id=Speaker->SettlementId;auto* T=RPG.Settlements.Find(Id);if(!T||SettlementHostile(Id))return true;
 if(Action==TEXT("town")||Action==TEXT("town_crew")||Action==TEXT("town_shop")){SettlementPage=0;BuildDialogue(Action);return true;}
 if(Action==TEXT("town_collect")){TickSettlements();Money+=T->Treasury;T->Treasury=0;}
 else if(Action==TEXT("town_join")){if(T->Reputation>=40)T->Member=true;else Notify(TEXT("40 REPUTATION REQUIRED"));}
 else if(Action==TEXT("town_lead")){if(T->Reputation==100){T->Member=T->Leader=true;T->LastEconomyHour=WorldHour();}else Notify(TEXT("100 REPUTATION REQUIRED"));}
 else if(Action==TEXT("town_spawn")&&T->Leader){RPG.Respawn=FLWRespawnPoint();RPG.Respawn.Enabled=true;RPG.Respawn.Settlement=Id;RPG.Respawn.Position=T->Center+FVector(0,650,0);RPG.Respawn.Name=T->Name;Notify(TEXT("RESPAWN POINT SET"));}
 else if(Action==TEXT("town_crew_next")){SettlementPage=(SettlementPage+1)%FMath::Max(1,(RPG.Crew.Num()+3)/4);BuildDialogue(TEXT("town_crew"));return true;}
 else if(Action==TEXT("town_shop_next")){SettlementPage++;BuildDialogue(TEXT("town_shop"));return true;}
 else if(A.StartsWith(TEXT("town_station:"))&&T->Leader){auto* C=RPG.Crew.FindByPredicate([&](const auto& X){return X.Id==FName(*A.Mid(13));});if(C){C->Following=false;C->Station=Id;for(TActorIterator<ALWResident> N(GetWorld());N;++N)if(N->ResidentId==C->Id){N->LeaveVehicle();N->Home=T->Center+FVector(C->Bedroom*85,700,0);N->SetActorLocation(N->Home);N->SettlementId=Id;}Notify(TEXT("COMPANION STATIONED"));}}
 else if(A.StartsWith(TEXT("town_list:"))){FGuid Item;FGuid::Parse(A.Mid(10),Item);if(!ConsignItem(Id,Item))Notify(TEXT("CANNOT LIST ITEM // CHECK SHOP CAPACITY"));BuildDialogue(TEXT("town_shop"));return true;}
 RequestSave40();BuildDialogue(TEXT("town"));return true;
}
