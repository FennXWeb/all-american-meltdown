#include "LWDebugMenu67.h"
#include "LWConsole47.h"
#include "LWInventory.h"
#include "LWPOITypes.h"
#include "LWVehicleSpec.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWDebugCatalog67,"LethalWorld.Debug67.CatalogAndFiltering",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWDebugCatalog67::RunTest(const FString&){
 const auto Items=LWDebug67::Catalog(ELWDebugSection67::Items);
 TestEqual(TEXT("every runtime item is available"),Items.Num(),LWItems::All47().Num());
 auto Medkits=LWDebug67::Filter(Items,TEXT("  MEDkit  "),TEXT("Consumable"),0,false);
 TestTrue(TEXT("search ignores case and whitespace and combines category filter"),Medkits.ContainsByPredicate([](const auto& E){return E->Id==TEXT("medkit");}));
 TestTrue(TEXT("all search terms are required"),LWDebug67::Filter(Items,TEXT("medkit not_a_real_term_67"),TEXT(""),0,false).IsEmpty());
 TestTrue(TEXT("category excludes unrelated entries"),LWDebug67::Filter(Items,TEXT("medkit"),TEXT("Weapon"),0,false).IsEmpty());
 auto Asc=LWDebug67::Filter(Items,TEXT(""),TEXT(""),0,false),Desc=LWDebug67::Filter(Items,TEXT(""),TEXT(""),0,true);
 for(int I=0;I<Asc.Num();I++)TestEqual(TEXT("descending is the inverse deterministic order"),Asc[I]->Id,Desc[Desc.Num()-1-I]->Id);
 const auto NPCs=LWDebug67::Catalog(ELWDebugSection67::NPCs);TestEqual(TEXT("NPC command and menu use one catalog"),NPCs.Num(),LWCheats47::NPCs60().Num());
 TestEqual(TEXT("friendly filter includes civilian merchant medic and recruit"),LWDebug67::Filter(NPCs,TEXT(""),TEXT("Friendly"),0,false).Num(),4);
 TestEqual(TEXT("all vehicles including aircraft and military"),LWDebug67::Catalog(ELWDebugSection67::Vehicles).Num(),LWTraffic::Specs().Num());
 auto POIs=LWDebug67::Filter(LWDebug67::Catalog(ELWDebugSection67::POIs),TEXT(""),TEXT(""),1,false);TestEqual(TEXT("all POI types included"),POIs.Num(),LWPlaces::Count);
 for(int I=0;I<POIs.Num();I++)TestEqual(TEXT("numeric POI IDs sort numerically"),POIs[I]->Id,FString::FromInt(I));
 for(int I=0;I<5;I++){TSet<FString> Ids;for(const auto& E:LWDebug67::Catalog(ELWDebugSection67(I))){TestFalse(TEXT("catalog IDs are unique within a tab"),Ids.Contains(E->Id));Ids.Add(E->Id);TestFalse(TEXT("display name is present"),E->Name.IsEmpty());TestFalse(TEXT("command is present"),E->Command.IsEmpty());}}
 return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWDebugCommands67,"LethalWorld.Debug67.BoundedCommands",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWDebugCommands67::RunTest(const FString&){
 auto Items=LWDebug67::Catalog(ELWDebugSection67::Items);auto Medkit=Items.FindByPredicate([](const auto& E){return E->Id==TEXT("medkit");});if(!TestNotNull(TEXT("medkit present"),Medkit))return false;
 TestEqual(TEXT("minimum item quantity"),LWDebug67::Command(**Medkit,-8),FString(TEXT("give medkit 1")));TestEqual(TEXT("maximum item quantity"),LWDebug67::Command(**Medkit,MAX_int32),FString(TEXT("give medkit 1000")));
 for(const auto& E:LWDebug67::Catalog(ELWDebugSection67::Cheats))if(E->Id==TEXT("time"))TestEqual(TEXT("midnight remains valid"),LWDebug67::Command(*E,0),FString(TEXT("time 0")));
 auto POIs=LWDebug67::Catalog(ELWDebugSection67::POIs);TestEqual(TEXT("POI locator uses exact numeric ID to avoid shared aliases"),LWDebug67::Command(*POIs[58],5),FString(TEXT("locate 58 5")));
 for(const auto& E:LWDebug67::Catalog(ELWDebugSection67::Vehicles))if(E->Id==TEXT("rv"))TestEqual(TEXT("no extra argument for vehicle spawn"),LWDebug67::Command(*E,100),FString(TEXT("spawnvehicle rv")));
 return true;
}
#endif
