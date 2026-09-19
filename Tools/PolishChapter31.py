from pathlib import Path
import re
r=Path('Source/LethalWorld')
for n in ['LWStory.cpp','LWStoryScenes.cpp']:
 p=r/n;s=p.read_text();s=re.sub(r'for\(auto\* (\w+):',r'for(auto& \1:',s);s=re.sub(r'auto\* (\w+)=People.FindRef\((.*?)\)',r'auto* \1=People.FindRef(\2).Get()',s);p.write_text(s)
def e(n,a,b):
 p=r/n;s=p.read_text();assert a in s,(n,a);p.write_text(s.replace(a,b))
e('LWStory.h','void BuildSite(int32 Index);void ClearActors();','void BuildSite(int32 Index);void ClearActors(bool All=true);\n UPROPERTY() TObjectPtr<class UStaticMeshComponent> Transport;\n UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> Fire;\n void AnimateScene(float Dt);\n void ClearActorsLegacy();')
# remove unused declaration
e('LWStory.h',' void ClearActorsLegacy();','')
e('LWStory.cpp','void ALWStoryDirector::ClearActors(){','void ALWStoryDirector::ClearActors(bool All){Transport=nullptr;Fire.Empty();')
e('LWStory.cpp','Nodes.Empty();for(auto& S:Sites)if(IsValid(S.Value))S.Value->Destroy();Sites.Empty();','Nodes.Empty();if(All){for(auto& S:Sites)if(IsValid(S.Value))S.Value->Destroy();Sites.Empty();}')
e('LWStory.cpp','Applied=S;ClearActors();','Applied=S;ClearActors(false);')
e('LWStory.cpp','if(I>=0)BuildSite(I);','if(I>=0){if(auto* Old=Sites.FindRef(I).Get()){Old->Destroy();Sites.Remove(I);}BuildSite(I);}')
e('LWStory.cpp','if(InScene){Player->bTrigger','if(InScene){AnimateScene(Dt);Player->bTrigger')
# Ensure movement inputs are delivered every frame, not at the 4Hz objective polling rate.
a='if(Locked())return;Clock+=Dt;if(Clock<.25f)return;float Step=Clock;Clock=0;const int S=State().Stage;const int Area=LWStory::Missions()[S].Site;'
b='if(Locked())return;const int S=State().Stage;const int Area=LWStory::Missions()[S].Site;float Step=Dt;'
e('LWStory.cpp',a,b)
e('LWStory.cpp',' if(S==2&&!ALWWorld::IsSafePosition',' Clock+=Dt;if(Clock<.25f)return;Clock=0;for(int I=0;I<9;I++){float Distance=FVector::Dist2D(Player->GetActorLocation(),At(I));if(Distance<18000)BuildSite(I);else if(I!=Area&&Distance>26000){if(auto* C=Sites.FindRef(I).Get())C->Destroy();Sites.Remove(I);}}\n if(S==2&&!ALWWorld::IsSafePosition')
e('LWStory.cpp','if(S==29){auto* J','if(S==26&&State().BossWave>0)for(int K=20;K<24;K++)Enemy(K,At(8,FVector((K-21.5f)*350,2100,110)),true);\n if(S==7||S==8||S>=24){Person(TEXT("story_elsie"),TEXT("Elsie Park"),At(I,FVector(-650,-2700,100)),2);Person(TEXT("story_tomas"),TEXT("Tomas Bell"),At(I,FVector(650,-2700,100)),0);if(S>=24)Person(TEXT("story_inez"),TEXT("Inez Soto"),At(I,FVector(900,-2450,100)),2);}\n if(S==29){if(auto* Mara=People.FindRef(TEXT("story_mara")).Get()){Mara->NpcRole=TEXT("recruit");Mara->SettlementId=TEXT("fort_resolute");}auto* J')
e('LWStory.cpp','void ALWStoryPerson::Tick(float Dt){ACharacter::Tick(Dt);','void ALWStoryPerson::Tick(float Dt){if(NpcRole!=TEXT("story")){Super::Tick(Dt);return;}ACharacter::Tick(Dt);if(GetVelocity().Size2D()>20&&Parts.Num()>=7){float Swing=FMath::Sin(GetWorld()->GetTimeSeconds()*9)*22;Parts[5]->SetRelativeRotation(FRotator(Swing,0,0));Parts[6]->SetRelativeRotation(FRotator(-Swing,0,0));}')
e('LWRPGHUD.cpp','if(P->RPG.Story.Enabled){Button(TEXT("story_journal_toggle")','if(!P->RPG.Story.Enabled)Button(TEXT("story_start"),TEXT("BEGIN CHAPTER 1"),45,130,290);\n if(P->RPG.Story.Enabled){Button(TEXT("story_journal_toggle")')
e('LWHUD.cpp','    if(N==TEXT("story_journal_toggle"))','    if(N==TEXT("story_start")){ALWStoryDirector::Ensure(P)->Start(true);return;}\n    if(N==TEXT("story_journal_toggle"))')
# Restrict scenic ambient spawns too.
p=r/'LWSpawnTable.cpp';s=p.read_text();idx=s.find('void ALWWorld::SpawnEnemies');print(s[idx:idx+200])
