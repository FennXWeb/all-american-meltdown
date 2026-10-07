#include "LWStory.h"
#include "LWSaveGame.h"
#include "Misc/AutomationTest.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "LWGeneration.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWStoryDefinitionTest,"LethalWorld.Story.Chapter1.Definitions",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWStoryDefinitionTest::RunTest(const FString&){const auto& M=LWStory::Missions();TestEqual(TEXT("complete chapter checkpoints"),M.Num(),30);TSet<uint32> IDs;for(int S=0;S<M.Num();S++){TestTrue(TEXT("named objective"),FCString::Strlen(M[S].Title)>0&&FCString::Strlen(M[S].Objective)>0);TestTrue(TEXT("bounded authored site"),M[S].Site>=-1&&M[S].Site<9);for(int I=0;I<64;I++){uint32 Id=LWStory::EnemyId(S,I);TestFalse(TEXT("unique kill record"),IDs.Contains(Id));IDs.Add(Id);}}for(int A=0;A<9;A++)for(int B=A+1;B<9;B++)TestTrue(TEXT("unique compounds do not overlap"),FVector2D::Distance(LWStory::Site(A),LWStory::Site(B))>90000);return true;}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWStorySaveTest,"LethalWorld.Story.Chapter1.CheckpointSerialization",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWStorySaveTest::RunTest(const FString&){auto* A=NewObject<ULWSaveGame>();A->RPG.Story.Enabled=true;A->RPG.Story.Stage=23;A->RPG.Story.LayoutVersion52=52;A->RPG.Story.GearHeld=true;A->RPG.Story.Flags.Add(TEXT("mercy_burned"));A->RPG.Story.Paid.Add(20);FLWItemInstance Item;Item.Id=FGuid::NewGuid();Item.Definition=TEXT("shotgun");Item.Rounds=3;A->RPG.Story.Confiscated.Add(Item);A->RPG.Story.ConfiscatedWeapon=Item.Id;TArray<uint8> Bytes;FMemoryWriter W(Bytes);FObjectAndNameAsStringProxyArchive Out(W,false);A->Serialize(Out);auto* B=NewObject<ULWSaveGame>();FMemoryReader R(Bytes);FObjectAndNameAsStringProxyArchive In(R,true);B->Serialize(In);TestEqual(TEXT("story layout version"),B->RPG.Story.LayoutVersion52,52);TestEqual(TEXT("checkpoint"),B->RPG.Story.Stage,23);TestTrue(TEXT("burnt settlement persists"),B->RPG.Story.Flags.Contains(TEXT("mercy_burned")));TestTrue(TEXT("reward idempotence persists"),B->RPG.Story.Paid.Contains(20));TestTrue(TEXT("gear escrow survives"),B->RPG.Story.GearHeld&&B->RPG.Story.Confiscated.Num()==1&&B->RPG.Story.Confiscated[0].Id==Item.Id&&B->RPG.Story.Confiscated[0].Rounds==3);return true;}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWStoryReserveTest,"LethalWorld.Story.Chapter1.Reservations",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWStoryReserveTest::RunTest(const FString&){for(int I=0;I<9;I++)TestFalse(TEXT("retired chapter reserves no land"),LWStory::Reserved(LWStory::Site(I),6000));return true;}
#endif
