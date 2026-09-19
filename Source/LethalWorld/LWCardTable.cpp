#include "LWCardGame.h"
#include "LWCharacter.h"
#include "LWHUD.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "LWWorld.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "LWWorldTextComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ALWCardTable::ALWCardTable(){PrimaryActorTick.bCanEverTick=true;}
void ALWCardTable::Tick(float Dt){Super::Tick(Dt);auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));if(!P||P->CardTable!=this||P->RPGPanel!=5||P->bMenu||P->CardRules||GetWorld()->GetTimeSeconds()<P->CardBusyUntil)return;if(P->Cards.AutoTurn())P->CardAction(TEXT("tick"));else if(P->Cards.Phase==TEXT("trick"))P->CardAction(TEXT("next"));}
void ALWCardTable::Setup(ALWWorld* W,FName Id,int32 Type){
 World=W;RecordId=Id;Kind=ELWObjectKind::CardTable;Game=Type;SetActorTickEnabled(true);
 Body->SetStaticMesh(W->Mesh(TEXT("Cube")));Body->SetRelativeLocation(FVector(0,0,82));Body->SetRelativeScale3D(FVector(2.35,1.65,.14));Body->SetMaterial(0,W->Material(TEXT("Wood")));
 Part(TEXT("Cube"),FVector(0,0,90),FVector(2.12,1.42,.02),FRotator::ZeroRotator,TEXT("CardFeltV12"));
 for(int X:{-1,1})for(int Y:{-1,1})Part(TEXT("Cube"),FVector(X*94,Y*61,39),FVector(.12,.12,.78),FRotator::ZeroRotator,TEXT("Wood"));
 for(int I=0;I<5;I++){auto* Card=NewObject<UStaticMeshComponent>(this);Card->SetupAttachment(Root);Card->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Plane.Plane")));Card->SetRelativeLocation(FVector(-35+I*17,-32,92));Card->SetRelativeRotation(FRotator(0,I*4-8,0));Card->SetRelativeScale3D(FVector(.13,.173,1));Card->SetMaterial(0,W->Material(TEXT("CardBackV12")));Card->SetCollisionEnabled(ECollisionEnabled::NoCollision);Card->RegisterComponent();}
 for(int I=0;I<4;I++)Part(TEXT("Cube"),FVector(65,25,93+I*2),FVector(.12,.12,.02),FRotator::ZeroRotator,I%2?TEXT("Red"):TEXT("Bone"));
 auto* Sign=NewObject<ULWWorldTextComponent>(this);Sign->SetupAttachment(Root);Sign->SetRelativeLocation(FVector(0,-85,73));Sign->SetRelativeRotation(FRotator(0,-90,0));Sign->SetText(FText::FromString(FLWCardGame::Title(Game)));Sign->SetHorizontalAlignment(EHTA_Center);Sign->SetWorldSize(12);Sign->SetTextRenderColor(FColor(228,201,146));Sign->RegisterComponent();
}
FString ALWCardTable::Prompt()const{return TEXT("[E] PLAY ")+FLWCardGame::Title(Game);}
void ALWCardTable::Use(ALWCharacter* P){
 if(!P||!P->CanAct())return;
 if(P->Cards.Active()&&P->Cards.Game!=Game){P->Notify(TEXT("Finish or forfeit your saved ")+FLWCardGame::Title(P->Cards.Game)+TEXT(" game first."));return;}
 P->ClosePanels();P->CancelReload();P->bAim=P->bSprint=P->bTrigger=false;P->GetCharacterMovement()->StopMovementImmediately();
 P->CardTable=this;P->RPGPanel=5;P->CardPage=0;P->CardRules=false;
 if(!P->Cards.Active()&&P->Cards.Game!=Game){P->Cards=FLWCardGame();P->Cards.Game=Game;}
 P->Cards.Paced=true;P->CardBusyUntil=GetWorld()->GetTimeSeconds()+.8;P->SetMenuInput(true);if(auto* PC=Cast<APlayerController>(P->Controller))if(auto* HUD=Cast<ALWHUD>(PC->GetHUD())){HUD->CardSeenRound=P->Cards.RoundId;HUD->CardSeenSerial=P->Cards.MoveSerial;HUD->CardBatchFirst=P->Cards.MoveSerial+1;HUD->CardDrag=-1;HUD->CardMouseDown=false;}
}
void ALWCharacter::CardAction(FName Action,int32 Value){
 if(RPGPanel!=5||!IsValid(CardTable)||Health<=0||FVector::DistSquared(GetActorLocation(),CardTable->GetActorLocation())>FMath::Square(550.f)){ClosePanels();return;}
 if(Action==TEXT("rules")){CardRules=!CardRules;return;}
 if(Action==TEXT("page")){CardPage++;return;}
 if(Action==TEXT("close")){ClosePanels();RequestSave40();return;}
 if(Action!=TEXT("forfeit")&&GetWorld()->GetTimeSeconds()<CardBusyUntil)return;
 const int PreviousSerial=Cards.MoveSerial;
 if(Action==TEXT("start")){
  if(Cards.Active()||Value<10||Value>100||Value%2||Money<Value){Notify(TEXT("Not enough credits for that wager."));return;}
  FLWCardGame Next;Next.Paced=true;Next.Game=CardTable->Game;if(!Next.Start(CardTable->Game,Value,FMath::Rand()))return;Money-=Value;Cards=MoveTemp(Next);CardPage=0;
 }else{
  const int32 Extra=Cards.ExtraCost(Action);
  if(Extra>Money){Notify(TEXT("Not enough credits for that wager."));return;}
  if(!Cards.Act(Action,Value))return;Money-=Extra;
 }
 if(Cards.Phase==TEXT("done")&&!Cards.Paid){Money+=Cards.Payout;Cards.Paid=true;World->Sound(Cards.Payout>0?TEXT("CardChips"):TEXT("CardDeal"),GetActorLocation(),.6f);}else World->Sound(Action==TEXT("start")?TEXT("CardShuffle"):TEXT("CardDeal"),GetActorLocation(),.55f);
 CardBusyUntil=GetWorld()->GetTimeSeconds()+(Cards.Phase==TEXT("trick")?2.f:FMath::Max(.85f,(Action==TEXT("start")?Cards.MoveSerial:Cards.MoveSerial-PreviousSerial)*.14f+.35f));
 // Wager, deck order and payout flag travel in the same save snapshot.
 RequestSave40();
}
