#include "LWHUD.h"
#include "LWCreator35.h"
#include "LWCharacter.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
void ALWHUD::CreatorScreen35(ALWCharacter* P){
 const float W=Canvas->ClipX/Scale;const FLinearColor Bone(.83f,.82f,.73f),Dim(.43f,.47f,.41f),Gold(.89f,.55f,.25f),Panel(.015f,.023f,.022f,.97f);auto& V=P->DraftIdentity;LWCreator35::Normalize(V);
 Rect(0,0,W,92,Panel);Rect(0,92,395,564,Panel);Rect(0,656,W,64,Panel);Text(TEXT("SURVIVOR"),24,17,1.7f,Gold);Text(V.Name,235,23,1.1f,Bone);
 const TCHAR* Tabs[]={TEXT("BODY"),TEXT("FACE"),TEXT("GROOMING"),TEXT("CLOTHING"),TEXT("TATTOOS"),TEXT("ATTRIBUTES"),TEXT("PRESETS")};float TW=FMath::Min(165.f,(W-32)/7);
 for(int I=0;I<7;I++){float X=16+I*TW;Control55(FName(*FString::Printf(TEXT("c35_tab_%d"),I)),Tabs[I],X,54,TW-4,36,P->CreatorTab35==I);}
 float MX=0,MY=0;Mouse55(MX,MY);MX/=Scale;MY/=Scale;bool Down=PlayerOwner->IsInputKeyDown(EKeys::LeftMouseButton),Pressed=Down&&!CreatorMouse35;
 auto Slider=[&](int I,FString Label,float Value,float Y){Text(Label,22,Y,.85f,Bone);Text(FString::Printf(TEXT("%03d"),FMath::RoundToInt(Value*100)),332,Y,.85f,Gold);float X=26,Width=334;Rect(X,Y+25,Width,4,Dim);Rect(X,Y+25,Width*Value,4,Gold);Rect(X+Width*Value-4,Y+19,8,16,Bone);
  if(Pressed&&MX>=X-8&&MX<X+Width+8&&MY>=Y+12&&MY<Y+43)CreatorDrag35=I;
  if(Down&&CreatorDrag35==I){float F=FMath::Clamp((MX-X)/Width,0.f,1.f);if(I<24)LWCreator35::SetShape(V,I,F);else if(I==100)V.TattooSize35[P->TattooZone35]=F;else if(I==101)V.TattooOpacity35=F;else if(I==102)V.TattooPosition35[P->TattooZone35].X=F;else if(I==103)V.TattooPosition35[P->TattooZone35].Y=F;else if(I==104)V.TattooRotation35[P->TattooZone35]=F;P->CreatorDirty35=true;P->ConsumeUIAttack();}
 };
 auto B=[&](const TCHAR* Key,FString Label,float Y){Button(FName(*(FString(TEXT("c35_"))+Key)),Label,20,Y,355);};
 switch(P->CreatorTab35){
 case 0:
  B(TEXT("body"),V.Body?TEXT("FEMALE BODY"):TEXT("MALE BODY"),108);B(TEXT("skin"),FString::Printf(TEXT("SKIN TONE  %d / 6"),V.Skin+1),155);
  for(int I=0;I<10;I++)Slider(I,LWCreator35::ShapeName(I),LWCreator35::Shape(V,I),208+I*42);
  break;
 case 1:
  B(TEXT("page"),P->CreatorPage35?TEXT("DETAILS  2 / 2"):TEXT("STRUCTURE  1 / 2"),108);
  for(int J=0;J<7;J++){int I=10+P->CreatorPage35*7+J;Slider(I,LWCreator35::ShapeName(I),LWCreator35::Shape(V,I),169+J*57);}
  B(TEXT("random_face"),TEXT("RANDOMIZE FACE"),591);break;
 case 2:
  B(TEXT("hair"),LWCreator35::HairName(V.Hair),110);B(TEXT("hair_prev"),TEXT("PREVIOUS HAIRSTYLE"),157);
  B(TEXT("haircolor"),FString(TEXT("HAIR  "))+LWCreator35::ColorName(V.HairColor),215);
  B(TEXT("beard"),LWCreator35::BeardName(V.Beard35),282);B(TEXT("beard_prev"),TEXT("PREVIOUS FACIAL HAIR"),329);
  B(TEXT("beardcolor"),FString(TEXT("BEARD  "))+LWCreator35::ColorName(V.BeardColor35),387);B(TEXT("eyecolor"),FString(TEXT("EYES  "))+LWCreator35::ColorName(V.EyeColor35),445);
  Text(TEXT("20 HAIRSTYLES / 11 GROOMING OPTIONS"),22,514,.8f,Dim);break;
 case 3:{
  static const TCHAR* Shoes[]={TEXT("WORK BOOTS"),TEXT("SNEAKERS"),TEXT("TALL BOOTS"),TEXT("DRESS SHOES")};static const TCHAR* Hats[]={TEXT("NO HEADWEAR"),TEXT("CAP"),TEXT("BEANIE"),TEXT("BACKWARD CAP"),TEXT("BRIMMED HAT"),TEXT("HELMET")};static const TCHAR* Hands[]={TEXT("BARE HANDS"),TEXT("GLOVES"),TEXT("FINGERLESS GLOVES"),TEXT("LONG GLOVES")};static const TCHAR* Eyes[]={TEXT("NO EYEWEAR"),TEXT("GLASSES"),TEXT("SUNGLASSES"),TEXT("GOGGLES")};
  B(TEXT("page"),P->CreatorPage35?TEXT("COLORS"):TEXT("CLOTHING SLOTS"),108);
  if(!P->CreatorPage35){B(TEXT("top"),LWCreator35::TopName(V.Top35),171);B(TEXT("bottom"),LWCreator35::BottomName(V.Bottom35),228);B(TEXT("shoes"),Shoes[V.Shoes35],285);B(TEXT("headwear"),Hats[V.Headwear35],342);B(TEXT("gloves"),Hands[V.Gloves35],399);B(TEXT("eyewear"),Eyes[V.Eyewear35],456);Text(TEXT("COSMETIC CLOTHING. ARMOR IS EQUIPPED\nFROM YOUR INVENTORY IN GAME."),22,526,.8f,Dim);}
  else{B(TEXT("topcolor"),FString(TEXT("TOP  "))+LWCreator35::ColorName(V.TopColor35),179);B(TEXT("bottomcolor"),FString(TEXT("BOTTOM  "))+LWCreator35::ColorName(V.BottomColor35),244);B(TEXT("shoecolor"),FString(TEXT("SHOES / GLOVES  "))+LWCreator35::ColorName(V.ShoesColor35),309);}break;}
 case 4:{
  const TCHAR* Zones[]={TEXT("FACE"),TEXT("NECK"),TEXT("CHEST"),TEXT("BACK"),TEXT("LEFT ARM"),TEXT("RIGHT ARM")};B(TEXT("zone"),Zones[P->TattooZone35],108);int Design=V.Tattoos35[P->TattooZone35];B(TEXT("tattoo"),Design?FString::Printf(TEXT("DESIGN  %d / 16"),Design):TEXT("NO TATTOO"),167);B(TEXT("tattoo_prev"),TEXT("PREVIOUS DESIGN"),214);
  if(P->CreatorPage35){Slider(100,TEXT("SIZE"),V.TattooSize35[P->TattooZone35],281);Slider(102,TEXT("HORIZONTAL POSITION"),V.TattooPosition35[P->TattooZone35].X,338);Slider(103,TEXT("VERTICAL POSITION"),V.TattooPosition35[P->TattooZone35].Y,395);Slider(104,TEXT("ROTATION"),V.TattooRotation35[P->TattooZone35],452);Slider(101,TEXT("INK STRENGTH"),V.TattooOpacity35,509);}
  else{Slider(100,TEXT("SIZE"),V.TattooSize35[P->TattooZone35],281);Slider(101,TEXT("INK STRENGTH"),V.TattooOpacity35,344);
  if(Design){auto* T=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Art/Textures/T_Tattoos35.T_Tattoos35"));if(T){Rect(126,406,128,128,Bone);DrawTexture(T,126*Scale,406*Scale,128*Scale,128*Scale,((Design-1)%4)*.25f,((Design-1)/4)*.25f,.25f,.25f,FLinearColor::White,BLEND_Translucent);}}
  B(TEXT("clear_tattoos"),TEXT("REMOVE ALL TATTOOS"),540);}
  B(TEXT("page"),P->CreatorPage35?TEXT("DESIGN PREVIEW"):TEXT("POSITION / ROTATION"),591);break;}
 case 5:{int Sum=0;for(int I:P->StartingAttributes)Sum+=I;Text(FString::Printf(TEXT("%d POINTS REMAINING"),28-Sum),24,115,1.15f,Gold);
  for(int I=0;I<7;I++){float Y=168+I*59;Text(LWRPG::Category(I),24,Y+12,.88f,Bone);Button(FName(*FString::Printf(TEXT("minus_%d"),I)),TEXT("-"),215,Y,45);Text(FString::FromInt(P->StartingAttributes[I]),277,Y+12,1.f,Gold);Button(FName(*FString::Printf(TEXT("plus_%d"),I)),TEXT("+"),321,Y,45);}Button(TEXT("reset_points"),TEXT("RESET POINTS"),20,595,355);break;}
 case 6:
  Button(TEXT("name"),TEXT("NAME: ")+V.Name+(P->NameEditing?TEXT("_"):TEXT("")),20,110,355);B(TEXT("preset"),FString::Printf(TEXT("PRESET  %d / 5"),P->CreatorPreset35),177);B(TEXT("save"),TEXT("SAVE LOOK"),236);B(TEXT("load"),TEXT("LOAD LOOK"),293);B(TEXT("random_all"),TEXT("RANDOMIZE APPEARANCE"),367);B(TEXT("reset_shape"),TEXT("RESET BODY AND FACE"),424);if(!P->CreationMessage.IsEmpty())Text(P->CreationMessage,22,564,.85f,Gold);break;
 }
 if(!Down)CreatorDrag35=-1;CreatorMouse35=Down;
 Button(TEXT("creator_back"),TEXT("BACK"),20,668,150);Button(TEXT("c35_left"),TEXT("<"),W*.5f-85,668,54);Button(TEXT("c35_zoom"),TEXT("VIEW"),W*.5f-23,668,110);Button(TEXT("c35_right"),TEXT(">"),W*.5f+95,668,54);Button(TEXT("creator_done"),TEXT("ENTER BUNKER"),W-268,668,248);
 if(!P->CreationMessage.IsEmpty()&&P->CreatorTab35!=6)Text(P->CreationMessage,414,631,.85f,Gold);
}
