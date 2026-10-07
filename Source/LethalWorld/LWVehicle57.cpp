#include "LWGeography84.h"
#include "LWVehicle.h"
#include "LWCharacter.h"
#include "LWResident.h"
#include "LWWorld.h"
#include "LWWeaponEffect.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
bool ALWVehicle::HasTurret57()const{return FName(Spec().Id)==TEXT("apc")||FName(Spec().Id)==TEXT("technical");}
bool ALWVehicle::IsHelicopter57()const{return FName(Spec().Id)==TEXT("helicopter");}
bool ALWVehicle::IsExpansion57()const{return HasTurret57()||IsHelicopter57()||FName(Spec().Id)==TEXT("armoredtruck");}
void ALWVehicle::Build57(){
 const bool Air=IsHelicopter57(),APC=FName(Spec().Id)==TEXT("apc");
 Body->SetStaticMesh(World->Mesh(FName(*(FString(TEXT("Vehicle42_"))+Spec().Id))));Body->EmptyOverrideMaterials();
 for(auto& C:Details){if(!C)continue;if(Wheels.Contains(C)&&!Air)continue;if(C==SteeringWheel||C==GloveLid||!C->ComponentTags.IsEmpty())continue;C->SetVisibility(false);C->SetCollisionEnabled(ECollisionEnabled::NoCollision);}
 auto Mesh=[&](FName Name,FVector At){Part(Name,At,FVector(1));auto* M=Details.Last().Get();M->EmptyOverrideMaterials();M->SetCollisionEnabled(ECollisionEnabled::NoCollision);return M;};
 Mesh(FName(*(FString(TEXT("Cabin42_"))+Spec().Id)),FVector::ZeroVector);
 if(Windshield){Windshield->SetStaticMesh(World->Mesh(FName(*(FString(TEXT("Windshield42_"))+Spec().Id))));Windshield->SetRelativeTransform(FTransform::Identity);Windshield->SetVisibility(true);Windshield->EmptyOverrideMaterials();Windshield->SetMaterial(0,World->Material(TEXT("WindshieldV9")));GlassWet=Windshield->CreateDynamicMaterialInstance(0);}
 for(auto& W:Wheels){W->SetVisibility(!Air);W->EmptyOverrideMaterials();W->SetStaticMesh(World->Mesh(APC?TEXT("Wheel57Heavy"):TEXT("Wheel57Light")));W->SetRelativeScale3D(FVector(1.f));W->SetRelativeLocation(FVector(W->GetRelativeLocation().X,W->GetRelativeLocation().Y,APC?42:32));}
 if(APC)for(float X:{-20.f,-150.f})for(int Side:{-1,1}){auto* W=Mesh(TEXT("Wheel57Heavy"),FVector(X,Side*117,42));W->SetRelativeScale3D(FVector(1.f));Wheels.Add(W);}
 SteeringWheel->SetStaticMesh(World->Mesh(TEXT("Steering42")));SteeringWheel->EmptyOverrideMaterials();SteeringWheel->SetRelativeLocation(Air?FVector(44,-43,119):FVector(-2,-43,98));
 for(int I=0;I<4;I++){const TCHAR* D[]={TEXT("Dial42Speed"),TEXT("Dial42RPM"),TEXT("Dial42Fuel"),TEXT("Dial42Temp")};FVector At(Air?62:9,-70+I*18,Air?129:111);Mesh(D[I],At)->SetRelativeScale3D(FVector(.7f));GaugeNeedles42.Add(Mesh(TEXT("Needle42"),At-FVector(.3,0,0)));GaugeNeedles42.Last()->SetRelativeScale3D(FVector(.7f));}
 if(HasTurret57()){
  Turret57=Mesh(TEXT("Turret57"),FVector(-170,0,APC?285:175));Turret57->SetRelativeScale3D(FVector(APC?1.f:.65f));
  Barrel57=NewObject<UStaticMeshComponent>(this);Barrel57->SetupAttachment(Turret57);Barrel57->SetStaticMesh(World->Mesh(APC?TEXT("Cannon57"):TEXT("MachineGun57")));Barrel57->SetRelativeLocation(FVector(30,0,15));Barrel57->SetCollisionEnabled(ECollisionEnabled::NoCollision);Barrel57->RegisterComponent();
  if(Record()->TurretRounds57<0)Record()->TurretRounds57=APC?24:100;
 }
 if(Air){Rotor57=Mesh(TEXT("Rotor57"),FVector(-100,0,305));auto* Tail=Mesh(TEXT("TailRotor57"),FVector(-630,-22,230));Tail->SetRelativeRotation(FRotator(0,0,90));Tail->ComponentTags.Add(TEXT("tailrotor57"));Chassis->SetBoxExtent(FVector(330,108,48));}
}
void ALWVehicle::ReloadTurret57(){
 if(LWGeography84::Canada(FVector2D(GetActorLocation()))){TurretTrigger57=false;return;}
 if(!HasTurret57()||ReloadClock57>0||!Record())return;const bool APC=FName(Spec().Id)==TEXT("apc");const int Capacity=APC?24:100;
 auto* Cargo=World->Containers.Find(RecordId);if(!Cargo)return;const FName Ammo=APC?TEXT("ammo_30mm"):TEXT("ammo_762");int Need=Capacity-Record()->TurretRounds57,Added=0;
 for(auto& Item:Cargo->Items)if(Item.Definition==Ammo&&Item.Count>0&&Need>0){int N=FMath::Min(Need,Item.Count);Item.Count-=N;Need-=N;Added+=N;}
 Cargo->Items.RemoveAll([](const auto& I){return I.Count<=0;});if(Added){Record()->TurretRounds57+=Added;ReloadClock57=APC?4.5f:3.4f;World->Sound(TEXT("TurretReload57"),Turret57->GetComponentLocation());if(Driver)Driver->RequestSave40();}
 else if(Driver&&PlayerSeat==0)Driver->Notify(APC?TEXT("PUT 30MM SHELLS IN VEHICLE CARGO"):TEXT("PUT 7.62 AMMO IN VEHICLE CARGO"));
}
void ALWVehicle::TickTurret57(float Dt){
 if(LWGeography84::Canada(FVector2D(GetActorLocation()))){TurretTrigger57=false;return;}
 if(!HasTurret57()||!Turret57||!Barrel57||!Record()||Held49||Airborne49)return;
 ShotClock57-=Dt;ReloadClock57=FMath::Max(0.f,ReloadClock57-Dt);TargetClock57-=Dt;TurretHeat57=FMath::Max(0.f,TurretHeat57-Dt*.17f);
 const bool Player=Driver&&PlayerSeat==0&&Driver->Health>0&&!Driver->IsUIOpen();ALWResident* NPC=Passengers.IsValidIndex(0)?Passengers[0].Get():nullptr;
 if(!Player&&(!IsValid(NPC)||NPC->DownTime>0)){if(ServoAudio57)ServoAudio57->SetVolumeMultiplier(0);TurretTrigger57=false;GunnerTarget57.Reset();return;}
 const bool APC=FName(Spec().Id)==TEXT("apc");FVector Start=Turret57->GetComponentLocation()+FVector(0,0,30),Aim;bool Fire=false;
 FCollisionQueryParams Q(SCENE_QUERY_STAT(Turret57),true,this);if(Driver)Q.AddIgnoredActor(Driver);if(NPC)Q.AddIgnoredActor(NPC);
 if(Player){Aim=Driver->Camera->GetComponentLocation()+Driver->Camera->GetForwardVector()*14000;Fire=TurretTrigger57;}
 else{
  if(TargetClock57<=0){TargetClock57=.4f;GunnerTarget57.Reset();float Best=10000*10000;
   for(TActorIterator<ALWZombie> I(GetWorld());I;++I){if(I->bDead||Cast<ALWResident>(*I))continue;float D=FVector::DistSquared(Start,I->GetActorLocation());if(D>=Best)continue;FHitResult H;if(GetWorld()->LineTraceSingleByChannel(H,Start,I->GetActorLocation(),ECC_Visibility,Q)&&H.GetActor()!=*I)continue;GunnerTarget57=*I;Best=D;}
  }
  if(!GunnerTarget57.IsValid()||GunnerTarget57->bDead)return;Aim=GunnerTarget57->GetActorLocation()+GunnerTarget57->GetVelocity()*.08f;Fire=true;
 }
 if(!ServoAudio57){ServoAudio57=World->Sound(TEXT("TurretServo57"),Start,.25f,1,true);if(ServoAudio57)ServoAudio57->AttachToComponent(Turret57,FAttachmentTransformRules::KeepWorldTransform);}
 const FRotator Local=GetActorRotation().UnrotateVector(Aim-Start).Rotation();if(ServoAudio57)ServoAudio57->SetVolumeMultiplier(FMath::Abs(FMath::FindDeltaAngleDegrees(Turret57->GetRelativeRotation().Yaw,Local.Yaw))>1?.25f:0.f);const float Yaw=FMath::FixedTurn(Turret57->GetRelativeRotation().Yaw,Local.Yaw,Dt*(APC?65:100));Turret57->SetRelativeRotation(FRotator(0,Yaw,0));Barrel57->SetRelativeRotation(FRotator(FMath::Clamp(Local.Pitch,APC?-8.f:FMath::Abs(FMath::FindDeltaAngleDegrees(Yaw,0))<65?22.f:-20.f,65.f),0,0));
 if(NPC){NPC->SetActorRelativeRotation(FRotator(0,Yaw,0));for(int I=3;I<5;I++)NPC->Parts[I]->SetRelativeRotation(FRotator(-65,0,0));}
 if(!Fire||ShotClock57>0||ReloadClock57>0||TurretHeat57>.95f||FMath::Abs(FMath::FindDeltaAngleDegrees(Yaw,Local.Yaw))>4||FMath::Abs(Barrel57->GetRelativeRotation().Pitch-Local.Pitch)>5)return;
 if(Record()->TurretRounds57<=0){ReloadTurret57();ShotClock57=.6f;return;}
 const FVector Muzzle=Barrel57->GetComponentTransform().TransformPosition(FVector(APC?205:128,0,APC?0:3.5));const FVector Dir=Barrel57->GetForwardVector();FHitResult H;
 // A muzzle-clearance trace is essential when the turret turns over the cab.
 FHitResult SelfHit;FCollisionQueryParams SelfQ;SelfQ.AddIgnoredActor(this);if(Driver)SelfQ.AddIgnoredActor(Driver);if(NPC)SelfQ.AddIgnoredActor(NPC);
 if(GetWorld()->LineTraceSingleByChannel(SelfHit,Start,Muzzle,ECC_Visibility,SelfQ))return;
 GetWorld()->LineTraceSingleByChannel(H,Muzzle,Muzzle+Dir*14000,ECC_Visibility,Q);
 if(!Player&&H.bBlockingHit&&(Cast<ALWResident>(H.GetActor())||Cast<ALWCharacter>(H.GetActor())||Cast<ALWVehicle>(H.GetActor())))return;
 Record()->TurretRounds57--;ShotClock57=APC?.65f:.11f;TurretHeat57+=APC?.1f:.045f;
 if(H.bBlockingHit)UGameplayStatics::ApplyPointDamage(H.GetActor(),APC?160:32,Dir,H,Driver?Driver->Controller:nullptr,this,nullptr);
 ALWWeaponEffect::Gunfire(this,World,Muzzle,H.bBlockingHit?H.ImpactPoint:Muzzle+Dir*14000,false);World->Sound(APC?TEXT("TurretCannon57"):TEXT("TurretMG57"),Muzzle,1.2f);World->Noise(Muzzle,14000);
}
bool ALWVehicle::TickFlight57(float Dt){
 if(!IsHelicopter57())return false;TickFuel(Dt);
 FCollisionQueryParams Q(NAME_None,false,this);if(Driver)Q.AddIgnoredActor(Driver);FHitResult Ground;const FVector Here=GetActorLocation();
 const bool Support=GetWorld()->LineTraceSingleByChannel(Ground,Here,Here-FVector(0,0,30000),ECC_WorldStatic,Q);const float Floor=Support?Ground.ImpactPoint.Z:World->HeightAt(FVector2D(Here));Altitude57=FMath::Max(0.f,float(Here.Z-Floor-75));
 auto* PC=Driver?Cast<APlayerController>(Driver->Controller):nullptr;const bool Input=PC&&PlayerSeat==-1&&!Driver->IsUIOpen()&&Driver->Health>0;
 RotorSpeed57=FMath::FInterpConstantTo(RotorSpeed57,EngineOn?1.f:0.f,Dt,EngineOn?.18f:.07f);
 if(Rotor57)Rotor57->AddLocalRotation(FRotator(0,Dt*RotorSpeed57*2100,0));for(auto& C:Details)if(C&&C->ComponentHasTag(TEXT("tailrotor57")))C->AddLocalRotation(FRotator(0,Dt*RotorSpeed57*3600,0));
 const float Lift=Input?(PC->IsInputKeyDown(EKeys::SpaceBar)?1.f:PC->IsInputKeyDown(EKeys::LeftControl)?-1.f:0.f):0.f;
 const float Gas=Input?Throttle:0,YawInput=Input?Steer:0;FRotator R=GetActorRotation();R.Yaw+=YawInput*Dt*38*RotorSpeed57;R.Pitch=FMath::FInterpTo(R.Pitch,-Gas*12,Dt,2);R.Roll=FMath::FInterpTo(R.Roll,-YawInput*10,Dt,2);
 FVector Desired=FRotator(0,R.Yaw,0).Vector()*Gas*Spec().MaxSpeed*RotorSpeed57;Desired.Z=Lift*950;
 if(EngineOn&&RotorSpeed57>.7f)Velocity57=FMath::VInterpTo(Velocity57,Desired,Dt,.8f);else{Velocity57.X*=FMath::Max(0.f,1-Dt*.2f);Velocity57.Y*=FMath::Max(0.f,1-Dt*.2f);Velocity57.Z-=980*Dt*(1-RotorSpeed57*.8f);}
 FVector Next=Here+Velocity57*Dt;if(Next.Z<Floor+75){if(Velocity57.Z<-900)UGameplayStatics::ApplyDamage(this,-Velocity57.Z*.07f,nullptr,this,nullptr);Next.Z=Floor+75;Velocity57.Z=0;Velocity57.X*=FMath::Max(0.f,1-Dt*2);Velocity57.Y*=FMath::Max(0.f,1-Dt*2);}
 FHitResult H;SetActorLocationAndRotation(Next,R,true,&H);if(H.bBlockingHit){const float Impact=FMath::Abs(FVector::DotProduct(Velocity57,H.ImpactNormal));if(Impact>450)UGameplayStatics::ApplyPointDamage(this,Impact*.045f,-H.ImpactNormal,H,nullptr,this,nullptr);Velocity57=FVector::VectorPlaneProject(Velocity57,H.ImpactNormal)*.45f;}
 Speed=Velocity57.Size();TickDriveAudio(Dt,false,FMath::Clamp(.35f+FMath::Abs(Gas)*.5f+FMath::Abs(Lift)*.3f,0.f,1.f));TickInstruments42(Dt);TickGlass(Dt);SyncRecord();if(!Driver&&!ConvoyOwner&&Record()&&!Record()->Stored45)if(auto* P=UGameplayStatics::GetPlayerPawn(this,0))if(FVector::Dist2D(P->GetActorLocation(),GetActorLocation())>(World->RenderRadius+1)*LWGen::ChunkSize)Destroy();return true;
}
