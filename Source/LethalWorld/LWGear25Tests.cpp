#include "Misc/AutomationTest.h"
#include "LWInventory.h"
#include "LWGeneration.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWGearCapacity25,"LethalWorld.Equipment.CapacityAndDependencies",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWGearCapacity25::RunTest(const FString&){
 TArray<FLWItemInstance> Items;
 TestEqual(TEXT("base grid"),LWItems::InventoryHeight(Items),10);
 for(FName Name:{TEXT("sling_pack"),TEXT("backpack"),TEXT("hiking_pack"),TEXT("military_pack"),TEXT("expedition_pack")}){
  Items.Empty();auto B=LWItems::Make(Name);Items.Add(B);TestTrue(TEXT("pack equips"),LWItems::Equip(Items,B.Id,TEXT("Backpack")));
  const int H=LWItems::InventoryHeight(Items);TestTrue(TEXT("pack adds rows"),H>10);
  auto A=LWItems::Make(TEXT("ammo_9mm"));TestTrue(TEXT("place in last extra row"),LWItems::Place(Items,A,12,H));
  TestTrue(TEXT("move into extra row"),LWItems::Move(Items,A.Id,0,H-1,false,12,H));TestTrue(TEXT("valid expanded grid"),LWItems::ValidateEquipment(Items));
  Items.RemoveAll([&](const auto& I){return I.Id==B.Id;});TestFalse(TEXT("pack removal would invalidate capacity"),LWItems::ValidateEquipment(Items));
 }
 Items.Empty();auto Rig=LWItems::Make(TEXT("rig")),Gun=LWItems::Make(TEXT("m4"));Items.Add(Gun);
 TestFalse(TEXT("no rig no extra weapon"),LWItems::Equip(Items,Gun.Id,TEXT("RigPrimary")));Items.Add(Rig);
 TestTrue(TEXT("rig equips"),LWItems::Equip(Items,Rig.Id,TEXT("Rig")));TestTrue(TEXT("extra primary accepted"),LWItems::Equip(Items,Gun.Id,TEXT("RigPrimary")));
 TestTrue(TEXT("valid rig loadout"),LWItems::ValidateEquipment(Items));Items.RemoveAll([&](const auto& I){return I.Id==Rig.Id;});TestFalse(TEXT("cannot remove occupied rig"),LWItems::ValidateEquipment(Items));
 auto NV=LWItems::Make(TEXT("night_vision"));TestTrue(TEXT("goggles use helmet"),LWItems::CanEquip(NV,TEXT("Helmet")));TestFalse(TEXT("goggles not armor"),LWItems::CanEquip(NV,TEXT("Armor")));
 return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWParcel25,"LethalWorld.Generation.DevelopedGroundClearance",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWParcel25::RunTest(const FString&){
 TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::FSite S;S.Position=FVector2D(120000,-85000);S.Size=FVector2D(11000,10000);S.Type=21;S.Yaw=37;Sites.Add(S);
 for(int X=-5500;X<=5500;X+=500)for(int Y=-9400;Y<=5000;Y+=500){FVector2D P=S.Position+FVector2D(X,Y).GetRotated(S.Yaw);TestEqual(TEXT("casino building and full parking on flat ground"),LWGen::Height(P,Roads,Sites),0.f);}
 return true;
}
#endif
