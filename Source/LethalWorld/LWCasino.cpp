#include "LWInteriors65.h"
#include "LWWorld.h"
#include "LWVehicle.h"
#include "LWResident.h"
#include "LWCardGame.h"
#include "LWSlotMachine.h"
#include "Components/StaticMeshComponent.h"
#include "LWWorldTextComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"
void ALWChunk::BuildingCasino(ALWWorld* W,const LWGen::FSite& S){
 LWInteriors65::FScope Interior65(this,W,S);
 const FRotator R(0,S.Yaw,0);const FVector Base=FVector(S.Position,12)-GetActorLocation();int Serial=0;FRandomStream Random(S.Id);
 auto At=[&](FVector V){return Base+R.RotateVector(V+FVector(0,2000,0));};
 auto B=[&](FName M,FVector V,FVector Size){Box(W,M,At(V),Size,R);};
 auto Model=[&](FName M,FVector V,float Yaw=0){Add(W,M,NAME_None,At(V),FVector(1),R+FRotator(0,Yaw,0));};
 auto Id=[&](){return FName(*FString::Printf(TEXT("resort_%u_%d"),S.Id,Serial++));};
 auto Object=[&](ELWObjectKind K,FVector V,float Yaw=0){auto* O=W->SpawnObject(K,Id(),GetActorLocation()+At(V),R+FRotator(0,Yaw,0));if(O)Residents.Add(O);return O;};
 auto Furniture=[&](FName Type,FVector V,float Yaw=0){if(auto* O=Object(ELWObjectKind::Furniture,V,Yaw))O->SetFurniture(Type);};
 auto Chair=[&](FVector V,float Yaw=0){if(auto* O=Object(ELWObjectKind::Furniture,V,Yaw)){O->SetFurniture(TEXT("chair"));O->Body->SetStaticMesh(W->Mesh(TEXT("CasinoChairV21")));O->Body->SetRelativeRotation(FRotator::ZeroRotator);}};
 auto Loot=[&](FName M,FVector V,int Lock=0){if(auto* O=Object(ELWObjectKind::Container,V)){W->EnsureSiteContainer(O->RecordId,S,O->GetActorLocation(),Lock);O->Body->SetStaticMesh(W->Mesh(M));}};
 auto Text=[&](FString Words,FVector V,float Size=35){auto* T=NewObject<ULWWorldTextComponent>(this);T->SetupAttachment(RootComponent);T->SetRelativeLocation(At(V));T->SetRelativeRotation(R+FRotator(0,-90,0));T->SetHorizontalAlignment(EHTA_Center);T->SetWorldSize(Size);T->SetTextRenderColor(FColor(235,190,90));T->SetText(FText::FromString(Words));T->RegisterComponent();};
 auto Glass=[&](FVector V,FVector Size){FName Key(*FString::Printf(TEXT("casino_glass_%u_%d_%d_%d"),S.Id,int(V.X),int(V.Y),int(V.Z)));if(auto* O=W->SpawnObject(ELWObjectKind::Window,Key,GetActorLocation()+At(V),R)){O->Body->SetRelativeScale3D(Size/100);Residents.Add(O);}};
 auto Wall=[&](FVector V,float Length,bool Door=false){if(!Door){B(TEXT("CasinoVelvet21"),V+FVector(0,0,200),FVector(Length,18,408));return;}for(int Side:{-1,1})B(TEXT("CasinoVelvet21"),V+FVector(Side*(Length+160)*.25f,0,200),FVector((Length-160)*.5f,18,408));B(TEXT("CasinoVelvet21"),V+FVector(0,0,318),FVector(160,18,172));Object(ELWObjectKind::Door,V-FVector(80,0,0));};
 auto Light=[&](FVector V){Model(TEXT("CasinoChandelierV21"),V);auto* L=NewObject<UPointLightComponent>(this);L->SetupAttachment(RootComponent);L->SetRelativeLocation(At(V-FVector(0,0,120)));L->SetIntensity(55000);L->SetAttenuationRadius(1900);L->SetLightColor(FLinearColor(1,.72f,.35f));L->SetCastShadows(false);L->SetMaxDrawDistance(4000);L->RegisterComponent();};
 auto NPC=[&](FVector V,FName Job,FString Name){auto* N=GetWorld()->SpawnActor<ALWResident>(GetActorLocation()+At(V+FVector(0,0,100)),R);if(N){N->ConfigureResident(Id(),Job,Name,Serial);N->ActivitySpots={N->Home,N->Home+R.RotateVector(FVector(250,0,0)),N->Home+R.RotateVector(FVector(-250,0,0))};Residents.Add(N);}};
 // Generous forecourt, two parking banks and clear central approach.
 B(TEXT("Asphalt"),FVector(0,-6500,-2),FVector(11000,5000,20));B(TEXT("Asphalt"),FVector(0,-9185,-2),FVector(650,370,20));
 for(int Side:{-1,1})for(int Row=0;Row<3;Row++)for(int Col=0;Col<4;Col++){
  FVector V(Side*(1700+Col*900),-4700-Row*1700,18);B(TEXT("Lane"),V+FVector(350,0,-6),FVector(9,700,1));
  const uint32 H=LWGen::Hash(Row*8+Col*2+(Side+1)/2,0,int32(S.Id),2121);
  if(H%100<65){FName VIN(*FString::Printf(TEXT("resort_%u_car_%d_%d_%d"),S.Id,Side,Row,Col));if(!W->Vehicles.Contains(VIN)){FLWVehicleRecord Car;Car.VIN=FGuid::NewDeterministicGuid(VIN.ToString(),uint64(uint32(W->Seed)));Car.Model=H%10<6?TEXT("supercar"):H%10<8?TEXT("muscle"):TEXT("suv");Car.Position=GetActorLocation()+At(V+FVector(0,0,57));Car.Rotation=R+FRotator(0,90,0);Car.LockTier=2;Car.Unlocked=false;W->Vehicles.Add(VIN,Car);}if(auto* C=W->SpawnObject(ELWObjectKind::Car,VIN,GetActorLocation()+At(V),R))Residents.Add(C);}
 }
 for(int Side:{-1,1})for(int I=0;I<3;I++)StreetLight(W,At(FVector(Side*5200,-4800-I*1700,0)),R);
 B(TEXT("CasinoMarble21"),FVector(0,-3900,4),FVector(11000,700,24));
 B(TEXT("CasinoGold21"),FVector(0,-4170,430),FVector(1800,480,30));Text(TEXT("THE GILDED REPUBLIC"),FVector(0,-4430,365),70);
 for(int Side:{-1,1}){Model(TEXT("CasinoColumnV21"),FVector(Side*750,-4300,18));Model(TEXT("PlazaPlanterV18"),FVector(Side*1100,-4300,18));}
 constexpr float FH=420;constexpr int Floors=7;
 static const TCHAR* Names[]={TEXT("CASINO / TABLE GAMES"),TEXT("THE ROYAL FLOOR / SLOTS"),TEXT("THE PROMENADE / SHOPS"),TEXT("RESTAURANTS & LOUNGE"),TEXT("HOTEL / ROOMS 401-416"),TEXT("HOTEL / ROOMS 501-516"),TEXT("PRESIDENTIAL CLUB / VIP")};
 for(int F=0;F<Floors;F++){
  float Z=F*FH+24;
  // Cove trim, structural piers and wall-mounted art close the facade and give circulation a finished edge.
  for(int Side:{-1,1}){B(TEXT("CasinoGold21"),FVector(Side*4968,0,Z+395),FVector(28,8000,26));for(int J=0;J<6;J++){FVector V(Side*4900,-3200+J*1250,Z);B(TEXT("CasinoGold21"),V+FVector(0,0,240),FVector(25,340,210));B(TEXT("CasinoVelvet21"),V+FVector(-Side*16,0,240),FVector(8,310,180));Model(TEXT("PlazaPlanterV18"),V+FVector(-Side*120,0,0));}}
  for(int J=0;J<4;J++){Loot(TEXT("CabinetV3"),FVector(-4600+J*1800,3650,Z),F>3?2:0);}

  if(F==2)for(int Side:{-1,1})B(TEXT("CasinoVelvet21"),FVector(2730,Side*2400,Z+204),FVector(18,2800,408));
  if(F==4||F==5)for(int Side:{-1,1})B(TEXT("CasinoVelvet21"),FVector(3300,Side*2450,Z+204),FVector(24,3100,408));
  // Floor opening is retained on every level for the incoming switchback stair.
  B(TEXT("CasinoMarble21"),FVector(-675,0,Z-8),FVector(8650,8000,16));
  B(TEXT("CasinoMarble21"),FVector(4325,-2210,Z-8),FVector(1350,3580,16));B(TEXT("CasinoMarble21"),FVector(4325,2200,Z-8),FVector(1350,3600,16));
  for(int Side:{-1,1}){B(TEXT("CasinoMarble21"),FVector(Side*5000,0,Z+210),FVector(48,8000,420));
   for(int I=0;I<10;I++){float X=-4500+I*1000;if(!(F==0&&I==5&&Side<0)){Glass(FVector(X,Side*4000,Z+200),FVector(960,22,280));B(TEXT("CasinoMarble21"),FVector(X,Side*4000,Z+30),FVector(960,24,60));}B(TEXT("CasinoGold21"),FVector(X-500,Side*4000,Z+210),FVector(48,48,420));}
   B(TEXT("CasinoGold21"),FVector(0,Side*4000,Z+370),FVector(10000,40,80));
  }
  // Main entrance bay is fitted around a standard working door.
  if(F==0){Wall(FVector(500,-4000,Z),1000,true);}
  else B(TEXT("CasinoVelvet21"),FVector(0,-4000,Z+30),FVector(10000,24,60));
  B(TEXT("CasinoVelvet21"),FVector(0,4000,Z+30),FVector(10000,24,60));
  B(TEXT("CasinoVelvet21"),FVector(-750,0,Z+2),FVector(8100,1100,4));
  for(float X:{-3600.f,-1300.f,1000.f,3300.f})for(float Y:{-2800.f,0.f,2800.f})Light(FVector(X,Y,Z+385));
  for(float X:{-3000.f,0.f,2800.f})for(float Y:{-650.f,650.f})Model(TEXT("CasinoColumnV21"),FVector(X,Y,Z));
  B(TEXT("CasinoVelvet21"),FVector(-700,655,Z+285),FVector(2300,24,100));for(int Side:{-1,1})B(TEXT("CasinoGold21"),FVector(-700+Side*1000,655,Z+368),FVector(12,12,80));Text(Names[F],FVector(-700,630,Z+270),42);Text(TEXT("STAIRS >"),FVector(3000,630,Z+240),30);
  B(TEXT("CasinoGold21"),FVector(3650,-30,Z+60),FVector(10,760,120));
  if(F<Floors-1){for(int I=0;I<12;I++){float H=(I+1)*FH/24;B(TEXT("CasinoMarble21"),FVector(3975,-420+(I+.5f)*32,Z+H-10),FVector(610,32,20));B(TEXT("CasinoMarble21"),FVector(4660,-36-(I+.5f)*32,Z+FH*.5f+H-10),FVector(610,32,20));}B(TEXT("CasinoMarble21"),FVector(4325,180,Z+FH*.5f-10),FVector(1350,432,20));}
  if(F<=1){
   // Extra playable banks use their own IDs; the established resort loot and
   // table IDs below must remain stable in existing saves.
   for(int Row=0;Row<2;Row++)for(int Col=0;Col<7;Col++)for(int Bank=0;Bank<4;Bank++){
    const FVector V(-3900+Col*1000+(Bank-1.5f)*150,1625+Row*850,Z);
    const FName Key(*FString::Printf(TEXT("casino65_slot_%u_%d_%d_%d_%d"),S.Id,F,Row,Col,Bank));
    if(auto* Slot=GetWorld()->SpawnActor<ALWSlotMachine>(GetActorLocation()+At(V),R)){Slot->Setup(W,Key);Residents.Add(Slot);}
    if(auto* Seat=W->SpawnObject(ELWObjectKind::Furniture,FName(*(Key.ToString()+TEXT("_seat"))),GetActorLocation()+At(V+FVector(0,-135,0)),R)){Seat->SetFurniture(TEXT("chair"));Seat->Body->SetStaticMesh(W->Mesh(TEXT("CasinoChairV21")));Seat->Body->SetRelativeRotation(FRotator::ZeroRotator);Seat->SetActorTickEnabled(false);Residents.Add(Seat);}
   }
   for(int Col=0;Col<4;Col++){
    const FVector V(-3600+Col*1800,-2425,Z);const FName Key(*FString::Printf(TEXT("casino65_table_%u_%d_%d"),S.Id,F,Col));
    if(auto* Table=GetWorld()->SpawnActor<ALWCardTable>(GetActorLocation()+At(V),R)){Table->Setup(W,Key,Col%3);Residents.Add(Table);}
    for(int Side:{-1,1})if(auto* Seat=W->SpawnObject(ELWObjectKind::Furniture,FName(*FString::Printf(TEXT("%s_seat_%d"),*Key.ToString(),Side)),GetActorLocation()+At(V+FVector(0,Side*150,0)),R+FRotator(0,Side>0?180:0,0))){Seat->SetFurniture(TEXT("chair"));Seat->Body->SetStaticMesh(W->Mesh(TEXT("CasinoChairV21")));Seat->Body->SetRelativeRotation(FRotator::ZeroRotator);Seat->SetActorTickEnabled(false);Residents.Add(Seat);}
   }
   for(int Row=0;Row<3;Row++)for(int Col=0;Col<7;Col++)for(int Bank=0;Bank<4;Bank++){FVector V(-3900+Col*1000+(Bank-1.5f)*150,1200+Row*850,Z);auto* Slot=GetWorld()->SpawnActor<ALWSlotMachine>(GetActorLocation()+At(V),R);if(Slot){Slot->Setup(W,Id());Residents.Add(Slot);}Chair(V+FVector(0,-135,0));}
   for(int Row=0;Row<2;Row++)for(int Col=0;Col<4;Col++)for(int Pair=0;Pair<2;Pair++){FVector V(-3600+Col*1800+(Pair?350:-350),-1700-Row*1450,Z);auto* Table=GetWorld()->SpawnActor<ALWCardTable>(GetActorLocation()+At(V),R);if(Table){Table->Setup(W,Id(),Col%3);Residents.Add(Table);}for(int Side:{-1,1})Chair(V+FVector(0,Side*150,0),Side>0?180:0);}
   Loot(TEXT("CasinoCounterV21"),FVector(-4300,-500,Z));Text(TEXT("CASHIER"),FVector(-4300,-550,Z+175));if(F==0)NPC(FVector(-4300,-300,Z),TEXT("civilian"),TEXT("Marlowe Voss"));
  }else if(F==2){for(int I=0;I<4;I++){float X=-3700+I*1850;Wall(FVector(X,1000,Z),1750,true);Wall(FVector(X,-1000,Z),1750,true);B(TEXT("CasinoVelvet21"),FVector(X-880,2400,Z+204),FVector(18,2800,408));B(TEXT("CasinoVelvet21"),FVector(X-880,-2400,Z+204),FVector(18,2800,408));Text(I%2?TEXT("TAILOR & SUPPLIES"):TEXT("REPUBLIC GIFTS"),FVector(X,985,Z+280),25);for(int Side:{-1,1}){Loot(TEXT("CasinoCounterV21"),FVector(X,Side*1800,Z));Loot(TEXT("Shelf"),FVector(X-500,Side*3300,Z));Loot(TEXT("BookcaseV13"),FVector(X+250,Side*3300,Z));}NPC(FVector(X,2100,Z),TEXT("merchant"),FString::Printf(TEXT("%s %s"),I%2?TEXT("Ellis"):TEXT("Morgan"),I<2?TEXT("Avery"):TEXT("Sloan")));}}
  else if(F==3||F==6){for(int Row=0;Row<3;Row++)for(int I=0;I<5;I++){FVector V(-3900+I*1450,1200+Row*900,Z);Model(TEXT("DiningTableV13"),V);for(int Side:{-1,1})Chair(V+FVector(0,Side*140,0),Side>0?180:0);}
   Wall(FVector(-750,-1500,Z),8100,true);for(int I=0;I<5;I++){FVector V(-3900+I*1450,-3200,Z);Furniture(I%2?TEXT("sink"):TEXT("cooker"),V);Loot(TEXT("CabinetV3"),V+FVector(450,0,0),F==6?3:0);Model(TEXT("CasinoBarShelfV21"),V+FVector(0,600,0));}Text(F==6?TEXT("MEMBERS LOUNGE"):TEXT("THE EXECUTIVE / DINING"),FVector(-700,900,Z+290),45);
   if(F==6){auto* Table=GetWorld()->SpawnActor<ALWCardTable>(GetActorLocation()+At(FVector(-1300,-900,Z)),R);if(Table){Table->Setup(W,Id(),0);Residents.Add(Table);}Loot(TEXT("EvidenceCabinetV18"),FVector(2700,-3400,Z),4);Furniture(TEXT("bed"),FVector(1800,-3000,Z));}
  }else{for(int Side:{-1,1})for(int I=0;I<8;I++){float X=-4200+I*1000;Wall(FVector(X,Side*900,Z),1000,true);Wall(FVector(X,Side*3000,Z),1000,true);B(TEXT("CasinoVelvet21"),FVector(X-500,Side*2450,Z+204),FVector(24,3100,408));Text(FString::Printf(TEXT("%d"),F*100+I+1+(Side>0?8:0)),FVector(X,Side*900-15,Z+270),28);Furniture(TEXT("bed"),FVector(X-150,Side*2350,Z));Loot(TEXT("CabinetV3"),FVector(X+250,Side*3300,Z),I%3);Model(TEXT("Desk"),FVector(X+400,Side*1650,Z));Chair(FVector(X+400,Side*1450,Z));Furniture(TEXT("sink"),FVector(X-350,Side*3650,Z));Model(TEXT("ToiletV4"),FVector(X+350,Side*3650,Z));}}
 }
 B(TEXT("CasinoMarble21"),FVector(0,0,Floors*FH+24),FVector(10100,8100,28));
 Text(TEXT("GILDED REPUBLIC"),FVector(0,-4040,Floors*FH-150),140);
 for(int I=0;I<5;I++)StreetLight(W,At(FVector(-4000+I*2000,-4040,410)),R,true);
}
