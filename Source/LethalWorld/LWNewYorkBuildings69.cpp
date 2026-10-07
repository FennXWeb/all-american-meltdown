#include "LWDestiny71.h"
#include "LWWorld.h"
#include "LWStreaming68.h"
#include "LWWorldTextComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"

namespace {
// One small construction job at a time. Full-scale landmarks do not become one blocking frame.
struct FBuilder69 {
 ALWChunk* C;ALWWorld* W;LWGen::FSite S;
 FVector At(FVector P)const{return FVector(S.Position,24)-C->GetActorLocation()+P;}
 void Box(FName M,FVector P,FVector Size,float Yaw=0,bool Solid=true)const{C->Box(W,M,At(P),Size,FRotator(0,Yaw,0),Solid);}
 void Mesh(FName M,FVector P,float Yaw=0,FVector Scale=FVector(1))const{C->Add(W,M,NAME_None,At(P),Scale,FRotator(0,Yaw,0));}
 void Label(const FString& Text,FVector P,float Yaw=-90,float Size=48)const{auto* T=NewObject<ULWWorldTextComponent>(C);T->SetupAttachment(C->GetRootComponent());T->SetRelativeLocation(At(P));T->SetRelativeRotation(FRotator(0,Yaw,0));T->SetText(FText::FromString(Text));T->SetHorizontalAlignment(EHTA_Center);T->SetWorldSize(Size);T->SetCullDistance(45000);T->RegisterComponent();}
 void Object(FName MeshName,FVector P,int Key,FName Use=NAME_None,float Yaw=0)const{
  const auto Self=*this;auto Work=[Self,MeshName,P,Key,Use,Yaw](){
   FName Id(*FString::Printf(TEXT("ny69_%u_%d"),Self.S.Id,Key));auto* O=Self.W->SpawnObject(Use.IsNone()?ELWObjectKind::Container:ELWObjectKind::Furniture,Id,FVector(Self.S.Position,24)+P,FRotator(0,Yaw,0));
   if(O){if(!Use.IsNone())O->SetFurniture(Use);O->Body->SetStaticMesh(Self.W->Mesh(MeshName));O->Body->SetRelativeRotation(FRotator::ZeroRotator);O->SetActorTickEnabled(false);O->Body->SetCullDistance(14000);if(Use.IsNone())Self.W->EnsureSiteContainer(Id,Self.S,O->GetActorLocation());Self.C->Residents.Add(O);}
  };if(C->Plan68)C->Plan68->Population.Add(Work);else Work();
 }
 void Light(FVector P)const{Mesh(TEXT("TubeLight65"),P);auto* L=NewObject<UPointLightComponent>(C);L->SetupAttachment(C->GetRootComponent());L->SetRelativeLocation(At(P-FVector(0,0,20)));L->SetIntensity(20000);L->SetAttenuationRadius(1700);L->SetCastShadows(false);L->SetMaxDrawDistance(4500);L->RegisterComponent();}
 void Queue(TFunction<void()> Job)const{if(C->Plan68)C->Plan68->Geometry.Add(MoveTemp(Job));else Job();}
 void Stairs(FVector Start,float Rise,int Steps,float Width=300,float Yaw=0)const{
  for(int I=0;I<Steps;I++){float H=(I+1)*Rise/Steps;Box(TEXT("Concrete"),Start+FRotator(0,Yaw,0).RotateVector(FVector(0,I*30,H-12)),FVector(Width,30,24),Yaw);}
 }
};
}
void ALWChunk::BuildingNewYork69(ALWWorld* W,const LWGen::FSite& S){
 if(S.Type==69){LWDestiny71::Build(this,W,S);return;}
 FBuilder69 B{this,W,S};MajorBounds+=FBox2D(S.Position-S.Size*.5-FVector2D(7000),S.Position+S.Size*.5+FVector2D(7000));
 B.Box(TEXT("Asphalt"),FVector(0,0,-10),FVector(S.Size,20));
 const bool Stadium=S.Type==68;
 // Lot islands and marked parking aisles surround both buildings. Keep the central approach open.
 for(int Bank:{-1,1})for(int Row=0;Row<5;Row++)B.Queue([B,Bank,Row,Stadium](){
  const float Y=Bank*((Stadium?18000:12000)+Row*1600);
  for(int I=-40;I<=40;I++){float X=I*600;if(FMath::Abs(X)<1400)continue;B.Box(TEXT("Lane"),FVector(X,Y,2),FVector(8,600,2),0,false);}
  for(int X:{-18000,18000}){B.Box(TEXT("Concrete"),FVector(X,Y+720,8),FVector(1400,260,16));B.C->StreetLight(B.W,B.At(FVector(X,Y+720,16)),FRotator::ZeroRotator);}
 });
 if(Stadium){
  // Regulation 120 x 53 1/3 yard playing surface; architecture is not map-scaled.
  B.Box(TEXT("CanadaGrass51"),FVector(0,0,5),FVector(10973,4877,10));
  for(int End:{-1,1}){B.Box(TEXT("Red"),FVector(End*5028,0,12),FVector(914,4877,4),0,false);B.Label(TEXT("BUFFALO"),FVector(End*5028,0,20),End>0?180:0,170);B.Box(TEXT("Steel"),FVector(End*5600,0,300),FVector(18,18,600));B.Box(TEXT("Lane"),FVector(End*5600,0,600),FVector(18,564,18));for(int Side:{-1,1})B.Box(TEXT("Lane"),FVector(End*5600,Side*282,850),FVector(18,18,500));}
  for(int I=-5;I<=5;I++){B.Box(TEXT("Lane"),FVector(I*914.4,0,12),FVector(8,4877,3),0,false);for(int Side:{-1,1})for(int J=0;J<4;J++)B.Box(TEXT("Lane"),FVector(I*914.4+J*182.88,Side*560,12),FVector(8,70,3),0,false);}
  constexpr int Sectors=64;
  for(int Sector=0;Sector<Sectors;Sector++)B.Queue([B,Sector](){
   const double A=2*PI*(Sector+.5)/64;const FVector Rad(FMath::Cos(A),FMath::Sin(A),0);const float Yaw=FMath::RadiansToDegrees(A)+90;
   const bool Aisle=Sector%8==0;const bool Tunnel=Sector%16==0;
   // Elliptical terraces with continuous ring concourses and eight radial stair aisles.
   for(int Row=0;Row<40;Row++){
    const bool Upper=Row>=22;const float D=Row*145+(Upper?1000:0);const FVector P(FMath::Cos(A)*(6900+D),FMath::Sin(A)*(3800+D),180+Row*60+(Upper?260:0));
    const float Width=2*PI*FMath::Sqrt(FMath::Square((6900+D)*FMath::Sin(A))+FMath::Square((3800+D)*FMath::Cos(A)))/64+30;
    if(Tunnel&&Row<22)continue;
    B.Box(TEXT("Concrete"),P-FVector(0,0,40),FVector(Width,170,80),Yaw);
    if(Aisle){for(int Step=0;Step<3;Step++)B.Box(TEXT("Concrete"),P-Rad*(50-Step*48)+FVector(0,0,-20+Step*20),FVector(Width,50,40),Yaw);continue;}
    // Repeated moulded seat rows are instanced; aisle openings remain clear.
    const int Seats=FMath::Max(2,int(Width/55));for(int Seat=0;Seat<Seats;Seat++){
     const FVector Tang(-Rad.Y,Rad.X,0);const FVector Q=P+Tang*((Seat+.5f)*Width/Seats-Width*.5f);
     B.Box(TEXT("Red"),Q+FVector(0,0,12),FVector(45,40,10),Yaw,false);B.Box(TEXT("Red"),Q+Rad*22+FVector(0,0,42),FVector(45,8,62),Yaw,false);
    }
   }
   const FVector Ring(FMath::Cos(A)*14500,FMath::Sin(A)*11400,1480);
   B.Box(TEXT("Concrete"),Ring,FVector(1500,1700,60),Yaw);B.Box(TEXT("Concrete"),FVector(Ring.X,Ring.Y,2750),FVector(1500,1700,60),Yaw);
   B.Box(TEXT("Steel"),FVector(Ring.X,Ring.Y,1800),FVector(75,75,3600));
   // Open roof canopy and translucent upper facade: the field remains outdoors.
   B.Box(TEXT("Steel"),FVector(FMath::Cos(A)*13000,FMath::Sin(A)*10500,3600),FVector(1600,3300,80),Yaw);
   if(Sector%8!=0)B.Box(TEXT("Glass"),FVector(Ring.X,Ring.Y,2800)+Rad*840,FVector(1500,16,1400),Yaw);
   if(Sector%4==1){B.Object(TEXT("Counter65"),Ring+FVector(0,0,32),1000+Sector);B.Mesh(TEXT("CashRegister53"),Ring+FVector(0,0,134));B.Label(TEXT("CONCESSIONS"),Ring+FVector(0,0,290),Yaw-90,35);B.Light(Ring+FVector(0,0,700));}
   if(Sector%8==0)B.Label(FString::Printf(TEXT("SECTION %d"),100+Sector),Ring+FVector(0,0,200),Yaw-90,50);
  });
  for(int Side:{-1,1})B.Queue([B,Side](){
   B.Box(TEXT("Steel"),FVector(Side*14700,0,3200),FVector(80,5000,1500));B.Box(TEXT("Red"),FVector(Side*14645,0,3200),FVector(8,4700,1250),0,false);B.Label(TEXT("HIGHMARK STADIUM"),FVector(Side*14590,0,3270),Side>0?180:0,135);
   // Exterior access stairs meet the concourse at 1,510 cm; wide field tunnels stay at grade.
   B.Stairs(FVector(Side*15600,-6000,0),1510,84,700);B.Stairs(FVector(Side*16350,-8350,0),2780,159,650);B.Box(TEXT("Concrete"),FVector(Side*15100,-3500,2780),FVector(3600,700,60));B.Box(TEXT("Concrete"),FVector(Side*14900,-3460,1510),FVector(2800,700,60));
   for(int Room=0;Room<6;Room++){const FVector P(Side*15700,3000+Room*1500,0);B.Box(TEXT("BrickV7"),P+FVector(Side*650,0,250),FVector(30,1450,500));B.Box(TEXT("Concrete"),P+FVector(0,0,500),FVector(1300,1450,30));for(int Edge:{-1,1})B.Box(TEXT("PlasterV7"),P+FVector(0,Edge*725,250),FVector(1300,24,500));B.Object(Room%2?TEXT("Locker65"):TEXT("Counter65"),P,2000+(Side>0?10:0)+Room);B.Light(P+FVector(0,0,480));B.Label(Room%2?TEXT("TEAM LOCKERS"):TEXT("GUEST SERVICES"),P+FVector(-Side*660,0,320),Side>0?180:0,28);}
  });
  B.Label(TEXT("HIGHMARK STADIUM / ORCHARD PARK"),FVector(0,-17200,800),-90,155);
 }

 // Reuse normal enemy/loot systems, but spawn on actual supported landmark floors.
 B.Queue([B,Stadium](){auto Combat=B.S;Combat.Type=Stadium?17:58;B.W->SpawnEnemies(B.C,&Combat,Stadium?FVector2D(10500,4500):FVector2D(44000,2200),1,550);});
}
