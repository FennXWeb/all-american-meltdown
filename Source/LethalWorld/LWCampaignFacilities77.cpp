#include "LWCampaignProduction77.h"
#include "LWCampaign76.h"
#include "LWCampaign76Corridor.h"
#include "LWWorld.h"
#include "LWWorldTextComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"

bool LWProduction77::Centered(FName Id){return Id==TEXT("rome")||Id==TEXT("meridian")||Id==TEXT("labor")||Id==TEXT("harbor");}
FVector2D LWProduction77::Parcel(FName Id){return Id==TEXT("meridian")?FVector2D(8000,7000):Id==TEXT("rome")?FVector2D(6400,5200):Id==TEXT("labor")?FVector2D(6000,4800):FVector2D(4400,3800);}
namespace {
struct FFacility77 {
 ALWChunk* C;ALWWorld* W;FTransform Frame;bool Clean=false;
 FVector At(FVector P)const{return Frame.TransformPosition(P)-C->GetActorLocation();}
 void Box(FName Mat,FVector P,FVector Size,bool Solid=true){if(Clean){if(Mat==TEXT("PlasterV7")||Mat==TEXT("Concrete"))Mat=TEXT("RV66_Ivory");else if(Mat==TEXT("TileV7"))Mat=TEXT("RV66_Quartz");else if(Mat==TEXT("Wood"))Mat=TEXT("RV66_Walnut");}C->Box(W,Mat,At(P),Size,Frame.Rotator(),Solid);}
 void Prop(FName Name,FVector P,float Yaw=0,float Scale=1){if(auto* M=W->Mesh(Name)){P.Z-=M->GetBoundingBox().Min.Z*Scale;C->Add(W,Name,NAME_None,At(P),FVector(Scale),Frame.Rotator()+FRotator(0,Yaw,0));}}
 void Sign(const FString& Text,FVector P,float Yaw=-90,float Size=16){auto* T=NewObject<ULWWorldTextComponent>(C);T->SetupAttachment(C->GetRootComponent());T->SetRelativeLocation(At(P));T->SetRelativeRotation(Frame.Rotator()+FRotator(0,Yaw,0));T->SetText(FText::FromString(Text));T->SetWorldSize(Size);T->SetHorizontalAlignment(EHTA_Center);T->RegisterComponent();}
 void Light(FVector P){Prop(TEXT("CeilingLightV13"),P);auto* L=NewObject<UPointLightComponent>(C);L->SetupAttachment(C->GetRootComponent());L->SetRelativeLocation(At(P-FVector(0,0,24)));L->SetIntensity(16000);L->SetAttenuationRadius(1050);L->SetLightColor(FLinearColor(1,.9,.76));L->SetCastShadows(false);L->RegisterComponent();}
 void Door(FVector P,float Yaw){auto* A=W->SpawnObject(ELWObjectKind::Door,FName(*FString::Printf(TEXT("c77_%u_%d"),C->GetUniqueID(),C->Residents.Num())),Frame.TransformPosition(P),Frame.Rotator()+FRotator(0,Yaw,0));if(A)C->Residents.Add(A);}
 void WallY(float X,float Y,float Length,float H=360,float Opening=0){
  if(Opening<=0){Box(TEXT("PlasterV7"),FVector(X,Y,H/2),FVector(Length,18,H));return;}
  for(int Side:{-1,1})Box(TEXT("PlasterV7"),FVector(X+Side*(Length+Opening)/4,Y,H/2),FVector((Length-Opening)/2,18,H));
  Box(TEXT("PlasterV7"),FVector(X,Y,(H+258)/2),FVector(Opening,18,H-258));
 }
 void WallX(float X,float Y,float Length,float H=360,float Opening=0){
  if(Opening<=0){Box(TEXT("PlasterV7"),FVector(X,Y,H/2),FVector(18,Length,H));return;}
  for(int Side:{-1,1})Box(TEXT("PlasterV7"),FVector(X,Y+Side*(Length+Opening)/4,H/2),FVector(18,(Length-Opening)/2,H));
  Box(TEXT("PlasterV7"),FVector(X,Y,(H+258)/2),FVector(18,Opening,H-258));
 }
 void Shell(float Width,float Depth,float Height=360){
  Box(TEXT("Concrete"),FVector(0,0,-14),FVector(Width,Depth,28));Box(TEXT("TileV7"),FVector(0,0,1),FVector(Width-20,Depth-20,2));
  WallX(-Width/2,0,Depth,Height);WallX(Width/2,0,Depth,Height);WallY(0,Depth/2,Width,Height);WallY(0,-Depth/2,Width,Height,160);Door(FVector(-80,-Depth/2,0),0);
  Box(TEXT("Concrete"),FVector(0,0,Height+12),FVector(Width+32,Depth+32,24));
  for(float Y=-Depth/2+600;Y<Depth/2;Y+=900)Light(FVector(0,Y,Height-8));
  for(int Side:{-1,1})Box(TEXT("Wood"),FVector(Side*(Width/2-12),0,7),FVector(8,Depth-24,14));
 }
 void Bedroom(FVector P){
  Prop(TEXT("Bed65"),P+FVector(-360,0,0));Prop(TEXT("Nightstand65"),P+FVector(-470,120,0));Prop(TEXT("Locker65"),P+FVector(420,280,0),180);Prop(TEXT("Desk65"),P+FVector(340,-240,0));Prop(TEXT("Chair65"),P+FVector(340,-360,0));Prop(TEXT("Rug65"),P+FVector(0,-180,1));Prop(TEXT("Artwork65"),P+FVector(0,480,130));
 }
};
}
bool LWProduction77::Dress(ALWChunk* C,ALWWorld* W,const LWGen::FSite& S){
 FName Site;for(const auto& Stop:LWCampaign76::Corridor())if(LWCampaign76::SiteId(Stop.Id)==S.Id){Site=Stop.Id;break;}
 if(S.Id==0xEC76FF00u)Site=TEXT("canada");
 if(!Centered(Site)&&Site!=TEXT("canada"))return false;
 FFacility77 B{C,W,FTransform(FRotator(0,S.Yaw,0),FVector(S.Position,36))};
 if(Site==TEXT("canada")){
  B.Clean=true;
  B.Shell(2200,1800,350);B.Sign(TEXT("NORTHBANK / RECEPTION & FAMILY TRACING"),FVector(0,-914,290),-90,12);
  for(int Side:{-1,1}){
   B.WallX(Side*490,610,580,350);B.WallY(Side*790,320,590,350,160);
   if(Side<0)B.Prop(TEXT("Bed65"),FVector(Side*760,600,0));B.Prop(TEXT("Nightstand65"),FVector(Side*1000,650,0));
   B.Prop(TEXT("PrivacyScreen77"),FVector(Side*480,-220,0),90);B.Prop(TEXT("Bookcase65"),FVector(Side*1010,-280,0),Side>0?-90:90);
   B.Prop(TEXT("WaitingBench65"),FVector(Side*600,-670,0));B.Prop(TEXT("CoatRack65"),FVector(Side*980,-720,0));B.Light(FVector(Side*750,590,342));
  }
  B.Prop(TEXT("TracingDesk77"),FVector(0,290,0));B.Prop(TEXT("Chair65"),FVector(0,430,0),180);
  B.Prop(TEXT("CoffeeStation65"),FVector(970,170,0),-90);
  B.Prop(TEXT("Workstation65"),FVector(-790,-250,0),90);B.Prop(TEXT("TracingDesk77"),FVector(790,-250,0),-90);
  B.Sign(TEXT("REST ROOMS"),FVector(0,886,285));B.Sign(TEXT("TRACING / PRIVATE INTERVIEWS"),FVector(0,305,155),-90,9);
  return true;
 }
 if(Site==TEXT("meridian")){
  B.Shell(8000,7000,410);B.Sign(TEXT("MERIDIAN / CONTINUITY RESIDENCE"),FVector(0,-3514,310),-90,24);
  // A continuous central concourse connects occupied housing, classrooms, care and services.
  for(int Side:{-1,1})for(int Row=0;Row<3;Row++){
   const float Y=-2300+Row*2300;B.WallX(Side*1200,Y,2300,410,180);B.Door(FVector(Side*1200,Y-Side*80,0),Side>0?90:-90);
   if(Row<2)B.WallY(Side*2600,Y+1150,2800,410);
   B.Light(FVector(Side*2600,Y,402));B.Sign(Side<0?(Row==0?TEXT("RESIDENTIAL / A"):Row==1?TEXT("FAMILY LOUNGE"):TEXT("CLINIC")):(Row==0?TEXT("SCHOOL / 1"):Row==1?TEXT("SCHOOL / 2"):TEXT("DINING / STORES")),FVector(Side*1190,Y,285),Side>0?180:0,18);
   if(Side<0&&Row==0){B.Bedroom(FVector(-3000,-2650,0));B.Bedroom(FVector(-3000,-1600,0));B.WallY(-3000,-2170,1900,410,160);}
   else if(Side<0&&Row==1){for(int J=0;J<3;J++){B.Prop(TEXT("Sofa65"),FVector(-3200,-650+J*650,0),90);B.Prop(TEXT("CoffeeTable65"),FVector(-2800,-650+J*650,0));}B.Prop(TEXT("Bookcase65"),FVector(-3800,650,0),90);B.Prop(TEXT("SchoolCubbies77"),FVector(-1900,1000,0));}
   else if(Side<0){for(int J=0;J<4;J++){B.Prop(TEXT("ClinicBedV3"),FVector(-3500+J%2*1000,1700+J/2*1100,0),90);B.Prop(TEXT("PrivacyScreen77"),FVector(-3200+J%2*1000,1800+J/2*1100,0),90);}B.Prop(TEXT("Cabinet65"),FVector(-1550,3250,0));B.Prop(TEXT("Sink65"),FVector(-1850,3250,0));}
   else if(Row<2){
    // Four proportioned classrooms open onto a shared internal passage.
    // Furniture is kept clear of the two metre doorway approaches.
    for(int Wing:{-1,1}){
     B.WallX(2600,Y+Wing*650,1000,410);
     for(int Room=0;Room<2;Room++){
      const float X=1900+Room*1400,DoorY=Y+Wing*150;
      B.WallY(X,DoorY,1400,410,180);B.Door(FVector(X-80,DoorY,0),0);
      for(int R=0;R<3;R++)for(int Col=0;Col<5;Col++){
       const FVector Desk(X-450+Col*220,Y+(Wing>0?400:-850)+R*250,0);
       B.Prop(TEXT("SchoolDesk77"),Desk);B.Prop(TEXT("Chair65"),Desk+FVector(0,-115,0),0,.8);
       if((R+Col)%4==0)B.Prop(TEXT("Dossier52"),Desk+FVector(0,0,79),Col*17,.5);
      }
      const float BoardY=Y+Wing*1138;
      B.Box(TEXT("Wood"),FVector(X,BoardY,208),FVector(840,12,165));B.Box(TEXT("Rubber"),FVector(X,BoardY-Wing*8,208),FVector(816,3,142));B.Sign(TEXT("READ / ASK / CHECK YOUR SOURCES"),FVector(X,BoardY-Wing*11,233),Wing>0?-90:90,11);
      B.Prop(TEXT("Desk65"),FVector(X+465,Y+Wing*1030,0),90,.8);
      B.Prop(TEXT("SchoolCubbies77"),FVector(X-475,Y+Wing*1010,0),0,.85);B.Light(FVector(X,Y+Wing*650,401));
     }
    }
   }else{
    for(int R=0;R<3;R++)for(int Col=0;Col<2;Col++){FVector P(1800+Col*1250,Y-650+R*550,0);B.Prop(TEXT("DiningTable65"),P);B.Prop(TEXT("Chair65"),P+FVector(0,-160,0));B.Prop(TEXT("Chair65"),P+FVector(0,160,0),180);}
    for(int J=0;J<4;J++)B.Prop(TEXT("Cabinet65"),FVector(1700+J*600,3310,0));
   }
  }
  for(int Side:{-1,1})for(int Row=0;Row<5;Row++){
   B.Prop(TEXT("WaitingBench65"),FVector(Side*980,-2600+Row*1250,0),Side>0?90:-90);B.Prop(TEXT("Artwork65"),FVector(Side*1180,-2600+Row*1250,160),Side>0?90:-90);
  }
  B.Prop(TEXT("Counter65"),FVector(-500,-2700,0));B.Prop(TEXT("Planter65"),FVector(780,-3050,0));return true;
 }
 if(Site==TEXT("labor")){
  B.Shell(6000,4800,380);B.Sign(TEXT("TRANSFER WORKS / WARD & WINTER RESERVES"),FVector(0,-2414,300),-90,18);
  for(int Side:{-1,1}){
   B.WallX(Side*900,0,4800,380,180);B.WallY(Side*1950,500,2100,380,160);
   for(int J=0;J<6;J++)B.Prop(Side<0?TEXT("ClinicBedV3"):TEXT("Shelf65"),FVector(Side*(1450+J%2*900),-1800+J/2*730,0),Side>0?90:0);
   for(int J=0;J<4;J++)B.Prop(TEXT("Crate"),FVector(Side*(1500+J%2*700),1000+J/2*600,0));B.Light(FVector(Side*1900,0,372));
  }
  B.Prop(TEXT("ServiceBenchV3"),FVector(0,1750,0));B.Prop(TEXT("ToolShelf65"),FVector(500,2070,0));return true;
 }
 if(Site==TEXT("rome")){
  // Workshop and exterior machinery bay; roof is supported on real columns.
  B.Box(TEXT("Concrete"),FVector(0,0,-18),FVector(6400,5200,36));
  B.WallX(-3100,0,4900,430);B.WallY(-1500,2450,3200,430);B.WallY(-1500,-2450,3200,430,240);B.Box(TEXT("CorrugatedV7"),FVector(-1500,0,450),FVector(3400,5100,30));
  for(int Y:{-2000,0,2000}){B.Box(TEXT("Steel"),FVector(200,Y,215),FVector(32,32,430));B.Light(FVector(-1400,Y,425));}
  for(int I=0;I<5;I++){B.Prop(TEXT("ServiceBenchV3"),FVector(-2820,-1800+I*800,0),90);B.Prop(TEXT("ToolShelf65"),FVector(-2300,-1800+I*800,0),90);}
  for(int Y:{-1200,1200})B.Prop(TEXT("Crate"),FVector(-1400,Y,0));
  B.Box(TEXT("Concrete"),FVector(1900,0,-95),FVector(1600,4800,180));B.Box(TEXT("WindowGlass"),FVector(1900,0,5),FVector(1250,4700,3),false);
  for(int X:{1180,2620})B.Box(TEXT("Concrete"),FVector(X,0,75),FVector(140,4800,150));
  B.Prop(TEXT("Generator"),FVector(450,-1700,0));B.Sign(TEXT("LOCKKEEPERS / CANAL FREIGHT"),FVector(-1500,-2464,340),-90,20);return true;
 }
 // Open ferry landing, with a real ramp connecting the paved quay to the gangway.
 B.Box(TEXT("Concrete"),FVector(-500,0,-15),FVector(3000,3500,30));
 for(int Side:{-1,1}){B.Prop(TEXT("WaitingBench65"),FVector(-900,Side*800,0));B.Prop(TEXT("Crate"),FVector(-1500,Side*1100,0));}
 B.Box(TEXT("Wood"),FVector(2100,0,17),FVector(4200,550,34));
 for(int X=700;X<=4000;X+=550)for(int Side:{-1,1})B.Box(TEXT("Wood"),FVector(X,Side*210,-145),FVector(30,30,340));
 B.Sign(TEXT("NIGHT FERRY / PASSENGERS FIRST"),FVector(700,400,240),180,17);return true;
}
void ULWCampaignProduction77::BuildCanal(){
 auto* Root=GetWorld()->SpawnActor<AActor>();Root->SetRootComponent(NewObject<USceneComponent>(Root));Root->GetRootComponent()->RegisterComponent();Root->SetActorLocation(Director->At(FVector(1900,0,0)));Director->Actors.Add(Root);
 Mechanisms.Add(Add(TEXT("LockGate77"),FVector(-620,-550,75),FRotator(0,-90,0),FVector(1,620.f/520.f,1),true,Root));
 Mechanisms.Add(Add(TEXT("LockGate77"),FVector(620,-550,75),FRotator(0,90,0),FVector(1,620.f/520.f,1),true,Root));
 Mechanisms.Add(Add(TEXT("LockGear77"),FVector(-800,-540,290),FRotator(90,0,0),FVector(1),false,Root));
 Mechanisms.Add(Add(TEXT("Winch77"),FVector(-830,850,100),FRotator(0,0,90),FVector(1),false,Root));
 auto Support=[&](FVector At,FVector Size){if(auto* M=Add(TEXT("Cube"),At,FRotator::ZeroRotator,Size/100,true,Root))M->SetMaterial(0,Director->World->Material(TEXT("Steel")));};
 // Bearing pedestals support the gear above the lock wall; the hoist has two
 // floor-mounted bearings. Neither moving mechanism floats in masonry.
 for(int Side:{-1,1}){
  Support(FVector(-800,-540+Side*140,220),FVector(35,35,140));
  Support(FVector(-830,850+Side*75,60),FVector(40,28,120));
 }
 Support(FVector(-760,-540,290),FVector(110,320,24));
 MechanismPhase=FMath::Clamp(Director->State().Values.FindRef(TEXT("canal_phase77"))/10000.f,0.f,1.f);
}
