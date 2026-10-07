#include "LWArsenal62.h"
#include "LWWeaponMods.h"
#include "LWSaveGame.h"
#include "LWSaveSlots62.h"
#include "LWLootTable.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWArsenalMigration62,"LethalWorld.Arsenal62.MigrationAndPersistence",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWArsenalMigration62::RunTest(const FString&){auto G=LWItems::Make(TEXT("rifle"));G.WeaponTier=4;G.LegendaryModifier=TEXT("Toxic");G.Chamber=1;G.Parts39=LWParts39::Recipe(G);auto A=LWParts39::Part(LWItems::Make(TEXT("att_reflex")));G.Parts39.Add(A);auto Id=G.Id;TArray<FLWItemInstance> Items={G,LWItems::Make(TEXT("part_barrel_09"))};LWArsenal62::Migrate(Items);TestEqual(TEXT("base weapon retained"),Items[0].Definition,FName(TEXT("rifle")));TestEqual(TEXT("chamber retained"),Items[0].Chamber,1);TestTrue(TEXT("identity retained"),Items[0].Id==Id);TestTrue(TEXT("free assembly removed"),Items[0].Parts39.IsEmpty());TestEqual(TEXT("compatible attachment recovered"),Items[0].Attachments.FindRef(TEXT("Optic")),FName(TEXT("att_reflex")));TestEqual(TEXT("loose parts become scrap"),Items[1].Definition,FName(TEXT("scrap")));
 auto* Save=NewObject<ULWSaveGame>();Items[0].Camo62=9;Save->Inventory=Items;Save->RPG.WeaponKills62.Add(TEXT("rifle"),250);TArray<uint8> Bytes;TestTrue(TEXT("encode new save fields"),UGameplayStatics::SaveGameToMemory(Save,Bytes));auto* Back=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));if(TestNotNull(TEXT("decode"),Back)){TestEqual(TEXT("mystic persists"),Back->Inventory[0].Camo62,9);TestEqual(TEXT("challenge progress persists"),Back->RPG.WeaponKills62.FindRef(TEXT("rifle")),250);TestEqual(TEXT("legendary gameplay effect preserved"),Back->Inventory[0].LegendaryModifier,FName(TEXT("Toxic")));}return true;}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWArsenalRules62,"LethalWorld.Arsenal62.MountsAndLoot",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWArsenalRules62::RunTest(const FString&){TSet<FString> Names;for(int W=0;W<21;W++){Names.Add(LWArsenal62::MysticName(W));TestTrue(TEXT("valid theme"),LWArsenal62::Theme(W)>=0&&LWArsenal62::Theme(W)<8);}TestEqual(TEXT("all weapons have unique mystic names"),Names.Num(),21);auto G=LWItems::Make(TEXT("rifle"));G.Attachments.Add(TEXT("Muzzle"),TEXT("att_brake62"));TestTrue(TEXT("compatible muzzle"),LWMods::Compatible(G.Definition,TEXT("att_brake62")));TestFalse(TEXT("melee has no gun muzzle"),LWMods::Compatible(TEXT("bat"),TEXT("att_brake62")));TestTrue(TEXT("brake reduces recoil"),LWMods::Recoil(&G)<1);TestTrue(TEXT("brake has spread tradeoff"),LWMods::Spread(&G,false)>1);int NewMods=0;for(int Seed=1;Seed<80;Seed++)for(const auto& I:GetDefault<ULWLootTable>()->Roll(TEXT("military"),Seed,.8f)){TestFalse(TEXT("no individual weapon parts in loot"),LWItems::Def(I.Definition).Category==TEXT("WeaponPart"));if(LWArsenal62::Find(I.Definition))++NewMods;}TestTrue(TEXT("new attachments are obtainable"),NewMods>0);TestFalse(TEXT("path traversal rejected"),LWSaves62::Managed(TEXT("../bad")));TestTrue(TEXT("manual managed"),LWSaves62::Managed(LWSaves62::Manual(1)));TestTrue(TEXT("autosave separate"),LWSaves62::Manual(1)!=LWSaves62::Auto());return true;}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWArsenalSlots62,"LethalWorld.Arsenal62.DiskSlotIsolation",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWArsenalSlots62::RunTest(const FString&){
 if(!LWSaves62::Auto().StartsWith(TEXT("AAM_Test62_"))){AddWarning(TEXT("Use -LWArsenal62Tests to enable isolated disk tests."));return true;}
 auto* S=NewObject<ULWSaveGame>();const FString Slot=LWSaves62::Manual(19);TArray<uint8> Bytes;
 S->Money=62;UGameplayStatics::SaveGameToMemory(S,Bytes);TestTrue(TEXT("first manual write"),LWSaves62::Write(Bytes,Slot,TEXT("First")));
 S->Money=162;UGameplayStatics::SaveGameToMemory(S,Bytes);TestTrue(TEXT("replace manual write"),LWSaves62::Write(Bytes,Slot,TEXT("Second")));
 TArray<uint8> Backup;TestTrue(TEXT("previous snapshot retained"),FFileHelper::LoadFileToArray(Backup,*(FPaths::ProjectSavedDir()/TEXT("SaveGames")/(Slot+TEXT(".bak")))));
 auto* Old=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Backup));TestTrue(TEXT("backup contains previous state"),Old&&Old->Money==62);
 S->Money=262;UGameplayStatics::SaveGameToMemory(S,Bytes);TestTrue(TEXT("independent autosave write"),LWSaves62::Write(Bytes,LWSaves62::Auto(),TEXT("Auto")));
 auto* Manual=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromSlot(Slot,0));auto* Auto=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromSlot(LWSaves62::Auto(),0));
 TestTrue(TEXT("autosave preserves manual state"),Manual&&Manual->Money==162);TestTrue(TEXT("autosave has separate state"),Auto&&Auto->Money==262);return true;
}
#endif
