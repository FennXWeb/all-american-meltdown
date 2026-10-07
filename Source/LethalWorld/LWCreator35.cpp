#include "LWCreator35.h"
#include "LWHair61.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"
#include "Materials/Material.h"
#include "KismetProceduralMeshLibrary.h"
#include "Misc/Crc.h"
namespace LWCreator35 {
void Normalize(FLWIdentity& V){
 if(V.StyleVersion35==0){V.Hair=FMath::Clamp(V.Hair+1,0,19);const int LegacyColors[]={2,1,7,4};V.TopColor35=LegacyColors[FMath::Clamp(V.Outfit,0,3)];V.StyleVersion35=1;}
 V.Body=FMath::Clamp(V.Body,0,1);V.Hair=FMath::Clamp(V.Hair,0,19);V.Skin=FMath::Clamp(V.Skin,0,5);V.HairColor=FMath::Clamp(V.HairColor,0,11);
 V.Top35=FMath::Clamp(V.Top35,0,8);V.Bottom35=FMath::Clamp(V.Bottom35,0,5);V.Shoes35=FMath::Clamp(V.Shoes35,0,3);V.Headwear35=FMath::Clamp(V.Headwear35,0,5);V.Gloves35=FMath::Clamp(V.Gloves35,0,3);V.Beard35=FMath::Clamp(V.Beard35,0,10);V.Eyewear35=FMath::Clamp(V.Eyewear35,0,3);
 V.Height=FMath::Clamp(V.Height,0.f,1.f);V.Weight=FMath::Clamp(V.Weight,0.f,1.f);V.Shoulders=FMath::Clamp(V.Shoulders,0.f,1.f);
 while(V.Shapes35.Num()<ShapeCount)V.Shapes35.Add(.5f);V.Shapes35.SetNum(ShapeCount);for(auto& F:V.Shapes35)F=FMath::IsFinite(F)?FMath::Clamp(F,0.f,1.f):.5f;
 while(V.Tattoos35.Num()<6)V.Tattoos35.Add(0);V.Tattoos35.SetNum(6);for(auto& I:V.Tattoos35)I=FMath::Clamp(I,0,16);
 while(V.TattooSize35.Num()<6)V.TattooSize35.Add(.5f);V.TattooSize35.SetNum(6);for(auto& F:V.TattooSize35)F=FMath::Clamp(F,0.f,1.f);
 while(V.TattooPosition35.Num()<6)V.TattooPosition35.Add(FVector2D(.5,.5));V.TattooPosition35.SetNum(6);for(auto& P:V.TattooPosition35){P.X=FMath::IsFinite(P.X)?FMath::Clamp(P.X,0.,1.):.5;P.Y=FMath::IsFinite(P.Y)?FMath::Clamp(P.Y,0.,1.):.5;}
 while(V.TattooRotation35.Num()<6)V.TattooRotation35.Add(.5f);V.TattooRotation35.SetNum(6);for(auto& R:V.TattooRotation35)R=FMath::IsFinite(R)?FMath::Clamp(R,0.f,1.f):.5f;
 V.TattooOpacity35=FMath::Clamp(V.TattooOpacity35,0.f,1.f);
}
float Shape(const FLWIdentity& V,int I){if(I==0)return V.Height;if(I==1)return V.Weight;if(I==2)return V.Shoulders;return V.Shapes35.IsValidIndex(I)?V.Shapes35[I]:.5f;}
void SetShape(FLWIdentity& V,int I,float F){Normalize(V);F=FMath::Clamp(F,0.f,1.f);if(I==0)V.Height=F;else if(I==1)V.Weight=F;else if(I==2)V.Shoulders=F;else if(V.Shapes35.IsValidIndex(I))V.Shapes35[I]=F;}
const TCHAR* ShapeName(int I){static const TCHAR* N[]={TEXT("HEIGHT"),TEXT("WEIGHT"),TEXT("SHOULDERS"),TEXT("MUSCLE"),TEXT("CHEST"),TEXT("WAIST"),TEXT("HIPS"),TEXT("UPPER ARMS"),TEXT("THIGHS"),TEXT("NECK"),TEXT("HEAD WIDTH"),TEXT("HEAD HEIGHT"),TEXT("JAW WIDTH"),TEXT("CHIN LENGTH"),TEXT("CHEEKBONES"),TEXT("NOSE WIDTH"),TEXT("NOSE LENGTH"),TEXT("EYE SPACING"),TEXT("EYE SIZE"),TEXT("BROW HEIGHT"),TEXT("MOUTH WIDTH"),TEXT("LIP FULLNESS"),TEXT("EAR SIZE"),TEXT("FACE DEPTH")};return N[FMath::Clamp(I,0,23)];}
const TCHAR* HairName(int I){static const TCHAR* N[]={TEXT("BALD"),TEXT("BUZZ"),TEXT("CROP"),TEXT("SIDE PART"),TEXT("SLICK BACK"),TEXT("SHOULDER LENGTH"),TEXT("BOB"),TEXT("AFRO"),TEXT("MOHAWK"),TEXT("PONYTAIL"),TEXT("BUN"),TEXT("LONG STRAIGHT"),TEXT("WAVY BOB"),TEXT("POMPADOUR"),TEXT("CREW CUT"),TEXT("BRAIDED TAIL"),TEXT("LOW BUN"),TEXT("SHORT CURLS"),TEXT("LAYERED LONG"),TEXT("LONG WAVES")};return N[FMath::Clamp(I,0,19)];}
const TCHAR* BeardName(int I){static const TCHAR* N[]={TEXT("CLEAN SHAVEN"),TEXT("MOUSTACHE"),TEXT("GOATEE"),TEXT("HORSESHOE"),TEXT("CHIN STRAP"),TEXT("CIRCLE BEARD"),TEXT("SHORT BOXED"),TEXT("VAN DYKE"),TEXT("FULL BEARD"),TEXT("LONG BEARD"),TEXT("BROAD BEARD")};return N[FMath::Clamp(I,0,10)];}
const TCHAR* TopName(int I){static const TCHAR* N[]={TEXT("WORK SHIRT"),TEXT("TANK TOP"),TEXT("T-SHIRT"),TEXT("LEATHER JACKET"),TEXT("HOODIE"),TEXT("FIELD COAT"),TEXT("UTILITY VEST"),TEXT("OVERSHIRT"),TEXT("UNDERLAYER")};return N[FMath::Clamp(I,0,8)];}
const TCHAR* BottomName(int I){static const TCHAR* N[]={TEXT("JEANS"),TEXT("CARGO TROUSERS"),TEXT("SLACKS"),TEXT("SHORTS"),TEXT("JOGGERS"),TEXT("FITTED TROUSERS")};return N[FMath::Clamp(I,0,5)];}
const TCHAR* ColorName(int I){static const TCHAR* N[]={TEXT("BLACK"),TEXT("NAVY"),TEXT("OLIVE"),TEXT("BROWN"),TEXT("SAND"),TEXT("GREY"),TEXT("CREAM"),TEXT("RED"),TEXT("BURGUNDY"),TEXT("TEAL"),TEXT("BLONDE"),TEXT("WHITE")};return N[FMath::Clamp(I,0,11)];}
FLinearColor Color(int I){static const FLinearColor C[]={FLinearColor(.045f,.038f,.032f),FLinearColor(.12f,.19f,.3f),FLinearColor(.28f,.33f,.18f),FLinearColor(.25f,.12f,.055f),FLinearColor(.58f,.46f,.28f),FLinearColor(.32f,.34f,.35f),FLinearColor(.77f,.72f,.60f),FLinearColor(.55f,.08f,.04f),FLinearColor(.26f,.045f,.075f),FLinearColor(.06f,.31f,.3f),FLinearColor(.72f,.48f,.19f),FLinearColor(.85f,.84f,.8f)};return C[FMath::Clamp(I,0,11)];}
FVector Morph(FVector P,int Part,const FLWIdentity& V){
 auto S=[&](int I){return (Shape(V,I)-.5f)*2;};auto G=[](double X,double C,double W){return FMath::Exp(-FMath::Square((X-C)/W));};
 if(Part==1){
  const double Front=FMath::SmoothStep(1.f,6.f,float(P.X));double Z=P.Z,Y=P.Y;
  P.Y*=1+.14*S(10);P.Z*=1+.1*S(11);P.X*=1+.12*S(23);
  P.Y*=1+.22*S(12)*G(Z,-12,5);P.Z-=S(13)*1.4*G(Z,-15,3);
  P.Y+=FMath::Sign(Y)*S(14)*.9*G(Z,-7,3)*G(FMath::Abs(Y),5,3);
  P.Y*=1+.35*S(15)*G(Z,-6,3)*G(Y,0,2);P.X+=S(16)*1.3*G(Z,-6,3)*G(Y,0,2)*Front;
  P.Y+=FMath::Sign(Y)*S(17)*.8*G(Z,-3.5,1.6)*G(FMath::Abs(Y),3.2,2.2);
  double Eye=G(Z,-3.5,1.1)*G(FMath::Abs(Y),3.2,1.8)*Front;P.Z+=(Z+3.5)*S(18)*.3*Eye;P.Z+=S(19)*.7*G(Z,-1.8,1.2)*Front;
  double Mouth=G(Z,-10.9,1.8)*Front;P.Y*=1+.25*S(20)*Mouth;P.Z+=(Z+10.9)*S(21)*.4*Mouth;
  double Ear=G(FMath::Abs(Y),8.8,1.1);P.Y+=FMath::Sign(Y)*S(22)*.8*Ear;P.Z+=(Z+6.5)*S(22)*.18*Ear;
  if(Z<-17){P.X*=1+.16*S(9);P.Y*=1+.16*S(9);}return P;
 }
 double W=1+.18*S(1);P.X*=W;P.Y*=W;
 if(Part==0){P.X*=1+.15*S(4)*G(P.Z,-17,10)+.20*S(5)*G(P.Z,-39,10);P.Y*=1+.14*S(2)*G(P.Z,-9,13)+.20*S(5)*G(P.Z,-39,10);P.X+=S(3)*.8*G(P.Z,-18,10);}
 if(Part==2){P.Y*=1+.19*S(6);P.X*=1+.10*S(6);}
 if(Part==3||Part==4){double Bulge=1+(.17*S(7)+.09*S(3))*G(P.Z,-14,12);P.X*=Bulge;P.Y*=Bulge;}
 if(Part==5||Part==6){double Bulge=1+(.18*S(8)+.06*S(3))*G(P.Z,-14,18);P.X*=Bulge;P.Y*=Bulge;}
 return P;
}
void Surface(FProcMeshSection& Section,int Part,const FLWIdentity& V){
 for(auto& A:Section.ProcVertexBuffer){FVector P=A.Position;A.Color=FColor::White;A.Color.G=0;A.Color.B=0;
  if(Part==1){auto Smooth=[](double A,double B,double X){const double T=FMath::Clamp((X-A)/(B-A),0.,1.);return T*T*(3-2*T);};A.Color.R=uint8(255*Smooth(.5,5.5,P.X)*(1-Smooth(5.2,7.8,FMath::Abs(P.Y)))*Smooth(-18.5,-14.5,P.Z));}
  if((Part==3||Part==4)&&V.Gloves35){float Cut=V.Gloves35==2?-58.f:-70.f;if(P.Z<(V.Gloves35==3?-43:-49)&&P.Z>Cut)A.Color.G=255;}
  int Zone=Part==1?(P.Z<-17?1:0):Part==0?(P.X>0?2:3):Part==3?4:Part==4?5:-1;
  if(V.Tattoos35.IsValidIndex(Zone)&&V.Tattoos35[Zone]>0){
   float Size=FMath::Lerp(.55f,1.6f,V.TattooSize35.IsValidIndex(Zone)?V.TattooSize35[Zone]:.5f);float U=.5f+P.Y/(Part==1?10.f:Part==0?28.f:10.f)/Size;float T=.5f-(P.Z+(Part==1?(Zone==1?20:9):Part==0?23:37))/(Part==1?9.f:Part==0?24.f:18.f)/Size;
   FVector2D Center=V.TattooPosition35.IsValidIndex(Zone)?V.TattooPosition35[Zone]:FVector2D(.5,.5);float Angle=((V.TattooRotation35.IsValidIndex(Zone)?V.TattooRotation35[Zone]:.5f)-.5f)*2*PI;float X=U-.5f-(Center.X-.5f)*1.5f,Y=T-.5f-(Center.Y-.5f)*1.5f;U=.5f+FMath::Cos(Angle)*X+FMath::Sin(Angle)*Y;T=.5f-FMath::Sin(Angle)*X+FMath::Cos(Angle)*Y;
   int Tile=V.Tattoos35[Zone]-1;A.UV1=FVector2D((Tile%4+FMath::Clamp(U,.002f,.998f))*.25f,(Tile/4+FMath::Clamp(T,.002f,.998f))*.25f);
   if(U>=0&&U<=1&&T>=0&&T<=1&&((Zone==3&&P.X<0)||(Zone!=3&&P.X>1)))A.Color.B=255;
  }
  A.Position=Morph(P,Part,V);
 }
 Section.SectionLocalBox=FBox(ForceInit);for(const auto& A:Section.ProcVertexBuffer)Section.SectionLocalBox+=A.Position;
}
void Materials(UMeshComponent* C,int Part,const FLWIdentity& V,bool Dead){
 for(int I=0;I<C->GetNumMaterials();I++){auto* Base=C->GetMaterial(I);if(!Base)continue;FString N=Base->GetMaterial()->GetName();auto* M=Cast<UMaterialInstanceDynamic>(Base);if(!M)M=UMaterialInstanceDynamic::Create(Base,C);
  FLinearColor Tint=(N.Contains(TEXT("Skin"))||N.Contains(TEXT("Face")))?LWOpening::Skin(V.Skin)*1.5f:N.Contains(TEXT("Hair"))?Color(V.HairColor):N.Contains(TEXT("Iris"))?Color(V.EyeColor35):Part==0||Part==3||Part==4?Color(V.TopColor35):Part==2||Part==5||Part==6?Color(V.BottomColor35):Color(V.ShoesColor35);
  if((N.Contains(TEXT("Eye35"))||N.Contains(TEXT("Eye61")))||N.Contains(TEXT("Pupil"))||N.Contains(TEXT("Lip"))||N.Contains(TEXT("Metal")))Tint=FLinearColor::White;
  if(N.Contains(TEXT("Iris"))){static const FLinearColor Eye[]={FLinearColor(1,.85,.65),FLinearColor(.35,.85,1.3),FLinearColor(.65,1.1,.45),FLinearColor(1,.75,.5),FLinearColor(1.1,1,.6),FLinearColor(.7,.85,.9),FLinearColor(.8,.8,.7),FLinearColor(.65,.8,.55),FLinearColor(.6,.5,.4),FLinearColor(.45,.85,.75),FLinearColor(1.1,.9,.45),FLinearColor(.85,.9,1)};Tint=Eye[FMath::Clamp(V.EyeColor35,0,11)];}
  if(N.Contains(TEXT("BootLeather61"))||N.Contains(TEXT("Rubber61")))Tint=Color(V.ShoesColor35);
  if(Dead&&(N.Contains(TEXT("Skin"))||N.Contains(TEXT("Face"))))Tint*=FLinearColor(.55f,.75f,.45f);
  M->SetVectorParameterValue(TEXT("Tint"),Tint);M->SetScalarParameterValue(TEXT("GloveAmount"),V.Gloves35?1:0);M->SetVectorParameterValue(TEXT("GloveTint"),Color(V.ShoesColor35));M->SetScalarParameterValue(TEXT("TattooOpacity"),V.TattooOpacity35);C->SetMaterial(I,M);
 }
}
void PreviewPart(UStaticMeshComponent* Part,int Index,const FLWIdentity& V,bool Visible){
 if(!Part||!Part->GetStaticMesh()||!Part->GetStaticMesh()->bAllowCPUAccess)return;
 if(ULWHair61::Supports(Part)){auto* Hair=NewObject<ULWHair61>(Part->GetOwner());Hair->ComponentTags.Add(TEXT("CreatorMesh35"));Hair->SetupAttachment(Part);Hair->RegisterComponent();Hair->Initialize(Part,V,Visible,true);return;}
 auto* Mesh=NewObject<UProceduralMeshComponent>(Part->GetOwner());Mesh->ComponentTags.Add(TEXT("CreatorMesh35"));Mesh->SetupAttachment(Part);Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);Mesh->SetCanEverAffectNavigation(false);Mesh->RegisterComponent();UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(Part,0,Mesh,false);
 for(int I=0;I<Mesh->GetNumSections();I++){Mesh->SetMaterial(I,Part->GetMaterial(Part->GetStaticMesh()->GetRenderData()->LODResources[0].Sections[I].MaterialIndex));auto Section=*Mesh->GetProcMeshSection(I);Surface(Section,Index,V);Mesh->SetProcMeshSection(I,Section);}
 Mesh->SetVisibility(Visible);Part->SetVisibility(false,false);
}
void Cleanup(AActor* Owner){for(auto* C:Owner->GetComponentsByTag(UProceduralMeshComponent::StaticClass(),TEXT("CreatorMesh35")))C->DestroyComponent();}
FLWIdentity ResidentIdentity(FName Id,bool Female,bool Raider,bool Undead){FLWIdentity V;V.StyleVersion35=1;V.Body=Female;FRandomStream R(FCrc::StrCrc32(*Id.ToString()));Normalize(V);V.Skin=R.RandRange(0,5);V.Hair=R.RandRange(0,19);V.HairColor=R.RandRange(0,5);V.Beard35=Female?0:R.RandRange(0,8);V.Top35=Raider?6:R.RandRange(0,7);V.Bottom35=R.RandRange(0,5);V.TopColor35=R.RandRange(0,9);V.BottomColor35=R.RandRange(0,5);for(int I=3;I<ShapeCount;I++)V.Shapes35[I]=R.FRandRange(.22f,.78f);if(Undead){V.TopColor35=2;V.BottomColor35=3;}return V;}
}
