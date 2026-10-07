#include "LWWorldTextComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"
#include "Engine/Font.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "CanvasItem.h"

namespace {
int Category78(const FString& S){
 const TCHAR* Words[]={TEXT("DINER"),TEXT("AUTO SERVICE"),TEXT("GENERAL STORE"),TEXT("MEDICAL"),TEXT("WAREHOUSE"),TEXT("CLOTHING"),TEXT("ARCADE"),TEXT("POLICE"),TEXT("HOTEL"),TEXT("CASINO"),TEXT("HARDWARE"),TEXT("FUEL"),TEXT("RESTROOM"),TEXT("EXIT"),TEXT("STORAGE"),TEXT("CAMPGROUND")};
 for(int I=0;I<16;I++)if(S.Contains(Words[I]))return I;
 if(S.Contains(TEXT("CLINIC")))return 3;if(S.Contains(TEXT("MOTEL")))return 8;if(S.Contains(TEXT("ATELIER")))return 5;return INDEX_NONE;
}
}
bool ULWWorldTextSubsystem::Mount78(ULWWorldTextComponent* Label){
 if(!Label||!Label->GetOwner()||!GetWorld())return false;
 const FString Words=Label->Text.ToString().TrimStartAndEnd();if(Words.IsEmpty()){Suppressed78++;return false;}
 const FVector Center=Label->CalcBounds(Label->GetComponentTransform()).Origin;
 const FVector Normal=Label->GetForwardVector(),Right=Label->GetRightVector(),Up=Label->GetUpVector();
 float Width=FMath::Clamp(float(Words.Len())*Label->WorldSize*.55f,35.f,650.f),Height=FMath::Clamp(Width*.25f,18.f,125.f);
 FCollisionQueryParams Query(SCENE_QUERY_STAT(MountedSign78),true);FHitResult Hit;
 // A physical face must exist just behind all four corners. Gaps and in-air headings are discarded.
 if(!GetWorld()->LineTraceSingleByChannel(Hit,Center+Normal*10,Center-Normal*110,ECC_Visibility,Query)||FVector::DotProduct(Hit.ImpactNormal,Normal)<.85f||FMath::Abs(Hit.ImpactNormal.Z)>.5f){Suppressed78++;return false;}
 FVector Anchor=Hit.ImpactPoint+Normal*1.4f;
 bool Fits=false;
 for(int Attempt=0;Attempt<4&&!Fits;Attempt++){
  Fits=true;for(int X:{-1,1})for(int Y:{-1,1}){FVector Corner=Anchor+Right*(X*Width*.48f)+Up*(Y*Height*.48f);FHitResult Support;
   if(!GetWorld()->LineTraceSingleByChannel(Support,Corner+Normal*8,Corner-Normal*12,ECC_Visibility,Query)||FVector::DotProduct(Support.ImpactNormal,Normal)<.9f||FMath::Abs(FVector::DotProduct(Support.ImpactPoint-Anchor,Normal))>4)Fits=false;
  }if(!Fits){Width*=.7f;Height*=.7f;}
 }
 if(!Fits){Suppressed78++;return false;}
 if(!SignMesh78)SignMesh78=LoadObject<UStaticMesh>(nullptr,TEXT("/Game/Art/World78/SM_SignPanel78.SM_SignPanel78"));
 if(!SignBase78)SignBase78=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Art/World78/M_Sign78.M_Sign78"));
 if(!SignAtlas78)SignAtlas78=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Art/World78/T_Signs78.T_Signs78"));
 if(!SignFont78)SignFont78=LoadObject<UFont>(nullptr,TEXT("/Engine/EngineFonts/RobotoDistanceField.RobotoDistanceField"));
 if(!SignMesh78||!SignBase78||!SignFont78){Suppressed78++;return false;}
 UMaterialInstanceDynamic* Material=SignCache78.FindRef(Words).Get();
 if(!Material){
  // One 512x128 texture per distinct live inscription, shared across identical signs.
  auto* Texture=UKismetRenderingLibrary::CreateRenderTarget2D(this,512,128,RTF_RGBA8,FLinearColor(.026,.065,.073,1));
  UCanvas* Canvas=nullptr;FVector2D Size;FDrawToRenderTargetContext Context;
  UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this,Texture,Canvas,Size,Context);
  if(Canvas){
   FCanvasTileItem Background(FVector2D::ZeroVector,FVector2D(512,128),FLinearColor(.026,.065,.073,1));Background.BlendMode=SE_BLEND_Opaque;Canvas->DrawItem(Background);
   Canvas->K2_DrawBox(FVector2D(5,5),FVector2D(502,118),2,FLinearColor(.64,.69,.56));
   const int Tile=Category78(Words);const float Left=Tile>=0?128:20;
   if(Tile>=0&&SignAtlas78)Canvas->K2_DrawTexture(SignAtlas78,FVector2D(8,8),FVector2D(112,112),FVector2D((Tile%4)*.25f+.008f,(Tile/4)*.25f+.008f),FVector2D(.234f),FLinearColor::White,BLEND_Opaque);
   TArray<FString> Parts;Words.Replace(TEXT(" / "),TEXT("\n")).ParseIntoArray(Parts,TEXT("\n"),true);
   if(Parts.Num()==1&&Parts[0].Len()>32){int At=Parts[0].Find(TEXT(" "),ESearchCase::CaseSensitive,ESearchDir::FromStart,Parts[0].Len()/2-6);if(At>0){FString Full=Parts[0];Parts[0]=Full.Left(At);Parts.Add(Full.Mid(At+1));}}
   const float LineH=FMath::Min(38.f,100.f/FMath::Max(1,Parts.Num()));
   for(int I=0;I<Parts.Num();I++){float TW=0,TH=0;Canvas->StrLen(SignFont78,Parts[I],TW,TH);const float Scale=FMath::Min((492-Left)/FMath::Max(1.f,TW),(LineH-4)/FMath::Max(1.f,TH));Canvas->K2_DrawText(SignFont78,Parts[I],FVector2D(Left,64-Parts.Num()*LineH*.5f+I*LineH),FVector2D(Scale),FLinearColor(.91,.87,.69),0,FLinearColor::Transparent,FVector2D::ZeroVector);}
  }
  UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this,Context);
  Material=UMaterialInstanceDynamic::Create(SignBase78,this);Material->SetTextureParameterValue(TEXT("Paint"),Texture);SignCache78.Add(Words,Material);
  for(auto It=SignCache78.CreateIterator();It;++It)if(!It.Value().IsValid())It.RemoveCurrent();
 }
 auto* Panel=NewObject<UStaticMeshComponent>(Label->GetOwner());
 Panel->SetupAttachment(Label->GetAttachParent()?Label->GetAttachParent():Label->GetOwner()->GetRootComponent());
 Panel->SetStaticMesh(SignMesh78);Panel->SetMaterial(0,Material);Panel->SetWorldLocationAndRotation(Anchor,Label->GetComponentQuat());Panel->SetWorldScale3D(FVector(1,Width/100,Height/25));Panel->SetCollisionEnabled(ECollisionEnabled::NoCollision);Panel->SetCanEverAffectNavigation(false);Panel->SetCullDistance(14000);Panel->ComponentTags.Add(TEXT("MountedSign78"));Panel->RegisterComponent();Label->GetOwner()->AddInstanceComponent(Panel);
 Mounted78++;return true;
}
