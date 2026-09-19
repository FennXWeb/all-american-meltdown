#pragma once
#include "GameFramework/SaveGame.h"
#include "LWOpening.h"
#include "LWAppearancePreset35.generated.h"
UCLASS()
class ULWAppearancePreset35:public USaveGame {
 GENERATED_BODY()
public:
 UPROPERTY() FLWIdentity Identity;
};
