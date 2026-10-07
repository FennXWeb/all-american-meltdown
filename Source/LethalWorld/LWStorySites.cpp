#include "LWStory.h"
#include "LWInteriors65.h"
#include "LWChapter52.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWWorldTextComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "LWLootTable.h"

int32 ALWStoryDirector::SiteRevision38(int32 I) const{return 520+ (I==1?int(State().Flags.Contains(TEXT("mercy_burned"))):I==6?int(State().Flags.Contains(TEXT("mara_cell_open52"))):I==8?(State().Stage<=22?0:State().Stage<27?1:2)+int(State().Flags.Contains(TEXT("settler_cells_open52")))*4:0);}
void ALWStoryDirector::BuildSite(int I){
 int Revision=SiteRevision38(I);if(Sites.Contains(I)){if(SiteRevisions38.FindRef(I)==Revision)return;if(auto* Old=Sites.FindRef(I).Get())Old->Destroy();Sites.Remove(I);}SiteRevisions38.Add(I,Revision);
 auto* C=GetWorld()->SpawnActor<ALWChunk>(At(I),FRotator::ZeroRotator);if(!C)return;Sites.Add(I,C);C->LightingWorld=World;int Serial=0;
 LWGen::FSite InteriorSite65;InteriorSite65.Position=FVector2D(C->GetActorLocation());InteriorSite65.Id=0x650000+I;InteriorSite65.Type=I==2?2:I==4?13:I==1?7:17;InteriorSite65.Size=FVector2D(12000);LWInteriors65::FScope Interior65(C,World,InteriorSite65);
 // Reserve future mission actors as well as objects already instantiated. Nodes
 // and combatants are spawned after BuildSite, and must retain their approaches.
 TSet<FIntVector> Reserved65;
 auto Reserve65=[&](FVector V){const FIntVector Key(FMath::RoundToInt(V.X),FMath::RoundToInt(V.Y),FMath::RoundToInt(V.Z));if(Reserved65.Contains(Key))return;Reserved65.Add(Key);const FVector Q=Interior65.Frame.InverseTransformPosition(At(I,V));Interior65.Record({FBox(Q-FVector(165,165,150),Q+FVector(165,165,130)),TEXT("story clearance"),false,false,true});};
 for(int Stage=0;Stage<LWStory::Missions().Num();Stage++){const auto& Mission=LWStory::Missions()[Stage];if(Mission.Site!=I)continue;Reserve65(LWChapter52::Recovery(Stage));if(FCString::Strlen(Mission.Action))Reserve65(LWChapter52::Objective(Mission.Action));for(FVector Post:LWChapter52::Posts(I,Stage))Reserve65(Post);}
 if(I==2)for(FName Action:{FName(TEXT("triage")),FName(TEXT("pharmacy"))})Reserve65(LWChapter52::Objective(Action));
 if(I==4)for(FName Action:{FName(TEXT("relay_a")),FName(TEXT("relay_b"))})Reserve65(LWChapter52::Objective(Action));
 if(I==7)for(FName Action:{FName(TEXT("breach_a")),FName(TEXT("breach_b"))})Reserve65(LWChapter52::Objective(Action));
 if(I==8)for(FName Action:{FName(TEXT("shield_a")),FName(TEXT("shield_b"))})Reserve65(LWChapter52::Objective(Action));
 for(FVector V:{FVector(-250,-2350,100),FVector(250,-2100,100),FVector(0,-2200,100),FVector(-650,-2700,100),FVector(650,-2700,100),FVector(900,-2450,100)})Reserve65(V);
 if(I==1)Reserve65(FVector(-2300,200,110));if(I==2)Reserve65(FVector(-2450,0,110));if(I==5)Reserve65(FVector(-2700,-1500,110));if(I==6){Reserve65(FVector(-2500,100,110));Reserve65(FVector(1600,3400,100));}if(I==7){Reserve65(FVector(0,700,100));for(int K=0;K<4;K++)Reserve65(FVector((K-1.5f)*180,350,100));}if(I==8)for(int K=0;K<8;K++)Reserve65(FVector(3350,-2350+K*320,110));
 auto Box=[&](FName M,FVector P,FVector D,float Yaw=0,bool Hit=true){C->Box(World,M,P,D,FRotator(0,Yaw,0),Hit);};
 auto Label=[&](FString S,FVector P,float Size=30,float Yaw=-90){auto* T=NewObject<ULWWorldTextComponent>(C);T->SetupAttachment(C->GetRootComponent());T->SetRelativeLocation(P);T->SetRelativeRotation(FRotator(0,Yaw,0));T->SetText(FText::FromString(S));T->SetWorldSize(Size);T->SetHorizontalAlignment(EHTA_Center);T->SetTextRenderColor(FColor(230,212,175));T->RegisterComponent();C->AddInstanceComponent(T);};
 auto Model=[&](FName M,FVector P,float Yaw=0,float Scale=1){auto* Mesh=World->Mesh(M);if(!Mesh)return;P.Z-=Mesh->GetBoundingBox().Min.Z*Scale;C->Add(World,M,NAME_None,P,FVector(Scale),FRotator(0,Yaw,0));};
 auto Object=[&](ELWObjectKind K,FName M,FVector P,float Yaw=0){auto* O=World->SpawnObject(K,FName(*FString::Printf(TEXT("story52_%d_%d"),I,Serial++)),At(I,P),FRotator(0,Yaw,0));if(O){C->Residents.Add(O);if(K==ELWObjectKind::Furniture)O->SetFurniture(M);else if(K!=ELWObjectKind::Door&&K!=ELWObjectKind::Window)O->Body->SetStaticMesh(World->Mesh(M));}return O;};
 auto Loot=[&](FVector P,FName M=TEXT("Crate")){auto* O=Object(ELWObjectKind::Container,M,P);if(!O)return;auto& R=World->Containers.FindOrAdd(O->RecordId);if(R.Id.IsNone()){R.Id=O->RecordId;R.Context=I==2?TEXT("clinic"):TEXT("depot");R.Position=At(I,P);for(auto Item:LWLoot::Roll(R.Context,GetTypeHash(R.Id)))LWItems::Place(R.Items,Item,12,12);}};
 auto Light=[&](FVector P,bool Warm=true){Model(TEXT("CeilingLightV13"),P);auto* L=NewObject<UPointLightComponent>(C);L->SetupAttachment(C->GetRootComponent());L->SetRelativeLocation(P-FVector(0,0,10));L->SetIntensity(24000);L->SetAttenuationRadius(1000);L->SetLightColor(Warm?FLinearColor(1,.72f,.40f):FLinearColor(.65f,.80f,1));L->SetCastShadows(false);L->RegisterComponent();C->AddInstanceComponent(L);};
 // Individually authored rooms. A 160cm hinge door fits the 164cm opening; side windows have actual breakable glass.
 auto Room=[&](FVector P,FVector2D Half,FName Mat,FString Name,float Yaw=0,bool Roof=true,bool Door=true){auto Q=[&](FVector V){return P+FRotator(0,Yaw,0).RotateVector(V);};float X=Half.X,Y=Half.Y;
 Box(I==2?TEXT("TileV7"):TEXT("Concrete"),Q(FVector(0,0,0)),FVector(X*2,Y*2,20),Yaw);
 for(int S:{-1,1}){Box(Mat,Q(FVector(S*(X+82)*.5,-Y,180)),FVector(X-82,22,360),Yaw);Box(Mat,Q(FVector(S*X,0,65)),FVector(22,Y*2,130),Yaw);Box(Mat,Q(FVector(S*X,0,320)),FVector(22,Y*2,80),Yaw);for(int End:{-1,1})Box(Mat,Q(FVector(S*X,End*(Y+160)*.5,205)),FVector(22,Y-160,150),Yaw);
 auto* Glass=Object(ELWObjectKind::Window,NAME_None,Q(FVector(S*X,0,205)),Yaw);if(Glass)Glass->Body->SetRelativeScale3D(FVector(.08,3.2,1.5));}
 Box(Mat,Q(FVector(0,Y,180)),FVector(X*2,22,360),Yaw);Box(Mat,Q(FVector(0,-Y,303)),FVector(164,22,114),Yaw);
 if(Door)Object(ELWObjectKind::Door,NAME_None,Q(FVector(-80,-Y,10)),Yaw);
 if(Roof){const FName RoofMat=(I==0||I==2||I==6||I==8)?TEXT("Rubber"):TEXT("CorrugatedV7");Box(RoofMat,Q(FVector(0,0,370)),FVector(X*2+35,Y*2+35,22),Yaw);Light(Q(FVector(0,0,340)),I!=8);
 if(I==1){const float Rise=240,Angle=FMath::RadiansToDegrees(FMath::Atan2(Rise,X));for(int Side:{-1,1})C->Box(World,TEXT("Wood"),Q(FVector(Side*X*.5f,0,382+Rise*.5f)),FVector(FMath::Sqrt(X*X+Rise*Rise)+30,Y*2+55,24),FRotator(-Side*Angle,Yaw,0),true);for(int Z=0;Z<8;Z++)for(int End:{-1,1})Box(TEXT("Wood"),Q(FVector(0,End*Y,385+Z*30)),FVector(2*X*(1-float(Z)/8),22,30),Yaw);Box(TEXT("BrickV7"),Q(FVector(X*.6f,Y*.5f,570)),FVector(130,150,390),Yaw);}
 else {for(int Side:{-1,1}){Box(Mat,Q(FVector(Side*X,0,412)),FVector(28,Y*2+28,80),Yaw);Box(Mat,Q(FVector(0,Side*Y,412)),FVector(X*2+28,28,80),Yaw);}if(X>600){Box(TEXT("Steel"),Q(FVector(X*.45f,Y*.45f,440)),FVector(240,300,115),Yaw);for(int V=0;V<6;V++)Box(TEXT("Rubber"),Q(FVector(X*.45f-90+V*36,Y*.45f,501)),FVector(18,250,6),Yaw,false);Box(TEXT("Rust"),Q(FVector(-X*.6f,Y*.55f,510)),FVector(65,65,270),Yaw);}}
 }
 // Continuous fascia and entrance canopies make each frontage readable on foot.
 const FName Trim=I==0?TEXT("Red"):I==2?TEXT("CanadaArmor51"):I==8?TEXT("Steel"):TEXT("Rust");Box(Trim,Q(FVector(0,-Y-15,272)),FVector(X*2+40,14,28),Yaw);if(X>600){Box(Trim,Q(FVector(0,-Y-150,260)),FVector(600,330,22),Yaw);for(int Side:{-1,1})Box(TEXT("Steel"),Q(FVector(Side*280,-Y-285,130)),FVector(14,14,260),Yaw);}

 Label(Name,Q(FVector(0,-Y-15,308)),27,Yaw-90);for(int S:{-1,1})Box(TEXT("Wood"),Q(FVector(S*(X-16),0,25)),FVector(12,Y*2,25),Yaw);
 };
 auto Cover=[&](FVector P,float Yaw=0){Box(TEXT("Concrete"),P+FVector(0,0,65),FVector(460,130,130),Yaw);for(int K=0;K<3;K++)Box(TEXT("Rust"),P+FRotator(0,Yaw,0).RotateVector(FVector(-150+K*150,0,133)),FVector(55,140,6),Yaw,false);};
 auto Desk=[&](FVector P,float Yaw=0){Model(TEXT("Desk"),P,Yaw);Model(TEXT("DeskSetV13"),P+FVector(0,0,80),Yaw);Object(ELWObjectKind::Furniture,TEXT("chair"),P+FRotator(0,Yaw,0).RotateVector(FVector(-90,0,0)),Yaw);};
 auto Bed=[&](FVector P,float Yaw=0){Object(ELWObjectKind::Furniture,TEXT("bed"),P,Yaw);Model(TEXT("CabinetV3"),P+FRotator(0,Yaw,0).RotateVector(FVector(0,170,0)),Yaw);};
 auto Gate=[&](FVector P,bool Open){if(Open)return;for(int X=-80;X<=80;X+=20)Box(TEXT("Steel"),P+FVector(X,0,120),FVector(6,10,240));for(int Z:{20,220})Box(TEXT("Steel"),P+FVector(0,0,Z),FVector(166,12,8));};
 const FName GroundMat=I==1||I==4?TEXT("Earth"):I==0||I==3||I==5||I==7?TEXT("Asphalt"):TEXT("Concrete");Box(GroundMat,FVector(0,0,-20),FVector(8800,8800,20));
 // Central circulation remains clear; curb islands and service details stay at the lot edges.
 if(I==1||I==4){Box(TEXT("Concrete"),FVector(0,-1900,-7),FVector(460,4700,6));for(int Side:{-1,1})Box(TEXT("Concrete"),FVector(Side*1400,-800,-7),FVector(2400,340,6));}
 if(I==0||I==3||I==5){for(int K=0;K<7;K++)Box(TEXT("CanadaArmor51"),FVector(-3500+K*520,-3250,-7),FVector(14,900,4),0,false);for(int Side:{-1,1})Box(TEXT("Concrete"),FVector(Side*4000,0,8),FVector(90,7400,35));}
 for(int Side:{-1,1}){for(int K=0;K<4;K++){FVector Edge(Side*4050,-1200+K*1200,0);Box(TEXT("Earth"),Edge+FVector(0,0,6),FVector(380,750,25));for(int J=0;J<3;J++)Model(TEXT("Crate"),Edge+FVector(J*55-55,150-J*110,20),J*27,.55f);}}
Box(TEXT("Asphalt"),FVector(0,-4200,-5),FVector(8800,400,12));
 Label(LWStory::SiteName(I),FVector(0,-3960,300),45);for(int S:{-1,1})Box(TEXT("Steel"),FVector(S*1300,-3970,170),FVector(25,25,340));Box(TEXT("Rust"),FVector(0,-3970,325),FVector(2750,25,180));
 for(int K=0;K<3;++K){C->StreetLight(World,FVector(-4000,-2900+K*2800,0),FRotator::ZeroRotator,false);}
 if(I==0){
 Room(FVector(-2100,400,10),{1000,1200},TEXT("BrickV7"),TEXT("MILE NINE DINER"));Room(FVector(-2100,2450,10),{1000,650},TEXT("TileV7"),TEXT("KITCHEN / DELIVERY"),180);Room(FVector(2400,1900,10),{1000,1000},TEXT("Concrete"),TEXT("SERVICE BAYS"),0,true,false);
 for(int K=0;K<3;K++){Model(TEXT("DinerBoothV9"),FVector(-2700,-450+K*650,22));Model(TEXT("DinerTableV9"),FVector(-2350,-450+K*650,22));Model(TEXT("TableSettingV13"),FVector(-2350,-450+K*650,102));}
 Model(TEXT("KitchenRangeV9"),FVector(-2450,2700,22));Model(TEXT("KitchenHoodV9"),FVector(-2450,2700,200));Object(ELWObjectKind::Furniture,TEXT("sink"),FVector(-1550,2700,22));Model(TEXT("ServiceBenchV3"),FVector(2450,2200,22));Loot(FVector(2900,2500,22),TEXT("LockerV4"));
 for(int K=0;K<5;K++){Box(TEXT("CanadaArmor51"),FVector(900+K*570,-2100,-7),FVector(12,1400,2),0,false);Cover(FVector(900+K*570,-3000,0));}Model(TEXT("DiningTableV13"),FVector(-1500,-2000,0));Label(TEXT("MARA'S CAMP / CLEAN WATER"),FVector(-1500,-1800,180),25);
 }else if(I==1){
 bool Burn=State().Flags.Contains(TEXT("mercy_burned"));Room(FVector(-2400,400,10),{1100,1500},Burn?TEXT("Rubber"):TEXT("Wood"),TEXT("COMMUNAL KITCHEN"),0,!Burn);Room(FVector(2450,700,10),{950,1300},TEXT("Concrete"),TEXT("CLINIC / WATER"));
 for(int K=0;K<4;K++){Model(TEXT("DiningTableV13"),FVector(-2300,-500+K*620,22));Object(ELWObjectKind::Furniture,TEXT("chair"),FVector(-2550,-500+K*620,22));Bed(FVector(2550,-200+K*480,22));}
 for(int S:{-1,1}){Room(FVector(S*2500,-2700,10),{800,550},TEXT("Wood"),TEXT("FAMILY SHELTER"),0,!Burn);Bed(FVector(S*2500,-2600,22));}
 for(int K=0;K<4;K++){Box(TEXT("Wood"),FVector(-700+K*440,1200,40),FVector(320,1600,80));Box(TEXT("Earth"),FVector(-700+K*440,1200,82),FVector(290,1550,8));}
 Model(TEXT("Generator"),FVector(900,2800,0));Object(ELWObjectKind::Furniture,TEXT("cooker"),FVector(-3150,1400,22));Label(TEXT("NO ONE OWNS THE WATER"),FVector(2450,-615,300));
 if(Burn)for(int K=0;K<16;K++){Box(TEXT("Rubber"),FVector(-3000+(K%4)*450,-700+(K/4)*650,45),FVector(420,45,60),K*37);C->Add(World,TEXT("IntroCloudV15"),TEXT("IntroSmoke"),FVector(-2400,K*120-500,550),FVector(1.2),FRotator::ZeroRotator,false);}
 }else if(I==2){
 Room(FVector(-2200,300,10),{950,1250},TEXT("Concrete"),TEXT("ST. AGNES / EMERGENCY"));Room(FVector(2100,1600,10),{1050,1400},TEXT("TileV7"),TEXT("PHARMACY / SURGERY"));Room(FVector(-2400,2700,10),{1000,650},TEXT("TileV7"),TEXT("RECOVERY WARD"));Room(FVector(1800,-2300,10),{1400,550},TEXT("Concrete"),TEXT("AMBULANCE INTAKE"));
 for(int K=0;K<4;K++){Model(TEXT("ClinicBedV3"),FVector(-2700+(K%2)*900,-300+(K/2)*900,22),90);Model(TEXT("ClinicBedV3"),FVector(-2900+K*350,2600,22),90);}
 for(int K=0;K<5;K++)Loot(FVector(2700,700+K*380,22),TEXT("CabinetV3"));Desk(FVector(1750,-2200,22));Object(ELWObjectKind::Furniture,TEXT("sink"),FVector(1300,2100,22));Model(TEXT("MedicalCase52"),FVector(-2700,850,95));Cover(FVector(0,-800,0));Cover(FVector(500,1400,0),90);
 }else if(I==3){
 Room(FVector(-2350,1800,10),{1150,1400},TEXT("BrickV7"),TEXT("COLLECTION / RECORDS"));Room(FVector(2600,2100,10),{950,1050},TEXT("Concrete"),TEXT("INSPECTION / IMPOUND"));
 for(int K=0;K<3;K++){Room(FVector(-1400+K*1400,-1100,10),{320,600},TEXT("Concrete"),FString::Printf(TEXT("LANE %d"),K+1),90);Box(TEXT("Steel"),FVector(-1400+K*1400,-2200,85),FVector(1000,18,28));Box(TEXT("Rust"),FVector(-1400+K*1400,-900,550),FVector(700,1400,35));}
 for(int K=0;K<3;K++){Desk(FVector(-2850+K*530,2000,22));Loot(FVector(-3000+K*650,2700,22),TEXT("EvidenceCabinetV18"));}Cover(FVector(1200,400,0),90);Model(TEXT("Wreck"),FVector(2600,-2300,0),90);Label(TEXT("NO RECEIPT / NO PASSAGE"),FVector(0,-1900,460),38);
 }else if(I==4){
 Room(FVector(-2400,900,10),{1000,1300},TEXT("Concrete"),TEXT("RELAY SIX / SWITCH ROOM"));Room(FVector(2500,300,10),{850,1000},TEXT("Concrete"),TEXT("DIESEL / POWER"));
 for(int K=0;K<4;K++){Model(TEXT("LockerV4"),FVector(-3000+K*390,1600,22));Model(TEXT("ServiceBenchV3"),FVector(-2900+K*350,650,22));}Loot(FVector(2800,900,22));Model(TEXT("Generator"),FVector(2700,800,22));
 for(int K=0;K<9;K++){float H=K*350;for(int S:{-1,1}){Box(TEXT("Steel"),FVector(S*(500-H*.12),2800,H+175),FVector(35,35,360));Box(TEXT("Steel"),FVector(S*(450-H*.1),2800,H+175),FVector(35,600,25),0);}Box(TEXT("Steel"),FVector(0,2800,H),FVector(FMath::Max(160.f,1050-H*.25),45,25));}
 for(int S:{-1,1}){Box(TEXT("Rubber"),FVector(S*1250,700,4),FVector(2200,25,12));Cover(FVector(S*1400,-1400,0));}Label(TEXT("EMERGENCY BROADCAST / 94.6"),FVector(-2400,-415,280),28);
 }else if(I==5){
 Room(FVector(-2700,-1400,10),{850,1000},TEXT("Concrete"),TEXT("DRIVER REST / MAINTENANCE"));Room(FVector(2500,1700,10),{1000,1500},TEXT("BrickV7"),TEXT("FREIGHT DISPATCH"));
 for(int X:{-1200,100}){for(int S:{-1,1})Box(TEXT("Steel"),FVector(X+S*95,800,3),FVector(10,5800,12));for(int Y=-2000;Y<3600;Y+=160)Box(TEXT("Wood"),FVector(X,Y,-3),FVector(300,35,18));}
 for(int K=0;K<5;K++){FVector P(-2700+K%2*1400,1000+K/2*1000,0);Box(TEXT("CorrugatedV7"),P+FVector(0,0,165),FVector(800,750,330));for(int J=-3;J<=3;J++)Box(TEXT("Steel"),P+FVector(J*110,-380,165),FVector(16,12,330));Loot(P+FVector(550,-350,10));}
 Desk(FVector(2300,1900,22));for(int K=0;K<4;K++)Loot(FVector(3100,400+K*500,22));Bed(FVector(-2900,-1100,22));Model(TEXT("ServiceBenchV3"),FVector(-2700,-1900,22));Cover(FVector(1800,-1500,0),90);
 }else if(I==6){
 Room(FVector(-2500,0,10),{1000,1100},TEXT("TileV7"),TEXT("PRISON INFIRMARY"));Room(FVector(-2450,2400,10),{1000,850},TEXT("BrickV7"),TEXT("WARDEN / RECORDS"));Desk(FVector(-2400,2300,22));
 // Cells face a real central corridor, not intersecting full-width walls across the route.
 for(int Row=0;Row<3;Row++)for(int Side:{-1,1}){FVector P(1600+Side*1050,-500+Row*1350,10);Room(P,{700,530},TEXT("Concrete"),FString::Printf(TEXT("CELL C-%d"),Row*2+(Side>0?2:1)),Side>0?-90:90);Bed(P+FVector(0,100,12),90);}
 Room(FVector(1600,3400,10),{450,430},TEXT("Concrete"),TEXT("MEDICAL HOLD"),0,true,false);Gate(FVector(1600,2970,22),State().Flags.Contains(TEXT("mara_cell_open52")));Bed(FVector(-2600,200,22));Model(TEXT("ToiletV4"),FVector(1800,3650,22));Cover(FVector(-800,-1800,0),90);
 }else if(I==7){
 // High foundry nave, furnaces, casting lines and an accessible service mezzanine.
 for(int Side:{-1,1}){Box(TEXT("BrickV7"),FVector(Side*3200,3200,1300),FVector(300,340,2600));for(int H:{1700,2250})Box(TEXT("Steel"),FVector(Side*3200,3200,H),FVector(330,370,45));}Box(TEXT("Steel"),FVector(0,1700,1140),FVector(1300,3800,300));Box(TEXT("Rubber"),FVector(0,1700,1300),FVector(1400,3900,30));for(int Side:{-1,1})for(int K=0;K<8;K++)Box(TEXT("CanadaArmor51"),FVector(Side*660,100+K*450,1130),FVector(12,340,140),0,false);

 for(int S:{-1,1}){Box(TEXT("BrickV7"),FVector(S*3700,800,450),FVector(40,6500,900));for(int Y=-2200;Y<=3400;Y+=1400)Box(TEXT("Steel"),FVector(S*3500,Y,450),FVector(60,60,900));}
 Box(TEXT("CorrugatedV7"),FVector(0,800,925),FVector(7500,6600,35));Box(TEXT("BrickV7"),FVector(0,4050,450),FVector(7400,40,900));
 for(int S:{-1,1}){for(int K=0;K<3;K++){FVector P(S*2500,-1100+K*1300,0);Box(TEXT("BrickV7"),P+FVector(0,0,200),FVector(800,600,400));Box(TEXT("Rubber"),P+FVector(0,-308,185),FVector(530,16,240));Box(TEXT("Glow"),P+FVector(0,-320,160),FVector(430,10,115),0,false);Light(P+FVector(0,0,700));}Loot(FVector(S*3200,3200,20));}
 Box(TEXT("Steel"),FVector(0,3400,420),FVector(6000,800,25));for(int K=0;K<24;K++)Box(TEXT("Concrete"),FVector(-3250,900+K*90,(K+1)*9),FVector(450,95,(K+1)*18));
 for(int S:{-1,1})Box(TEXT("Steel"),FVector(S*1500,2990,495),FVector(2900,15,135));for(int K=0;K<3;K++)Cover(FVector(-700+K*700,1200,0));Label(TEXT("ASH CROWN / CASTING HALL"),FVector(0,3900,720),55);Light(FVector(0,500,850));
 }else{
 for(int S:{-1,1}){Box(TEXT("Concrete"),FVector(S*4250,0,280),FVector(70,8500,560));Box(TEXT("Concrete"),FVector(S*2550,-4200,280),FVector(3400,70,560));}Box(TEXT("Concrete"),FVector(0,4200,280),FVector(8500,70,560));
 Room(FVector(-2500,-2200,10),{750,650},TEXT("Concrete"),TEXT("PROCESSING / CELL 04"),0,true,false);Gate(FVector(-2500,-2850,22),State().Stage<=22?false:true);Bed(FVector(-2850,-2000,22));Model(TEXT("ToiletV4"),FVector(-2900,-1700,22));
 Room(FVector(-2500,300,10),{900,1000},TEXT("Concrete"),TEXT("PROPERTY / EVIDENCE"));for(int K=0;K<4;K++)Loot(FVector(-3100+K*380,1000,22),TEXT("EvidenceCabinetV18"));
 Room(FVector(2600,-1600,10),{1100,1300},TEXT("Concrete"),TEXT("DETENTION / RELEASE"),0,true,false);for(int K=0;K<6;K++)Bed(FVector(2150+K%2*650,-2400+K/2*700,22),90);for(int K=0;K<6;K++)Gate(FVector(2150+K%2*650,-2520+K/2*700,22),State().Flags.Contains(TEXT("settler_cells_open52")));Model(TEXT("RelayConsole52"),FVector(3200,-1000,22));
 Room(FVector(0,3100,10),{1350,950},TEXT("Concrete"),TEXT("DIRECTOR / COMMAND"),0,true,false);Desk(FVector(0,3350,22));Room(FVector(-2800,2800,10),{1000,800},TEXT("Concrete"),TEXT("BARRACKS"));Room(FVector(2850,2200,10),{900,1050},TEXT("BrickV7"),TEXT("MESS / STORES"));
 for(int K=0;K<4;K++){Bed(FVector(-3250+K%2*850,2400+K/2*650,22));Model(TEXT("DiningTableV13"),FVector(2800,1550+K*400,22));}Object(ELWObjectKind::Furniture,TEXT("cooker"),FVector(3400,2700,22));
 for(FVector P:{FVector(-800,-600,0),FVector(700,350,0),FVector(-1000,1200,0),FVector(1100,1500,0)})Cover(P);Cover(FVector(1650,-1900,0),90);Cover(FVector(100,1850,0));
 Label(State().Stage>=27?TEXT("THE GATES STAY OPEN"):TEXT("ONE PEOPLE / ONE DIRECTOR"),FVector(0,2130,330),31);for(int S:{-1,1})C->StreetLight(World,FVector(S*3800,-3400,0),FRotator(0,S*90,0),false);
 }
 C->FlushSurfaces();
}
