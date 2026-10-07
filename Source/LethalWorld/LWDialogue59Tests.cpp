#include "LWDialogue59.h"
#include "LWRPG.h"
#include "Misc/AutomationTest.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWDialogue59Test,"LethalWorld.Update59.VoiceIdentity",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWDialogue59Test::RunTest(const FString&){
 TestEqual(TEXT("Whitespace does not regenerate recordings"),ULWDialogueCatalog59::Key(TEXT("M01"),TEXT("Hello.\n   Keep moving.")),ULWDialogueCatalog59::Key(TEXT("M01"),TEXT("Hello. Keep moving.")));
 TestEqual(TEXT("Python and Unreal use identical recording keys"),ULWDialogueCatalog59::Key(TEXT("M01"),TEXT("Hello. Keep moving.")),FString(TEXT("M01_0350acc5bcd7e893f22e5935f63772bf")));
 TestNotEqual(TEXT("Different profiles never share a recording key"),ULWDialogueCatalog59::Key(TEXT("M01"),TEXT("Hello.")),ULWDialogueCatalog59::Key(TEXT("F01"),TEXT("Hello.")));
 TestEqual(TEXT("Radio aliases retain Mara's identity"),ULWDialogueCatalog59::StoryIdentity(TEXT("Mara, through the wall")),FName(TEXT("story_mara")));
 TestEqual(TEXT("Voss full and abbreviated names agree"),ULWDialogueCatalog59::StoryIdentity(TEXT("Captain Adrienne Voss")),ULWDialogueCatalog59::StoryIdentity(TEXT("Captain Voss")));
 TestEqual(TEXT("Gender follows named cast even in cutscenes"),ULWDialogueCatalog59::DefaultProfile(TEXT("story_mara"),false),FName(TEXT("F01")));
 FLWRPGState Original;Original.VoiceProfiles59.Add(TEXT("settler_123"),TEXT("M03"));TArray<uint8> Bytes;
 {FMemoryWriter Memory(Bytes);FObjectAndNameAsStringProxyArchive Ar(Memory,false);FLWRPGState::StaticStruct()->SerializeItem(Ar,&Original,nullptr);}
 FLWRPGState Restored;{FMemoryReader Memory(Bytes);FObjectAndNameAsStringProxyArchive Ar(Memory,true);FLWRPGState::StaticStruct()->SerializeItem(Ar,&Restored,nullptr);}
 TestEqual(TEXT("NPC voice assignment survives save serialization"),Restored.VoiceProfiles59.FindRef(TEXT("settler_123")),FName(TEXT("M03")));
 return true;
}
#endif
