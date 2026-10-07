#include "LWHUD.h"
#include "LWWorkbench39.h"
#include "LWWeaponMods.h"
#include "LWCharacter.h"
#include "Engine/Canvas.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/PlayerController.h"
void ALWHUD::WorkbenchScreen39(ALWCharacter* P){auto* B=P->Workbench39.Get();if(!B)return;
 Rect(0,0,1280,720,FLinearColor(.012,.02,.027));Text(TEXT("GUNSMITH"),24,18,1.7);Control55(TEXT("wb_close"),TEXT("CLOSE"),1150,16,106,32);Control55(TEXT("wb_apply"),TEXT("APPLY"),1015,16,120,32,true);
 for(int T=0;T<3;T++)Control55(FName(*FString::Printf(TEXT("wb_tab%d"),T)),T==0?TEXT("WEAPONS"):T==1?TEXT("ATTACHMENTS"):TEXT("CAMOS"),24+T*160,65,154,32,B->Tab62==T);
 Rect(292,110,670,500,FLinearColor(.025,.035,.04));if(B->Target)DrawTexture(B->Target,292*Scale,110*Scale,670*Scale,500*Scale,0,0,1,1,FLinearColor::White,BLEND_Opaque);
 Text(TEXT("RMB ORBIT   MMB PAN   WHEEL ZOOM"),310,620,.85);Control55(TEXT("wb_frame"),TEXT("FRAME"),840,614,110,28);
 if(B->Draft.Gun.Id.IsValid()){
  const auto& G=B->Draft.Gun;const int W=LWItems::Def(G.Definition).WeaponIndex;
  Text(LWItems::Def(G.Definition).DisplayName.ToString(),300,117,1.2,LWMods::TierColor(G.WeaponTier));Text(LWArsenal62::CamoName(G.Camo62,W),300,145,.9);
  Text(FString::Printf(TEXT("%d / 5 ATTACHMENTS"),G.Attachments.Num()),984,115,.95);
  const TCHAR* Mounts[]={TEXT("Optic"),TEXT("Muzzle"),TEXT("Barrel"),TEXT("Stock"),TEXT("RearGrip"),TEXT("Grip"),TEXT("Light"),TEXT("Laser")};
  for(int I=0;I<8;I++){FName M(Mounts[I]);FString Name=Mounts[I];const FName A=G.Attachments.FindRef(M);if(!A.IsNone())Name+=TEXT(" +");Control55(FName(*FString::Printf(TEXT("wb_mount%d"),I)),Name,984,153+I*34,260,30,B->Mount62==M);}
  Control55(TEXT("wb_detach"),TEXT("REMOVE SELECTED"),984,432,260,30);
  const TCHAR* Labels[]={TEXT("DAMAGE"),TEXT("CONTROL"),TEXT("ACCURACY"),TEXT("FIRE RATE")};float Values[]={LWMods::Damage(&G),1/LWMods::Recoil(&G),1/LWMods::Spread(&G,true),1/LWMods::Interval(&G)};
  for(int I=0;I<4;I++){float V=FMath::IsFinite(Values[I])?Values[I]:3;Text(Labels[I],984,488+I*34,.8);Rect(1090,491+I*34,150,8,FLinearColor(.1,.13,.15));Rect(1090,491+I*34,FMath::Clamp(V/2,0.f,1.f)*150,8,FLinearColor(.25,.78,.65));Text(FString::Printf(TEXT("x%.2f"),FMath::Min(V,99.f)),1190,503+I*34,.7);}
 }
 if(B->Tab62<2){auto Items=B->Available();B->Page=FMath::Clamp(B->Page,0,FMath::Max(0,(Items.Num()-1)/10));for(int R=0;R<10;R++){int I=B->Page*10+R;if(!Items.IsValidIndex(I))break;const auto* V=B->Draft.Bank.FindByPredicate([&](const auto& X){return X.Id==Items[I];});if(!V)continue;Control55(FName(*FString::Printf(TEXT("wb_item%d"),I)),LWItems::Def(V->Definition).DisplayName.ToString().Left(28),24,116+R*46,250,40);if(const auto* A=LWArsenal62::Find(V->Definition)){Text(A->Pros.Left(35),28,143+R*46,.65);}}if(Items.IsEmpty())Text(B->Tab62?TEXT("NO COMPATIBLE ATTACHMENTS"):TEXT("NO WEAPONS AVAILABLE"),24,132,.8);Control55(TEXT("wb_prev"),TEXT("<"),24,594,55,30);Control55(TEXT("wb_next"),TEXT(">"),214,594,55,30);
 }else{for(int C=0;C<10;C++){const bool Open=LWArsenal62::Unlocked(P,B->Draft.Gun.Definition,C);Control55(FName(*FString::Printf(TEXT("wb_camo%d"),C)),(Open?TEXT(""):TEXT("LOCKED: "))+LWArsenal62::CamoName(C,LWItems::Def(B->Draft.Gun.Definition).WeaponIndex),24,113+C*49,250,30,B->Draft.Gun.Camo62==C);Text(LWArsenal62::Challenge(P,B->Draft.Gun.Definition,C),28,145+C*49,.65);}}
 Control55(TEXT("wb_undo"),TEXT("UNDO"),24,669,100,30);Control55(TEXT("wb_redo"),TEXT("REDO"),132,669,100,30);Text(B->Message.Left(98),254,676,.82);
 float X=0,Y=0;Mouse55(X,Y);FVector2D M(X/Scale,Y/Scale),D=M-B->LastMouse;if(M.X>=292&&M.X<=962&&M.Y>=110&&M.Y<=610){if(PlayerOwner->IsInputKeyDown(EKeys::RightMouseButton)){B->Yaw-=D.X*.5;B->Pitch+=D.Y*.5;}if(PlayerOwner->IsInputKeyDown(EKeys::MiddleMouseButton))B->Center+=B->Capture->GetRelativeRotation().RotateVector(FVector(0,-D.X,D.Y))*(B->Distance/500);if(PlayerOwner->WasInputKeyJustPressed(EKeys::MouseScrollUp))B->Distance*=.9;if(PlayerOwner->WasInputKeyJustPressed(EKeys::MouseScrollDown))B->Distance*=1.1;}B->LastMouse=M;
}
void ALWHUD::WorkbenchClick39(ALWCharacter* P,FName N){auto* B=P->Workbench39.Get();if(!B)return;FString S=N.ToString().Mid(3);if(S==TEXT("close")){P->ClosePanels();return;}if(S==TEXT("apply")){B->Apply();return;}if(S.StartsWith(TEXT("tab"))){B->Tab62=FMath::Clamp(FCString::Atoi(*S.Mid(3)),0,2);B->Page=0;}if(S.StartsWith(TEXT("mount"))){const TCHAR* Mounts[]={TEXT("Optic"),TEXT("Muzzle"),TEXT("Barrel"),TEXT("Stock"),TEXT("RearGrip"),TEXT("Grip"),TEXT("Light"),TEXT("Laser")};B->Mount62=Mounts[FMath::Clamp(FCString::Atoi(*S.Mid(5)),0,7)];B->Tab62=1;B->Page=0;}if(S.StartsWith(TEXT("item"))){auto A=B->Available();int I=FCString::Atoi(*S.Mid(4));if(A.IsValidIndex(I))B->Add(A[I]);}if(S.StartsWith(TEXT("camo")))B->SetCamo62(FCString::Atoi(*S.Mid(4)));if(S==TEXT("detach"))B->Detach62();if(S==TEXT("frame"))B->FrameAll();if(S==TEXT("prev"))B->Page--;if(S==TEXT("next"))B->Page++;if(S==TEXT("undo"))B->Undo();if(S==TEXT("redo"))B->Undo(true);
}
