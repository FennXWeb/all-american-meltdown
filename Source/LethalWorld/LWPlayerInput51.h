#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerInput.h"
#include "LWPlayerInput51.generated.h"
UCLASS()
class LETHALWORLD_API ULWPlayerInput51 : public UPlayerInput {
 GENERATED_BODY()
public:
 virtual bool InputKey(const FInputKeyEventArgs& Params) override;
 virtual void ProcessInputStack(const TArray<UInputComponent*>& Stack,float Dt,bool Paused) override;
 virtual void FlushPressedKeys() override;
 bool Controller58=false,Modifier58=false,Invert58=false;float LookSpeed58=150,DeadZone58=.16f,CursorSpeed58=850;
 TMap<FKey,float> Axes58;TMap<FKey,FKey> Held58;
 FString Context58;double LastPad58=0,LastFrame58=0;bool TextMode58=false;int TextCell58=0;
 bool PadInput58(const FInputKeyEventArgs& Params);void Emit58(FKey Key,EInputEvent Event,bool MouseUI=false);void Release58();
 void Load58();void Save58();void Change58(int Index);FString Label58(int Index)const;FString Prompt58(FKey Logical)const;
 struct FBinding {FKey Logical,Physical;FString Label;};
 TArray<FBinding> Bindings;
 int Capture=-1; uint64 CaptureFrame=0; FString Status;
 void Load51();void Save51();void Reset51();bool Assign51(int Index,FKey Key);
 FKey Translate51(FKey Key)const;
 static ULWPlayerInput51* Get51(const UObject* Context);
private:
 bool Loaded=false;TMap<FKey,FKey> Held51;TSet<FKey> Suppressed51;
};
