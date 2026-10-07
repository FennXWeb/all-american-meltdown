#include "LWHUD.h"
#include "LWCharacter.h"
#include "LWSaveSlots62.h"
#include "Engine/Canvas.h"
#include "Kismet/GameplayStatics.h"
void ALWHUD::SaveScreen62(ALWCharacter* P){
 Rect(0,0,1280,720,FLinearColor(.015,.025,.03));Text(P->SavePanel62==1?TEXT("SAVE GAME"):TEXT("LOAD GAME"),50,35,2);Control55(TEXT("save62_back"),TEXT("BACK"),1090,35,140,34);
 P->SavePage62=FMath::Clamp(P->SavePage62,0,2);for(int R=0;R<8;R++){int I=P->SavePage62*8+R;if(!P->SaveEntries62.IsValidIndex(I))break;const auto& E=P->SaveEntries62[I];const bool Allowed=P->SavePanel62==1?I>0:E.Exists;Control55(FName(*FString::Printf(TEXT("save62_slot%d"),I)),E.Title+(Allowed?TEXT(""):TEXT(" - UNAVAILABLE")),50,106+R*60,1130,34);Text(E.Detail.Left(115),64,143+R*60,.78);}
 Control55(TEXT("save62_prev"),TEXT("PREVIOUS"),50,627,160,34);Control55(TEXT("save62_next"),TEXT("NEXT"),220,627,160,34);Text(P->SaveMessage62,420,636,.85);
 if(!P->SaveConfirm62.IsEmpty()){Rect(320,240,650,220,FLinearColor(.025,.035,.05));Text(P->SavePanel62==1?TEXT("OVERWRITE THIS SAVE?"):TEXT("LOAD THIS SAVE?"),355,275,1.3);Text(P->SavePanel62==1?TEXT("The selected manual save will be replaced."):TEXT("Unsaved progress will be lost."),355,320,.95);Control55(TEXT("save62_confirm"),TEXT("CONFIRM"),355,384,250,40);Control55(TEXT("save62_cancel"),TEXT("CANCEL"),620,384,250,40);}
}
void ALWHUD::SaveClick62(ALWCharacter* P,FName N){FString S=N.ToString();if(!P->SaveConfirm62.IsEmpty()){if(S==TEXT("save62_cancel"))P->SaveConfirm62.Empty();else if(S==TEXT("save62_confirm")){FString Slot=P->SaveConfirm62;P->SaveConfirm62.Empty();if(P->SavePanel62==1)P->ManualSave62(Slot);else P->LoadSlot62(Slot);}return;}if(S==TEXT("save62_back")){P->SavePanel62=0;return;}if(S==TEXT("save62_prev"))P->SavePage62--;if(S==TEXT("save62_next"))P->SavePage62++;if(S.StartsWith(TEXT("save62_slot"))){int I=FCString::Atoi(*S.Mid(11));if(!P->SaveEntries62.IsValidIndex(I))return;const auto E=P->SaveEntries62[I];if(P->SavePanel62==1){if(I==0)return;if(E.Exists)P->SaveConfirm62=E.Slot;else P->ManualSave62(E.Slot);}else if(E.Exists)P->SaveConfirm62=E.Slot;}}
