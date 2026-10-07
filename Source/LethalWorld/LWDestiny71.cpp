#include "LWCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "LWCampaign76.h"
#include "LWDestiny71.h"
#include "LWWorld.h"
#include "LWZombie.h"
#include "LWStreaming68.h"
#include "LWWorldTextComponent.h"
#include "ProceduralMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"
namespace LWDestiny71 {
#include "LWDestinyData71.inl"
const FData71& Data(){static const FData71 D=MakeData71();return D;}
bool Inside(FVector2D P,const TArray<FVector2D>& Poly){bool B=false;for(int I=0,J=Poly.Num()-1;I<Poly.Num();J=I++){auto A=Poly[I],C=Poly[J];if((A.Y>P.Y)!=(C.Y>P.Y)&&P.X<(C.X-A.X)*(P.Y-A.Y)/(C.Y-A.Y)+A.X)B=!B;}return B;}
bool FloorAt(FVector2D P,int Level){for(const auto& M:Data().Panels){if(M.Z!=Level*600||M.Thickness!=24||M.Rings.IsEmpty()||!Inside(P,M.Rings[0]))continue;bool Hole=false;for(int I=1;I<M.Rings.Num();I++)Hole|=Inside(P,M.Rings[I]);if(!Hole)return true;}return false;}
namespace {
// Relocate saved stock from the retired rectangular mall instead of stranding it at old coordinates.
void RestoreStock(ALWWorld* W,const LWGen::FSite& S,FName NewId,FVector Position){
 if(W->Containers.Contains(NewId))return;const FString Prefix=FString::Printf(TEXT("ny69_%u_"),S.Id);FName Old;
 for(const auto& Pair:W->Containers){const FString Name=Pair.Key.ToString();if(!Name.StartsWith(Prefix)||Pair.Value.bDropped||Pair.Value.bTrader||W->PropStates.Contains(FName(*(TEXT("destiny71_moved_")+Name))))continue;if(Old.IsNone()||Name.Compare(Old.ToString())<0)Old=Pair.Key;}
 if(Old.IsNone())return;auto Record=W->Containers.FindChecked(Old);Record.Id=NewId;Record.Position=Position;W->Containers.FindChecked(Old).Items.Empty();W->Containers.Add(NewId,MoveTemp(Record));W->PropStates.Add(FName(*(TEXT("destiny71_moved_")+Old.ToString())),1);
}
struct FBuild {
 ALWChunk* C;ALWWorld* W;LWGen::FSite S;
 FVector At(FVector P)const{return FVector(S.Position,24)-C->GetActorLocation()+P;}
 void Queue(TFunction<void()> F)const{if(C->Plan68)C->Plan68->Geometry.Add(MoveTemp(F));else F();}
 void Box(FName M,FVector P,FVector Size,float Yaw=0,bool Solid=true)const{if(!Solid)C->Add(W,TEXT("Cube"),M,At(P),Size/100.,FRotator(0,Yaw,0),false);else C->Box(W,M,At(P),Size,FRotator(0,Yaw,0),true);}
 void Mesh(FName M,FVector P,float Yaw=0,FVector Scale=FVector(1),bool Solid=true)const{C->Add(W,M,NAME_None,At(P),Scale,FRotator(0,Yaw,0),Solid);}
 void Label(FString T,FVector P,float Yaw=0,float Size=36)const{auto* L=NewObject<ULWWorldTextComponent>(C);L->SetupAttachment(C->GetRootComponent());L->SetRelativeLocation(At(P));L->SetRelativeRotation(FRotator(0,Yaw,0));L->SetText(FText::FromString(T));L->SetHorizontalAlignment(EHTA_Center);L->SetWorldSize(Size);L->SetCullDistance(16000);L->RegisterComponent();}
 FLWSurfaceBatch26& Batch(FName Mat,bool Hit)const{
  const FName Key(*(Mat.ToString()+TEXT("_destiny71")+(Hit?TEXT("_C"):TEXT("_N"))));
  if(!C->SurfaceMeshes.Contains(Key)){auto* M=NewObject<UProceduralMeshComponent>(C);M->SetupAttachment(C->GetRootComponent());M->bUseAsyncCooking=!C->SyncCollision68;M->SetCollisionProfileName(Hit?TEXT("BlockAll"):TEXT("NoCollision"));M->SetCanEverAffectNavigation(false);M->SetMaterial(0,W->Material(Mat));M->RegisterComponent();C->SurfaceMeshes.Add(Key,M);}
  C->DirtySurfaces68.Add(Key);return C->SurfaceData.FindOrAdd(Key);
 }
 void Tri(FName M,FVector A,FVector B,FVector D,FVector N,bool Hit=true)const{
  auto& Q=Batch(M,Hit);int I=Q.Vertices.Num();for(auto P:{A,B,D}){Q.Vertices.Add(At(P));Q.Normals.Add(N);Q.UV.Add(FMath::Abs(N.Z)>.6?FVector2D(P.X,P.Y)/250:FVector2D(FVector2D(P).Size(),P.Z)/250);}
  if(FVector::DotProduct(FVector::CrossProduct(B-A,D-A),N)>0)Q.Triangles.Append({I,I+2,I+1});else Q.Triangles.Append({I,I+1,I+2});
 }
 void Quad(FName M,FVector A,FVector B,FVector D,FVector E,FVector N,bool Hit=true)const{Tri(M,A,B,D,N,Hit);Tri(M,A,D,E,N,Hit);}
 void Panel(const FPanel71& P)const{
  for(int I=0;I<P.Indices.Num();I+=3){const FVector A(P.Vertices[P.Indices[I]],P.Z),B(P.Vertices[P.Indices[I+1]],P.Z),D(P.Vertices[P.Indices[I+2]],P.Z);Tri(P.Material,A,B,D,FVector::UpVector);Tri(P.Material==TEXT("Terrazzo65")?FName(TEXT("Paint65")):P.Material,A-FVector(0,0,P.Thickness),B-FVector(0,0,P.Thickness),D-FVector(0,0,P.Thickness),-FVector::UpVector);}
  for(const auto& R:P.Rings)for(int I=0;I<R.Num();I++){auto A=R[I],B=R[(I+1)%R.Num()];auto N=FVector(B.Y-A.Y,A.X-B.X,0).GetSafeNormal();Quad(P.Material,FVector(A,P.Z),FVector(B,P.Z),FVector(B,P.Z-P.Thickness),FVector(A,P.Z-P.Thickness),N);}
 }
 void Beam(FName M,FVector A,FVector B,float Radius=10,bool Hit=false,int Sides=8)const{
  FVector Dir=(B-A).GetSafeNormal(),U=FVector::CrossProduct(Dir,FVector::UpVector).GetSafeNormal();if(U.IsNearlyZero())U=FVector::RightVector;const auto V=FVector::CrossProduct(Dir,U);
  for(int I=0;I<Sides;I++){auto R=U*FMath::Cos(I*2*PI/Sides)+V*FMath::Sin(I*2*PI/Sides),T=U*FMath::Cos((I+1)*2*PI/Sides)+V*FMath::Sin((I+1)*2*PI/Sides);Quad(M,A+R*Radius,B+R*Radius,B+T*Radius,A+T*Radius,(R+T).GetSafeNormal(),Hit);}
 }
 void Light(FVector P,bool Warm=true)const{Mesh(TEXT("TubeLight65"),P,0,FVector(1),false);auto* L=NewObject<UPointLightComponent>(C);L->SetupAttachment(C->GetRootComponent());L->SetRelativeLocation(At(P-FVector(0,0,15)));L->SetIntensity(9000);L->SetLightColor(Warm?FLinearColor(1,.88,.69):FLinearColor(.45,.75,1));L->SetAttenuationRadius(1400);L->SetCastShadows(false);L->SetMaxDrawDistance(3200);L->RegisterComponent();}
 void Object(FName M,FVector P,int Key,FName Use=NAME_None,float Yaw=0)const{
  auto B=*this;auto F=[B,M,P,Key,Use,Yaw](){const FName Id(*FString::Printf(TEXT("destiny71_%u_%d"),B.S.Id,Key));if(Use.IsNone())RestoreStock(B.W,B.S,Id,FVector(B.S.Position,24)+P);auto* O=B.W->SpawnObject(Use.IsNone()?ELWObjectKind::Container:ELWObjectKind::Furniture,Id,FVector(B.S.Position,24)+P,FRotator(0,Yaw,0));if(!O)return;if(!Use.IsNone())O->SetFurniture(Use);O->Body->SetStaticMesh(B.W->Mesh(M));O->Body->SetRelativeRotation(FRotator::ZeroRotator);O->SetActorTickEnabled(false);O->Body->SetCullDistance(12000);if(Use.IsNone())B.W->EnsureSiteContainer(Id,B.S,O->GetActorLocation());B.C->Residents.Add(O);};if(C->Plan68)C->Plan68->Population.Add(MoveTemp(F));else F();
 }
 void Rail(FVector2D A,FVector2D B,float Z,bool Glass=true)const{
  auto D=(B-A).GetSafeNormal();const float Yaw=FMath::RadiansToDegrees(FMath::Atan2(D.Y,D.X));const FVector Mid((A+B)*.5,Z+55);float Length=(B-A).Size();
  if(Glass)Box(TEXT("WindowGlass"),Mid,FVector(Length,6,100),Yaw);else for(float T=0;T<Length;T+=100)Box(TEXT("RV66_Chrome"),FVector(A+D*T,Z+55),FVector(5,5,110));
  Beam(TEXT("RV66_Chrome"),FVector(A,Z+110),FVector(B,Z+110),5,true);for(float T=0;T<=Length;T+=150)Box(TEXT("RV66_Chrome"),FVector(A+D*T,Z+55),FVector(8,8,110));
 }
};
float Angle(FVector2D D){return FMath::RadiansToDegrees(FMath::Atan2(D.Y,D.X));}
float SegmentDistance(FVector2D P,FVector2D A,FVector2D B){const auto D=B-A;return (P-(A+D*FMath::Clamp(FVector2D::DotProduct(P-A,D)/FMath::Max(1.,D.SizeSquared()),0.,1.))).Size();}
}
void Build(ALWChunk* C,ALWWorld* W,const LWGen::FSite& S){
 const FBuild B{C,W,S};const auto& D=Data();C->MajorBounds+=FBox2D(S.Position-S.Size*.5,S.Position+S.Size*.5);
 B.Box(TEXT("Asphalt"),FVector(0,0,-25),FVector(S.Size,24));
 for(int I=0;I<D.Panels.Num();I++)B.Queue([B,I](){const auto& P=Data().Panels[I];B.Panel(P);
  if(P.Z==1200||P.Z==1800)for(const auto& Ring:P.Rings)if(&Ring!=&P.Rings[0])for(int J=0;J<Ring.Num();J++){
   auto A=Ring[J],E=Ring[(J+1)%Ring.Num()];const auto Mid=(A+E)*.5;bool Landing=false;for(const auto& Esc:Data().Escalators)if((Esc.Level+1)*600==P.Z&&FVector2D::Distance(Mid,Esc.P+Esc.D*600)<350)Landing=true;
   if(!Landing)B.Rail(A,E,P.Z);
  }
 });
 // Mapped silhouette, with floor-aligned horizontal precast bands and recessed glazing.
 for(int I=0;I<D.Shell.Num();I++)B.Queue([B,I](){const auto& D=Data();auto A=D.Shell[I],E=D.Shell[(I+1)%D.Shell.Num()];auto Dir=(E-A).GetSafeNormal(),N=FVector2D(-Dir.Y,Dir.X);if(Inside((A+E)*.5+N*50,D.Shell))N=-N;
  const float L=(E-A).Size(),Yaw=Angle(Dir);const int Count=FMath::Max(1,FMath::CeilToInt(L/350));
  for(int J=0;J<Count;J++){auto P=FMath::Lerp(A,E,(J+.5)/Count);float H=1800;
   for(const auto& Panel:D.Panels)if(Panel.Z==2500&&!Panel.Rings.IsEmpty()&&Inside(P-N*100,Panel.Rings[0])){H=2500;break;}
   bool Door=false;for(auto Entry:D.Entries)Door|=(Entry-P).Size()<450;
   bool BridgeDoor=SegmentDistance(P,D.Bridge[0],D.Bridge[1])<850||SegmentDistance(P,D.Bridge[2],D.Bridge[3])<850;
   const bool Glass=Door||BridgeDoor||FVector2D::Distance(P,D.Carousel)<2400;const float Width=L/Count;
   for(float Z=600;Z<H;Z+=600){float Height=FMath::Min(600.f,H-Z);bool Opening=(Door&&Z==600)||(BridgeDoor&&Z==1200);float Bottom=Opening?Z+330:Z;
    if(Z+Height>Bottom)B.Box(Glass?TEXT("WindowGlass"):TEXT("RV66_Ivory"),FVector(P,(Bottom+Z+Height)*.5),FVector(Width,Glass?10:35,Z+Height-Bottom),Yaw);
    B.Box(TEXT("Concrete"),FVector(P,Z+Height-12),FVector(Width,45,24),Yaw);B.Box(TEXT("RV66_Chrome"),FVector(P-Dir*Width*.5,Z+Height*.5),FVector(12,42,Height),Yaw);
   }
   if(J%5==0&&!Glass){B.Box(TEXT("RV66_Walnut"),FVector(P+N*30,H*.5+300),FVector(65,10,H-600),Yaw);}
  }
 });
 // Enclose every upper-level edge above the lower roof, including the cinema connector.
 for(int I=0;I<D.Panels.Num();I++)if(D.Panels[I].Z==2500)B.Queue([B,I](){const auto& P=Data().Panels[I];if(P.Rings.IsEmpty())return;const auto& Ring=P.Rings[0];for(int J=0;J<Ring.Num();J++){auto A=Ring[J],E=Ring[(J+1)%Ring.Num()],Mid=(A+E)*.5;float EdgeDistance=MAX_flt;for(int K=0;K<Data().Shell.Num();K++)EdgeDistance=FMath::Min(EdgeDistance,SegmentDistance(Mid,Data().Shell[K],Data().Shell[(K+1)%Data().Shell.Num()]));if(EdgeDistance<80)continue;auto Dir=(E-A).GetSafeNormal();B.Box(TEXT("RV66_Ivory"),FVector(Mid,2150),FVector((E-A).Size()+4,24,700),Angle(Dir));B.Box(TEXT("Concrete"),FVector(Mid,2500),FVector((E-A).Size()+4,40,20),Angle(Dir));}});
 // Original building's open parking undercroft; structural spacing inferred from photographs.
 for(int X=-28000;X<30000;X+=1400)B.Queue([B,X](){const auto& D=Data();for(int Y=-17000;Y<17000;Y+=1400){FVector2D P(X,Y);bool Ground=false;for(const auto& M:D.Panels)if(M.Z==0&&!M.Rings.IsEmpty()&&Inside(P,M.Rings[0]))Ground=true;if(!Ground)continue;bool Shop=false;for(const auto& Z:D.Zones)if(Z.Level==0&&(Z.Center-P).Size()<5000)Shop=true;if(Shop)continue;B.Box(TEXT("Concrete"),FVector(P,290),FVector(60,60,580));B.Box(TEXT("Lane"),FVector(P+FVector2D(500,0),2),FVector(8,520,3),0,false);}});
 // Shop floors follow the actual-shaped tenant blocks; entrance widths, fixtures and stock are floor-rooted.
 for(int Index=0;Index<D.Zones.Num();Index++)B.Queue([B,Index](){const auto& Z=Data().Zones[Index];float Base=Z.Level*600;int Kind=LWGen::Hash(Z.Id,Z.Level,int32(B.S.Id),7101)%12;int Key=100000+Index*100;
  static const TCHAR* Names[]={TEXT("NORTHLINE OUTFITTERS"),TEXT("ELECTRIC AVENUE"),TEXT("PAPER & VINYL"),TEXT("CROWN DEPARTMENT STORE"),TEXT("TRAIL & FIELD"),TEXT("CORNERSTONE HOME"),TEXT("STUDIO FOOTWEAR"),TEXT("GOLDEN HOUR"),TEXT("DAYBREAK GIFTS"),TEXT("LITTLE PLANET TOYS"),TEXT("PANTRY & TABLE"),TEXT("NOVA FASHION")};
  static const TCHAR* Fixtures[]={TEXT("ClothesRail65"),TEXT("Shelf65"),TEXT("Bookcase65"),TEXT("DisplayTable53"),TEXT("SportsRack57"),TEXT("Sideboard65"),TEXT("Shelf65"),TEXT("Cabinet65"),TEXT("DisplayTable53"),TEXT("Shelf65"),TEXT("DiningTable65"),TEXT("ClothesRail65")};
  int Front=0;for(const auto& E:Z.Edges){float Len=(E.B-E.A).Size();auto Dir=(E.B-E.A).GetSafeNormal();float Yaw=Angle(Dir);
   if(!E.Front){B.Box(TEXT("Plaster65"),FVector((E.A+E.B)*.5,Base+280),FVector(Len,18,560),Yaw);continue;}
   int Bays=FMath::Max(1,FMath::RoundToInt(Len/1800));for(int K=0;K<Bays;K++){auto Mid=FMath::Lerp(E.A,E.B,(K+.5)/Bays);float Width=Len/Bays,Gap=FMath::Min(280.f,Width*.7f);
    B.Box(TEXT("RV66_Chrome"),FVector(Mid,Base+460),FVector(Width,24,180),Yaw);
    for(int Side:{-1,1}){float Part=(Width-Gap)*.5;B.Box(TEXT("WindowGlass"),FVector(Mid+Dir*(Side*(Gap*.5+Part*.5)),Base+180),FVector(Part,8,360),Yaw);B.Box(TEXT("RV66_Brass"),FVector(Mid+Dir*(Side*Gap*.5),Base+185),FVector(10,16,370),Yaw);}
    if(Front<12){B.Label(Z.Level==3?(Kind%3==0?TEXT("CANYON GRILL"):Names[Kind]):Names[Kind],FVector(Mid-E.In*24,Base+408),Angle(-E.In),30);Front++;}
   }
  }
  const auto* StoryPlayer=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(B.W,0));const bool Clinic=StoryPlayer&&StoryPlayer->RPG.Campaign76.Started&&Index==LWCampaign76::MarketZone();
  for(int J=0;J<Z.Fixtures.Num();J++){const auto P=Z.Fixtures[J];if(Clinic&&(P-Z.Center).Size()<1200)continue;if(!FloorAt(P,Z.Level))continue;const float Facing=(J%2)*180.f;
   FName Model=Fixtures[Kind];if((Kind==0||Kind==11)&&J%3==1)Model=TEXT("RoundRack53");if(Kind==3)Model=J%3==0?TEXT("ClothesRail65"):J%3==1?TEXT("DisplayTable53"):TEXT("Cabinet65");if(J<3)B.Object(Model,FVector(P,Base),Key+J);else B.Mesh(Model,FVector(P,Base),Facing);
   if(J%14==0)B.Light(FVector(P,Base+535));
   if(J%19==5){B.Mesh(TEXT("Pallet65"),FVector(P+FVector2D(0,180),Base));B.Mesh(TEXT("CleaningCart65"),FVector(P+FVector2D(180,0),Base));}
  }
  if(!Clinic&&Z.Fixtures.Num()>3){auto P=Z.Fixtures[3]+FVector2D(200,0);B.Mesh(TEXT("Counter65"),FVector(P,Base));B.Mesh(TEXT("CashRegister53"),FVector(P,Base+100));}
 });
 // Open courts and exposed framing: glazing in the courts is not capped by opaque slabs.
 for(int I=0;I<D.Route.Num();I++)B.Queue([B,I](){const auto& R=Data().Route[I];const auto N=FVector2D(-R.D.Y,R.D.X);bool Canyon=SegmentDistance(R.P,Data().CanyonA,Data().CanyonB)<1300;
  for(int L=1;L<=2;L++){float Z=L*600;if(I%2==0)B.Light(FVector(Canyon?R.P+N*850:R.P,Z+435));
   bool Clear=FloorAt(R.P+N*360,L)&&FloorAt(R.P-N*360,L);for(const auto& E:Data().Escalators)if(SegmentDistance(R.P,E.P-E.D*900,E.P+E.D*900)<800)Clear=false;
   if(I%4==0&&Clear){B.Object(TEXT("WaitingBench65"),FVector(R.P+N*360,Z),800000+I*4+L,TEXT("chair"),Angle(-N));B.Mesh(TEXT("Planter65"),FVector(R.P-N*360,Z));}
   if(!Canyon&&FloorAt(R.P,L)&&(R.P-Data().Atrium).Size()>1600){B.Beam(TEXT("RV66_Walnut"),FVector(R.P-N*500,Z+550),FVector(R.P+N*500,Z+550),18);}
  }
 });
 // Stair dimensions are fixed, with matching holes and guarded landings on the next floor.
 for(int I=0;I<D.Escalators.Num();I++)B.Queue([B,I](){const auto& E=Data().Escalators[I];const auto N=FVector2D(-E.D.Y,E.D.X);const float Z=E.Level*600,Yaw=Angle(E.D);
  for(int Side:{-1,1}){auto Center=E.P+N*(Side*140);for(int J=0;J<54;J++){auto P=Center+E.D*(-600+(J+.5)*1200/54);float H=(J+1)*600.f/54;B.Box(TEXT("RV66_Chrome"),FVector(P,Z+H-8),FVector(1200.f/54,220,16),Yaw);B.Box(TEXT("Lane"),FVector(P+E.D*(1200.f/108-2),Z+H+1),FVector(4,214,2),Yaw,false);}
   for(int Edge:{-1,1}){auto A=Center+N*(Edge*115)-E.D*600,End=Center+N*(Edge*115)+E.D*600;B.Beam(TEXT("Rubber"),FVector(A,Z+95),FVector(End,Z+695),7,true);B.Quad(TEXT("WindowGlass"),FVector(A,Z+20),FVector(End,Z+620),FVector(End,Z+690),FVector(A,Z+90),FVector(N*Edge,0));}
  }
  B.Label(FString::Printf(TEXT("LEVEL %d"),E.Level+1),FVector(E.P-E.D*800,Z+150),Angle(-E.D),45);
 });
 // Glazed Hiawatha pedestrian bridge follows the mapped footprint and reaches level 2.
 B.Queue([B](){const auto& D=Data();if(D.Bridge.Num()<4)return;auto A=(D.Bridge[0]+D.Bridge[3])*.5,E=(D.Bridge[1]+D.Bridge[2])*.5;
  if((E-A).Size()<1000){A=(D.Bridge[0]+D.Bridge[1])*.5;E=(D.Bridge[2]+D.Bridge[3])*.5;}
  auto Dir=(E-A).GetSafeNormal(),N=FVector2D(-Dir.Y,Dir.X);float Yaw=Angle(Dir),Len=(E-A).Size();B.Box(TEXT("Concrete"),FVector((A+E)*.5,1184),FVector(Len+60,650,32),Yaw);B.Box(TEXT("WindowGlass"),FVector((A+E)*.5,1580),FVector(Len+60,650,16),Yaw);
  for(int Side:{-1,1}){B.Quad(TEXT("WindowGlass"),FVector(A+N*Side*325,1200),FVector(E+N*Side*325,1200),FVector(E+N*Side*325,1580),FVector(A+N*Side*325,1580),FVector(N*Side,0));}
  for(float T=0;T<Len;T+=450){auto P=A+Dir*T;for(int Side:{-1,1})B.Box(TEXT("RV66_Chrome"),FVector(P+N*(Side*325),1390),FVector(18,18,400));B.Beam(TEXT("RV66_Chrome"),FVector(P-N*325,1590),FVector(P+N*325,1590),12);}
  const auto Far=FVector2D::Distance(A,D.CanyonA)>FVector2D::Distance(E,D.CanyonA)?A:E;for(int I=0;I<72;I++)B.Box(TEXT("Concrete"),FVector(Far+N*(I*30),1200-I*1200.f/72-10),FVector(650,30,20),Yaw);
  B.Label(TEXT("HIAWATHA BRIDGE"),FVector((A+E)*.5,1480),Angle(-N),55);
 });
 // Exterior entrance terraces and accessible ramps connect the raised first level to the lots.
 for(int I=0;I<D.Entries.Num();I++)B.Queue([B,I](){const auto& D=Data();auto P=D.Entries[I];FVector2D N(0,1);float Best=MAX_flt;for(int J=0;J<D.Shell.Num();J++){auto A=D.Shell[J],E=D.Shell[(J+1)%D.Shell.Num()];float Dist=SegmentDistance(P,A,E);if(Dist<Best){Best=Dist;auto Dir=(E-A).GetSafeNormal();N=FVector2D(-Dir.Y,Dir.X);if(Inside(P+N*80,D.Shell))N=-N;}}
  const auto T=FVector2D(-N.Y,N.X),A=P+N*500,E=P+N*7700;B.Box(TEXT("Concrete"),FVector(P+N*250,584),FVector(600,1200,32),Angle(N));
  B.Quad(TEXT("Concrete"),FVector(A-T*250,600),FVector(E-T*250,0),FVector(E+T*250,0),FVector(A+T*250,600),FVector::UpVector);
  for(int Side:{-1,1})B.Beam(TEXT("RV66_Chrome"),FVector(A+T*(Side*255),700),FVector(E+T*(Side*255),100),5,true);
  B.Box(TEXT("WindowGlass"),FVector(P+N*500,1070),FVector(1200,1200,20),Angle(N));B.Label(I==0?TEXT("DESTINY USA / THE CANYON"):I==2?TEXT("DESTINY USA / CAROUSEL COURT"):TEXT("DESTINY USA"),FVector(P+N*60,980),Angle(N),70);
 });
 // Canyon's sculptural laminated-timber trees and tensile fabric petals, from architect photographs.
 B.Queue([B](){const auto& D=Data();auto Dir=(D.CanyonB-D.CanyonA).GetSafeNormal(),N=FVector2D(-Dir.Y,Dir.X);float Len=(D.CanyonB-D.CanyonA).Size();
  for(float T=700;T<Len-400;T+=1800){auto P=D.CanyonA+Dir*T;B.Mesh(TEXT("Planter65"),FVector(P,600),0,FVector(3,3,1.2));
   for(int K=0;K<10;K++){float A=K*2*PI/10;auto R=Dir*FMath::Cos(A)+N*FMath::Sin(A);FVector Last(P+R*75,700);for(int Segment=1;Segment<=10;Segment++){float CurveT=Segment/10.f,U=1-CurveT;FVector Next=FVector(P+R*75,700)*(U*U*U)+FVector(P+R*100,1400)*(3*U*U*CurveT)+FVector(P+R*360,2200)*(3*U*CurveT*CurveT)+FVector(P+R*850,2380)*(CurveT*CurveT*CurveT);B.Beam(TEXT("RV66_Walnut"),Last,Next,18,false,8);Last=Next;}B.Beam(TEXT("RV66_Walnut"),FVector(P+R*100,1000),FVector(P+R*600,1860),13,false,6);
    auto V=Dir*FMath::Cos(A+.42)+N*FMath::Sin(A+.42);B.Tri(TEXT("RV66_Ivory"),FVector(P+R*850,2380),FVector(P+V*800,2500),FVector(P+R*250,2160),FVector::DownVector,false);
   }
   B.Light(FVector(P,1700),false);
  }
  for(float T=0;T<Len+200;T+=450){auto P=D.CanyonA+Dir*T;B.Beam(TEXT("RV66_Chrome"),FVector(P-N*1050,2500),FVector(P,2800),10);B.Beam(TEXT("RV66_Chrome"),FVector(P,2800),FVector(P+N*1050,2500),10);}
  B.Quad(TEXT("WindowGlass"),FVector(D.CanyonA-N*1050,2500),FVector(D.CanyonB-N*1050,2500),FVector(D.CanyonB,2800),FVector(D.CanyonA,2800),FVector::UpVector,false);
  B.Quad(TEXT("WindowGlass"),FVector(D.CanyonA,2800),FVector(D.CanyonB,2800),FVector(D.CanyonB+N*1050,2500),FVector(D.CanyonA+N*1050,2500),FVector::UpVector,false);
  for(int Side:{-1,1}){auto P=Side<0?D.CanyonA:D.CanyonB;B.Tri(TEXT("WindowGlass"),FVector(P-N*1050,2500),FVector(P+N*1050,2500),FVector(P,2800),FVector(Dir*Side,0));}
  B.Label(TEXT("THE CANYON"),FVector(D.CanyonB,1020),Angle(-Dir),65);
 });
 // Octagonal original atrium tower, green-framed glazing and a central elevator bank.
 B.Queue([B](){auto P=Data().Atrium;for(int I=0;I<8;I++){float A=I*PI/4;auto R=FVector2D(FMath::Cos(A),FMath::Sin(A)),T=FVector2D(FMath::Cos(A+PI/4),FMath::Sin(A+PI/4));
  B.Beam(TEXT("CanadaArmor51"),FVector(P+R*1200,1800),FVector(P+R*1200,3800),22);B.Quad(TEXT("WindowGlass"),FVector(P+R*1200,1800),FVector(P+T*1200,1800),FVector(P+T*1200,3800),FVector(P+R*1200,3800),FVector(R,0),false);B.Tri(TEXT("WindowGlass"),FVector(P+R*1200,3800),FVector(P+T*1200,3800),FVector(P,4200),FVector::UpVector,false);}
  for(int I=0;I<8;I++){auto A=FVector2D(FMath::Cos(I*PI/4),FMath::Sin(I*PI/4))*1200,E=FVector2D(FMath::Cos((I+1)*PI/4),FMath::Sin((I+1)*PI/4))*1200;B.Beam(TEXT("CanadaArmor51"),FVector(P+A,3800),FVector(P+E,3800),18);B.Beam(TEXT("CanadaArmor51"),FVector(P+A,3800),FVector(P,4200),14);}
  auto E=P+FVector2D(1200,0);for(int Side:{-1,1}){B.Box(TEXT("RV66_Chrome"),FVector(E+FVector2D(0,Side*200),1200),FVector(30,30,2400));B.Box(TEXT("WindowGlass"),FVector(E+FVector2D(0,Side*200),1200),FVector(380,8,2400));}for(int L=0;L<4;L++){B.Box(TEXT("RV66_Chrome"),FVector(E,L*600+170),FVector(360,12,340));B.Label(TEXT("ELEVATORS"),FVector(E-FVector2D(200,0),L*600+370),180,32);}
 });
 // Carousel Court: real round projection, upper food terrace and a purpose-built antique carousel.
 B.Queue([B](){auto P=Data().Carousel;const float Z=1200;for(int I=0;I<48;I++){float A=I*2*PI/48,Next=(I+1)*2*PI/48;auto R=FVector2D(FMath::Cos(A),FMath::Sin(A)),T=FVector2D(FMath::Cos(Next),FMath::Sin(Next));B.Tri(TEXT("Wood"),FVector(P,Z+20),FVector(P+R*700,Z+20),FVector(P+T*700,Z+20),FVector::UpVector);B.Tri(I%2?TEXT("RV66_Ivory"):TEXT("Red"),FVector(P,Z+490),FVector(P+R*760,Z+370),FVector(P+T*760,Z+370),FVector::UpVector,false);B.Beam(TEXT("RV66_Brass"),FVector(P+R*760,Z+370),FVector(P+T*760,Z+370),7);}
  for(int I=0;I<24;I++){float A=I*PI/12;auto R=FVector2D(FMath::Cos(A),FMath::Sin(A)),T=FVector2D(-R.Y,R.X);auto Q=P+R*(I%2?510:330);B.Beam(TEXT("RV66_Brass"),FVector(Q,Z+20),FVector(Q,Z+390),3);const FVector2D Profile[]={{-70,0},{-48,38},{30,42},{48,105},{90,118},{98,85},{65,65},{70,0},{45,-12},{-50,-15}};
   for(int J=1;J<9;J++)for(int Side:{-1,1})B.Tri(TEXT("RV66_Ivory"),FVector(Q+T*Profile[0].X+R*(Side*12),Z+100+Profile[0].Y),FVector(Q+T*Profile[J].X+R*(Side*12),Z+100+Profile[J].Y),FVector(Q+T*Profile[J+1].X+R*(Side*12),Z+100+Profile[J+1].Y),FVector(R*Side,0),false);
   for(int Leg:{-1,1})B.Beam(TEXT("RV66_Ivory"),FVector(Q+T*(Leg*40),Z+100),FVector(Q+T*(Leg*55),Z+35),7);B.Box(TEXT("Red"),FVector(Q,Z+146),FVector(40,34,8),Angle(T),false);
  }
  B.Label(TEXT("CAROUSEL COURT / FOOD TERRACE"),FVector(P+FVector2D(1400,0),Z+350),180,44);
  for(int I=0;I<12;I++){auto Q=P+FVector2D((I%4-1.5)*500,1300+(I/4)*450);if(!FloorAt(Q,2)||!FloorAt(Q+FVector2D(0,180),2)||!FloorAt(Q-FVector2D(0,180),2))continue;B.Mesh(TEXT("DiningTable65"),FVector(Q,Z));for(int Side:{-1,1})B.Object(TEXT("Chair65"),FVector(Q+FVector2D(0,Side*140),Z),900000+I*3+(Side>0),TEXT("chair"),Side>0?180:0);}
 });
 // Metal cloud-ceiling portals at the original/expansion connections.
 for(int I:{18,19,20,48,49,50})if(D.Route.IsValidIndex(I))B.Queue([B,I](){const auto& R=Data().Route[I];auto N=FVector2D(-R.D.Y,R.D.X);for(int L=1;L<=2;L++)for(int J=0;J<10;J++){float A=-PI*.38+J*PI*.076,Next=A+PI*.076;auto P=R.P;B.Quad(TEXT("RV66_Chrome"),FVector(P+N*(FMath::Sin(A)*520)-R.D*220,L*600+380+FMath::Cos(A)*160),FVector(P+N*(FMath::Sin(A)*520)+R.D*220,L*600+380+FMath::Cos(A)*160),FVector(P+N*(FMath::Sin(Next)*520)+R.D*220,L*600+380+FMath::Cos(Next)*160),FVector(P+N*(FMath::Sin(Next)*520)-R.D*220,L*600+380+FMath::Cos(Next)*160),FVector::DownVector,false);}});
 // Access streets and parking aisles follow the map, including the road under the pedestrian bridge.
 for(int I=0;I<D.Roads.Num();I++)B.Queue([B,I](){const auto& R=Data().Roads[I];auto Dir=(R.B-R.A).GetSafeNormal(),N=FVector2D(-Dir.Y,Dir.X);float Len=(R.B-R.A).Size(),Yaw=Angle(Dir);B.Box(TEXT("Asphalt"),FVector((R.A+R.B)*.5,-9),FVector(Len+30,R.Width,6),Yaw);
  if(R.Parking){for(float T=0;T<Len;T+=270)for(int Side:{-1,1}){auto P=R.A+Dir*T+N*(Side*(R.Width*.5+260));bool Clear=!Inside(P,Data().Shell);for(auto E:Data().Entries)if((P-E).Size()<8000)Clear=false;if(Clear)B.Box(TEXT("Lane"),FVector(P,-4),FVector(7,520,3),Yaw,false);}}
  else{for(float T=0;T<Len;T+=600)B.Box(TEXT("Lane"),FVector(R.A+Dir*T,-4),FVector(250,8,3),Yaw,false);if(I%5==0)B.C->StreetLight(B.W,B.At(FVector((R.A+R.B)*.5+N*(R.Width*.5+120),0)),FRotator(0,Yaw,0));}
 });
 // Population uses explicit supported first-level positions and persistent IDs.
 for(int I=6;I<D.Route.Num();I+=18){auto Spawn=[B,I](){const auto& R=Data().Route[I];if(!FloorAt(R.P,1)||B.W->ZombieCount>=100)return;const uint32 Id=B.S.Id^(0x71cafeu+I);if(B.W->KilledZombies.Contains(Id))return;FVector P=FVector(B.S.Position+R.P,718);FCollisionQueryParams Q;FHitResult H;if(!B.W->GetWorld()->LineTraceSingleByChannel(H,P,P-FVector(0,0,110),ECC_Visibility,Q)||FMath::Abs(H.ImpactPoint.Z-624)>8)return;if(B.W->GetWorld()->OverlapBlockingTestByChannel(P,FQuat::Identity,ECC_Pawn,FCollisionShape::MakeCapsule(40,88),Q))return;FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;if(auto* Z=B.W->GetWorld()->SpawnActor<ALWZombie>(P,FRotator::ZeroRotator,Params)){Z->PersistentId=Id;Z->ConfigureKind(I%3?ELWEnemyKind::Zombie:ELWEnemyKind::Raider);Z->Home=P;B.C->Residents.Add(Z);B.W->ZombieCount++;}};if(C->Plan68)C->Plan68->Population.Add(MoveTemp(Spawn));else Spawn();}
}
}

