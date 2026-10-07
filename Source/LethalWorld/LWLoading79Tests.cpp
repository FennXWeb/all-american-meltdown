#include "LWLoading79.h"
#include "Misc/AutomationTest.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWLoading79BankTest,"LethalWorld.Loading79.Content",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWLoading79BankTest::RunTest(const FString&){
 const auto Bank=LWLoading79::ReadBank(FPaths::ProjectContentDir()/TEXT("Loading79"));
 TestEqual(TEXT("Complete message bank"),Bank.Messages.Num(),81);TestEqual(TEXT("Six illustrations"),Bank.Images.Num(),6);
 TSet<FString> Ids,Texts;int Lore=0;
 for(const auto& M:Bank.Messages){TestFalse(TEXT("Unique message ID"),Ids.Contains(M.Id));TestFalse(TEXT("Unique message text"),Texts.Contains(M.Text));Ids.Add(M.Id);Texts.Add(M.Text);Lore+=M.Id.StartsWith(TEXT("lore_"));TestTrue(TEXT("Tip fits readable loading layout"),M.Text.Len()<=230);TestFalse(TEXT("No unbound hardcoded keyboard hint"),M.Text.Contains(TEXT("[")));}
 TestEqual(TEXT("Spoiler-light lore entries"),Lore,24);
 for(const auto& I:Bank.Images){TArray<uint8> Bytes;TestTrue(TEXT("Staged illustration is readable"),FFileHelper::LoadFileToArray(Bytes,*I.File));TestTrue(TEXT("Illustration is a real PNG"),Bytes.Num()>24&&Bytes[0]==137&&Bytes[1]==80&&Bytes[2]==78&&Bytes[3]==71);}
 return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWLoading79ShuffleTest,"LethalWorld.Loading79.Rotation",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWLoading79ShuffleTest::RunTest(const FString&){
 for(int Count:{0,1,6,81})for(int Seed=0;Seed<20;++Seed){const auto Order=LWLoading79::ShuffledOrder(Count,Seed);TSet<int32> Seen;for(int I:Order){TestTrue(TEXT("Valid entry"),I>=0&&I<Count);TestFalse(TEXT("No repeats within a cycle"),Seen.Contains(I));Seen.Add(I);}TestEqual(TEXT("Every entry included"),Seen.Num(),Count);if(Count>1)TestTrue(TEXT("Cycle boundary never repeats"),Order[0]!=Order.Last());}
 const auto At=LWLoading79::FrameAt(18.45,18,.9,false);
 TestEqual(TEXT("Artwork advanced on real time"),At.Current,int64(1));TestEqual(TEXT("Previous frame retained for dissolve"),At.Previous,int64(0));TestTrue(TEXT("Half dissolve"),FMath::IsNearlyEqual(At.Blend,.5f,.001f));
 TestEqual(TEXT("No first-frame fade from black"),LWLoading79::FrameAt(0,18,.9,false).Blend,1.f);
 TestEqual(TEXT("Reduced motion switches directly"),LWLoading79::FrameAt(18,18,.9,true).Blend,1.f);
 TestEqual(TEXT("Tips have independent timing"),LWLoading79::FrameAt(29,14,.35,false).Current,int64(2));
 TestEqual(TEXT("Long stalls do not require sequential catch-up"),LWLoading79::FrameAt(3600,18,.9,false).Current,int64(200));
 TestEqual(TEXT("Invalid interval is safe"),LWLoading79::FrameAt(5,0,1,false).Current,int64(0));return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWLoading79LayoutTest,"LethalWorld.Loading79.AspectRatio",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWLoading79LayoutTest::RunTest(const FString&){
 for(FVector2D View:{FVector2D(1600,900),FVector2D(2560,1080),FVector2D(5120,1440),FVector2D(1024,768),FVector2D(1280,720)}){
  const auto Fit=LWLoading79::CoverSize(FVector2D(1672,941),View);TestTrue(TEXT("No letterbox or uncovered screen"),Fit.X>=View.X-.01&&Fit.Y>=View.Y-.01);TestTrue(TEXT("No image stretching"),FMath::IsNearlyEqual(Fit.X/Fit.Y,1672./941.,.0001));
 }return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWLoading79FallbackTest,"LethalWorld.Loading79.Fallback",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWLoading79FallbackTest::RunTest(const FString&){
 const auto Parsed=LWLoading79::ParseMessages(TEXT("id,category,text\na,TIP,\"A comma, inside text.\"\na,TIP,duplicate\nb,,missing category\nc,TIP,\nd,LORE,Valid lore\n"));
 TestEqual(TEXT("Bad and duplicate rows skipped"),Parsed.Num(),2);TestEqual(TEXT("Quoted commas preserved"),Parsed[0].Text,FString(TEXT("A comma, inside text.")));
 const auto Missing=LWLoading79::ReadBank(FPaths::ProjectSavedDir()/TEXT("NonexistentLoading79Directory"));TestEqual(TEXT("Missing bank has useful fallback"),Missing.Messages.Num(),1);TestEqual(TEXT("Missing images allow solid background"),Missing.Images.Num(),0);return true;
}
#endif
