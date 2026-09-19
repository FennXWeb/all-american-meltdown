#include "LWWorldObject.h"
#include "LWWorld.h"
#include "GameFramework/Pawn.h"
#include "EngineUtils.h"
#include "Engine/World.h"

bool ALWWorldObject::CanCompanionOpenDoor()const{
 if(Kind!=ELWObjectKind::Door||!IsValid(World))return false;
 const auto* Lock=World->Containers.Find(RecordId);
 return !Lock||Lock->LockTier<=0||Lock->Unlocked;
}
bool ALWWorldObject::RequestCompanionDoor(AActor* User){
 if(!IsValid(User)||!CanCompanionOpenDoor())return false;
 const FVector Local=GetActorTransform().InverseTransformPosition(User->GetActorLocation());
 if(FVector2D::Distance(FVector2D(Local),FVector2D(80,0))>280||Local.Z<0||Local.Z>250)return false;
 // A manual opening is already usable; never reverse a recent manual closing.
 if(GetWorld()->GetTimeSeconds()<ManualDoorUntil)return bChanged&&DoorAngle>=88;
 if(!bChanged){bChanged=true;CompanionOpenedDoor=true;World->PropStates.Add(RecordId,1);World->Sound(TEXT("DoorHinge"),GetActorLocation());}
 if(CompanionOpenedDoor)CompanionDoorUntil=GetWorld()->GetTimeSeconds()+2;
 return DoorAngle>=88;
}
void ALWWorldObject::TickCompanionDoor(){
 if(!CompanionOpenedDoor||!bChanged||GetWorld()->GetTimeSeconds()<CompanionDoorUntil)return;
 // Reserve the doorway and complete swing arc for people, including the player and downed crew.
 for(TActorIterator<APawn> It(GetWorld());It;++It){FVector L=GetActorTransform().InverseTransformPosition(It->GetActorLocation());if(FVector2D::Distance(FVector2D(L),FVector2D(70,0))<235&&L.Z>-20&&L.Z<300){CompanionDoorUntil=GetWorld()->GetTimeSeconds()+.4;return;}}
 bChanged=false;CompanionOpenedDoor=false;World->PropStates.Add(RecordId,0);World->Sound(TEXT("DoorHinge"),GetActorLocation());
}
