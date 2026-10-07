#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LWDebugMenu67.generated.h"

class ALWCharacter;
class ULWConsole47;
class SLWDebugPanel67;
class SWidget;

enum class ELWDebugSection67 : uint8 { Cheats, Items, NPCs, Vehicles, POIs };
struct FLWDebugEntry67 {
 FString Id,Name,Category,Details,Command;
 int32 Min=1,Max=1,Default=1;
 bool HasAmount=false;
};
using FLWDebugRow67=TSharedPtr<FLWDebugEntry67>;
namespace LWDebug67 {
 TArray<FLWDebugRow67> Catalog(ELWDebugSection67 Section);
 TArray<FLWDebugRow67> Filter(const TArray<FLWDebugRow67>& Entries,const FString& Query,const FString& Category,int Sort,bool Descending);
 FString Command(const FLWDebugEntry67& Entry,int32 Amount);
}

UCLASS()
class LETHALWORLD_API ULWDebugMenu67 : public UObject {
 GENERATED_BODY()
public:
 static FString OpenFor(ALWCharacter* Player);
 virtual UWorld* GetWorld()const override;
 void Close(bool RestoreInput=true);
 bool IsOpen()const;
 void SetSection(ELWDebugSection67 Section);
 void SetSearch(const FString& Value);
 void SetCategory(const FString& Value);
 void SetSort(int Column);
 void Refresh();
 bool Select(const FString& Id);
 void Execute();
 void CancelSearch();
 FString ActionLabel()const;
 int32 Minimum()const;int32 Maximum()const;
 FString Status=TEXT("Choose an entry to get started."),Search,Category;
 ELWDebugSection67 Section=ELWDebugSection67::Cheats;
 int32 Sort=0,Amount=1,LocateRequest=0;bool Descending=false;
 TArray<FLWDebugRow67> Entries,Visible;
 TArray<TSharedPtr<FString>> Categories;
 FLWDebugRow67 Selected;
 UPROPERTY() TWeakObjectPtr<ALWCharacter> Player;
 UPROPERTY() TWeakObjectPtr<ULWConsole47> Console;
private:
 void UpdateRows();
 bool WasPaused=false,WasUIInput=false,Closing=false;
 FDelegateHandle LocateHandle;
 TSharedPtr<SLWDebugPanel67> Panel;
 TSharedPtr<SWidget> RootWidget;
};
