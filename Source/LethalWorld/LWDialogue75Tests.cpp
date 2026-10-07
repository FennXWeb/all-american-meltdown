#include "LWDialogue75.h"
#include "LWDialogue59.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWDialogue75Test,"LethalWorld.Update75.ChatterVariety",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWDialogue75Test::RunTest(const FString&){
 TSet<FName> Ids;TSet<FString> Texts;TMap<FName,int> Groups;int Companions=0,Settlers=0,Raiders=0;
 for(const auto& L:LWDialogue75::Lines()){
  TestFalse(TEXT("Chatter IDs are unique"),Ids.Contains(L.Id));Ids.Add(L.Id);
  TestFalse(TEXT("Different lines are not copies"),Texts.Contains(L.Text));Texts.Add(L.Text);Groups.FindOrAdd(L.Group)++;
  const FString Group=L.Group.ToString();Companions+=Group.StartsWith(TEXT("companion_"));Settlers+=Group.StartsWith(TEXT("settler_"));Raiders+=Group.StartsWith(TEXT("raider_"));
 }
 TestTrue(TEXT("At least 25 new companion lines"),Companions>=25);TestTrue(TEXT("At least 15 new settler lines"),Settlers>=15);TestTrue(TEXT("At least 15 new raider lines"),Raiders>=15);
 FRandomStream R(75001);FLWChatterHistory75 H;
 for(const auto& G:Groups){
  TSet<FName> Heard;FName Previous;
  for(int I=0;I<G.Value*3;I++){
   const auto* L=H.Choose(G.Key,R,[](const auto&){return true;});TestNotNull(TEXT("A populated group returns a line"),L);if(!L)continue;
   TestNotEqual(TEXT("No immediate repeat at cycle boundaries"),L->Id,Previous);Previous=L->Id;
   if(I%G.Value==0)Heard.Empty();TestFalse(TEXT("Every line plays before the pool repeats"),Heard.Contains(L->Id));Heard.Add(L->Id);
  }
 }
 TestNull(TEXT("Unrecognized groups stay silent"),H.Choose(TEXT("missing"),R,[](const auto&){return true;}));
 TestNull(TEXT("Shared cooldown can suppress every candidate without consuming the pool"),H.Choose(TEXT("companion_travel"),R,[](const auto&){return false;}));
 TestNotNull(TEXT("Pool resumes when nearby speech clears"),H.Choose(TEXT("companion_travel"),R,[](const auto&){return true;}));
 return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWDialogue75CoverageTest,"LethalWorld.Update75.RecordedCoverage",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWDialogue75CoverageTest::RunTest(const FString&){
 auto* C=ULWDialogueCatalog59::Get();TestNotNull(TEXT("Voice catalog exists"),C);if(!C)return false;
 TestTrue(TEXT("Validated catalog enabled"),C->Enabled);TestEqual(TEXT("Male active profile"),C->ProceduralMale75,FName(TEXT("M04")));TestEqual(TEXT("Female active profile"),C->ProceduralFemale75,FName(TEXT("F04")));
 TestEqual(TEXT("Budgeted four-voice active cast"),C->ProceduralProfiles75.Num(),4);
 for(FName Profile:C->ProceduralProfiles75)for(const auto& L:LWDialogue75::Lines()){
  const auto* Line=C->Lines.Find(ULWDialogueCatalog59::Key(Profile,L.Text));TestTrue(*(Profile.ToString()+TEXT(" ")+L.Id.ToString()),Line&&!Line->Audio.IsNull()&&Line->Duration>.2f);
 }
 TestEqual(TEXT("Original named cast retained"),C->Cast.FindRef(TEXT("story_mara")),FName(TEXT("F01")));
 TestEqual(TEXT("Prior male procedural assignments migrate consistently"),C->ProceduralReplacements75.FindRef(TEXT("M02")),FName(TEXT("M05")));
 TSet<FName> Male,Female;for(int I=0;I<100;I++){FName Id(*FString::Printf(TEXT("npc75_%d"),I));Male.Add(C->ProceduralProfile75(Id,false));Female.Add(C->ProceduralProfile75(Id,true));}
 TestEqual(TEXT("Both male voices assigned"),Male.Num(),2);TestEqual(TEXT("Both female voices assigned"),Female.Num(),2);
 return true;
}
#endif
