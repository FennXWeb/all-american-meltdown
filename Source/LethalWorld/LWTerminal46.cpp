#include "LWBunker45.h"
#include "LWCharacter.h"
#include "LWHUD.h"
#include "GameFramework/PlayerController.h"
void ALWBunker45::TerminalButton46(ALWHUD& H,FName Id,const FString& Label,float X,float Y,float W){
 Entries46.Add({Id,FVector2D(X+W*.5f,Y+20)});if(Focus46.IsNone())Focus46=Id;
 H.Control55(Id,Label.ToUpper(),X,Y,W,41,Id==Focus46);

}
void ALWBunker45::Navigate46(FVector2D Direction){
 if(Entries46.IsEmpty())return;int Current=Entries46.IndexOfByPredicate([&](const auto& E){return E.Id==Focus46;});if(Current<0)Current=0;int Pick=INDEX_NONE;double Best=DBL_MAX;
 for(int I=0;I<Entries46.Num();++I){FVector2D D=Entries46[I].Center-Entries46[Current].Center;double Forward=FVector2D::DotProduct(D,Direction);if(Forward<1)continue;double Side=FMath::Abs(D.X*Direction.Y-D.Y*Direction.X);double Score=Forward+Side*3;if(Score<Best){Best=Score;Pick=I;}}
 if(Pick==INDEX_NONE)Pick=(Current+(Direction.X+Direction.Y>0?1:Entries46.Num()-1))%Entries46.Num();Focus46=Entries46[Pick].Id;
}
void ALWBunker45::TickTerminal46(){
 auto* PC=Cast<APlayerController>(Player->Controller);if(!PC)return;
 if(PC->WasInputKeyJustPressed(EKeys::Up))Navigate46(FVector2D(0,-1));if(PC->WasInputKeyJustPressed(EKeys::Down))Navigate46(FVector2D(0,1));
 if(PC->WasInputKeyJustPressed(EKeys::Left))Navigate46(FVector2D(-1,0));if(PC->WasInputKeyJustPressed(EKeys::Right))Navigate46(FVector2D(1,0));
 if(PC->WasInputKeyJustPressed(EKeys::Enter)&&Entries46.ContainsByPredicate([&](const auto& E){return E.Id==Focus46;})){const FName ActionId=Focus46;Player->ConsumeUIAttack();Action(ActionId);}
}
