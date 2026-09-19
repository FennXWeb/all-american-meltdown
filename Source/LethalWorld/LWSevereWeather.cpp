#include "LWWorld.h"
#include "Engine/StaticMesh.h"
#include "LWCharacter.h"
#include "LWVehicle.h"
#include "Camera/CameraComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
void ALWWorld::TickSevereWeather(float Dt,ALWCharacter* P){
 const bool Severe=WeatherType>=5,Snow=WeatherType>=8,Shelter=P->bIndoors||P->bSafehouse||(P->Vehicle&&P->Vehicle->Spec().Seats>1);
 VisibilityRange=WeatherType==7?700:WeatherType==5||WeatherType==10?1400:WeatherType==8||WeatherType==9?2200:WeatherType==2?2800:6000;
 if(Severe){WeatherFog->GetComponent()->SetFogDensity(WeatherType==7?.18f:WeatherType==5?.095f:.06f);WeatherFog->GetComponent()->SetStartDistance(35);WeatherFog->GetComponent()->SetFogInscatteringColor(WeatherType==5?FLinearColor(.32,.19,.07):WeatherType==8?FLinearColor(.12,.19,.13):FLinearColor(.23,.26,.29));}
 auto Batch=[&](TObjectPtr<UInstancedStaticMeshComponent>& B,FName M,int Count){if(B)return;B=NewObject<UInstancedStaticMeshComponent>(this);B->SetStaticMesh(Mesh(TEXT("Cube")));B->SetMaterial(0,Material(M));B->SetCollisionEnabled(ECollisionEnabled::NoCollision);B->SetCastShadow(false);B->RegisterComponent();for(int I=0;I<Count;I++)B->AddInstance(FTransform(FVector::ZeroVector));};
 Batch(StormParticles,Snow?TEXT("Bone"):TEXT("Cloth"),240);StormParticles->SetMaterial(0,Material(Snow?TEXT("Bone"):TEXT("Cloth")));StormParticles->SetVisibility(Severe&&WeatherType!=7&&!Shelter);
 const double T=(DayNumber*24.+TimeOfDay)*60;const FVector Eye=P->Camera->GetComponentLocation();
 if(Severe&&!Shelter&&WeatherClock+Dt>=.07f)for(int I=0;I<240;I++){
  FRandomStream R(I*7919+Seed);float X=FMath::Fmod(R.FRandRange(0,3000)+T*(Snow?90:600),3000)-1500,Y=R.FRandRange(-1500,1500)+FMath::Sin(T+I)*60,Z=FMath::Fmod(R.FRandRange(0,1400)-T*(Snow?55:130),1400);if(Z<0)Z+=1400;
  FVector V=Eye+FVector(X,Y,Z-400);FHitResult Hit;FCollisionQueryParams Q(NAME_None,false,P);bool Covered=GetWorld()->LineTraceSingleByChannel(Hit,V,V+FVector(0,0,2500),ECC_Visibility,Q);
  StormParticles->UpdateInstanceTransform(I,FTransform(FRotator(I*13,T*40,0),V,FVector(Snow?.035:.18,Snow?.025:.018,Covered?0:Snow?.008:.02)),true,I==239,true);
 }
 Batch(Tornado,TEXT("Cloth"),80);Tornado->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere")));Tornado->SetVisibility(WeatherType==6&&!P->bSafehouse);
 if(WeatherType!=6)TornadoActive=false;
 if(WeatherType==6){const FVector2D Cell(FMath::FloorToDouble(Eye.X/15000)*15000,FMath::FloorToDouble(Eye.Y/15000)*15000);if(!TornadoActive){StormOrigin=FVector(Cell+FVector2D(7500,7500),0);TornadoActive=true;}FVector Center=StormOrigin+FVector(FMath::Sin(T*.006)*3500,FMath::Cos(T*.004)*3500,0);Center.Z=HeightAt(FVector2D(Center));
  for(int I=0;I<80;I++){float Z=I*65,Radius=100+I*10,A=T*2+I*.7;Tornado->UpdateInstanceTransform(I,FTransform(FRotator(0,A*57.3,0),Center+FVector(FMath::Sin(A*.25)*Radius*.12,FMath::Cos(A*.25)*Radius*.12,Z),FVector(Radius*.020,Radius*.020,3.2)),true,I==79,true);}
  ExposureClock+=Dt;if(ExposureClock>=1){ExposureClock=0;float D=FVector::Dist2D(Eye,Center);if(!Shelter&&D<1100){P->LaunchCharacter((Center-Eye).GetSafeNormal2D()*350+FVector(0,0,300),false,false);UGameplayStatics::ApplyDamage(P,D<450?18:5,nullptr,this,nullptr);}
   for(TActorIterator<ALWVehicle> V(GetWorld());V;++V)if(FVector::Dist2D(V->GetActorLocation(),Center)<700){UGameplayStatics::ApplyDamage(*V,12,nullptr,this,nullptr);if(!V->Driver)V->AddActorWorldOffset(FVector(0,0,25),true);}
  }
 }else if(WeatherType==8||WeatherType==10){ExposureClock+=Dt;if(ExposureClock>=8){ExposureClock=0;if(!Shelter){P->Stamina=FMath::Max(0.f,P->Stamina-12);P->Thirst=FMath::Max(0.f,P->Thirst-1);}}}else ExposureClock=0;
}
