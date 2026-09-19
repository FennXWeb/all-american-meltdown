#include "LWSaveGame.h"
#include "Misc/AutomationTest.h"
#include "UObject/UnrealType.h"

#if WITH_DEV_AUTOMATION_TESTS
// Background serialization is permitted only for detached value data.
// Fail loudly if a future save field introduces a reference to mutable world/asset objects.
namespace {
bool ValueOnly43(const FProperty* P){
 if(CastField<FObjectPropertyBase>(P)||CastField<FInterfaceProperty>(P))return false;
 if(const auto* S=CastField<FStructProperty>(P)){
  for(TFieldIterator<FProperty> I(S->Struct);I;++I)if(!ValueOnly43(*I))return false;
 }
 if(const auto* A=CastField<FArrayProperty>(P))return ValueOnly43(A->Inner);
 if(const auto* M=CastField<FMapProperty>(P))return ValueOnly43(M->KeyProp)&&ValueOnly43(M->ValueProp);
 if(const auto* S=CastField<FSetProperty>(P))return ValueOnly43(S->ElementProp);
 return true;
}
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWSaveValues43,"LethalWorld.Save43.DetachedValueSnapshot",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWSaveValues43::RunTest(const FString&){
 for(TFieldIterator<FProperty> I(ULWSaveGame::StaticClass());I;++I)TestTrue(*I->GetName(),ValueOnly43(*I));
 return true;
}
#endif
