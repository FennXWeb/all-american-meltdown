#include "LWAircraft84.h"
#include "LWWorld.h"
#include "LWVehicle.h"
#include "LWCharacter.h"
#include "LWNewYork69.h"
#include "LWGeography84.h"
#include "LWCanada68.h"
#include "LWStreaming68.h"
#include "LWWorldTextComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "ProceduralMeshComponent.h"
#include "LWAirportData84.inl"
namespace LWAviation84 {
const TArray<FRunway>& Runways(){static const TArray<FRunway> Data=[](){TArray<FRunway> R;
 auto G=[](double Lat,double Lon){return FVector(LWNY69::Project(Lat,Lon),12);};
 const FVector SYR=G(43.1133,-76.1112);auto S=[&](double Lat,double Lon){return SYR+FVector((Lat-43.1133)*11110000.,(Lon+76.1112)*8130000.,0);};
 R.Add({TEXT("SYR10"),TEXT("Syracuse Hancock / 10–28"),S(43.1081977,-76.1261762),S(43.1093106,-76.092479),SYR+FVector(-15000,-5000,0),4600});
 R.Add({TEXT("SYR15"),TEXT("Syracuse Hancock / 15–33"),S(43.1212335,-76.1128358),S(43.1069875,-76.0925882),SYR+FVector(12000,12000,0),4600});
 auto Regional=[&](FName Id,const TCHAR* Name,double Lat,double Lon,float Heading,float Length){// Keep the full-size playable Griffiss strip clear of the compressed Rome story district.
 FVector C=G(Lat,Lon)+(Id==TEXT("RME")?FVector(0,135000,0):FVector::ZeroVector);FVector D=FRotator(0,Heading,0).Vector();R.Add({Id,Name,C-D*(Length*.5),C+D*(Length*.5),C+FVector(-D.Y,D.X,0)*9500,4000});};
 Regional(TEXT("BUF"),TEXT("Buffalo Niagara"),42.9405,-78.7322,48,140000);
 Regional(TEXT("ROC"),TEXT("Greater Rochester"),43.1189,-77.6724,40,140000);
 Regional(TEXT("ART"),TEXT("Watertown International"),43.9919,-76.0217,70,120000);
 Regional(TEXT("RME"),TEXT("Rome / Griffiss"),43.232,-75.407,150,160000);
 Regional(TEXT("YYZ"),TEXT("Toronto Pearson"),43.6777,-79.6248,60,180000);
 return R;}();return Data;}
const FRunway* Runway(FName Id){return Runways().FindByPredicate([&](const auto& R){return R.Id==Id;});}
const FRunway* Closest(FVector2D P,float MinimumLength,bool AllowCanada){const FRunway* Best=nullptr;double Dist=DBL_MAX;for(const auto& R:Runways()){if(R.Length()<MinimumLength||(!AllowCanada&&LWGeography84::Canada(FVector2D(R.Apron))))continue;const double D=FVector2D::DistSquared(P,FVector2D(R.Apron));if(D<Dist){Best=&R;Dist=D;}}return Best;}
float Surface(FVector2D P,float Terrain){for(const auto& R:Runways()){FVector2D D(R.Direction()),Q=P-FVector2D(R.A);double T=FVector2D::DotProduct(Q,D),Side=FMath::Abs(Q.X*D.Y-Q.Y*D.X);if(T>-6000&&T<R.Length()+6000&&Side<R.Width*.5+4000)return FMath::Lerp(R.A.Z-12,Terrain,FMath::Clamp(float((Side-R.Width*.5-500)/3500),0.f,1.f));if(FVector2D::Distance(P,FVector2D(R.Apron))<9500)return R.Apron.Z-12;}return Terrain;}
namespace {
void Strip(ALWChunk* C,ALWWorld* W,FVector A,FVector B,float Width,FName Mat,float Z,bool Solid=true){FVector D=B-A;D.Z=0;if(D.Size2D()<1)return;C->Box(W,Mat,(A+B)*.5-C->GetActorLocation()+FVector(0,0,Z),FVector(D.Size2D(),Width,12),D.Rotation(),Solid);}
bool Clip(FVector& A,FVector& B,FBox2D Box){const FVector Original=A,D=B-A;double Lo=0,Hi=1;for(int I=0;I<2;I++){double X=I?A.Y:A.X,V=I?D.Y:D.X,Min=I?Box.Min.Y:Box.Min.X,Max=I?Box.Max.Y:Box.Max.X;if(FMath::Abs(V)<.001){if(X<Min||X>Max)return false;}else{double U=(Min-X)/V,T=(Max-X)/V;if(U>T)Swap(U,T);Lo=FMath::Max(Lo,U);Hi=FMath::Min(Hi,T);if(Lo>Hi)return false;}}A=Original+D*Lo;B=Original+D*Hi;return true;}
}
void Ground(ALWChunk* C,ALWWorld* W){const FVector O=C->GetActorLocation();const FBox2D Bounds(FVector2D(O),FVector2D(O)+FVector2D(LWGen::ChunkSize));
 for(const auto& R:Runways()){FVector A=R.A,B=R.B;if(Clip(A,B,Bounds)){Strip(C,W,A,B,R.Width,TEXT("Asphalt"),-5);const FVector D=R.Direction(),Right(-D.Y,D.X,0);for(int Side:{-1,1})Strip(C,W,A+Right*(Side*(R.Width*.5-50)),B+Right*(Side*(R.Width*.5-50)),24,TEXT("Lane"),2,false);for(float T=0;T<R.Length();T+=3000){FVector P=R.A+D*T;if(!Bounds.IsInside(FVector2D(P)))continue;Strip(C,W,P,P+D*1100,50,TEXT("Lane"),3,false);for(int Side:{-1,1})C->Box(W,TEXT("Glow"),P+Right*(Side*(R.Width*.5+60))-O+FVector(0,0,8),FVector(28,28,16),FRotator::ZeroRotator,false);}}
  // Apron and a taxi lane connect to the departure threshold without crossing the runway.
  FVector D=R.Direction(),Right(-D.Y,D.X,0);FVector Corner=R.A+Right*9500;FVector2D Min=FVector2D(R.Apron)-FVector2D(8000),Max=FVector2D(R.Apron)+FVector2D(8000);FVector2D L(FMath::Max(Min.X,Bounds.Min.X),FMath::Max(Min.Y,Bounds.Min.Y)),H(FMath::Min(Max.X,Bounds.Max.X),FMath::Min(Max.Y,Bounds.Max.Y));if(H.X>L.X&&H.Y>L.Y)C->Box(W,TEXT("Concrete"),FVector((L+H)*.5,R.Apron.Z-7)-O,FVector(H-L,12));
  for(auto Pair:{TPair<FVector,FVector>(R.Apron,Corner),{Corner,R.A}}){A=Pair.Key;B=Pair.Value;if(Clip(A,B,Bounds))Strip(C,W,A,B,2400,TEXT("Asphalt"),-5);}
 }
 const FVector SYR(LWNY69::Project(43.1133,-76.1112),12);for(int I=0;I<UE_ARRAY_COUNT(AirportData84::Taxi);I+=2){FVector A=SYR+FVector(AirportData84::Taxi[I],0),B=SYR+FVector(AirportData84::Taxi[I+1],0);if(Clip(A,B,Bounds))Strip(C,W,A,B,2286,TEXT("Asphalt"),-5);}
}
void BuildAirport(ALWChunk* C,ALWWorld* W,const LWGen::FSite& S){
 const bool Syracuse=S.Id==0xEA690004u;const FRunway* Field=Closest(S.Position);if(!Field)return;const FVector Origin=C->GetActorLocation();C->MajorBounds+=FBox2D(S.Position-S.Size*.5,S.Position+S.Size*.5);
 auto Job=[&](TFunction<void()> Fn){if(C->Plan68)C->Plan68->Geometry.Add(MoveTemp(Fn));else Fn();};
 if(Syracuse){
  // The union of the five surveyed terminal buildings preserves both concourses,
  // the central security hall, landside curve and the original gate fingers.
  const FVector Base(S.Position,0);
  auto Deck=[&](const FVector2D* Points,int Count,float Z,FName Material,bool Down){for(int Start=0;Start<Count;Start+=90)Job([C,W,Base,Points,Count,Z,Material,Down,Start](){const FName Key(*(TEXT("Airport84_")+Material.ToString()));auto& Batch=C->SurfaceData.FindOrAdd(Key);for(int I=Start;I<FMath::Min(Start+90,Count);I+=3){int Index=Batch.Vertices.Num();for(int J=0;J<3;J++){FVector2D P=Points[I+J];Batch.Vertices.Add(Base+FVector(P,Z)-C->GetActorLocation());Batch.Normals.Add(FVector(0,0,Down?-1:1));Batch.UV.Add(P/350);}Batch.Triangles.Append({Index,Index+(Down?1:2),Index+(Down?2:1)});}if(!C->SurfaceMeshes.Contains(Key)){auto* M=NewObject<UProceduralMeshComponent>(C);M->SetupAttachment(C->GetRootComponent());M->bUseAsyncCooking=!C->SyncCollision68;M->SetCollisionProfileName(TEXT("BlockAll"));M->SetCanEverAffectNavigation(false);M->SetMaterial(0,W->Material(Material));M->RegisterComponent();C->SurfaceMeshes.Add(Key,M);}C->DirtySurfaces68.Add(Key);});};
  Deck(AirportData84::Floor,UE_ARRAY_COUNT(AirportData84::Floor),24,TEXT("TileV7"),false);
  Deck(AirportData84::UpperFloor,UE_ARRAY_COUNT(AirportData84::UpperFloor),384,TEXT("Acoustic65"),true);
  Deck(AirportData84::UpperFloor,UE_ARRAY_COUNT(AirportData84::UpperFloor),404,TEXT("TileV7"),false);
  Deck(AirportData84::Floor,UE_ARRAY_COUNT(AirportData84::Floor),760,TEXT("Acoustic65"),true);
  Deck(AirportData84::Floor,UE_ARRAY_COUNT(AirportData84::Floor),780,TEXT("Concrete"),false);
  for(const auto& Stair:AirportData84::Stairs)Job([C,W,Base,Stair](){FVector P=Base+FVector(Stair,24)-C->GetActorLocation();for(int I=0;I<20;I++)C->Box(W,TEXT("Concrete"),P+FVector(I*40+20,130,(I+1)*9.5),FVector(40,260,(I+1)*19));C->Box(W,TEXT("TileV7"),P+FVector(900,130,370),FVector(200,260,20));for(int Side:{-1,1}){C->Box(W,TEXT("Steel"),P+FVector(500,130+Side*145,425),FVector(1020,12,10));for(int X=0;X<=1000;X+=200)C->Box(W,TEXT("Steel"),P+FVector(X,130+Side*145,420),FVector(10,10,90));}});
  for(int Start=0;Start<UE_ARRAY_COUNT(AirportData84::Walls);Start+=24)Job([C,W,Base,Start](){for(int I=Start;I<FMath::Min<int32>(Start+24,UE_ARRAY_COUNT(AirportData84::Walls));I+=2){FVector A=Base+FVector(AirportData84::Walls[I],0),B=Base+FVector(AirportData84::Walls[I+1],0),D=B-A;float Len=D.Size2D();FRotator R=D.Rotation();const int Bays=FMath::Max(1,FMath::CeilToInt(Len/1000));for(int J=0;J<Bays;J++){FVector P=A+D*((J+.5f)/Bays);bool Door=Len>1800&&J==Bays/2&&I%6==0;C->Box(W,TEXT("Concrete"),P-C->GetActorLocation()+FVector(0,0,690),FVector(Len/Bays+3,35,140),R);if(!Door){C->Box(W,TEXT("Concrete"),P-C->GetActorLocation()+FVector(0,0,100),FVector(Len/Bays+3,35,150),R);C->Box(W,TEXT("Glass"),P-C->GetActorLocation()+FVector(0,0,385),FVector(Len/Bays-30,12,420),R);}C->Box(W,TEXT("Steel"),P-D.GetSafeNormal()*(Len/Bays*.5)-C->GetActorLocation()+FVector(0,0,380),FVector(30,40,740),R);}}});
 }else{
  FVector At=FVector(S.Position,24)-Origin;C->Box(W,TEXT("TileV7"),At,FVector(9000,4200,40));C->Box(W,TEXT("Concrete"),At+FVector(0,0,700),FVector(9040,4240,35));for(int Side:{-1,1})C->Box(W,TEXT("Concrete"),At+FVector(Side*4500,0,350),FVector(40,4200,700));for(int Side:{-1,1})for(int J:{-1,1})C->Box(W,TEXT("Glass"),At+FVector(J*2500,Side*2100,350),FVector(4000,20,700));
 }
 // Terminal A/B check-in and reclaim are downstairs; gate lounges and security
 // are upstairs. Fixtures are baked only where their footprint fits the survey.
 const FVector Base(S.Position,24);int Serial=0;
 if(Syracuse)for(const auto& Zone:AirportData84::Zones){const int N=Serial++;Job([C,W,Base,Zone,N,S](){const FVector At=Base+FVector(Zone.At,Zone.Kind>=2?380:0),O=C->GetActorLocation();const FRotator Facing(0,Zone.Yaw,0);auto Local=[&](FVector P){return At+Facing.RotateVector(P)-O;};
  if(Zone.Kind==0){for(int I=-1;I<=1;I++){C->Add(W,TEXT("SalesCounter53"),NAME_None,Local(FVector(I*180,0,0)));C->Add(W,TEXT("CashRegister53"),NAME_None,Local(FVector(I*180,0,100)));}C->Box(W,TEXT("Rubber"),Local(FVector(0,160,60)),FVector(650,110,110));}
  if(Zone.Kind==1)C->Add(W,TEXT("AV84_BaggageCarousel"),NAME_None,Local(FVector::ZeroVector));
  if(Zone.Kind==2){for(int Row:{-1,1})for(int Col:{-1,0,1})C->Add(W,TEXT("WaitingBench65"),NAME_None,Local(FVector(Row*230,Col*280,0)),FVector(1),Facing+FRotator(0,Row<0?0:180,0));C->Add(W,TEXT("SalesCounter53"),NAME_None,Local(FVector(450,0,0)),FVector(1),Facing);}
  if(Zone.Kind==3){C->Add(W,TEXT("AV84_SecurityScanner"),NAME_None,Local(FVector::ZeroVector));for(int Side:{-1,1})C->Box(W,TEXT("Steel"),Local(FVector(0,300+Side*70,130)),FVector(45,16,260));C->Box(W,TEXT("Steel"),Local(FVector(0,300,255)),FVector(45,155,18));for(int X=-4;X<=0;X++)for(int Y:{-1,1}){C->Box(W,TEXT("Steel"),Local(FVector(X*180,300+Y*130,50)),FVector(12,12,100));if(X<0)C->Box(W,TEXT("Rubber"),Local(FVector(X*180+90,300+Y*130,90)),FVector(180,8,20));}}
  C->Add(W,TEXT("WasteBinV13"),NAME_None,Local(FVector(500,500,0)));C->Box(W,TEXT("Glow"),Local(FVector(0,0,325)),FVector(500,35,8),Facing,false);
  const FString Label=Zone.Kind==0?TEXT("CHECK IN"):Zone.Kind==1?TEXT("BAGGAGE CLAIM"):Zone.Kind==3?TEXT("SECURITY"):FString::Printf(TEXT("GATE %s %02d"),Zone.At.X<0?TEXT("A"):TEXT("B"),N+1);
  const FVector SignAt=Local(FVector(0,480,250));C->Box(W,TEXT("Steel"),SignAt,FVector(540,15,100),Facing);auto* Sign=NewObject<ULWWorldTextComponent>(C);Sign->SetupAttachment(C->GetRootComponent());Sign->SetRelativeLocation(SignAt+Facing.RotateVector(FVector(0,-10,15)));Sign->SetRelativeRotation(Facing+FRotator(0,-90,0));Sign->SetWorldSize(26);Sign->SetHorizontalAlignment(EHTA_Center);Sign->SetText(FText::FromString(Label));Sign->RegisterComponent();
  FName Id(*FString::Printf(TEXT("airport84_%u_shop%d"),S.Id,N));const FVector Storage=At+Facing.RotateVector(FVector(450,-440,0));W->EnsureSiteContainer(Id,S,Storage);if(auto* O2=W->SpawnObject(ELWObjectKind::Container,Id,Storage)){O2->Body->SetStaticMesh(W->Mesh(TEXT("CabinetV3")));C->Residents.Add(O2);}
 });}
 else for(int I=-3;I<=3;I++){C->Add(W,TEXT("WaitingBench65"),NAME_None,Base+FVector(I*1000,500,0)-Origin);C->Add(W,TEXT("SalesCounter53"),NAME_None,Base+FVector(I*1000,-900,0)-Origin);}
 const FVector Service=FVector(S.Position,24)+(Syracuse?FVector(7500,-5000,0):FVector(0,-900,0));Job([C,W,S,Service](){if(auto* O=W->SpawnObject(ELWObjectKind::Furniture,FName(*FString::Printf(TEXT("airport84_service_%u"),S.Id)),Service)){O->SetFurniture(TEXT("airfield84"));O->Body->SetStaticMesh(W->Mesh(TEXT("EV74_ControlPanel")));O->Body->SetRelativeScale3D(FVector(3));C->Residents.Add(O);}});
 const FRunway Airport=*Field;Job([C,W,S,Airport](){int I=0;for(FName Model:{FName(TEXT("private_jet")),FName(TEXT("airbus")),FName(TEXT("luxury_airbus"))}){FName Id(*FString::Printf(TEXT("airport84_%u_plane%d"),S.Id,I));if(!W->Vehicles.Contains(Id)){FLWVehicleRecord R;R.Model=Model;R.VIN=FGuid::NewDeterministicGuid(Id.ToString());auto D=Airport.Direction();R.Position=Airport.Apron+D*((I-1)*6000)+FVector(-D.Y,D.X,0)*4500;R.Rotation=Airport.Direction().Rotation();R.Unlocked=R.Hotwired=true;R.FuelLitres=Spec(Model).Capacity;W->Vehicles.Add(Id,R);}if(auto* V=W->SpawnObject(ELWObjectKind::Car,Id,W->Vehicles[Id].Position,W->Vehicles[Id].Rotation))C->Residents.Add(V);I++;}});
}
}
