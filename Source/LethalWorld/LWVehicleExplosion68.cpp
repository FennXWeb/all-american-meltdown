#include "LWVehicleExplosion68.h"
#include "LWVehicle.h"
#include "LWWorld.h"
#include "ProceduralMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
ALWVehicleExplosion68::ALWVehicleExplosion68(){PrimaryActorTick.bCanEverTick=true;Plume=CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Plume"));SetRootComponent(Plume);Plume->SetCollisionEnabled(ECollisionEnabled::NoCollision);Plume->SetCanEverAffectNavigation(false);Plume->SetCastShadow(false);Flash=CreateDefaultSubobject<UPointLightComponent>(TEXT("Flash"));Flash->SetupAttachment(Plume);Flash->SetCastShadows(false);Flash->SetLightColor(FLinearColor(1,.31,.055));}
void ALWVehicleExplosion68::Spawn(ALWVehicle* V){if(V&&V->World)Burst(V->World,V->GetActorLocation()+FVector(0,0,85),V->IsAircraft84()?FMath::Clamp(V->Spec().HalfLength/320.f,3.f,6.f):FMath::Clamp(V->Spec().HalfWidth/95.f,.8f,1.8f),V->IsAircraft84()?80:35);}
ALWVehicleExplosion68* ALWVehicleExplosion68::Burst(ALWWorld* W,FVector Where,float Scale,float FireSeconds){if(!W)return nullptr;int Count=0;ALWVehicleExplosion68* Oldest=nullptr;for(TActorIterator<ALWVehicleExplosion68> E(W->GetWorld());E;++E){Count++;if(!Oldest||E->Age>Oldest->Age)Oldest=*E;}if(Count>=18&&Oldest)Oldest->Destroy();auto* E=W->GetWorld()->SpawnActor<ALWVehicleExplosion68>(Where,FRotator::ZeroRotator);if(E){E->FireDuration84=FMath::Clamp(FireSeconds,0.f,90.f);E->Initialize(W,FMath::Clamp(Scale,.15f,6.f));}return E;}
void ALWVehicleExplosion68::Initialize(ALWWorld* W,float Scale){
 Plume->SetVisibleInRayTracing(false);
 FHitResult Surface;FCollisionQueryParams Q(NAME_None,false,this);FCollisionObjectQueryParams Types;Types.AddObjectTypesToQuery(ECC_WorldStatic);
 const FVector At=GetActorLocation();if(GetWorld()->LineTraceSingleByObjectType(Surface,At+FVector(0,0,20),At-FVector(0,0,650),Types,Q))FireZ84=Surface.ImpactPoint.Z-At.Z+8;
 else {const float Ground=W->HeightAt(FVector2D(At));if(At.Z-Ground>650)FireDuration84=0;else FireZ84=Ground-At.Z+8;}
 World84=W;Size=Scale;Plume->SetMaterial(0,W->Material(TEXT("Explosion84")));Flash->SetAttenuationRadius(2600*Size);Flash->SetIntensity(360000*Size);SetLifeSpan(FMath::Max(14.f,FireDuration84+5));
 FRandomStream R(GetUniqueID());for(int I=0;I<140;I++){
  int K=I<22?0:I<68?1:I<106?2:I<118?3:4;FVector D=R.VRand();D.Z=FMath::Abs(D.Z);
  FParticle P;P.Kind=K;P.P=R.VRand()*55*Size;P.Age=-R.FRandRange(0,K==1?1.2f:.15f);P.Spin=R.FRandRange(-PI,PI);
  P.V=D*R.FRandRange(K==2?800:80,K==2?2200:420)*Size;P.V.Z+=K==1?140:0;
  if(K==3){P.V.Z=0;P.V=P.V.GetSafeNormal2D()*R.FRandRange(450,1100)*Size;P.P.Z=-50;}
  P.Radius=(K==0?R.FRandRange(75,180):K==1?R.FRandRange(85,190):K==2?4:R.FRandRange(70,110))*Size;
  P.Life=K==0?R.FRandRange(.6f,1.7f):K==1?R.FRandRange(8,13):K==2?R.FRandRange(.6f,2.f):K==4?R.FRandRange(.7f,1.4f):1.7f;if(K==4){P.P=FVector(R.FRandRange(-150,150),R.FRandRange(-150,150),-65)*Size;P.P.Z=FireZ84;P.V=FVector(0,0,R.FRandRange(140,260))*Size;P.Radius=R.FRandRange(28,65)*Size;}Particles.Add(P);
 }
 Tick(0);
}
void ALWVehicleExplosion68::Tick(float Dt){
 Super::Tick(Dt);Age+=Dt;Flash->SetIntensity(360000*Size*FMath::Pow(FMath::Max(0.f,1-Age/.65f),3)+(Age<FireDuration84?18000*Size*(.8f+.2f*FMath::Sin(Age*17)):0));
 FireDamageClock84+=Dt;if(World84&&Age<FireDuration84&&Age>1&&FireDamageClock84>.5f){FireDamageClock84=0;TArray<FOverlapResult> Near;FCollisionObjectQueryParams Types;Types.AddObjectTypesToQuery(ECC_Pawn);FCollisionQueryParams Q(NAME_None,false,this);GetWorld()->OverlapMultiByObjectType(Near,GetActorLocation(),FQuat::Identity,Types,FCollisionShape::MakeSphere(190*Size),Q);TSet<AActor*> Burned;for(const auto& Hit:Near){AActor* A=Hit.GetActor();if(!A||Burned.Contains(A)||ALWWorld::IsSafePosition(A->GetActorLocation()))continue;Burned.Add(A);UGameplayStatics::ApplyDamage(A,5.f*FMath::Min(Size,2.f),nullptr,this,nullptr);}}
 auto* Camera=UGameplayStatics::GetPlayerCameraManager(this,0);if(!Camera)return;const FVector Right=Camera->GetCameraRotation().RotateVector(FVector::RightVector),Up=Camera->GetCameraRotation().RotateVector(FVector::UpVector);
 TArray<FVector> V,N;TArray<int32> Tri;TArray<FVector2D> UV;TArray<FLinearColor> Colors;
 for(auto& P:Particles){P.Age+=Dt;if(P.Kind==4&&P.Age>P.Life&&Age<FireDuration84){P.P.Z=FireZ84;P.Age=0;P.V.Z=200*Size;}const bool Live=P.Age>=0&&P.Age<=P.Life&&(P.Kind!=4||Age<FireDuration84);if(Live){P.P+=P.V*Dt;P.V*=FMath::Exp(-Dt*(P.Kind==2?.12f:P.Kind==4?0:1.1f));if(P.Kind==2)P.V.Z-=600*Dt;else if(P.Kind==1)P.V.Z+=75*Dt;}
  float T=FMath::Clamp(P.Age/P.Life,0.f,1.f),Alpha=Live?FMath::Min(1.f,P.Age*15)*(1-T):0;float Radius=Live?P.Radius*(1+T*(P.Kind==1?4:1.5f)):0;
  FLinearColor Color=P.Kind==0?FLinearColor::LerpUsingHSV(FLinearColor(10,3,.2),FLinearColor(.14,.025,.006),T):P.Kind==1?FLinearColor(.07,.065,.06):P.Kind==2?FLinearColor(15,4,.2):P.Kind==4?FLinearColor::LerpUsingHSV(FLinearColor(8,2,.08),FLinearColor(.6,.055,.001),T):FLinearColor(.22,.20,.15);Color.A=Alpha*(P.Kind==1?.7f:1.f);
  FVector R=(Right*FMath::Cos(P.Spin)+Up*FMath::Sin(P.Spin))*Radius,U=(Up*FMath::Cos(P.Spin)-Right*FMath::Sin(P.Spin))*Radius;if(P.Kind==2){R=Right*Radius;U=P.V.GetSafeNormal()*Radius*8;}
  if(P.Kind==4){R=Right*Radius;U=Up*Radius*2.2f;}int Base=V.Num();V.Append({P.P-R-U,P.P+R-U,P.P+R+U,P.P-R+U});UV.Append({FVector2D(0,1),FVector2D(1,1),FVector2D(1,0),FVector2D(0,0)});for(int J=0;J<4;J++){N.Add(-Camera->GetCameraRotation().Vector());Colors.Add(Color);}Tri.Append({Base,Base+1,Base+2,Base,Base+2,Base+3});
 }
 // Keep one fixed vertex/index allocation. Recreating a changing topology every
 // frame races render resource retirement during overlapping blasts/streaming.
 if(!Plume->GetProcMeshSection(0))Plume->CreateMeshSection_LinearColor(0,V,Tri,N,UV,Colors,TArray<FProcMeshTangent>(),false);
 else Plume->UpdateMeshSection_LinearColor(0,V,N,UV,Colors,TArray<FProcMeshTangent>());
}
