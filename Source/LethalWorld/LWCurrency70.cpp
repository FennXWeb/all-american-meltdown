#include "LWGeography84.h"
#include "LWCurrency70.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWResident.h"
#include "LWWorldObject.h"
#include "LWBorder51.h"
namespace LWCurrency70 {
bool CanadianTrade(const ALWCharacter* P){
 if(!P||!P->World||!IsValid(P->OpenObject)||P->OpenObject->Kind!=ELWObjectKind::Trader)return false;
 const auto* C=P->World->Containers.Find(P->OpenObject->RecordId);
 return LWGeography84::Canada(FVector2D(P->OpenObject->GetActorLocation()))||(C&&C->Context==TEXT("canada68"));
}
int64 Balance(const ALWCharacter* P){return P?(CanadianTrade(P)?P->RPG.Canada68.CanadianDollars:P->Money):0;}
int64& Wallet(ALWCharacter* P){return CanadianTrade(P)?P->RPG.Canada68.CanadianDollars:P->Money;}
const TCHAR* Unit(const ALWCharacter* P){return CanadianTrade(P)?TEXT("CAD $"):TEXT("CR ");}
double Rate(double Hours,int Seed){
 if(!FMath::IsFinite(Hours))return 1.25;
 const double T=FMath::Max(0.,Hours)*2;const int64 Block=FMath::FloorToInt64(T);const double F=T-Block;
 auto Anchor=[Seed](int64 B){FRandomStream R(int32(GetTypeHash(B)^uint32(Seed)^0xCA70D011u));return .85+R.FRand()*.8;};
 return FMath::Lerp(Anchor(Block),Anchor(Block+1),F*F*(3-2*F));
}
double Rate(const ALWCharacter* P){return P&&P->World?Rate(P->WorldHour(),P->World->Seed):1.25;}
bool Convert(int64& Credits,int64& CAD,int64 Amount,bool ToCAD,double Mid,int64& Received){
 Received=0;if(Amount<=0||Credits<0||CAD<0||!FMath::IsFinite(Mid)||Mid<.85||Mid>1.65)return false;
 int64& From=ToCAD?Credits:CAD;int64& To=ToCAD?CAD:Credits;if(Amount>From)return false;
 // Bound double conversion below its exact-integer limit and round payouts down.
 const double Out=double(Amount)*(ToCAD?Mid*.97:.97/Mid);
 if(Out<1||Out>9000000000000000.||Out>double(MAX_int64-To))return false;
 const int64 Pay=FMath::FloorToInt64(Out);if(Pay>MAX_int64-To)return false;
 From-=Amount;To+=Pay;Received=Pay;return true;
}
FString ExchangeText(const ALWCharacter* P){
 const double R=Rate(P);return FString::Printf(TEXT("TORONTO EXCHANGE | Credits: %lld | CAD $%lld\nLive rate: 100 CR buys CAD $%.2f. CAD $100 buys %.2f CR. A 3%% spread is included; payouts round down. Rates change with world time."),P->Money,P->RPG.Canada68.CanadianDollars,R*97,97/R);
}
bool Exchange(ALWCharacter* P,int64 Amount,bool ToCAD){
 if(!P||!P->Speaker||P->Speaker->NpcRole!=TEXT("exchange70")||!LWGeography84::Canada(FVector2D(P->Speaker->GetActorLocation()))||FVector::Dist(P->GetActorLocation(),P->Speaker->GetActorLocation())>550)return false;
 int64 Paid=0;if(!Convert(P->Money,P->RPG.Canada68.CanadianDollars,Amount,ToCAD,Rate(P),Paid)){P->Notify(TEXT("INSUFFICIENT FUNDS OR AMOUNT TOO SMALL"));return false;}
 P->Notify(FString::Printf(TEXT("EXCHANGED %lld %s FOR %lld %s"),Amount,ToCAD?TEXT("CR"):TEXT("CAD"),Paid,ToCAD?TEXT("CAD"):TEXT("CR")),5);P->PersistWorldChange();return true;
}
}
