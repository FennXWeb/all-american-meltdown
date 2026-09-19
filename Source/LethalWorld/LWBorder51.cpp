#include "LWBorder51.h"
#include "LWBorderGuard51.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWVehicle.h"
#include "Components/TextRenderComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ProceduralMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
void LWBorder51::Build(ALWChunk* C,ALWWorld* W){
 FVector O=C->GetActorLocation();FRandomStream R(LWGen::Hash(C->Coordinate.X,C->Coordinate.Y,W->Seed,5100));
 if(O.X>=North){C->Terrain->SetMaterial(0,W->Material(TEXT("CanadaGrass51")));for(int I=0;I<65;++I){FVector P(R.FRandRange(200,12600),R.FRandRange(100,12700),0);if(O.X+P.X<North+2200)continue;TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;P.Z=LWGen::Height(FVector2D(P+O),Roads,Sites);C->Add(W,TEXT("LivingPine51"),NAME_None,P,FVector(R.FRandRange(.8f,1.65f)),FRotator(0,R.FRand()*360,0),false);}return;}
 const float BX=North-O.X;
 // High steel anti-climb fence is transparent between uprights, preserving the green vista.
 for(int Y=0;Y<12800;Y+=400){C->Box(W,TEXT("Concrete"),FVector(BX-90,Y+200,75),FVector(180,400,150));C->Box(W,TEXT("Steel"),FVector(BX-20,Y+200,490),FVector(35,35,820));for(int J=0;J<8;++J)C->Box(W,TEXT("Steel"),FVector(BX-20,Y+J*50,465),FVector(12,8,640));for(int Z:{200,440,760})C->Box(W,TEXT("Steel"),FVector(BX-20,Y+200,Z),FVector(18,400,12));C->Box(W,TEXT("CanadaRed51"),FVector(BX-1780,Y+200,5),FVector(22,395,6),FRotator::ZeroRotator,false);}
 // Invisible collision follows the visible fence rather than a giant opaque wall.
 auto* Wall=NewObject<UStaticMeshComponent>(C);Wall->SetupAttachment(C->GetRootComponent());Wall->SetStaticMesh(W->Mesh(TEXT("Cube")));Wall->SetRelativeLocation(FVector(BX-20,6400,500));Wall->SetRelativeScale3D(FVector(.35,128,10));Wall->SetVisibility(false);Wall->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);Wall->SetCollisionResponseToAllChannels(ECR_Block);Wall->RegisterComponent();C->AddInstanceComponent(Wall);
 float GateY=6400;
 C->Box(W,TEXT("Asphalt"),FVector(BX-7350,GateY,10),FVector(10900,900,12));
 for(int X=0;X<10500;X+=600)C->Box(W,TEXT("CanadaArmor51"),FVector(X+200,GateY,17),FVector(300,12,2),FRotator::ZeroRotator,false);
 // Sheltered observation booth beside the sealed checkpoint, with clear windows.
 FVector Booth(BX-650,GateY-2200,0);
 C->Box(W,TEXT("Concrete"),Booth+FVector(0,0,20),FVector(620,760,40));
 C->Box(W,TEXT("CanadaArmor51"),Booth+FVector(0,0,385),FVector(660,800,35));
 for(int Side:{-1,1}){C->Box(W,TEXT("CanadaArmor51"),Booth+FVector(Side*300,0,95),FVector(20,740,150));C->Box(W,TEXT("Glass"),Booth+FVector(Side*300,0,250),FVector(12,740,160));for(int Y:{-370,370})C->Box(W,TEXT("Steel"),Booth+FVector(Side*300,Y,220),FVector(30,30,310));}
 C->Box(W,TEXT("CanadaRed51"),Booth+FVector(0,-370,200),FVector(600,24,340));
 
 C->Box(W,TEXT("Concrete"),FVector(BX-760,GateY,12),FVector(1800,3600,24));
 for(int Side:{-1,1}){C->Box(W,TEXT("Concrete"),FVector(BX-500,GateY+Side*1400,180),FVector(750,200,360));C->Box(W,TEXT("Steel"),FVector(BX-400,GateY+Side*1180,660),FVector(45,45,1320));C->StreetLight(W,FVector(BX-2500,GateY+Side*1700,0),FRotator(0,Side*90,0),false);}
 C->Box(W,TEXT("CanadaRed51"),FVector(BX-410,GateY,1050),FVector(55,2450,280));
 auto Sign=[&](FVector P,const TCHAR* Text,float Size){auto* T=NewObject<UTextRenderComponent>(C);T->SetupAttachment(C->GetRootComponent());T->SetRelativeLocation(P);T->SetRelativeRotation(FRotator(0,180,0));T->SetText(FText::FromString(Text));T->SetWorldSize(Size);T->SetHorizontalAlignment(EHTA_Center);T->SetTextRenderColor(FColor(240,240,216));T->SetCollisionEnabled(ECollisionEnabled::NoCollision);T->RegisterComponent();C->AddInstanceComponent(T);};
 Sign(FVector(BX-443,GateY,1100),TEXT("CANADA  /  BORDER CLOSED"),75);Sign(FVector(BX-443,GateY,1000),TEXT("FRONTIERE FERMEE"),48);
 for(int Side:{-1,1}){C->Box(W,TEXT("Steel"),FVector(BX-1810,GateY+Side*1850,175),FVector(20,530,280));Sign(FVector(BX-1823,GateY+Side*1850,240),TEXT("NO ENTRY"),53);Sign(FVector(BX-1823,GateY+Side*1850,155),TEXT("DO NOT CROSS"),34);}
 // Deterministic guard identities and chunk ownership prevent duplicate posts when streaming.
 for(int I=0;I<3;++I){bool Mech=I==2;FVector Pos=O+FVector(BX-(Mech?750:1100),GateY+(I-1)*900,Mech?225:100);FActorSpawnParameters S;S.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;auto* G=W->GetWorld()->SpawnActor<ALWBorderGuard51>(Pos,FRotator(0,180,0),S);if(G){G->Setup51(Mech,LWGen::Hash(C->Coordinate.X,C->Coordinate.Y,W->Seed,5110+I));W->ZombieCount++;C->Residents.Add(G);}}
}
void LWBorder51::Enforce(ALWWorld* W,ALWCharacter* P){
 if(!P||P->bMenu||P->Health<=0)return;
 // Continuous backstop also covers streamed-out fence segments and very fast airborne cars.
 if(P->GetActorLocation().X>North-100){if(P->Vehicle){auto* V=P->Vehicle.Get();FVector L=V->GetActorLocation();L.X=North-1000;V->SetActorLocation(L,false);V->Speed=FMath::Min(0.f,V->Speed);}FVector L=P->GetActorLocation();L.X=North-250;P->SetActorLocation(L,false);P->GetCharacterMovement()->Velocity.X=FMath::Min(0.,P->GetCharacterMovement()->Velocity.X);}
}
