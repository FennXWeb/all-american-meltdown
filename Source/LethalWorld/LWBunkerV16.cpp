#include "LWWorld.h"
#include "LWWorldObject.h"
#include "Components/StaticMeshComponent.h"
#include "LWWorldTextComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
void ALWWorld::CreateBunkerV4(){
 const FVector C(-1700,1700,-2000);
 auto B=[&](FVector P,FVector Size,FName Mat){auto* A=GetWorld()->SpawnActor<AStaticMeshActor>(C+P,FRotator::ZeroRotator);A->SetMobility(EComponentMobility::Movable);auto* M=A->GetStaticMeshComponent();M->SetStaticMesh(Mesh(TEXT("Cube")));M->SetMaterial(0,Material(Mat));M->SetWorldScale3D(Size/100);M->SetCollisionProfileName(TEXT("BlockAll"));BunkerParts.Add(A);};
 auto Model=[&](FName Name,FVector P,float Yaw=0,FVector Scale=FVector(1)){if(Name==TEXT("BookcaseV13")||Name==TEXT("PantryV13")||Name==TEXT("SideboardV13")||Name==TEXT("NightstandV13")){FName Id(*FString::Printf(TEXT("bunker_storage_%s_%d_%d"),*Name.ToString(),int(P.X),int(P.Y)));auto* O=SpawnObject(ELWObjectKind::Container,Id,C+P,FRotator(0,Yaw,0));O->Body->SetStaticMesh(Mesh(Name));O->Body->SetRelativeScale3D(Scale);BunkerParts.Add(O);return;}auto* A=GetWorld()->SpawnActor<AStaticMeshActor>(C+P,FRotator(0,Yaw,0));A->SetMobility(EComponentMobility::Movable);auto* M=A->GetStaticMeshComponent();M->SetStaticMesh(Mesh(Name));M->SetCollisionProfileName(TEXT("BlockAll"));M->SetWorldScale3D(Scale);BunkerParts.Add(A);};
 auto Furn=[&](FName Type,FVector P,int I,float Yaw=0,FName Override=NAME_None){FName Id(*FString::Printf(TEXT("bunker_%s_%d"),*Type.ToString(),I));if(Type==TEXT("locker")&&!Containers.Contains(Id)){FLWContainerRecord R;R.Id=Id;R.Context=TEXT("bunker");R.Position=C+P;Containers.Add(Id,R);}auto* F=SpawnObject(ELWObjectKind::Furniture,Id,C+P,FRotator(0,Yaw,0));F->SetFurniture(Type);if(!Override.IsNone())F->Body->SetStaticMesh(Mesh(Override));BunkerParts.Add(F);};
 auto Storage=[&](FName OldMesh,FVector OldPosition,FVector P,FName NewMesh,float Yaw=0){FName Id(*FString::Printf(TEXT("bunker_storage_%s_%d_%d"),*OldMesh.ToString(),int(OldPosition.X),int(OldPosition.Y)));auto* O=SpawnObject(ELWObjectKind::Container,Id,C+P,FRotator(0,Yaw,0));O->Body->SetStaticMesh(Mesh(NewMesh));BunkerParts.Add(O);};
 auto Label=[&](FString Name,FVector P,float Yaw){auto* A=GetWorld()->SpawnActor<AActor>(C+P,FRotator(0,Yaw,0));auto* T=NewObject<ULWWorldTextComponent>(A);A->SetRootComponent(T);T->SetWorldLocation(C+P);T->SetWorldRotation(FRotator(0,Yaw,0));T->SetText(FText::FromString(Name));T->SetWorldSize(18);T->SetHorizontalAlignment(EHTA_Center);T->SetTextRenderColor(FColor(207,199,161));T->RegisterComponent();BunkerParts.Add(A);};
 auto Lamp=[&](FVector P,bool Warm=true){Model(TEXT("CeilingLightV13"),P,0);auto* A=GetWorld()->SpawnActor<AActor>(C+P-FVector(0,0,12),FRotator::ZeroRotator);auto* L=NewObject<UPointLightComponent>(A);A->SetRootComponent(L);L->SetWorldLocation(C+P-FVector(0,0,12));L->SetIntensity(Warm?18000:20000);L->SetAttenuationRadius(730);L->SetCastShadows(false);L->SetLightColor(Warm?FLinearColor(1,.81,.58):FLinearColor(.67,.80,.85));L->RegisterComponent();BunkerParts.Add(A);};
 auto DoorWall=[&](float Mid,float Y,float Width,FName Id){for(int S:{-1,1})B(FVector(Mid+S*(Width/4+40),Y,180),FVector(Width/2-80,18,360),TEXT("Concrete"));B(FVector(Mid,Y,310),FVector(160,18,100),TEXT("Concrete"));B(FVector(Mid,Y,22),FVector(160,22,4),TEXT("Steel"));BunkerParts.Add(SpawnObject(ELWObjectKind::Door,Id,C+FVector(Mid+(Y<0?80:-80),Y,24),FRotator(0,Y<0?180:0,0)));};
 B(FVector(-200,0,0),FVector(3440,3140,40),TEXT("Concrete"));B(FVector(1900,0,0),FVector(40,3140,40),TEXT("Concrete"));for(int S:{-1,1})B(FVector(1700,S*870,0),FVector(360,1400,40),TEXT("Concrete"));B(FVector(0,0,370),FVector(3840,3140,30),TEXT("Steel"));
 B(FVector(1900,0,185),FVector(30,3140,370),TEXT("Concrete"));
 // Exact hatch recess: casing front is at X=-1885, the inner wall face.
 for(int S:{-1,1}){B(FVector(-1900,S*835,185),FVector(30,1470,370),TEXT("Concrete"));B(FVector(0,S*1550,185),FVector(3800,30,370),TEXT("Concrete"));}B(FVector(-1900,0,323),FVector(30,200,94),TEXT("Concrete"));
 for(int I=0;I<10;I++){
  const int Side=I<5?-1:1;const float X=-1200+(I%5)*600;
  DoorWall(X,Side*250,600,FName(*FString::Printf(TEXT("bedroom_door_%d"),I)));
  B(FVector(X,Side*1050,180),FVector(600,20,360),TEXT("Concrete"));
  for(int E:{-1,1}){B(FVector(X+E*300,Side*650,180),FVector(18,800,360),TEXT("Concrete"));B(FVector(X+E*287,Side*650,38),FVector(8,780,28),TEXT("Wood"));}
  Furn(TEXT("bed"),FVector(X-150,Side*690,24),I,90,TEXT("HomeBedV13"));
  Furn(TEXT("locker"),FVector(X+230,Side*925,24),I,Side>0?0:180);
  Storage(TEXT("Desk"),FVector(X+100,Side*970,24),FVector(X+100,Side*950,24),TEXT("SideboardV13"),Side>0?0:180);
  Storage(TEXT("Crate"),FVector(X-190,Side*945,24),FVector(X-240,Side*915,24),TEXT("NightstandV13"),Side>0?0:180);
  Model(TEXT("TableLampV13"),FVector(X-240,Side*915,85));
  Model(TEXT("DiningTableV13"),FVector(X+155,Side*505,24),90,FVector(.65,.75,1));Furn(TEXT("chair"),FVector(X+35,Side*505,24),I,0);
  Model(TEXT("DeskSetV13"),FVector(X+155,Side*505,102),90);
  Model(TEXT("BookcaseV13"),FVector(X+235,Side*740,24),90);
  Model(TEXT("WasteBinV13"),FVector(X+240,Side*350,24));
  Model(TEXT("LandscapeV13"),FVector(X-80,Side*1031,214),Side>0?0:180);
  B(FVector(X-80,Side*670,22),FVector(260,350,3),I%3==0?TEXT("Red"):I%3==1?TEXT("Cloth"):TEXT("Rubber"));
  Lamp(FVector(X,Side*650,335));Label(FString::Printf(TEXT("%02d"),I+1),FVector(X,Side*230,282),Side<0?90:-90);
 }
 // Four full service rooms reached through the side corridors.
 for(int S:{-1,1}){B(FVector(0,S*1300,180),FVector(20,500,360),TEXT("Concrete"));for(int X:{-1650,1650}){DoorWall(float(X),S*1050,480,FName(*FString::Printf(TEXT("utility_%d_%d"),S,X)));Lamp(FVector(X,S*650,335),false);}for(int X:{-1250,-500,500,1250})Lamp(FVector(X,S*1300,335),S<0);}
 // Mess: complete galley against rear wall, facing two dining settings.
 Furn(TEXT("cooker"),FVector(-1350,-1450,24),0,180);Furn(TEXT("sink"),FVector(-1160,-1450,24),1,180);Furn(TEXT("water"),FVector(-910,-1450,24),1,180);Model(TEXT("PantryV13"),FVector(-700,-1460,24),180);
 for(int I=0;I<2;I++){float X=-1330+I*830;Model(TEXT("DiningTableV13"),FVector(X,-1235,24));for(int S:{-1,1}){Furn(TEXT("chair"),FVector(X,-1235+S*100,24),12+I*2+(S>0),S<0?90:-90);Model(TEXT("TableSettingV13"),FVector(X,-1235+S*28,103),S<0?180:0);}}
 Storage(TEXT("Desk"),FVector(-350,-1320,24),FVector(-250,-1440,24),TEXT("SideboardV13"),180);
 // Workshop: dirty plant bay, organised workbench, storage and communications.
 Furn(TEXT("workbench"),FVector(-900,1430,24),0);Furn(TEXT("locker"),FVector(-400,1450,24),11);Model(TEXT("Generator"),FVector(-1370,1380,24));Model(TEXT("FuseBoxV13"),FVector(-1370,1520,205));
 Furn(TEXT("radio"),FVector(-260,1180,105),0);Model(TEXT("DiningTableV13"),FVector(-260,1200,24));Furn(TEXT("chair"),FVector(-260,1320,24),11,-90);Model(TEXT("DeskSetV13"),FVector(-320,1200,103));
 // Infirmary: bed, bedside supplies, washable fixtures and a screened washroom.
 Furn(TEXT("bed"),FVector(500,1320,24),11,0,TEXT("HomeBedV13"));Furn(TEXT("sink"),FVector(1100,1440,24),0);Furn(TEXT("water"),FVector(1470,1430,24),0);
 Storage(TEXT("CabinetV3"),FVector(300,1440,24),FVector(270,1420,24),TEXT("SideboardV13"));Model(TEXT("NightstandV13"),FVector(740,1410,24));Model(TEXT("TableLampV13"),FVector(740,1410,85));Model(TEXT("WasteBinV13"),FVector(1110,1190,24));
 // Wall-mounted storage, service fittings and work surfaces keep the aisles clear.
 for(int Side:{-1,1})for(int X:{-1200,-400,400,1200}){
 B(FVector(X,Side*1527,295),FVector(230,12,60),TEXT("Steel"));
 for(int V=-90;V<=90;V+=30)B(FVector(X+V,Side*1518,295),FVector(9,3,42),TEXT("Rubber"));
 }
 // Kitchen backsplash and extractor; pantry supplies stay on shelves.
 B(FVector(-1250,-1522,165),FVector(440,8,115),TEXT("Steel"));B(FVector(-1350,-1450,258),FVector(130,75,28),TEXT("Steel"));B(FVector(-1350,-1490,303),FVector(55,38,64),TEXT("Steel"));
 for(int I=0;I<5;I++){B(FVector(-670+I*65,-1504,202),FVector(52,44,8),TEXT("Wood"));for(int J=0;J<3;J++)B(FVector(-690+I*65+J*15,-1498,220),FVector(10,14,28),J%2?TEXT("Bone"):TEXT("Cloth"));}
 Model(TEXT("LandscapeV13"),FVector(-500,-1070,215),180);Model(TEXT("WasteBinV13"),FVector(-1800,-1400,24));
 // Infirmary shelving, supply drawers, a privacy screen and wall first-aid sign.
 Model(TEXT("PantryV13"),FVector(930,1450,24));Model(TEXT("SideboardV13"),FVector(1170,1110,24),180);
 for(int I=0;I<5;I++)B(FVector(1090+I*36,1120,118),FVector(25,30,24),I%2?TEXT("Bone"):TEXT("Cloth"));
 B(FVector(780,1260,130),FVector(10,150,210),TEXT("Cloth"));for(int S:{-1,1})B(FVector(780,1260+S*80,135),FVector(7,7,225),TEXT("Steel"));
 B(FVector(1200,1520,215),FVector(76,10,76),TEXT("Bone"));B(FVector(1200,1513,215),FVector(14,4,54),TEXT("Red"));B(FVector(1200,1513,215),FVector(54,4,14),TEXT("Red"));
 // Workshop toolboard, ducting and grounded materials rack.
 B(FVector(-900,1520,185),FVector(250,10,95),TEXT("Wood"));for(int I=0;I<7;I++){B(FVector(-1000+I*32,1511,188),FVector(8,6,40),TEXT("Steel"));B(FVector(-1000+I*32,1508,210),FVector(23,9,10),TEXT("Steel"));}
 Model(TEXT("BookcaseV13"),FVector(-520,1110,24),180);B(FVector(-1370,1460,283),FVector(80,80,115),TEXT("Steel"));
 B(FVector(-900,1260,22),FVector(350,210,3),TEXT("Rubber"));B(FVector(500,1260,22),FVector(320,320,3),TEXT("Cloth"));
 // Secure storeroom replaces the old corridor box. First locker opens the original stash.
 auto* Stash=SpawnObject(ELWObjectKind::Stash,TEXT("stash"),C+FVector(230,-1460,24),FRotator(0,180,0));BunkerParts.Add(Stash);
 for(int I=0;I<13;I++)Furn(TEXT("locker"),FVector(330+I*100,-1460,24),20+I,180);
 Model(TEXT("WorkbenchV4"),FVector(600,-1150,24),180);Model(TEXT("DeskSetV13"),FVector(600,-1150,114),180);Model(TEXT("BookcaseV13"),FVector(1050,-1130,24),180);Model(TEXT("WasteBinV13"),FVector(1450,-1180,24));
 for(int Side:{-1,1}){B(FVector(0,Side*215,333),FVector(3700,10,10),TEXT("Steel"));for(int X=-1500;X<=1500;X+=300)B(FVector(X,Side*220,335),FVector(8,45,8),TEXT("Rust"));}
 for(int X=-1200;X<=1200;X+=600)Lamp(FVector(X,0,335),false);
 Model(TEXT("FuseBoxV13"),FVector(1870,130,180),90);Model(TEXT("LandscapeV13"),FVector(1880,-110,205),90);
 Label(TEXT("WORKSHOP / COMMS"),FVector(-1100,1080,285),90);Label(TEXT("INFIRMARY"),FVector(900,1080,285),90);Label(TEXT("MESS"),FVector(-900,-1080,285),-90);Label(TEXT("STORAGE"),FVector(900,-1080,285),-90);
}

void ALWWorld::RestoreBunkerContainers(){
 for(AActor* A:BunkerParts)if(auto* O=Cast<ALWWorldObject>(A)){
 if(O->Kind==ELWObjectKind::Furniture)O->SetFurniture(O->UseType);
 if(O->Kind==ELWObjectKind::Container&&!Containers.Contains(O->RecordId)){FLWContainerRecord R;R.Id=O->RecordId;R.Context=TEXT("bunker");R.Position=O->GetActorLocation();Containers.Add(R.Id,R);}
 }
}
