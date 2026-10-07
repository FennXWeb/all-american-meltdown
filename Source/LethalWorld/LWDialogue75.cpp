#include "LWDialogue75.h"
#include "LWDialogue59.h"
#include "Engine/World.h"

const TArray<FLWChatterLine75>& LWDialogue75::Lines(){
 static const TArray<FLWChatterLine75> Data={
#include "LWDialogue75Defaults.inl"
 };return Data;
}
const FLWChatterLine75* FLWChatterHistory75::Choose(FName Group,FRandomStream& Random,const TFunction<bool(const FLWChatterLine75&)>& Eligible){
 auto& Bag=Remaining.FindOrAdd(Group);
 if(Bag.IsEmpty())for(const auto& L:LWDialogue75::Lines())if(L.Group==Group)Bag.Add(L.Id);
 TArray<const FLWChatterLine75*> Choices;
 for(const auto& L:LWDialogue75::Lines())if(L.Group==Group&&Bag.Contains(L.Id)&&L.Id!=Last.FindRef(Group)&&Eligible(L))Choices.Add(&L);
 if(Choices.IsEmpty())return nullptr; // Other speakers may have used these lines recently; wait.
 const auto* Pick=Choices[Random.RandRange(0,Choices.Num()-1)];Bag.Remove(Pick->Id);Last.Add(Group,Pick->Id);return Pick;
}
bool ULWDialogueDirector75::CanSpeak(bool Combat)const{if(Speaker.IsValid()&&Speaker->Busy())return false;const double Now=GetWorld()->GetTimeSeconds();return Now>=(Combat?CombatUntil:FMath::Max(AmbientUntil,CombatUntil));}
void ULWDialogueDirector75::Speaking(ULWDialogue59* Channel){Speaker=Channel;}
bool ULWDialogueDirector75::Recent(FName Line)const{const auto* At=LastSpoken.Find(Line);return At&&GetWorld()->GetTimeSeconds()-*At<100;}
void ULWDialogueDirector75::Reserve(FName Line,float Duration,bool Combat){
 const double Now=GetWorld()->GetTimeSeconds();if(!Line.IsNone())LastSpoken.Add(Line,Now);
 // Combat can cut across a quiet pause, but not another spoken bark.
 CombatUntil=Now+FMath::Clamp(Duration+.8f,2.5f,20.f);
 AmbientUntil=Now+FMath::Max(double(Duration+2),Combat?12.:18.);
}
