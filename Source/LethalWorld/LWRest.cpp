#include "LWVehicle.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWZombie.h"
#include "LWResident.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"

void ALWCharacter::SitOnChair(ALWWorldObject* Chair){
 if(!Chair||!CanAct())return;SitReturn=GetActorLocation();SittingChair=Chair;CancelReload();bAim=bSprint=bTrigger=false;
 GetCharacterMovement()->StopMovementImmediately();GetCharacterMovement()->DisableMovement();GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 SetActorLocation(Chair->GetActorTransform().TransformPosition(FVector(8,0,55)),false,nullptr,ETeleportType::TeleportPhysics);Controller->SetControlRotation(FRotator(0,Chair->GetActorRotation().Yaw,0));WeaponRoot->SetVisibility(false,true);Notify(TEXT("[E / SPACE] STAND UP"),5);
}
void ALWCharacter::StandFromChair(bool Force){
 if(!SittingChair)return;FVector At=SitReturn;FRotator Rotation=GetActorRotation();GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
 if(!GetWorld()->FindTeleportSpot(this,At,Rotation)&&!Force){GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);Notify(TEXT("NO ROOM TO STAND"));return;}
 SittingChair=nullptr;SetActorLocation(At,false,nullptr,ETeleportType::TeleportPhysics);GetCharacterMovement()->SetMovementMode(MOVE_Walking);SetMenuInput(bMenu||bInventory);if(!Force)RequestSave40();
}
bool ALWCharacter::SleepInBed(float Hours){
 if((!CanAct()&&!(Vehicle&&FName(Vehicle->Spec().Id)==TEXT("rv")&&FMath::Abs(Vehicle->Speed)<5&&!Vehicle->AutoDriving&&!Vehicle->Boarding&&!bMenu&&!bInventory&&Health>0))||Hunger<10||Thirst<10){Notify(TEXT("EAT AND DRINK BEFORE SLEEPING"));return false;}
 if(!bSafehouse)for(TActorIterator<ALWZombie> I(GetWorld());I;++I)if(!Cast<ALWResident>(*I)&&!I->bDead&&FVector::DistSquared(I->GetActorLocation(),GetActorLocation())<FMath::Square(1800.f)){Notify(TEXT("ENEMIES TOO CLOSE TO SLEEP"));return false;}
 World->TickWeather(FMath::Clamp(Hours,1.f,8.f)*World->DayLengthMinutes*60.f/24.f,this);Hunger-=5;Thirst-=5;Health=FMath::Min(MaxHealth(),Health+35*(1+Stat(TEXT("healing"))));Stamina=MaxStamina();Notify(TEXT("SLEPT 4 HOURS"));RequestSave40();return true;
}

void ALWCharacter::OpenBedMenu(ALWWorldObject* Bed){
 if(!Bed||Health<=0)return;if(auto* Car=Cast<ALWVehicle>(Bed))if(Car!=Vehicle||FMath::Abs(Car->Speed)>5||Car->AutoDriving||Car->Boarding){Notify(TEXT("PARK BEFORE USING THE BED"));return;}
 ClosePanels();RestBed=Bed;RPGPanel=4;DialogueText=TEXT("Rest here or make this bed your respawn point.");DialogueChoices={TEXT("Sleep for four hours."),TEXT("Set as respawn point."),TEXT("Leave.")};DialogueActions={TEXT("bed_sleep"),TEXT("bed_spawn"),TEXT("bye")};SetMenuInput(true);
}
void ALWCharacter::SetBedSpawn(ALWWorldObject* Bed){
 if(!Bed)return;FLWRespawnPoint Point;Point.Enabled=true;Point.Name=TEXT("BED");
 if(auto* Car=Cast<ALWVehicle>(Bed)){if(Car!=Vehicle||FMath::Abs(Car->Speed)>5||!Car->Record()||Car->Record()->Health<=0)return;Point.Vehicle=Car->RecordId;Point.Position=Car->GetActorLocation();Point.Name=TEXT("MOTORHOME BED");}
 else {if(Bed->UseType!=TEXT("bed")||FVector::Dist(GetActorLocation(),Bed->GetActorLocation())>550)return;Point.Position=GetActorLocation();}
 RPG.Respawn=Point;RequestSave40();Notify(TEXT("RESPAWN POINT SET"));
}
bool ALWCharacter::RestoreRespawn(){
 const auto Point=RPG.Respawn;if(!Point.Enabled)return false;
 if(!Point.Settlement.IsNone()){const auto* T=RPG.Settlements.Find(Point.Settlement);if(!T||!T->Leader||SettlementHostile(Point.Settlement))return false;}
 FVector At=Point.Position;
 if(!Point.Vehicle.IsNone()){const auto* R=World->Vehicles.Find(Point.Vehicle);if(!R||R->Health<=0)return false;At=R->Position;}
 SetActorLocation(At,false,nullptr,ETeleportType::TeleportPhysics);World->Stream(At,true);
 if(!Point.Vehicle.IsNone())for(TActorIterator<ALWVehicle> V(GetWorld());V;++V)if(V->RecordId==Point.Vehicle){if(V->Driver||V->AutoDriving||FMath::Abs(V->Speed)>5)return false;SetActorLocation(At);if(V->Enter(this,-2)){V->CabinEye=FVector(-735,0,150);V->Speed=0;TickVehicleSeat();bSafehouse=false;SetMenuInput(false);Notify(TEXT("RESPAWNED IN MOTORHOME"));return true;}return false;}
 if(!Point.Vehicle.IsNone())return false;
 FRotator Rotation=FRotator::ZeroRotator;if(!GetWorld()->FindTeleportSpot(this,At,Rotation))return false;
 SetActorLocation(At,false,nullptr,ETeleportType::TeleportPhysics);GetCharacterMovement()->SetMovementMode(MOVE_Walking);bSafehouse=ALWWorld::IsSafePosition(At);SetMenuInput(false);Notify(TEXT("RESPAWNED AT ")+Point.Name);return true;
}
