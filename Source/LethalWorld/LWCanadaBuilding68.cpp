#include "LWStreaming68.h"
#include "LWCampaign76.h"
#include "LWWorld.h"
#include "LWResident.h"
#include "LWSiteIdentity.h"
#include "LWWorldTextComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"

void ALWChunk::BuildingCanada68(ALWWorld* W,const LWGen::FSite& S){
 if(LWCampaign76::Dress(this,W,S))return;
 const FVector O=FVector(S.Position,24)-GetActorLocation();const float X=S.Size.X*.5,Y=S.Size.Y*.5,H=380;
 auto B=[&](FName M,FVector P,FVector Size){Box(W,M,O+P,Size);};
 auto M=[&](FName Mesh,FVector P,float Yaw=0){Add(W,Mesh,NAME_None,O+P,FVector(1),FRotator(0,Yaw,0));};
 auto Text=[&](FString Label,FVector P,float Size){auto* T=NewObject<ULWWorldTextComponent>(this);T->SetupAttachment(RootComponent);T->SetRelativeLocation(O+P);T->SetRelativeRotation(FRotator(0,-90,0));T->SetText(FText::FromString(Label));T->SetWorldSize(Size);T->SetHorizontalAlignment(EHTA_Center);T->SetTextRenderColor(FColor(224,242,229));T->SetCollisionEnabled(ECollisionEnabled::NoCollision);T->SetCullDistance(14000);T->RegisterComponent();AddInstanceComponent(T);};
 B(TEXT("RV66_Quartz"),FVector(0,0,-12),FVector(2*X,2*Y,24));
 // Continuous envelope, with a real doorway and separate storefront panes.
 for(int Side:{-1,1})B(TEXT("RV66_Ivory"),FVector(Side*X,0,H*.5),FVector(30,2*Y,H));
 B(TEXT("RV66_Ivory"),FVector(0,Y,H*.5),FVector(2*X,30,H));
 for(int Side:{-1,1}){
  B(TEXT("RV66_Walnut"),FVector(Side*(X*.5+65),-Y,75),FVector(X-130,26,150));
  B(TEXT("Glass"),FVector(Side*(X*.5+65),-Y,230),FVector(X-130,10,155));
  for(int J=1;J<6;J++)B(TEXT("Steel"),FVector(Side*(150+J*(X-150)/6),-Y,235),FVector(12,26,175));
 }
 B(TEXT("RV66_Walnut"),FVector(0,-Y,350),FVector(2*X,36,60));
 B(TEXT("RV66_Ivory"),FVector(0,0,H),FVector(2*X+40,2*Y+40,26));
 for(int Side:{-1,1})B(TEXT("Steel"),FVector(Side*140,-Y,155),FVector(20,40,310));
 B(TEXT("Steel"),FVector(0,-Y,310),FVector(300,40,20));
 auto* Door=W->SpawnObject(ELWObjectKind::Door,FName(*FString::Printf(TEXT("ca_door_%u"),S.Id)),O+GetActorLocation()+FVector(-128,-Y,0),FRotator::ZeroRotator);if(Door)Residents.Add(Door);
 Text(LWSites::Label(S),FVector(0,-Y-23,360),65);
 // Weather canopies, illuminated fascia, separate upper apartments and rooftop equipment.
 B(TEXT("CanadaRed51"),FVector(0,-Y-190,330),FVector(2*X+60,390,18));
 for(int Floor=1;Floor<S.Floors;Floor++){
  float Z=Floor*H;B(TEXT("Concrete"),FVector(0,0,Z+H*.5),FVector(2*X-30,2*Y-30,H));
  for(int Side:{-1,1})for(int Col=-4;Col<=4;Col++){
   B(TEXT("Steel"),FVector(Col*400,Side*(Y+2),Z+205),FVector(250,10,245));
   B(TEXT("Glass"),FVector(Col*400,Side*(Y+9),Z+205),FVector(230,5,225));
   B(TEXT("RV66_Ivory"),FVector(Col*400,Side*(Y+15),Z+75),FVector(280,45,20));
  }
 }
 B(TEXT("Steel"),FVector(-600,600,S.Floors*H+100),FVector(500,420,200));
 // Stock room and staff washroom occupy the rear instead of leaving a cavernous blank floor.
 B(TEXT("RV66_Walnut"),FVector(-X*.65,Y-700,180),FVector(X*.7,22,360));
 B(TEXT("RV66_Walnut"),FVector(X*.65,Y-700,180),FVector(X*.7,22,360));
 B(TEXT("RV66_Walnut"),FVector(X-700,Y-350,180),FVector(22,700,360));
 M(TEXT("SinkV4"),FVector(X-1100,Y-220,0));M(TEXT("Toilet65"),FVector(X-360,Y-250,0));
 for(int I=-3;I<2;I++)M(TEXT("Shelf65"),FVector(I*450,Y-190,0),180);
 // Counter, checkouts, aisle islands, seating, plants, wall art and ceiling fixtures.
 M(TEXT("Counter65"),FVector(X-550,-Y+720,0),90);
 M(TEXT("CashRegister53"),FVector(X-560,-Y+720,100),90);
 for(int Col=-2;Col<=2;Col++)for(int Row=-1;Row<=1;Row++){
  const FVector P(Col*600,Row*720+120,0);
  if(S.Type==65){if(Row==1){M(TEXT("Counter65"),P);M(TEXT("CashRegister53"),P+FVector(0,0,100));}else if(Col%2==0)M(TEXT("WaitingBench65"),P);}
  else if(S.Type==4){M(TEXT("DinerTableV9"),P);M(TEXT("ChairV3"),P+FVector(0,-140,0),90);M(TEXT("ChairV3"),P+FVector(0,140,0),-90);}
  else if(S.Type==18){M(Row%2?TEXT("ClothesRail65"):TEXT("Shelf65"),P,Row*90);}
  else if(S.Type==2){M(TEXT("MotelBedV3"),P);M(TEXT("LockerV4"),P+FVector(220,100,0));}
  else{M(TEXT("SofaV13"),P);M(TEXT("CoffeeTableV13"),P+FVector(0,-160,0));}
 }
 for(int Side:{-1,1})for(int J=-2;J<=2;J++){
  B(TEXT("RV66_Brass"),FVector(Side*(X-25),J*610,225),FVector(14,190,150));
  B(TEXT("CanadaGrass51"),FVector(Side*(X-35),J*610,225),FVector(8,172,132));
 }
 for(int A=-1;A<=1;A++)for(int Bn=-1;Bn<=1;Bn++){
  B(TEXT("RV66_Light"),FVector(A*1200,Bn*1200,H-22),FVector(210,80,10));
  if(A==0){auto* L=NewObject<UPointLightComponent>(this);L->SetupAttachment(RootComponent);L->SetRelativeLocation(O+FVector(0,Bn*1200,H-50));L->SetIntensity(1000);L->SetLightColor(FLinearColor(1,.87,.68));L->SetAttenuationRadius(1600);L->SetCastShadows(false);L->RegisterComponent();AddInstanceComponent(L);}
 }
 for(int Side:{-1,1}){M(TEXT("WaitingBench65"),FVector(Side*1200,-Y-600,0));StreetLight(W,O+FVector(Side*(X-180),-Y-400,0),FRotator::ZeroRotator);Add(W,TEXT("LivingPine51"),NAME_None,O+FVector(Side*(X-80),-Y-900,0),FVector(.6),FRotator::ZeroRotator,false);}
 static const TCHAR* First[]={TEXT("Amelie"),TEXT("Noah"),TEXT("Olivia"),TEXT("Ethan"),TEXT("Zoe"),TEXT("Liam"),TEXT("Chloe"),TEXT("Lucas"),TEXT("Maya"),TEXT("Owen"),TEXT("Aisha"),TEXT("Felix")};
 static const TCHAR* Last[]={TEXT("Tremblay"),TEXT("Chen"),TEXT("Patel"),TEXT("Roy"),TEXT("Martin"),TEXT("Singh"),TEXT("Gagnon"),TEXT("Wilson"),TEXT("Dubois"),TEXT("Wong"),TEXT("Ahmed"),TEXT("Fraser")};
 for(int I=0;I<5;I++){auto Spawn=[this,W,S,O,X,Y,I](){
  FVector P=I==0?FVector(X-800,-Y+950,100):FVector(-1400+I*580,-Y-700,100);
  FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
  auto* N=W->GetWorld()->SpawnActor<ALWResident>(GetActorLocation()+O+P,FRotator(0,-90,0),Params);if(!N)return;
  N->SetActorTickInterval(.15f);N->ConfigureResident(FName(*FString::Printf(TEXT("ca_%u_%d"),S.Id,I)),I==0?(S.Type==65?TEXT("exchange70"):TEXT("canada_shop68")):TEXT("civilian"),FString::Printf(TEXT("%s %s"),First[(S.Id+I)%12],Last[(S.Id/12+I)%12]),I%6);
  if(I==0)N->ActivitySpots.Add(N->GetActorLocation());else for(int J=0;J<4;J++)N->ActivitySpots.Add(GetActorLocation()+O+FVector(-1800+J*1100,-Y-700,100));
  Residents.Add(N);
 };if(Plan68)Plan68->Population.Add(MoveTemp(Spawn));else Spawn();
 }
}
