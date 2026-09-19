#include "Misc/AutomationTest.h"
#include "LWGarage45.h"
#include "LWBunker45State.h"
#include "LWSaveGame.h"
#include "Kismet/GameplayStatics.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWGarage45Catalog,"LethalWorld.Bunker45.ModCatalogAndBays",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWGarage45Catalog::RunTest(const FString&){
 TestEqual(TEXT("upgrade count"),LWGarage45::Mods().Num(),50);TSet<FName> Unique;int Exclusive=0;for(const auto& M:LWGarage45::Mods()){TestTrue(TEXT("positive effect and cost"),M.Value>0&&M.Cost>0);Unique.Add(M.Id);Exclusive+=!M.Models.IsEmpty();}TestEqual(TEXT("unique ids"),Unique.Num(),50);TestEqual(TEXT("exclusive upgrades"),Exclusive,20);
 TMap<FName,FLWVehicleRecord> Cars;for(int I=0;I<10;++I){TestEqual(TEXT("next free single bay"),LWGarage45::FreeBay(Cars,TEXT("sedan")),I);FLWVehicleRecord R;R.Owned45=true;R.Model=TEXT("sedan");R.GarageBay45=I;Cars.Add(FName(*FString::FromInt(I)),R);}TestEqual(TEXT("full garage rejects more cars"),LWGarage45::FreeBay(Cars,TEXT("sedan")),-1);
 Cars.Remove(TEXT("1"));Cars.Remove(TEXT("3"));Cars.Remove(TEXT("5"));TestEqual(TEXT("fragmented bays reject large vehicle"),LWGarage45::FreeBay(Cars,TEXT("rv")),-1);Cars.Remove(TEXT("2"));TestEqual(TEXT("three contiguous bays accept bus"),LWGarage45::FreeBay(Cars,TEXT("bus")),1);
 FLWVehicleRecord R;R.Model=TEXT("sedan");R.Mods45.Add(TEXT("mod45_49"));TestEqual(TEXT("incompatible saved mod grants no effect"),LWGarage45::Stat(&R,TEXT("power")),0.f);R.Mods45.Add(TEXT("mod45_02"));TestTrue(TEXT("compatible mod changes stat"),LWGarage45::Stat(&R,TEXT("power"))>.3f);return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWBunker45Save,"LethalWorld.Bunker45.Persistence",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWBunker45Save::RunTest(const FString&){
 auto* S=NewObject<ULWSaveGame>();TestEqual(TEXT("legacy default capacity"),S->Bunker45.Bedrooms(),10);S->Bunker45.BedroomFloors=9;S->Bunker45.Garage=true;S->Bunker45.WorkSeconds=151;S->Bunker45.Utilities.Add(TEXT("trade"));S->Bunker45.Assignments.Add(TEXT("crew"),TEXT("trade"));auto& F=S->Bunker45.Furniture.FindOrAdd(TEXT("custom"));F.Model=TEXT("ChairV3");F.Cost=5;F.Removed=true;F.Transform=FTransform(FVector(1,2,-2000));auto& V=S->Vehicles.FindOrAdd(TEXT("owned"));V.VIN=FGuid::NewGuid();V.Owned45=V.Stored45=true;V.GarageBay45=3;V.Mods45.Add(TEXT("mod45_02"));V.Paint45=8;
 TArray<uint8> Data;TestTrue(TEXT("serialize"),UGameplayStatics::SaveGameToMemory(S,Data));auto* R=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Data));if(!TestNotNull(TEXT("deserialize"),R))return false;
 TestEqual(TEXT("capacity roundtrip"),R->Bunker45.Bedrooms(),100);TestTrue(TEXT("dismantled furniture stays removed"),R->Bunker45.Furniture.FindRef(TEXT("custom")).Removed);TestTrue(TEXT("assignment roundtrip"),R->Bunker45.Assignments.FindRef(TEXT("crew"))==TEXT("trade"));TestEqual(TEXT("passive work progress roundtrip"),R->Bunker45.WorkSeconds,151.);const auto* C=R->Vehicles.Find(TEXT("owned"));TestTrue(TEXT("ownership upgrades paint and VIN survive"),C&&C->Owned45&&C->Stored45&&C->GarageBay45==3&&C->Mods45.Contains(TEXT("mod45_02"))&&C->Paint45==8&&C->VIN==V.VIN);return true;
}
