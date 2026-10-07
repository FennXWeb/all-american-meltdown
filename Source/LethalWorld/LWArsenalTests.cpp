#include "LWWeaponMods.h"
#include "LWSaveGame.h"
#include "Kismet/GameplayStatics.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWArsenal24,"LethalWorld.Arsenal24.Definitions",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWArsenal24::RunTest(const FString&){
 TSet<int> Indices;
 for(FName Id:{TEXT("missile_launcher"),TEXT("minigun"),TEXT("sawedoff"),TEXT("desert_eagle"),TEXT("m4"),TEXT("taser"),TEXT("flamethrower")}){
  auto G=LWItems::Make(Id);const auto& D=LWItems::Def(Id);TestTrue(*Id.ToString(),G.Id.IsValid()&&D.Category==TEXT("Weapon")&&D.WeaponIndex>=9&&D.WeaponIndex<=15);Indices.Add(D.WeaponIndex);TestFalse(TEXT("mesh assigned"),D.Mesh.IsNull());
  if(Id==TEXT("sawedoff")){TestEqual(TEXT("two break action barrels"),G.Cylinder.Num(),2);continue;}
  if(Id==TEXT("missile_launcher")||Id==TEXT("taser")){TestTrue(TEXT("direct ammunition without magazine"),D.MagazineType.IsNone());TestEqual(TEXT("direct ammo"),D.AmmoType, FName(Id==TEXT("taser")?TEXT("battery"):TEXT("ammo_rocket")));continue;}
  const auto& M=LWItems::Def(D.MagazineType);TestTrue(TEXT("valid matching magazine"),M.Category==TEXT("Magazine")&&M.AmmoType==D.AmmoType&&M.Capacity==D.Capacity);TestTrue(TEXT("ammunition exists"),LWItems::Def(D.AmmoType).Category==TEXT("Ammo"));
 }
 TestEqual(TEXT("seven distinct weapon indices"),Indices.Num(),7);TestTrue(TEXT("M4 accepts scope"),LWMods::Compatible(TEXT("m4"),TEXT("att_scope4")));TestFalse(TEXT("taser rejects rifle grip"),LWMods::Compatible(TEXT("taser"),TEXT("att_vertical")));return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWFinishes24,"LethalWorld.Arsenal24.ModifiersAndSkins",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWFinishes24::RunTest(const FString&){
 auto G=LWItems::Make(TEXT("m4"));FRandomStream R(724);TSet<FName> Effects;
 for(int T=0;T<5;T++){G.WeaponTier=T;TSet<int> Skins;int Bare=0;for(int I=0;I<1000;I++){LWMods::RollFinish(G,R);if(G.WeaponSkin<0)Bare++;else Skins.Add(G.WeaponSkin);if(T==4)Effects.Add(LWMods::Modifier(&G));else TestTrue(TEXT("lower tiers have no modifier"),LWMods::Modifier(&G).IsNone());}TestEqual(TEXT("new drops reserve camos for challenges"),Skins.Num(),0);TestTrue(TEXT("skins are a chance, not guaranteed"),Bare>0);}
 TestEqual(TEXT("six possible legendary effects"),Effects.Num(),6);
 G.LegendaryModifier=TEXT("Two Shot");TestEqual(TEXT("two shot"),LWMods::Shots(&G),2);G.LegendaryModifier=TEXT("Rapid Fire");TestTrue(TEXT("rapid fire cuts interval"),LWMods::Interval(&G)<.5f);G.LegendaryModifier=TEXT("Recoil Free");TestEqual(TEXT("zero recoil"),LWMods::Recoil(&G),0.f);
 G.WeaponSkin=7;auto* Save=NewObject<ULWSaveGame>();Save->Inventory.Add(G);TArray<uint8> Bytes;TestTrue(TEXT("serialize finish"),UGameplayStatics::SaveGameToMemory(Save,Bytes));auto* Back=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));TestTrue(TEXT("save restores identity, perk and skin"),Back&&Back->Inventory.Num()==1&&Back->Inventory[0].Id==G.Id&&Back->Inventory[0].LegendaryModifier==G.LegendaryModifier&&Back->Inventory[0].WeaponSkin==7);return true;
}
#endif
