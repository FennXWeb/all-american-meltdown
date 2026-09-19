#pragma once
#include "LWWorldObject.h"
#include "LWSlotMachine.generated.h"
UCLASS()
class LETHALWORLD_API ALWSlotMachine:public ALWWorldObject{
 GENERATED_BODY()
public:
 UPROPERTY() TArray<TObjectPtr<class UTextRenderComponent>> Reels;
 float SpinTime=0;int32 Result[3]={0,0,0},Award=0;
 void Setup(class ALWWorld* W,FName Id);
 virtual FString Prompt()const override;
 virtual void Use(class ALWCharacter* P)override;
 virtual void Tick(float Dt)override;
 static int Payout(int A,int B,int C){static const int Prize[]={60,90,160,400,2000};return A==B&&B==C?Prize[FMath::Clamp(A,0,4)]:(A==B||A==C||B==C)?5:0;}
};
