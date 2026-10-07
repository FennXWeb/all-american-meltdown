#include "LWSettlement82.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"

UStaticMeshComponent* ALWBuildPiece82::Panel(FVector At,FVector Size,FName Material,int Paint,FRotator Rot){
 auto* M=NewObject<UStaticMeshComponent>(this);M->SetupAttachment(Root);M->SetMobility(EComponentMobility::Movable);M->SetStaticMesh(World->Mesh(TEXT("Cube")));M->SetRelativeLocation(At);M->SetRelativeRotation(Rot);M->SetRelativeScale3D(Size/100.);M->SetCollisionProfileName(TEXT("BlockAll"));
 auto* Base=World->Material(Material);if(Base){if(Cast<UMaterialInstanceDynamic>(Base))M->SetMaterial(0,Base);else{auto* Tint=UMaterialInstanceDynamic::Create(Base,this);Tint->SetVectorParameterValue(TEXT("Tint"),LWBuilding82::Color(Paint));M->SetMaterial(0,Tint);}}M->RegisterComponent();Shell.Add(M);return M;
}
void ALWBuildPiece82::Build(ALWSettlement82* M,const FLWConstruction82& P){
 World=M->World;RecordId=P.Id;CatalogId=P.Catalog;SetActorTransform(P.Transform);Kind=ELWObjectKind::Furniture;Body->SetStaticMesh(nullptr);Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);SetActorTickEnabled(false);
 auto Finish=[](int I){return FName(*FString::Printf(TEXT("Build82_%d"),FMath::Clamp(I,0,6)));};
 if(P.Catalog==TEXT("flag")){
  UseType=TEXT("flag");Panel({0,0,6},{60,60,12},TEXT("Concrete"));Panel({0,0,135},{6,6,270},TEXT("Steel"));Panel({52,0,218},{100,3,66},TEXT("Build82_1"),3);Panel({52,-2,218},{70,1,8},TEXT("Build82_1"),7);Panel({52,-2,218},{8,1,46},TEXT("Build82_1"),7);return;
 }
 const auto* D=LWBuilding82::Find(P.Catalog);if(!D)return;UseType=D->Use;
 if(D->Wall()){
  SetActorTickEnabled(D->Use!=TEXT("wall"));
  auto Wall=[&](FVector At,FVector Size){Panel(At+FVector(0,-Size.Y*.25,0),{Size.X,Size.Y*.5,Size.Z},Finish(P.Outside),P.OutsideColor);Panel(At+FVector(0,Size.Y*.25,0),{Size.X,Size.Y*.5,Size.Z},Finish(P.Inside),P.InsideColor);};
  if(D->Use==TEXT("wall"))Wall({0,0,150},D->Size);
  else if(D->Use==TEXT("door")){
   Wall({-130,0,150},{140,20,300});Wall({130,0,150},{140,20,300});Wall({0,0,266},{120,20,68});
   Panel({-62,0,116},{5,26,232},TEXT("Wood"));Panel({62,0,116},{5,26,232},TEXT("Wood"));Panel({0,0,234},{129,26,6},TEXT("Wood"));
   Kind=ELWObjectKind::Door;Body->SetRelativeLocation({-59,0,0});auto* Leaf=Panel({59,0,115},{118,10,230},Finish(P.Inside),P.InsideColor);Leaf->AttachToComponent(Body,FAttachmentTransformRules::KeepRelativeTransform);
   for(float Z:{56.f,166.f}){auto* Trim=Panel({59,-6,Z},{92,3,80},Finish(P.Outside),P.OutsideColor);Trim->AttachToComponent(Body,FAttachmentTransformRules::KeepRelativeTransform);}
   auto* Handle=Panel({104,-10,112},{16,6,4},TEXT("Steel"));Handle->AttachToComponent(Body,FAttachmentTransformRules::KeepRelativeTransform);bChanged=World->PropStates.FindRef(RecordId)!=0;DoorAngle=bChanged?100:0;Body->SetRelativeRotation(FRotator(0,DoorAngle,0));
  }else{
   Wall({-175,0,150},{50,20,300});Wall({175,0,150},{50,20,300});Wall({0,0,48},{300,20,96});Wall({0,0,276},{300,20,48});
   for(float X:{-150.f,150.f})Panel({X,0,174},{6,28,158},TEXT("Steel"));for(float Z:{95.f,253.f})Panel({0,0,Z},{306,32,6},TEXT("Steel"));Panel({0,0,174},{4,20,152},TEXT("Steel"));
   Kind=ELWObjectKind::Window;Body->SetStaticMesh(World->Mesh(TEXT("Cube")));Body->SetRelativeLocation({0,0,174});Body->SetRelativeScale3D({2.95,.025,1.52});Body->SetMaterial(0,World->Material(TEXT("WindowGlass")));bChanged=World->PropStates.FindRef(RecordId)!=0;Body->SetVisibility(!bChanged);Body->SetCollisionEnabled(bChanged?ECollisionEnabled::NoCollision:ECollisionEnabled::QueryAndPhysics);
  }
  // Insets stop at wall ends so shared corners have no protruding trim.
  for(float Side:{-1.f,1.f}){Panel({0,Side*11,10},{400,3,20},TEXT("Wood"));Panel({0,Side*11,291},{400,3,18},TEXT("Wood"));}return;
 }
 if(D->Deck()){
  const bool Foundation=D->Use==TEXT("foundation");Panel(FVector(0,0,-D->Size.Z*.5-(Foundation?1:0)),D->Size-FVector(0,0,Foundation?2:4),Finish(Foundation?6:P.Outside),P.OutsideColor);
  const bool Roof=D->Use==TEXT("roof");Panel({0,0,-1},{400,400,2},Finish(Roof?P.Outside:P.Inside),Roof?P.OutsideColor:P.InsideColor);if(!Foundation)Panel({0,0,-D->Size.Z+1},{400,400,2},Finish(Roof?P.Inside:P.Outside),Roof?P.InsideColor:P.OutsideColor);return;
 }
 if(D->Use==TEXT("stairs")||D->Use==TEXT("ramp")){
  const int Count=D->Use==TEXT("stairs")?20:10;const float Step=D->Size.Y/Count,Rise=D->Size.Z/Count;
  for(int I=0;I<Count;I++){float H=(I+1)*Rise;Panel({0,-D->Size.Y*.5+(I+.5f)*Step,H*.5f},{D->Size.X,Step+.5f,H},Finish(P.Inside),P.InsideColor);}
  if(D->Use==TEXT("stairs"))for(float X:{-83.f,83.f}){for(int I=0;I<5;I++){float Y=-190+I*95.f,Bottom=FMath::CeilToFloat((Y+200)/Step)*Rise,Top=116+(Y+190)*.8f;Panel({X,Y,(Top+Bottom)*.5f},{5,5,Top-Bottom},TEXT("Steel"));}Panel({X,0,268},{5,FMath::Sqrt(380.f*380+304.f*304),5},TEXT("Steel"),0,FRotator(0,0,-FMath::RadiansToDegrees(FMath::Atan2(304.f,380.f))));}return;
 }
 if(D->Use==TEXT("fence")){
  for(int I=0;I<9;I++)Panel({-190+I*47.5f,0,50},{9,12,100},Finish(P.Outside),P.OutsideColor);
  for(float Z:{28.f,80.f})Panel({0,0,Z},{400,15,9},Finish(P.Inside),P.InsideColor);return;
 }
 if(D->Use==TEXT("parking")){
  Panel({0,0,-16},D->Size,TEXT("Asphalt"));for(int I=0;I<6;I++)for(float Y:{-850.f,850.f})Panel({-2000+I*800.f,Y,1},{7,870,2},TEXT("Build82_1"),7);
  for(float Y:{-1240.f,1240.f})Panel({0,Y,12},{3970,30,24},TEXT("Concrete"));
  // Island terminal sits outside the clear aisle and all ten bay footprints.
  Panel({2020,0,80},{40,70,160},TEXT("Steel"));Panel({1997,0,124},{3,55,38},TEXT("Glow"));return;
 }
 if(D->Use==TEXT("farm")){
  Panel({0,0,13},{240,180,26},TEXT("Wood"));Panel({0,0,28},{225,165,8},TEXT("Earth"));
  for(int I=0;I<8;I++)for(int J=0;J<3;J++){FVector At(-98+I*28,-55+J*55,34);Panel(At+FVector(0,0,14),{3,3,28},TEXT("Build82_1"),3);for(float Side:{-1.f,1.f})Panel(At+FVector(Side*6,0,20),{16,7,2},TEXT("Build82_1"),3,FRotator(Side*30,0,0));}return;
 }
 // Normalize each authored asset's bounds to a catalog footprint, including its floor contact.
 auto* Mesh=World->Mesh(D->Model);if(Mesh){Body->SetStaticMesh(Mesh);const FBox Bounds=Mesh->GetBoundingBox();FVector Scale=D->Size/Bounds.GetSize().ComponentMax(FVector(1));Body->SetRelativeScale3D(Scale);Body->SetRelativeLocation(-FVector(Bounds.GetCenter().X,Bounds.GetCenter().Y,Bounds.Min.Z)*Scale);Body->SetCollisionProfileName(TEXT("BlockAll"));}
 if(D->Use==TEXT("ham")){
  Body->SetRelativeScale3D(Body->GetRelativeScale3D()*FVector(.65,.65,.25));Body->AddLocalOffset({0,0,83});Panel({0,0,78},{130,76,6},TEXT("Wood"));for(float X:{-56.f,56.f})for(float Y:{-27.f,27.f})Panel({X,Y,38},{5,5,76},TEXT("Steel"));Panel({-45,22,180},{2,2,190},TEXT("Steel"));
 }
 if(D->Use==TEXT("locker")||D->Use==TEXT("supplies")){
  const FName Store=D->Use==TEXT("supplies")?FName(*(Claim.ToString()+TEXT("_supplies"))):RecordId;
  auto& R=World->Containers.FindOrAdd(Store);R.Id=Store;R.Context=TEXT("settlement82");R.Position=GetActorLocation();R.Width=12;R.Height=D->Use==TEXT("supplies")?24:12;R.Unlocked=true;
 }
 if(D->Use==TEXT("light")||D->Use==TEXT("ceilinglight")){
  auto* L=NewObject<UPointLightComponent>(this);L->SetupAttachment(Root);L->SetRelativeLocation(FVector(0,0,D->Use==TEXT("ceilinglight")?-25:D->Size.Z));L->SetIntensity(1300);L->SetAttenuationRadius(750);L->SetLightColor(FLinearColor(1,.81,.59));L->SetCastShadows(false);L->RegisterComponent();
 }
}
void ALWBuildPiece82::Use(ALWCharacter* P){if(!P)return;auto* M=ALWSettlement82::Ensure(P);if(!M)return;
 if(UseType==TEXT("flag")||UseType==TEXT("ham")||UseType==TEXT("parking")||(LWBuilding82::Find(CatalogId)&&LWBuilding82::Find(CatalogId)->Job())){M->Open(Claim);M->Panel=UseType==TEXT("parking")?2:UseType==TEXT("flag")?0:1;M->SelectedLot=UseType==TEXT("parking")?RecordId:NAME_None;return;}
 if(UseType==TEXT("supplies")){M->Selected=Claim;M->Action(TEXT("s82_supplies"));return;}
 if(UseType==TEXT("light")||UseType==TEXT("ceilinglight")){if(auto* L=FindComponentByClass<UPointLightComponent>())L->ToggleVisibility();return;}
 Super::Use(P);
}
FString ALWBuildPiece82::Prompt()const{
 if(UseType==TEXT("flag"))return TEXT("[E] SETTLEMENT");if(UseType==TEXT("ham"))return TEXT("[E] RECRUITMENT RADIO");if(UseType==TEXT("parking"))return TEXT("[E] MANAGE PARKING");if(UseType==TEXT("supplies"))return TEXT("[E] SHARED SUPPLIES");
 const auto* D=LWBuilding82::Find(CatalogId);if(D&&D->Job())return TEXT("[E] ASSIGN RESIDENT");if(D&&(D->Structural()&&Kind!=ELWObjectKind::Door&&Kind!=ELWObjectKind::Window||UseType==TEXT("decor")||UseType==TEXT("walldecor")))return FString();return Super::Prompt();
}
