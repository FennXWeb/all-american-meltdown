#include "Misc/AutomationTest.h"
#include "LWWeaponMods.h"
#include "LWSpawnTable.h"
#include "LWPOITypes.h"
#include "LWVehicleSpec.h"
#include "LWSaveGame.h"
#include "Kismet/GameplayStatics.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWV14Equipment,"LethalWorld.V14.WeaponPayload",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWV14Equipment::RunTest(const FString&){
 auto G=LWItems::Make(TEXT("rifle"));G.WeaponTier=4;G.Attachments.Add(TEXT("Optic"),TEXT("att_scope4"));G.Attachments.Add(TEXT("Grip"),TEXT("att_vertical"));G.Chamber=1;
 TArray<FLWItemInstance> A,B;TestTrue(TEXT("gun fits"),LWItems::Place(A,G,12,10));TestTrue(TEXT("transfer preserves entire gun"),LWItems::Transfer(A,B,G.Id,0,0,false,12,10));TestEqual(TEXT("rarity retained"),B[0].WeaponTier,4);TestEqual(TEXT("attachments retained"),B[0].Attachments.Num(),2);
 TestTrue(TEXT("sort succeeds"),LWItems::AutoSort(B,12,10));TestTrue(TEXT("scope removes into grid"),LWMods::Detach(B,G.Id,TEXT("Optic")));TestEqual(TEXT("detachment creates exactly one attachment"),B.Num(),2);TestFalse(TEXT("duplicate detach impossible"),LWMods::Detach(B,G.Id,TEXT("Optic")));
 auto* Gun=B.FindByPredicate([&](const auto& I){return I.Id==G.Id;});TestEqual(TEXT("ammo unaffected"),Gun->Chamber,1);TestEqual(TEXT("other mount retained"),Gun->Attachments.FindRef(TEXT("Grip")),FName(TEXT("att_vertical")));
 for(int T=0;T<4;T++){G.WeaponTier=T;float D=LWMods::Damage(&G),S=LWMods::Spread(&G,true),R=LWMods::Recoil(&G);G.WeaponTier++;TestTrue(TEXT("tiers improve damage accuracy and recoil"),LWMods::Damage(&G)>D&&LWMods::Spread(&G,true)<S&&LWMods::Recoil(&G)<R);}
 for(FName W:{FName(TEXT("crowbar")),FName(TEXT("bat"))})for(FName M:{FName(TEXT("att_scope4")),FName(TEXT("att_light")),FName(TEXT("att_vertical"))})TestFalse(TEXT("melee has no gun mounts"),LWMods::Compatible(W,M));
 TestFalse(TEXT("revolver rejects grip"),LWMods::Compatible(TEXT("revolver"),TEXT("att_vertical")));TestTrue(TEXT("revolver supports reflex"),LWMods::Compatible(TEXT("revolver"),TEXT("att_reflex")));TestFalse(TEXT("SMG rejects eight power"),LWMods::Compatible(TEXT("smg"),TEXT("att_scope8")));
 TArray<FLWItemInstance> Full;G.X=G.Y=-1;G.Slot=TEXT("Primary");Full.Add(G);for(int Y=0;Y<10;Y++)for(int X=0;X<12;X++){auto I=LWItems::Make(TEXT("att_light"));I.X=X;I.Y=Y;Full.Add(I);}TestFalse(TEXT("full grid detach fails atomically"),LWMods::Detach(Full,G.Id,TEXT("Optic")));TestEqual(TEXT("full grid keeps gun attachments"),Full[0].Attachments.Num(),2);
 return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWV14Spawn,"LethalWorld.V14.SpawnAndSeatDefinitions",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWV14Spawn::RunTest(const FString&){auto* C=NewObject<ULWSpawnTable>();TestEqual(TEXT("every POI has a table"),C->POIs.Num(),LWPlaces::Count);TSet<int> Types;for(const auto& R:C->POIs){Types.Add(R.POIType);TestTrue(TEXT("bounded spawn counts"),R.MinCount>=0&&R.MaxCount>=R.MinCount&&R.MaxCount<=8);if(R.POIType==18||R.POIType==19)TestEqual(TEXT("boutique/tavern no generic enemies"),R.MaxCount,0);}TestEqual(TEXT("unique table keys"),Types.Num(),LWPlaces::Count);TestTrue(TEXT("wilderness spawns rarely"),C->WildernessChance>0&&C->WildernessChance<.2f);TestEqual(TEXT("RV driver plus four passengers"),LWTraffic::Get(TEXT("rv")).Seats,5);TestTrue(TEXT("RV has residential storage"),LWTraffic::Get(TEXT("rv")).CargoH>=24);return true;}
