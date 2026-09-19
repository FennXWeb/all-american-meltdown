#include "LWRPG.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWLevelTest,"LethalWorld.RPG.ExperienceAndPoints",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWLevelTest::RunTest(const FString&){
 FLWRPGState S;TestEqual(TEXT("No points before leveling"),S.Points,0);
 TestEqual(TEXT("Negative XP ignored"),LWRPG::AddXP(S,-100),0);TestEqual(TEXT("Below threshold"),LWRPG::AddXP(S,99),0);
 TestEqual(TEXT("Exact threshold levels once"),LWRPG::AddXP(S,1),1);TestEqual(TEXT("Two points each level"),S.Points,2);
 TestEqual(TEXT("One award crosses multiple thresholds"),LWRPG::AddXP(S,1500),3);TestEqual(TEXT("All points retained"),S.Points,8);
 S.Perks.Add(TEXT("perk_34"),1);LWRPG::AddXP(S,900);TestEqual(TEXT("Learning perk grants future extra point"),S.Points,11);
 LWRPG::AddXP(S,MAX_int64);TestEqual(TEXT("Level capped"),S.Level,100);const int Points=S.Points;LWRPG::AddXP(S,MAX_int64);TestEqual(TEXT("Cap cannot duplicate points"),S.Points,Points);return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWPerkTest,"LethalWorld.RPG.RankAndAttributeGates",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWPerkTest::RunTest(const FString&){
 FLWRPGState S;TestFalse(TEXT("No free perk"),LWRPG::Buy(S,TEXT("perk_0")));S.Points=20;
 TestFalse(TEXT("Unknown perk rejected"),LWRPG::Buy(S,TEXT("missing")));TestTrue(TEXT("First rank available"),LWRPG::Buy(S,TEXT("perk_0")));
 TestFalse(TEXT("Rank two level gate"),LWRPG::Buy(S,TEXT("perk_0")));S.Level=4;TestTrue(TEXT("Rank two unlocks"),LWRPG::Buy(S,TEXT("perk_0")));
 S.Level=50;TestTrue(TEXT("Rank three unlocks"),LWRPG::Buy(S,TEXT("perk_0")));const int Points=S.Points;
 TestFalse(TEXT("Rank cap"),LWRPG::Buy(S,TEXT("perk_0")));TestEqual(TEXT("Rejected purchase preserves points"),S.Points,Points);
 TestTrue(TEXT("Damage scales with rank"),FMath::IsNearlyEqual(LWRPG::Stat(S,TEXT("melee")),.3f));
 TestFalse(TEXT("Attribute gate blocks squad perk"),LWRPG::Buy(S,TEXT("perk_23")));S.Attributes[3]=7;
 TestTrue(TEXT("Squad rank one"),LWRPG::Buy(S,TEXT("perk_23")));for(int Rank=2;Rank<=8;Rank++)TestTrue(TEXT("Additional squad rank unlocks"),LWRPG::Buy(S,TEXT("perk_23")));TestFalse(TEXT("Squad caps at eight ranks"),LWRPG::Buy(S,TEXT("perk_23")));
 TestTrue(TEXT("Commander unlock"),LWRPG::Buy(S,TEXT("perk_27")));TestEqual(TEXT("Nine extra companions maximum"),LWRPG::Stat(S,TEXT("companions")),9.f);return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWCatalogTest,"LethalWorld.RPG.CatalogAndQuestGraph",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWCatalogTest::RunTest(const FString&){
 const auto* C=ULWRPGCatalog::Get();TestEqual(TEXT("51 distinct perks"),C->Perks.Num(),51);TSet<FName> IDs;int Categories[7]={};
 for(const auto& P:C->Perks){TestFalse(TEXT("Unique perk identity"),IDs.Contains(P.Id));IDs.Add(P.Id);TestTrue(TEXT("Valid category"),P.Category>=0&&P.Category<7);if(P.Category>=0&&P.Category<7)Categories[P.Category]++;TestTrue(TEXT("Valid rank gates"),P.MaxRank>0&&P.LevelRequired>0&&P.AttributeRequired>=1&&P.AttributeRequired<=10);TestFalse(TEXT("Effect configured"),P.Effect.IsNone());}
 for(int I=0;I<7;I++)TestEqual(TEXT("Category perks including stamina additions"),Categories[I],I==2?9:7);TestEqual(TEXT("Contracts and landmark quests"),C->Quests.Num(),32);IDs.Empty();
 for(const auto& Q:C->Quests){TestFalse(TEXT("Unique quest identity"),IDs.Contains(Q.Id));IDs.Add(Q.Id);TestTrue(TEXT("Nonempty positive reward quest"),Q.Objectives.Num()>0&&Q.XP>0&&Q.Credits>0);
  for(const auto& O:Q.Objectives)TestTrue(TEXT("Valid objective"),O.Count>0&&!O.Event.IsNone()&&!O.Text.IsEmpty());
  TSet<FName> Seen;const FLWQuest* Node=&Q;while(Node&&!Node->Prerequisite.IsNone()){if(Seen.Contains(Node->Id)){AddError(TEXT("Quest prerequisite cycle"));break;}Seen.Add(Node->Id);Node=C->Quests.FindByPredicate([&](const auto& X){return X.Id==Node->Prerequisite;});TestNotNull(TEXT("Prerequisite exists"),Node);}
 }return true;
}
#endif
