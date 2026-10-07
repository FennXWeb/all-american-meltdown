#include "LWCampaign76.h"
#include "LWCampaignProduction77.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWWeaponEffect.h"
#include "LWWorldTextComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void ALWCampaign76::BuildCarrier(){if(Production){Production->Director=this;Production->BuildCarrier();}}
void ALWCampaign76::RestoreWeather(){if(HasWeather&&World){if(World->WeatherOverride==10)World->WeatherOverride=PreviousWeather;HasWeather=false;}}
void ALWCampaign76::AfterEffect(){
 if(Production)Production->UpdateCarrier(0);
 // The names at the rear stretcher become people moving to the marked cover.
 if(State().Values.FindRef(TEXT("evacuate_group")))for(FName Id:{FName(TEXT("milo")),FName(TEXT("tomas")),FName(TEXT("ruth"))})if(auto* N=People.FindRef(Id).Get()){N->Mark=At(FVector((Id==TEXT("milo")?-1:1)*450,-250,92));N->MoveTime=90;}
}
void ALWCampaign76::TickStage(float Dt){
 const auto* S=LWCampaign76::Stage(State().Stage);if(!S)return;
 const bool Storm=S->Site==TEXT("tughill")||S->Id==TEXT("meridian_assault")||S->Id==TEXT("meridian_inside")||S->Site==TEXT("transport");
 if(Storm&&!HasWeather){PreviousWeather=World->WeatherOverride;HasWeather=true;World->WeatherOverride=10;}else if(!Storm)RestoreWeather();
 if(SceneOpen||Player->IsUIOpen())return;
 if(State().Values.FindRef(TEXT("evacuate_group"))&&!State().Values.FindRef(TEXT("group_at_cover"))){
  bool Ready=true;int Survivors=0;for(FName Id:{FName(TEXT("milo")),FName(TEXT("tomas")),FName(TEXT("ruth"))})if(auto* N=People.FindRef(Id).Get();N&&!N->bDead){Survivors++;if(N->MoveTime>0||FVector::Dist2D(N->GetActorLocation(),N->Mark)>180)Ready=false;}
  if(Ready){Effects({TEXT("group_at_cover"),Survivors?TEXT("rear_group_saved"):TEXT("civilian_losses+=2")});Player->Notify(Survivors?TEXT("The rear group has reached cover."):TEXT("The rear group was lost. The remaining crossing is open."),5);Refresh();}
 }
 if(!Carrier||S->Id!=TEXT("carrier_battle"))return;
 const bool Off=State().Values.FindRef(TEXT("carrier_feed_cut"))!=0,Trapped=State().Values.FindRef(TEXT("carrier_trapped"))!=0;
 if(!Trapped){PlatformClock+=Dt;Carrier->SetActorLocation(CarrierOrigin+LWCampaign76::Frame(S->Site).TransformVector(FVector(FMath::Sin(PlatformClock*.35)*140,0,0)),true);}
 if(Off)return;
 const FVector Muzzle=CarrierTurret->GetComponentLocation()+FVector(0,0,25);FVector Aim=Player->GetActorLocation()+FVector(0,0,35);
 CarrierTurret->SetWorldRotation(FRotator(0,(Aim-Muzzle).Rotation().Yaw,0));DefenseClock+=Dt;
 // Long, audible acquisition followed by a short inaccurate burst; walls and
 // freight crates block the shot, and the independent feed can be destroyed.
 if(DefenseClock>=2&&DefenseClock-Dt<2)World->Sound(TEXT("Click"),Muzzle,.7f);
 if(DefenseClock<2.6f||DefenseClock>3.3f){if(DefenseClock>4.8f)DefenseClock=0;return;}
 const int Shot=FMath::FloorToInt((DefenseClock-2.6f)/.24f),Previous=FMath::FloorToInt((DefenseClock-Dt-2.6f)/.24f);if(Shot==Previous)return;
 const FVector Dir=ALWWeaponEffect::EnemyAim(Muzzle,Aim,100,Player->GetVelocity().Size2D()+150,.5f);FHitResult Hit;FCollisionQueryParams Q(TEXT("CarrierDefense76"),false,Carrier);Q.AddIgnoredActor(this);
 GetWorld()->LineTraceSingleByChannel(Hit,Muzzle,Muzzle+Dir*5000,ECC_Visibility,Q);
 if(Hit.GetActor()==Player)UGameplayStatics::ApplyPointDamage(Player,9,Dir,Hit,nullptr,Carrier,nullptr);
 ALWWeaponEffect::Gunfire(Carrier,World,Muzzle,Hit.bBlockingHit?Hit.ImpactPoint:Muzzle+Dir*5000);
}
float ALWCampaignNode76::TakeDamage(float D,const FDamageEvent&,AController*,AActor*){
 if(!Campaign||D<=0||Campaign->SceneOpen||Action!=TEXT("carrier_feed"))return 0;
 float& Health=Campaign->State().Health.FindOrAdd(TEXT("carrier_feed"),100);Health-=D;
 if(Health<=0&&!Campaign->State().Values.FindRef(TEXT("carrier_feed_cut"))){Campaign->Effects({TEXT("carrier_feed_cut")});Campaign->Player->Notify(TEXT("Carrier defense power disabled."));Campaign->Refresh();}return D;
}
