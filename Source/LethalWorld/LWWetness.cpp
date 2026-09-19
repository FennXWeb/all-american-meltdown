#include "LWWorld.h"
#include "LWVehicle.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

void ALWWorld::InitWetMaterials(){
 for(const TCHAR* Name:{TEXT("Asphalt"),TEXT("Concrete"),TEXT("Earth"),TEXT("PuddleV9")}){
 FName Source=FString(Name)==TEXT("PuddleV9")?FName(Name):FName(*(FString(TEXT("Wet"))+Name+TEXT("V9")));
 if(auto* Base=Material(Source)){auto* Dynamic=UMaterialInstanceDynamic::Create(Base,this);Materials.Add(Name,Dynamic);WetMaterials.Add(Dynamic);}
 }
}
void ALWWorld::UpdateWetness(float Dt){
 SurfaceWetness=FMath::Clamp(SurfaceWetness+Dt*(RainAmount>.05f?RainAmount*.012f:-.0018f),0.f,1.f);
 for(auto M:WetMaterials)if(M)M->SetScalarParameterValue(TEXT("WeatherWet"),SurfaceWetness);
}
void ALWVehicle::TickGlass(float Dt){
 FHitResult Roof;FCollisionQueryParams Q(NAME_None,false,this);FVector At=GetActorLocation()+FVector(0,0,Spec().Height);
 bool Covered=GetWorld()->LineTraceSingleByChannel(Roof,At,At+FVector(0,0,5000),ECC_Visibility,Q);
 float Rain=Covered?0:World->RainAmount;
 WindshieldWater=FMath::Clamp(WindshieldWater+Dt*(Rain*.16f-(WipersOn?1.2f:.007f)),0.f,1.f);
 BodyWater=FMath::Clamp(BodyWater+Dt*(Rain>.05f?Rain*.014f:-.002f),0.f,1.f);
 if(GlassWet){GlassWet->SetScalarParameterValue(TEXT("WeatherWet"),WindshieldWater);GlassWet->SetScalarParameterValue(TEXT("Wiping"),WipersOn?1.f:0.f);GlassWet->SetScalarParameterValue(TEXT("Sweep"),FMath::Sin(GetWorld()->GetTimeSeconds()*4));}
 for(auto M:PaintWet)if(M){M->SetScalarParameterValue(TEXT("WeatherWet"),BodyWater);M->SetScalarParameterValue(TEXT("Roughness"),FMath::Lerp(.38f,.1f,BodyWater));}
}
