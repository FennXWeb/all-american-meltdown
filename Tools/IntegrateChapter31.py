from pathlib import Path
root=Path('Source/LethalWorld')
def edit(n,a,b):
 p=root/n;s=p.read_text();
 if b in s:return
 assert a in s,(n,a);p.write_text(s.replace(a,b))
edit('LWRPG.h','#include "LWRPG.generated.h"','#include "LWStoryState.h"\n#include "LWRPG.generated.h"')
edit('LWRPG.h','struct FLWRPGState {\n GENERATED_BODY()','struct FLWRPGState {\n GENERATED_BODY()\n UPROPERTY() FLWStoryState Story;')
edit('LWCharacter.h','    UPROPERTY() FLWRPGState RPG;','    UPROPERTY() FLWRPGState RPG;\n    UPROPERTY() TObjectPtr<class ALWStoryDirector> Story;\n    bool bStoryLocked=false;')
edit('LWCharacter.h','return bUIInputActive||','return bStoryLocked||bUIInputActive||')
edit('LWCharacter.cpp','#include "LWCharacter.h"','#include "LWCharacter.h"\n#include "LWStory.h"')
edit('LWCharacter.cpp','    Identity=FLWIdentity();','    if(Story){Story->Destroy();Story=nullptr;}bStoryLocked=false;\n    Identity=FLWIdentity();')
edit('LWCharacter.cpp','ApplyIdentity();SaveProgress();','ApplyIdentity();RPG.Story.Enabled=true;SaveProgress();')
edit('LWCharacter.cpp','void ALWCharacter::Interact(){','void ALWCharacter::Interact(){if(bStoryLocked)return;')
edit('LWCharacter.cpp','void ALWCharacter::DoJump(){','void ALWCharacter::DoJump(){if(bStoryLocked)return;')
edit('LWCharacter.cpp','if(!bStarted||bMenu||Health<=0||bSafehouse','if(bStoryLocked||!bStarted||bMenu||Health<=0||bSafehouse')
edit('LWCharacter.cpp','    HurtFlash=FMath::Max','    if(RPG.Story.Enabled&&!Story)ALWStoryDirector::Ensure(this)->Start(false);if(bStoryLocked){bTrigger=bSprint=bAim=false;GetCharacterMovement()->StopMovementImmediately();return;}\n    HurtFlash=FMath::Max')
edit('LWSurvival.cpp','#include "LWCharacter.h"','#include "LWCharacter.h"\n#include "LWStory.h"')
edit('LWSurvival.cpp','    StandFromChair(true);\n    TGuardValue<bool> LoadingGuard','    if(Story){Story->Destroy();Story=nullptr;}bStoryLocked=false;\n    StandFromChair(true);\n    TGuardValue<bool> LoadingGuard')
edit('LWSurvival.cpp','void ALWCharacter::Respawn()\n{','void ALWCharacter::Respawn()\n{\n    if(RPG.Story.Enabled&&RPG.Story.GearHeld&&RPG.Story.Stage>=21&&RPG.Story.Stage<=23){Health=MaxHealth();bDeadSaved=false;bMenu=false;ClosePanels();ALWStoryDirector::Ensure(this)->RestorePrison();SetMenuInput(false);SaveProgress();return;}')
edit('LWPlayerRPG.cpp','#include "LWResident.h"','#include "LWResident.h"\n#include "LWStory.h"')
edit('LWPlayerRPG.cpp','void ALWCharacter::Talk(ALWResident* N){','void ALWCharacter::Talk(ALWResident* N){if(N&&N->NpcRole==TEXT("story")&&Story){Story->SpeakTo(N);return;}')
edit('LWZombie.h','    float Health=100,','    FString CustomName;\n    float Health=100,')
edit('LWEnemy29.cpp','FString N=Names[FMath::Clamp(int(Kind),0,11)];','FString N=CustomName.IsEmpty()?FString(Names[FMath::Clamp(int(Kind),0,11)]):CustomName;')
edit('LWDiscovery.cpp','bool ALWCharacter::FastTravel(){','bool ALWCharacter::FastTravel(){\n if(bStoryLocked||(RPG.Story.Enabled&&RPG.Story.Stage>=20&&RPG.Story.Stage<=26)){Notify(TEXT("ESCAPE FORT RESOLUTE BEFORE FAST TRAVELING"));return false;}')
edit('LWStory.cpp','Player->InventoryRows()','LWItems::InventoryHeight(Player->Inventory)')
edit('LWStory.cpp','Player->RPG.Respawn.Settlement=TEXT("fort_resolute");','Player->RPG.Respawn.Vehicle=NAME_None;')
edit('LWStory.cpp','if(I==0){Speech+=','if(Choices[I].StartsWith(TEXT("Tell me"))){Speech+=')
for n in ['LWStory.h','LWStory.cpp']:
 p=root/n;s=p.read_text().replace('RadioV7','RadioV4').replace('GeneratorV13','Generator');p.write_text(s)
# Permanent reservations keep random buildings, roads and vegetation out of authored compounds.
edit('LWGeneration.h','#include "LWPOITypes.h"','#include "LWPOITypes.h"\n#include "LWStoryState.h"')
edit('LWGeneration.h','const auto Reserved=SpecialNear(R,Seed);','auto Reserved=SpecialNear(R,Seed);for(int StoryIndex=0;StoryIndex<9;StoryIndex++){FSite Plot;Plot.Position=LWStory::Site(StoryIndex);Plot.Size=FVector2D(9600);if(FVector2D::Distance(Plot.Position,FVector2D(R.X*RegionSize,R.Y*RegionSize))<RegionSize*2)Reserved.Add(Plot);}')
edit('LWGeneration.h','if(Cache.Num()>512)Cache.Empty();FCachedRegion Entry;','Filtered.RemoveAll([](const FSite& S){return LWStory::Reserved(S.Position,S.Size.Size()*.5f+500);});\n        if(Cache.Num()>512)Cache.Empty();FCachedRegion Entry;')
edit('LWGeneration.h','float Blend=FMath::Clamp(float(ShelterDistance/900.),0.f,1.f);','float Blend=FMath::Clamp(float(ShelterDistance/900.),0.f,1.f);\n        for(int I=0;I<9;I++){auto D=P-LWStory::Site(I);Blend=FMath::Min(Blend,FMath::Clamp(float((FMath::Max(FMath::Abs(D.X),FMath::Abs(D.Y))-5300)/900),0.f,1.f));}')
# Custom HUD owns its clicks before generic callbacks.
edit('LWHUD.h','    void OpeningScreen','    bool StoryScreen(class ALWCharacter* P);\n    void StoryJournal(class ALWCharacter* P);\n    void OpeningScreen')
edit('LWHUD.cpp','#include "LWZombie.h"','#include "LWZombie.h"\n#include "LWStory.h"')
edit('LWHUD.cpp','    if(P->OpeningMode){P->OpeningClick(N);return;}','    if(P->Story&&P->Story->Locked()){if(N==TEXT("story_skip")&&P->Story->InScene)P->Story->FinishScene();else if(N.ToString().StartsWith(TEXT("story_choice_")))P->Story->Choose(FCString::Atoi(*N.ToString().Mid(13)));return;}\n    if(N==TEXT("story_route")){if(P->Story)P->Story->Route();return;}\n    if(P->OpeningMode){P->OpeningClick(N);return;}')
edit('LWHUD.cpp','    if(P->OpeningMode){OpeningScreen(P);return;}','    if(P->OpeningMode){OpeningScreen(P);return;}\n    if(StoryScreen(P))return;')
edit('LWRPGHUD.cpp','#include "LWHUD.h"','#include "LWHUD.h"\n#include "LWStory.h"')
edit('LWRPGHUD.cpp','Text(TEXT("CONTRACTS"),45,137,.85f);','if(P->RPG.Story.Enabled){StoryJournal(P);return;}\n Text(TEXT("CONTRACTS"),45,137,.85f);')
# Preserve contract view via a separate chapter toggle, added below.
edit('LWRPGHUD.cpp','if(!P->RPG.TrackedQuest.IsNone())for','if(P->RPG.Story.Enabled&&P->RPG.Story.Stage<29){const auto& M=LWStory::Missions()[P->RPG.Story.Stage];Text(FString(M.Title)+TEXT("\\n")+Wrap(M.Objective,45),24,80,.75f);if(P->Story&&M.Enemies)Text(FString::Printf(TEXT("Hostiles remaining: %d"),P->Story->Remaining()),24,140,.7f);}\n else if(!P->RPG.TrackedQuest.IsNone())for')
