#include "LWPOIExpansion.h"
#include "LWPOISurvivors.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWResident.h"
#include "LWSiteIdentity.h"
#include "LWSpawnTable.h"
#include "Components/StaticMeshComponent.h"
#include "LWWorldTextComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

// One local coordinate frame; immutable site IDs own every prop and encounter.
// Public circulation is deliberately separate from shelves, cells and ride machinery.
void LWPOIExpansion::Build(ALWChunk* C,ALWWorld* W,const LWGen::FSite& S){
 const FRotator R(0,S.Yaw,0);const FVector Base=FVector(S.Position,12)-C->GetActorLocation();int Serial=0;
 auto At=[&](FVector V){return Base+R.RotateVector(V);};
 auto Box=[&](FName M,FVector V,FVector Size,float Yaw=0,bool Hit=true){C->Box(W,M,At(V),Size,R+FRotator(0,Yaw,0),Hit);};
 auto Model=[&](FName M,FVector V,float Yaw=0){C->Add(W,M,NAME_None,At(V),FVector(1),R+FRotator(0,Yaw,0));};
 auto Object=[&](ELWObjectKind K,FVector V,float Yaw=0){auto* O=W->SpawnObject(K,FName(*FString::Printf(TEXT("poi30_%u_%d"),S.Id,Serial++)),C->GetActorLocation()+At(V),R+FRotator(0,Yaw,0));if(O)C->Residents.Add(O);return O;};
 auto Loot=[&](FName M,FVector V,int Lock=0){auto* O=Object(ELWObjectKind::Container,V);if(O){O->Body->SetStaticMesh(W->Mesh(M));W->EnsureSiteContainer(O->RecordId,S,O->GetActorLocation(),Lock);}};
 auto Furn=[&](FName M,FVector V,float Yaw=0){if(auto* O=Object(ELWObjectKind::Furniture,V,Yaw))O->SetFurniture(M);};
 auto Label=[&](const FString& T,FVector V,float Size=36,float Yaw=-90){auto* L=NewObject<ULWWorldTextComponent>(C);L->SetupAttachment(C->GetRootComponent());L->SetRelativeLocation(At(V));L->SetRelativeRotation(R+FRotator(0,Yaw,0));L->SetText(FText::FromString(T));L->SetWorldSize(Size);L->SetHorizontalAlignment(EHTA_Center);L->SetTextRenderColor(FColor(238,215,168));L->RegisterComponent();};
 auto Light=[&](FVector V){Model(TEXT("CeilingLightV13"),V);auto* L=NewObject<UPointLightComponent>(C);L->SetupAttachment(C->GetRootComponent());L->SetRelativeLocation(At(V-FVector(0,0,20)));L->SetIntensity(22000);L->SetAttenuationRadius(1600);L->SetCastShadows(false);L->RegisterComponent();};
 // Wide openings keep doors and traffic routes clear. Shells face local -Y.
 auto Wall=[&](FVector V,float Length,float Yaw=0,bool Open=false,float H=380){auto Seg=[&](float X,float L,float Z,float Tall){Box(TEXT("BrickV7"),V+FRotator(0,Yaw,0).RotateVector(FVector(X,0,Z)),FVector(L,24,Tall),Yaw);};if(Open){for(int Side:{-1,1})Seg(Side*(Length+320)*.25f,(Length-320)*.5f,H*.5f,H);Seg(0,320,(H+260)*.5f,H-260);}else Seg(0,Length,H*.5f,H);};
 auto Shell=[&](FVector V,float X,float Y,const FString& T,float H=380){Box(TEXT("TileV7"),V+FVector(0,0,12),FVector(X*2,Y*2,24));Wall(V+FVector(0,-Y,24),X*2,0,true,H);Wall(V+FVector(0,Y,24),X*2,0,false,H);for(int Side:{-1,1})Wall(V+FVector(Side*X,0,24),Y*2,90,false,H);Box(TEXT("CorrugatedV7"),V+FVector(0,0,H+40),FVector(X*2+24,Y*2+24,24));Label(T,V+FVector(0,-Y-18,320),T.Len()>26?28:36);Light(V+FVector(0,0,H));};
 auto Shelf=[&](FVector V){Loot(TEXT("Shelf"),V);for(float Z:{13.f,65.f,118.f})Model(TEXT("PantryV13"),V+FVector(0,0,Z));};
 auto Table=[&](FVector V){Model(TEXT("DiningTableV13"),V);for(int Side:{-1,1})Furn(TEXT("chair"),V+FVector(0,Side*125,0),-Side*90);};
 const float X=S.Size.X*.5f,Y=S.Size.Y*.5f;
 Box(TEXT("Asphalt"),FVector(0,0,0),FVector(2*X,2*Y,20));
 Box(TEXT("Concrete"),FVector(0,-Y+600,12),FVector(900,1200,24));
 Label(LWSites::Label(S),FVector(0,-Y+150,550),65);
 if(S.Type==LWPlaces::ShoppingMall){
  // Enclosed 174m hall: the outer envelope surrounds every store and service room.
  // The 12m-high common roof leaves shop roofs below it and all circulation open.
  constexpr float MallX=8700,Front=-5500,Back=6900,Roof=1224;
  Box(TEXT("TileV7"),FVector(0,700,12),FVector(2*MallX,Back-Front,24));
  Box(TEXT("Concrete"),FVector(0,-6500,12),FVector(1200,2000,24));
  for(int Side:{-1,1}){
   Wall(FVector(Side*MallX,700,24),Back-Front,90,false,1200);
   // A twelve-meter-wide, four-meter-high front portal leads into the concourse.
   Box(TEXT("BrickV7"),FVector(Side*(MallX+600)*.5f,Front,624),FVector(MallX-600,24,1200));
   Box(TEXT("Glass"),FVector(Side*2600,Front-14,640),FVector(3000,8,520),0,false);
  }
  Wall(FVector(0,Back,24),2*MallX,0,false,1200);
  Box(TEXT("BrickV7"),FVector(0,Front,824),FVector(1200,24,800));
  Label(LWSites::Label(S),FVector(0,Front-28,950),65);
  // Opaque roof wings and end caps leave a genuine 36m-wide skylight aperture.
  for(int Side:{-1,1})Box(TEXT("CorrugatedV7"),FVector(Side*(MallX+1800)*.5f,700,Roof),FVector(MallX-1800,Back-Front,32));
  Box(TEXT("CorrugatedV7"),FVector(0,-5350,Roof),FVector(3600,300,32));
  Box(TEXT("CorrugatedV7"),FVector(0,5400,Roof),FVector(3600,3000,32));
  Box(TEXT("Glass"),FVector(0,-650,Roof),FVector(3600,9100,20));
  for(int Side:{-1,1})Box(TEXT("Steel"),FVector(Side*1800,-650,Roof+8),FVector(32,9100,40));
  for(int I=0;I<=7;I++)Box(TEXT("Steel"),FVector(0,-5200+I*1300,Roof+8),FVector(3600,32,40));
  for(int I=0;I<5;I++){
   Box(TEXT("Steel"),FVector(0,-4300+I*2400,1120),FVector(2*MallX,36,56));
   for(int Side:{-1,1})Light(FVector(Side*1050,-4000+I*1800,1080));
  }
  // Twelve stores, two anchor halls, food court and cinema retain their layouts.
  Box(TEXT("TileV7"),FVector(0,700,14),FVector(15000,11600,28));
  for(int Side:{-1,1})for(int I=0;I<6;I++){
   FVector V(Side*4900,-3800+I*1650,0);const TCHAR* Names[]={TEXT("FASHION"),TEXT("BOOK EXCHANGE"),TEXT("ELECTRONICS"),TEXT("PHARMACY"),TEXT("SPORTING GOODS"),TEXT("HOME & GIFTS")};
   // Each bank opens onto the axial concourse: left faces +X, right faces -X.
   Box(TEXT("TileV7"),V+FVector(0,0,12),FVector(3400,1440,24));
   for(int End:{-1,1})Wall(V+FVector(0,End*720,24),3400);
   Wall(V+FVector(Side*1700,0,24),1440,90);
   Wall(V+FVector(-Side*1700,0,24),1440,90,true);
   Box(TEXT("CorrugatedV7"),V+FVector(0,0,420),FVector(3424,1464,24));
   Label(Names[I],V+FVector(-Side*1720,0,330),32,Side<0?0:180);
   Light(V+FVector(0,0,380));
   for(int J=0;J<4;J++){FVector P=V+FVector(-1100+J*650,360,24);if(I==1)Loot(TEXT("BookcaseV13"),P);else if(I==0)Loot(TEXT("ClothesRackV9"),P);else Shelf(P);}Loot(TEXT("ShopCounterV18"),V+FVector(Side*650,-400,24));
  }
  for(int Side:{-1,1}){Shell(FVector(Side*1900,5200,0),1700,1050,Side<0?TEXT("DEPARTMENT STORE"):TEXT("CINEMA / SCREEN ONE"),560);if(Side<0){for(int I=0;I<4;I++)Shelf(FVector(-2900+I*650,5750,24));}else{Box(TEXT("Bone"),FVector(1900,6160,310),FVector(2100,16,430));for(int Row=0;Row<4;Row++)for(int Col=0;Col<6;Col++)Furn(TEXT("chair"),FVector(900+Col*390,4700+Row*320,24),90);}}
  for(int Side:{-1,1})for(int I=0;I<4;I++){Model(TEXT("PlazaPlanterV18"),FVector(Side*2500,-3300+I*1800,24));Model(TEXT("BoutiqueBenchV9"),FVector(Side*1900,-3100+I*1800,24));}
  // Welcome amenities stay outside the central 12m clear entrance/through aisle.
  Box(TEXT("Steel"),FVector(-1200,-4600,180),FVector(420,90,320));
  Label(TEXT("MALL DIRECTORY"),FVector(-1200,-4650,290),27);
  Label(TEXT("SHOPS: LEFT / RIGHT"),FVector(-1200,-4650,230),20);
  Label(TEXT("FOOD / CINEMA: AHEAD"),FVector(-1200,-4650,185),19);
  Model(TEXT("ShopCounterV18"),FVector(1200,-4600,28));
  Label(TEXT("INFORMATION"),FVector(1200,-4650,240),24);
  for(int Side:{-1,1}){
   Model(TEXT("BoutiqueBenchV9"),FVector(Side*1850,-4100,28),Side<0?0:180);
   Model(TEXT("PlazaPlanterV18"),FVector(Side*2500,-4450,28));
   Model(TEXT("PlazaPlanterV18"),FVector(Side*1450,-2600,28));
  }
  // Lower pendant fill illuminates pedestrians and storefronts, not only the roof.
  for(int Row=0;Row<6;Row++)for(int Side:{-1,1}){
   const FVector V(Side*2100,-4400+Row*1600,600);
   Model(TEXT("CeilingLightV13"),V);
   Box(TEXT("Steel"),V+FVector(0,0,250),FVector(8,8,500),0,false);
   auto* Fill=NewObject<UPointLightComponent>(C);Fill->SetupAttachment(C->GetRootComponent());Fill->SetRelativeLocation(At(V-FVector(0,0,30)));Fill->SetIntensity(90000);Fill->SetAttenuationRadius(2800);Fill->SetLightColor(FLinearColor(1.f,.9f,.75f));Fill->SetCastShadows(false);Fill->RegisterComponent();
  }
  Label(TEXT("FOOD COURT / NORTH ANCHORS"),FVector(0,3000,420),36);
  for(int I=0;I<3;I++){Loot(TEXT("ShopCounterV18"),FVector(-900+I*900,3200,24));Furn(TEXT("cooker"),FVector(-900+I*900,3480,24));}
  for(int Side:{-1,1})for(int I=0;I<3;I++)Table(FVector(Side*700,500+I*650,24));
  for(int Side:{-1,1}){Shell(FVector(Side*7600,5500,0),800,1000,TEXT("DELIVERIES / STAFF"));Loot(TEXT("Crate"),FVector(Side*7600+400,6000,24),2);Furn(TEXT("workbench"),FVector(Side*7600-400,6000,24));}
 }else if(S.Type==LWPlaces::Prison){
  // Perimeter, intake, two cell wings, open exercise yard, mess and infirmary.
  for(int Side:{-1,1}){Wall(FVector(Side*(X-120),0,24),2*Y-240,90,false,650);Wall(FVector(0,Side*(Y-120),24),2*X-240,0,Side<0,650);}
  Shell(FVector(0,-4100,0),1700,850,TEXT("INTAKE / VISITATION"));for(int I=0;I<4;I++)Table(FVector(-1150+I*760,-4000,24));
  for(int Side:{-1,1}){FVector Wing(Side*3800,0,0);Shell(Wing,1550,2600,Side<0?TEXT("CELL BLOCK A"):TEXT("CELL BLOCK B"));
   for(int Row=0;Row<6;Row++)for(int Bank:{-1,1}){FVector V=Wing+FVector(Bank*1000,-2000+Row*780,24);Wall(V+FVector(0,350,0),1000);// Barred cell front leaves a 320cm opening onto the longitudinal corridor.
    for(int Bar=-4;Bar<=4;Bar++)if(FMath::Abs(Bar)>=2)Box(TEXT("Steel"),V+FVector(Bank*-500,Bar*80,160),FVector(14,14,320));
    Box(TEXT("Steel"),V+FVector(Bank*-500,0,325),FVector(20,700,20));Model(TEXT("CellBunkV18"),V+FVector(Bank*180,90,0));Model(TEXT("ToiletV4"),V+FVector(Bank*220,-210,0));if(Row%3==0)Loot(TEXT("LockerV4"),V+FVector(-Bank*120,100,0),1);}
  }
  Box(TEXT("Concrete"),FVector(0,0,16),FVector(3600,4600,32));for(int I=0;I<3;I++){Box(TEXT("Lane"),FVector(-1100+I*1100,0,34),FVector(12,3400,2),0,false);Model(TEXT("BoutiqueBenchV9"),FVector(-1000+I*1000,1700,34));}
  Shell(FVector(-2100,4400,0),1700,1000,TEXT("MESS HALL"));for(int I=0;I<4;I++)Table(FVector(-3200+I*720,4400,24));Furn(TEXT("cooker"),FVector(-3000,5000,24));
  Shell(FVector(2100,4400,0),1700,1000,TEXT("INFIRMARY / WARDEN"));for(int I=0;I<3;I++)Furn(TEXT("bed"),FVector(1200+I*750,4900,24));Loot(TEXT("EvidenceCabinetV18"),FVector(3200,3800,24),3);
 }else if(S.Type==LWPlaces::Church){
  Shell(FVector(0,100,0),1700,2000,TEXT("SAINT BRIGID / SANCTUARY"),850);
  for(int Side:{-1,1})for(int Row=0;Row<8;Row++){Box(TEXT("Wood"),FVector(Side*870,-1300+Row*330,75),FVector(1000,110,100));Box(TEXT("Wood"),FVector(Side*870,-1350+Row*330,130),FVector(1000,20,180));}
  // Level chancel keeps altar reachable without a jump-only dais.
  Table(FVector(0,1600,24));Box(TEXT("Wood"),FVector(0,2030,530),FVector(45,32,480));Box(TEXT("Wood"),FVector(0,2030,620),FVector(280,32,45));
  for(int Side:{-1,1})for(int I=0;I<4;I++)Box(TEXT("Glass"),FVector(Side*1680,-1100+I*800,500),FVector(12,330,430),0,false);
  Shell(FVector(0,2470,0),1500,260,TEXT("VESTRY / RELIEF SUPPLIES"));Loot(TEXT("CabinetV3"),FVector(-900,2500,24));Loot(TEXT("BookcaseV13"),FVector(900,2500,24));
  Box(TEXT("BrickV7"),FVector(-1450,-1850,1100),FVector(430,430,550));Box(TEXT("Wood"),FVector(-1450,-1850,1490),FVector(35,35,230));Box(TEXT("Wood"),FVector(-1450,-1850,1530),FVector(170,35,35));
 }else if(S.Type==LWPlaces::CarDealership){
  Shell(FVector(-1300,800,0),2450,2450,TEXT("SUNSET MOTORS / SHOWROOM"),560);
  for(int I=0;I<3;I++){Object(ELWObjectKind::Car,FVector(-2700+I*1400,150,75),90);Loot(TEXT("Desk"),FVector(-2800+I*1500,2450,24));Furn(TEXT("chair"),FVector(-2800+I*1500,2300,24),90);}
  Label(TEXT("SALES / FINANCE / KEYS"),FVector(-1300,3100,350));
  for(int I=0;I<3;I++){FVector V(2600,-1000+I*1700,0);Shell(V,1100,750,TEXT("SERVICE BAY"),500);Object(ELWObjectKind::Car,V+FVector(0,100,75),90);Furn(TEXT("workbench"),V+FVector(700,400,24));Loot(TEXT("LockerV4"),V+FVector(-700,450,24),1);}
  for(int I=0;I<6;I++){float PX=-3100+I*1150;Box(TEXT("Lane"),FVector(PX,-2650,14),FVector(8,800,2),0,false);if(I%2==0)Object(ELWObjectKind::Car,FVector(PX+430,-2650,75),90);}
 }else if(S.Type==LWPlaces::HardwareStore){
  Shell(FVector(-500,100,0),3100,2850,TEXT("BUILDRIGHT / TRADE SUPPLIES"),650);
  const TCHAR* Aisles[]={TEXT("TOOLS"),TEXT("FASTENERS"),TEXT("ELECTRICAL"),TEXT("PLUMBING"),TEXT("PAINT"),TEXT("GARDEN")};
  for(int I=0;I<6;I++){float PX=-2800+I*920;Label(Aisles[I],FVector(PX,-1300,380),25);for(int J=0;J<5;J++)Shelf(FVector(PX,-900+J*610,24));}
  for(int I=0;I<3;I++)Loot(TEXT("ShopCounterV18"),FVector(-2100+I*1700,-2150,24));
  Wall(FVector(-500,2300,24),6200,0,true,650);Furn(TEXT("workbench"),FVector(-2600,2700,24));Loot(TEXT("Crate"),FVector(1700,2700,24),2);
  for(int I=0;I<5;I++){FVector V(3350,-2000+I*950,40);Box(TEXT("Wood"),V,FVector(700,650,55));for(int J=0;J<3;J++)Box(TEXT("Wood"),V+FVector(0,0,70+J*65),FVector(680,130,45));}Label(TEXT("LUMBER YARD"),FVector(3350,-2600,340),26);
 }else if(S.Type==LWPlaces::ThemePark){
  // Broad midway connects ticket booths, carousel, wheel, arcade and ride engineering.
  Box(TEXT("Concrete"),FVector(0,0,15),FVector(2200,16000,30));Box(TEXT("Concrete"),FVector(0,1500,15),FVector(18000,1600,30));
  for(int Side:{-1,1}){Shell(FVector(Side*1700,-7300,0),750,550,TEXT("TICKETS"));Loot(TEXT("ShopCounterV18"),FVector(Side*1700-350,-7300,24));}
  for(int Side:{-1,1})for(int I=0;I<3;I++){FVector V(Side*5800,-5300+I*1750,0);Shell(V,1800,700,I==0?TEXT("MIDWAY GAMES"):I==1?TEXT("FOOD / DRINK"):TEXT("SOUVENIRS"));for(int J=0;J<4;J++)if(I==0)Model(TEXT("ArcadeCabinetV18"),V+FVector(-1200+J*800,300,24));else Shelf(V+FVector(-1200+J*800,300,24));}
  const FVector Carousel(-5000,4200,0);Box(TEXT("Wood"),Carousel+FVector(0,0,20),FVector(3100,3100,40));Box(TEXT("Steel"),Carousel+FVector(0,0,500),FVector(90,90,960));Box(TEXT("Red"),Carousel+FVector(0,0,1000),FVector(3300,3300,70));
  for(int I=0;I<12;I++){float A=I*PI/6;FVector V=Carousel+FVector(FMath::Cos(A)*1150,FMath::Sin(A)*1150,40);Box(TEXT("Steel"),V+FVector(0,0,450),FVector(12,12,900));Furn(TEXT("chair"),V,I*30);}
  Label(TEXT("CAROUSEL / POWER OFF"),Carousel+FVector(0,-1600,350),35);
  // Static Ferris wheel: segmented rim, spokes, gondolas, and ground loading court.
  FVector Wheel(4600,5400,2200);for(int Side:{-1,1})Box(TEXT("Steel"),FVector(4600+Side*650,5400,1100),FVector(100,180,2200));
  for(int I=0;I<16;I++){float A=I*2*PI/16,B=(I+1)*2*PI/16;FVector V=Wheel+FVector(FMath::Cos(A)*1900,0,FMath::Sin(A)*1900),Next=Wheel+FVector(FMath::Cos(B)*1900,0,FMath::Sin(B)*1900);FVector D=Next-V;C->Box(W,TEXT("Steel"),At((V+Next)*.5f),FVector(D.Size(),45,45),R+D.Rotation());FVector Spoke=V-Wheel;C->Box(W,TEXT("Steel"),At((V+Wheel)*.5f),FVector(Spoke.Size(),20,20),R+Spoke.Rotation());Box(TEXT("Red"),V-FVector(0,0,100),FVector(240,260,160));}
  Label(TEXT("SKY WHEEL / MAINTENANCE ACCESS"),FVector(4600,3100,340),32);Shell(FVector(8100,5500,0),1000,1700,TEXT("RIDE ENGINEERING"));Furn(TEXT("workbench"),FVector(8400,6200,24));Loot(TEXT("EvidenceCabinetV18"),FVector(8400,4300,24),2);
  for(int I=0;I<5;I++)Table(FVector(-650,-3200+I*900,30));
 }
 const bool SurvivorStop=LWPOISurvivors::Selected(S,W->Seed);
 if(SurvivorStop)LWPOISurvivors::Spawn(C,W,S);
 W->SpawnSiteLoot(C,S);
 // Hostile sites retain their existing enemy population; survivor stops stay approachable.
 if(!SurvivorStop){
  W->SpawnEnemies(C,&S,S.Size,1,360);
  // Cooked spawn tables can predate the appended IDs. Reuse a known row until
  // designers author dedicated rows; retain site ID, transform and footprint.
  if(W->SpawnTable&&!W->SpawnTable->POIs.ContainsByPredicate([&](const auto& Row){return Row.POIType==S.Type;})){
   auto CombatSite=S;CombatSite.Type=S.Type==LWPlaces::Prison?17:13;W->SpawnEnemies(C,&CombatSite,S.Size,1,360);
  }
 }
}
