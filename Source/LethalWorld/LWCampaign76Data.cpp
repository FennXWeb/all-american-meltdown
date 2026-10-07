#include "LWCampaign76Corridor.h"
#include "LWCampaignProduction77.h"
#include "LWCampaign76.h"
#include "LWNewYork69.h"
#include "LWSyracuse73.h"
#include "LWGeneration.h"
#include "LWDestiny71.h"
#include "LWCampaign76Data.inl"

const FLWC76Stage* LWCampaign76::Stage(FName Id){return Stages().FindByPredicate([&](const auto& S){return S.Id==Id;});}
const FLWC76Scene* LWCampaign76::Scene(FName Id){return Scenes().FindByPredicate([&](const auto& S){return S.Id==Id;});}
const FLWC76Person* LWCampaign76::Person(FName Id){return Cast().FindByPredicate([&](const auto& S){return S.Id==Id;});}
bool LWCampaign76::Meets(const FLWCampaign76State& S,const TArray<FString>& Conditions){
 for(FString C:Conditions){bool Not=C.RemoveFromStart(TEXT("!"));FString Key,Value;bool Pass=false;
  if(C.Split(TEXT(">="),&Key,&Value))Pass=S.Values.FindRef(FName(*Key))>=FCString::Atoi(*Value);
  else if(C.Split(TEXT("="),&Key,&Value))Pass=S.Values.FindRef(FName(*Key))==FCString::Atoi(*Value);
  else Pass=S.Values.FindRef(FName(*C))!=0;
  if(Pass==Not)return false;
 }return true;
}
void LWCampaign76::Apply(FLWCampaign76State& S,const TArray<FString>& Effects){
 for(FString E:Effects){FString Key,Value;
  if(E.Split(TEXT("+="),&Key,&Value))S.Values.FindOrAdd(FName(*Key))+=FCString::Atoi(*Value);
  else if(E.Split(TEXT("-="),&Key,&Value))S.Values.FindOrAdd(FName(*Key))-=FCString::Atoi(*Value);
  else if(E.Split(TEXT("="),&Key,&Value))S.Values.Add(FName(*Key),FCString::Atoi(*Value));
  else if(E.RemoveFromStart(TEXT("journal:"))){if(!S.Journal.Contains(E))S.Journal.Add(E);}
  else S.Values.Add(FName(*E),1);
 }
}
bool LWCampaign76::Alive(const FLWCampaign76State& S,FName Who){return !S.Values.FindRef(FName(*(TEXT("dead_")+Who.ToString())));}
FString LWCampaign76::Ending(const FLWCampaign76State& S){
 if(S.Values.FindRef(TEXT("remain_canada")))return TEXT("The Other Shore");
 if(S.Values.FindRef(TEXT("loyalist"))){if(S.Values.FindRef(TEXT("richardson_dead"))&&S.Values.FindRef(TEXT("takeover"))&&S.Values.FindRef(TEXT("leverage"))>=3&&S.Values.FindRef(TEXT("loyal_personnel"))&&S.Values.FindRef(TEXT("reserve_control")))return TEXT("The Inheritance");return S.Values.FindRef(TEXT("leverage"))<=0&&S.Values.FindRef(TEXT("support"))<=0?TEXT("Useful Until Dawn"):TEXT("First Citizen");}
 if(!S.Values.FindRef(TEXT("richardson_dead")))return FString();
 if(S.Values.FindRef(TEXT("reserves_lost"))||S.Values.FindRef(TEXT("civilian_losses"))>=3)return TEXT("A Victory of Ash");
 const int LivingSupport=
  (S.Values.FindRef(TEXT("logistics"))>0&&(Alive(S,TEXT("della"))||(S.Values.FindRef(TEXT("canal_control"))>=2&&Alive(S,TEXT("hank")))))+
  (S.Values.FindRef(TEXT("guard_support"))>0&&Alive(S,S.Values.FindRef(TEXT("imani_leads"))?FName(TEXT("imani")):FName(TEXT("rook"))))+
  (S.Values.FindRef(TEXT("freehold_refuge"))>0&&Alive(S,TEXT("hannah")))+
  (S.Values.FindRef(TEXT("ferry_repaired"))!=0&&Alive(S,TEXT("ada")));
 if(LivingSupport>=3&&S.Values.FindRef(TEXT("coalition"))>=3&&S.Values.FindRef(TEXT("evidence_secured"))&&S.Values.FindRef(TEXT("reserves_secured"))&&S.Values.FindRef(TEXT("postwar_control"))==0)return TEXT("Open Country");
 return TEXT("New Uniforms");
}
namespace {
const LWGen::FSite* Pick76(FName Id){
 const FVector2D Syracuse=LWNY69::Project(43.0481,-76.1474);const LWGen::FSite* Best=nullptr;double Score=DBL_MAX;
 for(int I=0;I<LWCampaign76::Corridor().Num();I++)if(LWCampaign76::Corridor()[I].Id==Id)return LWNY69::Sites().FindByPredicate([&](const auto& S){return S.Id==0xEC760000u+I;});
 const int Type=Id==TEXT("bellwether")?0:Id==TEXT("market")?69:Id==TEXT("records")?17:Id==TEXT("housing")?9:-1;
 for(const auto& S:LWNY69::Sites()){if(S.Type!=Type)continue;const double D=(S.Position-Syracuse).SizeSquared();if(D<Score){Score=D;Best=&S;}}
 return Best;
}
}
FVector2D LWCampaign76::Site(FName Id){if(const auto* S=Pick76(Id))return S->Position;return LWSyracuse73::Start();}
uint32 LWCampaign76::SiteId(FName Id){if(const auto* S=Pick76(Id))return S->Id;return 0;}
bool LWCampaign76::Reserved(FVector2D P){for(FName Id:{FName(TEXT("bellwether")),FName(TEXT("market")),FName(TEXT("records")),FName(TEXT("housing"))})if(const auto* S=Pick76(Id);S&&(P-S->Position).Size()<S->Size.Size()*.6+700)return true;return false;}
int32 LWCampaign76::MarketZone(){static const int32 Index=[]()->int32{const auto& Zones=LWDestiny71::Data().Zones;for(int I=0;I<Zones.Num();I++){const auto& Z=Zones[I];if(Z.Level!=1||Z.Area<250)continue;bool Clear=true;for(int X:{-1,0,1})for(int Y:{-1,0,1})Clear&=LWDestiny71::FloorAt(Z.Center+FVector2D(X*850,Y*650),1);if(Clear)return I;}return INDEX_NONE;}();return Index;}
FTransform LWCampaign76::Frame(FName Id){
 if(Id==TEXT("canada"))return FTransform(FRotator::ZeroRotator,FVector(LWGen::CanadaCity68()+FVector2D(-4500,-4500),36));
 if(const auto* S=Pick76(Id)){
  if(Id==TEXT("market")){const int I=MarketZone();if(LWDestiny71::Data().Zones.IsValidIndex(I)){const auto& Z=LWDestiny71::Data().Zones[I];return FTransform(FRotator::ZeroRotator,FVector(S->Position+Z.Center,24+Z.Level*600));}}
  if(Id==TEXT("bellwether")||Id==TEXT("transport")||LWProduction77::Centered(Id))return FTransform(FRotator(0,S->Yaw,0),FVector(S->Position,36));
  return FTransform(FRotator(0,S->Yaw,0),FVector(LWGen::Entrance(*S),36));
 }
 return FTransform(FRotator::ZeroRotator,FVector(LWSyracuse73::Start(),36));
}
