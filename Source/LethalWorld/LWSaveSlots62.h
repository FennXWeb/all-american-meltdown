#pragma once
#include "CoreMinimal.h"
struct FLWSlot62 {FString Slot,Title,Detail;bool Exists=false;};
namespace LWSaves62 {FString Prefix();FString Auto();FString Manual(int I);bool Managed(const FString& Slot);FString Latest();TArray<FLWSlot62> List();bool Write(const TArray<uint8>& Bytes,const FString& Slot,const FString& Metadata);}
