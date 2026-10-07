#include "LWInteriors65.h"
#include "LWWorld.h"
#include "LWWorldTextComponent.h"
#include "LWSiteIdentity.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"

// Integrated retail wings share the main envelope; no freestanding buildings inside a shed.
void LWInteriors65::Mall(ALWChunk* C,ALWWorld* W,const LWGen::FSite& S){
 const FRotator R(0,S.Yaw,0);const FVector Base=FVector(S.Position,12)-C->GetActorLocation();int Serial=0;
 auto At=[&](FVector V){return Base+R.RotateVector(V);};
 auto B=[&](FName M,FVector P,FVector Size,float Yaw=0){C->Box(W,M,At(P),Size,R+FRotator(0,Yaw,0));};
 auto M=[&](FName N,FVector P,float Yaw=0){C->Add(W,N,NAME_None,At(P),FVector(1),R+FRotator(0,Yaw,0));};
 auto O=[&](FName N,FVector P,float Yaw=0,FName Use=TEXT("storage")){auto* A=W->SpawnObject(Use==TEXT("storage")?ELWObjectKind::Container:ELWObjectKind::Furniture,FName(*FString::Printf(TEXT("mall65_%u_%d"),S.Id,Serial++)),C->GetActorLocation()+At(P),R+FRotator(0,Yaw,0));if(A){A->SetActorTickEnabled(false);A->Body->SetCullDistance(11000);if(Use!=TEXT("storage"))A->SetFurniture(Use);A->Body->SetStaticMesh(W->Mesh(N));A->Body->SetRelativeRotation(FRotator::ZeroRotator);if(Use==TEXT("storage"))W->EnsureSiteContainer(A->RecordId,S,A->GetActorLocation());C->Residents.Add(A);}};
 auto Text=[&](FString Words,FVector P,float Yaw=-90,float Size=28){auto* T=NewObject<ULWWorldTextComponent>(C);T->SetupAttachment(C->GetRootComponent());T->SetRelativeLocation(At(P));T->SetRelativeRotation(R+FRotator(0,Yaw,0));T->SetText(FText::FromString(Words));T->SetWorldSize(Size);T->SetHorizontalAlignment(EHTA_Center);T->SetCullDistance(8000);T->RegisterComponent();};
 auto Wall=[&](FVector P,float Length,float Yaw=0,bool Door=false,float H=600){auto Seg=[&](float X,float L,float Z,float Height){B(TEXT("PlasterV7"),P+FRotator(0,Yaw,0).RotateVector(FVector(X,0,Z)),FVector(L,24,Height),Yaw);};if(!Door)Seg(0,Length,H*.5,H);else{for(int Side:{-1,1})Seg(Side*(Length+320)*.25,(Length-320)*.5,H*.5,H);Seg(0,320,(H+270)*.5,H-270);}};
 auto Light=[&](FVector P){M(TEXT("TubeLight65"),P);auto* L=NewObject<UPointLightComponent>(C);L->SetupAttachment(C->GetRootComponent());L->SetRelativeLocation(At(P-FVector(0,0,20)));L->SetIntensity(22000);L->SetAttenuationRadius(1500);L->SetCastShadows(false);L->SetMaxDrawDistance(4500);L->RegisterComponent();};
 constexpr float X=7600,Front=-5500,Back=6900,Ceiling=624;
 B(TEXT("TileV7"),FVector(0,700,12),FVector(2*X,Back-Front,24));
 for(int Side:{-1,1}){Wall(FVector(Side*X,700,24),Back-Front,90);Wall(FVector(Side*(X+600)*.5f,Front,24),X-600);}
 Wall(FVector(0,Back,24),2*X);B(TEXT("PlasterV7"),FVector(0,Front,494),FVector(1200,24,260));
 // Raised central skylight with continuous opaque ceilings over the occupied wings.
 for(int Side:{-1,1})B(TEXT("PlasterV7"),FVector(Side*(X+1100)*.5,700,Ceiling),FVector(X-1100,Back-Front,28));
 B(TEXT("Glass"),FVector(0,700,Ceiling+7),FVector(2200,Back-Front,14));
 for(int I=0;I<=12;I++)B(TEXT("Steel"),FVector(0,Front+I*(Back-Front)/12,Ceiling),FVector(2225,26,35));
 for(int Side:{-1,1})B(TEXT("Steel"),FVector(Side*1100,700,Ceiling),FVector(28,Back-Front,35));
 Text(LWSites::Label(S),FVector(0,Front-25,435),-90,58);
 const TCHAR* Stores[2][6]={{TEXT("VANTA / FASHION"),TEXT("PAPER TRAIL BOOKS"),TEXT("CIRCUIT CITY SURPLUS"),TEXT("WELLSPRING PHARMACY"),TEXT("TRAILBOUND OUTDOORS"),TEXT("HOME & HEARTH")},{TEXT("FOOTWORK SHOES"),TEXT("RECORD EXCHANGE"),TEXT("THE WATCH COUNTER"),TEXT("TOY CHEST"),TEXT("HEALTH & BEAUTY"),TEXT("REPUBLIC GIFTS")}};
 for(int Side:{-1,1})for(int I=0;I<6;I++){
  const float Y=-3800+I*1650;const float CX=Side*3800;const float Face=Side<0?0:180;
  // Shop walls, stockroom walls and finishes all terminate at a continuous ceiling.
  for(int Edge:{-1,1})Wall(FVector(Side*4600,Y+Edge*825,24),6000);
  // Glazed shopfronts expose merchandise to the gallery instead of reading as blank corridors.
  B(TEXT("PlasterV7"),FVector(Side*1600,Y,459),FVector(24,1650,330));
  for(int End:{-1,1}){B(TEXT("Steel"),FVector(Side*1600,Y+End*485,44),FVector(28,650,40));
   for(int Pane=0;Pane<4;Pane++){const float PY=Y+End*(240+Pane*162);auto* G=W->SpawnObject(ELWObjectKind::Window,FName(*FString::Printf(TEXT("mall65_glass_%u_%d_%d_%d_%d"),S.Id,Side,I,End,Pane)),C->GetActorLocation()+At(FVector(Side*1600,PY,179)),R);if(G){G->Body->SetRelativeScale3D(FVector(.12,1.5,2.3));C->Residents.Add(G);}B(TEXT("Steel"),FVector(Side*1600,PY+81,169),FVector(30,12,250));}
  }Wall(FVector(Side*6000,Y,24),1650,90,true);
  B(TEXT("Acoustic65"),FVector(Side*3800,Y,444),FVector(4376,1626,18));
  B(TEXT("Acoustic65"),FVector(Side*6800,Y,414),FVector(1576,1626,18));
  Text(Stores[Side>0?1:0][I],FVector(Side*1580,Y,342),Face,31);
  const FName Goods[]={TEXT("ClothesRail65"),TEXT("Bookcase65"),TEXT("Shelf65"),TEXT("Cabinet65"),TEXT("SportsRack57"),TEXT("Sideboard65")};
  for(int Row=0;Row<9;Row++)for(int Bank:{-1,1}){const float RX=Side*(2750+Row*350);O(Goods[(I+(Side>0?2:0))%6],FVector(RX,Y+Bank*490,24),Side<0?90:-90);}
  for(int Row=0;Row<3;Row++)O(TEXT("Shelf65"),FVector(Side*7500,Y-440+Row*420,24),Side<0?90:-90);
  O(TEXT("Counter65"),FVector(Side*2300,Y-500,24),Side<0?-90:90);M(TEXT("CashRegister53"),FVector(Side*2300,Y-500,126),Side<0?-90:90);
  O(TEXT("Pallet65"),FVector(Side*6900,Y-450,24));O(TEXT("Locker65"),FVector(Side*7250,Y+500,24),Face);
  O(TEXT("CleaningCart65"),FVector(Side*6500,Y+550,24));M(TEXT("Noticeboard65"),FVector(Side*7550,Y,170),Side<0?90:-90);
  for(float XX:{2400.f,4400.f,6750.f})Light(FVector(Side*XX,Y,XX>6000?401:431));
  // Gallery seating occupies the edges, leaving a continuous eight-metre through route.
  if(I%2==0){O(TEXT("WaitingBench65"),FVector(Side*1050,Y+220,24),Side<0?90:-90,TEXT("chair"));M(TEXT("Planter65"),FVector(Side*1170,Y-200,24));}
 }
 // Merchandise kiosks and paired seating break up the concourse while two
 // broad through routes connect every storefront and the food hall.
 for(int K=0;K<5;K++){const float KY=-3400+K*1250;
  B(TEXT("CasinoMarble21"),FVector(0,KY,25),FVector(660,720,2));
  for(int Side:{-1,1}){O(TEXT("Counter65"),FVector(Side*215,KY,26),Side<0?90:-90);M(K%2?TEXT("Books65"):TEXT("Bottles65"),FVector(Side*215,KY,129),Side<0?90:-90);O(TEXT("WaitingBench65"),FVector(Side*410,KY+330,24),Side<0?90:-90,TEXT("chair"));}
  M(TEXT("Planter65"),FVector(0,KY+350,24));M(TEXT("Planter65"),FVector(0,KY-350,24));
 }
 // Entrance welcome desk and directory are backed by actual freestanding fixtures.
 O(TEXT("Counter65"),FVector(-1100,-4950,24));M(TEXT("Desktop65"),FVector(-1100,-4950,126));M(TEXT("Noticeboard65"),FVector(1100,-4950,140));for(int Side:{-1,1}){B(TEXT("Steel"),FVector(1100+Side*40,-4945,95),FVector(6,6,142));B(TEXT("Steel"),FVector(1100+Side*40,-4945,27),FVector(12,65,6));}M(TEXT("CoatRack65"),FVector(-1450,-4950,24));
 Text(TEXT("INFORMATION"),FVector(-1100,-4985,230),-90,22);
 // Food hall fills the north end with dining, service counters and a closed kitchen.
 Wall(FVector(0,5600,24),3100,0,true);Wall(FVector(-1550,6150,24),1100,90);Wall(FVector(1550,6150,24),1100,90);B(TEXT("Acoustic65"),FVector(0,6150,424),FVector(3100,1100,20));
 for(int I=-1;I<=1;I++){O(TEXT("Counter65"),FVector(I*850,5500,24));O(TEXT("Stove65"),FVector(I*850,6600,24),0,TEXT("cooker"));O(TEXT("Sink65"),FVector(I*850+220,6600,24),0,TEXT("sink"));Light(FVector(I*900,6200,408));}
 for(int Row=0;Row<3;Row++)for(int Side:{-1,1}){const FVector P(Side*1000,3500+Row*610,24);M(TEXT("DiningTable65"),P);for(int Chair:{-1,1})O(TEXT("Chair65"),P+FVector(0,Chair*125,0),Chair>0?0:180,TEXT("chair"));M(TEXT("Bottles65"),P+FVector(0,0,79));}
 Text(TEXT("NORTH FOOD HALL"),FVector(0,5585,335),-90,30);
 // Two real service rooms behind the last shop banks, with fitted openings and doors.
 for(int Side:{-1,1}){FVector P(Side*4550,6100,24);Wall(P+FVector(0,-650,0),5500,0,true);Wall(P+FVector(-Side*2750,75,0),1450,90);B(TEXT("Acoustic65"),P+FVector(0,75,390),FVector(5500,1450,20));Text(Side<0?TEXT("RESTROOMS / FAMILY ROOM"):TEXT("MALL OPERATIONS / SECURITY"),P-FVector(0,665,-290),-90,27);
  if(Side<0){for(int I=0;I<6;I++){const FVector V(-6550+I*760,6500,24);M(TEXT("Toilet65"),V);Wall(V+FVector(360,-150,0),800,90,false,350);O(TEXT("Sink65"),V-FVector(0,750,0),0,TEXT("sink"));}}
  else for(int I=0;I<5;I++){O(TEXT("Workstation65"),FVector(2650+I*850,6500,24));O(TEXT("Locker65"),FVector(2650+I*850,5650,24));}
  for(int I=-2;I<=2;I++)Light(P+FVector(I*1000,0,377));
 }
 for(int I=0;I<7;I++)Light(FVector(0,-4800+I*1600,604));
}
