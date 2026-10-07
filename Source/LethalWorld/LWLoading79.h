#pragma once
#include "CoreMinimal.h"

class SWidget;
namespace LWLoading79 {
struct FMessage { FString Id, Category, Text; };
struct FArtwork { FString Id, File; };
struct FBank { TArray<FMessage> Messages; TArray<FArtwork> Images; };
struct FFrame { int64 Current=0, Previous=0; float Blend=1; };
struct FTelemetry { int64 Paints=0, Art=-1, Message=-1; };

// Plain data only: no world, assets or UObjects are accessed by the loading thread.
FBank ReadBank(const FString& Directory);
TArray<FMessage> ParseMessages(const FString& Csv);
TArray<int32> ShuffledOrder(int32 Count,int32 Seed);
FFrame FrameAt(double Seconds,double Interval,double Fade,bool ReducedMotion);
FVector2D CoverSize(FVector2D Image,FVector2D Viewport);
void Preload();
TSharedRef<SWidget> Create(const FString& Label);
FTelemetry Telemetry();
#if WITH_DEV_AUTOMATION_TESTS
TSharedRef<SWidget> Preview(const FString& Label,double Age,bool ReducedMotion=false,bool HighContrast=false);
#endif
}
