#include "LWLandmark.h"
#include "LWWorld.h"
#include "Components/StaticMeshComponent.h"
#include "LWWorldTextComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"
void ALWChunk::BuildingLandmark(ALWWorld* W,const LWGen::FSite& S){
 const auto& D=LWLandmarks::Get(S.Type);const int Theme=S.Type-33;const FVector Base=FVector(S.Position,0)-GetActorLocation();const FRotator R(0,S.Yaw,0);auto At=[&](FVector V){return Base+R.RotateVector(V);};
 auto B=[&](FName Mat,FVector V,FVector Size,bool Solid=true){Box(W,Mat,At(V),Size,R,Solid);};auto M=[&](FName Mesh,FVector V,float Yaw=0,FVector Scale=FVector(1)){Add(W,Mesh,NAME_None,At(V),Scale,R+FRotator(0,Yaw,0));};
 auto Label=[&](FString T,FVector V,float Size=28){if(D.Layout==3){const float Width=FMath::Max(200.f,T.Len()*Size*.65f);B(TEXT("Steel"),V+FVector(0,10,Size*.4f),FVector(Width,14,Size*1.5f),false);for(int Side:{-1,1})B(TEXT("Steel"),FVector(V.X+Side*Width*.4f,V.Y+10,(V.Z+28)*.5f),FVector(14,14,V.Z-28));}auto* C=NewObject<ULWWorldTextComponent>(this);C->SetupAttachment(RootComponent);C->SetRelativeLocation(At(V));C->SetRelativeRotation(R+FRotator(0,-90,0));C->SetHorizontalAlignment(EHTA_Center);C->SetWorldSize(Size);C->SetText(FText::FromString(T));C->SetTextRenderColor(FColor(214,200,160));C->SetCullDistance(8000);C->RegisterComponent();};
 auto Light=[&](FVector V){if(D.Layout==3)B(TEXT("Steel"),V-FVector(0,0,245),FVector(24,24,490));else B(TEXT("Steel"),V+FVector(0,0,40),FVector(8,8,80),false);B(TEXT("Bone"),V,FVector(80,40,12),false);auto* L=NewObject<UPointLightComponent>(this);L->SetupAttachment(RootComponent);L->SetRelativeLocation(At(V));L->SetIntensity(18000);L->SetAttenuationRadius(1600);L->SetCastShadows(false);L->SetMaxDrawDistance(4500);L->RegisterComponent();};
 auto Loot=[&](FName Mesh,FVector V,int I){FName Id(*FString::Printf(TEXT("landmark_%d_cache_%d"),Theme,I));W->EnsureSiteContainer(Id,S,GetActorLocation()+At(V),I%3);auto* O=W->SpawnObject(ELWObjectKind::Container,Id,GetActorLocation()+At(V),R);if(O){O->Body->SetStaticMesh(W->Mesh(Mesh));Residents.Add(O);}};
 auto* A=GetWorld()->SpawnActor<ALWLandmark>();A->World=W;A->Chunk=this;A->Site=S;Residents.Add(A);
 auto Device=[&](int I,FVector V,int Choice=0){auto* O=GetWorld()->SpawnActor<ALWLandmarkDevice>(GetActorLocation()+At(V),R);if(!O)return;O->Configure(W,ELWObjectKind::Container,I==2?A->Key(TEXT("reserve")):A->Key(*FString::Printf(TEXT("control_%d_%d"),I,Choice)));O->Landmark=A;O->Index=I;O->Choice=Choice;O->Body->SetStaticMesh(W->Mesh(I==2?TEXT("EvidenceCabinetV18"):TEXT("PoliceDispatchV18")));Residents.Add(O);Label(I==1&&D.Mechanic==2?FString::Printf(TEXT("CHANNEL %d"),Choice+1):FString(D.Steps[I]),V+FVector(0,-90,175),19);};
 const float X=S.Size.X*.5f-250,Y=S.Size.Y*.5f-250;const bool Outdoor=D.Layout==3;FName Wall=Theme==6?TEXT("BrickV7"):Theme==9?TEXT("WindowGlass"):Theme==14?TEXT("CasinoMarble21"):TEXT("Concrete");
 B(TEXT("Concrete"),FVector(0,0,14),FVector(2*X,2*Y,28));B(TEXT("Concrete"),FVector(0,-Y-150,7),FVector(700,300,14));
 // A central route connects the entrance, side rooms, objective gallery and final reserve.
 const float HallX=D.Layout==2?X:2100,HallY=D.Layout==2?800:2100;
 if(!Outdoor){for(int Side:{-1,1})B(Wall,FVector(Side*HallX,0,300),FVector(24,2*HallY,600));B(Wall,FVector(0,HallY,300),FVector(2*HallX,24,600));for(int Side:{-1,1})B(Wall,FVector(Side*(HallX+260)*.5f,-HallY,300),FVector(HallX-260,24,600));B(Wall,FVector(0,-HallY,525),FVector(520,24,150));B(TEXT("Steel"),FVector(0,0,610),FVector(2*HallX+40,2*HallY+40,20));}
 Label(D.Name,FVector(0,-HallY-24,480),38);Device(0,D.Layout==2?FVector(-HallX+450,0,28):FVector(-500,-HallY+240,28));
 if(D.Mechanic==2)Label(FString::Printf(TEXT("INSCRIPTION: %d - %d - %d"),(S.Type+1)%3+1,S.Type%3+1,(S.Type+2)%3+1),FVector(250,-HallY+32,240),24);
 if(D.Mechanic==2){for(int I=0;I<3;I++)Device(1,FVector(-650+I*420,HallY-420,28),I);}else Device(1,FVector(-650,HallY-420,28));
 Device(2,FVector(HallX-430,HallY-440,28));if(D.Mechanic==8||D.Mechanic==9)Device(2,FVector(HallX-800,HallY-440,28),1);
 // Theme-specific furnished side bays, with clear thresholds and no furniture on circulation axes.
 const FName Sets[20][3]={
 {TEXT("PoliceDispatchV18"),TEXT("BookcaseV13"),TEXT("Generator")},{TEXT("Shelf"),TEXT("CabinetV3"),TEXT("SinkV4")},
 {TEXT("PoliceDispatchV18"),TEXT("DeskSetV13"),TEXT("BookcaseV13")},{TEXT("HomeBedV13"),TEXT("SideboardV13"),TEXT("ChairV3")},
 {TEXT("Generator"),TEXT("WorkbenchV4"),TEXT("LockerV4")},{TEXT("EvidenceCabinetV18"),TEXT("PoliceDispatchV18"),TEXT("Generator")},
 {TEXT("ChairV3"),TEXT("BookcaseV13"),TEXT("SideboardV13")},{TEXT("ChairV3"),TEXT("PoliceDispatchV18"),TEXT("Generator")},
 {TEXT("HomeBedV13"),TEXT("BookcaseV13"),TEXT("Generator")},{TEXT("SinkV4"),TEXT("ClinicBedV3"),TEXT("CabinetV3")},
 {TEXT("LockerV4"),TEXT("WorkbenchV4"),TEXT("ChairV3")},{TEXT("PoliceDispatchV18"),TEXT("BookcaseV13"),TEXT("Shelf")},
 {TEXT("PoliceDispatchV18"),TEXT("Generator"),TEXT("WorkbenchV4")},{TEXT("DeskSetV13"),TEXT("BookcaseV13"),TEXT("ChairV3")},
 {TEXT("CasinoCounterV21"),TEXT("BookcaseV13"),TEXT("CasinoChairV21")},{TEXT("WorkbenchV4"),TEXT("Generator"),TEXT("Crate")},
 {TEXT("Crate"),TEXT("LockerV4"),TEXT("ChairV3")},{TEXT("HomeBedV13"),TEXT("NightstandV13"),TEXT("SinkV4")},
 {TEXT("PoliceDispatchV18"),TEXT("WorkbenchV4"),TEXT("BookcaseV13")},{TEXT("CellBunkV18"),TEXT("CabinetV3"),TEXT("SinkV4")}};
 for(int Side:{-1,1})for(int J=0;J<3;J++){FVector V(Side*(HallX-500),-HallY+650+J*(2*HallY-1300)/2,28);if(J==2&&Side>0)continue;
 M(Sets[Theme][J],V,Side>0?180:0);Loot(J%2?TEXT("LockerV4"):TEXT("CabinetV3"),V+FVector(-Side*200,280,0),(Side+1)*3+J);Light(V+FVector(-Side*200,0,490));
 if(!Outdoor&&D.Layout!=2){B(Wall,V+FVector(0,510,220),FVector(780,18,440));B(Wall,V+FVector(-Side*450,-290,220),FVector(18,300,440));}
 }
 // Strong silhouettes and distinct site landmarks; each lies within its reserved parcel.
 if(D.Layout==0){FVector T(-HallX+300,HallY-350,620);for(int I=0;I<3;I++)B(TEXT("Steel"),T+FVector(I==0?-150:I==1?150:0,I==2?160:0,1000),FVector(35,35,2000));for(int Z=0;Z<5;Z++)B(TEXT("Rust"),T+FVector(0,0,Z*370),FVector(420,420,25));
 if(Theme==8){B(TEXT("Bone"),T+FVector(0,0,2000),FVector(550,550,150));auto* Beacon=NewObject<UPointLightComponent>(A);Beacon->SetupAttachment(RootComponent);Beacon->SetRelativeLocation(At(T+FVector(0,0,2120)));Beacon->SetIntensity(160000);Beacon->SetAttenuationRadius(6500);Beacon->SetCastShadows(false);Beacon->SetLightColor(FLinearColor(1,.85f,.45f));Beacon->SetVisibility(A->Stage()==3);Beacon->RegisterComponent();}else{Add(W,TEXT("Sphere"),TEXT("Steel"),At(T+FVector(0,0,2000)),FVector(6,6,1.3),R,false);B(TEXT("Red"),T+FVector(0,0,2220),FVector(30,30,120),false);}}
 if(D.Layout==2){for(int I=-3;I<=3;I++){float CarX=I*1000;for(int Side:{-1,1}){M(I<0?TEXT("ChairV3"):I>1?TEXT("HomeBedV13"):TEXT("DeskSetV13"),FVector(CarX,Side*570,28),Side<0?0:180);if(I<3)B(TEXT("PlasterV7"),FVector(CarX+490,Side*530,250),FVector(18,540,444));}Label(FString::Printf(TEXT("CAR %02d"),I+4),FVector(CarX,-785,480),24);}for(int I=-3;I<=3;I++){B(TEXT("Steel"),FVector(I*1000,0,680),FVector(950,1620,80));for(int Side:{-1,1}){B(TEXT("Rust"),FVector(I*1000,Side*815,190),FVector(920,24,300));B(TEXT("WindowGlass"),FVector(I*1000,Side*818,410),FVector(760,12,140),false);}}for(int Side:{-1,1})B(TEXT("Steel"),FVector(0,Side*960,32),FVector(2*X,20,10));Label(TEXT("EXECUTIVE EVACUATION / CAR 04"),FVector(0,800,480),28);}
 if(Theme==4){for(int I=-2;I<=2;I++){B(TEXT("Concrete"),FVector(I*1350,Y-250,650),FVector(180,500,1300));B(TEXT("Steel"),FVector(I*1350,Y-510,560),FVector(850,36,1080));M(TEXT("Generator"),FVector(I*1350,Y-1100,28));}}
 if(Theme==7){B(TEXT("Bone"),FVector(0,Y-180,1600),FVector(4800,45,2500));B(TEXT("Steel"),FVector(0,Y-160,330),FVector(4800,110,600));for(int I=-2;I<=2;I++)for(int J=0;J<3;J++)B(TEXT("Lane"),FVector(I*1300,-1000+J*700,29),FVector(12,520,1),false);}
 if(Theme==11){for(int I=0;I<3;I++){FVector V(-2800+I*2000,800,28);Add(W,TEXT("Cylinder"),TEXT("Bone"),At(V+FVector(0,0,1000)),FVector(2.5,2.5,20),R);Add(W,TEXT("Sphere"),TEXT("Red"),At(V+FVector(0,0,2000)),FVector(2.5,2.5,5),R);B(TEXT("Steel"),V+FVector(0,0,100),FVector(450,450,200));}}
 if(Theme==15){for(int I=0;I<8;I++)M(TEXT("Rubble"),FVector(-2200+(I%4)*1000,-300+(I/4)*1700,28),I*39,FVector(2));B(TEXT("Rust"),FVector(-2600,0,1200),FVector(80,80,2400));B(TEXT("Steel"),FVector(-1200,0,2350),FVector(3000,80,80));}
 if(Theme==16){Add(W,TEXT("Sphere"),TEXT("Bone"),At(FVector(-800,0,240)),FVector(28,5,4),R+FRotator(0,25,0));B(TEXT("Steel"),FVector(-800,0,300),FVector(1000,3500,30));for(int I=0;I<7;I++)M(TEXT("Rubble"),FVector(-2400+I*650,1500,28),I*32);}
 if(Theme==6){B(TEXT("Wood"),FVector(0,HallY-40,850),FVector(90,45,700));B(TEXT("Wood"),FVector(0,HallY-40,1000),FVector(400,45,80));for(int I=0;I<5;I++)for(int Side:{-1,1})B(TEXT("Wood"),FVector(Side*650,-600+I*250,95),FVector(650,95,130));}
 if(Theme==9||Theme==1){for(int I=0;I<8;I++){FVector V((I%2?1:-1)*1100,-800+(I/2)*500,28);B(TEXT("Wood"),V+FVector(0,0,45),FVector(450,260,90));B(TEXT("Rust"),V+FVector(0,0,95),FVector(420,230,10));for(int J=-1;J<=1;J++)B(TEXT("Wood"),V+FVector(J*110,0,170),FVector(20,20,140),false);}}
 if(Theme==10){for(int Side:{-1,1})for(int I=0;I<3;I++)B(TEXT("Concrete"),FVector(Side*(1300+I*180),500,70+I*80),FVector(180,2200,80+I*160));}
 if(Theme==13||Theme==14){for(int Side:{-1,1})for(int I=0;I<3;I++)M(TEXT("CasinoColumnV21"),FVector(Side*1050,-500+I*700,28));B(TEXT("Wood"),FVector(0,1300,80),FVector(1400,450,100));}
}
void ALWChunk::BuildingAirport(ALWWorld* W,const LWGen::FSite& S){
 const FVector Base=FVector(S.Position,0)-GetActorLocation();const FRotator R(0,S.Yaw,0);auto At=[&](FVector V){return Base+R.RotateVector(V);};MajorBounds+=S.Position-S.Size*.5-FVector2D(1600);MajorBounds+=S.Position+S.Size*.5+FVector2D(1600);
 auto B=[&](FName Mat,FVector V,FVector Size,bool Solid=true){Box(W,Mat,At(V),Size,R,Solid);};auto M=[&](FName Mesh,FVector V,float Yaw=0,FVector Scale=FVector(1)){Add(W,Mesh,NAME_None,At(V),Scale,R+FRotator(0,Yaw,0));};
 auto Label=[&](FString T,FVector V,float Size=32){auto* C=NewObject<ULWWorldTextComponent>(this);C->SetupAttachment(RootComponent);C->SetRelativeLocation(At(V));C->SetRelativeRotation(R+FRotator(0,-90,0));C->SetHorizontalAlignment(EHTA_Center);C->SetWorldSize(Size);C->SetText(FText::FromString(T));C->SetCullDistance(12000);C->RegisterComponent();};int Serial=0;
 auto Loot=[&](FName Mesh,FVector V,int Lock=0){FName Id(*FString::Printf(TEXT("airport_%u_%d"),S.Id,Serial++));W->EnsureSiteContainer(Id,S,GetActorLocation()+At(V),Lock);auto* O=W->SpawnObject(ELWObjectKind::Container,Id,GetActorLocation()+At(V),R);if(O){O->Body->SetStaticMesh(W->Mesh(Mesh));Residents.Add(O);}};
 auto Light=[&](FVector V){B(TEXT("Bone"),V,FVector(150,40,12),false);auto* L=NewObject<UPointLightComponent>(this);L->SetupAttachment(RootComponent);L->SetRelativeLocation(At(V));L->SetIntensity(28000);L->SetAttenuationRadius(2000);L->SetCastShadows(false);L->SetMaxDrawDistance(5000);L->RegisterComponent();};
 // Landside approaches, drop-off lanes, parking, apron, taxiway and runway are separate surfaces.
 B(TEXT("Asphalt"),FVector(0,0,-8),FVector(29800,21800,24));B(TEXT("Concrete"),FVector(0,-9300,6),FVector(15000,800,12));B(TEXT("Lane"),FVector(0,-10500,5),FVector(600,800,2),false);
 B(TEXT("Asphalt"),FVector(0,7200,6),FVector(27800,2100,4));for(int I=-13;I<=13;I++){B(TEXT("Lane"),FVector(I*950,7200,9),FVector(450,24,2),false);for(int Side:{-1,1}){B(TEXT("Lane"),FVector(I*950,Side*1050+7200,9),FVector(600,14,2),false);B(TEXT("Bone"),FVector(I*950,Side*1160+7200,14),FVector(24,24,24),false);}}
 Label(TEXT("RICHARDSON INTERNATIONAL AIRPORT"),FVector(0,-8100,1750),100);Label(TEXT("ARRIVALS  /  DEPARTURES"),FVector(0,-8118,390),38);
 for(int I=-7;I<=7;I++)for(int J=0;J<3;J++){B(TEXT("Lane"),FVector(I*800,-9800+J*300,5),FVector(650,10,2),false);if(J==0&&I%3==0)Loot(TEXT("Crate"),FVector(I*800,-9800,8));}
 // Three traversable terminal decks. The west stair annexe has open flight volumes on every level.
 const float SX=-6900;for(int F=0;F<3;F++){const float Z=28+F*520;B(TEXT("TileV7"),FVector(300,-4800,Z-10),FVector(13200,6400,20));B(TEXT("Steel"),FVector(SX,-1880,Z-10),FVector(1200,400,20));if(F==0)B(TEXT("TileV7"),FVector(SX,-5000,18),FVector(1200,6600,20));
 for(int Side:{-1,1})B(TEXT("Concrete"),FVector(Side*7500,-4800,Z+250),FVector(30,6400,500));
 for(int Side:{-1,1}){float Y=Side<0?-8000:-1600;for(int I=-6;I<=6;I++){float X=I*1000;bool Door=F==0&&Side<0&&FMath::Abs(X)<1500;if(!Door)B(TEXT("WindowGlass"),FVector(X,Y,Z+240),FVector(970,18,440));B(TEXT("Steel"),FVector(X+490,Y,Z+250),FVector(25,35,500));}}
 for(int I=-5;I<=5;I++){Light(FVector(I*1150,-4600,Z+455));M(TEXT("CasinoColumnV21"),FVector(I*1150,-3400,Z));}
 for(int I=-5;I<=5;I++){FVector V(I*1050,-6900,Z);if(F==0){M(TEXT("CasinoCounterV21"),V);Loot(TEXT("CabinetV3"),V+FVector(240,0,0));Label(FString::Printf(TEXT("CHECK-IN %02d"),I+6),V+FVector(0,-180,260),26);}else if(F==1){for(int J=0;J<3;J++)M(TEXT("ChairV3"),V+FVector(J*190,0,0));Loot(TEXT("Crate"),V+FVector(0,400,0));Label(FString::Printf(TEXT("GATE A%02d"),I+6),V+FVector(0,-100,340),30);}else{M(TEXT("DeskSetV13"),V);Loot(TEXT("LockerV4"),V+FVector(280,0,0),2);Label(I<0?TEXT("AIRLINE OPERATIONS"):TEXT("EXECUTIVE LOUNGE"),V+FVector(0,-180,320),24);}}
 // Functional shops, kitchens, security lockers and baggage storage flank the main concourse.
 for(int I=-4;I<=4;I++){FVector V(I*1400,-2600,Z);B(TEXT("PlasterV7"),V+FVector(630,0,230),FVector(22,1500,460));Label(F==0?(I<0?TEXT("BAGGAGE CLAIM"):TEXT("SECURITY")):F==1?(I%2?TEXT("CAFE"):TEXT("DUTY FREE")):TEXT("CREW QUARTERS"),V+FVector(0,-500,370),28);
 Loot(F==0?TEXT("EvidenceCabinetV18"):F==1?TEXT("Shelf"):TEXT("LockerV4"),V+FVector(-370,350,0),F==2?2:0);
 M(F==0?TEXT("PoliceDispatchV18"):F==1?TEXT("SinkV4"):TEXT("HomeBedV13"),V+FVector(250,250,0));if(F==1){M(TEXT("DiningTableV13"),V+FVector(0,-150,0));M(TEXT("ChairV3"),V+FVector(-220,-150,0));}if(F==0)B(TEXT("Steel"),V+FVector(0,-200,70),FVector(700,250,110));}
 Label(FString::Printf(TEXT("LEVEL %d   STAIRS WEST"),F+1),FVector(-5400,-4300,Z+300),32);
 if(F<2){for(int J=0;J<32;J++){float H=(J+1)*520.f/32;B(TEXT("Concrete"),FVector(SX,-3500+J*45,Z+H-8),FVector(650,45,16));}B(TEXT("Steel"),FVector(SX,-1870,Z+520-10),FVector(650,420,20));}
 }
 B(TEXT("Steel"),FVector(0,-4800,1600),FVector(15060,6460,60));
 // Jet bridges meet the ground apron using wide walkable stairs rather than floating cabin entrances.
 for(int I=-2;I<=2;I++){float X=I*2800;B(TEXT("Concrete"),FVector(X,-800,30),FVector(650,1600,60));for(int Side:{-1,1})B(TEXT("Steel"),FVector(X+Side*320,-800,170),FVector(16,1600,220));Label(FString::Printf(TEXT("STAND %d"),I+3),FVector(X,-300,270),35);}
 // Original airliner silhouette: tapered fuselage, swept wings, nacelles, cockpit and tail surfaces.
 auto Plane=[&](FVector V,float Yaw){FRotator Q=R+FRotator(0,Yaw,0);auto P=[&](FVector A){return At(V+FRotator(0,Yaw,0).RotateVector(A));};auto Part=[&](FName Mesh,FName Mat,FVector A,FVector Scale){Add(W,Mesh,Mat,P(A),Scale,Q);};
 Part(TEXT("Sphere"),TEXT("Bone"),FVector(0,0,400),FVector(40,6,6));Part(TEXT("Sphere"),TEXT("WindowGlass"),FVector(1500,0,470),FVector(7,5.3,3));
 for(int Side:{-1,1}){Box(W,TEXT("Bone"),P(FVector(-150,Side*750,350)),FVector(1550,1600,38),Q+FRotator(0,Side*22,0));Part(TEXT("Sphere"),TEXT("Steel"),FVector(50,Side*750,235),FVector(8,2.8,2.8));Box(W,TEXT("Steel"),P(FVector(-1550,Side*350,550)),FVector(650,900,28),Q+FRotator(0,Side*20,0));for(int I=-9;I<=9;I++)Part(TEXT("Sphere"),TEXT("WindowGlass"),FVector(I*115,Side*285,460),FVector(.7,.12,.6));for(int I:{-800,1100}){Box(W,TEXT("Steel"),P(FVector(I,Side*160,160)),FVector(28,28,230),Q);Part(TEXT("Sphere"),TEXT("Steel"),FVector(I,Side*160,68),FVector(1.4,.7,1.4));}}
 Box(W,TEXT("Bone"),P(FVector(-1500,0,850)),FVector(620,40,850),Q+FRotator(-20,0,0));};
 for(int I=-2;I<=2;I++)Plane(FVector(I*5200,2800,0),90);Plane(FVector(10500,-4000,0),0);
 // Freight hangar with lootable cargo lanes and maintenance equipment.
 B(TEXT("Concrete"),FVector(11100,-5200,20),FVector(5300,5500,40));for(int Side:{-1,1})B(TEXT("CorrugatedV7"),FVector(11100+Side*2650,-5200,500),FVector(30,5500,1000));B(TEXT("CorrugatedV7"),FVector(11100,-7950,500),FVector(5300,30,1000));B(TEXT("Steel"),FVector(11100,-5200,1020),FVector(5360,5560,40));Label(TEXT("FREIGHT / MAINTENANCE"),FVector(11100,-2430,880),48);
 for(int I=0;I<16;I++)Loot(I%2?TEXT("Shelf"):TEXT("Crate"),FVector(9000+(I%4)*1200,-7300+(I/4)*750,40),I%4==0?3:0);
 // Tower silhouette and apron service station.
 B(TEXT("Concrete"),FVector(-11800,-3600,1600),FVector(1100,1100,3200));B(TEXT("Steel"),FVector(-11800,-3600,3220),FVector(2000,2000,60));B(TEXT("WindowGlass"),FVector(-11800,-3600,3480),FVector(1960,1960,460));B(TEXT("Steel"),FVector(-11800,-3600,3750),FVector(2100,2100,70));Label(TEXT("CONTROL"),FVector(-11800,-4180,2300),75);
 for(int I=0;I<4;I++){M(TEXT("FuelTank24"),FVector(-13000+I*800,-1000,12));Loot(TEXT("WorkbenchV4"),FVector(-13000+I*800,-1700,12),1);}
}
