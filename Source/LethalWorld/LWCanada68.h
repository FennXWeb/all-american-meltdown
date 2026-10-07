#pragma once
#include "CoreMinimal.h"
class ALWCharacter;class ALWWorld;class ALWVehicle;
struct FLWItemInstance;
namespace LWCanada68 {
 constexpr int PassportPrice=1000000;
 constexpr float AirGrace=20;
 bool Authorized(const ALWCharacter* P);
 bool BuyPassport(ALWCharacter* P);
 bool Deposit(ALWCharacter* P);
 void Reclaim(ALWCharacter* P);
 bool Dialogue(ALWCharacter* P);
 bool Action(ALWCharacter* P,FName Action);
 void Tick(ALWWorld* W,ALWCharacter* P,float Dt);
 int TradePrice(const ALWCharacter* P,const FLWItemInstance& I,bool Buy);
}
