#pragma once
#include "CoreMinimal.h"
#include "LWWorldObject.h"
#include "LWCardGame.generated.h"

USTRUCT()
struct FLWCardHand {
 GENERATED_BODY()
 UPROPERTY() TArray<int32> Cards;
};

USTRUCT()
struct FLWCardMove {
 GENERATED_BODY()
 UPROPERTY() int32 Serial=0;
 UPROPERTY() int32 Card=-1;
 UPROPERTY() int32 From=-1;
 UPROPERTY() int32 To=0;
 UPROPERTY() int32 Index=0;
};
// Serializable rules state. Credits are debited before dealing and payouts claimed once.
USTRUCT()
struct FLWCardGame {
 GENERATED_BODY()
 UPROPERTY() bool Paced=false;
 UPROPERTY() FGuid RoundId;
 UPROPERTY() TArray<FLWCardMove> Moves;
 UPROPERTY() int32 MoveSerial=0;
 UPROPERTY() int32 ActiveHand=0;
 UPROPERTY() TArray<int32> HandBets;
 UPROPERTY() TArray<int32> HandReturns;
 UPROPERTY() int32 AuctionSeat=0;
 UPROPERTY() int32 AuctionCount=0;
 UPROPERTY() TArray<int32> SeatBids;
 void MoveCard(int Card,int From,int To,int Index=0);
 void DealTo(int Seat);
 int ExtraCost(FName Action)const;
 bool CanSplit()const;
 void BlackjackNext();void BlackjackAdvance();void BlackjackSettle();
 bool AutoTurn()const;
 void AuctionAdvance();void AuctionOffer(int Seat,int Offer);
 int PitchChoice(int Seat)const;
 UPROPERTY() int32 Game=0; // blackjack, four-point partnership pitch, last card
 UPROPERTY() FName Phase=TEXT("lobby");
 UPROPERTY() int32 Stake=10;
 UPROPERTY() int32 Payout=0;
 UPROPERTY() bool Paid=false;
 UPROPERTY() FRandomStream Random;
 UPROPERTY() TArray<int32> Deck;
 UPROPERTY() TArray<int32> Discard;
 UPROPERTY() TArray<FLWCardHand> Hands;
 UPROPERTY() FString Message;
 UPROPERTY() TArray<FString> Log;
 UPROPERTY() int32 Turn=0;
 UPROPERTY() int32 Color=0;
 UPROPERTY() int32 PendingCard=-1;
 UPROPERTY() bool Drawn=false;
 UPROPERTY() bool Called=false;
 UPROPERTY() int32 Trump=-1;
 UPROPERTY() int32 Bid=0;
 UPROPERTY() int32 Bidder=0;
 UPROPERTY() int32 Dealer=3;
 UPROPERTY() TArray<int32> Trick;
 UPROPERTY() TArray<int32> Seats;
 UPROPERTY() TArray<int32> Scores;
 UPROPERTY() TArray<int32> Points;
 UPROPERTY() TArray<int32> GamePoints;
 UPROPERTY() int32 Tricks=0;
 UPROPERTY() int32 HighSeat=0;
 UPROPERTY() int32 LowSeat=0;
 bool Active() const {return Phase!=TEXT("lobby")&&Phase!=TEXT("done");}
 void Note(const FString& S);
 bool Start(int32 Type,int32 Wager,int32 Seed);
 bool Act(FName Action,int32 Value=0);
 bool Legal(int32 Seat,int32 Index) const;
 void Finish(bool Win,bool Push=false);
 void Shuffle(); int32 Draw();
 void PitchDeal();void PitchAuction(int32 PlayerBid);void PitchTrump(int32 Suit);void PitchAdvance();void PitchPlay(int32 Seat,int32 Index);void PitchScore();
 void ColorPlay(int32 Seat,int32 Index,int32 Suit);void ColorAdvance();
 static int32 BlackjackValue(const TArray<int32>& Cards);
 static FString CardName(int32 Card,int32 Type);
 static FString Title(int32 Type);
 static FString Rules(int32 Type);
};

UCLASS()
class ALWCardTable : public ALWWorldObject {
 GENERATED_BODY()
public:
 ALWCardTable();
 virtual void Tick(float Dt) override;
 int32 Game=0;
 float TableClock=0;
 void Setup(class ALWWorld* W,FName Id,int32 Type);
 virtual void Use(class ALWCharacter* P) override;
 virtual FString Prompt() const override;
 virtual float TakeDamage(float,const FDamageEvent&,AController*,AActor*) override {return 0;}
};
