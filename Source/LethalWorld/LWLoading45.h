#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LWLoading45.generated.h"
UCLASS()
class ULWLoading45:public UGameInstance {
 GENERATED_BODY()
public:
 virtual void Init() override;
 virtual void Shutdown() override;
 void BeforeMap(const FString& Map);
};
struct FLWLoadingScope45 {
 bool Active=false;
 explicit FLWLoadingScope45(const FString& Label);
 ~FLWLoadingScope45();
};

