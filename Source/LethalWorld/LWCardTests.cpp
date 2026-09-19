#include "LWCardGame.h"
#include "LWAudioCatalog.h"
#include "LWGeneration.h"
#include "LWSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#include "Sound/SoundWave.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWBlackjackTest,"LethalWorld.Taverns.Blackjack",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWBlackjackTest::RunTest(const FString&){
 TestEqual(TEXT("two aces soften"),FLWCardGame::BlackjackValue({12,25,8}),12);TestEqual(TEXT("soft seventeen"),FLWCardGame::BlackjackValue({12,4}),17);
 FLWCardGame G;G.Start(0,20,19);G.Phase=TEXT("play");G.Hands[0].Cards={8,7};G.Hands[1].Cards={12,4};TestTrue(TEXT("stand accepted"),G.Act(TEXT("stand")));TestEqual(TEXT("dealer stands soft17"),G.Hands[1].Cards.Num(),2);TestEqual(TEXT("win returns stake and winnings"),G.Payout,40);TestFalse(TEXT("cannot settle twice"),G.Act(TEXT("stand")));
 G=FLWCardGame();G.Start(0,20,8);G.Phase=TEXT("play");G.Hands[0].Cards={8,7};G.Hands[1].Cards={21,20};G.Act(TEXT("stand"));TestEqual(TEXT("push returns stake"),G.Payout,20);
 G=FLWCardGame();G.Start(0,20,8);G.Phase=TEXT("play");G.Hands[0].Cards={8,7};G.Deck={5};G.Act(TEXT("double"));TestEqual(TEXT("doubling increases stake"),G.Stake,40);TestEqual(TEXT("double bust loses"),G.Payout,0);return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWPitchTest,"LethalWorld.Taverns.PitchRules",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWPitchTest::RunTest(const FString&){FLWCardGame G;G.Start(1,10,4);TestEqual(TEXT("six cards per seat"),G.Hands[3].Cards.Num(),6);G.Trump=3;G.Tricks=1;G.Trick={14};G.Hands[0].Cards={16,42,5};TestTrue(TEXT("follow suit"),G.Legal(0,0));TestTrue(TEXT("trump despite holding led suit"),G.Legal(0,1));TestFalse(TEXT("cannot discard other suit when holding lead"),G.Legal(0,2));G.Hands[0].Cards.RemoveAt(0);TestTrue(TEXT("void permits discard"),G.Legal(0,1));
 G.Scores={0,0};G.Points={1,2};G.GamePoints={10,10};G.Bid=3;G.Bidder=0;G.PitchScore();TestEqual(TEXT("failed bid subtracts bid"),G.Scores[0],-3);TestEqual(TEXT("game tie awards none"),G.Scores[1],2);return true;}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWColorTest,"LethalWorld.Taverns.LastCardRules",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWColorTest::RunTest(const FString&){FLWCardGame G;G.Start(2,10,11);TestEqual(TEXT("108 card deck conserved"),G.Deck.Num()+G.Discard.Num()+G.Hands[0].Cards.Num()+G.Hands[1].Cards.Num(),108);G.Color=0;G.Discard={5};G.Hands[0].Cards={3,74,73,20};TestFalse(TEXT("wild four blocked with current color"),G.Legal(0,1));TestTrue(TEXT("wild legal"),G.Legal(0,2));TestTrue(TEXT("rank matching legal"),G.Legal(0,3));G.Hands[0].Cards={74,20};TestTrue(TEXT("wild four allowed when void of current color"),G.Legal(0,0));
 G.Hands[0].Cards={10,4};G.ColorPlay(0,0,0);TestEqual(TEXT("forgot call draws two"),G.Hands[0].Cards.Num(),3);TestEqual(TEXT("skip grants own next turn"),G.Turn,0);
 G.Hands[0].Cards={11,4};G.Called=true;G.ColorPlay(0,0,0);TestEqual(TEXT("called last card prevents penalty"),G.Hands[0].Cards.Num(),1);TestEqual(TEXT("two-player reverse skips"),G.Turn,0);
 G.Hands[0].Cards={12};int N=G.Hands[1].Cards.Num();G.ColorPlay(0,0,0);TestEqual(TEXT("final draw penalty applied"),G.Hands[1].Cards.Num(),N+2);TestEqual(TEXT("empty hand wins"),G.Payout,20);return true;}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWCardSimulationTest,"LethalWorld.Taverns.MatchesAndSaveResume",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWCardSimulationTest::RunTest(const FString&){
 for(int Type=0;Type<3;Type++)for(int Seed=0;Seed<200;Seed++){
 FLWCardGame G;G.Paced=Seed%2==0;G.Start(Type,20,Seed);int Steps=0;
 while(G.Active()&&Steps++<5000){bool Changed=false;
 if(G.AutoTurn())Changed=G.Act(TEXT("tick"));
 else if(Type==0)Changed=G.Act(FLWCardGame::BlackjackValue(G.Hands[0].Cards)<17?TEXT("hit"):TEXT("stand"));
 else if(Type==1){if(G.Phase==TEXT("bid"))Changed=G.Act(TEXT("bid"),G.Bid<4?FMath::Max(2,G.Bid+1):0);else if(G.Phase==TEXT("trump"))Changed=G.Act(TEXT("color"),G.Hands[0].Cards[0]/13);else if(G.Phase==TEXT("trick")||G.Phase==TEXT("hand"))Changed=G.Act(TEXT("next"));else{for(int I=0;I<G.Hands[0].Cards.Num();I++)if(G.Legal(0,I)){Changed=G.Act(TEXT("card"),I);break;}}}
 else {if(G.Phase==TEXT("color"))Changed=G.Act(TEXT("color"),Seed%4);else{if(G.Hands[0].Cards.Num()==2)G.Act(TEXT("call"));for(int I=0;I<G.Hands[0].Cards.Num();I++)if(G.Legal(0,I)){Changed=G.Act(TEXT("card"),I);break;}if(!Changed)Changed=G.Act(G.Drawn?TEXT("pass"):TEXT("draw"));}}
 if(!Changed){AddError(FString::Printf(TEXT("Stuck game=%d seed=%d phase=%s"),Type,Seed,*G.Phase.ToString()));break;}
 if(Type==2){int Total=G.Deck.Num()+G.Discard.Num();for(auto& H:G.Hands)Total+=H.Cards.Num();if(Total!=108){AddError(TEXT("Card conservation failed"));break;}}
 }TestFalse(TEXT("match finishes without deadlock"),G.Active());TestTrue(TEXT("payout bounded"),G.Payout>=0&&G.Payout<=50);
 }
 auto* Save=NewObject<ULWSaveGame>();Save->Cards.Start(1,50,31);Save->Cards.Act(TEXT("bid"),3);TArray<uint8> Bytes;TestTrue(TEXT("save serializes"),UGameplayStatics::SaveGameToMemory(Save,Bytes));auto* Restored=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));TestNotNull(TEXT("save restores"),Restored);if(Restored){TestEqual(TEXT("phase preserved"),Restored->Cards.Phase,Save->Cards.Phase);TestTrue(TEXT("deck order preserved"),Restored->Cards.Deck==Save->Cards.Deck);TestEqual(TEXT("random state preserved"),Restored->Cards.Random.GetCurrentSeed(),Save->Cards.Random.GetCurrentSeed());}return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWTavernPlacementTest,"LethalWorld.Taverns.SettlementVariation",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWTavernPlacementTest::RunTest(const FString&){TArray<LWGen::FRoad> HomeRoads;TArray<LWGen::FSite> HomeSites;LWGen::Region({0,0},198706,HomeRoads,HomeSites);TestTrue(TEXT("settlement with a cleared plot receives its requested tavern"),!HomeSites.ContainsByPredicate([](const auto& S){return S.SettlementBuilding;})||HomeSites.ContainsByPredicate([](const auto& S){return S.Type==LWPlaces::Tavern;}));int Taverns=0,Other=0,Games=0,NoGames=0;for(int X=-10;X<=10;X++)for(int Y=-10;Y<=10;Y++){TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Region({X,Y},198706,Roads,Sites);for(const auto& S:Sites)if(S.Type==LWPlaces::Tavern){Taverns++;TestTrue(TEXT("tavern stays friendly"),S.Friendly&&S.SettlementBuilding);if(S.Id%4)Games++;else NoGames++;}else if(S.SettlementBuilding&&S.Type==4)Other++;}TestTrue(TEXT("taverns and ordinary diners both appear"),Taverns>0&&Other>0);TestTrue(TEXT("gaming and social taverns both appear"),Games>0&&NoGames>0);return true;}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWPlaylistTest,"LethalWorld.Audio.MusicPlaylists",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWPlaylistTest::RunTest(const FString&){auto* Catalog=NewObject<ULWAudioCatalog>();auto* A=LoadObject<USoundBase>(nullptr,TEXT("/Game/Audio/S_MenuMusic.S_MenuMusic"));auto* B=LoadObject<USoundBase>(nullptr,TEXT("/Game/Audio/S_CombatMusic.S_CombatMusic"));if(!A||!B){AddError(TEXT("Music fixtures missing"));return false;}FLWAudioSlot Slot;Slot.bMusic=true;Slot.Source=A;Slot.Tracks={A,B,B};Catalog->Slots.Add(TEXT("TestMusic"),Slot);TSet<USoundBase*> Seen;for(int I=0;I<80;I++){FLWResolvedAudioSlot R;TestTrue(TEXT("playlist resolves"),Catalog->ResolveSlot(TEXT("TestMusic"),R,false));Seen.Add(R.Sound);}TestEqual(TEXT("both tracks selected"),Seen.Num(),2);Slot.Tracks={TSoftObjectPtr<USoundBase>()};Catalog->Slots[TEXT("TestMusic")]=Slot;FLWResolvedAudioSlot R;TestTrue(TEXT("empty playlist falls back to assigned source"),Catalog->ResolveSlot(TEXT("TestMusic"),R,false));TestTrue(TEXT("fallback preserved"),R.Sound==A);Slot.bMusic=false;Slot.Tracks={B};Catalog->Slots[TEXT("TestMusic")]=Slot;Catalog->ResolveSlot(TEXT("TestMusic"),R,false);TestTrue(TEXT("SFX tracks take priority over source"),R.Sound==B);return true;}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWCardPacingTest,"LethalWorld.Taverns.PacedSplitAndAuction",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWCardPacingTest::RunTest(const FString&){
 FLWCardGame G;G.Paced=true;G.Start(0,20,9);G.Phase=TEXT("play");G.Hands[0].Cards={6,19};G.Hands[1].Cards={8,5};G.Deck={8,8,3,2};
 TestTrue(TEXT("split allowed for pair"),G.CanSplit());TestEqual(TEXT("split costs original bet"),G.ExtraCost(TEXT("split")),20);G.Act(TEXT("split"));TestEqual(TEXT("three hands including dealer"),G.Hands.Num(),3);TestEqual(TEXT("split stake"),G.Stake,40);TestFalse(TEXT("no resplitting"),G.CanSplit());
 G.Act(TEXT("double"));TestEqual(TEXT("double adds only active hand bet"),G.Stake,60);TestEqual(TEXT("second hand now active"),G.ActiveHand,2);G.Act(TEXT("stand"));TestTrue(TEXT("dealer waits for paced turn"),G.AutoTurn());G.Act(TEXT("tick"));TestEqual(TEXT("round settled"),G.Phase,FName(TEXT("done")));TestEqual(TEXT("first hand bust second loses"),G.Payout,0);
 G=FLWCardGame();G.Start(0,20,9);G.Phase=TEXT("play");G.Hands[0].Cards={2,5};TestTrue(TEXT("surrender accepted"),G.Act(TEXT("surrender")));TestEqual(TEXT("half stake returned"),G.Payout,10);
 G=FLWCardGame();G.Paced=true;G.Start(1,20,42);TestEqual(TEXT("deal emits 24 card moves"),G.Moves.Num(),24);TestEqual(TEXT("auction starts left of dealer"),G.AuctionSeat,(G.Dealer+1)%4);TestFalse(TEXT("cannot underbid"),G.Act(TEXT("bid"),1));TestTrue(TEXT("player opening pass accepted"),G.Act(TEXT("bid"),0));TestFalse(TEXT("player cannot bid out of turn"),G.Act(TEXT("bid"),4));G.Act(TEXT("tick"));TestEqual(TEXT("only one opponent bids per tick"),G.AuctionCount,2);G.Act(TEXT("tick"));G.Act(TEXT("tick"));TestEqual(TEXT("four seats bid once"),G.AuctionCount,4);
 auto* Save=NewObject<ULWSaveGame>();Save->Cards=G;TArray<uint8> Bytes;UGameplayStatics::SaveGameToMemory(Save,Bytes);auto* Restored=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));TestNotNull(TEXT("paced save restores"),Restored);if(Restored){TestTrue(TEXT("pacing survives save"),Restored->Cards.Paced);TestEqual(TEXT("auction survives save"),Restored->Cards.AuctionCount,G.AuctionCount);TestEqual(TEXT("animation serial survives save"),Restored->Cards.MoveSerial,G.MoveSerial);}return true;
}
#endif
