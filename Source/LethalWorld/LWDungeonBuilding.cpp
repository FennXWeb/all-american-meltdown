#include "LWInteriors65.h"
#include "LWDungeon.h"
#include "LWWorld.h"
#include "Components/StaticMeshComponent.h"
#include "LWWorldTextComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
void ALWChunk::BuildingDungeon(ALWWorld* W,const LWGen::FSite& S){
 LWInteriors65::FScope Interior65(this,W,S);
 using namespace LWDungeons;const auto& P=Profile(S.Type);const int Theme=S.Type==53?7:S.Type==54?0:S.Type==55?8:S.Type-First;const float X=P.Columns*Cell*.5f,Y=P.Rows*Cell*.5f,Front=-Y+Apron*.5f;const FRotator R(0,S.Yaw,0);const FVector Base=FVector(S.Position,0)-GetActorLocation();int Serial=0;
 auto At=[&](FVector V){return Base+R.RotateVector(V);};
 auto B=[&](FName M,FVector V,FVector Size,bool Solid=true){Box(W,M,At(V),Size,R,Solid);};
 auto M=[&](FName Mesh,FVector V,float Yaw=0,FVector Scale=FVector(1),bool Solid=true){Add(W,Mesh,NAME_None,At(V),Scale,R+FRotator(0,Yaw,0),Solid);};
 auto Label=[&](const FString& T,FVector V,float Size=24,float Yaw=-90,FColor Color=FColor(214,200,150)){auto* C=NewObject<ULWWorldTextComponent>(this);C->SetupAttachment(RootComponent);C->SetRelativeLocation(At(V));C->SetRelativeRotation(R+FRotator(0,Yaw,0));C->SetHorizontalAlignment(EHTA_Center);C->SetText(FText::FromString(T));C->SetWorldSize(Size);C->SetTextRenderColor(Color);C->SetCullDistance(3600);C->RegisterComponent();};
 auto Light=[&](FVector V,bool Red=false){B(TEXT("Steel"),V,FVector(150,36,16),false);B(Red?TEXT("Red"):TEXT("Bone"),V-FVector(0,0,10),FVector(130,26,5),false);auto* L=NewObject<UPointLightComponent>(this);L->SetupAttachment(RootComponent);L->SetRelativeLocation(At(V-FVector(0,0,30)));L->SetIntensity(Red?9000:18000);L->SetAttenuationRadius(1150);L->SetLightColor(Red?FLinearColor(1,.14f,.05f):Theme==6?FLinearColor(.4f,.85f,.7f):FLinearColor(.85f,.76f,.56f));L->SetCastShadows(false);L->SetMaxDrawDistance(2600);L->RegisterComponent();};
 auto* D=GetWorld()->SpawnActor<ALWDungeon>();D->Setup(W,this,S);Residents.Add(D);
 auto Device=[&](int I,FVector V){auto* O=GetWorld()->SpawnActor<ALWDungeonDevice>(GetActorLocation()+At(V),R);if(!O)return;O->Configure(W,ELWObjectKind::Container,I==3?D->Key(TEXT("reserve")):D->Key(*FString::Printf(TEXT("control_%d"),I)));O->Dungeon=D;O->Index=I;O->Body->SetStaticMesh(W->Mesh(I==3?TEXT("EvidenceCabinetV18"):TEXT("PoliceDispatchV18")));Residents.Add(O);};
 auto Loot=[&](FName Mesh,FVector V,int Lock=0,float Yaw=0){FName Id(*FString::Printf(TEXT("dungeon_%u_cache_%d"),S.Id,Serial++));W->EnsureSiteContainer(Id,S,GetActorLocation()+At(V),Lock);auto* O=W->SpawnObject(ELWObjectKind::Container,Id,GetActorLocation()+At(V),R+FRotator(0,Yaw,0));if(O){O->Body->SetStaticMesh(W->Mesh(Mesh));Residents.Add(O);}};
 // Wide arches fit the largest guards; shared partitions are emitted once.
 auto Partition=[&](FVector V,bool AlongY,bool Open){float H=Deck-16;auto Segment=[&](float Offset,float Width,float Z,float Height){B(P.Wall,V+(AlongY?FVector(0,Offset,Z):FVector(Offset,0,Z)),AlongY?FVector(22,Width,Height):FVector(Width,22,Height));};if(!Open){Segment(0,Cell,H*.5f,H);return;}for(int Sign:{-1,1})Segment(Sign*(Cell+380)*.25f,(Cell-380)*.5f,H*.5f,H);Segment(0,380,(H+410)*.5f,H-410);for(int Sign:{-1,1})B(TEXT("DoorPaintV7"),V+(AlongY?FVector(0,Sign*197,205):FVector(Sign*197,0,205)),AlongY?FVector(32,14,410):FVector(14,32,410));};
 // Exterior: freight apron, a sheltered crosswalk, and an unmistakable guarded entrance.
 B(TEXT("Concrete"),FVector(0,-Y,20),FVector(2*X+100,Apron,32));B(TEXT("Concrete"),FVector(0,-Y-Apron*.5f-150,8),FVector(600,400,16));
 const float StairX=-X+Cell*.5f;const float LandingY=Front-90;
 for(int Side:{-1,1}){B(P.Wall,FVector(Side*(X+12),0,P.Floors*Deck*.5f+28),FVector(24,2*Y+Apron,P.Floors*Deck+24));}
 B(P.Wall,FVector(0,Y+Apron*.5f,28+P.Floors*Deck*.5f),FVector(2*X,24,P.Floors*Deck));
 B(P.Wall,FVector(0,Front-Apron+12,36+(420+P.Floors*Deck)*.5f),FVector(2*X,24,P.Floors*Deck-420));
 // Front entrance opening at ground level; facade above starts beyond head height.
 // The outer forecourt has no solid front wall across its entrance.
 // Remove the front wall from the main approach by using a recessed circulation core instead.
 for(int Side:{-1,1}){B(P.Wall,FVector(Side*(X+380)*.5f,Front-Apron+35,210),FVector(X-380,30,420));}
 Label(P.Name,FVector(0,Front-Apron-8,510),45);Label(TEXT("RESTRICTED FACILITY"),FVector(0,Front-Apron-9,445),25);
 Device(4,FVector(550,Front-700,36));M(TEXT("Rubble"),FVector(X-400,Front-600,36),25,FVector(1.5));
 for(int F=0;F<P.Floors;F++){
  const float Z=36+F*Deck;const auto Plan=Layout(S.Type,S.Id,F);
  B(P.Floor,FVector(0,Apron*.5f,Z-8),FVector(2*X,2*Y,16));
  // Annexe landing is separate from the floor slab, leaving both incoming flights open.
  B(TEXT("Steel"),FVector(0,LandingY,Z-8),FVector(2*X,180,16));
  if(F==0)B(P.Floor,FVector(0,Front-Apron*.5f,Z-8),FVector(2*X,Apron,16));
  for(int I=0;I<P.Columns;I++)Partition(FVector(-X+(I+.5f)*Cell,Front,Z),false,I==0);
  for(int I=0;I<Plan.Links.Num();I++){
   const FVector C=Room(S.Type,F,I);const int Col=I%P.Columns,Row=I/P.Columns;
   if(Col<P.Columns-1)Partition(C+FVector(Cell*.5f,0,0),true,Plan.Links[I]&1);
   if(Row<P.Rows-1)Partition(C+FVector(0,Cell*.5f,0),false,Plan.Links[I]&2);
   // Continuous clear cross-shaped circulation; all heavy furniture sits in corner bays.
   const int RoomRole=(I*3+F+Theme)%6;const bool End=I==Plan.End;bool Control=false;for(int K=0;K<3;K++)Control|=F==RelayFloor(S.Type,K)&&I==RelayRoom(S.Type,S.Id,K);
   static const TCHAR* Rooms[10][6]={
    {TEXT("PUMP HALL"),TEXT("FILTER GALLERY"),TEXT("VALVE CONTROL"),TEXT("MAINTENANCE"),TEXT("CHEMICAL STORE"),TEXT("OPERATORS' ROOM")},
    {TEXT("ISOLATION WARD"),TEXT("SURGICAL THEATER"),TEXT("TRIAGE"),TEXT("MORGUE"),TEXT("PHARMACY"),TEXT("STAFF QUARTERS")},
    {TEXT("CELL BLOCK"),TEXT("VISITATION"),TEXT("INTAKE"),TEXT("MESS HALL"),TEXT("SOLITARY"),TEXT("SECURITY OFFICE")},
    {TEXT("COUNTING FLOOR"),TEXT("AUDIT ARCHIVE"),TEXT("SECURITY"),TEXT("DEPOSIT LOCKERS"),TEXT("TRANSPORT DESK"),TEXT("VAULT ANTECHAMBER")},
    {TEXT("LIVE STUDIO"),TEXT("NEWSROOM"),TEXT("TAPE ARCHIVE"),TEXT("TRANSMITTER"),TEXT("EDIT SUITE"),TEXT("EMERGENCY BROADCAST")},
    {TEXT("CASTING HALL"),TEXT("MACHINE SHOP"),TEXT("FURNACE CONTROL"),TEXT("TOOL CRIB"),TEXT("SHIFT LOCKERS"),TEXT("CRANE GALLERY")},
    {TEXT("SPECIMEN LAB"),TEXT("CLEAN ROOM"),TEXT("CONTAINMENT"),TEXT("OBSERVATION"),TEXT("COLD STORAGE"),TEXT("STAFF DECONTAMINATION")},
    {TEXT("LAUNCH CONTROL"),TEXT("MISSILE SERVICE"),TEXT("BARRACKS"),TEXT("COMMS"),TEXT("WARHEAD STORAGE"),TEXT("BLAST CONTROL")},
    {TEXT("PLATFORM"),TEXT("TICKET HALL"),TEXT("FREIGHT"),TEXT("SIGNAL ROOM"),TEXT("WORKSHOP"),TEXT("EVACUATION HOLDING")},
    {TEXT("RECEPTION"),TEXT("EXECUTIVE SUITE"),TEXT("RECORDS"),TEXT("SECURITY"),TEXT("COMMAND FLOOR"),TEXT("PRIVATE REFUGE")}};
   const FString RoomName=Control?TEXT("SECURITY CONTROL"):(S.Type==54?(RoomRole%2?TEXT("EXCAVATION CHAMBER"):TEXT("LIMESTONE GALLERY")):S.Type==53?(RoomRole%3?TEXT("SHELTER QUARTERS"):TEXT("LIFE SUPPORT")):Rooms[Theme][RoomRole]);Label(FString::Printf(TEXT("%d-%02d  %s"),F+1,I+1,*RoomName),C+FVector(0,-Cell*.5f+22,440),21);
   Light(C+FVector(-360,-360,450),(I+F)%7==0);if(End||RoomRole==0)Light(C+FVector(360,360,450));
   // Strong orientation cues: numbered wall bands, pipes or cable trays, and lit room thresholds.
   B(Theme==6?TEXT("Bone"):TEXT("Rust"),C+FVector(-Cell*.5f+17,0,310),FVector(8,Cell-60,18),false);
   B(TEXT("Steel"),C+FVector(460,0,435),FVector(26,Cell-60,26),false);
   for(int Side:{-1,1})B(TEXT("Steel"),C+FVector(460,Side*500,458),FVector(60,12,45),false);
   auto Prop=[&](FName Mesh,int Bay,float Yaw=0){if((Control||End)&&Bay>=2)return;const FVector Offset[]={{-490,-490,0},{490,-490,0},{-490,490,0},{490,490,0}};M(Mesh,C+Offset[Bay],Yaw);};
   auto Store=[&](FName Mesh,int Bay,int Lock=0){if((Control||End)&&Bay>=2)return;const FVector Offset[]={{-500,-500,0},{500,-500,0},{-500,500,0},{500,500,0}};Loot(Mesh,C+Offset[Bay],Lock);};
   if(S.Type==53&&RoomRole%3){Prop(TEXT("CellBunkV18"),0);Prop(TEXT("CellBunkV18"),3);Store(TEXT("LockerV4"),1,1);Prop(TEXT("SinkV4"),2);}
   else switch(Theme){
    case 0:case 5:case 7:
     if(RoomRole==0||RoomRole==1){for(int Bay:{0,3}){FVector V=C+FVector(Bay?470:-470,Bay?470:-470,0);B(TEXT("Steel"),V+FVector(0,0,35),FVector(370,420,70));M(TEXT("Generator"),V+FVector(0,0,70));B(TEXT("Rust"),V+FVector(0,0,295),FVector(75,75,310));}Store(TEXT("WorkbenchV4"),1,1);}
     else if(RoomRole==4){Store(TEXT("LockerV4"),0,1);Store(TEXT("Crate"),3,2);Prop(TEXT("FuelTank24"),1);}
     else{Prop(TEXT("PoliceDispatchV18"),0);Store(TEXT("Shelf"),3,1);Prop(TEXT("ChairV3"),2);}
     if(Theme==7&&RoomRole==1){B(TEXT("Bone"),C+FVector(-440,440,160),FVector(120,120,320));B(TEXT("Red"),C+FVector(-440,440,340),FVector(80,80,40));}
     break;
    case 1:case 6:
     if(RoomRole<=2){Prop(TEXT("ClinicBedV3"),0);Prop(TEXT("ClinicBedV3"),3,180);Store(TEXT("CabinetV3"),1,RoomRole==1?2:0);Prop(TEXT("SinkV4"),2);}
     else if(RoomRole==3){for(int Bay:{0,1,3})Prop(TEXT("CellBunkV18"),Bay);Store(TEXT("CabinetV3"),2,2);}
     else{Store(TEXT("EvidenceCabinetV18"),0,2);Store(TEXT("Shelf"),3,1);Prop(TEXT("PoliceDispatchV18"),1);}
     if(Theme==6){for(int Side:{-1,1}){B(TEXT("WindowGlass"),C+FVector(Side*480,480,270),FVector(320,180,200),false);B(TEXT("Steel"),C+FVector(Side*480,480,175),FVector(330,195,16));}}
     break;
    case 2:
     if(RoomRole==0||RoomRole==4){Prop(TEXT("CellBunkV18"),0);Prop(TEXT("CellBunkV18"),3);Prop(TEXT("ToiletV4"),1);Store(TEXT("LockerV4"),2,1);for(int Side:{-1,1})for(int J=0;J<5;J++)B(TEXT("Steel"),C+FVector(Side*290,-650+J*70,180),FVector(9,9,360));}
     else{Prop(TEXT("DiningTableV13"),0);Prop(TEXT("ChairV3"),2);Store(TEXT("EvidenceCabinetV18"),3,2);Prop(TEXT("PoliceDispatchV18"),1);}break;
    case 3:
     Store(TEXT("EvidenceCabinetV18"),0,2);Store(TEXT("LockerV4"),3,2);Prop(TEXT("DeskSetV13"),1);Prop(TEXT("ChairV3"),2);if(RoomRole==0){for(int J=0;J<5;J++)B(TEXT("CasinoGold21"),C+FVector(420+(J%2)*55,-450+(J/2)*35,95),FVector(45,28,22),false);}break;
    case 4:
     if(RoomRole==0){B(TEXT("Wood"),C+FVector(-490,-490,15),FVector(520,500,30));Prop(TEXT("PoliceDispatchV18"),0);B(TEXT("Red"),C+FVector(-490,-735,310),FVector(210,12,90),false);Label(TEXT("ON AIR"),C+FVector(-490,-725,300),28,90);}
     else Prop(TEXT("DeskSetV13"),0);Store(TEXT("BookcaseV13"),3,1);Prop(TEXT("ChairV3"),1);Store(TEXT("CabinetV3"),2);break;
    case 8:
     if(RoomRole==0){for(int Side:{-1,1}){B(TEXT("Steel"),C+FVector(Side*450,0,5),FVector(18,1200,10));for(int J=-4;J<=4;J++)B(TEXT("Wood"),C+FVector(Side*450,J*130,1),FVector(210,28,2),false);}Store(TEXT("Crate"),3,1);}
     else if(RoomRole==1||RoomRole==5){for(int Bay:{0,1,2})Prop(TEXT("ChairV3"),Bay);Store(TEXT("CabinetV3"),3);}
     else{Store(TEXT("Crate"),0,1);Store(TEXT("Shelf"),3,2);Prop(TEXT("WorkbenchV4"),1);}break;
    default:
     if(RoomRole==1||RoomRole==5){Prop(TEXT("HomeBedV13"),0);Store(TEXT("NightstandV13"),1,2);Store(TEXT("SideboardV13"),3,2);Prop(TEXT("ChairV3"),2);}
     else{Prop(TEXT("CasinoCounterV21"),0);Store(TEXT("BookcaseV13"),3,2);Prop(TEXT("PoliceDispatchV18"),1);Prop(TEXT("ChairV3"),2);}break;
   }
   // The final control is an alcove; reward chest and sentries keep a clear central arena.
   for(int Relay=0;Relay<3;Relay++)if(F==RelayFloor(S.Type,Relay)&&I==RelayRoom(S.Type,S.Id,Relay)){Device(Relay,C+FVector(430,480,0));Label(FString::Printf(TEXT("CONTROL %d"),Relay+1),C+FVector(0,Cell*.5f-24,305),30);}
   if(F==P.Floors-1&&End){Device(3,C+FVector(-430,480,0));Label(P.Finale,C+FVector(0,Cell*.5f-22,395),27);Label(TEXT("LEGENDARY GUARDIAN / SECURED RESERVE"),C+FVector(0,Cell*.5f-22,345),21);}
  }
  Label(FString::Printf(TEXT("LEVEL %d / %d  |  %s"),F+1,P.Floors,P.Theme),FVector(0,Front+14,Z+360),25,90);
  // Two return flights, 17.86cm rises, 30cm treads, 260cm middle landing.
  if(F<P.Floors-1){for(int J=0;J<14;J++){float H=(J+1)*Deck/28;
    B(TEXT("Steel"),FVector(StairX-290,Front-180-(J+.5f)*30,Z+H-10),FVector(540,30,20));
    B(TEXT("Steel"),FVector(StairX+290,Front-600+(J+.5f)*30,Z+Deck*.5f+H-10),FVector(540,30,20));}
   B(TEXT("Steel"),FVector(StairX,Front-725,Z+Deck*.5f-10),FVector(1160,250,20));
   for(int Side:{-1,1}){B(TEXT("Steel"),FVector(StairX+Side*590,Front-420,Z+185),FVector(10,440,10));B(TEXT("Steel"),FVector(StairX+Side*590,Front-420,Z+420),FVector(10,440,10));}
  }
  Light(FVector(StairX,Front-470,Z+465));Label(TEXT("STAIRS"),FVector(StairX,Front+15,Z+275),30,90);
 }
 B(P.Wall,FVector(0,Apron*.5f,36+P.Floors*Deck),FVector(2*X+70,2*Y+60,24));
 B(TEXT("CorrugatedV7"),FVector(0,Front-Apron*.5f,36+P.Floors*Deck),FVector(2*X+70,Apron+40,24));
 // Recessed shutter bays and structural pilasters break up the large exterior mass.
 for(int F=0;F<P.Floors;F++){
  const float Z=36+F*Deck;
  for(int Col=0;Col<P.Columns;Col++){float XX=-X+(Col+.5f)*Cell;
   if(F>0||FMath::Abs(XX)>850){B(TEXT("Steel"),FVector(XX,Front-Apron-4,Z+220),FVector(720,8,190),false);for(int J=0;J<6;J++)B(TEXT("CorrugatedV7"),FVector(XX,Front-Apron-12,Z+150+J*26),FVector(690,20,12),false);}
   B(TEXT("Concrete"),FVector(-X+Col*Cell,Front-Apron-18,Z+245),FVector(60,60,490));
  }
  B(TEXT("Steel"),FVector(0,Front-Apron-28,Z+465),FVector(2*X+70,70,26));
  for(int Side:{-1,1})for(int Row=0;Row<P.Rows;Row++){float YY=-Y+(Row+.5f)*Cell+Apron*.5f;B(TEXT("Concrete"),FVector(Side*(X+28),YY-Cell*.5f,Z+245),FVector(60,60,490));B(TEXT("CorrugatedV7"),FVector(Side*(X+27),YY,Z+270),FVector(6,680,210),false);}
 }
 // Type-specific rooftop silhouettes: chimney bank, transmission mast, or command aerials.
 if(Theme==0||Theme==5||Theme==7)for(int I=0;I<3;I++){FVector V(-X+650+I*650,Y-200,36+P.Floors*Deck);B(TEXT("Rust"),V+FVector(0,0,600),FVector(240,240,1200));B(TEXT("Steel"),V+FVector(0,0,1190),FVector(290,290,40));}
 if(Theme==4||Theme==9){FVector V(0,0,36+P.Floors*Deck);B(TEXT("Steel"),V+FVector(0,0,1000),FVector(60,60,2000));for(int J=0;J<5;J++)B(TEXT("Steel"),V+FVector(0,0,450+J*300),FVector(1100-J*180,35,35));}
 for(int I=0;I<P.Columns;I++){FVector V(-X+800+I*1600,Front-Apron-20,600);M(TEXT("Generator"),FVector(V.X,Y-300,40+P.Floors*Deck));}
}
