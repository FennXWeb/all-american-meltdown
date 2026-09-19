#include "LWHUD.h"
#include "LWCharacter.h"
#include "Engine/World.h"
#include "LWCardGame.h"
#include "LWCardArt.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
void ALWHUD::CardScreen(ALWCharacter* P){
 if(!CardAtlas)CardAtlas=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Art/Textures/T_CardAtlasV12.T_CardAtlasV12"));
 auto& G=P->Cards;const float L=FMath::Max(0.f,(Canvas->ClipX/Scale-1280)*.5f);double Now=GetWorld()->GetTimeSeconds();
 float MX=0,MY=0;Mouse55(MX,MY);MX=MX/Scale-L;MY/=Scale;
 bool Down=PlayerOwner->IsInputKeyDown(EKeys::LeftMouseButton),Ready=Now>=P->CardBusyUntil&&!G.AutoTurn();
 auto B=[&](const TCHAR* A,const FString& S,float X,float Y,float W=145){Button(FName(*(FString(TEXT("cards_"))+A)),S,L+X,Y,W);};
 auto C=[&](int V,float X,float Y,float W=85,float Angle=0){LWCardArt::Card(Canvas,CardAtlas,FontAtlas,FVector2D(L+X,Y)*Scale,W*Scale,V,G.Game,Angle);};
 auto Seat=[&](int S){return S==0?FVector2D(610,535):S==1?(G.Game==1?FVector2D(1070,285):FVector2D(620,185)):S==2?FVector2D(620,185):FVector2D(205,285);};
 Rect(L+24,28,1232,650,FLinearColor(.13,.085,.045));LWCardArt::Tile(Canvas,CardAtlas,7,FVector2D(L+640,353)*Scale,FVector2D(1204,622)*Scale);
 Rect(L+38,42,1204,622,FLinearColor(.025f,.065f,.035f,.6f));
 Text(FLWCardGame::Title(G.Game),L+55,52,1.4);Text(FString::Printf(TEXT("CREDITS %lld   BET %d"),P->Money,G.Stake),L+385,57,.95);B(TEXT("rules"),TEXT("RULES"),920,42);B(TEXT("close"),TEXT("LEAVE"),1080,42);
 if(P->CardRules){Rect(L+55,115,1170,520,FLinearColor(.035,.055,.04,.98));Text(FLWCardGame::Rules(G.Game),L+80,140,.92);Text(TEXT("Click a card or drag it onto the middle of the table.\nClick the deck to draw. Leaving saves the current game."),L+80,420,.9);B(TEXT("rules"),TEXT("BACK"),80,550);CardMouseDown=Down;return;}
 if(CardSeenRound!=G.RoundId){CardSeenRound=G.RoundId;CardSeenSerial=0;CardMoveStart=Now;CardDrag=-1;}
 if(CardSeenSerial!=G.MoveSerial){CardBatchFirst=CardSeenSerial+1;CardSeenSerial=G.MoveSerial;CardMoveStart=Now;}
 auto Pending=[&](int SeatIndex,int CardIndex){for(const auto& M:G.Moves)if(M.Serial>=CardBatchFirst&&M.To==SeatIndex&&M.Index==CardIndex&&Now<CardMoveStart+(M.Serial-CardBatchFirst)*.14+.38)return true;return false;};
 // Physical deck and chip stacks remain on the felt throughout the round.
 for(int I=0;I<5;I++)C(-1,355-I*2,306-I*2,66,-7);
 Text(FString::Printf(TEXT("%d"),G.Deck.Num()),L+342,360,.7);
 if(Ready&&G.Active()&&G.Phase==TEXT("play")&&(G.Game==0||(G.Game==2&&G.Turn==0&&!G.Drawn)))AddHitBox(FVector2D(L+310,250)*Scale,FVector2D(90,110)*Scale,TEXT("cards_deck"),true);
 for(int I=0;I<FMath::Clamp(G.Stake/10,1,10);I++){LWCardArt::Chip(Canvas,FVector2D(L+896,355-I*3)*Scale,19*Scale,I%2?FLinearColor(.7,.55,.26):FLinearColor(.5,.15,.1));}
 Text(G.Message.Left(106),L+65,625,.83);
 if(G.Hands.Num()){
  for(int S=1;S<G.Hands.Num();S++){
   if(G.Game==0&&S==2)continue;FVector2D Pos=Seat(S);int N=G.Hands[S].Cards.Num();
   const TCHAR* Name=G.Game==1?(S==1?TEXT("EAST"):S==2?TEXT("NORTH - PARTNER"):TEXT("WEST")):TEXT("DEALER");Text(FString::Printf(TEXT("%s (%d)"),Name,N),L+Pos.X-70,Pos.Y-(S==2&&G.Game==1?65:85),.75);
   for(int I=0;I<FMath::Min(N,16);I++){if(Pending(S,I))continue;bool Show=G.Game==0&&(I==0||G.Phase==TEXT("dealer")||G.Phase==TEXT("done"));C(Show?G.Hands[S].Cards[I]:-1,Pos.X+(I-(FMath::Min(N,16)-1)*.5f)*(G.Game==0?70:18),Pos.Y, G.Game==0?82:55,(I-(FMath::Min(N,16)-1)*.5f)*3);}
   if(G.Game==0&&(G.Phase==TEXT("dealer")||G.Phase==TEXT("done")))Text(FString::FromInt(FLWCardGame::BlackjackValue(G.Hands[1].Cards)),L+Pos.X-8,Pos.Y+65,.9);
   if(G.Game==1&&G.SeatBids.IsValidIndex(S)&&G.SeatBids[S]>=0)Text(G.SeatBids[S]?FString::Printf(TEXT("BID %d"),G.SeatBids[S]):TEXT("PASS"),L+Pos.X-35,Pos.Y+50,.75);
  }
  if(G.Game==1){for(int I=0;I<G.Trick.Num();I++){if(Pending(-2,I))continue;FVector2D Pos=Seat(G.Seats[I]);Pos=FVector2D(640,320)+(Pos-FVector2D(640,320))*.27;C(G.Trick[I],Pos.X,Pos.Y,72,G.Seats[I]*8-12);}Text(FString::Printf(TEXT("YOU / NORTH %d     EAST / WEST %d"),G.Scores[0],G.Scores[1]),L+440,100,.75);if(G.Trump>=0){const TCHAR* Suits[]={TEXT("CLUBS"),TEXT("DIAMONDS"),TEXT("HEARTS"),TEXT("SPADES")};Text(FString(TEXT("TRUMP: "))+Suits[G.Trump],L+510,415,.8);}}
  if(G.Game==2&&!G.Discard.IsEmpty()){for(int I=FMath::Max(0,G.Discard.Num()-3);I<G.Discard.Num();I++)C(G.Discard[I],635,310,98,(I%3-1)*7);const TCHAR* Colors[]={TEXT("RED"),TEXT("GOLD"),TEXT("GREEN"),TEXT("BLUE")};Text(Colors[G.Color],L+600,390,.8);}
  int Hover=-1;const int Groups=G.Game==0&&G.Hands.Num()>2?2:1;
  for(int H=0;H<Groups;H++){int S=H?2:0,N=G.Hands[S].Cards.Num();float Center=Groups==2?(H?825:435):640,Span=Groups==2?315:850,Step=FMath::Min(72.f,Span/FMath::Max(1,N)),Width=Groups==2?82:98;
   for(int I=0;I<N;I++){float X=Center+(I-(N-1)*.5f)*Step;float Y=530+FMath::Abs(I-(N-1)*.5f)*1.5f;bool Legal=Ready&&G.Phase==TEXT("play")&&G.Turn==0&&G.Game!=0&&G.Legal(0,I);if(Legal&&MX>=X-Width*.5f&&MX<X+Width*.5f&&MY>Y-70&&MY<Y+65)Hover=I;}
   for(int I=0;I<N;I++){float X=Center+(I-(N-1)*.5f)*Step,Y=530+FMath::Abs(I-(N-1)*.5f)*1.5f;if((CardDrag==I&&G.Game!=0)||Pending(S,I))continue;C(G.Hands[S].Cards[I],X,Y-(Hover==I?20:0),Width,(I-(N-1)*.5f)*FMath::Min(3.f,22.f/FMath::Max(1,N)));}
   if(G.Game==0)Text(FString::Printf(TEXT("%s %d"),G.ActiveHand==S&&G.Active()?TEXT("> YOUR HAND"):TEXT("HAND"),FLWCardGame::BlackjackValue(G.Hands[S].Cards)),L+Center-80,440,.9);
  }
  if(Down&&!CardMouseDown&&Hover>=0)CardDrag=Hover;
  if(CardDrag>=0&&G.Hands[0].Cards.IsValidIndex(CardDrag)){C(G.Hands[0].Cards[CardDrag],MX,MY,105,-3);if(!Down&&CardMouseDown){int Index=CardDrag;CardDrag=-1;if(Ready&&((MY>220&&MY<425&&MX>410&&MX<850)||Hover==Index))P->CardAction(TEXT("card"),Index);}}
 }
 if(!G.Active()){
  Text(G.Phase==TEXT("done")?FString::Printf(TEXT("RETURNED %d"),G.Payout):TEXT("PLACE YOUR BET"),L+525,280,1);
  for(int I=0;I<4;I++){int Bet=I==0?10:I==1?20:I==2?50:100;float X=470+I*110;LWCardArt::Chip(Canvas,FVector2D(L+X,353)*Scale,(CardBet==Bet?30:24)*Scale,CardBet==Bet?FLinearColor(.65f,.4f,.16f):FLinearColor(.3f,.12f,.08f));Text(FString::FromInt(Bet),L+X-9,345,.85);AddHitBox(FVector2D(L+X-32,321)*Scale,FVector2D(64,64)*Scale,FName(*FString::Printf(TEXT("cards_bet_%d"),Bet)),true);}B(TEXT("deal"),TEXT("DEAL"),555,395,170);
 }else if(Ready){
  if(G.Game==0&&G.Phase==TEXT("play")){B(TEXT("hit"),TEXT("HIT"),70,420);B(TEXT("stand"),TEXT("STAND"),70,468);if(G.Hands[G.ActiveHand].Cards.Num()==2)B(TEXT("double"),TEXT("DOUBLE"),1030,420);if(G.CanSplit())B(TEXT("split"),TEXT("SPLIT"),1030,468);if(G.Hands.Num()==2&&G.Hands[0].Cards.Num()==2)B(TEXT("surrender"),TEXT("SURRENDER"),1030,516,170);}
  if(G.Game==1&&G.Phase==TEXT("bid")&&G.AuctionSeat==0){B(TEXT("bid_0"),TEXT("PASS"),430,360,95);for(int I=FMath::Max(2,G.Bid+1);I<=4;I++)B(*FString::Printf(TEXT("bid_%d"),I),FString::Printf(TEXT("BID %d"),I),430+(I-1)*110,360,95);}
  if(G.Phase==TEXT("hand"))B(TEXT("next"),TEXT("NEXT DEAL"),540,360,190);
  if(G.Game==2&&G.Phase==TEXT("play")){B(TEXT("call"),G.Called?TEXT("CALLED!"):TEXT("LAST CARD!"),70,460,180);if(G.Drawn)B(TEXT("pass"),TEXT("PASS"),1030,460);}
  if((G.Phase==TEXT("trump")&&G.Bidder==0)||G.Phase==TEXT("color")){const TCHAR* Suits[]={TEXT("CLUBS"),TEXT("DIAMONDS"),TEXT("HEARTS"),TEXT("SPADES")};const TCHAR* Colors[]={TEXT("RED"),TEXT("GOLD"),TEXT("GREEN"),TEXT("BLUE")};for(int I=0;I<4;I++)if(G.Game!=1||G.Hands[0].Cards.ContainsByPredicate([&](int V){return V/13==I;}))B(*FString::Printf(TEXT("color_%d"),I),G.Game==1?Suits[I]:Colors[I],430+I*115,360,108);}
 }
 if(G.Active())B(TEXT("forfeit"),TEXT("FORFEIT"),1030,572);
 // A short arc carries the latest dealt/played card between actual seat positions.
 for(const auto& M:G.Moves){if(M.Serial<CardBatchFirst)continue;double Elapsed=Now-CardMoveStart-(M.Serial-CardBatchFirst)*.14;if(Elapsed<0||Elapsed>.38)continue;float T=FMath::Clamp(float(Elapsed/.38),0.f,1.f);FVector2D A=M.From<0?FVector2D(350,300):Seat(M.From),Z=M.To<0?FVector2D(640,320):Seat(M.To),Pos=FMath::Lerp(A,Z,T);Pos.Y-=FMath::Sin(T*PI)*45;bool Reveal=M.To==-2||M.To==0||(G.Game==0&&G.Phase!=TEXT("play"));C(Reveal?M.Card:-1,Pos.X,Pos.Y,83,(1-T)*25);}
 CardMouseDown=Down;
}
bool ALWHUD::CardClick(ALWCharacter* P,FName N){FString S=N.ToString();if(!S.StartsWith(TEXT("cards_")))return false;S.RightChopInline(6);FString A,V;if(S.Split(TEXT("_"),&A,&V)){if(A==TEXT("bet"))CardBet=FCString::Atoi(*V);else P->CardAction(FName(*A),FCString::Atoi(*V));}else if(S==TEXT("deal"))P->CardAction(TEXT("start"),CardBet);else if(S==TEXT("deck"))P->CardAction(P->Cards.Game==0?TEXT("hit"):TEXT("draw"));else P->CardAction(FName(*S));return true;}
