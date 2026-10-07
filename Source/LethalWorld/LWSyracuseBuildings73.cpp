#include "LWSyracuse73.h"
#include "LWWorld.h"
#include "LWStreaming68.h"
#include "LWWorldTextComponent.h"
#include "ProceduralMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/World.h"
namespace LWSyracuse73 {
namespace {
struct FBuild {
 ALWChunk* C;ALWWorld* W;LWGen::FSite S;
 FVector World(FVector P)const{return FVector(S.Position,24)+FRotator(0,S.Yaw,0).RotateVector(P);}
 FVector At(FVector P)const{return World(P)-C->GetActorLocation();}
 void Queue(TFunction<void()> F)const{if(C->Plan68)C->Plan68->Geometry.Add(MoveTemp(F));else F();}
 void Box(FName M,FVector P,FVector Size,float Yaw=0,bool Hit=true)const{C->Box(W,M,At(P),Size,FRotator(0,S.Yaw+Yaw,0),Hit);}
 void Mesh(FName M,FVector P,float Yaw=0,FVector Scale=FVector(1),bool Hit=true)const{C->Add(W,M,NAME_None,At(P),Scale,FRotator(0,S.Yaw+Yaw,0),Hit);}
 void Label(FString Text,FVector P,float Yaw=-90,float Size=35)const{auto* T=NewObject<ULWWorldTextComponent>(C);T->SetupAttachment(C->GetRootComponent());T->SetRelativeLocation(At(P));T->SetRelativeRotation(FRotator(0,S.Yaw+Yaw,0));T->SetText(FText::FromString(Text));T->SetWorldSize(Size);T->SetHorizontalAlignment(EHTA_Center);T->SetCullDistance(26000);T->RegisterComponent();}
 void Light(FVector P,float Radius=1400,FLinearColor Color=FLinearColor(1,.83,.62))const{auto* L=NewObject<UPointLightComponent>(C);L->SetupAttachment(C->GetRootComponent());L->SetRelativeLocation(At(P));L->SetIntensity(Radius>=3000?150000:18000);L->SetAttenuationRadius(Radius);L->SetLightColor(Color);L->SetCastShadows(false);L->SetMaxDrawDistance(7000);L->RegisterComponent();}
 void Tri(FName M,FVector A,FVector B,FVector D,bool Hit=true)const{
  const FName Key(*(M.ToString()+(Hit?TEXT("_syr73_C"):TEXT("_syr73_N"))));
  if(!C->SurfaceMeshes.Contains(Key)){auto* Mesh=NewObject<UProceduralMeshComponent>(C);Mesh->SetupAttachment(C->GetRootComponent());Mesh->bUseAsyncCooking=!C->SyncCollision68;Mesh->SetCollisionProfileName(Hit?TEXT("BlockAll"):TEXT("NoCollision"));Mesh->SetCanEverAffectNavigation(false);Mesh->SetMaterial(0,W->Material(M));Mesh->RegisterComponent();C->SurfaceMeshes.Add(Key,Mesh);}
  auto& Q=C->SurfaceData.FindOrAdd(Key);C->DirtySurfaces68.Add(Key);const int I=Q.Vertices.Num();auto N=FVector::CrossProduct(B-A,D-A).GetSafeNormal();for(auto P:{A,B,D}){Q.Vertices.Add(At(P));Q.Normals.Add(FRotator(0,S.Yaw,0).RotateVector(N));Q.UV.Add(FMath::Abs(N.Z)>.6?FVector2D(P.X,P.Y)/200:FVector2D(P.X+P.Y,P.Z)/200);}Q.Triangles.Append({I,I+2,I+1});
 }
 void Quad(FName M,FVector A,FVector B,FVector D,FVector E,bool Hit=true)const{Tri(M,A,B,D,Hit);Tri(M,A,D,E,Hit);}
 void Beam(FName M,FVector A,FVector B,float R=10,bool Hit=false)const{const auto D=(B-A).GetSafeNormal();auto U=FVector::CrossProduct(D,FVector::UpVector).GetSafeNormal();if(U.IsNearlyZero())U=FVector::RightVector;const auto V=FVector::CrossProduct(D,U);for(int I=0;I<8;I++){auto P=U*FMath::Cos(I*PI/4)+V*FMath::Sin(I*PI/4),Q=U*FMath::Cos((I+1)*PI/4)+V*FMath::Sin((I+1)*PI/4);Quad(M,A+Q*R,B+Q*R,B+P*R,A+P*R,Hit);}}
 void Stairs(FVector Bottom,FVector2D Dir,int Steps,float Rise=16,float Run=28,float Width=160)const{float Yaw=FMath::RadiansToDegrees(FMath::Atan2(Dir.Y,Dir.X));for(int I=0;I<Steps;I++)Box(TEXT("Concrete"),Bottom+FVector(Dir*((I+.5)*Run),(I+1)*Rise*.5),FVector(Run+1,Width,(I+1)*Rise),Yaw);}
 void Rail(FVector A,FVector B)const{Beam(TEXT("Steel"),A+FVector(0,0,105),B+FVector(0,0,105),4,true);int N=FMath::Max(1,FMath::CeilToInt((B-A).Size()/140));for(int I=0;I<=N;I++){auto P=FMath::Lerp(A,B,double(I)/N);Beam(TEXT("Steel"),P,P+FVector(0,0,105),4,true);}}
 void Object(FName Model,FVector P,int Key,FName Use=NAME_None,float Yaw=0)const{auto B=*this;auto F=[B,Model,P,Key,Use,Yaw](){FName Id(*FString::Printf(TEXT("syr73_%u_%d"),B.S.Id,Key));auto* O=B.W->SpawnObject(Use.IsNone()?ELWObjectKind::Container:ELWObjectKind::Furniture,Id,B.World(P),FRotator(0,B.S.Yaw+Yaw,0));if(!O)return;if(!Use.IsNone())O->SetFurniture(Use);O->Body->SetStaticMesh(B.W->Mesh(Model));O->Body->SetRelativeRotation(FRotator::ZeroRotator);O->Body->SetCullDistance(12000);if(Use.IsNone())B.W->EnsureSiteContainer(Id,B.S,O->GetActorLocation());O->SetActorTickEnabled(false);B.C->Residents.Add(O);};if(C->Plan68)C->Plan68->Population.Add(MoveTemp(F));else F();}
 // Front-facing partition with a human-sized portal and its lintel, never a fake door texture.
 void Wall(FVector P,float Width,float Height,FName M=TEXT("PlasterV7"),float Door=160)const{for(int Side:{-1,1})Box(M,P+FVector(Side*(Width+Door)*.25,0,Height*.5),FVector((Width-Door)*.5,22,Height));if(Height>240)Box(M,P+FVector(0,0,240+(Height-240)*.5),FVector(Door,22,Height-240));}
 void Room(FVector P,FVector2D Size,int Key,const TCHAR* Name)const{Box(TEXT("TileV7"),P-FVector(0,0,10),FVector(Size,20));Wall(P+FVector(0,-Size.Y*.5,0),Size.X,320);Box(TEXT("PlasterV7"),P+FVector(0,Size.Y*.5,160),FVector(Size.X,22,320));for(int Side:{-1,1})Box(TEXT("PlasterV7"),P+FVector(Side*Size.X*.5,0,160),FVector(22,Size.Y,320));Box(TEXT("PalaceCeiling73"),P+FVector(0,0,330),FVector(Size,20));Object(TEXT("Locker65"),P+FVector(-Size.X*.5+80,Size.Y*.5-80,0),Key);Object(TEXT("Counter65"),P+FVector(Size.X*.5-110,Size.Y*.5-80,0),Key+1);Label(Name,P+FVector(0,-Size.Y*.5-16,275),-90,24);Light(P+FVector(0,0,300),600);}
};
void Palace(FBuild B){
 const float X=B.S.Size.X*.5,Y=B.S.Size.Y*.5;
 B.Box(TEXT("Wood"),FVector(0,0,-12),FVector(X*2,Y*2,24));B.Wall(FVector(0,-Y,0),X*2,900,TEXT("BrickV7"),300);B.Wall(FVector(0,Y,0),X*2,900,TEXT("BrickV7"),180);
 for(int Side:{-1,1}){B.Box(TEXT("BrickV7"),FVector(Side*X,0,450),FVector(28,Y*2,900));B.Box(TEXT("PalaceVelvet73"),FVector(Side*(X-18),200,280),FVector(8,Y*1.3,540));}
 B.Box(TEXT("PalaceCeiling73"),FVector(0,0,910),FVector(X*2,Y*2,24));B.Box(TEXT("Roof73"),FVector(0,0,932),FVector(X*2+20,Y*2+20,20));
 B.Box(TEXT("ArtDeco73"),FVector(0,-Y-18,610),FVector(X*2-100,20,550));for(int I=-2;I<=2;I++){B.Box(TEXT("Steel"),FVector(I*370,-Y-35,620),FVector(210,15,250));B.Box(TEXT("WindowGlass"),FVector(I*370,-Y-46,620),FVector(180,8,215),0,false);}
 B.Box(TEXT("RedPaint73"),FVector(-X+150,-Y-120,655),FVector(210,130,510));for(int I=0;I<6;I++)B.Label(FString::Chr(TEXT("PALACE")[I]),FVector(-X+150,-Y-192,850-I* 70),-90,65);

 // Front lobby, two restrooms, and ticket/concession counters.
 B.Wall(FVector(0,-Y+900,0),X*2,340,TEXT("PalaceVelvet73"),360);
 B.Object(TEXT("Counter65"),FVector(-X+200,-Y+420,0),1);B.Mesh(TEXT("CashRegister53"),FVector(-X+200,-Y+420,104));B.Mesh(TEXT("TicketBooth73"),FVector(-550,-Y-180,0));
 for(int Side:{-1,1}){B.Room(FVector(Side*(X-290),-Y+1250,0),FVector2D(540,650),10+(Side>0?4:0),Side>0?TEXT("RESTROOM"):TEXT("RESTROOM"));B.Mesh(TEXT("Toilet65"),FVector(Side*(X-290),-Y+1460,0));B.Mesh(TEXT("Sink65"),FVector(Side*(X-450),-Y+1280,0));}
 // Auditorium: central/side aisles, sloped seating terraces and a clear stage approach.
 const float Front=-Y+1650,Back=Y-1150,Span=Back-Front;const int Rows=FMath::Clamp(int(Span/90),14,27);
 B.Stairs(FVector(0,Front-300,0),FVector2D(0,1),10,(Rows-1)*5.5f/10,30,180);
 for(int R=0;R<Rows;R++)B.Queue([B,X,Y,Front,Span,Rows,R](){const float YY=Front+(R+.5)*Span/Rows;const float Z=(Rows-1-R)*5.5;B.Box(TEXT("Wood"),FVector(0,YY,Z*.5),FVector(X*2-90,Span/Rows+1,FMath::Max(2.f,Z)));const int Seats=FMath::Max(4,int((X-250)/58));for(int Side:{-1,1})for(int I=0;I<Seats;I++)B.Mesh(TEXT("TheatreSeat73"),FVector(Side*(115+I*58),YY,Z),180,FVector(1),false);});
 // Proscenium, fly loft, stage deck and backstage dressing rooms are physically connected.
 B.Box(TEXT("Wood"),FVector(0,Y-690,45),FVector(X*2-80,1250,90));B.Stairs(FVector(-X+240,Y-1450,0),FVector2D(0,1),6,15,28,260);
 for(int Side:{-1,1}){B.Mesh(TEXT("PalacePilaster73"),FVector(Side*(X-230),Y-1290,90),0,FVector(1.5,1.5,2.1));B.Box(TEXT("PalaceVelvet73"),FVector(Side*(X-380),Y-1240,430),FVector(320,38,680),0,false);}
 B.Box(TEXT("ArtDeco73"),FVector(0,Y-1280,780),FVector(X*2-200,60,100));B.Box(TEXT("Projection73"),FVector(0,Y-780,400),FVector(X*1.15,20,450),0,false);
 B.Room(FVector(-X+320,Y-280,90),FVector2D(580,500),30,TEXT("DRESSING ROOM"));B.Room(FVector(X-320,Y-280,90),FVector2D(580,500),34,TEXT("PRODUCTION"));
 for(int I=0;I<5;I++)for(int Side:{-1,1})B.Mesh(TEXT("PalacePilaster73"),FVector(Side*(X-70),Front+I*Span/5,120),Side>0?-90:90,FVector(1,1,1.5),false);
 // Real second-floor event room, kitchen and projection booth over the front of house.
 B.Box(TEXT("Wood"),FVector(-160,-Y+540,350),FVector(X*2-400,1080,24));B.Stairs(FVector(X-150,-Y+80,0),FVector2D(0,1),22,16,34,190);B.Box(TEXT("Wood"),FVector(X-150,-Y+920,350),FVector(280,250,24));
 B.Box(TEXT("Wood"),FVector(X-370,-Y+980,350),FVector(700,350,24));B.Box(TEXT("Wood"),FVector(0,-Y+1120,350),FVector(X*2-80,180,24));
 B.Rail(FVector(-X+40,-Y+1080,365),FVector(X-380,-Y+1080,365));B.Room(FVector(-X+360,-Y+490,364),FVector2D(640,780),40,TEXT("KITCHEN"));B.Mesh(TEXT("KitchenRangeV9"),FVector(-X+210,-Y+670,364));B.Mesh(TEXT("Fridge65"),FVector(-X+520,-Y+700,364));
 for(int I=0;I<4;I++){FVector P(-X+1050+(I%2)*380,-Y+250+(I/2)*420,364);B.Mesh(TEXT("DiningTable65"),P);for(int Side:{-1,1})B.Object(TEXT("Chair65"),P+FVector(0,Side*110,0),60+I*2+(Side>0),TEXT("chair"),Side>0?180:0);}
 B.Room(FVector(0,-Y+1310,364),FVector2D(650,400),80,TEXT("PROJECTION"));B.Mesh(TEXT("Desk65"),FVector(0,-Y+1390,364));
 B.Box(TEXT("Steel"),FVector(0,-Y-170,360),FVector(X*2+100,450,65));B.Box(TEXT("Bone"),FVector(0,-Y-410,365),FVector(X*2,16,135),0,false);B.Label(TEXT("THE PALACE THEATRE"),FVector(0,-Y-424,375),-90,72);
 for(int I=0;I<5;I++)B.Light(FVector(0,-Y+500+I*900,750),1800);B.Label(TEXT("2384 JAMES STREET"),FVector(0,-Y-35,260),-90,28);
}
void Arena(FBuild B,bool Dome){
 const float X=B.S.Size.X*.5-60,Y=B.S.Size.Y*.5-60;const float FX=Dome?5500:3050,FY=Dome?2450:1300;const float H=Dome?3000:2000;const int Rows=Dome?34:19;
 B.Box(Dome?TEXT("Turf73"):TEXT("Ice73"),FVector(0,0,-14),FVector(X*2,Y*2,28));
 // Regulation-sized playing area; arena circulation is outside the continuous seating bowl.
 B.Box(Dome?TEXT("Turf73"):TEXT("Ice73"),FVector(0,0,2),FVector(FX*2,FY*2,3),0,false);
 for(int I=0;I<=(Dome?12:4);I++){float P=-FX+2*FX*I/(Dome?12:4);B.Box(Dome?TEXT("LineWhite73"):I==2?TEXT("RedPaint73"):TEXT("BluePaint73"),FVector(P,0,5),FVector(Dome?9:18,FY*2,2),0,false);}
 if(Dome){for(int Side:{-1,1}){B.Beam(TEXT("Bone"),FVector(Side*(FX+100),0,0),FVector(Side*(FX+100),0,300),12);B.Beam(TEXT("Bone"),FVector(Side*(FX+100),-280,300),FVector(Side*(FX+100),280,300),9);for(int K:{-1,1})B.Beam(TEXT("Bone"),FVector(Side*(FX+100),K*280,300),FVector(Side*(FX+100),K*280,900),9);}}
 else {for(int Side:{-1,1}){B.Wall(FVector(0,Side*(FY+70),0),FX*2,110,TEXT("Bone"),320);B.Wall(FVector(0,Side*(FY+70),110),FX*2,110,TEXT("WindowGlass"),320);B.Box(TEXT("Bone"),FVector(Side*(FX+80),0,55),FVector(18,FY*2,110));}B.Wall(FVector(0,-FY-80,0),700,110,TEXT("Bone"),160);}
 // Rink face-off circles, goals and team benches; football sidelines and hash marks.
 if(!Dome){for(int C=0;C<5;C++){FVector Center(C==0?0:(C<3?-1800:1800),C==0?0:(C%2?-720:720),8);for(int I=0;I<48;I++){float A=I*2*PI/48,D=(I+1)*2*PI/48;B.Beam(TEXT("RedPaint73"),Center+FVector(FMath::Cos(A)*450,FMath::Sin(A)*450,0),Center+FVector(FMath::Cos(D)*450,FMath::Sin(D)*450,0),4);}}
  for(int Side:{-1,1}){float XX=Side*2650;B.Beam(TEXT("RedPaint73"),FVector(XX,-90,0),FVector(XX,-90,122),4);B.Beam(TEXT("RedPaint73"),FVector(XX,90,0),FVector(XX,90,122),4);B.Beam(TEXT("RedPaint73"),FVector(XX,-90,122),FVector(XX,90,122),4);for(int I=-3;I<=3;I++)B.Beam(TEXT("LineWhite73"),FVector(XX,I*30,120),FVector(XX+Side*75,I*30,0),1.2);for(int I=0;I<4;I++)B.Object(TEXT("WaitingBench65"),FVector(-850+I*550,Side*(FY+320),0),700+(Side>0?10:0)+I,TEXT("chair"),Side>0?180:0);}
 }else{for(int Side:{-1,1}){B.Box(TEXT("LineWhite73"),FVector(0,Side*FY,8),FVector(FX*2,12,2),0,false);for(int I=0;I<100;I++)B.Box(TEXT("LineWhite73"),FVector(-4572+I*91.44,Side*950,8),FVector(8,70,2),0,false);}}
 const float RX=FX+800,RY=FY+800;const float Run=FMath::Min((X-RX-600)/Rows,(Y-RY-600)/Rows);const float Rise=Dome?32:27,Top=Rows*Rise;
 auto Curve=[](float A,float AX,float AY,float Z){return FVector(FMath::Sign(FMath::Cos(A))*FMath::Sqrt(FMath::Abs(FMath::Cos(A)))*AX,FMath::Sign(FMath::Sin(A))*FMath::Sqrt(FMath::Abs(FMath::Sin(A)))*AY,Z);};
 for(int Sector=0;Sector<48;Sector++)B.Queue([B,Dome,X,Y,RX,RY,Rows,Run,Rise,Top,Sector,Curve](){
  const float A0=Sector*2*PI/48,A1=(Sector+1)*2*PI/48;
  auto Strip=[&](float Inner,float Outer,float Z,float Prev){for(int K=0;K<4;K++){float A=FMath::Lerp(A0,A1,K/4.f),D=FMath::Lerp(A0,A1,(K+1)/4.f);auto P=Curve(A,RX+Inner,RY+Inner,Z),Q=Curve(D,RX+Inner,RY+Inner,Z),U=Curve(D,RX+Outer,RY+Outer,Z),V=Curve(A,RX+Outer,RY+Outer,Z);B.Quad(TEXT("Concrete"),V,U,Q,P);B.Quad(TEXT("Concrete"),P,Q,Q-FVector(0,0,Z-Prev),P-FVector(0,0,Z-Prev));}};
  for(int R=0;R<Rows;R++){
   if((Sector%12==0||Sector%12==11)&&R<Rows-3)continue;
   const float Z=(R+1)*Rise;
   if(Sector%6==3){Strip(R*Run,(R+.5)*Run,Z-Rise*.5,Z-Rise);Strip((R+.5)*Run,(R+1)*Run,Z,Z-Rise*.5);continue;}
   Strip(R*Run,(R+1)*Run,Z,Z-Rise);
   auto Left=Curve(A0,RX+(R+.5)*Run,RY+(R+.5)*Run,Z),Right=Curve(A1,RX+(R+.5)*Run,RY+(R+.5)*Run,Z);int Count=FMath::Clamp(int((Right-Left).Size()/62),2,60);
   for(int I=0;I<Count;I++){float A=FMath::Lerp(A0,A1,(I+.5f)/Count);auto P=Curve(A,RX+(R+.5)*Run,RY+(R+.5)*Run,Z),Ahead=Curve(A+.001,RX+(R+.5)*Run,RY+(R+.5)*Run,Z);float Yaw=(Ahead-P).Rotation().Yaw+180;B.Mesh(TEXT("ArenaSeat73"),P,Yaw,FVector(1),false);}
  }
  Strip(Rows*Run,Rows*Run+600,Top,Top-24);
  auto P=Curve((A0+A1)*.5,RX+Rows*Run+350,RY+Rows*Run+350,Top);
  if(Sector%6==0){B.Label(FString::Printf(TEXT("SECTION %d"),100+Sector),P+FVector(0,0,210),FMath::RadiansToDegrees((A0+A1)*.5)+90,45);B.Object(TEXT("Counter65"),P,100+Sector);B.Mesh(TEXT("CashRegister53"),P+FVector(0,0,105));B.Light(P+FVector(0,0,450),2200);}
 });
 const float InnerX=RX+Rows*Run-200,InnerY=RY+Rows*Run-200;
 const float StairEnd=-Y+400+FMath::RoundToInt(Top/16)*27;
 for(int Side:{-1,1}){
  auto Floor=[&](float A,float D,float U,float V){if(D>A&&V>U)B.Box(TEXT("Concrete"),FVector(Side*(A+D)*.5,(U+V)*.5,Top-12),FVector(D-A,V-U,24));};
  Floor(InnerX,X,-Y,-Y+240);Floor(InnerX,X,StairEnd,Y);Floor(InnerX,X-540,-Y+240,StairEnd);Floor(X-160,X,-Y+240,StairEnd);
  B.Box(TEXT("Concrete"),FVector(0,Side*(Y+InnerY)*.5,Top-12),FVector(InnerX*2,Y-InnerY,24));
 }
 // Continuous facade with four real entrances and doors opening directly onto the floor tunnels.
 B.Wall(FVector(0,-Y,0),X*2,H,TEXT("Concrete"),600);B.Wall(FVector(0,Y,0),X*2,H,TEXT("Concrete"),600);
 for(int Side:{-1,1}){B.Box(TEXT("Concrete"),FVector(Side*X,-Y*.5-160,H*.5),FVector(30,Y-320,H));B.Box(TEXT("Concrete"),FVector(Side*X,Y*.5+160,H*.5),FVector(30,Y-320,H));B.Box(TEXT("Concrete"),FVector(Side*X,0,H*.5+180),FVector(30,640,H-360));
  B.Stairs(FVector(Side*(X-350),-Y+400,0),FVector2D(0,1),FMath::RoundToInt(Top/16),16,27,300);
  B.Box(TEXT("Concrete"),FVector(Side*(X-350),-Y+700+FMath::RoundToInt(Top/16)*27,Top-12),FVector(650,600,24));
 }
 // Continuous double-sided curved roof, steel ribs, and the Dome's raised crown/cable silhouette.
 for(int K=0;K<20;K++)B.Queue([B,Dome,X,Y,H,K](){float A=-X+K*X/10,E=-X+(K+1)*X/10;for(int J=0;J<32;J++){float Y1=-Y+J*Y/16,Y2=-Y+(J+1)*Y/16;auto Z=[&](float X0,float Y0){return H+(Dome?1500:1200)*FMath::Sqrt(FMath::Max(0.f,1-FMath::Square(Y0/Y)))*(Dome?(.85+.15*FMath::Cos(X0/X*PI*.5)):1);};FVector V1(A,Y1,Z(A,Y1)),V2(E,Y1,Z(E,Y1)),V3(E,Y2,Z(E,Y2)),V4(A,Y2,Z(A,Y2));B.Quad(TEXT("Roof73"),V1,V2,V3,V4);B.Quad(TEXT("Steel"),V4-FVector(0,0,35),V3-FVector(0,0,35),V2-FVector(0,0,35),V1-FVector(0,0,35));if(K%2==0)B.Beam(TEXT("Steel"),V1-FVector(0,0,90),V4-FVector(0,0,90),Dome?25:15);}});
 // Seal the curved roof ends instead of leaving open crescents above the end walls.
 for(int Side:{-1,1})for(int J=0;J<32;J++){float A=-Y+J*Y/16,D=-Y+(J+1)*Y/16;auto Z=[&](float V){return H+(Dome?1275:1200)*FMath::Sqrt(FMath::Max(0.f,1-FMath::Square(V/Y)));};B.Quad(TEXT("Concrete"),FVector(Side*X,A,H),FVector(Side*X,D,H),FVector(Side*X,D,Z(D)),FVector(Side*X,A,Z(A)));B.Quad(TEXT("Concrete"),FVector(Side*X,A,Z(A)),FVector(Side*X,D,Z(D)),FVector(Side*X,D,H),FVector(Side*X,A,H));}
 for(int I=0;I<16;I++)for(int Side:{-1,1}){float XX=-X+I*(X*2/15);B.Box(TEXT("Steel"),FVector(XX,Side*Y,H*.5),FVector(40,70,H));if(Dome){B.Beam(TEXT("Steel"),FVector(XX,Side*Y,H),FVector(XX,Side*(Y-400),H+950),24);B.Beam(TEXT("Steel"),FVector(XX,Side*(Y-400),H+950),FVector(XX,0,H+1420),10);}}
 B.Box(TEXT("Steel"),FVector(0,0,H-600),FVector(Dome?1600:650,Dome?1200:650,Dome?800:350));for(int Side:{-1,1}){B.Box(TEXT("Glass"),FVector(0,Side*(Dome?605:330),H-600),FVector(Dome?1500:580,8,Dome?650:280),0,false);B.Label(TEXT("SYRACUSE"),FVector(0,Side*(Dome?615:342),H-580),Side>0?90:-90,Dome?120:65);}
 // Locker rooms, concessions and toilets occupy the outer corners, off the arena circulation.
 for(int Side:{-1,1})for(int End:{-1,1}){FVector P(Side*(X-1200),End*(Y-800),0);B.Room(P,FVector2D(1000,950),400+(Side>0?20:0)+(End>0?10:0),End>0?TEXT("TEAM LOCKERS"):TEXT("RESTROOMS"));if(End<0){B.Mesh(TEXT("Toilet65"),P+FVector(-280,320,0));B.Mesh(TEXT("Sink65"),P+FVector(280,320,0));}}
 if(!Dome){
  // Memorial hall, veterans' exhibits, production offices and upper club floor.
  for(int I=0;I<6;I++){FVector P(-X+1000+I*(X*2-2000)/5,-Y+110,180);B.Box(TEXT("ArtDeco73"),P,FVector(420,24,250));B.Label(TEXT("IN HONOR OF ALL WHO SERVED"),P+FVector(0,18,0),90,14);}
  for(int Side:{-1,1}){float Z=Top+420;const float Begin=-Y*.525,End=Y*.525,HoleEnd=-Y*.6+28*28;B.Box(TEXT("Wood"),FVector(Side*(X-1018),(Begin+End)*.5,Z-12),FVector(415,End-Begin,24));B.Box(TEXT("Wood"),FVector(Side*(X-332),(Begin+End)*.5,Z-12),FVector(315,End-Begin,24));B.Box(TEXT("Wood"),FVector(Side*(X-650),(HoleEnd+End)*.5,Z-12),FVector(320,End-HoleEnd,24));B.Stairs(FVector(Side*(X-650),-Y*.6,Top),FVector2D(0,1),28,15,28,220);B.Rail(FVector(Side*(X-1250),-Y*.5,Z),FVector(Side*(X-1250),Y*.5,Z));for(int I=0;I<5;I++){FVector P(Side*(X-650),-Y*.23+I*Y*.16,Z);B.Mesh(TEXT("DiningTable65"),P);B.Object(TEXT("Chair65"),P+FVector(0,120,0),600+(Side>0?10:0)+I,TEXT("chair"));}}
 }
 B.Label(Dome?TEXT("CARRIER DOME"):TEXT("ONONDAGA COUNTY WAR MEMORIAL"),FVector(0,-Y-35,H*.67),-90,Dome?170:105);
 for(int I=0;I<6;I++)B.Box(TEXT("Glow"),FVector(-FX+I*FX*.4,0,H-290),FVector(160,90,12),0,false);
 for(int I=0;I<6;I++)B.Light(FVector(-FX+I*FX*.4,0,H-300),Dome?5500:4000,FLinearColor(.78,.85,1));
}
void Historic(FBuild B){
 const bool Niagara=B.S.Type==71;const float X=B.S.Size.X*.5,Y=B.S.Size.Y*.5;const float H=Niagara?1700:1450;
 B.Box(Niagara?TEXT("ArtDeco73"):TEXT("CampusStone73"),FVector(0,0,H*.5),FVector(X*2,Y*2,H));B.Box(TEXT("Steel"),FVector(0,0,H+25),FVector(X*2+100,Y*2+100,50));
 for(int Side:{-1,1})for(int I=0;I<int(X*2/260);I++)for(int L=0;L<4;L++){float XX=-X+150+I*260;B.Box(TEXT("Glass"),FVector(XX,Side*(Y+4),250+L*340),FVector(125,8,210),0,false);B.Box(Niagara?TEXT("Steel"):TEXT("Bone"),FVector(XX,Side*(Y+13),135+L*340),FVector(160,25,16),0,false);}
 if(Niagara){for(int Tier=0;Tier<5;Tier++){float Width=1700-Tier*240;B.Box(TEXT("ArtDeco73"),FVector(0,-Y+250,1500+Tier*230),FVector(Width,1100-Tier*120,300));for(int Side:{-1,1})B.Box(TEXT("RV66_Chrome"),FVector(Side*(Width*.5-60),-Y-20,1500+Tier*230),FVector(35,30,320));}B.Mesh(TEXT("SpiritLight73"),FVector(0,-Y-90,1350),0,FVector(2.1),false);B.Label(TEXT("NIAGARA MOHAWK"),FVector(0,-Y-35,550),-90,78);}
 else{for(int I=-1;I<=1;I++){float XX=I*X*.73,H0=I==0?2900:2100;B.Box(TEXT("CampusStone73"),FVector(XX,0,H0*.5),FVector(650,800,H0));for(int J=0;J<4;J++){float A=J*PI*.5;B.Tri(TEXT("Steel"),FVector(XX+500*FMath::Cos(A),500*FMath::Sin(A),H0),FVector(XX+500*FMath::Cos(A+PI*.5),500*FMath::Sin(A+PI*.5),H0),FVector(XX,0,H0+650));}}B.Label(TEXT("SYRACUSE UNIVERSITY"),FVector(0,-Y-25,500),-90,75);}
 B.Mesh(TEXT("Rubble"),FVector(0,-Y-90,25),0,FVector(2.5,1.2,1.4));B.Label(TEXT("STRUCTURE UNSAFE"),FVector(0,-Y-120,240),-90,28);
}
void Block(FBuild B){
 const float X=B.S.Size.X*.5,Y=B.S.Size.Y*.5,H=FMath::Max(330,B.S.Floors*320);FName Mat=B.S.District==5?TEXT("CampusStone73"):B.S.District==3?TEXT("BrickV7"):TEXT("Brick");
 B.Box(Mat,FVector(0,0,H*.5),FVector(X*2,Y*2,H));B.Box(TEXT("Steel"),FVector(0,0,H+12),FVector(X*2+30,Y*2+30,24));
 for(int Side:{-1,1})for(int L=0;L<B.S.Floors;L++)for(int I=0;I<FMath::Min(18,int(X*2/260));I++){float XX=-X+140+I*260;B.Box(TEXT("Glass"),FVector(XX,Side*(Y+4),L*320+180),FVector(120,8,170),0,false);B.Box(TEXT("Bone"),FVector(XX,Side*(Y+8),L*320+85),FVector(145,20,14),0,false);}
 B.Box(TEXT("Wood"),FVector(0,-Y-6,110),FVector(130,12,220));if(B.S.District!=1){B.Mesh(TEXT("Rubble"),FVector(0,-Y-130,0),0,FVector(1.1));}else{for(int Side:{-1,1}){B.Quad(TEXT("Roof73"),FVector(-X,Side*Y,H),FVector(X,Side*Y,H),FVector(X,0,H+360),FVector(-X,0,H+360));B.Quad(TEXT("Roof73"),FVector(-X,0,H+360),FVector(X,0,H+360),FVector(X,Side*Y,H),FVector(-X,Side*Y,H));}B.Box(TEXT("Wood"),FVector(0,-Y-170,35),FVector(400,340,70));}
 if(B.S.PlaceName69.Contains(TEXT("Crouse College"))){for(int L=0;L<3;L++)B.Box(TEXT("CampusStone73"),FVector(-X*.65,0,H+L*450),FVector(700-L*80,700-L*80,500));for(int J=0;J<8;J++){float A=J*PI/4;B.Tri(TEXT("Steel"),FVector(-X*.65+400*FMath::Cos(A),400*FMath::Sin(A),H+1300),FVector(-X*.65+400*FMath::Cos(A+PI/4),400*FMath::Sin(A+PI/4),H+1300),FVector(-X*.65,0,H+2500));}}
}
void Square(FBuild B){
 B.Box(TEXT("Concrete"),FVector(0,0,-15),FVector(B.S.Size,30));B.Box(TEXT("Steel"),FVector(-600,0,12),FVector(3500,2600,24));B.Box(TEXT("RV66_Water"),FVector(-600,0,30),FVector(3400,2500,8),0,false);
 for(int I=0;I<6;I++){FVector P(-1800+(I%3)*1400,(I<3?-1:1)*3100,0);B.Object(TEXT("WaitingBench65"),P,10+I,TEXT("chair"),90);B.Mesh(TEXT("Planter65"),P+FVector(0,450,0));}
 // Soldiers and Sailors monument: stepped plinth, classical column and figure.
 FVector M(2400,180,0);for(int I=0;I<4;I++)B.Box(TEXT("Concrete"),M+FVector(0,0,I*45+22),FVector(1100-I*140,1100-I*140,45));B.Beam(TEXT("Bone"),M+FVector(0,0,180),M+FVector(0,0,1950),90,true);B.Box(TEXT("Bone"),M+FVector(0,0,1930),FVector(350,350,100));B.Mesh(TEXT("SpiritLight73"),M+FVector(0,0,1980),0,FVector(.6),false);B.Label(TEXT("SOLDIERS AND SAILORS"),M+FVector(0,-420,230),-90,32);
 B.Label(TEXT("CLINTON SQUARE"),FVector(-2500,0,200),-90,50);for(int Side:{-1,1})B.Light(FVector(-2200,Side*3600,480),2000);
}
void Fair(FBuild B){
 int I=0;for(const auto& F:FairBuildings()){const int Key=I++;B.Queue([B,F,Key](){auto Q=B;Q.S.Position=F.P;Q.S.Yaw=F.Yaw;Q.S.Size=F.Size;const float X=F.Size.X*.5,Y=F.Size.Y*.5,H=F.Size.GetMax()>8000?1300:620;Q.Box(TEXT("Concrete"),FVector(0,0,-12),FVector(F.Size,24));Q.Wall(FVector(0,-Y,0),X*2,H,TEXT("BrickV7"),400);Q.Box(TEXT("BrickV7"),FVector(0,Y,H*.5),FVector(X*2,30,H));for(int Side:{-1,1})Q.Box(TEXT("BrickV7"),FVector(Side*X,0,H*.5),FVector(30,Y*2,H));for(int Side:{-1,1}){Q.Quad(TEXT("Roof73"),FVector(-X,Side*Y,H),FVector(X,Side*Y,H),FVector(X,0,H+220),FVector(-X,0,H+220));Q.Quad(TEXT("Roof73"),FVector(-X,0,H+220),FVector(X,0,H+220),FVector(X,Side*Y,H),FVector(-X,Side*Y,H));}Q.Label(F.Name,FVector(0,-Y-25,H*.6),-90,65);if(Key%4){Q.Mesh(TEXT("Rubble"),FVector(0,-Y+100,0),0,FVector(3,2,1.6));}else{for(int K=0;K<5;K++)Q.Object(TEXT("Crate"),FVector(-X+160+K*280,Y-180,0),1000+Key*10+K);Q.Object(TEXT("Counter65"),FVector(0,0,0),1000+Key*10+8);}});}
 // The fair's entrance is on State Fair Boulevard, southeast of the central grounds.
 const auto Gate=LWNY69::Project(43.07167,-76.21389);FBuild G=B;G.S.Position=Gate;G.S.Yaw=-45;for(int Side:{-1,1}){G.Box(TEXT("Bone"),FVector(Side*650,0,420),FVector(300,300,840));G.Mesh(TEXT("TicketBooth73"),FVector(Side*1100,-100,0));}G.Box(TEXT("Bone"),FVector(0,0,850),FVector(1700,260,160));G.Label(TEXT("NEW YORK STATE FAIR"),FVector(0,-145,855),-90,70);
}
void Amphitheater(FBuild B){
 B.Box(TEXT("Wood"),FVector(0,-4400,100),FVector(6000,2600,200));B.Stairs(FVector(-2800,-2800,0),FVector2D(0,-1),13,15.4,30,230);B.Box(TEXT("Rubber"),FVector(0,-5650,1100),FVector(6200,50,2000));
 for(int R=0;R<38;R++)B.Queue([B,R](){float Radius=2400+R*145,Z=R*18;int Seats=40+R;for(int I=0;I<Seats;I++){float A=.18*PI+I*.64*PI/(Seats-1);if(I%12==0)continue;B.Mesh(TEXT("ArenaSeat73"),FVector(FMath::Cos(A)*Radius,-3300+FMath::Sin(A)*Radius,Z),FMath::RadiansToDegrees(A)-90,FVector(1),false);}for(int K=0;K<24;K++){float A=.18*PI+K*.64*PI/24,D=.18*PI+(K+1)*.64*PI/24;B.Quad(TEXT("Concrete"),FVector(FMath::Cos(A)*(Radius+145),-3300+FMath::Sin(A)*(Radius+145),Z+18),FVector(FMath::Cos(D)*(Radius+145),-3300+FMath::Sin(D)*(Radius+145),Z+18),FVector(FMath::Cos(D)*Radius,-3300+FMath::Sin(D)*Radius,Z),FVector(FMath::Cos(A)*Radius,-3300+FMath::Sin(A)*Radius,Z));}});
 // Faceted fan pavilion and radiating trusses; open grass lawn and lake behind the stage.
 for(int I=0;I<16;I++){float A=.10*PI+I*.80*PI/16,D=.10*PI+(I+1)*.80*PI/16;FVector P(FMath::Cos(A)*6800,-3400+FMath::Sin(A)*6800,1900),Q(FMath::Cos(D)*6800,-3400+FMath::Sin(D)*6800,1900),Peak(0,-5000,3200);B.Tri(TEXT("Bone"),Peak,P,Q);B.Tri(TEXT("Steel"),Q-FVector(0,0,25),P-FVector(0,0,25),Peak-FVector(0,0,25));B.Beam(TEXT("Steel"),Peak-FVector(0,0,60),P-FVector(0,0,60),22);if(I%4==0)B.Beam(TEXT("Steel"),P,FVector(P.X,P.Y,0),35,true);}
 for(int Side:{-1,1}){B.Room(FVector(Side*3900,-4400,0),FVector2D(1500,1900),200+(Side>0?10:0),TEXT("BACKSTAGE"));B.Box(TEXT("Rubber"),FVector(Side*2800,-4400,1500),FVector(350,350,1200));B.Light(FVector(Side*1600,-4000,2600),4000);}
 B.Label(TEXT("EMPOWER FCU AMPHITHEATER AT LAKEVIEW"),FVector(0,6200,260),90,70);
}
}
void Build(ALWChunk* C,ALWWorld* W,const LWGen::FSite& S){
 FBuild B{C,W,S};B.Queue([B](){switch(B.S.Type){case 70:Fair(B);break;case 71:case 73:Historic(B);break;case 72:Palace(B);break;case 74:Arena(B,true);break;case 75:Square(B);break;case 76:Arena(B,false);break;case 77:Amphitheater(B);break;case 78:B.Label(TEXT("ONONDAGA LAKE"),FVector(0,0,170),-90,45);B.Object(TEXT("WaitingBench65"),FVector(0,200,0),1,TEXT("chair"));break;default:Block(B);break;}});
}
}
