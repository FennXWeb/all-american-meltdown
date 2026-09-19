#pragma once
#include "CoreMinimal.h"
#include "Engine/Console.h"
#include "LWConsole47.generated.h"
class ALWCharacter;
UCLASS()
class LETHALWORLD_API ULWConsole47 : public UConsole {
 GENERATED_BODY()
public:
 virtual void ConsoleCommand(const FString& Command) override;
 virtual void FakeGotoState(FName NextStateName) override;
 virtual void AugmentRuntimeAutoCompleteList(TArray<FAutoCompleteCommand>& List) override;
 virtual bool InputKey(FInputDeviceId DeviceId,FKey Key,EInputEvent Event,float AmountDepressed=1.f,bool bGamepad=false) override;
 virtual bool InputAxis(FInputDeviceId DeviceId,FKey Key,float Delta,float DeltaTime,int32 NumSamples=1,bool bGamepad=false) override {return ConsoleActive();}
 static void Install(ALWCharacter* Player);
 FString Execute47(const FString& Command);
private:
 ALWCharacter* Player47() const;
 bool WasPaused47=false;
};
namespace LWCheats47 { bool PositiveAmount(const FString& Text,int64 Maximum,int64& Out); }
