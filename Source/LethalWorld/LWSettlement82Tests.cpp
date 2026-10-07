#include "Misc/AutomationTest.h"
#include "LWSettlement82.h"
#include "LWSaveGame.h"
#include "LWLootTable.h"
#include "Kismet/GameplayStatics.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWBuilding82Rules,"LethalWorld.Settlement82.SnapGeometryAndCatalog",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWBuilding82Rules::RunTest(const FString&){
 const auto& C=LWBuilding82::Catalog();TestTrue(TEXT("complete catalog"),C.Num()>=55);TSet<FName> Seen;int Jobs=0;
 for(const auto& D:C){TestFalse(TEXT("unique catalog identity"),Seen.Contains(D.Id));Seen.Add(D.Id);TestTrue(TEXT("scrap cost and positive dimensions"),D.Cost>0&&D.Size.GetMin()>0);Jobs+=D.Job();}
 TestEqual(TEXT("nine functional jobs"),Jobs,9);
 TestTrue(TEXT("rotated overlapping footprints"),LWBuilding82::Overlap(FTransform(FRotator(0,45,0),FVector::ZeroVector),{200,200,50},FTransform(FVector(200,0,0)),{200,200,50}));
 TestFalse(TEXT("adjacent foundation seam does not overlap"),LWBuilding82::Overlap(FTransform::Identity,{200,200,60},FTransform(FVector(400,0,0)),{200,200,60}));
 TestFalse(TEXT("separate vertical floors do not overlap"),LWBuilding82::Overlap(FTransform::Identity,{200,200,10},FTransform(FVector(0,0,320)),{200,200,10}));
 for(int Angle:{0,27,89,143})for(int Side:{-1,1}){FVector2D Tangent=FVector2D(1,0).GetRotated(Angle),Normal(-Tangent.Y,Tangent.X);LWGen::FRoad R{Tangent*-5000,Tangent*5000,900,false};FTransform T=LWBuilding82::RoadSnap(FVector(Tangent*300+Normal*Side*800,40),R,{400,400,120});LWGen::FSite S;S.Position=FVector2D(T.GetLocation());S.Yaw=T.Rotator().Yaw;S.Size={400,400};TestFalse(TEXT("road edge snapping preserves lanes and shoulder at arbitrary headings"),LWGen::RoadOverlaps(S,R));TestTrue(TEXT("road alignment"),FMath::Abs(FMath::FindDeltaAngleDegrees(T.Rotator().Yaw,float(Angle)))<.01f);}
 FLWClaim82 Claim;FLWConstruction82 Foundation;Foundation.Id=TEXT("base");Foundation.Catalog=TEXT("foundation");Claim.Pieces.Add(Foundation);FLWConstruction82 Wall;Wall.Id=TEXT("wall");Wall.Catalog=TEXT("wall");Wall.Support=Foundation.Id;Claim.Pieces.Add(Wall);TestTrue(TEXT("support cannot be dismantled under attached wall"),LWBuilding82::DependsOn(Claim,Foundation.Id));TestEqual(TEXT("no accidental bed capacity"),LWBuilding82::Beds(Claim),0);Wall.Catalog=TEXT("bed");Claim.Pieces.Add(Wall);TestEqual(TEXT("beds determine capacity"),LWBuilding82::Beds(Claim),1);
 return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWBuilding82Save,"LethalWorld.Settlement82.SaveAndLoot",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWBuilding82Save::RunTest(const FString&){
 auto* S=NewObject<ULWSaveGame>();auto& C=S->RPG.Claims82.FindOrAdd(TEXT("test82"));C.Id=TEXT("test82");C.Name=TEXT("Hearthstead");C.Scrap=500;C.Treasury=90;C.Broadcast=false;C.RecruitmentSeconds=231;C.WorkSeconds=177;C.Contracts.Add(TEXT("q1"));
 FLWConstruction82 P;P.Id=TEXT("wall");P.Catalog=TEXT("window");P.Support=TEXT("foundation");P.Outside=4;P.Inside=2;P.OutsideColor=5;P.InsideColor=3;P.Paid=16;P.Transform=FTransform(FRotator(0,42,0),FVector(100,400,20));C.Pieces.Add(P);
 FLWSettler82 N;N.Id=TEXT("june");N.Name=TEXT("June Reed");N.Station=TEXT("farm");N.Bed=TEXT("bed");C.Residents.Add(N);auto& V=S->Vehicles.FindOrAdd(TEXT("car"));V.VIN=FGuid::NewGuid();V.Owned45=true;V.HomeSettlement82=C.Id;V.HomeParking82=TEXT("lot");V.ParkingBay82=3;V.Mods45.Add(TEXT("mod45_02"));V.Paint45=4;V.Replacement45=12;
 auto& Box=S->WorldContainers.FindOrAdd(TEXT("car"));Box.Id=TEXT("car");Box.Items.Add(LWItems::Make(TEXT("medkit"),2));
 TArray<uint8> Bytes;TestTrue(TEXT("serialize all claim data"),UGameplayStatics::SaveGameToMemory(S,Bytes));auto* R=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));if(!TestNotNull(TEXT("round trip"),R))return false;
 const auto& Out=R->RPG.Claims82.FindChecked(C.Id);TestTrue(TEXT("economy timers and balances survive"),Out.Scrap==500&&Out.Treasury==90&&Out.WorkSeconds==177&&Out.RecruitmentSeconds==231&&!Out.Broadcast);TestTrue(TEXT("structure and independent face finishes survive"),Out.Pieces.Num()==1&&Out.Pieces[0].Support==P.Support&&Out.Pieces[0].Inside==2&&Out.Pieces[0].Outside==4&&Out.Pieces[0].InsideColor==3&&Out.Pieces[0].Transform.Equals(P.Transform));TestTrue(TEXT("resident job and bed survive"),Out.Residents[0].Station==N.Station&&Out.Residents[0].Bed==N.Bed);TestTrue(TEXT("insurance VIN upgrades cargo persist together"),R->Vehicles[TEXT("car")].VIN==V.VIN&&R->Vehicles[TEXT("car")].HomeParking82==V.HomeParking82&&R->Vehicles[TEXT("car")].ParkingBay82==3&&R->WorldContainers[TEXT("car")].Items[0].Count==2);
 TestTrue(TEXT("flag is a usable inventory item"),LWItems::Def(TEXT("settlement_flag")).Width==2&&LWItems::Def(TEXT("settlement_flag")).Price>0);
 auto* Loot=NewObject<ULWLootTable>();bool FoundFlag=false;int Scrap=0;for(int Seed=0;Seed<150;Seed++)for(const auto& I:Loot->Roll(TEXT("depot"),Seed,0)){FoundFlag|=I.Definition==TEXT("settlement_flag");if(I.Definition==TEXT("scrap"))Scrap+=I.Count;}
 TestTrue(TEXT("flags can be found in world loot"),FoundFlag);TestTrue(TEXT("increased scrap supplies support construction"),Scrap>500);for(int Seed=0;Seed<5;Seed++)TestTrue(TEXT("traders sell flags"),Loot->Roll(TEXT("trader"),Seed,0).ContainsByPredicate([](const auto& I){return I.Definition==TEXT("settlement_flag");}));return true;
}
