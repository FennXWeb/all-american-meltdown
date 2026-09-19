#include "LWEncounter.h"
#include "LWInventory.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWEventCatalogTest,"LethalWorld.Encounters.CatalogAndStoryGraph",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWEventCatalogTest::RunTest(const FString&){
 const auto* C=GetDefault<ULWEncounterCatalog>();TestTrue(TEXT("at least fifty authored encounters"),C->Events.Num()>=50);TSet<FName> IDs;TSet<int> Tasks,Scenes;
 for(const auto& D:C->Events){TestFalse(TEXT("unique encounter ID"),IDs.Contains(D.Id));IDs.Add(D.Id);Tasks.Add(int(D.Task));Scenes.Add(int(D.Setting));TestTrue(TEXT("authored introduction and outcome"),D.Intro.Len()>40&&D.Outcome.Len()>30);TestTrue(TEXT("valid reward definition"),LWItems::Def(D.RewardItem).Id==D.RewardItem);if(!D.CostItem.IsNone())TestTrue(TEXT("valid resource cost"),LWItems::Def(D.CostItem).Id==D.CostItem);TestTrue(TEXT("bounded difficulty and enemy count"),D.Enemies>=0&&D.Enemies<=10&&D.EnemyKind>=0&&D.EnemyKind<=2);TestTrue(TEXT("positive duration and cooldown"),D.WorkSeconds>0&&D.Lifetime>D.WorkSeconds&&D.Cooldown>0);
 TSet<FName> Chain;const FLWEncounterDefinition* N=&D;while(N&&!N->Prerequisite.IsNone()){TestFalse(TEXT("story graph has no cycles"),Chain.Contains(N->Id));if(Chain.Contains(N->Id))break;Chain.Add(N->Id);N=C->Find(N->Prerequisite);TestNotNull(TEXT("story prerequisite exists"),N);}}
 TestEqual(TEXT("fifteen gameplay mechanisms"),Tasks.Num(),15);TestEqual(TEXT("eight visual scene families"),Scenes.Num(),8);return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWEventPacingTest,"LethalWorld.Encounters.EligibilityAndDeterminism",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWEventPacingTest::RunTest(const FString&){const auto* C=GetDefault<ULWEncounterCatalog>();FLWEncounterState S;S.Elapsed=5000;
 for(const auto& D:C->Events){TestFalse(TEXT("disabled cannot be selected"),[&](){auto X=D;X.Enabled=false;return LWEncounters::Eligible(X,S,50,12,1,true);}());if(LWEncounters::Hostile(D))TestFalse(TEXT("recovery excludes threats"),LWEncounters::Eligible(D,S,50,12,1,false));
 auto Ready=S;if(!D.Prerequisite.IsNone())Ready.Completed.Add(D.Prerequisite);float Hour=D.Hours==2?23:12,Rain=D.Weather==0?0:1;
 TestTrue(TEXT("each event can become eligible"),LWEncounters::Eligible(D,Ready,50,Hour,Rain,true));Ready.LastType.Add(D.Id,Ready.Elapsed);TestFalse(TEXT("cooldown enforced"),LWEncounters::Eligible(D,Ready,50,Hour,Rain,true));Ready.LastType.Empty();Ready.Recent.Add(D.Id);TestFalse(TEXT("recent events excluded"),LWEncounters::Eligible(D,Ready,50,Hour,Rain,true));}
 TSet<FName> Chosen;for(int I=0;I<300;I++){S.Serial=I;S.Elapsed+=20;auto* A=LWEncounters::Select(*C,S,7919,10,12,.8,true);auto* B=LWEncounters::Select(*C,S,7919,10,12,.8,true);TestTrue(TEXT("same state produces same weighted choice"),A&&A==B);if(A)Chosen.Add(A->Id);}TestTrue(TEXT("selection has meaningful variety"),Chosen.Num()>20);
 auto X=C->Events[0];X.Weight=0;TestFalse(TEXT("zero weight ignored"),LWEncounters::Eligible(X,S,50,12,1,true));return true;}
#endif
