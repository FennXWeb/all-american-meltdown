#include "LWSlotMachine.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "LWWorldTextComponent.h"
#include "Misc/Crc.h"
void ALWSlotMachine::Setup(ALWWorld* W,FName Id){World=W;RecordId=Id;Kind=ELWObjectKind::Furniture;Body->SetStaticMesh(W->Mesh(TEXT("SlotMachineV21")));Body->SetCollisionProfileName(TEXT("BlockAll"));for(int I=0;I<3;I++){auto* T=NewObject<ULWWorldTextComponent>(this);T->SetupAttachment(Root);T->SetRelativeLocation(FVector(-18+I*18,-38,141));T->SetRelativeRotation(FRotator(0,-90,0));T->SetHorizontalAlignment(EHTA_Center);T->SetWorldSize(14);T->SetTextRenderColor(FColor(180,30,35));T->SetText(FText::FromString(TEXT("7")));T->RegisterComponent();Reels.Add(T);}SetActorTickEnabled(false);}
FString ALWSlotMachine::Prompt()const{return SpinTime>0?TEXT("SPINNING..."):TEXT("[E] SPIN / 10 CREDITS");}
void ALWSlotMachine::Use(ALWCharacter* P){if(!P||!P->CanAct()||SpinTime>0||FVector::DistSquared(P->GetActorLocation(),GetActorLocation())>FMath::Square(350.f))return;if(P->Money<10){P->Notify(TEXT("You need 10 credits."));return;}
 int& Serial=World->PropStates.FindOrAdd(RecordId);Serial=Serial==MAX_int32?0:Serial+1;FRandomStream R(LWGen::Hash(Serial,int(FCrc::StrCrc32(*RecordId.ToString())),World->Seed,21021));for(int& V:Result){int Roll=R.RandRange(0,14);V=Roll<5?0:Roll<9?1:Roll<12?2:Roll<14?3:4;}Award=Payout(Result[0],Result[1],Result[2]);P->Money=P->Money-10+Award;SpinTime=2.1f;SetActorTickEnabled(true);P->RequestSave40();World->Sound(TEXT("CardShuffle"),GetActorLocation(),.6f);P->Notify(Award?FString::Printf(TEXT("Paid %d credits"),Award):TEXT("No win"));}
void ALWSlotMachine::Tick(float Dt){if(SpinTime<=0)return;SpinTime=FMath::Max(0.f,SpinTime-Dt);static const TCHAR* Symbols[]={TEXT("CH"),TEXT("BAR"),TEXT("$"),TEXT("7"),TEXT("A")};for(int I=0;I<3;I++)if(Reels.IsValidIndex(I))Reels[I]->SetText(FText::FromString(Symbols[SpinTime>(2-I)*.4f?int(SpinTime*19+I*2)%5:Result[I]]));if(SpinTime==0){SetActorTickEnabled(false);World->Sound(Award?TEXT("CardChips"):TEXT("CardDeal"),GetActorLocation(),.65f);}}
