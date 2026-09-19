#include "LWWorld.h"
#include "LWSurface26.h"
#include "ProceduralMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"

void ALWChunk::BuildRoads33(ALWWorld* W,const TArray<LWGen::FRoad>& Roads,const TArray<LWGen::FSite>& Sites){
 using namespace LWSurface26;const FVector2D Origin(GetActorLocation());LightingWorld=W;
 const Polygon Tile{{0,0},{LWGen::ChunkSize,0},{LWGen::ChunkSize,LWGen::ChunkSize},{0,LWGen::ChunkSize}};
 auto Patch=[&](Polygon P,FName Mat,float Z,bool Hit){for(auto& V:P)V-=Origin;for(int I=0;I<4;I++)P=Clip(P,Tile[I],Tile[(I+1)%4],true);if(P.Num()<3||Area(P)<.1)return;
  auto& Old=SlabFootprints.FindOrAdd(FMath::RoundToInt(Z*10));TArray<Polygon> Pieces{P};FBox2D Bounds(P);for(const auto& Prior:Old){if(!Bounds.Intersect(FBox2D(Prior)))continue;TArray<Polygon> Next;for(auto& Piece:Pieces)Next.Append(Subtract(Piece,Prior));Pieces=MoveTemp(Next);if(Pieces.IsEmpty())return;}Old.Add(P);
  FName Key(*(Mat.ToString()+(Hit?TEXT("_C"):TEXT("_N"))));auto& B=SurfaceData.FindOrAdd(Key);for(const auto& Piece:Pieces){int Base=B.Vertices.Num();for(auto V:Piece){B.Vertices.Add(FVector(V,Z));B.Normals.Add(FVector::UpVector);B.UV.Add((V+Origin)/400);}for(int I=1;I<Piece.Num()-1;I++)if(FMath::Abs(Side(Piece[0],Piece[I],Piece[I+1]))>.01)B.Triangles.Append({Base,Base+I+1,Base+I});}
  if(!SurfaceMeshes.Contains(Key)){auto* M=NewObject<UProceduralMeshComponent>(this);M->SetupAttachment(RootComponent);M->bUseAsyncCooking=false;M->SetCollisionProfileName(Hit?TEXT("BlockAll"):TEXT("NoCollision"));M->SetCanEverAffectNavigation(false);M->SetMaterial(0,W->Material(Mat));M->ComponentTags.Add(TEXT("RoadSurface"));M->RegisterComponent();SurfaceMeshes.Add(Key,M);}
 };
 auto Capsule=[](const LWGen::FRoad& R,float Pad){Polygon P;FVector2D D=(R.B-R.A).GetSafeNormal();float Radius=R.Width*.5f+Pad;for(int I=0;I<=8;I++)P.Add(R.A+D.GetRotated(90+I*22.5)*Radius);for(int I=0;I<=8;I++)P.Add(R.B+D.GetRotated(-90+I*22.5)*Radius);return P;};
 TArray<LWGen::FRoad> Near;for(auto& R:Roads){auto P=Capsule(R,180);FBox2D Bounds(P);if(Bounds.Intersect(FBox2D(Origin,Origin+FVector2D(LWGen::ChunkSize)))){Near.Add(R);Patch(Capsule(R,90),TEXT("Concrete"),.5,true);Patch(Capsule(R,0),TEXT("Asphalt"),10,true);}}
 TArray<LWGen::FRoad> TrafficRoads;for(const auto& R:Roads)if(LWGen::DistanceToSegment(Origin+FVector2D(LWGen::ChunkSize*.5),R)<LWGen::ChunkSize*.72+5000)TrafficRoads.Add(R);
 auto Junctions=LWRoads33::Junctions(TrafficRoads,W->Seed);for(auto& J:Junctions)if(LWGen::ChunkAt(J.Position)==Coordinate)RoadJunctions.Add(J);
 auto Clear=[&](FVector2D P,float Radius){if(LWRoads33::InJunction(P,Junctions,Radius))return false;for(auto& R:Roads)if(LWGen::DistanceToSegment(P,R)<R.Width*.5+Radius)return false;for(auto& S:Sites){auto V=(P-S.Position).GetRotated(-S.Yaw);if(FMath::Abs(V.X)<S.Size.X*.5+Radius&&FMath::Abs(V.Y)<S.Size.Y*.5+Radius)return false;}return true;};
 auto Paint=[&](FVector2D P,FVector2D D,FVector2D Size,FName Mat,float Z=11,bool Hit=false){FVector2D N(-D.Y,D.X);Polygon Q{P-D*Size.X*.5-N*Size.Y*.5,P+D*Size.X*.5-N*Size.Y*.5,P+D*Size.X*.5+N*Size.Y*.5,P-D*Size.X*.5+N*Size.Y*.5};Patch(Q,Mat,Z,Hit);};
 auto Label=[&](FString S,FVector At,FVector2D Facing,float Size){auto* T=NewObject<UTextRenderComponent>(this);T->SetupAttachment(RootComponent);T->SetRelativeLocation(At-FVector(Origin,0));T->SetRelativeRotation(FRotator(0,FMath::RadiansToDegrees(FMath::Atan2(Facing.Y,Facing.X)),0));T->SetHorizontalAlignment(EHTA_Center);T->SetVerticalAlignment(EVRTA_TextCenter);T->SetWorldSize(Size);T->SetText(FText::FromString(S));T->SetTextRenderColor(FColor(238,233,210));T->SetCullDistance(7000);T->SetCollisionEnabled(ECollisionEnabled::NoCollision);T->RegisterComponent();};
 auto Sign=[&](FVector2D P,FVector2D Face,FString Message,FName Mat,bool Stop=false){FRotator Rot(0,FMath::RadiansToDegrees(FMath::Atan2(Face.Y,Face.X)),0);Box(W,TEXT("Steel"),FVector(P-Origin,140),FVector(7,7,280));
  // Real octagonal stop face, rectangular backed information/warning plates.
  if(Stop){auto* M=NewObject<UProceduralMeshComponent>(this);M->SetupAttachment(RootComponent);TArray<FVector> V;TArray<int32> Tri;for(int I=0;I<8;I++){float A=(22.5+I*45)*PI/180;V.Add(FVector(P-Origin,285)+Rot.RotateVector(FVector(0,FMath::Cos(A)*46,FMath::Sin(A)*46)));}for(int I=1;I<7;I++)Tri.Append({0,I,I+1});TArray<FVector> Normals;Normals.Init(FVector(Face,0),V.Num());M->CreateMeshSection(0,V,Tri,Normals,TArray<FVector2D>(),TArray<FColor>(),TArray<FProcMeshTangent>(),false);M->SetMaterial(0,W->Material(Mat));M->RegisterComponent();}
  else{Box(W,TEXT("Steel"),FVector(P-Origin,290),FVector(8,154,118),Rot);Box(W,Mat,FVector(P-Origin+Face*5,290),FVector(3,146,110),Rot,false);}
  Label(Message,FVector(P+Face*7,Stop?285:290),Face,Stop?23:18);
 };
 for(const auto& R:Near){const FVector2D D=(R.B-R.A).GetSafeNormal(),N(-D.Y,D.X);const double Length=FVector2D::Distance(R.A,R.B);const FRotator Rot(0,FMath::RadiansToDegrees(FMath::Atan2(D.Y,D.X)),0);const int Lanes=LWRoads33::Lanes(R);const bool UrbanRoad=!R.Highway&&Sites.ContainsByPredicate([&](const auto& S){return S.District>0&&LWGen::DistanceToSegment(S.Position,R)<4800;});
  if(Lanes>=2){
   for(double Start=0;Start<Length;Start+=240){double Mark=FMath::Min(240.,Length-Start);FVector2D P=R.A+D*(Start+Mark*.5);if(LWRoads33::InJunction(P,Junctions,150))continue;for(int Side:{-1,1})Paint(P+N*(R.Width*.5-38)*Side,D,FVector2D(Mark,9),TEXT("Bone"));
    for(int Side:{-1,1})Paint(P+N*Side*12,D,FVector2D(Mark,9),TEXT("Lane"));
    if(UrbanRoad)for(int Side:{-1,1}){FVector2D Walk=P+N*(R.Width*.5+100)*Side;if(Clear(Walk,90))Paint(Walk,D,FVector2D(Mark,180),TEXT("Concrete"),18,true);}
    if(Lanes>2){float LaneWidth=(R.Width-180)/Lanes;for(int Lane=1;Lane<Lanes/2;Lane++)for(int Side:{-1,1})Paint(P+N*Lane*LaneWidth*Side,D,FVector2D(Mark*.75,10),TEXT("Bone"));}
   }
  }
  // Props belong to the chunk containing their anchor, even if the road crosses a boundary.
  uint32 Id=LWGen::Hash(FMath::RoundToInt(R.A.X),FMath::RoundToInt(R.A.Y),W->Seed,33020)^LWGen::Hash(FMath::RoundToInt(R.B.X),FMath::RoundToInt(R.B.Y),W->Seed,33021);
  FVector2D Pool=(R.A+R.B)*.5+N*(R.Width*.22);if(Lanes>=2&&Id%100<42&&!LWRoads33::InJunction(Pool,Junctions,500))Paint(Pool,D,FVector2D(FMath::Min(700.,Length*.5),180+Id%140),TEXT("PuddleV9"),10.7f,false);
  for(double T=Length*.5;T<Length;T+=2200){FVector2D Center=R.A+D*T;FVector2D P=Center+N*(R.Width*.5+260);if(LWGen::ChunkAt(P)!=Coordinate||!Clear(P,110))continue;
   bool Urban=Sites.ContainsByPredicate([&](const auto& S){return S.District>0&&FVector2D::Distance(S.Position,Center)<4800;});
   if(Urban){StreetLight(W,FVector(P-Origin,10),Rot);FVector2D Drain=Center+N*(R.Width*.5-65);if(!LWRoads33::InJunction(Drain,Junctions,160)){Paint(Drain,D,FVector2D(100,70),TEXT("RoadBlack33"));for(int Slot=-2;Slot<=2;Slot++)Box(W,TEXT("Steel"),FVector(Drain-Origin+D*Slot*18,12),FVector(6,68,2),Rot,false);}}
   else if(Id%5==0&&Lanes>=2)Sign(P,-D,R.Highway?TEXT("SPEED LIMIT\n65"):TEXT("SPEED LIMIT\n35"),TEXT("RoadGreen33"));
   if(R.Highway){for(int Side:{-1,1}){FVector2D Rail=Center+N*(R.Width*.5+150)*Side;if(LWGen::ChunkAt(Rail)!=Coordinate||!Clear(Rail,60)||!Clear(Rail+D*650,60)||!Clear(Rail-D*650,60))continue;Box(W,TEXT("Steel"),FVector(Rail-Origin,72),FVector(FMath::Min(Length*.65,1300.),14,28),Rot);for(int Post:{-1,0,1})Box(W,TEXT("Steel"),FVector(Rail-Origin+D*Post*300,40),FVector(12,14,80),Rot);Box(W,TEXT("Lane"),FVector(Rail-Origin,95),FVector(22,24,8),Rot,false);}}
   else if(Id%7==0&&Lanes>=2)Add(W,TEXT("Pole"),NAME_None,FVector(P-Origin,0),FVector(1),Rot);
   if(Id%13==0&&Lanes>=2){FVector2D Park=Center-N*(R.Width*.5+420);if(Clear(Park,230)){auto* Car=W->SpawnObject(ELWObjectKind::Car,FName(*FString::Printf(TEXT("road33_car_%u_%d"),Id,FMath::RoundToInt(T))),FVector(Park,10),Rot);if(Car)Residents.Add(Car);}}
   if(Id%9==0&&Id%5!=0&&!Urban&&Lanes>=2)Sign(P,-D,TEXT("CURVES\nAHEAD"),TEXT("RoadOchre33"));
  }
 }
 for(int Index=0;Index<RoadJunctions.Num();Index++){const auto& J=RoadJunctions[Index];
  for(const auto& A:J.Arms){FVector2D Out=A.Out,Right(Out.Y,-Out.X);FRotator Rot(0,FMath::RadiansToDegrees(FMath::Atan2(Out.Y,Out.X)),0);const FVector2D Stop=J.Position+Out*(J.Radius+80),Pole=Stop+Right*(A.Width*.5+100);
   Paint(Stop+Right*A.Width*.25,Out,FVector2D(28,A.Width*.45),TEXT("Bone"));
   // Crosswalk bars leave a center refuge on wider arterials.
   if(A.Width>=720)for(float Y=-A.Width*.5+70;Y<A.Width*.5-50;Y+=100)Paint(J.Position+Out*(J.Radius-70)+Right*Y,Out,FVector2D(180,42),TEXT("Bone"));
   if(!J.Signals){Sign(Pole,Out,TEXT("STOP"),TEXT("RoadRed33"),true);continue;}
   Box(W,TEXT("Steel"),FVector(Pole-Origin,265),FVector(15,15,530));Box(W,TEXT("Steel"),FVector(Stop-Origin+Right*(A.Width*.25),520),FVector(16,A.Width*.5+210,16),Rot);
   FVector2D Head=Stop+Right*(A.Width*.25);Box(W,TEXT("RoadBlack33"),FVector(Head-Origin,447),FVector(30,70,202),Rot);
   FSignalLamp Lamp;Lamp.Junction=Index;Lamp.Approach=Out;
   for(int L=0;L<3;L++){auto* M=NewObject<UStaticMeshComponent>(this);M->SetupAttachment(RootComponent);M->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere")));M->SetRelativeLocation(FVector(Head-Origin+Out*20,503-L*56));M->SetRelativeRotation(Rot);M->SetRelativeScale3D(FVector(.12,.43,.43));M->SetCollisionEnabled(ECollisionEnabled::NoCollision);M->SetCastShadow(false);M->SetMaterial(0,W->Material(TEXT("TrafficOff33")));M->RegisterComponent();Lamp.Lens[L]=M;Box(W,TEXT("RoadBlack33"),FVector(Head-Origin+Out*32,526-L*56),FVector(44,57,8),Rot,false);}
   SignalLamps.Add(Lamp);
   Sign(Pole+Right*130,Out,FString::Printf(TEXT("ROUTE %02u\nJUNCTION"),J.Id%90+10),TEXT("RoadGreen33"));
  }
 }
 TickTraffic33();
}
void ALWChunk::TickTraffic33(){if(!LightingWorld)return;const FName Mats[]={TEXT("TrafficRed33"),TEXT("TrafficAmber33"),TEXT("TrafficGreen33")};for(auto& S:SignalLamps){if(!RoadJunctions.IsValidIndex(S.Junction))continue;int Active=LWRoads33::Phase(RoadJunctions[S.Junction],S.Approach,GetWorld()->GetTimeSeconds());for(int I=0;I<3;I++)if(S.Lens[I].IsValid())S.Lens[I]->SetMaterial(0,LightingWorld->Material(I==Active?Mats[I]:FName(TEXT("TrafficOff33"))));}}
