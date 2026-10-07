#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "LWDialogue75.generated.h"

struct FLWChatterLine75 { FName Id,Group; FString Text; };
struct FLWChatterHistory75 {
 TMap<FName,TArray<FName>> Remaining;
 TMap<FName,FName> Last;
 // Exhaust each group before repeating; refill never repeats the last line.
 const FLWChatterLine75* Choose(FName Group,FRandomStream& Random,const TFunction<bool(const FLWChatterLine75&)>& Eligible);
};
namespace LWDialogue75 { const TArray<FLWChatterLine75>& Lines(); }

// One conversation space per world, not one independent timer per NPC.
UCLASS()
class ULWDialogueDirector75 : public UWorldSubsystem {
 GENERATED_BODY()
public:
 bool CanSpeak(bool Combat) const;
 bool Recent(FName Line) const;
 void Reserve(FName Line,float Duration,bool Combat);
 void Speaking(class ULWDialogue59* Channel);
private:
 double AmbientUntil=0,CombatUntil=0;
 TMap<FName,double> LastSpoken;
 TWeakObjectPtr<class ULWDialogue59> Speaker;
};
