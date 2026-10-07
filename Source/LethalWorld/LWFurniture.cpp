#include "LWAircraft84.h"
#include "Engine/StaticMesh.h"
#include "LWBunker45.h"
#include "LWVehicle.h"
#include "EngineUtils.h"
#include "Misc/Crc.h"
#include "LWWorldObject.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWLootTable.h"
#include "Components/StaticMeshComponent.h"
void ALWWorldObject::SetFurniture(FName Type){
 UseType=Type;FName Mesh=Type==TEXT("bed")?TEXT("MotelBedV3"):Type==TEXT("chair")?TEXT("ChairV3"):Type==TEXT("radio")?TEXT("RadioV4"):Type==TEXT("water")?TEXT("FridgeV4"):Type==TEXT("workbench")?TEXT("WeaponBench39"):Type==TEXT("cooker")?TEXT("StoveV4"):Type==TEXT("sink")?TEXT("SinkV4"):TEXT("LockerV4");if(Type==TEXT("bed")&&RecordId.ToString().StartsWith(TEXT("bunker_")))Mesh=TEXT("HomeBedV13");Body->SetStaticMesh(World->Mesh(Mesh));if(Type==TEXT("chair")&&Body->GetStaticMesh()&&Body->GetStaticMesh()->GetName()==TEXT("SM_Chair65"))Body->SetRelativeRotation(FRotator(0,90,0));
 if(Type==TEXT("locker")&&!World->Containers.Contains(RecordId)){FLWContainerRecord R;R.Id=RecordId;R.Context=TEXT("road");R.Position=GetActorLocation();if(!RecordId.ToString().StartsWith(TEXT("bunker_")))for(auto I:LWLoot::Roll(TEXT("road"),int32(FCrc::StrCrc32(*RecordId.ToString()))))LWItems::Place(R.Items,I,12,12);World->Containers.Add(RecordId,R);}
}
void ALWWorldObject::UseFurniture(ALWCharacter* P){if(UseType==TEXT("airfield84")){LWAviation84::OpenService(P,GetActorLocation());return;}
 if(UseType==TEXT("delivery66")){P->OpenDelivery66(this);return;}
 if(UseType==TEXT("base45")||UseType==TEXT("garage45")||UseType==TEXT("surface45")||UseType==TEXT("lift45")||UseType.ToString().StartsWith(TEXT("call45_"))){
 auto* B=ALWBunker45::Ensure(P);if(!B)return;
 if(UseType==TEXT("surface45")){ALWVehicle* Car=nullptr;float Best=1500;for(TActorIterator<ALWVehicle> C(GetWorld());C;++C){float D=FVector::Dist(C->GetActorLocation(),B->Surface);if(D<Best){Best=D;Car=*C;}}B->StoreVehicle(Car);P->Notify(B->Message,5);}
 else if(UseType.ToString().StartsWith(TEXT("call45_")))B->GoFloor(FCString::Atoi(*UseType.ToString().Mid(7)));
 else B->Open(UseType==TEXT("garage45")?3:UseType==TEXT("lift45")?6:0);return;
 }
 P->QuestEvent(TEXT("interact"),UseType);
 if(UseType==TEXT("locker")){P->OpenContainer(this);return;}
 if(UseType==TEXT("radio")){World->Sound(TEXT("Speech1"),GetActorLocation());if(!World->PropStates.FindRef(RecordId)){World->PropStates.Add(RecordId,1);P->GainXP(25);}P->Notify(TEXT("CIVIL BAND // WAYSTATIONS OFFER WORK AND SHELTER"),6);}
 if(UseType==TEXT("bed")){P->OpenBedMenu(this);return;}
 if(UseType==TEXT("chair")){P->SitOnChair(this);return;}
 if(UseType==TEXT("water")||UseType==TEXT("sink")){
 int32 Now=int32(FDateTime::UtcNow().ToUnixTimestamp());if(Now-World->PropStates.FindRef(RecordId)<60){P->Notify(TEXT("FILTER RECHARGING // ONE MINUTE"));return;}
 P->Thirst=100;World->PropStates.Add(RecordId,Now);P->Notify(TEXT("FILTERED WATER // THIRST RESTORED"));}
 if(UseType==TEXT("cooker")){if(!P->ConsumeSupply(TEXT("food"),1)){P->Notify(TEXT("REQUIRES ONE FOOD RATION"));return;}P->Hunger=100;P->Notify(TEXT("HOT MEAL // HUNGER RESTORED"));}
 if(UseType==TEXT("workbench")){P->OpenWorkbench39(this);return;}
 P->RequestSave40();
}
