#include "LWVehicleExplosion68.h"
#include "LWVehicle.h"
#include "LWGarage45.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWResident.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SpotLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "ProceduralMeshComponent.h"

float ALWVehicle::TakeDamage(float Damage,const FDamageEvent& Event,AController* DamageInstigator,AActor* Causer){
 auto* R=Record();if(!R||R->Exploded||Damage<=0||!FMath::IsFinite(Damage))return 0;
 if(Event.IsOfType(FRadialDamageEvent::ClassID))Damage=AActor::TakeDamage(Damage,Event,DamageInstigator,Causer);
 Damage/=1+(IsAircraft84()?4.f:0.f)+(FName(Spec().Id)==TEXT("apc")?3.f:FName(Spec().Id)==TEXT("armoredtruck")?2.f:0.f)+LWGarage45::Stat(R,TEXT("armor"));
 FVector At=GetActorLocation(),Dir=-GetActorForwardVector();
 if(Event.IsOfType(FPointDamageEvent::ClassID)){const auto& E=static_cast<const FPointDamageEvent&>(Event);At=E.HitInfo.ImpactPoint;Dir=E.ShotDirection;}
 else if(Causer&&Causer!=this){Dir=(At-Causer->GetActorLocation()).GetSafeNormal();At-=Dir*Spec().HalfWidth;}
 if(!IsAircraft84())AddDent(At,Dir,Damage);
 R->Health=FMath::Max(0.f,R->Health-Damage);if(R->Health<=45&&R->FireRemaining<=0){R->FireRemaining=R->Health<=0?5:18;World->Sound(TEXT("CarImpact"),GetActorLocation());if(Driver)Driver->Notify(TEXT("VEHICLE ON FIRE // GET OUT"),5);}
 if(R->Health<=0)EngineOn=false;return Damage;
}
void ALWVehicle::Explode(){
 auto* R=Record();if(!R||R->Exploded)return;R->Exploded=true;R->Health=0;R->FireRemaining=0;EngineOn=AutoDriving=Boarding=false;if(ServoAudio57)ServoAudio57->Stop();Throttle=Speed=0;
 DamageClock=0;auto* P=Driver.Get();if(P)Exit(true);UnloadPassengers();ConvoyLeader=nullptr;ConvoyOwner=nullptr;StopDriveAudio();
 for(auto& L:Headlamps)if(L)L->SetVisibility(false);for(auto& FX:DamageFX)if(FX)FX->DestroyComponent();DamageFX.Empty();
 ALWVehicleExplosion68::Spawn(this);World->Sound(TEXT("FuelExplosion"),GetActorLocation(),1.0f);World->Noise(GetActorLocation(),18000);
 TArray<AActor*> Ignore;Ignore.Add(this);UGameplayStatics::ApplyRadialDamageWithFalloff(this,IsAircraft84()?600:180,20,GetActorLocation()+FVector(0,0,70),IsAircraft84()?900:180,IsAircraft84()?5500:1150,1,nullptr,Ignore,this,nullptr,ECC_Visibility);
 if(P)P->RequestSave40();
}
void ALWVehicle::TickDamage(float Dt){
 auto* R=Record();if(!R)return;DamageClock+=Dt;if(bDentsDirty&&GetWorld()->GetTimeSeconds()-LastDentBuild>=.2f)RebuildDents();
 if(R->Exploded&&!WreckShown){if(DamageClock>.1f)DamageClock=2;WreckShown=true;for(auto& M:Details)if(M)for(int I=0;I<M->GetNumMaterials();I++)M->SetMaterial(I,World->Material(TEXT("Rubber")));if(Body)Body->SetMaterial(0,World->Material(TEXT("Rust")));if(DentedBody)for(int I=0;I<DentedBody->GetNumMaterials();I++)DentedBody->SetMaterial(I,World->Material(TEXT("Rust")));}
 if(R->Exploded)return;
 if(R->Health<=45&&R->FireRemaining<=0)R->FireRemaining=12;
 if(R->FireRemaining<=0)return;
 R->FireRemaining=FMath::Max(0.f,R->FireRemaining-Dt);R->Health=FMath::Max(0.f,R->Health-Dt*2);
 if(DamageFX.IsEmpty())for(int I=0;I<10;I++){auto* M=NewObject<UStaticMeshComponent>(this);M->SetupAttachment(Root);M->SetStaticMesh(World->Mesh(TEXT("Sphere")));if(!M->GetStaticMesh())M->SetStaticMesh(World->Mesh(TEXT("Cube")));M->SetMaterial(0,World->Material(I<5?TEXT("Glow"):TEXT("Rubber")));M->SetCollisionEnabled(ECollisionEnabled::NoCollision);M->RegisterComponent();DamageFX.Add(M);}
 for(int I=0;I<DamageFX.Num();I++){float Cycle=FMath::Fmod(DamageClock*(I<5?1.8f:.65f)+I*.21f,1.f);DamageFX[I]->SetRelativeLocation(FVector(100.f+FMath::Sin(I*2.f)*35,FMath::Cos(I*2.f)*45,70+Cycle*(I<5?130:380)));DamageFX[I]->SetRelativeScale3D(FVector(I<5?.25f:.4f+Cycle*.8f,I<5?.25f:.4f+Cycle*.8f,I<5?.8f*(1-Cycle):.4f+Cycle));}
 if(R->FireRemaining<=0)Explode();
}

