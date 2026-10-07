#include "LWAircraft84.h"
#include "LWCampaign76.h"
#include "LWSyracuse73.h"
#include "LWStreaming68.h"
#include "LWInteriors65.h"
#include "LWTradingCards36.h"
#include "TimerManager.h"
#include "LWWorld.h"
#include "LWPOIExpansion.h"
#include "LWExpansion57.h"
#include "LWPOISurvivors.h"
#include "Misc/ScopeExit.h"
#include "LWZombie.h"
#include "LWInteractable.h"
#include "LWCardGame.h"
#include "LWResident.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "LWWorldTextComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"

// Buildings are assembled from circulation, room, facade and furnishing modules.
// All coordinates are local to a persistent parcel; prop IDs are deterministic.
void ALWChunk::Building(ALWWorld* W,const LWGen::FSite& Input){
 if(LWCampaign76::Dress(this,W,Input))return;
 if(Input.Type>=70&&Input.Type<80){LWSyracuse73::Build(this,W,Input);return;}
 if(Input.Type==68||Input.Type==69){BuildingNewYork69(W,Input);return;}
 LWInteriors65::FScope Interior65(this,W,Input);
 auto S=Input;
 // The same seeded selection controls the survivor cast and generic enemy exclusion.
 const bool NormalStop=S.Type<64&&!LWPlaces::Expanded(S.Type)&&LWPOISurvivors::Selected(S,W->Seed);
 if(NormalStop)S.Friendly=true;
 // Spawn only after the normal builder has authored its floor and furnishings.
 ON_SCOPE_EXIT { W->PendingCards37.Add({this,S});if(NormalStop)LWPOISurvivors::Spawn(this,W,S); };
 if(S.Type>=64&&S.Type<68){LWExpansion57::Build(this,W,S);return;}
 if(LWPlaces::Expanded(S.Type)){LWPOIExpansion::Build(this,W,S);return;}
 if(S.Type==32){LWAviation84::BuildAirport(this,W,S);return;}if(LWLandmarks::Unique(S.Type)){BuildingLandmark(W,S);return;}
 if(LWDungeons::IsDungeon(S.Type)){BuildingDungeon(W,S);return;}
 if(S.Type==21){BuildingCasino(W,S);return;}
 if(LWPlaces::Underground(S.Type)){BuildingUnderground(W,S);return;}
 if(LWPlaces::IsTower(S.Type)){BuildingTower(W,S);return;}
 if(S.Type==5||S.Type==11||S.Type==17){BuildingV18(W,S);return;}
 const FVector C=FVector(S.Position,12)-GetActorLocation();const FRotator R(0,S.Yaw,0);FRandomStream Rand(S.Id);
 auto P=[&](FVector V){return C+R.RotateVector(V);};
 auto B=[&](FName M,FVector V,FVector Size,bool Collision=true){Box(W,M,P(V),Size,R,Collision);};
 FVector2D Footprint=S.Size;if(S.Type==7)Footprint=FVector2D(FMath::Min(S.Size.X,1500.0),FMath::Min(S.Size.Y,1300.0));if(S.Type==8)Footprint=FVector2D(FMath::Min(S.Size.X,1600.0),FMath::Min(S.Size.Y,1500.0));
 TArray<FBox> Occupied;int LootIndex=0;
 auto Fits=[&](FName M,FVector V,float Yaw,FVector Scale=FVector(1)){
 auto* Mesh=W->Mesh(M);if(!Mesh)return false;
 FBox Bounds=Mesh->GetBoundingBox().TransformBy(FTransform(FRotator(0,Yaw,0),V,Scale)).ExpandBy(5);
 if(Bounds.Min.X<-Footprint.X*.5+25||Bounds.Max.X>Footprint.X*.5-25||Bounds.Min.Y<-Footprint.Y*.5+25||Bounds.Max.Y>Footprint.Y*.5-25)return false;
 for(const auto& O:Occupied)if(O.Intersect(Bounds))return false;Occupied.Add(Bounds);return true;};
 auto Loot=[&](FName M,FVector V,float Yaw,FVector Scale=FVector(1)){
 FName Id(*FString::Printf(TEXT("site_%u_v8_loot_%d"),S.Id,LootIndex++));
 W->EnsureSiteContainer(Id,S,GetActorLocation()+P(V));
 if(auto* O=W->SpawnObject(ELWObjectKind::Container,Id,GetActorLocation()+P(V),R+FRotator(0,Yaw,0))){O->Body->SetStaticMesh(W->Mesh(M));O->SetActorScale3D(Scale);Residents.Add(O);}};
 auto A=[&](FName M,FVector V,float Yaw=0,FVector Scale=FVector(1)){
 bool Storage=M==TEXT("RoundRack53")||M==TEXT("GarmentRail53")||M==TEXT("DisplayTable53")||M==TEXT("SalesCounter53")||M==TEXT("PalletRack53")||M==TEXT("LoadedPallet53")||M==TEXT("PackingBench53")||M==TEXT("Crate")||M==TEXT("Shelf")||M==TEXT("CabinetV3")||M==TEXT("Desk")||M==TEXT("Barrel")||M==TEXT("ServiceBenchV3")||M==TEXT("ClothesRackV9")||M==TEXT("SideboardV13")||M==TEXT("NightstandV13")||M==TEXT("BookcaseV13");
 // Outside service barrels are placed explicitly; interior models reserve their full bounds.
 if(FMath::Abs(V.X)<Footprint.X*.5&&FMath::Abs(V.Y)<Footprint.Y*.5){if((M==TEXT("Crate")||M==TEXT("Barrel"))&&Rand.FRand()<.18f)return false;if(!Fits(M,V,Yaw,Scale))return false;}
 if(Storage)Loot(M,V,Yaw,Scale);else Add(W,M,NAME_None,P(V),Scale,R+FRotator(0,Yaw,0));return true;};
 // Dressing follows the supporting mesh, so plates, papers and lamps never float.
 auto Deco=[&](FName M,FVector V,float Yaw=0,FVector Scale=FVector(1)){Add(W,M,NAME_None,P(V),Scale,R+FRotator(0,Yaw,0),false);};
 int N=0;auto Prop=[&](ELWObjectKind K,FVector V,float Yaw=0,FVector Scale=FVector(1)){
 if(K==ELWObjectKind::Car&&Plan68){const FName Id(*FString::Printf(TEXT("site_%u_v7_%d"),S.Id,N++));const FVector Position=GetActorLocation()+P(V);const FRotator Rotation=R+FRotator(0,Yaw,0);Plan68->Population.Add([this,W,Id,Position,Rotation,Scale](){if(auto* O=W->SpawnObject(ELWObjectKind::Car,Id,Position,Rotation)){O->SetActorScale3D(Scale);Residents.Add(O);}});return static_cast<ALWWorldObject*>(nullptr);}
 auto* O=W->SpawnObject(K,FName(*FString::Printf(TEXT("site_%u_v7_%d"),S.Id,N++)),GetActorLocation()+P(V),R+FRotator(0,Yaw,0));if(O){O->SetActorScale3D(Scale);Residents.Add(O);}return O;};
 auto Furn=[&](FName Type,FVector V,float Yaw=0){
 FName M=Type==TEXT("bed")?TEXT("HomeBedV13"):Type==TEXT("chair")?TEXT("ChairV3"):Type==TEXT("water")?TEXT("FridgeV4"):Type==TEXT("workbench")?TEXT("WeaponBench39"):Type==TEXT("cooker")?TEXT("StoveV4"):Type==TEXT("sink")?TEXT("SinkV4"):TEXT("LockerV4");
 if(!Fits(M,V,Yaw))return;
 if(Type==TEXT("locker")||Type==TEXT("water")){Loot(M,V,Yaw);return;}
 auto* O=Prop(ELWObjectKind::Furniture,V,Yaw);if(O){O->SetFurniture(Type);O->Body->SetStaticMesh(W->Mesh(M));}};
 auto Label=[&](FString T,FVector V,float Yaw=-90,float Size=24){auto* L=NewObject<ULWWorldTextComponent>(this);L->SetupAttachment(RootComponent);L->SetRelativeLocation(P(V));L->SetRelativeRotation(R+FRotator(0,Yaw,0));L->SetText(FText::FromString(T));L->SetHorizontalAlignment(EHTA_Center);L->SetWorldSize(Size);L->SetTextRenderColor(FColor(220,201,155));L->RegisterComponent();};
 auto Light=[&](FVector V){auto* L=NewObject<UPointLightComponent>(this);L->SetupAttachment(RootComponent);L->SetRelativeLocation(P(V));L->SetIntensity(16000);L->SetAttenuationRadius(1300);L->SetLightColor(FLinearColor(.85,.72,.5));L->SetCastShadows(false);L->RegisterComponent();Deco(TEXT("CeilingLightV13"),V+FVector(0,0,16));B(TEXT("Steel"),V+FVector(0,0,21),FVector(6,6,24),false);};
 float X=Footprint.X*.5,Y=Footprint.Y*.5;int T=S.Type;bool Home=T>=6&&T<=9,Outdoor=T==15||T==16;int Floors=T==6?3:T==9?2:1;float H=T==10||T==12||T==13||T==3?560:330;
 FName Exterior=T==18?TEXT("BoutiqueStoneV9"):T==8?TEXT("DoorWoodV7"):T==13||T==3?TEXT("CorrugatedV7"):Home?TEXT("BrickV7"):TEXT("PlasterV7");FName Interior=T==19?TEXT("DoorWoodV7"):T==18?TEXT("BoutiqueWallV9"):Home?TEXT("WallpaperV7"):TEXT("PlasterV7");FName Floor=T==19?TEXT("ParquetV7"):T==18?TEXT("BoutiqueStoneV9"):Home?TEXT("ParquetV7"):TEXT("TileV7");
 auto Door=[&](FVector V,float Yaw=0){auto* O=Prop(ELWObjectKind::Door,V,Yaw);if(O)O->Body->SetMaterial(0,W->Material(Home?TEXT("DoorWoodV7"):TEXT("DoorPaintV7")));};
 auto Wall=[&](FVector V,float Length,float Yaw,bool Opening,FName Mat,float Height=330){Height+=30;Occupied.Add(FBox(FVector(-Length*.5,-18,0),FVector(Length*.5,18,Height)).TransformBy(FTransform(FRotator(0,Yaw,0),V)));if(Opening)Occupied.Add(FBox(FVector(-110,-130,0),FVector(110,130,236)).TransformBy(FTransform(FRotator(0,Yaw,0),V)));auto Segment=[&](float Along,float Width,float Z,float Tall){Box(W,Mat,P(V+FRotator(0,Yaw,0).RotateVector(FVector(Along,0,Z))),FVector(Width,18,Tall),R+FRotator(0,Yaw,0));};
 if(!Opening){Segment(0,Length,Height*.5,Height);for(int Face:{-1,1})Box(W,TEXT("Wood"),P(V+FRotator(0,Yaw,0).RotateVector(FVector(0,Face*11,7))),FVector(Length,4,14),R+FRotator(0,Yaw,0),false);return;}for(int Side:{-1,1})Segment(Side*(Length+160)*.25,(Length-160)*.5,Height*.5,Height);Segment(0,160,(Height+236)*.5,Height-236);Door(V+FRotator(0,Yaw,0).RotateVector(FVector(-80,0,0)),Yaw);
 };
 auto Shelf=[&](FVector V,float Yaw=0){if(Rand.FRand()<.08f||!Fits(TEXT("Shelf"),V,Yaw))return;Loot(TEXT("Shelf"),V,Yaw);for(float Level:{13.f,65.f,118.f})if(Rand.FRand()<.78f)Deco(TEXT("PantryV13"),V+FVector(0,0,Level),Yaw);};
 auto Rug=[&](FVector V,FVector2D Size){B(TEXT("Cloth"),V+FVector(0,0,.7f),FVector(Size,1.4f),false);for(int Side:{-1,1})B(TEXT("Red"),V+FVector(Side*(Size.X*.5f-12),0,1.5f),FVector(8,Size.Y-15,1),false);};
 auto Sofa=[&](FVector V){if(!A(TEXT("SofaV13"),V))return;Rug(V+FVector(0,-100,0),FVector2D(300,270));if(A(TEXT("CoffeeTableV13"),V+FVector(0,-145,0)))Deco(TEXT("Papers65"),V+FVector(10,-145,43.4));for(int Side:{-1,1})if(A(TEXT("NightstandV13"),V+FVector(Side*160,0,0)))Deco(TEXT("TableLampV13"),V+FVector(Side*160,0,61));};
 auto Table=[&](FVector V){if(!A(TEXT("DiningTableV13"),V))return;for(int Side:{-1,1}){Furn(TEXT("chair"),V+FVector(0,Side*105,0),-Side*90);Deco(TEXT("TableSettingV13"),V+FVector(0,Side*27,78),Side>0?180:0);}Deco(TEXT("PantryV13"),V+FVector(-35,0,78),90,FVector(.55));};
 auto WorkDesk=[&](FVector V){if(A(TEXT("Desk"),V)){Deco(TEXT("DeskSetV13"),V+FVector(0,0,82));Furn(TEXT("chair"),V-FVector(0,100,0),90);}};
 auto Kitchen=[&](FVector V){Furn(TEXT("cooker"),V);Furn(TEXT("sink"),V+FVector(100,0,0));Furn(TEXT("water"),V+FVector(-104,0,0));if(A(TEXT("CabinetV3"),V+FVector(230,0,0)))Deco(TEXT("PantryV13"),V+FVector(230,0,99));};
 auto Bath=[&](FVector V){A(TEXT("ToiletV4"),V,0);Furn(TEXT("sink"),V+FVector(130,0,0));B(TEXT("Steel"),V+FVector(130,65,160),FVector(95,5,80),false);};
 auto Poster=[&](FVector V,const TCHAR* Text){if(Home){Deco(TEXT("LandscapeV13"),V);return;}B(TEXT("Wood"),V,FVector(130,8,165),false);B(TEXT("PosterV7"),V-FVector(0,5,0),FVector(115,2,150),false);Label(Text,V-FVector(0,7,15),-90,12);};
 B(TEXT("Concrete"),FVector(0,0,0),FVector(2*X+100,2*Y+100,24));
 if(S.Size.Y*.5>Y+10){float Setback=S.Size.Y*.5-Y;B(TEXT("Concrete"),FVector(0,-Y-Setback*.5f,5),FVector(180,Setback+100,10));}
 if(Outdoor){
 B(TEXT("Asphalt"),FVector(0,0,15),FVector(2*X,2*Y,10));
 for(float XX=-X+350;XX<X-250;XX+=420)for(float YY=-Y+600;YY<Y-700;YY+=1250){B(TEXT("Lane"),FVector(XX,YY,22),FVector(9,540,2),false);if(!(T==16&&XX<-X+1190&&YY<-Y+1850)&&Rand.FRand()<.35)Prop(ELWObjectKind::Car,FVector(XX+190,YY,30),90);}
 if(T==16){auto* Terminal=Prop(ELWObjectKind::Furniture,FVector(-X+1200,-Y+300,22),0);if(Terminal){Terminal->UseType=TEXT("delivery66");Terminal->Body->SetStaticMesh(W->Mesh(TEXT("RV66_DeliveryTerminal")));Terminal->Route={FVector2D(GetActorLocation()+P(FVector(-X+700,-Y+1050,22)))};}Label(TEXT("VEHICLE DELIVERY / 150 CREDITS"),FVector(-X+1200,-Y+322,210),-90,13);}
 if(T==15){B(TEXT("Steel"),FVector(0,Y-160,550),FVector(2400,70,1050));B(TEXT("Bone"),FVector(0,Y-202,650),FVector(2240,8,770));Label(TEXT("LAST PICTURE SHOW"),FVector(0,Y-212,400),-90,60);for(float XX=-X+300;XX<X;XX+=840)B(TEXT("Steel"),FVector(XX,0,55),FVector(12,12,110));}
 B(TEXT("BrickV7"),FVector(X-400,-Y+300,120),FVector(600,440,240));B(TEXT("Glass"),FVector(X-400,-Y+72,150),FVector(400,8,95));Label(T==15?TEXT("TICKETS / SNACKS"):TEXT("PARKING"),FVector(X-400,-Y+64,230),-90,20);
 }else{
 for(int F=0;F<Floors;F++){
 float Z=24+F*(H+30);Occupied.Add(FBox(FVector(-115,-Y+130,Z),FVector(115,Y-150,Z+H)));if(F==0||Floors==1)B(Floor,FVector(0,0,Z-6),FVector(2*X,2*Y,12));else{
 B(Floor,FVector(0,-360,Z-6),FVector(2*X,2*Y-720,12));for(int Side:{-1,1})B(Floor,FVector(Side*(X+150)*.5,Y-360,Z-6),FVector(X-150,720,12));B(Floor,FVector(0,Y-60,Z-6),FVector(300,120,12));
 for(int Side:{-1,1})B(TEXT("Steel"),FVector(Side*148,Y-400,Z+100),FVector(8,560,8));}
 // Upper floors have a genuine opening over the rear stairwell.
 for(int Side:{-1,1}){
 B(Exterior,FVector(Side*X,0,Z+(H+30)*.5),FVector(24,2*Y,H+30));
 B(Interior,FVector(Side*(X-14),0,Z+H*.5),FVector(3,2*Y-20,H),false);
 B(TEXT("Wood"),FVector(Side*(X-18),0,Z+12),FVector(5,2*Y,18),false);
 }
 B(Exterior,FVector(0,Y,Z+(H+30)*.5),FVector(2*X,24,H+30));
 // Storefront windows and fitted door, with sills, jambs and lintels.
 if(Home){for(int Side:{-1,1}){float Start=100,End=X;int Count=FMath::Max(1,int((X-100)/430));float Step=(End-Start)/Count;
 for(int I=0;I<Count;I++){float Mid=Start+(I+.5f)*Step,Span=FMath::Min(165.f,Step-70);float XX=Side*Mid;
 B(Exterior,FVector(XX,-Y,Z+55),FVector(Step,24,110));B(Exterior,FVector(XX,-Y,Z+(H+260)*.5f),FVector(Step,24,H-200));
 for(int E:{-1,1})B(Exterior,FVector(XX+E*(Step+Span)*.25f,-Y,Z+170),FVector((Step-Span)*.5f,24,120));
 Prop(ELWObjectKind::Window,FVector(XX,-Y,Z+170),0,FVector((Span-12)/100,.035,1.16));
 for(int E:{-1,1}){B(TEXT("Wood"),FVector(XX+E*Span*.5f,-Y-4,Z+170),FVector(8,36,128));B(TEXT("Wood"),FVector(XX,-Y-4,Z+170+E*60),FVector(Span+12,36,8));}
 Deco(TEXT("CurtainsV13"),FVector(XX,-Y+30,Z+165),0,FVector(Span/180,1,1));
 }} }else{ float Bay=(2*X-200)/2;for(int Side:{-1,1}){float XX=Side*(X*.5+50);B(Exterior,FVector(XX,-Y,Z+52),FVector(Bay,24,104));B(Exterior,FVector(XX,-Y,Z+(H+275)*.5),FVector(Bay,24,H-215));B(TEXT("Steel"),FVector(XX,-Y-5,Z+104),FVector(Bay,36,9));int Panes=FMath::Max(1,FMath::CeilToInt(Bay/220));float Span=Bay/Panes;for(int K=0;K<Panes;K++){float WX=XX-Bay*.5+(K+.5f)*Span;Prop(ELWObjectKind::Window,FVector(WX,-Y,Z+173),0,FVector((Span-18)/100,.035,1.32));B(TEXT("Steel"),FVector(WX-Span*.5,-Y,Z+174),FVector(12,28,142));}for(int E:{-1,1})B(Exterior,FVector(XX+E*(Bay/2-10),-Y,Z+174),FVector(20,24,142));}}
 Wall(FVector(0,-Y,Z),200,0,true,Exterior,H);
 if(Home&&F==0){Rug(FVector(0,-Y+85,Z),FVector2D(160,90));B(TEXT("Wood"),FVector(135,-Y-16,Z+165),FVector(58,8,34),false);Label(FString::FromInt(10+S.Id%990),FVector(135,-Y-22,Z+156),-90,18);}
 B(TEXT("Wood"),FVector(0,Y-18,Z+14),FVector(2*X,5,20),false);
 if(F==0&&!Home){B(TEXT("Steel"),FVector(0,-Y-40,Z+H+55),FVector(FMath::Min(2*X-100,1800.f),40,125));if(T<5){auto* Sign=Prop(ELWObjectKind::Sign,FVector(0,-Y-95,Z+H+65),-90,FVector(1,4,1.3));if(Sign){Sign->Body->SetStaticMesh(W->Mesh(T==4?TEXT("SignMarqueeV3"):TEXT("SignFrameV3")));Sign->SignFace(T);}}else if(T!=18)Label(LWPlaces::Name(T),FVector(0,-Y-64,Z+H+35),-90,T==12?32:42);}
 // Apartment and motel rooms open onto a central hall; each has private facilities.
 if(T==1||T==6||T==9){
 float Depth=(2*Y-720)/3;
 for(int Side:{-1,1})for(int I=0;I<3;I++){
 float CY=-Y+Depth*(I+.5f)+80,CX=Side*(X+180)*.5f;float RW=X-180;
 Wall(FVector(Side*180,CY,Z),Depth,90,true,Interior,H);
 if(I<2)Wall(FVector(CX,CY+Depth*.5,Z),RW,0,false,Interior,H);
 Wall(FVector(Side*(X-200),CY+Depth*.5-330,Z),380,0,true,Interior,H);Wall(FVector(Side*(X-390),CY+Depth*.5-165,Z),330,90,false,Interior,H);
 Furn(TEXT("bed"),FVector(CX,CY-40,Z),0);Rug(FVector(CX,CY-55,Z),FVector2D(300,230));if(A(TEXT("NightstandV13"),FVector(CX-130,CY-40,Z)))Deco(TEXT("TableLampV13"),FVector(CX-130,CY-40,Z+61));A(TEXT("CabinetV3"),FVector(Side*(X-100),CY-Depth*.3,Z),90);A(TEXT("Desk"),FVector(CX-Side*230,CY-Depth*.28,Z),90);Furn(TEXT("chair"),FVector(CX-Side*100,CY-Depth*.28,Z),Side<0?180:0);Bath(FVector(Side*(X-210),CY+Depth*.5-160,Z));

 Label(FString::Printf(TEXT("%d%02d"),F+1,I+1+(Side>0?3:0)),FVector(Side*180-4,CY-70,Z+240),Side<0?0:180,17);Light(FVector(CX,CY,Z+H-12));
 }
 if(F<Floors-1)for(int I=0;I<18;I++)B(TEXT("Concrete"),FVector(0,Y-650+I*30,Z+(I+1)*20*.5f),FVector(250,30,(I+1)*20));
 }else if(Home){
 // A fitted plaster ceiling closes the domestic rooms below the roof void.
 B(TEXT("Plaster65"),FVector(0,0,Z+H+12),FVector(2*X-26,2*Y-26,12));
 const float Hall=85,Rear=-40,BathY=Y-270;
 const float BedroomDepth=(Y-Rear)*.5f;
 for(int Room=0;Room<2;Room++)Wall(FVector(-Hall,Rear+(Room+.5f)*BedroomDepth,Z),BedroomDepth,90,true,Interior,H);
 Wall(FVector(-(X+Hall)*.5f,Rear+BedroomDepth,Z),X-Hall,0,false,Interior,H);
 Wall(FVector(Hall,(BathY+Rear)*.5f,Z),BathY-Rear,90,true,Interior,H);
 Wall(FVector(Hall,(Y+BathY)*.5f,Z),Y-BathY,90,true,Interior,H);
 Wall(FVector(-(X+Hall)*.5f,Rear,Z),X-Hall,0,false,Interior,H);
 Wall(FVector((X+Hall)*.5f,Rear,Z),X-Hall,0,false,Interior,H);
 Wall(FVector((X+Hall)*.5f,BathY,Z),X-Hall,0,false,Interior,H);
 // Living room grouped around a coffee table, with a clear route past it.
 FVector Living(-X*.53f,-Y*.46f,Z);Sofa(Living);A(TEXT("BookcaseV13"),FVector(-X+60,-Y*.35f,Z),90);
 if(A(TEXT("SideboardV13"),FVector(-X*.52f,Rear-85,Z)))Deco(TEXT("Papers65"),FVector(-X*.52f,Rear-85,Z+88.4));
 Table(FVector(X*.5f,-Y*.65f,Z));Kitchen(FVector(X*.48f,Rear-60,Z));
 // Two bedrooms have their own hall doors, headboards toward a wall, and bedside storage.
 for(int Room=0;Room<2;Room++){float Start=Rear+Room*BedroomDepth,End=Start+BedroomDepth;FVector Bed(-(X+Hall)*.5f,Start+140,Z);Furn(TEXT("bed"),Bed,90);Rug(Bed,FVector2D(290,270));
 for(int Side:{-1,1})if(A(TEXT("NightstandV13"),Bed+FVector(Side*120,-80,0)))Deco(TEXT("TableLampV13"),Bed+FVector(Side*120,-80,61));
 A(TEXT("SideboardV13"),FVector(Bed.X,End-60,Z));Poster(FVector(Bed.X,End-18,Z+180),TEXT(""));}
 FVector Study(X*.72f,(Rear+BathY)*.5f,Z);if(A(TEXT("Desk"),Study,90)){Deco(TEXT("DeskSetV13"),Study+FVector(0,0,82),90);Furn(TEXT("chair"),Study-FVector(105,0,0),0);}A(TEXT("BookcaseV13"),FVector(X-75,Study.Y,Z),-90);
 Bath(FVector(X-300,Y-60,Z));A(TEXT("BathtubV13"),FVector(X*.52f,Y-195,Z));A(TEXT("WasteBinV13"),FVector(X-80,Y-110,Z));
 // Wall-mounted services have shallow bounds and sit against an actual wall.
 Deco(TEXT("FuseBoxV13"),FVector(Hall+22,Y-100,Z+160),90);
 Poster(FVector(-X*.55f,Y-18,Z+185),TEXT(""));
 }else{
 float Back=Y-650;Wall(FVector(0,Back,Z),2*X,0,true,Interior,H);Wall(FVector(X*.5,(Y+Back)*.5,Z),650,90,true,Interior,H);
 Bath(FVector(X-320,Y-130,Z));Furn(TEXT("locker"),FVector(X*.25,Y-90,Z));Furn(TEXT("workbench"),FVector(-X+180,Y-120,Z));A(TEXT("Desk"),FVector(-X*.4,Y-260,Z));Furn(TEXT("chair"),FVector(-X*.4,Y-390,Z),90);Label(TEXT("STAFF"),FVector(0,Back-15,Z+260),-90,20);
 if(T==13){
 // Receiving east, bulk storage west, packing beside the staff/shipping office.
 for(int Row=0;Row<3;Row++)for(int Bay=0;Bay<4;Bay++){FVector V(-X+450+Row*600,-Y+650+Bay*500,Z);if(Rand.FRand()<.94f)A(TEXT("PalletRack53"),V,90);}
 for(int I=0;I<4;I++)A(TEXT("LoadedPallet53"),FVector(X-450-(I%2)*210,-Y+600+(I/2)*300,Z),I%2?8:-5);
 A(TEXT("PalletJack53"),FVector(X-780,-Y+1250,Z),-20);A(TEXT("PackingBench53"),FVector(-600,Back-220,Z));A(TEXT("PackingBench53"),FVector(300,Back-220,Z));
 for(int Side:{-1,1})B(TEXT("Lane"),FVector(X-1080+Side*260,-Y+1150,Z+2),FVector(10,1600,2),false);
 Label(TEXT("RECEIVING / KEEP CLEAR"),FVector(X-115,-Y+850,Z+290),180,23);Label(TEXT("DISPATCH / PACKING"),FVector(-350,Back-16,Z+280),-90,24);
 }else if(T==0||T==10||T==11||T==12||T==3){
 if(T==11){for(int I=1;I<4;I++){float XX=-X+I*X*.5;Wall(FVector(XX,(-Y+450+Back)*.5,Z),Back+Y-450,90,false,Interior,H);}}
 int Aisles=FMath::Max(2,int(X/480));for(int I=0;I<Aisles;I++){float XX=-X+360+I*(2*X-720)/FMath::Max(1,Aisles-1);for(float YY=-Y+600;YY<Back-350;YY+=350){if(T==12){int Display=(I+int((YY+Y)/350))%3;if(Display==0)Sofa(FVector(XX,YY,Z));else if(Display==1)Table(FVector(XX,YY,Z));else Furn(TEXT("bed"),FVector(XX,YY,Z),90);}else{Shelf(FVector(XX,YY,Z));if(T==13||T==3){A(TEXT("Crate"),FVector(XX+130,YY,Z));B(TEXT("Lane"),FVector(XX+260,YY,Z+1),FVector(8,480,1),false);}}}
 }
 if(T!=13&&T!=18)for(int I=0;I<3;I++){A(TEXT("ServiceBenchV3"),FVector(-X+350+I*330,-Y+260,Z));B(TEXT("Rubber"),FVector(-X+350+I*330,-Y+230,Z+112),FVector(40,30,30));}
 if(T==11){for(int I=0;I<4;I++)Label(I==0?TEXT("PHARMACY"):I==1?TEXT("VIDEO"):I==2?TEXT("MARKET"):TEXT("HARDWARE"),FVector(-X+(I+.5)*X*.5,-Y-65,Z+H+35),-90,22);}
 }else if(T==4||T==14){
 // The cooking line belongs in the staff kitchen, with a service counter facing diners.
 A(TEXT("KitchenRangeV9"),FVector(-X*.60,Y-115,Z));A(TEXT("KitchenHoodV9"),FVector(-X*.60,Y-115,Z+H-35));Furn(TEXT("sink"),FVector(-X*.32,Y-115,Z));Furn(TEXT("water"),FVector(-X+100,Back+185,Z));
 A(TEXT("CoffeeStation65"),FVector(-X*.45,Back-95,Z));
 for(float XX=-X+250;XX<X-200;XX+=450)for(float YY=-Y+380;YY<Back-330;YY+=400){if(FMath::Abs(XX)<260)continue;FVector V(XX,YY,Z);if(A(TEXT("DinerTableV9"),V)){for(int Place:{-1,1})Deco(TEXT("TableSettingV13"),V+FVector(Place*30,0,79));A(TEXT("DinerBoothV9"),V+FVector(0,125,0));A(TEXT("DinerBoothV9"),V-FVector(0,125,0),180);}}

 Poster(FVector(-X*.4,Back-18,Z+210),TEXT("HOT MEALS"));
 }else if(T==19){
 // Clear central entry/circulation; kitchen behind the bar and booths at the west wall.
 Kitchen(FVector(-600,Back+350,Z));
 for(int I=0;I<3;I++){B(TEXT("Wood"),FVector(-900+I*280,Back-90,Z+53),FVector(260,85,106));B(TEXT("Steel"),FVector(-900+I*280,Back-90,Z+110),FVector(270,105,8));}
 for(int I=0;I<2;I++){FVector V(-X+310,-Y+430+I*530,Z);if(A(TEXT("DinerTableV9"),V))for(int Place:{-1,1})Deco(TEXT("TableSettingV13"),V+FVector(Place*30,0,79));A(TEXT("DinerBoothV9"),V+FVector(0,130,0));A(TEXT("DinerBoothV9"),V-FVector(0,130,0),180);}
 bool Gaming=S.Id%4!=0; // Three quarters of taverns; stable across streaming.
 for(int I=0;I<3;I++){
 FVector V(540,-Y+420+I*410,Z);
 if(Gaming){auto* TableActor=GetWorld()->SpawnActor<ALWCardTable>(GetActorLocation()+P(V),R);if(TableActor){TableActor->Setup(W,FName(*FString::Printf(TEXT("tavern_%u_table_%d"),S.Id,I)),I);Residents.Add(TableActor);}}
 else A(TEXT("DinerTableV9"),V);
 for(int Side:{-1,1})Furn(TEXT("chair"),V+FVector(Side*165,0,0),Side<0?0:180);
 }
 for(int I=0;I<4;I++){B(TEXT("Wood"),FVector(-860+I*210,Back-30,Z+230),FVector(160,38,9));for(int J=0;J<4;J++)B(J%2?TEXT("Glass"):TEXT("Bone"),FVector(-915+I*210+J*35,Back-30,Z+251),FVector(15,15,34),false);}
 Label(TEXT("FOOD / DRINK / GOOD COMPANY"),FVector(-550,Back-20,Z+285),-90,14);
 Poster(FVector(X-250,Back-20,Z+210),Gaming?TEXT("CARDS TONIGHT"):TEXT("HOT SUPPER"));
 // Residents keep their normal chatter and routines; the host offers the established shop UI.
 for(int I=0;I<4;I++){
 FVector V=I==0?FVector(-650,Back-280,Z+90):FVector(-X+570,-Y+300+(I-1)*410,Z+90);
 auto* Npc=GetWorld()->SpawnActor<ALWResident>(GetActorLocation()+P(V),R);
 if(Npc){FName Id(*FString::Printf(TEXT("tavern_%u_npc_%d"),S.Id,I));Npc->ConfigureResident(Id,I==0?TEXT("merchant"):TEXT("civilian"),I==0?TEXT("LEN / TAVERN HOST"):I==1?TEXT("RED"):I==2?TEXT("NORTH"):TEXT("WREN"),I);Npc->ActivitySpots={Npc->Home,GetActorLocation()+P(V+FVector(0,90,0)),Npc->Home};Npc->ActivityTime=8+I*4;Npc->ChatterTime=5+I*6;Residents.Add(Npc);}
 }
 }else if(T==18){
 B(TEXT("BoutiqueSignV9"),FVector(0,-Y-71,Z+H+100),FVector(1800,8,450),false);
 // A decompression zone at the entrance opens onto offset merchandising islands.
 A(TEXT("RoundRack53"),FVector(-650,-500,Z));A(TEXT("RoundRack53"),FVector(600,120,Z));
 A(TEXT("RoundRack53"),FVector(-1150,-700,Z));A(TEXT("RoundRack53"),FVector(1150,-600,Z));A(TEXT("DisplayTable53"),FVector(550,-750,Z));A(TEXT("DisplayTable53"),FVector(-500,350,Z));A(TEXT("GarmentRail53"),FVector(-X+180,-200,Z),90);A(TEXT("GarmentRail53"),FVector(X-180,-650,Z),90);A(TEXT("GarmentRail53"),FVector(-X+180,350,Z),90);A(TEXT("GarmentRail53"),FVector(X-180,-1100,Z),90);
 A(TEXT("SalesCounter53"),FVector(-1000,Back-240,Z));Deco(TEXT("CashRegister53"),FVector(-1050,Back-240,Z+100));
 A(TEXT("BoutiqueBenchV9"),FVector(700,Back-250,Z));
 Wall(FVector(X-620,Back-350,Z),700,90,false,Interior,H);Wall(FVector(X-310,Back-700,Z),620,0,true,Interior,H);A(TEXT("FittingMirror53"),FVector(X-200,Back-90,Z));Label(TEXT("FITTING ROOMS"),FVector(X-310,Back-716,Z+265),-90,18);
 const FVector DisplaySpots[]={FVector(-1250,-1120,0),FVector(1200,-1120,0),FVector(-650,50,0),FVector(1060,80,0)};
 for(int I=0;I<4;I++){FVector V=DisplaySpots[I]+FVector(0,0,Z);A(TEXT("DisplayPlinthV9"),V);bool Alive=!S.Friendly&&((I+S.Id)%3==0);
 if(Alive&&!W->KilledZombies.Contains(S.Id^(0x91aau+I))){auto* E=GetWorld()->SpawnActor<ALWZombie>(GetActorLocation()+P(V+FVector(0,0,118)),R+FRotator(0,-90,0));if(E){E->PersistentId=S.Id^(0x91aau+I);E->ConfigureKind(ELWEnemyKind::Mannequin);E->Home=E->GetActorLocation();Residents.Add(E);W->ZombieCount++;}}
 else if(!Alive)Prop(ELWObjectKind::MannequinDisplay,V+FVector(0,0,118),-90);
 }
 }
 else if(T==5){for(int Side:{-1,1})for(int I=0;I<6;I++){FVector V(Side*(X-150),-Y+220+I*230,Z);B(TEXT("Rubber"),V+FVector(0,0,85),FVector(110,100,170));B(TEXT("Red"),V+FVector(0,-55,105),FVector(110,20,18));B(TEXT("Glow"),V+FVector(0,-52,152),FVector(82,3,65),false);Label(I%2?TEXT("DEAD ORBIT"):TEXT("NIGHT RACE"),V+FVector(0,-55,205),-90,11);Furn(TEXT("chair"),V+FVector(0,-130,0),90);}Table(FVector(0,-Y*.2,Z));}
 else if(T==2){for(int Side:{-1,1})for(int I=0;I<3;I++){FVector V(Side*X*.55,-Y+380+I*410,Z);A(TEXT("ClinicBedV3"),V,90);B(TEXT("Cloth"),V+FVector(220,0,155),FVector(5,260,290));}Furn(TEXT("water"),FVector(350,Back-120,Z));}
 else if(T==17){for(int I=0;I<4;I++){WorkDesk(FVector(-X*.6,-Y+350+I*350,Z));Furn(TEXT("locker"),FVector(X-120,-Y+240+I*350,Z));}Wall(FVector(450,0,Z),2*Y-1400,90,true,Interior,H);for(int I=0;I<18;I++)B(TEXT("Steel"),FVector(450,-Y+900+I*55,Z+150),FVector(12,12,300));Label(TEXT("HOLDING / EVIDENCE"),FVector(500,Back-30,Z+260),-90,22);}
 }
 for(float XX=-X+350;XX<X;XX+=1100)for(float YY=-Y+500;YY<Y;YY+=1100)Light(FVector(XX,YY,Z+H-12));
 if(!Home){Poster(FVector(X*.55,Y-18,Z+200),TEXT("EVACUATION ORDER"));Deco(TEXT("FuseBoxV13"),FVector(-X+24,Y-340,Z+165),-90);}
 // Back-wall storage and detail at room scale, with deterministic gaps and no blocked entry axis.
 if(!Home&&T!=18&&T!=19&&T!=13){for(float XX=-X+160;XX<X-500;XX+=380){if(FMath::Abs(XX)<230)continue;if(A(TEXT("SideboardV13"),FVector(XX,Y-75,Z)))Deco(T==3||T==13?TEXT("DeskSetV13"):TEXT("PantryV13"),FVector(XX,Y-75,Z+87));}}
 if(Home){for(int Side:{-1,1})A(TEXT("RadiatorV13"),FVector(Side*(X-45),-Y*.55f,Z),Side>0?-90:90);}

 }
 B(Home?TEXT("PlasterV7"):TEXT("CorrugatedV7"),FVector(0,0,24+Floors*(H+30)-10),FVector(2*X+70,2*Y+70,25));
 if(T>=7&&T<=9){const float RoofBase=24+Floors*(H+30),Pitch=25,Rise=Y*FMath::Tan(FMath::DegreesToRadians(Pitch));
 for(int Side:{-1,1}){Box(W,TEXT("CorrugatedV7"),P(FVector(0,Side*Y*.5f,RoofBase+Rise*.5f)),FVector(2*X+100,Y/FMath::Cos(FMath::DegreesToRadians(Pitch))+70,22),R+FRotator(0,0,Side*Pitch));Add(W,TEXT("GableV13"),Exterior,P(FVector(Side*X,0,RoofBase)),FVector(1,Y/100,Rise/100),R);
 B(TEXT("Wood"),FVector(0,Side*(Y+22),RoofBase-5),FVector(2*X+105,14,38));}
 B(TEXT("Steel"),FVector(0,0,RoofBase+Rise+12),FVector(2*X+110,45,18));
 }

 for(int Side:{-1,1}){B(TEXT("Steel"),FVector(Side*(X+12),Y-15,Floors*(H+30)*.5),FVector(12,12,Floors*(H+30)));A(TEXT("Barrel"),FVector(Side*(X+140),Y-180,12));}
 if(T==0){for(int Side:{-1,1}){Prop(ELWObjectKind::FuelPump,FVector(Side*460,-Y-650,24),-90);B(TEXT("Concrete"),FVector(Side*460,-Y-650,8),FVector(230,180,30));B(TEXT("Red"),FVector(Side*850,-Y-650,230),FVector(25,25,460));}B(TEXT("CorrugatedV7"),FVector(0,-Y-650,470),FVector(1900,900,40));}
 // Off-street parking and a loading/service apron, with a clear front approach.
 for(int I=0;I<2;I++){B(TEXT("Asphalt"),FVector(-X-440,-Y+380+I*700,0),FVector(700,650,18));if(Rand.FRand()<.7f)Prop(ELWObjectKind::Car,FVector(-X-440,-Y+380+I*700,24),90);}
 }
 if(T==2){auto* Terminal=GetWorld()->SpawnActor<ALWInteractable>(GetActorLocation()+P(FVector(0,-Y+600,24)),R);if(Terminal){Terminal->Configure(W);Residents.Add(Terminal);}}
 W->SpawnSiteLoot(this,S);
 W->SpawnEnemies(this,&S,Footprint,Floors,H+30);
}
