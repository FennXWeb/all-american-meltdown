#include "LWWorld.h"
#include "Components/StaticMeshComponent.h"
#include "LWWorldTextComponent.h"
#include "Components/PointLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

void ALWChunk::StreetLight(ALWWorld* W,FVector P,FRotator R,bool Neon){
 auto At=[&](FVector V){return P+R.RotateVector(V);};
 if(!Neon){
 Box(W,TEXT("Concrete"),At(FVector(0,0,15)),FVector(60,60,30),R);
 Box(W,TEXT("Steel"),At(FVector(0,0,295)),FVector(14,14,570),R);
 Box(W,TEXT("Steel"),At(FVector(0,-65,580)),FVector(16,150,16),R);
 Box(W,TEXT("Steel"),At(FVector(0,-138,570)),FVector(75,110,24),R);
 Box(W,TEXT("LampWarm20"),At(FVector(0,-138,556)),FVector(58,88,4),R,false);
 }else{
 Box(W,TEXT("Steel"),P,FVector(400,28,135),R);
 for(int Sign:{-1,1})Box(W,TEXT("NeonPink20"),At(FVector(0,-18,Sign*57)),FVector(385,5,5),R,false);
 auto* T=NewObject<ULWWorldTextComponent>(this);T->SetupAttachment(RootComponent);T->SetRelativeLocation(At(FVector(0,-20,-20)));T->SetRelativeRotation(R+FRotator(0,-90,0));T->SetHorizontalAlignment(EHTA_Center);T->SetText(FText::FromString(TEXT("OPEN LATE")));T->SetWorldSize(45);T->SetTextRenderColor(FColor(255,70,130));T->RegisterComponent();
 }
 auto* L=NewObject<UPointLightComponent>(this);L->SetupAttachment(RootComponent);L->SetRelativeLocation(At(Neon?FVector(0,-65,0):FVector(0,-138,540)));L->SetIntensity(0);L->SetAttenuationRadius(Neon?650:1250);L->SetCastShadows(false);L->SetLightColor(Neon?FLinearColor(1,.12f,.3f):FLinearColor(1,.66f,.3f));L->RegisterComponent();StreetLights.Add(L);LightingWorld=W;
}
void ALWChunk::Tick(float Dt){
 Super::Tick(Dt);TickTraffic33();if(!LightingWorld)return;auto* P=UGameplayStatics::GetPlayerPawn(this,0);
 const float Night=1-FMath::Clamp(FMath::Min(LightingWorld->TimeOfDay-5.5f,19.5f-LightingWorld->TimeOfDay),0.f,1.f);
 for(auto& L:StreetLights)if(L){const bool Near=P&&FVector::DistSquared(P->GetActorLocation(),L->GetComponentLocation())<FMath::Square(6500.f);L->SetIntensity(Near?Night*18000:0);}
}
void ALWChunk::BuildingTower(ALWWorld* W,const LWGen::FSite& S){
 const FVector Origin=FVector(S.Position,12)-GetActorLocation();const FRotator R(0,S.Yaw,0);FRandomStream Rand(S.Id);int Serial=0;
 auto At=[&](FVector V){return Origin+R.RotateVector(V);};
 auto B=[&](FName M,FVector V,FVector Size,bool Hit=true){Box(W,M,At(V),Size,R,Hit);};
 auto Model=[&](FName M,FVector V,float Yaw=0){Add(W,M,NAME_None,At(V),FVector(1),R+FRotator(0,Yaw,0));};
 auto Object=[&](ELWObjectKind Kind,FVector V,float Yaw=0){auto* O=W->SpawnObject(Kind,FName(*FString::Printf(TEXT("tower_%u_%d"),S.Id,Serial++)),GetActorLocation()+At(V),R+FRotator(0,Yaw,0));if(O)Residents.Add(O);return O;};
 auto Loot=[&](FName M,FVector V,int Lock=0){if(auto* O=Object(ELWObjectKind::Container,V)){W->EnsureSiteContainer(O->RecordId,S,O->GetActorLocation(),Lock);O->Body->SetStaticMesh(W->Mesh(M));}};
 auto Text=[&](FString Str,FVector V,float Size,float Yaw=-90){auto* T=NewObject<ULWWorldTextComponent>(this);T->SetupAttachment(RootComponent);T->SetRelativeLocation(At(V));T->SetRelativeRotation(R+FRotator(0,Yaw,0));T->SetHorizontalAlignment(EHTA_Center);T->SetText(FText::FromString(Str));T->SetWorldSize(Size);T->RegisterComponent();};
 auto Glass=[&](FVector V,FVector Size){const FName Key(*FString::Printf(TEXT("tower_glass_%u_%d_%d_%d"),S.Id,int(V.X),int(V.Y),int(V.Z)));if(auto* O=W->SpawnObject(ELWObjectKind::Window,Key,GetActorLocation()+At(V),R)){O->Body->SetRelativeScale3D(Size/100);Residents.Add(O);}};
 const int Variant=S.Type==56?1:S.Type==57?2:0;
 const int Floors=FMath::Clamp(S.Floors,12,24),Access=FMath::Clamp(S.AccessibleFloors,0,Floors);const float X=1800,Y=1600,FH=400;
 B(TEXT("Concrete"),FVector(0,0,-18),FVector(3900,3500,30));
 // Curtain wall panels, projecting columns and spandrels produce a complete silhouette.
 for(int F=0;F<Floors;F++){
 const float Z=F*FH;
 for(int Side:{-1,1}){
 for(int I=0;I<9;I++){float XX=-1600+I*400;
 if(!(Side==-1&&I==4&&F==0)){Glass(FVector(XX,Side*Y,Z+200),FVector(382,22,280));B(TEXT("Steel"),FVector(XX,Side*Y,Z+30),FVector(400,35,60));}
 B(TEXT("Concrete"),FVector(XX-195,Side*Y,Z+200),FVector(24,55,400));}
 for(int I=0;I<8;I++){float YY=-1400+I*400;Glass(FVector(Side*X,YY,Z+200),FVector(22,382,280));B(TEXT("Concrete"),FVector(Side*X,YY-195,Z+200),FVector(55,24,400));}
 B(TEXT("Steel"),FVector(0,Side*Y,Z+365),FVector(3600,50,70));B(TEXT("Steel"),FVector(Side*X,0,Z+365),FVector(50,3200,70));}
 for(int Side:{-1,1})B(TEXT("Steel"),FVector(Side*X,0,Z+30),FVector(50,3200,60));
 if(F>=Access){B(TEXT("Concrete"),FVector(0,0,Z+10),FVector(3600,3200,16));continue;}
 // Guard the floor opening without blocking the front and rear landings.
 B(TEXT("Steel"),FVector(945,-50,Z+65),FVector(10,680,130));
 // Floor ends at x=950; the east stairwell remains open above both flights.
 B(TEXT("Concrete"),FVector(-425,0,Z),FVector(2750,3200,24));
 B(Variant==1?TEXT("Wood"):TEXT("TileV7"),FVector(-425,0,Z+15),FVector(2750,3200,6));
 B(TEXT("Concrete"),FVector(1375,-990,Z+10),FVector(850,1220,16));
 B(TEXT("Concrete"),FVector(1375,950,Z+10),FVector(850,1300,16));
 // Rear landing joins the two flights. 12 risers per half-floor, 30cm treads.
 if(F<Access-1){
 for(int I=0;I<12;I++){float Top=18+(I+1)*FH/24;B(TEXT("Concrete"),FVector(1150,-385+(I+.5f)*30,Z+Top-10),FVector(370,30,20));
 float Upper=18+FH*.5f+(I+1)*FH/24;B(TEXT("Concrete"),FVector(1580,-25-(I+.5f)*30,Z+Upper-10),FVector(370,30,20));}
 B(TEXT("Concrete"),FVector(1375,145,Z+FH*.5f+8),FVector(850,340,20));
 B(TEXT("Steel"),FVector(1365,-210,Z+190),FVector(12,360,180));
 }
 // Floor circulation: wide lobby, two office suites, and a rear records/kitchen zone.
 for(int Side:{-1,1}){
 const float CY=Side*850;
 if(Variant==1){
 const float Ends[]={-1800,-1380,-1220,-780,-620,-180,-20,790};
 for(int Piece=0;Piece<4;Piece++){float A=Ends[Piece*2],BEnd=Ends[Piece*2+1];B(TEXT("PlasterV7"),FVector((A+BEnd)*.5f,CY,Z+210),FVector(BEnd-A,18,384));}
 for(int Room=0;Room<3;Room++){float Center=-1300+Room*600;Object(ELWObjectKind::Door,FVector(Center-80,CY,Z+18));B(TEXT("PlasterV7"),FVector(Center,CY,Z+326),FVector(160,18,148));Text(FString::Printf(TEXT("STUDIO %d"),F*10+Room+1+(Side>0?3:0)),FVector(Center,CY-Side*14,Z+285),19,Side<0?90:-90);
 if(auto* Fixture=Object(ELWObjectKind::Furniture,FVector(Center+80,Side*1470,Z+18),Side<0?180:0))Fixture->SetFurniture(TEXT("sink"));if(auto* Fixture=Object(ELWObjectKind::Furniture,FVector(Center-120,Side*1470,Z+18),Side<0?180:0))Fixture->SetFurniture(TEXT("cooker"));}
 for(float Divider:{-1000.f,-400.f})B(TEXT("PlasterV7"),FVector(Divider,Side*1225,Z+210),FVector(18,750,384));
 }else{
 B(TEXT("PlasterV7"),FVector(-850,CY,Z+210),FVector(1850,18,384));
 if(auto* O=Object(ELWObjectKind::Door,FVector(75,CY,Z+18)))O->Body->SetMaterial(0,W->Material(TEXT("DoorPaintV7")));
 B(TEXT("PlasterV7"),FVector(515,CY,Z+210),FVector(550,18,384));
 B(TEXT("PlasterV7"),FVector(155,CY,Z+326),FVector(160,18,148));
 }
 for(int I=0;I<3;I++){FVector Desk(-1300+I*600,Side*1180,Z+18);if(Variant==0)Model(TEXT("DeskSetV13"),Desk);else{if(auto* Bed=Object(ELWObjectKind::Furniture,Desk)){Bed->SetFurniture(TEXT("bed"));Bed->Body->SetStaticMesh(W->Mesh(Variant==1?TEXT("HomeBedV13"):TEXT("ClinicBedV3")));}}
 Loot(Variant==2?TEXT("CabinetV3"):TEXT("NightstandV13"),Desk+FVector(240,0,0),F%3);if(auto* O=Object(ELWObjectKind::Furniture,Desk+(Variant?FVector(200,-200,0):FVector(0,-120,0)),90))O->SetFurniture(TEXT("chair"));}
 Loot(TEXT("BookcaseV13"),FVector(-1570,Side*1450,Z+18),F>4?2:0);
 }
 if(F==0){Loot(TEXT("ShopCounterV18"),FVector(-600,-350,18));Model(TEXT("PlazaPlanterV18"),FVector(-1450,-400,18));Text(LWPlaces::Name(S.Type),FVector(-550,400,240),55);}
 else {Model(Variant==2?TEXT("PoliceDispatchV18"):TEXT("DiningTableV13"),FVector(-850,0,Z+18));Loot(TEXT("CabinetV3"),FVector(-1550,500,Z+18),F%4==0?2:0);}
 for(int Side:{-1,1}){Model(TEXT("PlazaPlanterV18"),FVector(-1550,Side*650,Z+18));Loot(Variant==2?TEXT("EvidenceCabinetV18"):TEXT("LockerV4"),FVector(620,Side*1300,Z+18),Variant==2?2:0);Model(TEXT("SinkV4"),FVector(-1350,Side*400,Z+18));Model(TEXT("ToiletV4"),FVector(-1650,Side*400,Z+18));B(TEXT("PlasterV7"),FVector(-1100,Side*475,Z+210),FVector(18,540,384));B(TEXT("Wood"),FVector(-1087,Side*475,Z+220),FVector(8,200,150));Text(Variant==1?TEXT("RENT DUE"):Variant==2?TEXT("WARD SCHEDULE"):TEXT("EVACUATION PLAN"),FVector(-1082,Side*475,Z+235),16,0);Text(Variant==1?TEXT("RESIDENT SERVICES"):Variant==2?TEXT("PATIENT WARD"):TEXT("OFFICES / RECORDS"),FVector(-850,Side*850-15,Z+285),20);}
 for(int Bay=0;Bay<3;Bay++){B(TEXT("Steel"),FVector(-800+Bay*600,0,Z+370),FVector(380,160,8));B(TEXT("Bone"),FVector(-800+Bay*600,0,Z+360),FVector(330,120,5),false);}
 Text(FString::Printf(TEXT("FLOOR %02d / STAIRS >"),F+1),FVector(250,830,Z+265),28);
 Model(TEXT("CeilingLightV13"),FVector(-300,0,Z+355));
 auto* Light=NewObject<UPointLightComponent>(this);Light->SetupAttachment(RootComponent);Light->SetRelativeLocation(At(FVector(-300,0,Z+320)));Light->SetIntensity(7500);Light->SetAttenuationRadius(1100);Light->SetCastShadows(false);Light->SetMaxDrawDistance(4500);Light->RegisterComponent();
 }
 // A blocked tower is physically sealed; partial towers stop at a structural collapse.
 if(Access==0){B(TEXT("Concrete"),FVector(0,-Y,180),FVector(410,80,360));for(int I=0;I<8;I++)Model(TEXT("Rubble"),FVector(Rand.FRandRange(-650,650),-Y-120-Rand.FRandRange(0,220),12));Text(TEXT("CONDEMNED"),FVector(0,-Y-48,260),45);}
 else{
 // Fitted 160cm entrance with side glazing and a canopy.
 for(int Side:{-1,1})Glass(FVector(Side*140,-Y,130),FVector(120,22,260));B(TEXT("Steel"),FVector(0,-Y,320),FVector(400,30,160));Object(ELWObjectKind::Door,FVector(-80,-Y,18));
 B(TEXT("Steel"),FVector(0,-Y-130,350),FVector(1000,320,25));
 if(Access<Floors){B(TEXT("Concrete"),FVector(0,0,Access*FH),FVector(3600,3200,45));Text(TEXT("UPPER FLOORS COLLAPSED"),FVector(1320,420,(Access-1)*FH+220),22);for(int I=0;I<4;I++)Model(TEXT("Rubble"),FVector(1100+I*150,700,(Access-1)*FH+18));}
 }
 B(TEXT("Concrete"),FVector(0,0,Floors*FH),FVector(3700,3300,35));
 for(int I=0;I<3;I++){B(TEXT("Steel"),FVector(-800+I*800,500,Floors*FH+100),FVector(500,650,170));for(int J=0;J<5;J++)B(TEXT("Concrete"),FVector(-800+I*800,260+J*110,Floors*FH+190),FVector(470,25,10));}
 StreetLight(W,At(FVector(-1500,-Y-130,0)),R);StreetLight(W,At(FVector(1100,-Y-70,400)),R,true);
 W->SpawnEnemies(this,&S,S.Size,FMath::Max(1,Access),FH);
}
