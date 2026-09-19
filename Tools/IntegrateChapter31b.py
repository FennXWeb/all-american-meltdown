from pathlib import Path
r=Path('Source/LethalWorld')
def e(n,a,b):
 p=r/n;s=p.read_text();assert a in s,(n,a);p.write_text(s.replace(a,b))
e('LWCharacter.h','bool EncounterJournal=false;','bool EncounterJournal=false;bool ChapterJournal=true;')
e('LWRPGHUD.cpp','if(P->RPG.Story.Enabled){StoryJournal(P);return;}','if(P->RPG.Story.Enabled){Button(TEXT("story_journal_toggle"),P->ChapterJournal?TEXT("SIDE CONTRACTS"):TEXT("MAIN STORY"),45,130,290);if(P->ChapterJournal){StoryJournal(P);return;}}')
e('LWHUD.cpp','    if(N==TEXT("story_route"))','    if(N==TEXT("story_journal_toggle")){P->ChapterJournal=!P->ChapterJournal;return;}\n    if(N==TEXT("story_route"))')
e('LWStory.cpp','if(S!=0&&S!=29)Route();SpawnStage();','SpawnStage();if(S==6)if(auto* N=People.FindRef(TEXT("story_mara")))N->SetActorLocation(Player->GetActorLocation()-Player->GetActorForwardVector()*250);if(S!=0&&S!=29)Route();')
e('LWStory.cpp','if((S==0||S==20||S==27)&&!State().SceneFinished)Scene(S);','if((S==0||S==20||S==27)&&!State().SceneFinished)Scene(S);else if(S==20){Conversation=TEXT("20");FinishScene();if(State().Flags.Contains(TEXT("execution_ending"))){Conversation=TEXT("-2");FinishScene();}}')
e('LWStory.cpp','Failed=false;State().SceneFinished=true;','Failed=false;State().Flags.Remove(TEXT("execution_ending"));State().SceneFinished=true;')
e('LWStoryScenes.cpp','if(S==-2){Failed=true;','if(S==-2){State().Flags.Add(TEXT("execution_ending"));Failed=true;')
e('LWStoryScenes.cpp','S==1?TEXT("RadioStatic")','S==1?TEXT("CarRadio")')
e('LWStory.cpp','return I<0?ALWWorld::BunkerSpawn():At(I,FVector(0,-2300,100));','for(auto* N:Nodes)if(IsValid(N)&&N->Action==LWStory::Missions()[State().Stage].Action)return N->GetActorLocation();return I<0?ALWWorld::BunkerSpawn():At(I,FVector(0,-2300,100));')
# Prevent cinematics from accidentally initializing inside legacy automated fixtures.
e('LWCharacter.cpp','#include "LWStory.h"','#include "LWStory.h"\n#include "Misc/CommandLine.h"\n#include "Misc/Parse.h"')
e('LWCharacter.cpp','RPG.Story.Enabled=true;','RPG.Story.Enabled=!FParse::Param(FCommandLine::Get(),TEXT("LWV17Smoke"))&&!FParse::Param(FCommandLine::Get(),TEXT("LWSmoke"));')
# No trees through story rooms.
e('LWWorld.cpp','const FVector2D Local(Rand.FRandRange(0,LWGen::ChunkSize),Rand.FRandRange(0,LWGen::ChunkSize));','const FVector2D Local(Rand.FRandRange(0,LWGen::ChunkSize),Rand.FRandRange(0,LWGen::ChunkSize));if(LWStory::Reserved(FVector2D(Origin)+Local))continue;')
