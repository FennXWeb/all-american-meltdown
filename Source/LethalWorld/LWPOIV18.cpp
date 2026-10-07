#include "LWInteriors65.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "LWWorldTextComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"
void ALWChunk::BuildingV18(ALWWorld* W,const LWGen::FSite& S){
 LWInteriors65::FScope Interior65(this,W,S);
 const FVector Base=FVector(S.Position,12)-GetActorLocation();const FRotator Rot(0,S.Yaw,0);int Serial=0;FRandomStream Rand(S.Id);
 auto At=[&](FVector V){return Base+Rot.RotateVector(V);};
 auto BoxAt=[&](FName Mat,FVector V,FVector Size,float Yaw=0,bool Hit=true){Box(W,Mat,At(V),Size,Rot+FRotator(0,Yaw,0),Hit);};
 auto Model=[&](FName Mesh,FVector V,float Yaw=0,bool Collision=true){Add(W,Mesh,NAME_None,At(V),FVector(1),Rot+FRotator(0,Yaw,0),Collision);};
 auto Object=[&](ELWObjectKind Kind,FVector V,float Yaw=0){auto* O=W->SpawnObject(Kind,FName(*FString::Printf(TEXT("site_%u_v18_%d"),S.Id,Serial++)),GetActorLocation()+At(V),Rot+FRotator(0,Yaw,0));if(O)Residents.Add(O);return O;};
 auto Text=[&](const FString& Words,FVector V,float Yaw=-90,float Size=24){auto* L=NewObject<ULWWorldTextComponent>(this);L->SetupAttachment(RootComponent);L->SetRelativeLocation(At(V));L->SetRelativeRotation(Rot+FRotator(0,Yaw,0));L->SetText(FText::FromString(Words));L->SetHorizontalAlignment(EHTA_Center);L->SetWorldSize(Size);L->SetTextRenderColor(FColor(222,207,170));L->RegisterComponent();};
 auto Loot=[&](FName Mesh,FVector V,float Yaw=0,int Lock=0){auto* O=Object(ELWObjectKind::Container,V,Yaw);if(!O)return;W->EnsureSiteContainer(O->RecordId,S,O->GetActorLocation(),Lock);O->Body->SetStaticMesh(W->Mesh(Mesh));};
 auto Furniture=[&](FName Type,FVector V,float Yaw=0){if(auto* O=Object(ELWObjectKind::Furniture,V,Yaw))O->SetFurniture(Type);};
 auto Door=[&](FVector V,float Yaw=0){auto* O=Object(ELWObjectKind::Door,V,Yaw);if(O)O->Body->SetMaterial(0,W->Material(TEXT("DoorPaintV7")));};
 auto Wall=[&](FVector V,float Length,float Yaw=0,bool Open=false,FName Mat=TEXT("PolicePaintV18")){auto Seg=[&](float X,float Width,float Z,float Height){BoxAt(Mat,V+FRotator(0,Yaw,0).RotateVector(FVector(X,0,Z)),FVector(Width,18,Height),Yaw);};if(Open){for(int Side:{-1,1})Seg(Side*(Length+160)*.25f,(Length-160)*.5f,165,330);Seg(0,160,283,94);Door(V+FRotator(0,Yaw,0).RotateVector(FVector(-80,0,0)),Yaw);}else Seg(0,Length,165,330);};
 auto Light=[&](FVector V,FLinearColor Color=FLinearColor(.9f,.8f,.65f)){Model(TEXT("CeilingLightV13"),V);auto* L=NewObject<UPointLightComponent>(this);L->SetupAttachment(RootComponent);L->SetRelativeLocation(At(V-FVector(0,0,20)));L->SetIntensity(18000);L->SetAttenuationRadius(1150);L->SetLightColor(Color);L->SetCastShadows(false);L->RegisterComponent();};
 auto Papers=[&](FVector V,float Yaw=0){Model(TEXT("DeskSetV13"),V,Yaw,false);};
 auto Shelf=[&](FVector V,float Yaw=0){Loot(TEXT("Shelf"),V,Yaw);for(float Z:{13.f,65.f,118.f})Model(TEXT("PantryV13"),V+FVector(0,0,Z),Yaw,false);};
 auto Bench=[&](FVector V,float Yaw=0){for(int I=-1;I<=1;I++)Furniture(TEXT("chair"),V+FRotator(0,Yaw,0).RotateVector(FVector(I*85,0,0)),Yaw);};
 auto Shell=[&](FVector Center,float X,float Y,float Yaw,const FString& Name,FName Exterior){auto U=[&](FVector V){return Center+FRotator(0,Yaw,0).RotateVector(V);};
 BoxAt(TEXT("Concrete"),U(FVector(0,0,6)),FVector(X*2+40,Y*2+40,24),Yaw);BoxAt(TEXT("TileV7"),U(FVector(0,0,20)),FVector(2*X,2*Y,8),Yaw);
 Wall(U(FVector(0,Y,24)),2*X,Yaw,false,Exterior);for(int Side:{-1,1})Wall(U(FVector(Side*X,0,24)),2*Y,Yaw+90,false,Exterior);
 for(int Side:{-1,1}){float Mid=Side*(X+100)*.5f,Width=X-100;BoxAt(Exterior,U(FVector(Mid,-Y,72)),FVector(Width,18,96),Yaw);BoxAt(Exterior,U(FVector(Mid,-Y,304)),FVector(Width,18,100),Yaw);int Panes=FMath::Max(1,int(Width/210));for(int I=0;I<Panes;I++){float PX=Mid-Width*.5+(I+.5)*Width/Panes;auto* Window=Object(ELWObjectKind::Window,U(FVector(PX,-Y,186)),Yaw);if(Window)Window->SetActorScale3D(FVector((Width/Panes-16)/100,.035,1.3));BoxAt(TEXT("Steel"),U(FVector(PX-Width/Panes*.5,-Y,187)),FVector(12,26,142),Yaw);}}
 Wall(U(FVector(0,-Y,24)),200,Yaw,true,Exterior);BoxAt(TEXT("CorrugatedV7"),U(FVector(0,0,367)),FVector(2*X+65,2*Y+65,26),Yaw);
 BoxAt(TEXT("Acoustic65"),U(FVector(0,0,350)),FVector(2*X-18,2*Y-18,8),Yaw);
 BoxAt(TEXT("Steel"),U(FVector(0,-Y-25,342)),FVector(FMath::Min(X*1.8f,1500.f),32,80),Yaw);Text(Name,U(FVector(0,-Y-45,325)),Yaw-90,Name.Len()>20?22:32);
 for(float XX=-X+300;XX<X;XX+=800)for(float YY=-Y+330;YY<Y;YY+=800)Light(U(FVector(XX,YY,334)),S.Type==5?FLinearColor(.65f,.68f,1.f):FLinearColor(.9f,.8f,.65f));
 };
 float X=S.Size.X*.5f,Y=S.Size.Y*.5f;
 if(S.Type==11){
 BoxAt(TEXT("Asphalt"),FVector(0,0,0),FVector(2*X,2*Y,20));BoxAt(TEXT("Concrete"),FVector(0,400,10),FVector(2*X,1800,20));
 const TCHAR* Shops[]={TEXT("CROSSROADS PHARMACY"),TEXT("VIDEO EXCHANGE"),TEXT("HARDWARE & REPAIR"),TEXT("CORNER GROCER")};
 for(int I=0;I<4;I++){FVector C(-2700+I*1800,1700,0);Shell(C,820,650,0,Shops[I],TEXT("BrickV7"));for(int Side:{-1,1})for(int J=0;J<3;J++){FVector V=C+FVector(Side*530,-240+J*240,24);if(I==1){Loot(TEXT("BookcaseV13"),V);Papers(V+FVector(0,0,130));}else Shelf(V);}Loot(TEXT("ShopCounterV18"),C+FVector(-390,-440,24));Papers(C+FVector(-390,-440,122));Furniture(TEXT("chair"),C+FVector(-400,-290,24),-90);Wall(C+FVector(0,350,24),1640,0,true,TEXT("PlasterV7"));Loot(TEXT("EvidenceCabinetV18"),C+FVector(500,530,24),0,I==0?2:1);Model(TEXT("WasteBinV13"),C+FVector(720,-500,24));}
 for(int Side:{-1,1}){FVector C(Side*2850,-550,0);float Face=Side<0?90:-90;Shell(C,700,600,Face,Side<0?TEXT("PLAZA CAFE"):TEXT("LAUNDROMAT"),TEXT("BrickV7"));auto U=[&](FVector V){return C+FRotator(0,Face,0).RotateVector(V);};if(Side<0){Furniture(TEXT("cooker"),U(FVector(-300,420,24)),Face);Furniture(TEXT("sink"),U(FVector(-170,420,24)),Face);for(int I=0;I<3;I++){Model(TEXT("DinerTableV9"),U(FVector(-400+I*400,-150,24)),Face);Furniture(TEXT("chair"),U(FVector(-400+I*400,-260,24)),Face+90);}}else for(int I=0;I<5;I++){Loot(TEXT("CabinetV3"),U(FVector(-480+I*240,350,24)),Face);BoxAt(TEXT("Steel"),U(FVector(-480+I*240,270,88)),FVector(110,8,80),Face);}}
 for(int Side:{-1,1})for(int I=0;I<3;I++){Model(TEXT("PlazaPlanterV18"),FVector(Side*1350,-750+I*550,24));Bench(FVector(Side*900,-500+I*550,24),Side<0?0:180);}
 for(int I=0;I<8;I++){float PX=-1650+I*470;BoxAt(TEXT("Lane"),FVector(PX,-2000,12),FVector(8,650,2),0,false);if(Rand.FRand()<.35f)Object(ELWObjectKind::Car,FVector(PX+190,-2000,75),90);}
 BoxAt(TEXT("PolicePaintV18"),FVector(0,-1430,342),FVector(1480,34,125));Text(TEXT("CROSSROADS PLAZA"),FVector(0,-1450,318),-90,50);for(int Side:{-1,1})BoxAt(TEXT("Steel"),FVector(Side*660,-1430,160),FVector(12,12,320));
 }else{
 Shell(FVector::ZeroVector,X,Y,0,S.Type==17?TEXT("POLICE / PUBLIC SAFETY"):TEXT("DEAD ORBIT ARCADE"),S.Type==17?TEXT("BrickV7"):TEXT("PolicePaintV18"));
 if(S.Type==17){
 // Straight central corridor; rooms use complete partitions with fitted doors.
 for(int Side:{-1,1})for(int Row=0;Row<3;Row++){float CY=-900+Row*900;Wall(FVector(Side*220,CY,24),900,90,true);if(Row<2)Wall(FVector(Side*(X+220)*.5f,CY+450,24),X-220);}
 // Public lobby / reception and dispatcher consoles.
 Loot(TEXT("ShopCounterV18"),FVector(-800,-980,24));Model(TEXT("PoliceDispatchV18"),FVector(-1320,-1080,24));Furniture(TEXT("chair"),FVector(-1320,-1190,24),90);Bench(FVector(-750,-1260,24),90);Text(TEXT("RECEPTION"),FVector(-950,-470,280));
 // Briefing room with a central table, notice board and wall cabinets.
 Model(TEXT("DiningTableV13"),FVector(900,-930,24));for(int Side:{-1,1})Bench(FVector(900,-930+Side*130,24),Side<0?90:-90);BoxAt(TEXT("Wood"),FVector(1020,-465,205),FVector(750,8,160),0,false);Papers(FVector(900,-930,102));Text(TEXT("BRIEFING / SHIFT ROSTER"),FVector(1020,-477,250),-90,22);
 // Bullpen: desks face away from the circulation path; paper sets sit on the desktop.
 for(int Row=0;Row<2;Row++)for(int Col=0;Col<2;Col++){FVector V(-650-Col*720,-200+Row*430,24);Model(TEXT("PoliceDispatchV18"),V);Furniture(TEXT("chair"),V-FVector(0,115,0),90);Papers(V+FVector(45,0,96));}
 // Interrogation room and secure evidence store.
 Wall(FVector(1050,0,24),900,90,true);Model(TEXT("DiningTableV13"),FVector(650,0,24));Furniture(TEXT("chair"),FVector(650,-110,24),90);Furniture(TEXT("chair"),FVector(650,110,24),-90);
 for(int I=0;I<3;I++)Loot(TEXT("EvidenceCabinetV18"),FVector(1500,-220+I*250,24),90,3);Text(TEXT("EVIDENCE / AUTHORIZED STAFF"),FVector(1400,432,270),-90,18);
 // Locker room and kitchenette.
 for(int I=0;I<5;I++)Loot(TEXT("LockerV4"),FVector(-1550+I*240,1320,24));Furniture(TEXT("sink"),FVector(-1600,700,24));Furniture(TEXT("water"),FVector(-1460,700,24));Model(TEXT("DinerTableV9"),FVector(-850,800,24));Furniture(TEXT("chair"),FVector(-850,650,24),90);
 // Two cells with barred fronts and separate doors, fitted bunks, toilets.
 for(int I=0;I<2;I++){float CX=650+I*800;for(int J=0;J<8;J++)if(J<2||J>3)BoxAt(TEXT("Steel"),FVector(CX-320+J*85,640,170),FVector(10,10,290));Wall(FVector(CX+390,1000,24),760,90,false);if(auto* Bed=Object(ELWObjectKind::Furniture,FVector(CX,1250,24))){Bed->SetFurniture(TEXT("bed"));Bed->Body->SetStaticMesh(W->Mesh(TEXT("CellBunkV18")));}Model(TEXT("ToiletV4"),FVector(CX+230,1050,24));Door(FVector(CX-192,640,24),0);}Text(TEXT("HOLDING"),FVector(600,460,282));
 }else{
 Wall(FVector(0,600,24),2*X,0,true,TEXT("PolicePaintV18"));Wall(FVector(500,850,24),500,90,true,TEXT("PolicePaintV18"));Furniture(TEXT("sink"),FVector(950,950,24));Model(TEXT("ToiletV4"),FVector(1150,820,24));Loot(TEXT("EvidenceCabinetV18"),FVector(-900,950,24),0,2);Furniture(TEXT("workbench"),FVector(-450,920,24));
 // Banks of cabinets, racing/pinball zone, central air hockey and prize counter.
 for(int Side:{-1,1})for(int I=0;I<5;I++){FVector V(Side*(X-100),-850+I*270,24);float Yaw=Side<0?90:-90;Model(TEXT("ArcadeCabinetV18"),V,Yaw);if(I%2==0){auto* Glow=NewObject<UPointLightComponent>(this);Glow->SetupAttachment(RootComponent);Glow->SetRelativeLocation(At(V+FRotator(0,Yaw,0).RotateVector(FVector(0,-100,155))));Glow->SetIntensity(5500);Glow->SetAttenuationRadius(450);Glow->SetLightColor(Side<0?FLinearColor(.15f,.6f,1.f):FLinearColor(1.f,.18f,.45f));Glow->SetCastShadows(false);Glow->RegisterComponent();}Text(I%2?TEXT("INSERT COIN"):TEXT("OUT OF ORDER"),V+FRotator(0,Yaw,0).RotateVector(FVector(0,-41,153)),Yaw-90,8);}
 for(int I=0;I<3;I++)Model(TEXT("PinballV18"),FVector(-650+I*500,350,24));Model(TEXT("AirHockeyV18"),FVector(500,-300,24),90);Model(TEXT("AirHockeyV18"),FVector(-500,-300,24),90);
 Loot(TEXT("ShopCounterV18"),FVector(-620,-830,24));for(int I=0;I<4;I++)Model(TEXT("PantryV13"),FVector(-680+I*45,-830,122),0,false);Model(TEXT("WasteBinV13"),FVector(1100,-950,24));
 BoxAt(TEXT("ArcadeMuralV18"),FVector(0,585,218),FVector(2000,3,195),0,false);Text(TEXT("REDEEM TICKETS / WIN PRIZES"),FVector(-650,-910,225),-90,18);
 }
 }
 W->SpawnSiteLoot(this,S);W->SpawnEnemies(this,&S,S.Size,1,360);
}
