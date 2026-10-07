#include "LWBorder51.h"
#include "LWGeography84.h"
#include "LWCanada68.h"
#include "LWBorderGuard51.h"
#include "LWStreaming68.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWVehicle.h"
#include "LWWorldTextComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
void LWGeography84::BuildBorder(ALWChunk* C,ALWWorld* W){
 const FVector Origin=C->GetActorLocation();
 if(Canada(FVector2D(Origin)+FVector2D(6400))){
  FRandomStream R(LWGen::Hash(C->Coordinate.X,C->Coordinate.Y,W->Seed,84051));TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;if(C->Plan68){Roads=C->Plan68->Roads;Sites=C->Plan68->Sites;}else LWGen::Gather(FVector2D(Origin)+FVector2D(6400),W->Seed,Roads,Sites);
  for(int I=0;I<70;I++){const FVector2D XY=FVector2D(Origin)+FVector2D(R.FRandRange(100,12700),R.FRandRange(100,12700));if(!Canada(XY)||LWNY69::WaterDepth(XY)>0||NearCheckpoint(XY,8000)||Roads.ContainsByPredicate([&](const auto& A){return LWGen::DistanceToSegment(XY,A)<A.Width*.5+550;})||Sites.ContainsByPredicate([&](const auto& S){return (XY-S.Position).Size()<S.Size.Size()*.5+800;}))continue;C->Add(W,TEXT("LivingPine51"),NAME_None,FVector(XY,LWGen::Height(XY,Roads,Sites))-Origin,FVector(R.FRandRange(.8f,1.4f)),FRotator(0,R.FRand()*360,0),false);}
 }
 for(int K=0;K<Checkpoints().Num();K++){const auto Post=Checkpoints()[K];if(LWGen::ChunkAt(Post.Position)!=C->Coordinate)continue;const FRotator Rotation(0,Post.Yaw,0);auto At=[&](FVector P){return FVector(Post.Position,0)-Origin+Rotation.RotateVector(P);};auto Box=[&](FName M,FVector P,FVector S){C->Box(W,M,At(P),S,Rotation);};
  // Real crossings occupy a bridge deck. The guarded apron is on the approach,
  // and the no-entry stripe lies behind the guards toward Canadian territory.
  Box(TEXT("Concrete"),FVector(0,0,-25),FVector(14500,3200,65));Box(TEXT("Asphalt"),FVector(0,0,12),FVector(14500,2200,14));
  for(int Side:{-1,1}){Box(TEXT("Steel"),FVector(0,Side*1450,70),FVector(14500,30,140));Box(TEXT("Concrete"),FVector(-1600,Side*1200,130),FVector(800,250,260));C->StreetLight(W,At(FVector(-2300,Side*1400,20)),Rotation,false);}
  Box(TEXT("CanadaRed51"),FVector(300,0,24),FVector(45,2200,6));
  const FVector Booth(-1800,-2100,30);Box(TEXT("Concrete"),Booth,FVector(900,950,60));Box(TEXT("CanadaArmor51"),Booth+FVector(0,0,380),FVector(950,1000,35));
  for(int Side:{-1,1}){Box(TEXT("CanadaArmor51"),Booth+FVector(Side*435,0,90),FVector(30,950,150));Box(TEXT("Glass"),Booth+FVector(Side*435,0,245),FVector(12,950,160));Box(TEXT("CanadaArmor51"),Booth+FVector(0,Side*460,185),FVector(850,30,370));}
  Box(TEXT("Steel"),FVector(-600,-1600,340),FVector(20,920,190));
  auto* Sign=NewObject<ULWWorldTextComponent>(C);Sign->SetupAttachment(C->GetRootComponent());Sign->SetRelativeLocation(At(FVector(-615,-1600,375)));Sign->SetRelativeRotation(Rotation+FRotator(0,180,0));Sign->SetText(FText::FromString(FString(Post.Name)+TEXT("\nCANADIAN CUSTOMS")));Sign->SetWorldSize(38);Sign->SetHorizontalAlignment(EHTA_Center);Sign->SetCollisionEnabled(ECollisionEnabled::NoCollision);Sign->RegisterComponent();C->AddInstanceComponent(Sign);
  for(int I=0;I<3;I++){auto Spawn=[C,W,Post,Rotation,K,I](){const bool Mech=I==2;const FVector Location=FVector(Post.Position,0)+Rotation.RotateVector(FVector(-1100,(I-1)*850,Mech?255:130));FActorSpawnParameters S;S.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;auto* G=W->GetWorld()->SpawnActor<ALWBorderGuard51>(Location,Rotation+FRotator(0,180,0),S);if(G){G->Setup51(Mech,8405100+K*10+I);C->Residents.Add(G);}};if(C->Plan68)C->Plan68->Population.Add(Spawn);else Spawn();}
 }
}
void LWBorder51::Build(ALWChunk* C,ALWWorld* W){LWGeography84::BuildBorder(C,W);}
void LWBorder51::Enforce(ALWWorld* W,ALWCharacter* P){
 if(!P||P->bMenu||P->Health<=0||LWCanada68::Authorized(P)||!LWGeography84::Restricted(FVector2D(P->GetActorLocation()))||(P->Vehicle&&(P->Vehicle->IsHelicopter57()||P->Vehicle->IsAircraft84())))return;
 const auto Edge=LWGeography84::BorderNearest(FVector2D(P->GetActorLocation()));FVector2D Safe=Edge;
 for(float A:{0.f,45.f,90.f,135.f,180.f,225.f,270.f,315.f}){auto Candidate=Edge+FVector2D(1400,0).GetRotated(A);if(!LWGeography84::Canada(Candidate)){Safe=Candidate;break;}}
 if(P->Vehicle){P->Vehicle->SetActorLocation(FVector(Safe,P->Vehicle->GetActorLocation().Z));P->Vehicle->Speed=P->Vehicle->Throttle=0;}
 P->SetActorLocation(FVector(Safe,P->GetActorLocation().Z));P->GetCharacterMovement()->StopMovementImmediately();P->Notify(TEXT("PASSPORT AND WEAPON CHECK REQUIRED AT CUSTOMS"),3);
}
