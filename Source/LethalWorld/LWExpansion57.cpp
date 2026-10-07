#include "LWInteriors65.h"
#include "LWExpansion57.h"
#include "LWWorld.h"
#include "LWSiteIdentity.h"
#include "LWVehicle.h"
#include "LWWorldObject.h"
#include "LWWorldTextComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"
namespace {
struct FBuilder57{
 ALWChunk* C;ALWWorld* W;const LWGen::FSite& S;int Serial=0;
 FVector At(FVector V)const{return FVector(S.Position,12)+FRotator(0,S.Yaw,0).RotateVector(V);}
 void Box(FName M,FVector V,FVector Size,float Yaw=0,bool Hit=true){C->Box(W,M,At(V)-C->GetActorLocation(),Size,FRotator(0,S.Yaw+Yaw,0),Hit);}
 void Model(FName M,FVector V,float Yaw=0){C->Add(W,M,NAME_None,At(V)-C->GetActorLocation(),FVector(1),FRotator(0,S.Yaw+Yaw,0));}
 void Label(FString T,FVector V,float Size=35){auto* L=NewObject<ULWWorldTextComponent>(C);L->SetupAttachment(C->GetRootComponent());L->SetRelativeLocation(At(V)-C->GetActorLocation());L->SetRelativeRotation(FRotator(0,S.Yaw-90,0));L->SetText(FText::FromString(T));L->SetWorldSize(Size);L->SetHorizontalAlignment(EHTA_Center);L->SetTextRenderColor(FColor(220,210,170));L->SetCullDistance(14000);L->RegisterComponent();}
 ALWWorldObject* Object(ELWObjectKind Kind,FVector V,float Yaw=0){auto* O=W->SpawnObject(Kind,FName(*FString::Printf(TEXT("exp57_%u_%d"),S.Id,Serial++)),At(V),FRotator(0,S.Yaw+Yaw,0));if(O)C->Residents.Add(O);return O;}
 void Loot(FName Mesh,FVector V,int Lock=0,float Yaw=0){auto* O=Object(ELWObjectKind::Container,V,Yaw);if(O){O->Body->SetStaticMesh(W->Mesh(Mesh));W->EnsureSiteContainer(O->RecordId,S,At(V),Lock);}}
 void Furn(FName M,FVector V,float Yaw=0){if(auto* O=Object(ELWObjectKind::Furniture,V,Yaw))O->SetFurniture(M);}
 void Light(FVector V){Model(TEXT("CeilingLightV13"),V);auto* L=NewObject<UPointLightComponent>(C);L->SetupAttachment(C->GetRootComponent());L->SetRelativeLocation(At(V-FVector(0,0,20))-C->GetActorLocation());L->SetIntensity(22000);L->SetAttenuationRadius(1100);L->SetCastShadows(false);L->SetMaxDrawDistance(4500);L->RegisterComponent();}
 void Wall(FVector V,float Length,bool Door=false,float Yaw=0,float H=360){auto Segment=[&](float X,float N,float Z,float Height){Box(TEXT("Concrete"),V+FRotator(0,Yaw,0).RotateVector(FVector(X,0,Z)),FVector(N,20,Height),Yaw);};if(!Door)Segment(0,Length,H*.5f,H);else{for(int Sign:{-1,1})Segment(Sign*(Length+140)*.25f,(Length-140)*.5f,H*.5f,H);Segment(0,140,(H+240)*.5f,H-240);}}
 void Room(FVector V,float X,float Y,FString Name){Box(TEXT("TileV7"),V+FVector(0,0,12),FVector(X*2,Y*2,24));Wall(V+FVector(0,-Y,24),X*2,true);Wall(V+FVector(0,Y,24),X*2);for(int Sign:{-1,1})Wall(V+FVector(Sign*X,0,24),Y*2,false,90);Box(TEXT("CorrugatedV7"),V+FVector(0,0,398),FVector(X*2+20,Y*2+20,28));Label(Name,V+FVector(0,-Y-16,310));Light(V+FVector(0,0,360));}
 void Vehicle(FName Model,FVector V,int Index){FName Id(*FString::Printf(TEXT("exp57_car_%u_%d"),S.Id,Index));if(!W->Vehicles.Contains(Id)){FLWVehicleRecord R;R.Model=Model;R.VIN=FGuid::NewDeterministicGuid(Id.ToString(),uint64(uint32(W->Seed)));R.Position=At(V);R.Position.Z=W->HeightAt(FVector2D(R.Position))+75;FHitResult Support;if(C->GetWorld()->LineTraceSingleByChannel(Support,R.Position+FVector(0,0,800),R.Position-FVector(0,0,800),ECC_WorldStatic))R.Position.Z=Support.ImpactPoint.Z+75;R.Rotation=FRotator(0,S.Yaw,0);R.LockTier=Model==TEXT("helicopter")?3:2;R.Unlocked=false;W->Vehicles.Add(Id,R);}if(auto* O=W->SpawnObject(ELWObjectKind::Car,Id,W->Vehicles[Id].Position,W->Vehicles[Id].Rotation))C->Residents.Add(O);}
 void Pad(FVector V){Box(TEXT("Concrete"),V+FVector(0,0,3),FVector(2200,2200,12));for(int Sign:{-1,1})Box(TEXT("Lane"),V+FVector(Sign*220,0,10),FVector(38,680,2),0,false);Box(TEXT("Lane"),V+FVector(0,0,10),FVector(440,38,2),0,false);for(int Sign:{-1,1})for(int Other:{-1,1})Model(TEXT("CeilingLightV13"),V+FVector(Sign*1000,Other*1000,18));}
};
}
void LWExpansion57::Airport(ALWChunk* C,ALWWorld* W,const LWGen::FSite& S){FBuilder57 B{C,W,S};B.Pad(FVector(11000,0,0));B.Label(TEXT("ROTOR OPERATIONS / KEEP CLEAR"),FVector(11000,-1200,140),48);B.Vehicle(TEXT("helicopter"),FVector(11000,0,75),90);B.Loot(TEXT("Crate"),FVector(12400,-1000,15),2);B.Loot(TEXT("CabinetV3"),FVector(12400,-800,15),1);}
void LWExpansion57::Build(ALWChunk* C,ALWWorld* W,const LWGen::FSite& S){
 LWInteriors65::FScope Interior65(C,W,S);
 FBuilder57 B{C,W,S};const float X=S.Size.X*.5f,Y=S.Size.Y*.5f;B.Box(TEXT("Asphalt"),FVector(0,0,-12),FVector(X*2,Y*2,24));B.Label(LWSites::Label(S),FVector(0,-Y+80,450),60);
 if(S.Type==64){
  // A secure campus: checkpoint -> motor pool -> barracks -> command and armory.
  for(int Side:{-1,1}){B.Wall(FVector(Side*X,0,0),2*Y,false,90,300);B.Wall(FVector(Side*(X+500)*.5f,-Y,0),X-500,false,0,300);}B.Wall(FVector(0,Y,0),2*X,false,0,300);
  B.Room(FVector(-1100,-Y+650,0),450,450,TEXT("GATE CONTROL"));B.Furn(TEXT("Desk"),FVector(-1100,-Y+800,24));B.Loot(TEXT("GunRack57"),FVector(-1350,-Y+450,24),2);
  B.Room(FVector(-3100,-1500,0),1500,900,TEXT("BARRACKS 01"));for(int I=0;I<5;I++){B.Furn(TEXT("bed"),FVector(-4250+I*550,-1200,24),90);B.Loot(TEXT("LockerV4"),FVector(-4250+I*550,-2100,24),I%3);}
  B.Room(FVector(-3100,1600,0),1500,1100,TEXT("OPERATIONS / COMMUNICATIONS"));for(int I=0;I<4;I++)B.Furn(TEXT("Desk"),FVector(-4100+I*600,1900,24));B.Wall(FVector(-3100,2100,24),2800,true);B.Loot(TEXT("CabinetV3"),FVector(-4000,2400,24),3);
  B.Room(FVector(2700,2200,0),1900,1000,TEXT("ORDNANCE / ARMORY"));for(int I=0;I<6;I++)B.Loot(TEXT("GunRack57"),FVector(1300+I*520,2700,24),2+I%2);for(int I=0;I<4;I++)B.Loot(TEXT("Crate"),FVector(1500+I*700,1600,24),2);
  B.Pad(FVector(2900,-1900,0));B.Vehicle(TEXT("helicopter"),FVector(2900,-1900,75),0);B.Vehicle(TEXT("apc"),FVector(-1700,-3700,75),1);B.Vehicle(TEXT("technical"),FVector(-3700,-3700,75),2);
  for(int I=0;I<5;I++){B.Box(TEXT("Lane"),FVector(-4500+I*800,-3300,2),FVector(12,1400,2),0,false);B.Model(TEXT("RangeTarget57"),FVector(-4300+I*1800,4200,0));}
 }else if(S.Type==65){
  B.Room(FVector(0,250,0),X-300,Y-850,TEXT("AMERICAN RESERVE / BANK"));const float Front=-Y+1100;
  B.Wall(FVector(0,950,24),X*2-650,true);B.Label(TEXT("TELLER SERVICES"),FVector(0,300,300));for(int I=-2;I<=2;I++){B.Model(TEXT("Teller57"),FVector(I*280,550,24));B.Loot(TEXT("CabinetV3"),FVector(I*280,780,24),1);}
  for(int I=0;I<3;I++){B.Model(TEXT("ATM57"),FVector(-X+500,Front+I*260,24),90);B.Furn(TEXT("chair"),FVector(X-600,Front+I*220,24),180);}
  B.Room(FVector(0,1900,0),600,600,TEXT("SECURE DEPOSIT"));if(auto* Door=B.Object(ELWObjectKind::Door,FVector(-65,1290,24),90)){Door->Body->SetStaticMesh(W->Mesh(TEXT("VaultDoor57")));Door->Body->SetRelativeScale3D(FVector(1,.5f,.92f));Door->Body->SetRelativeLocation(FVector(0,65,0));Door->UseType=TEXT("vault57");}for(int I=-1;I<=1;I++)B.Loot(TEXT("LockerV4"),FVector(I*270,2230,24),4);B.Vehicle(TEXT("armoredtruck"),FVector(-X+700,-Y+500,75),0);
 }else if(S.Type==66){
  B.Room(FVector(0,0,0),X-300,Y-500,TEXT("RICHARDSON SHOOTING CLUB"));B.Wall(FVector(0,-Y+1450,24),X*2-650,true);B.Model(TEXT("Teller57"),FVector(-X+1000,-Y+1100,24));B.Loot(TEXT("GunRack57"),FVector(-X+650,-Y+750,24),2);
  for(int I=0;I<7;I++){const float Lane=-X+750+I*450;B.Model(TEXT("RangeTarget57"),FVector(Lane,Y-850,24));B.Box(TEXT("Wood"),FVector(Lane,-Y+2100,115),FVector(350,100,18));B.Wall(FVector(Lane+220,-Y+2050,24),650,false,90,200);B.Label(FString::Printf(TEXT("LANE %02d"),I+1),FVector(Lane,-Y+2000,260),22);B.Loot(TEXT("Crate"),FVector(Lane,-Y+1800,24),1);}
 }else{
  B.Room(FVector(0,0,0),X-250,Y-550,TEXT("TRAILBOUND SPORTING GOODS"));B.Label(TEXT("CAMPING / HUNTING / OUTDOORS"),FVector(0,-Y+520,325),30);
  for(int A=-1;A<=1;A++)for(int I=0;I<4;I++)B.Loot(TEXT("SportsRack57"),FVector(A*850,-900+I*520,24),0,90);
  B.Model(TEXT("Teller57"),FVector(X-750,-Y+900,24));B.Model(TEXT("ATM57"),FVector(X-650,-Y+700,24));B.Wall(FVector(0,Y-1200,24),2*X-550,true);
  for(int I=-2;I<=2;I++)B.Loot(I%2?TEXT("Crate"):TEXT("GunRack57"),FVector(I*600,Y-900,24),I%2?0:2);B.Label(TEXT("STOCKROOM / AUTHORIZED STAFF"),FVector(0,Y-1220,300),26);
 }
 W->SpawnEnemies(C,&S,S.Size-FVector2D(700,1000),1,400);
}
