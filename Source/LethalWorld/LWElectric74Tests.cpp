#include "LWElectric74.h"
#include "LWVehicleSpec.h"
#include "LWVehicleState.h"
#include "LWGarage45.h"
#include "Misc/AutomationTest.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWElectric74,"LethalWorld.Update74.Electric",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWElectric74::RunTest(const FString&){
 TestTrue(TEXT("No solar generation at night"),LWElectric74::SunHours(19,29)==0);
 TestTrue(TEXT("One full day solar integral"),FMath::Abs(LWElectric74::SunHours(0,24)-24/PI)<.001);
 TestTrue(TEXT("Time skip equals incremental charging"),FMath::Abs(LWElectric74::SunHours(5,55)-(LWElectric74::SunHours(5,20)+LWElectric74::SunHours(20,55)))<.001);
 TestEqual(TEXT("No negative charge on clock rollback"),LWElectric74::SunHours(20,5),0.);
 TestTrue(TEXT("Distinct electric models"),LWTraffic::IsElectric(TEXT("solstice_rv"))&&LWTraffic::IsElectric(TEXT("apex_ev"))&&!LWTraffic::IsElectric(TEXT("rv")));
 TestTrue(TEXT("Roomier solar residence"),LWTraffic::Get(TEXT("solstice_rv")).HalfWidth>LWTraffic::Get(TEXT("rv")).HalfWidth&&LWTraffic::Get(TEXT("solstice_rv")).HalfLength>LWTraffic::Get(TEXT("rv")).HalfLength);
 TestEqual(TEXT("Electric coach reserves three garage bays"),LWGarage45::Bays(TEXT("solstice_rv")),3);
 TestEqual(TEXT("Electric hypercar reserves one garage bay"),LWGarage45::Bays(TEXT("apex_ev")),1);
 FLWVehicleRecord In,Out;In.Model=TEXT("solstice_rv");In.BatteryKWh74=234.5;In.SolarHours74=51.25;In.FuelLitres=19;In.VIN=FGuid::NewGuid();In.Shades66.Add(2);TArray<uint8> Bytes;
 {FMemoryWriter W(Bytes);FObjectAndNameAsStringProxyArchive A(W,false);FLWVehicleRecord::StaticStruct()->SerializeItem(A,&In,nullptr);}
 {FMemoryReader R(Bytes);FObjectAndNameAsStringProxyArchive A(R,false);FLWVehicleRecord::StaticStruct()->SerializeItem(A,&Out,nullptr);}
 TestEqual(TEXT("Persistent battery"),Out.BatteryKWh74,In.BatteryKWh74);TestEqual(TEXT("Persistent charging epoch"),Out.SolarHours74,In.SolarHours74);TestTrue(TEXT("Identity and cabin state retained"),Out.VIN==In.VIN&&Out.Shades66.Contains(2));TestEqual(TEXT("Petrol records unaffected"),Out.FuelLitres,19.f);
 return true;
}
#endif
