#include "LWVoice44.h"
void ALWGameMode::BuildNPCAudio44Smoke(ALWCharacter& Initial){
 V2->Steps.Add({TEXT("NPC voice arrays"),2,120,[this](ALWCharacter& P){
  P.NewGame();P.RPG.Story.Enabled=false;P.World->SetActorTickEnabled(false);
  TArray<FName> Slots;for(int K=0;K<12;++K)if(K!=1)Slots.Add(LWVoice44::Enemy(ELWEnemyKind(K)));
  Slots.Add(LWVoice44::Human(false));Slots.Add(LWVoice44::Human(true));
  for(FName Name:Slots){
   const auto* Entry=P.World->AudioCatalog->Slots.Find(Name);
   const int Count=Name.ToString().StartsWith(TEXT("Human"))?10:5;
   if(!RequireV2(Entry&&Entry->Tracks.Num()==Count,TEXT("NPC array has required variation count")))return;
   TSet<FString> Paths;for(const auto& Track:Entry->Tracks)Paths.Add(Track.ToSoftObjectPath().ToString());
   Check(Paths.Num()==Count,TEXT("NPC array contains distinct assets"));
   TSet<FString> Seen;bool Valid=true;for(int I=0;I<32;++I){FLWResolvedAudioSlot R;Valid&=P.World->AudioCatalog->ResolveSlot(Name,R,false)&&R.Sound&&!R.Settings.bLoop;if(R.Sound)Seen.Add(R.Sound->GetPathName());}
   Check(Valid&&Seen.Num()>1,TEXT("NPC array resolves randomized non-looping voices"));
   auto* A=P.World->Sound(Name,P.GetActorLocation());Check(IsValid(A)&&A->IsPlaying(),TEXT("NPC voice plays in engine"));if(A)A->Stop();
  }
  Check(LWVoice44::Enemy(ELWEnemyKind::Raider,1)==LWVoice44::Human(true),TEXT("female human enemy uses female voice bank"));
  Check(LWVoice44::Story(TEXT("Captain Adrienne Voss"))==LWVoice44::Human(true),TEXT("story speaker uses matching voice bank"));
 },FLWV2Action(),[](ALWCharacter&){return true;}});
}
