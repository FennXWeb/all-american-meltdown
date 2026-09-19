#include "LWFuel.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWWorldObject.h"
#include "LWZombie.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
void ALWCharacter::GunBash(){if(!CanAct()||IsUIOpen()||Vehicle||!ActiveGun()||Weapon<2||AttackTimer>0||bReloading||Stamina<8||bSafehouse)return;bTrigger=false;bAim=false;Stamina-=8;GunBashTimer=AttackTimer=AttackDuration=.55f;FVector A=Camera->GetComponentLocation(),D=Camera->GetForwardVector();World->Sound(TEXT("Swing"),A,.6f);FHitResult H;FCollisionQueryParams Q(NAME_None,true,this);if(GetWorld()->SweepSingleByChannel(H,A,A+D*180,FQuat::Identity,ECC_Visibility,FCollisionShape::MakeSphere(20),Q)&&!ALWWorld::IsSafePosition(H.ImpactPoint)){UGameplayStatics::ApplyPointDamage(H.GetActor(),8,D,H,Controller,this,nullptr);HitMarker=.12f;if(auto* Z=Cast<ALWZombie>(H.GetActor());Z&&!Z->bDead){Z->LaunchCharacter(D.GetSafeNormal2D()*FMath::Min(180.f,180.f/Z->BodyMassScale())+FVector(0,0,30),false,false);Z->Stagger=.18f/FMath::Max(1.f,Z->BodyMassScale());}}}
void ALWCharacter::RefillCanisters(ALWWorldObject* Pump){if(!CanAct()||!Pump||Pump->Kind!=ELWObjectKind::FuelPump||Pump->bChanged||FVector::Dist(Pump->GetActorLocation(),GetActorLocation())>400)return;int Fuel=LWFuel::RefillGasCans(this,Pump);for(auto& I:Inventory)if(I.Definition==TEXT("fuel_tank")){const int Capacity=LWItems::Def(I.Definition).Capacity;Fuel+=FMath::Max(0,Capacity-I.Rounds);I.Rounds=Capacity;}if(Fuel){World->Sound(TEXT("LockOpen"),Pump->GetActorLocation());SyncAmmoHUD();PersistWorldChange();}Notify(Fuel?TEXT("GAS CANS AND FUEL CANISTERS REFILLED"):TEXT("NO EMPTY GAS CANS OR CANISTERS CARRIED"));}
void ALWCharacter::MigrateDirectAmmo(){
 TArray<FLWItemInstance> Recovery;
 auto Convert=[&](TArray<FLWItemInstance>& Items,int W,int H){
  for(auto& G:Items)if(G.Definition==TEXT("missile_launcher")||G.Definition==TEXT("taser")){
   G.Rounds+=G.Chamber==1?1:0;G.Chamber=0;G.LoadedMagazine.Invalidate();
  }
  TArray<FLWItemInstance> Changed;
  for(int I=Items.Num()-1;I>=0;I--){auto& Old=Items[I];bool Rocket=Old.Definition==TEXT("rocket_tube");bool Taser=Old.Definition==TEXT("taser_cartridge")||Old.Definition==TEXT("ammo_dart");if(!Rocket&&!Taser)continue;
   int Count=Old.Definition==TEXT("ammo_dart")?Old.Count:Old.Rounds;
   const FName Id=Rocket?TEXT("ammo_rocket"):TEXT("battery");int Serial=0;
   while(Count>0){int N=FMath::Min(Count,LWItems::Def(Id).MaxStack);auto New=LWItems::Make(Id,N);New.Id=FGuid::NewDeterministicGuid(Old.Id.ToString()+FString::FromInt(Serial++));Changed.Add(New);Count-=N;}Items.RemoveAt(I);
  }
  for(auto I:Changed){for(auto& Other:Items)if(LWItems::StackCompatible(I,Other)){int N=FMath::Min(I.Count,LWItems::Def(I.Definition).MaxStack-Other.Count);Other.Count+=N;I.Count-=N;}if(I.Count>0&&!LWItems::Place(Items,I,W,H))Recovery.Add(I);}
 };
 Convert(Inventory,12,LWItems::InventoryHeight(Inventory));Convert(Stash,12,14);
 if(World){for(auto& Pair:World->Containers)Convert(Pair.Value.Items,Pair.Value.Width,Pair.Value.Height);
  if(!Recovery.IsEmpty()){FLWContainerRecord C;C.Id=FName(*FString::Printf(TEXT("ammo_recovery_%s"),*FGuid::NewGuid().ToString()));C.Context=TEXT("recovered_ammunition");C.Position=GetActorLocation();C.Width=12;C.Height=FMath::Max(12,Recovery.Num()*4);C.Unlocked=true;C.bDropped=true;for(auto I:Recovery)LWItems::Place(C.Items,I,C.Width,C.Height);World->Containers.Add(C.Id,C);World->SpawnObject(ELWObjectKind::Container,C.Id,C.Position);Notify(TEXT("Recovered ammunition placed in a bag beside you."),8);}
 }
}
