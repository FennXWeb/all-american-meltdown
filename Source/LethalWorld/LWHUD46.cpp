#include "LWPlayerInput51.h"
#include "Engine/Texture2D.h"
#include "LWHUD.h"
#include "LWCharacter.h"
#include "LWWeaponMods.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"
void ALWHUD::WeaponSlots46(ALWCharacter* P){
 auto KeyName=[&](FKey Logical){auto* K=ULWPlayerInput51::Get51(this);if(K){K->Load51();for(const auto& B:K->Bindings)if(B.Logical==Logical)return B.Physical.GetDisplayName().ToString().ToUpper();}return Logical.GetDisplayName().ToString().ToUpper();};
 const FKey NumberKeys[]={EKeys::One,EKeys::Two,EKeys::Three,EKeys::Four,EKeys::Five};
 const FName Slots[]={TEXT("Primary"),TEXT("Secondary"),TEXT("Sidearm"),TEXT("Melee"),TEXT("RigPrimary")};
 const bool Rig=P->Inventory.ContainsByPredicate([](const auto& I){return I.Slot==TEXT("Rig");});const int Count=Rig?5:4;
 const float Start=294,Cell=70,Y=645;const FLinearColor Paper(.8f,.85f,.72f),Dim(.32f,.4f,.34f);
 for(int S=0;S<Count;++S){const auto* Item=P->Inventory.FindByPredicate([&](const auto& I){return I.Slot==Slots[S];});float X=Start+S*Cell;bool Active=Item&&P->ActiveWeaponId==Item->Id;FLinearColor C=Item?LWMods::TierColor(LWMods::Tier(Item)):Dim;
  Rect(X,Y,Cell-4,47,FLinearColor(.01f,.02f,.014f,.65f));Rect(X,Y+(Active?0:46),Cell-4,Active?2:1,C);Text(KeyName(NumberKeys[S]).Left(7),X+3,Y+2,.8f,Active?Paper:Dim);
  if(!Item){Line(X+27,Y+25,X+41,Y+25,Dim,2);continue;}
  const int W=LWItems::Def(Item->Definition).WeaponIndex;
  auto R=[&](float A,float B,float Width,float Height){Rect(X+7+A,Y+16+B,Width,Height,C);};
  auto L=[&](float A,float B,float D,float E,float Weight=2){Line(X+7+A,Y+16+B,X+7+D,Y+16+E,C,Weight);};
  if(W==0){L(6,19,37,0,4);L(37,0,44,0,4);L(44,0,46,5,3);L(6,19,3,18,2);}
  else if(W==1){L(7,19,39,1,5);L(30,6,43,-1,9);}
  else if(W==3||W==12||W==14||W==17){R(15,2,W==14?23:34,6);R(13,8,13,7);L(20,12,15,23,8);if(W==3){R(21,6,10,8);R(48,4,5,3);}if(W==14){R(36,10,4,4);R(8,0,6,3);}}
  else if(W==19){R(20,7,16,10);L(33,8,53,8,4);L(27,9,27,0,4);L(23,17,10,23,7);}
  else if(W==16){R(5,7,22,12);for(int I=0;I<5;++I)L(25,2+I*4,57,2+I*4,2);}
  else if(W==9){R(5,2,50,12);R(0,0,6,16);R(53,0,5,16);R(21,14,5,9);L(21,-3,32,-3);R(20,-4,2,6);}
  else if(W==10){R(19,0,35,12);for(int I=0;I<3;++I)L(23,2+I*4,58,2+I*4,1);R(9,3,12,10);R(15,13,14,11);R(4,5,6,12);}
  else if(W==15){R(8,4,28,9);R(35,5,20,4);R(9,14,7,11);R(20,12,13,13);L(25,24,43,18,2);}
  else{const bool Scope=W==4;const bool Short=W==11;R(15,5,24,7);R(38,6,Short?10:20,3);R(3,6,12,9);L(5,7,1,15,4);L(23,12,20,22,5);if(W==2||W==8||W==11){R(37,10,Short?9:18,2);R(31,9,10,5);}else {L(32,12,35,23,6);R(9,7,10,4);}if(Scope){R(23,-1,19,4);R(24,2,3,4);R(39,-2,4,6);}else{R(17,2,5,3);R(52,3,3,3);}if(W==7)R(31,12,14,12);if(W==13){R(24,0,13,3);R(25,2,2,3);}}
 }
 const float X=Start+Count*Cell;Rect(X,Y,64,47,FLinearColor(.01f,.02f,.014f,.65f));int Kits=P->CountSupply(TEXT("medkit"));Text(KeyName(EKeys::H).Left(7),X+3,Y+2,.8f,Kits?Paper:Dim);Rect(X+19,Y+21,20,6,Kits?Paper:Dim);Rect(X+26,Y+14,6,20,Kits?Paper:Dim);Text(FString::FromInt(Kits),X+43,Y+26,.8f,Kits?Paper:Dim);
}
void ALWHUD::MenuBackdrop46(){
 const float W=Canvas->ClipX/Scale,H=Canvas->ClipY/Scale,T=ReducedMotion55?0:GetWorld()->GetRealTimeSeconds();
 DrawRect(FLinearColor(.009,.015,.018),0,0,W*Scale,H*Scale);
 if(Backdrop55){float Z=1.045f+FMath::Sin(T*.04f)*.008f;float PW=W*Z,PH=H*Z;float MX=0,MY=0;Mouse55(MX,MY);float Pan=ReducedMotion55?0:FMath::Clamp(MX/Scale/W-.5f,-.5f,.5f)*9;DrawTexture(Backdrop55,((W-PW)*.5f+FMath::Sin(T*.026f)*5+Pan)*Scale,((H-PH)*.5f+FMath::Sin(T*.019f)*3)*Scale,PW*Scale,PH*Scale,0,0,1,1,FLinearColor::White,BLEND_Opaque);}
 if(!ReducedMotion55)for(int I=0;I<34;++I){float Life=FMath::Fmod(T*.04f+I*.618f,1.f);float X=FMath::Fmod(I*97.3f+T*(2+I%4),W),Y=H*(1-Life);Rect(X+FMath::Sin(T*.3f+I)*8,Y,1,1+I%2,FLinearColor(.95,.63,.31,FMath::Sin(Life*PI)*.28f));}
 for(int I=0;I<12;++I)DrawRect(FLinearColor(.005,.012,.017,.12f*(1-I/12.f)),0,(H-I*12-12)*Scale,W*Scale,12*Scale);
}
