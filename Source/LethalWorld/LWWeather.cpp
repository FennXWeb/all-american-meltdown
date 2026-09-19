#include "Engine/StaticMesh.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWVehicle.h"
#include "LWAudioCatalog.h"
#include "Camera/CameraComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/StaticMeshActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"

void ALWWorld::TickWeather(float Dt,ALWCharacter* P)
{
 const bool Sheltered=P->bIndoors||(P->Vehicle&&P->Vehicle->Spec().Seats>1);
 TimeOfDay+=Dt*24.f/(FMath::Max(1.f,DayLengthMinutes)*60.f);
 while(TimeOfDay>=24){TimeOfDay-=24;++DayNumber;}
 const double Hours=(DayNumber-1)*24.+TimeOfDay;
 const int Period=FMath::FloorToInt(Hours/3.);
 auto Type=[&](int N){uint32 H=LWGen::Hash(N,0,Seed,8181)%100;return H<20?0:H<35?1:H<45?2:H<61?3:H<72?4:H<80?5:H<83?6:H<89?7:H<93?8:H<97?9:10;};
 const int CrownUntil=PropStates.FindRef(TEXT("weather_crown_until"));const int Override=Hours*60<CrownUntil?PropStates.FindRef(TEXT("weather_crown_type")):WeatherOverride;
 WeatherType=Override>=0?Override:Type(Period);int Prev=Override>=0?Override:Type(Period-1);
 float Blend=FMath::SmoothStep(0.f,1.f,float(FMath::Fmod(Hours,3.)/.35));
 auto Cloud=[](int T){return T==0?.05f:T==1?.65f:T==2?.8f:1.f;};
 auto Wet=[](int T){return T==3?.6f:T==4||T==6?1.f:0.f;};
 CloudAmount=FMath::Lerp(Cloud(Prev),Cloud(WeatherType),Blend);RainAmount=FMath::Lerp(Wet(Prev),Wet(WeatherType),Blend);
 UpdateWetness(Dt);
 float FogAmount=FMath::Lerp(Prev==2||Prev>=5?1.f:0.f,WeatherType==2||WeatherType>=5?1.f:0.f,Blend);
 const float Altitude=FMath::Sin((TimeOfDay-6)*PI/12);
 float Day=FMath::SmoothStep(0.f,.38f,FMath::Max(0.f,Altitude));
 const FVector SunDirection=FRotator(Altitude*75,TimeOfDay*15-90,0).Vector(),MoonDirection=-SunDirection;
 auto Disc=[&](TObjectPtr<UStaticMeshComponent>& C,FName Mat,FVector Dir,float Radius,bool Visible){if(!C){C=NewObject<UStaticMeshComponent>(this);C->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere")));C->SetMaterial(0,Material(Mat));C->SetCollisionEnabled(ECollisionEnabled::NoCollision);C->SetCastShadow(false);C->RegisterComponent();}C->SetWorldLocation(P->Camera->GetComponentLocation()+Dir*60000);C->SetWorldScale3D(FVector(Radius));C->SetVisibility(Visible&&!P->bSafehouse);};
 Disc(SunDisc,TEXT("SunV16"),SunDirection,14,Altitude>=0&&CloudAmount<.85f);
 Disc(MoonDisc,TEXT("MoonV16"),MoonDirection,18,Altitude<.03f&&CloudAmount<.9f);
 float Flash=WeatherType==4&&FMath::Fmod(Hours*240.,23.)<.15?3.f:0.f;
 SunLight->SetActorRotation((-SunDirection).Rotation());
 SunLight->GetLightComponent()->SetIntensity(Day*(7.5f-CloudAmount*4.5f)+Flash);
 SunLight->GetLightComponent()->SetLightColor(FMath::Lerp(FLinearColor(1.f,.45f,.23f),FLinearColor(.87f,.9f,.8f),FMath::Clamp(Day*3,0.f,1.f)));
 MoonLight->SetActorRotation((-MoonDirection).Rotation());MoonLight->GetLightComponent()->SetIntensity((1-Day)*.055f*(1-CloudAmount*.75f));
 AmbientLight->GetLightComponent()->SetIntensity(.022f+Day*(1.15f-.50f*CloudAmount));
 FLinearColor SkyColor=FMath::Lerp(FLinearColor(.006f,.012f,.022f),FLinearColor(.29f,.38f,.46f),Day)*(1-.5f*CloudAmount);
 if(!WeatherSky){WeatherSky=UMaterialInstanceDynamic::Create(Material(TEXT("Sky")),this);Sky->GetStaticMeshComponent()->SetMaterial(0,WeatherSky);}
 WeatherSky->SetVectorParameterValue(TEXT("SkyColor"),SkyColor+FLinearColor(Flash*.15f,Flash*.15f,Flash*.15f,0));
 WeatherFog->GetComponent()->SetFogDensity(.005f+FogAmount*.06f+RainAmount*.018f);
 WeatherFog->GetComponent()->SetStartDistance(FMath::Lerp(1800.f,70.f,FMath::Max(FogAmount,RainAmount*.65f)));
 WeatherFog->GetComponent()->SetFogInscatteringColor(SkyColor);
 if(!Rain){Rain=NewObject<UInstancedStaticMeshComponent>(this);Rain->SetStaticMesh(Mesh(TEXT("Cube")));Rain->SetMaterial(0,Material(TEXT("Bone")));Rain->SetCollisionEnabled(ECollisionEnabled::NoCollision);Rain->SetCastShadow(false);Rain->RegisterComponent();for(int I=0;I<128;I++)Rain->AddInstance(FTransform(FVector::ZeroVector));}
 Rain->SetVisibility(RainAmount>.01f&&!Sheltered);
 if(!RainAudio)RainAudio=Sound(TEXT("Rain"),P->GetActorLocation(),0);
 if(RainAudio){const FLWAudioSlot* Slot=AudioCatalog?AudioCatalog->Slots.Find(TEXT("Rain")):nullptr;const float SlotVolume=Slot&&FMath::IsFinite(Slot->Volume)?FMath::Clamp(Slot->Volume,0.f,4.f):1.f;RainAudio->SetVolumeMultiplier(RainAmount*(Sheltered?.09f:.45f)*SlotVolume);RainAudio->SetLowPassFilterEnabled(true);RainAudio->SetLowPassFilterFrequency(Sheltered?700:16000);}
 TickSevereWeather(Dt,P);WeatherClock+=Dt;if(WeatherClock<.07f)return;WeatherClock=0;
 if(Flash>0){int64 Strike=int64(Hours*240./23.);if(Strike!=LastThunder){LastThunder=Strike;Sound(TEXT("Thunder"),P->GetActorLocation(),P->bIndoors?.2f:.6f);}}
 if(RainAmount>.01f&&!Sheltered){FVector Camera=P->Camera->GetComponentLocation();for(int I=0;I<128;I++){
 FRandomStream R(I*7919);FVector V=Camera+FVector(R.FRandRange(-1000,1000),R.FRandRange(-1000,1000),FMath::Fmod(R.FRandRange(0,1000)+float(Hours*240)*-1100,1000.f));if(V.Z<Camera.Z-100)V.Z+=1000;
 FHitResult Roof;FCollisionQueryParams Q(NAME_None,false,P);bool Covered=GetWorld()->LineTraceSingleByChannel(Roof,V,V+FVector(0,0,3000),ECC_Visibility,Q);
 Rain->UpdateInstanceTransform(I,FTransform(FRotator(12,0,0),V,FVector(.006f,.006f,Covered||I>RainAmount*128?0:.4f)),true,I==127,true);
 }}
}
