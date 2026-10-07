#include "LWCampaign76.h"
#include "LWCampaignProduction77.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWWorldTextComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"

bool LWCampaign76::Dress(ALWChunk* C,ALWWorld* W,const LWGen::FSite& S){
 auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(W,0));
 if(!P||!P->RPG.Campaign76.Started)return false;
 if(LWProduction77::Dress(C,W,S))return true;
 if(S.Id==0xEC76FF00u){
  const FVector Base=FVector(S.Position,36)-C->GetActorLocation();auto Box=[&](FName M,FVector V,FVector Size){C->Box(W,M,Base+V,Size);};
  Box(TEXT("Concrete"),FVector(0,0,-12),FVector(2200,1800,24));Box(TEXT("TileV7"),FVector(0,0,1),FVector(2160,1760,2));
  for(int X:{-1100,1100})Box(TEXT("PlasterV7"),FVector(X,0,175),FVector(20,1800,350));Box(TEXT("PlasterV7"),FVector(0,900,175),FVector(2200,20,350));
  for(int X:{-590,590})Box(TEXT("PlasterV7"),FVector(X,-900,175),FVector(1020,20,350));Box(TEXT("PlasterV7"),FVector(0,-900,304),FVector(160,20,92));Box(TEXT("Concrete"),FVector(0,0,365),FVector(2240,1840,30));
  if(auto* Door=W->SpawnObject(ELWObjectKind::Door,TEXT("c76_northbank_door"),FVector(S.Position,36)+FVector(-80,-900,0),FRotator::ZeroRotator))C->Residents.Add(Door);
  for(int X:{-500,500})Box(TEXT("PlasterV7"),FVector(X,650,145),FVector(14,480,290));
  return true;
 }
 if(S.Id==SiteId(TEXT("transport"))){
  const FVector Base=FVector(S.Position,36)-C->GetActorLocation();auto Box=[&](FName M,FVector V,FVector Size){C->Box(W,M,Base+V,Size);};
  Box(TEXT("Concrete"),FVector(0,0,-15),FVector(4800,4200,30));
  Box(TEXT("BrickV7"),FVector(-2400,0,350),FVector(30,4200,700));Box(TEXT("BrickV7"),FVector(2400,0,350),FVector(30,4200,700));Box(TEXT("BrickV7"),FVector(0,2100,350),FVector(4800,30,700));
  for(int Side:{-1,1})Box(TEXT("BrickV7"),FVector(Side*1500,-2100,350),FVector(1800,30,700));
  Box(TEXT("CorrugatedV7"),FVector(0,0,715),FVector(4900,4300,30));
  for(int Y=-1500;Y<=1500;Y+=750)Box(TEXT("Steel"),FVector(0,Y,655),FVector(4780,25,70));
  for(FVector V:{FVector(-800,-700,0),FVector(800,-700,0),FVector(-1400,700,0),FVector(1400,1000,0)})C->Add(W,TEXT("Crate"),NAME_None,Base+V,FVector(2));
  return true;
 }
 if(S.Id!=SiteId(TEXT("bellwether")))return false;
 const auto F=Frame(TEXT("bellwether"));auto At=[&](FVector V){return F.TransformPosition(V)-C->GetActorLocation();};
 auto Box=[&](FName M,FVector V,FVector Size){C->Box(W,M,At(V),Size,F.Rotator());};
 // One service-plaza conversion, inside its existing parcel. Other buildings keep their authored geometry.
 Box(TEXT("Concrete"),FVector(0,0,-12),FVector(1800,1200,24));Box(TEXT("TileV7"),FVector(0,0,-2),FVector(1760,1160,4));
 for(int Side:{-1,1}){Box(TEXT("BrickV7"),FVector(Side*900,0,165),FVector(24,1200,330));Box(TEXT("BrickV7"),FVector(Side*490,-600,165),FVector(820,24,330));}
 Box(TEXT("BrickV7"),FVector(0,600,165),FVector(1800,24,330));Box(TEXT("BrickV7"),FVector(0,-600,294),FVector(160,24,72));
 Box(TEXT("CorrugatedV7"),FVector(0,0,348),FVector(1900,1300,32));Box(TEXT("Concrete"),FVector(0,-740,-22),FVector(220,300,24));
 const FName DoorId=TEXT("c76_bell_door");if(auto* Door=W->SpawnObject(ELWObjectKind::Door,DoorId,F.TransformPosition(FVector(-80,-600,0)),F.Rotator()))C->Residents.Add(Door);
 auto Mesh=[&](FName M,FVector V,float Yaw=0){C->Add(W,M,NAME_None,At(V),FVector(1),F.Rotator()+FRotator(0,Yaw,0));};
 Mesh(TEXT("Radiator65"),FVector(-730,480,0));Mesh(TEXT("ClinicBedV3"),FVector(620,250,0),90);Mesh(TEXT("ClinicBedV3"),FVector(620,-120,0),90);
 Mesh(TEXT("Shelf"),FVector(-730,100,0));Mesh(TEXT("DiningTable65"),FVector(-600,-300,0));Mesh(TEXT("Chair65"),FVector(-740,-300,0),0);Mesh(TEXT("Chair65"),FVector(-460,-300,0),180);
 Mesh(TEXT("Generator"),FVector(1150,400,0));Mesh(TEXT("Crate"),FVector(-690,420,0));Mesh(TEXT("Bookcase65"),FVector(720,500,0));
 auto* Sign=NewObject<ULWWorldTextComponent>(C);Sign->SetupAttachment(C->GetRootComponent());Sign->SetRelativeLocation(At(FVector(0,-616,295)));Sign->SetRelativeRotation(F.Rotator()+FRotator(0,-90,0));Sign->SetText(FText::FromString(TEXT("BELLWETHER / ALL ARRIVALS AT THE FRONT DESK")));Sign->SetHorizontalAlignment(EHTA_Center);Sign->SetWorldSize(15);Sign->RegisterComponent();
 return true;
}
void ALWCampaign76::StageDressing(){
 const auto* S=LWCampaign76::Stage(State().Stage);if(!S)return;
 auto Mesh=[&](FName Key,FVector V,FRotator R=FRotator::ZeroRotator,FVector Scale=FVector(1),bool Solid=true){
  auto* Asset=World->Mesh(Key);if(!Asset)return static_cast<UStaticMeshComponent*>(nullptr);auto* M=NewObject<UStaticMeshComponent>(this);M->SetupAttachment(GetRootComponent());M->SetStaticMesh(Asset);V.Z-=Asset->GetBoundingBox().Min.Z*Scale.Z;M->SetWorldLocation(At(V));M->SetWorldRotation(LWCampaign76::Frame(S->Site).Rotator()+R);M->SetWorldScale3D(Scale);M->SetCollisionProfileName(Solid?TEXT("BlockAll"):TEXT("NoCollision"));M->SetCanEverAffectNavigation(false);M->RegisterComponent();Dressing.Add(M);return M;
 };
 if(S->Site==TEXT("bellwether")){
  Heater=NewObject<UPointLightComponent>(this);Heater->SetupAttachment(GetRootComponent());Heater->SetWorldLocation(At(FVector(-700,430,120)));Heater->SetLightColor(FLinearColor(1,.57,.25));Heater->SetIntensity(200);Heater->SetAttenuationRadius(1300);Heater->SetCastShadows(false);Heater->RegisterComponent();
  if(State().Values.FindRef(TEXT("gate_braced")))Mesh(TEXT("Crate"),FVector(-720,-430,0));
  if(State().Values.FindRef(TEXT("bell_condition"))==2||State().Values.FindRef(TEXT("intake_burned")))Mesh(TEXT("Scrap52"),FVector(430,-280,0));
 }else if(S->Site==TEXT("market")){
  Mesh(TEXT("ClinicBedV3"),FVector(650,180,0),FRotator(0,90,0));Mesh(TEXT("ClinicBedV3"),FVector(650,-200,0),FRotator(0,90,0));
  Mesh(TEXT("Chair65"),FVector(-650,-200,0));Mesh(TEXT("Cabinet65"),FVector(630,480,0));Mesh(TEXT("ServiceBenchV3"),FVector(-630,430,0));
  if(!State().Values.FindRef(TEXT("annex_drained"))){auto* Water=Mesh(TEXT("Cube"),FVector(-280,190,1),FRotator::ZeroRotator,FVector(3.7,4,.012),false);if(Water)Water->SetMaterial(0,World->Material(TEXT("WindowGlass")));}
 }else if(S->Site==TEXT("transport")){BuildCarrier();}
 else if(S->Site==TEXT("canada")){
  // The fully dressed reception, rest bays and tracing rooms are built with the parcel.

 }else{
  Mesh(TEXT("Chair65"),FVector(-570,450,0));Mesh(TEXT("Chair65"),FVector(590,450,0));
  if(S->Site==TEXT("rome")){Mesh(TEXT("Generator"),FVector(-1000,400,0));Mesh(TEXT("Crate"),FVector(850,550,0));Mesh(TEXT("ServiceBenchV3"),FVector(-900,0,0));}
  if(S->Site==TEXT("guard")||S->Site==TEXT("watertown")||S->Site==TEXT("crossing")){
   Mesh(TEXT("ClinicBedV3"),FVector(-850,230,0));Mesh(TEXT("ClinicBedV3"),FVector(-850,550,0));
   // Use the original, complete static van, with matching wheel meshes; no new vehicle system.
   Mesh(TEXT("Vehicle42_van"),FVector(950,300,0));Mesh(TEXT("Cabin42_van"),FVector(950,300,0));
   for(int X:{-150,150})for(int Y:{-90,90})Mesh(TEXT("Wheel42_van"),FVector(950+X,300+Y,0));
  }
  if(S->Site==TEXT("meridian")){Mesh(TEXT("Desk65"),FVector(0,590,0));Mesh(TEXT("Bookcase65"),FVector(780,550,0));Mesh(TEXT("Radiator65"),FVector(-780,550,0));}
 }
 // Small papers and keys rest on a measured tabletop instead of floating at a guessed height.
 for(const auto& A:S->Actions)if(A.Mesh==TEXT("Dossier52")||A.Mesh==TEXT("Keys52")||(A.Mesh.IsNone()&&!CastAvailable(FName(*A.Id.ToString().Mid(5)))))Mesh(TEXT("CoffeeTable65"),A.At);
 if(!Heater){Heater=NewObject<UPointLightComponent>(this);Heater->SetupAttachment(GetRootComponent());Heater->SetWorldLocation(At(FVector(0,150,S->Site==TEXT("transport")?620:310)));Heater->SetIntensity(S->Site==TEXT("canada")?4200:2600);Heater->SetAttenuationRadius(S->Site==TEXT("transport")?4000:1500);Heater->SetLightColor(S->Site==TEXT("canada")?FLinearColor(.95,1,.96):FLinearColor(1,.83,.62));Heater->SetCastShadows(false);Heater->RegisterComponent();}
 for(int Side:{-1,1}){
  const bool Hall=S->Site==TEXT("transport");const bool Indoors=Hall||S->Site==TEXT("bellwether")||S->Site==TEXT("canada")||S->Site==TEXT("market");const FVector V(Side*(Hall?1200:450),Hall?-600:-240,Hall?560:285);
  const float Ceiling=Hall?680:S->Site==TEXT("market")?558:S->Site==TEXT("canada")?342:328;
  Mesh(TEXT("CeilingLightV13"),FVector(V.X,V.Y,Indoors?Ceiling:V.Z+8),FRotator::ZeroRotator,FVector(1),false);
  if(!Indoors){auto* Pole=Mesh(TEXT("Cube"),FVector(V.X,V.Y,0),FRotator::ZeroRotator,FVector(.06,.06,V.Z/100),false);if(Pole)Pole->SetMaterial(0,World->Material(TEXT("Steel")));}
  auto* L=NewObject<UPointLightComponent>(this);L->SetupAttachment(GetRootComponent());L->SetWorldLocation(At(V));L->SetIntensity(Hall?60000:24000);L->SetAttenuationRadius(Hall?3500:1700);L->SetLightColor(S->Site==TEXT("canada")?FLinearColor(.96f,1.f,.98f):FLinearColor(1.f,.88f,.74f));L->SetCastShadows(false);L->RegisterComponent();SceneLights.Add(L);
 }
}
