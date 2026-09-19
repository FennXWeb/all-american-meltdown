#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerInput.h"
#include "LWPlayerInput51.generated.h"
UCLASS()
class LETHALWORLD_API ULWPlayerInput51 : public UPlayerInput {
 GENERATED_BODY()
public:
 virtual bool InputKey(const FInputKeyEventArgs& Params) override;
 struct FBinding {FKey Logical,Physical;FString Label;};
 TArray<FBinding> Bindings;
 int Capture=-1; uint64 CaptureFrame=0; FString Status;
 void Load51();void Save51();void Reset51();bool Assign51(int Index,FKey Key);
 FKey Translate51(FKey Key)const;
 static ULWPlayerInput51* Get51(const UObject* Context);
private:
 bool Loaded=false;TMap<FKey,FKey> Held51;TSet<FKey> Suppressed51;
};
