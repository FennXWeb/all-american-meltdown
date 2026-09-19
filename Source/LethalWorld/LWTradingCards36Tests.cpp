#include "LWTradingCards36.h"
#include "LWRPG.h"
#include "LWSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWCollect36Test,"LethalWorld.TradingCards36.CollectionPersistenceAndRarity",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWCollect36Test::RunTest(const FString&){
 TestEqual(TEXT("complete deck"),LWCollect36::Deck().Num(),100);TSet<FString> Names;TSet<int> Indices;int Tiers[5]={};
 for(const auto& C:LWCollect36::Deck()){Names.Add(C.Name);Indices.Add(C.Index);TestTrue(TEXT("valid positive permanent bonus"),C.Value>0&&C.Rarity>=0&&C.Rarity<5);Tiers[C.Rarity]++;FLWRPGState S;TestTrue(TEXT("first acquisition accepted"),LWCollect36::Collect(S,C.Index));TestFalse(TEXT("duplicate gives no second bonus"),LWCollect36::Collect(S,C.Index));TestEqual(TEXT("bonus integrated with live RPG stats"),LWRPG::Stat(S,C.Effect),C.Value);}
 TestEqual(TEXT("unique names"),Names.Num(),100);TestEqual(TEXT("unique indices"),Indices.Num(),100);for(int I=0;I<5;I++)TestTrue(TEXT("all rarity tiers populated"),Tiers[I]>0);
 FLWRPGState S;TestFalse(TEXT("invalid index rejected"),LWCollect36::Collect(S,-1));TestFalse(TEXT("invalid high index rejected"),LWCollect36::Collect(S,100));for(int I=0;I<100;I++)LWCollect36::Collect(S,I);
 auto* Save=NewObject<ULWSaveGame>();Save->RPG=S;TArray<uint8> Bytes;TestTrue(TEXT("save collection"),UGameplayStatics::SaveGameToMemory(Save,Bytes));auto* Loaded=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));TestNotNull(TEXT("load collection"),Loaded);if(Loaded){TestEqual(TEXT("all cards survive save/load"),Loaded->RPG.TradingCards36.Num(),100);TestEqual(TEXT("health bonus survives save/load"),LWCollect36::Bonus(Loaded->RPG,TEXT("health")),LWCollect36::Bonus(S,TEXT("health")));}
 FLWRPGState Fresh;TestEqual(TEXT("new and legacy default state starts empty"),Fresh.TradingCards36.Num(),0);
 int NormalLegend=0,DangerLegend=0;TSet<int> Reachable;for(uint32 I=0;I<100000;I++){int A=LWCollect36::Select(I*2654435761u);int B=LWCollect36::Select(I*2654435761u,1);NormalLegend+=LWCollect36::Deck()[A].Rarity==4;DangerLegend+=LWCollect36::Deck()[B].Rarity==4;Reachable.Add(A);if(I<100)TestEqual(TEXT("selection deterministic"),A,LWCollect36::Select(I*2654435761u));}TestEqual(TEXT("every card reachable"),Reachable.Num(),100);TestTrue(TEXT("dangerous POIs improve legendary odds"),DangerLegend>NormalLegend*2);
 return true;
}
#endif
