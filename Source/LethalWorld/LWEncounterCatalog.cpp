#include "LWEncounter.h"
#include "LWGeneration.h"
#include "Misc/PackageName.h"
ULWEncounterCatalog::ULWEncounterCatalog(){ResetToNativeDefaults();}
void ULWEncounterCatalog::ResetToNativeDefaults(){Events.Empty();
#include "LWEncounterDefaults.inl"
}
const ULWEncounterCatalog* ULWEncounterCatalog::Get(){static TWeakObjectPtr<ULWEncounterCatalog> C;if(!C.IsValid()&&FPackageName::DoesPackageExist(TEXT("/Game/Data/DA_Encounters")))C=LoadObject<ULWEncounterCatalog>(nullptr,TEXT("/Game/Data/DA_Encounters.DA_Encounters"));return C.IsValid()?C.Get():GetDefault<ULWEncounterCatalog>();}
const FLWEncounterDefinition* ULWEncounterCatalog::Find(FName Id)const{return Events.FindByPredicate([&](const auto& D){return D.Id==Id;});}
bool LWEncounters::Hostile(const FLWEncounterDefinition& D){return D.Enemies>0||D.Task==ELWEncounterTask::Hazard;}
bool LWEncounters::Eligible(const FLWEncounterDefinition& D,const FLWEncounterState& S,int Level,float Hour,float Rain,bool Danger){
 if(!D.Enabled||D.Id.IsNone()||!FMath::IsFinite(D.Weight)||D.Weight<=0||!FMath::IsFinite(D.WorkSeconds)||D.WorkSeconds<=0||!FMath::IsFinite(D.Lifetime)||D.Lifetime<=0||Level<D.MinLevel||(!Danger&&Hostile(D)))return false;
 bool Night=Hour<6||Hour>=20;if((D.Hours==1&&Night)||(D.Hours==2&&!Night)||(D.Weather==0&&Rain>.05f)||(D.Weather==1&&Rain<=.05f))return false;
 if(!D.Prerequisite.IsNone()&&(!S.Completed.Contains(D.Prerequisite)||S.Completed.Contains(D.Id)))return false;
 if(auto* Last=S.LastType.Find(D.Id))if(S.Elapsed-*Last<FMath::Max(60.f,D.Cooldown))return false;
 if(S.Recent.Contains(D.Id))return false;
 for(const auto& E:S.Records)if(E.Value.Stage<2&&E.Value.Type==D.Id)return false;
 return true;
}
const FLWEncounterDefinition* LWEncounters::Select(const ULWEncounterCatalog& C,const FLWEncounterState& S,int Seed,int Level,float Hour,float Rain,bool Danger){
 FRandomStream R(LWGen::Hash(S.Serial,int(S.Elapsed/20),Seed,32021));const FLWEncounterDefinition* Pick=nullptr;float Total=0;
 for(const auto& D:C.Events)if(Eligible(D,S,Level,Hour,Rain,Danger)){float W=D.Weight*(S.Completed.Contains(D.Id)?.35f:1.f)*(D.Prerequisite.IsNone()?1.f:3.f);if(Hostile(D)&&S.Reputation<0)W*=1.35f;Total+=W;if(R.FRand()*Total<W)Pick=&D;}return Pick;
}
