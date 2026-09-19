#pragma once
#include "LWWorldObject.h"
#include "LWUnderground.generated.h"
UCLASS()
class LETHALWORLD_API ALWUndergroundDoor:public ALWWorldObject{
 GENERATED_BODY()
public:
 FVector Destination;bool Exit=false;
 virtual FString Prompt()const override{return Exit?TEXT("[E] RETURN TO SURFACE"):TEXT("[E] DESCEND UNDERGROUND");}
 virtual void Use(class ALWCharacter* P)override;
};
