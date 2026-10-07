#pragma once
#include "CoreMinimal.h"
class ALWCharacter;
namespace LWCurrency70 {
 bool CanadianTrade(const ALWCharacter* P);
 int64 Balance(const ALWCharacter* P);
 int64& Wallet(ALWCharacter* P);
 const TCHAR* Unit(const ALWCharacter* P);
 // Fictional CAD per credit. Stable across saves, independent of streaming/load order.
 double Rate(double Hours,int Seed);
 double Rate(const ALWCharacter* P);
 bool Convert(int64& Credits,int64& CAD,int64 Amount,bool ToCAD,double Mid,int64& Received);
 FString ExchangeText(const ALWCharacter* P);
 bool Exchange(ALWCharacter* P,int64 Amount,bool ToCAD);
}
