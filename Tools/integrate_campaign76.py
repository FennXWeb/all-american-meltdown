"""One-time, exact-match integration edits. Not used by the game."""
from pathlib import Path
R=Path(__file__).resolve().parents[1]/'Source/LethalWorld'
def edit(file,old,new):
 p=R/file;s=p.read_text(encoding='utf-8');assert old in s,(file,old[:90]);p.write_text(s.replace(old,new,1),encoding='utf-8')
def include(file):
 p=R/file;s=p.read_text(encoding='utf-8');p.write_text('#include "LWCampaign76.h"\n'+s,encoding='utf-8')
for file in ['LWCharacter.cpp','LWCreation.cpp','LWSurvival.cpp','LWPlayerRPG.cpp','LWHUD.cpp','LWStoryHUD.cpp','LWRPGHUD.cpp','LWPOI.cpp','LWDestiny71.cpp','LWSpawnTable.cpp','LWMissionRecovery37.cpp'] :include(file)
edit('LWResident.h','FString Name,int32 VoiceIndex);','FString Name,int32 VoiceIndex,int32 BodyOverride=-1);')
edit('LWResident.cpp','FString Name,int32 V){','FString Name,int32 V,int32 BodyOverride){')
edit('LWResident.cpp','const bool Female=Id==','const bool Female=BodyOverride>=0?BodyOverride==1:Id==')
edit('LWCharacter.cpp','if(Story){Story->Destroy();Story=nullptr;}bStoryLocked=false;','if(Campaign76){Campaign76->Destroy();Campaign76=nullptr;}if(Story){Story->Destroy();Story=nullptr;}bStoryLocked=false;')
edit('LWCharacter.cpp','void ALWCharacter::ClosePanels(){','void ALWCharacter::ClosePanels(){if(Campaign76&&Campaign76->SceneOpen){Campaign76->Pause();RPG.Campaign76.Paused=true;}')
edit('LWCharacter.cpp','void ALWCharacter::ToggleMenu(){','void ALWCharacter::ToggleMenu(){if(Campaign76&&Campaign76->SceneOpen){ClosePanels();return;}')
edit('LWCreation.cpp','NewGame();Identity=Chosen;RPG.Attributes=Stats;ApplyIdentity();Message=TEXT("WELCOME TO SYRACUSE, ")','NewGame();Identity=Chosen;RPG.Attributes=Stats;ApplyIdentity();ALWCampaign76::Ensure(this)->Begin(true);Message=TEXT("WELCOME TO BELLWETHER, ")')
edit('LWSurvival.cpp','if(Story){Story->Destroy();Story=nullptr;}bStoryLocked=false;','if(Campaign76){Campaign76->Destroy();Campaign76=nullptr;}if(Story){Story->Destroy();Story=nullptr;}bStoryLocked=false;')
edit('LWSurvival.cpp','World->RestoreBunkerContainers();if(BunkerManager45)BunkerManager45->Rebuild();ApplySettings();SetMenuInput(false);','World->RestoreBunkerContainers();if(BunkerManager45)BunkerManager45->Rebuild();ApplySettings();SetMenuInput(false);if(RPG.Campaign76.Started)ALWCampaign76::Ensure(this)->Begin(false);')
edit('LWPlayerRPG.cpp','void ALWCharacter::Talk(ALWResident* N){','void ALWCharacter::Talk(ALWResident* N){if(Campaign76&&Campaign76->Talk(N))return;')
edit('LWHUD.cpp','if(P->Story&&P->Story->Locked()){','if(N.ToString().StartsWith(TEXT("c76_"))){ALWCampaign76::Ensure(P)->Click(N);return;}\n    if(P->Story&&P->Story->Locked()){')
edit('LWStoryHUD.cpp','bool ALWHUD::StoryScreen(ALWCharacter* P){','bool ALWHUD::StoryScreen(ALWCharacter* P){if(P->Campaign76&&P->Campaign76->Screen(*this))return true;')
edit('LWRPGHUD.cpp','}else if(P->RPGPanel==2){','}else if(P->RPGPanel==2){\n Button(TEXT("story_journal_toggle"),P->ChapterJournal?TEXT("SHOW CONTRACTS"):TEXT("CAMPAIGN"),W-660,130,290);\n if(P->ChapterJournal){ALWCampaign76::Ensure(P)->Journal(*this);return;}')
edit('LWRPGHUD.cpp','if(P->RPG.Story.Enabled&&P->RPG.Story.Stage<29){','if(P->RPG.Campaign76.Started&&!P->RPG.Campaign76.Paused){if(const auto* M=LWCampaign76::Stage(P->RPG.Campaign76.Stage))Text(M->Title+TEXT("\\n")+Wrap(M->Goal,45),24,80,.75f);}\n else if(P->RPG.Story.Enabled&&P->RPG.Story.Stage<29){')
edit('LWPOI.cpp','void ALWChunk::Building(ALWWorld* W,const LWGen::FSite& Input){','void ALWChunk::Building(ALWWorld* W,const LWGen::FSite& Input){\n if(LWCampaign76::Dress(this,W,Input))return;')
include('LWSpawnTable.cpp') if False else None
edit('LWSpawnTable.cpp','if(!C||Site&&','if(auto* StoryPlayer=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));StoryPlayer&&StoryPlayer->RPG.Campaign76.Started&&Site&&LWCampaign76::Reserved(Site->Position))return;\n if(!C||Site&&')
# Reserve only the clinic storefront. The mall footprint, circulation, and all other stores remain intact.
edit('LWDestiny71.cpp','for(int J=0;J<Z.Fixtures.Num();J++){const auto P=Z.Fixtures[J];','const auto* StoryPlayer=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(B.W,0));const bool Clinic=StoryPlayer&&StoryPlayer->RPG.Campaign76.Started&&Index==LWCampaign76::MarketZone();\n  for(int J=0;J<Z.Fixtures.Num();J++){const auto P=Z.Fixtures[J];if(Clinic&&(P-Z.Center).Size()<1200)continue;')
edit('LWDestiny71.cpp','if(Z.Fixtures.Num()>3){','if(!Clinic&&Z.Fixtures.Num()>3){')
p=R/'LWDestiny71.cpp';s=p.read_text();p.write_text('#include "LWCharacter.h"\n#include "Kismet/GameplayStatics.h"\n'+s)
# No future conspiracy disclosure in the retrospective. Preserve compatible existing scenes.
edit('LWOpening.cpp','2030 / BEFORE THE SILENCE','2028 / BEFORE THE SILENCE')
edit('LWOpening.cpp','The year was 2030.','The year was 2028.')
edit('LWOpening.cpp','THE EXCHANGE','SEPTEMBER 2028')
edit('LWOpening.cpp','By the time the broadcasts stopped, the war was already everywhere.','Across America, cities went dark. We did not yet know what had survived.')
edit('LWOpening.cpp','{TEXT("SHELTER 01"),','{TEXT("AUTUMN 2030 / BELLWETHER"),')
edit('LWOpening.cpp','This bunker is yours now.','The bunker is still yours. Bellwether is where people know your name.')
edit('LWOpening.cpp','Before you open the door, remember who you are.','Two years later, there is work to do. Begin with who you are.')
# The existing checkpoint backend owns all snapshots; the campaign supplies only its current mission boundary.
edit('LWMissionRecovery37.cpp','FName ALWCharacter::ActiveMission37()const {','FName ALWCharacter::ActiveMission37()const {\n if(RPG.Campaign76.Started&&!RPG.Campaign76.Paused&&!RPG.Campaign76.Values.FindRef(TEXT("syracuse_finished")))return TEXT("campaign76");')
edit('LWMissionRecovery37.cpp','if(Key==TEXT("chapter1")){\n','if(Key==TEXT("campaign76")){\n  const auto* Current=LWCampaign76::Stage(RPG.Campaign76.Stage);if(!Current)return;Stage=RPG.Campaign76.Revision;Title=Current->Title;StartStage=0;for(int I=0;I<LWCampaign76::Stages().Num();I++)if(LWCampaign76::Stages()[I].Mission==Current->Mission){StartStage=I;break;}\n }else if(Key==TEXT("chapter1")){\n')
edit('LWMissionRecovery37.cpp','const bool Active=Job->Key==TEXT("chapter1")?','const bool Active=Job->Key==TEXT("campaign76")?RPG.Campaign76.Started&&!RPG.Campaign76.Paused:Job->Key==TEXT("chapter1")?')
edit('LWMissionRecovery37.cpp','if(Choice==2){if(Key==TEXT("chapter1"))','if(Choice==2){if(Key==TEXT("campaign76"))Snapshot->RPG.Campaign76.Paused=true;else if(Key==TEXT("chapter1"))')
print('Integrated campaign entry, save/load, checkpoint, dialogue, UI and parcel hooks.')
