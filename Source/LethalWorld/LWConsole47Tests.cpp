#include "LWConsole47.h"
#include "LWInventory.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWCheatAmounts47,"LethalWorld.Console47.AmountValidation",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWCheatAmounts47::RunTest(const FString&){int64 N=77;
 for(const FString& S:{FString(TEXT("-1")),FString(TEXT("0")),FString(TEXT("1.5")),FString(TEXT("1e3")),FString(TEXT("1001")),FString(TEXT("999999999999999999999999")),FString(TEXT("")),FString(TEXT("12oops"))}){TestFalse(*S,LWCheats47::PositiveAmount(S,1000,N));TestEqual(TEXT("invalid input does not change amount"),N,int64(77));}
 TestTrue(TEXT("maximum credits accepted"),LWCheats47::PositiveAmount(TEXT("1000000000"),1000000000,N));TestEqual(TEXT("parsed exactly"),N,int64(1000000000));return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWCheatCatalog47,"LethalWorld.Console47.CatalogDiscovery",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWCheatCatalog47::RunTest(const FString&){const auto All=LWItems::All47();TestTrue(TEXT("includes game item catalog"),All.Num()>50);TSet<FName> Seen;for(const auto& D:All){TestFalse(TEXT("no duplicate IDs"),Seen.Contains(D.Id));Seen.Add(D.Id);TestEqual(TEXT("each listed item resolves"),LWItems::Def(D.Id).Id,D.Id);}TestTrue(TEXT("medkits discoverable"),Seen.Contains(TEXT("medkit")));return true;}
#endif
