#include "LWCardGame.h"

FString FLWCardGame::Title(int32 Type){return Type==0?TEXT("BLACKJACK"):Type==1?TEXT("PITCH"):TEXT("LAST CARD");}
FString FLWCardGame::Rules(int32 Type){
 if(Type==0)return TEXT("Beat the dealer without going over 21. Aces count as 1 or 11.\nDealer stands on soft 17. Blackjack pays 3:2; wins pay 1:1.\nDouble on either initial hand; split one pair into two hands.\nSplit aces get one card each. Split 21 pays 1:1. No resplitting or insurance.\nLate surrender returns half your original wager. Ties push.");
 if(Type==1)return TEXT("Four-point partnership Pitch. You + North against East + West.\nSix cards each; clockwise auction starts left of the rotating dealer.\nMinimum bid 2; raise or pass once. Bidder names trump.\nFollow suit or play trump; discard freely when void.\nHigh / low dealt trump, captured jack, most game value score 1 each.\nGame values: ten=10, ace=4, king=3, queen=2, jack=1. Ties score none.\nMiss your bid: lose that bid instead of scoring. First team to 7 wins.\nIf both reach 7, bidding team wins if it made its bid. Wager is per match.");
 return TEXT("Two-player UNO-style house game. Match color or symbol; wilds pick color.\nSkip and reverse give another turn; +2 / +4 draw and skip. No stacking.\nWild +4 is legal only with no card of the current color.\nDraw one: play that card if legal, or pass. Call LAST CARD before\nplaying down to one, or draw two penalty cards. First empty hand wins.\nA win pays 1:1. Closing the table saves your round; forfeit loses the stake.");
}
void FLWCardGame::Note(const FString& S){Message=S;Log.Add(S);while(Log.Num()>5)Log.RemoveAt(0);}
void FLWCardGame::Shuffle(){for(int I=Deck.Num()-1;I>0;--I)Deck.Swap(I,Random.RandRange(0,I));}
int32 FLWCardGame::Draw(){if(Deck.IsEmpty()&&Discard.Num()>1){int Top=Discard.Pop();Deck=Discard;Discard={Top};Shuffle();}return Deck.IsEmpty()?-1:Deck.Pop();}
int32 FLWCardGame::BlackjackValue(const TArray<int32>& C){int V=0,A=0;for(int Card:C){int R=Card%13;V+=R==12?11:FMath::Min(R+2,10);A+=R==12;}while(V>21&&A-->0)V-=10;return V;}
FString FLWCardGame::CardName(int32 Card,int32 Type){
 if(Card<0)return TEXT("?");
 if(Type==2){const TCHAR* Colors[]={TEXT("RED"),TEXT("GOLD"),TEXT("GREEN"),TEXT("BLUE"),TEXT("WILD")};int R=Card%15;FString Rank=R<10?FString::FromInt(R):R==10?TEXT("SKIP"):R==11?TEXT("REVERSE"):R==12?TEXT("+2"):R==13?TEXT("WILD"):TEXT("+4");return FString(Colors[FMath::Clamp(Card/15,0,4)])+TEXT(" ")+Rank;}
 const TCHAR* Suits[]={TEXT("CLUBS"),TEXT("DIAMONDS"),TEXT("HEARTS"),TEXT("SPADES")};int R=Card%13;FString Rank=R<9?FString::FromInt(R+2):R==9?TEXT("J"):R==10?TEXT("Q"):R==11?TEXT("K"):TEXT("A");return Rank+TEXT(" ")+Suits[Card/13];
}
void FLWCardGame::Finish(bool Win,bool Push){Phase=TEXT("done");Payout=Push?Stake:Win?Stake*2:0;Note(Push?TEXT("PUSH - stake returned."):Win?TEXT("YOU WIN."):TEXT("HOUSE WINS."));}
bool FLWCardGame::Start(int32 Type,int32 Wager,int32 Seed){
 if(Active()||Type<0||Type>2||Wager<10||Wager>100||Wager%2)return false;
 const bool Timed=Paced;*this=FLWCardGame();Paced=Timed;RoundId=FGuid::NewGuid();Game=Type;Stake=Wager;Random.Initialize(Seed);Hands.SetNum(Type==1?4:2);Scores={0,0};HandBets={Wager,0};
 if(Game==1){PitchDeal();return true;}
 if(Game==0){for(int I=0;I<52;I++)Deck.Add(I);Shuffle();for(int I=0;I<2;I++)for(int Seat=0;Seat<2;Seat++)DealTo(Seat);Phase=TEXT("play");int P=BlackjackValue(Hands[0].Cards),D=BlackjackValue(Hands[1].Cards);if(P==21||D==21){Finish(P==21&&D!=21,P==D);if(P==21&&D!=21){Payout=Stake*5/2;Note(TEXT("BLACKJACK - pays 3:2."));}}else Note(TEXT("Hit, stand, or double."));return true;}
 for(int C=0;C<4;C++){Deck.Add(C*15);for(int R=1;R<=12;R++){Deck.Add(C*15+R);Deck.Add(C*15+R);}}for(int I=0;I<4;I++){Deck.Add(73);Deck.Add(74);}Shuffle();for(int I=0;I<7;I++)for(int Seat=0;Seat<2;Seat++)DealTo(Seat);
 // Opening card is a number, so no invisible opening penalties.
 int Index=Deck.IndexOfByPredicate([](int C){return C/15<4&&C%15<10;});Discard.Add(Deck[Index]);Deck.RemoveAt(Index);Color=Discard.Last()/15;Phase=TEXT("play");Note(TEXT("Your turn. Match the discard."));return true;
}
bool FLWCardGame::Legal(int32 Seat,int32 Index)const{
 if(!Hands.IsValidIndex(Seat)||!Hands[Seat].Cards.IsValidIndex(Index))return false;
 int Card=Hands[Seat].Cards[Index];
 if(Game==2){if(Seat==0&&Drawn&&Index!=Hands[0].Cards.Num()-1)return false;if(Card%15==14)return !Hands[Seat].Cards.ContainsByPredicate([&](int C){return C/15==Color;});return Card/15==4||Card/15==Color||Card%15==Discard.Last()%15;}
 if(Game==1&&Tricks==0&&Trick.IsEmpty())return Card/13==Trump;
 if(Game==1&&!Trick.IsEmpty()){int Led=Trick[0]/13;return Card/13==Led||Card/13==Trump||!Hands[Seat].Cards.ContainsByPredicate([&](int C){return C/13==Led;});}return true;
}
void FLWCardGame::PitchDeal(){
 Deck.Empty();for(int C=0;C<52;C++)Deck.Add(C);Shuffle();Hands.SetNum(4);for(auto& H:Hands)H.Cards.Empty();for(int I=0;I<6;I++)for(int Seat=0;Seat<4;Seat++)DealTo(Seat);Trick.Empty();Seats.Empty();Points={0,0};GamePoints={0,0};Tricks=0;Trump=-1;Bid=0;Phase=TEXT("bid");AuctionCount=0;AuctionSeat=(Dealer+1)%4;SeatBids.Init(-1,4);Note(TEXT("Auction opens. North is your partner."));if(!Paced)AuctionAdvance();
}
void FLWCardGame::PitchAuction(int32 PlayerBid){AuctionOffer(0,PlayerBid);if(!Paced)AuctionAdvance();}
void FLWCardGame::AuctionOffer(int Seat,int Offer){
 SeatBids[Seat]=Offer;AuctionCount++;if(Offer>Bid){Bid=Offer;Bidder=Seat;}
 Note(FString::Printf(TEXT("%s %s"),Seat==0?TEXT("YOU"):Seat==1?TEXT("EAST"):Seat==2?TEXT("NORTH"):TEXT("WEST"),Offer?*FString::Printf(TEXT("bids %d."),Offer):TEXT("passes.")));
 AuctionSeat=(Seat+1)%4;
 if(AuctionCount==4){if(Bid==0){Bid=2;Bidder=Dealer;Note(TEXT("All pass. Dealer is set to two."));}Phase=TEXT("trump");}
}
void FLWCardGame::AuctionAdvance(){
 int Budget=Paced?1:5;
 while(Phase==TEXT("bid")&&AuctionSeat!=0&&Budget-->0){int Seat=AuctionSeat,Best=0;
  for(int Suit=0;Suit<4;Suit++){int Power=0;for(int C:Hands[Seat].Cards)if(C/13==Suit)Power+=1+(C%13>=9?1:0);Best=FMath::Max(Best,Power);}
  int Offer=Best>=7?4:Best>=5?3:Best>=3?2:0;AuctionOffer(Seat,Offer>Bid?Offer:0);
 }
 if(Phase==TEXT("trump")&&Bidder!=0&&(!Paced||Budget>0)){int Best=-1,Suit=0;for(int S=0;S<4;S++){int V=0;for(int C:Hands[Bidder].Cards)if(C/13==S)V+=C%13+5;if(V>Best){Best=V;Suit=S;}}PitchTrump(Suit);}
}
void FLWCardGame::PitchTrump(int32 Suit){Trump=Suit;int High=-1,Low=99;for(int S=0;S<4;S++)for(int C:Hands[S].Cards)if(C/13==Trump){if(C%13>High){High=C%13;HighSeat=S;}if(C%13<Low){Low=C%13;LowSeat=S;}}if(High>=0){Points[HighSeat%2]++;Points[LowSeat%2]++;}Turn=Bidder;Phase=TEXT("play");if(!Paced)PitchAdvance();}
void FLWCardGame::PitchPlay(int32 Seat,int32 Index){int C=Hands[Seat].Cards[Index];Hands[Seat].Cards.RemoveAt(Index);MoveCard(C,Seat,-2,Trick.Num());Trick.Add(C);Seats.Add(Seat);Note(FString::Printf(TEXT("%s: %s"),Seat==0?TEXT("YOU"):Seat==2?TEXT("NORTH"):Seat==1?TEXT("EAST"):TEXT("WEST"),*CardName(C,1)));Turn=(Seat+1)%4;}
void FLWCardGame::PitchAdvance(){
 int Budget=Paced?1:4;while(Trick.Num()<4&&Turn!=0&&Budget-->0){int Pick=PitchChoice(Turn);if(Pick<0)break;PitchPlay(Turn,Pick);}
 if(Trick.Num()==4){int Winner=0;auto Strength=[&](int C){return C%13+(C/13==Trump?100:C/13==Trick[0]/13?30:0);};for(int I=1;I<4;I++)if(Strength(Trick[I])>Strength(Trick[Winner]))Winner=I;Turn=Seats[Winner];for(int C:Trick){int R=C%13;GamePoints[Turn%2]+=R==8?10:R==12?4:R==11?3:R==10?2:R==9?1:0;if(C/13==Trump&&R==9)Points[Turn%2]++;}Tricks++;Phase=TEXT("trick");Note(FString::Printf(TEXT("%s takes trick %d."),Turn==0?TEXT("YOU"):Turn==2?TEXT("NORTH"):Turn==1?TEXT("EAST"):TEXT("WEST"),Tricks));}
}
void FLWCardGame::PitchScore(){if(GamePoints[0]!=GamePoints[1])Points[GamePoints[0]>GamePoints[1]?0:1]++;bool Made=Points[Bidder%2]>=Bid;for(int T=0;T<2;T++)Scores[T]+=T==Bidder%2&&!Made?-Bid:Points[T];Note(FString::Printf(TEXT("Hand: %d-%d points. Bid %s. Match: %d-%d."),Points[0],Points[1],Made?TEXT("made"):TEXT("set"),Scores[0],Scores[1]));if(Scores[0]>=7||Scores[1]>=7){bool Win=Scores[0]>=7;if(Scores[0]>=7&&Scores[1]>=7)Win=Made?Bidder%2==0:Bidder%2!=0;Finish(Win);}else Phase=TEXT("hand");}
void FLWCardGame::ColorPlay(int32 Seat,int32 Index,int32 Suit){
 int C=Hands[Seat].Cards[Index],R=C%15;Hands[Seat].Cards.RemoveAt(Index);MoveCard(C,Seat,-2);Discard.Add(C);Color=C/15==4?Suit:C/15;Drawn=false;Note(FString::Printf(TEXT("%s plays %s."),Seat?TEXT("DEALER"):TEXT("YOU"),*CardName(C,2)));
 int Other=1-Seat;if(R==12||R==14)for(int I=0;I<(R==12?2:4);I++){DealTo(Other);}
 if(Hands[Seat].Cards.IsEmpty()){Finish(Seat==0);return;}
 if(Seat==0&&Hands[0].Cards.Num()==1&&!Called){for(int I=0;I<2;I++){DealTo(0);}Note(TEXT("You forgot LAST CARD. Draw two."));}Called=false;
 Turn=R==10||R==11||R==12||R==14?Seat:Other;
}
void FLWCardGame::ColorAdvance(){
 for(int Steps=0;Turn==1&&Active()&&Steps<(Paced?1:128);Steps++){
 int Pick=-1;for(int I=0;I<Hands[1].Cards.Num();I++)if(Legal(1,I)){Pick=I;if(Hands[1].Cards[I]/15<4)break;}
 if(Pick<0){int D=Draw();if(D>=0){Hands[1].Cards.Add(D);MoveCard(D,-1,1,Hands[1].Cards.Num()-1);if(Legal(1,Hands[1].Cards.Num()-1))Pick=Hands[1].Cards.Num()-1;}if(Pick<0){Turn=0;Note(TEXT("Dealer draws and passes."));break;}}
 int Counts[4]={};for(int C:Hands[1].Cards)if(C/15<4)Counts[C/15]++;int Suit=0;for(int I=1;I<4;I++)if(Counts[I]>Counts[Suit])Suit=I;ColorPlay(1,Pick,Suit);
 }if(Active()&&Turn==0){Drawn=false;Called=false;}
}
bool FLWCardGame::Act(FName Action,int32 Value){
 if(!Active())return false;
 if(Action==TEXT("tick")){if(Game==0&&Phase==TEXT("dealer")){BlackjackAdvance();return true;}if(Game==1&&AutoTurn()){if(Phase==TEXT("play"))PitchAdvance();else AuctionAdvance();return true;}if(Game==2&&AutoTurn()){ColorAdvance();return true;}return false;}
 if(Action==TEXT("forfeit")){Finish(false);Note(TEXT("Round forfeited. Stake lost."));return true;}
 if(Game==0){if(Phase!=TEXT("play"))return false;
 if(HandBets.Num()!=Hands.Num()){HandBets.Init(0,Hands.Num());HandBets[0]=Stake;}
 if(Action==TEXT("split")){if(!CanSplit())return false;int C=Hands[0].Cards.Pop();FLWCardHand Second;Second.Cards={C};Hands.Add(Second);const int SplitBet=HandBets[0];HandBets.Add(SplitBet);Stake+=HandBets[0];MoveCard(C,0,2,0);DealTo(0);DealTo(2);Note(TEXT("Pair split. Play the left hand first."));if(C%13==12){ActiveHand=2;BlackjackNext();}return true;}
 if(Action==TEXT("surrender")&&Hands.Num()==2&&Hands[0].Cards.Num()==2){Phase=TEXT("done");Payout=Stake/2;Note(TEXT("Surrender. Half your stake returned."));return true;}
 if(Action==TEXT("hit")){DealTo(ActiveHand);if(BlackjackValue(Hands[ActiveHand].Cards)>21){Note(TEXT("Hand busts."));BlackjackNext();}return true;}
 if(Action!=TEXT("stand")&&Action!=TEXT("double"))return false;
 if(Action==TEXT("double")){if(Hands[ActiveHand].Cards.Num()!=2)return false;Stake+=HandBets[ActiveHand];HandBets[ActiveHand]*=2;DealTo(ActiveHand);}
 BlackjackNext();return true;
 }
 if(Game==1){
 if(Phase==TEXT("bid")&&AuctionSeat==0&&Action==TEXT("bid")&&(Value==0||(Value>=2&&Value<=4&&Value>Bid))){PitchAuction(Value);return true;}
 if(Phase==TEXT("trump")&&Bidder==0&&Action==TEXT("color")&&Value>=0&&Value<4){if(!Hands[Bidder].Cards.ContainsByPredicate([&](int C){return C/13==Value;}))return false;PitchTrump(Value);return true;}
 if(Phase==TEXT("play")&&Turn==0&&Action==TEXT("card")&&Legal(0,Value)){PitchPlay(0,Value);if(Trick.Num()==4||!Paced)PitchAdvance();return true;}
 if(Phase==TEXT("trick")&&Action==TEXT("next")){if(Tricks==6)PitchScore();else {Trick.Empty();Seats.Empty();Phase=TEXT("play");if(!Paced)PitchAdvance();}return true;}
 if(Phase==TEXT("hand")&&Action==TEXT("next")){Dealer=(Dealer+1)%4;PitchDeal();return true;}return false;
 }
 if(Phase==TEXT("color")&&Action==TEXT("color")&&Value>=0&&Value<4){int I=PendingCard;PendingCard=-1;Phase=TEXT("play");ColorPlay(0,I,Value);if(!Paced)ColorAdvance();return true;}
 if(Phase!=TEXT("play")||Turn!=0)return false;
 if(Action==TEXT("call")){Called=true;Note(TEXT("LAST CARD!"));return true;}
 if(Action==TEXT("draw")&&!Drawn){int C=Draw();if(C>=0){Hands[0].Cards.Add(C);MoveCard(C,-1,0,Hands[0].Cards.Num()-1);}Drawn=true;if(C<0){Turn=1;if(!Paced)ColorAdvance();}else Note(TEXT("Play the drawn card, or pass."));return true;}
 if(Action==TEXT("pass")&&Drawn){Turn=1;if(!Paced)ColorAdvance();return true;}
 if(Action==TEXT("card")&&Legal(0,Value)){if(Hands[0].Cards[Value]/15==4){PendingCard=Value;Phase=TEXT("color");}else{ColorPlay(0,Value,Color);if(!Paced)ColorAdvance();}return true;}return false;
}

void FLWCardGame::MoveCard(int Card,int From,int To,int Index){FLWCardMove M;M.Serial=++MoveSerial;M.Card=Card;M.From=From;M.To=To;M.Index=Index;Moves.Add(M);if(Moves.Num()>128)Moves.RemoveAt(0);}
void FLWCardGame::DealTo(int Seat){int C=Draw();if(C>=0){Hands[Seat].Cards.Add(C);MoveCard(C,-1,Seat,Hands[Seat].Cards.Num()-1);}}
int FLWCardGame::ExtraCost(FName Action)const{if(Game!=0)return 0;if(Action==TEXT("split"))return CanSplit()?(HandBets.IsValidIndex(0)?HandBets[0]:Stake):0;if(Action==TEXT("double")&&Phase==TEXT("play")&&Hands.IsValidIndex(ActiveHand)&&Hands[ActiveHand].Cards.Num()==2)return HandBets.IsValidIndex(ActiveHand)?HandBets[ActiveHand]:Stake;return 0;}
bool FLWCardGame::CanSplit()const{return Game==0&&Phase==TEXT("play")&&Hands.Num()==2&&Hands[0].Cards.Num()==2&&Hands[0].Cards[0]%13==Hands[0].Cards[1]%13;}
void FLWCardGame::BlackjackNext(){if(Hands.Num()>2&&ActiveHand==0){ActiveHand=2;Note(TEXT("Play the right hand."));return;}Phase=TEXT("dealer");MoveCard(Hands[1].Cards[1],1,1,1);if(!Paced)BlackjackAdvance();}
void FLWCardGame::BlackjackAdvance(){int Budget=Paced?1:20;bool AllBust=true;for(int I=0;I<Hands.Num();I++)if(I!=1&&BlackjackValue(Hands[I].Cards)<=21)AllBust=false;
 while(!AllBust&&BlackjackValue(Hands[1].Cards)<17&&Budget-->0)DealTo(1);
 if(AllBust||BlackjackValue(Hands[1].Cards)>=17)BlackjackSettle();
}
void FLWCardGame::BlackjackSettle(){int DealerValue=BlackjackValue(Hands[1].Cards);Payout=0;HandReturns.Init(0,Hands.Num());for(int I=0;I<Hands.Num();I++)if(I!=1){int V=BlackjackValue(Hands[I].Cards),Bet=HandBets.IsValidIndex(I)?HandBets[I]:Stake;int R=V>21?0:DealerValue>21||V>DealerValue?Bet*2:V==DealerValue?Bet:0;HandReturns[I]=R;Payout+=R;}Phase=TEXT("done");Note(Payout>Stake?TEXT("YOU WIN."):Payout==Stake?TEXT("PUSH - stake returned."):Payout>0?TEXT("One hand returned. Round settled."):TEXT("HOUSE WINS."));}
bool FLWCardGame::AutoTurn()const{return Game==0?Phase==TEXT("dealer"):Game==1?((Phase==TEXT("bid")&&AuctionSeat!=0)||(Phase==TEXT("trump")&&Bidder!=0)||(Phase==TEXT("play")&&Turn!=0)):Phase==TEXT("play")&&Turn==1;}
int FLWCardGame::PitchChoice(int Seat)const{
 auto Strength=[&](int C){return C%13+(C/13==Trump?100:!Trick.IsEmpty()&&C/13==Trick[0]/13?30:0);};
 int Best=-1;for(int I=0;I<Trick.Num();I++)if(Best<0||Strength(Trick[I])>Strength(Trick[Best]))Best=I;
 const bool PartnerWinning=Best>=0&&Seats[Best]%2==Seat%2;int Pick=-1,Score=-100000;
 for(int I=0;I<Hands[Seat].Cards.Num();I++)if(Legal(Seat,I)){int C=Hands[Seat].Cards[I],Cost=C%13+(C/13==Trump?30:0),Value=-Cost;
  bool Wins=Best<0||Strength(C)>Strength(Trick[Best]);int Prize=0;for(int T:Trick)Prize+=T%13==8?10:T%13>=9?4:0;
  if(Trick.IsEmpty())Value=C%13>=11?70+Cost:-Cost;
  else if(PartnerWinning)Value=(C%13==8?45:0)-Cost;
  else if(Wins)Value=60+Prize-Cost;
  if(Value>Score){Score=Value;Pick=I;}
 }return Pick;
}
