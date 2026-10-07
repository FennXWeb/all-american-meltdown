#include "LWAircraft84.h"
#include "LWSettlement82.h"
#include "LWCampaign76.h"
#include "LWSiteIdentity.h"
#include "Misc/Crc.h"
#include "LWAudioCatalog.h"
#include "LWCharacter.h"
#include "LWCanada68.h"
#include "LWBunker45.h"
#include "LWEncounter.h"
#include "LWResident.h"
#include "LWVehicle.h"
#include "LWStory.h"
#include "LWWorld.h"
#include "LWLootTable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
void ALWCharacter::GainXP(int32 Amount){int G=LWRPG::AddXP(RPG,FMath::RoundToInt(Amount*(1+Stat(TEXT("xp")))));if(G){ULWAudioCatalog::PlaySlot(this,TEXT("LevelUp"),GetActorLocation());Notify(FString::Printf(TEXT("LEVEL %d // %d SKILL POINTS AVAILABLE [K]"),RPG.Level,RPG.Points),6);}}
bool ALWCharacter::BuyPerk(FName Id){if(!LWRPG::Buy(RPG,Id)){Notify(TEXT("REQUIRES POINTS, ATTRIBUTE AND LEVEL"));return false;}RequestSave40();return true;}
bool ALWCharacter::RaiseAttribute(int32 C){if(!RPG.Attributes.IsValidIndex(C)||RPG.Points<=0||RPG.Attributes[C]>=10)return false;RPG.Attributes[C]++;RPG.Points--;RequestSave40();return true;}
void ALWCharacter::ToggleSkills(){if(!bStarted||bMenu||Health<=0)return;if(RPGPanel==1)ClosePanels();else SwitchTab(2);}
void ALWCharacter::ToggleJournal(){if(!bStarted||bMenu||Health<=0)return;if(RPGPanel==2)ClosePanels();else SwitchTab(3);}
void ALWCharacter::ToggleCrew(){if(!bStarted||bMenu||Health<=0)return;if(RPGPanel==3)ClosePanels();else SwitchTab(4);}
int32 ALWCharacter::CountSupply(FName D)const{int N=0;for(const auto& I:Inventory)if(I.Definition==D&&I.Slot.IsNone())N+=I.Count;return N;}
bool ALWCharacter::ConsumeSupply(FName D,int N){if(N<0||CountSupply(D)<N)return false;for(int I=Inventory.Num()-1;I>=0&&N>0;I--)if(Inventory[I].Definition==D&&Inventory[I].Slot.IsNone()){int Take=FMath::Min(N,Inventory[I].Count);Inventory[I].Count-=Take;N-=Take;if(!Inventory[I].Count)Inventory.RemoveAt(I);}return true;}
float ALWCharacter::CombatMultiplier(bool Head)const{
 float V=1+Stat(Weapon<2?TEXT("melee"):TEXT("gun"));
 const TCHAR* E[]={TEXT("crowbar"),TEXT("bat"),TEXT("shotgun"),TEXT("revolver"),TEXT("rifle"),TEXT("smg"),TEXT("rifle"),TEXT("smg")};V+=Stat(Weapon==8?TEXT("shotgun"):E[FMath::Clamp(Weapon,0,7)]);if(Head)V*=1+Stat(TEXT("headshot"));
 if(FMath::FRand()<FMath::Min(.5f,Stat(TEXT("critical"))))V*=1.5f+Stat(TEXT("critdamage"));return V;
}
bool ALWCharacter::AcceptQuest(FName Id){
 const auto* Q=ULWRPGCatalog::Get()->Quests.FindByPredicate([&](const auto& X){return X.Id==Id;});
 if(!Q||RPG.Quests.ContainsByPredicate([&](const auto& X){return X.Id==Id;}))return false;
 if(!Q->Prerequisite.IsNone()&&!RPG.Quests.ContainsByPredicate([&](const auto& X){return X.Id==Q->Prerequisite&&X.Rewarded;}))return false;
 int Active=0;for(const auto& X:RPG.Quests)Active+=!X.Rewarded&&!X.Id.ToString().StartsWith(TEXT("landmark_"));if(Active>=6){Notify(TEXT("FINISH A CONTRACT // SIX ACTIVE LIMIT"));return false;}
 FLWQuestState S;S.Id=Id;RPG.Quests.Add(S);RPG.TrackedQuest=Id;
 if(Q->Objectives.Num()&&Q->Objectives[0].Event==TEXT("recruit")&&RPG.Crew.Num())QuestEvent(TEXT("recruit"),NAME_None,RPG.Crew.Num());
 CaptureMission37(Id);Notify(TEXT("CONTRACT ACCEPTED // [J] JOURNAL"));RequestSave40();return true;
}
void ALWCharacter::QuestEvent(FName Event,FName Target,int32 Count){
 if(Count<=0)return;bool Changed=false;
 for(auto& S:RPG.Quests){if(S.Rewarded)continue;const auto* Q=ULWRPGCatalog::Get()->Quests.FindByPredicate([&](const auto& X){return X.Id==S.Id;});if(!Q||!Q->Objectives.IsValidIndex(S.Stage))continue;const auto& O=Q->Objectives[S.Stage];
 if(O.Event==Event&&(O.Target.IsNone()||O.Target==Target)){S.Progress=FMath::Min(O.Count,S.Progress+Count);Changed=true;if(S.Progress>=O.Count){S.Stage++;S.Progress=0;Notify(Q->Objectives.IsValidIndex(S.Stage)?TEXT("CONTRACT OBJECTIVE COMPLETE // [J]"):TEXT("CONTRACT READY // RETURN TO A WARDEN"));}}}
 if(Changed){for(const auto& Q:RPG.Quests)if(!Q.Rewarded)CaptureMission37(Q.Id);RequestSave40();}
}
bool ALWCharacter::TurnInQuest(FName Id){
 if(Id.ToString().StartsWith(TEXT("landmark_")))return false;
 auto* S=RPG.Quests.FindByPredicate([&](const auto& X){return X.Id==Id;});const auto* Q=ULWRPGCatalog::Get()->Quests.FindByPredicate([&](const auto& X){return X.Id==Id;});
 if(!S||!Q||S->Rewarded||!Speaker||Speaker->NpcRole!=TEXT("warden")||FVector::Dist(Speaker->GetActorLocation(),GetActorLocation())>550)return false;
 if(Q->Objectives.IsValidIndex(S->Stage)){const auto& O=Q->Objectives[S->Stage];if(O.Event!=TEXT("deliver")||!ConsumeSupply(O.Target,O.Count))return false;S->Stage++;S->Progress=0;}
 if(S->Stage>=Q->Objectives.Num()){S->Rewarded=true;MissionRecovery37.Remove(Id);ChangeReputation(Speaker->SettlementId,12);Money+=FMath::RoundToInt(Q->Credits*(1+Stat(TEXT("questcredits"))));GainXP(FMath::RoundToInt(Q->XP*(1+Stat(TEXT("questxp")))));Notify(TEXT("CONTRACT PAID // EXPERIENCE AWARDED"));}
 RequestSave40();return true;
}
bool ALWCharacter::TrackQuest(FName Id){
 const auto* S=RPG.Quests.FindByPredicate([&](const auto& X){return X.Id==Id&&!X.Rewarded;});const auto* Q=ULWRPGCatalog::Get()->Quests.FindByPredicate([&](const auto& X){return X.Id==Id;});if(!S||!Q)return false;RPG.TrackedQuest=Id;CaptureMission37(Id);
 if(Q->Objectives.IsValidIndex(S->Stage)&&Q->Objectives[S->Stage].Event==TEXT("visit")){
 FName T=Q->Objectives[S->Stage].Target;if(T==TEXT("bunker"))RouteToBunker();else{TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Gather(FVector2D(GetActorLocation()),World->Seed,Roads,Sites);const TCHAR* Types[]={TEXT("gas"),TEXT("motel"),TEXT("clinic"),TEXT("depot"),TEXT("diner")};float Best=MAX_flt;for(const auto& Site:Sites)if(T==LWPlaces::Event(Site.Type)){float D=FVector2D::Distance(Site.Position,FVector2D(GetActorLocation()));if(D<Best){Best=D;SetWaypoint(Site.Position);}}}
 }else{FVector2D P(GetActorLocation());FIntPoint R(LWGen::FloorDiv(P.X+LWGen::RegionSize*.5,LWGen::RegionSize),LWGen::FloorDiv(P.Y+LWGen::RegionSize*.5,LWGen::RegionSize));FVector2D BestTown=FVector2D::ZeroVector;float Best=MAX_flt;for(int Y=-3;Y<=3;Y++)for(int X=-3;X<=3;X++){FIntPoint N=R+FIntPoint(X,Y);if(!LWGen::HasTown(N,World->Seed))continue;FVector2D At=LWGen::Hub(N,World->Seed)+FVector2D(0,-5000);float D=FVector2D::Distance(At,P);if(D<Best){Best=D;BestTown=At;}}if(Best<MAX_flt)SetWaypoint(BestTown);}RequestSave40();return true;
}
bool ALWCharacter::Recruit(ALWResident* N){
 if(N&&Settlement82){FName Home=Settlement82->HomeClaim(N->ResidentId);if(!Home.IsNone()){FName Before=Settlement82->Selected;Settlement82->Selected=Home;bool Result=Settlement82->Crew(N->ResidentId,true);Settlement82->Selected=Before;return Result;}}
 if(!N||N!=Speaker||N->NpcRole!=TEXT("recruit")||FVector::Dist(GetActorLocation(),N->GetActorLocation())>550||RPG.Crew.Num()>=Bunker45.Bedrooms()||RPG.Crew.ContainsByPredicate([&](const auto& C){return C.Id==N->ResidentId;}))return false;
 int Cost=FMath::RoundToInt(200*FMath::Max(.2f,1-Stat(TEXT("hire"))));if(Money<Cost){Notify(TEXT("NOT ENOUGH CREDITS"));return false;}
 FLWCrewRecord C;C.Id=N->ResidentId;C.Name=N->DisplayName;C.Voice=N->Voice;C.Bedroom=RPG.Crew.Num();int Active=0;for(const auto& X:RPG.Crew)Active+=X.Following;C.Following=Active<CompanionLimit();RPG.Crew.Add(C);N->SettlementId=NAME_None;Money-=Cost;
 for(auto& Chunk:World->Chunks)Chunk.Value->Residents.Remove(N);QuestEvent(TEXT("recruit"));GainXP(50);RequestSave40();Notify(C.Following?TEXT("COMPANION JOINED YOUR SQUAD"):TEXT("COMPANION ASSIGNED A BUNKER BEDROOM"));return true;
}
bool ALWCharacter::SetCompanion(FName Id,bool Follow){if(Settlement82){FName Home=Settlement82->HomeClaim(Id);if(!Home.IsNone()){FName Before=Settlement82->Selected;Settlement82->Selected=Home;bool Result=Settlement82->Crew(Id,Follow);Settlement82->Selected=Before;return Result;}}auto* C=RPG.Crew.FindByPredicate([&](const auto& X){return X.Id==Id;});if(!C)return false;int Active=0;for(const auto& X:RPG.Crew)Active+=X.Following;if(Follow&&!C->Following&&Active>=CompanionLimit()){Notify(TEXT("SQUAD FULL // UNLOCK PRESENCE PERKS"));return false;}C->Following=Follow;if(Follow){C->Station=NAME_None;for(TActorIterator<ALWResident> N(GetWorld());N;++N)if(N->ResidentId==Id)N->SettlementId=NAME_None;}RequestSave40();return true;}
void ALWCharacter::TickRPG(float Dt){
 if(!bStarted||bMenu||Health<=0)return;if(RPGPanel){GetCharacterMovement()->StopMovementImmediately();bTrigger=false;bSprint=false;bAim=false;}
 if(Speaker&&(!IsValid(Speaker)||FVector::Dist(Speaker->GetActorLocation(),GetActorLocation())>550))ClosePanels();
 if(Health<MaxHealth())Health=FMath::Min(MaxHealth(),Health+Stat(TEXT("regen"))*Dt);
 RPGClock-=Dt;if(RPGClock>0)return;RPGClock=1;
 TickSettlements();
 if(bSafehouse)QuestEvent(TEXT("visit"),TEXT("bunker"));else{
 TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Gather(FVector2D(GetActorLocation()),World->Seed,Roads,Sites);
 const TCHAR* Types[]={TEXT("gas"),TEXT("motel"),TEXT("clinic"),TEXT("depot"),TEXT("diner")};
 for(const auto& S:Sites)if(FVector2D::Distance(FVector2D(GetActorLocation()),S.Position)<700){FName Id(*FString::Printf(TEXT("site_%u"),S.Id));if(!S.SettlementBuilding)DiscoverPlace(S.Id,FVector(LWGen::Entrance(S),0),LWSites::Name(S));QuestEvent(TEXT("visit"),LWPlaces::Event(S.Type));}
 }
 for(const auto& C:RPG.Crew){if(!C.Following&&Settlement82&&!Settlement82->HomeClaim(C.Id).IsNone())continue;ALWResident* Found=nullptr;for(TActorIterator<ALWResident> I(GetWorld());I;++I)if(I->ResidentId==C.Id){Found=*I;break;}
 FVector Home45=World->BedroomPosition(C.Bedroom);bool RVHome66=false;
 if(!C.HomeVehicle66.IsNone()&&C.Station.IsNone())if(const auto* CarRecord=World->Vehicles.Find(C.HomeVehicle66))if(!CarRecord->Exploded&&CarRecord->Health>0){
  RVHome66=true;Home45=CarRecord->Position;
  if(!C.Following&&FVector::Dist2D(GetActorLocation(),Home45)>(World->RenderRadius+1)*LWGen::ChunkSize){if(Found&&Found!=Speaker)Found->Destroy();continue;}
  ALWVehicle* RV=nullptr;for(TActorIterator<ALWVehicle> V(GetWorld());V;++V)if(V->RecordId==C.HomeVehicle66){RV=*V;break;}
  if(!C.Following&&!RV)RV=Cast<ALWVehicle>(World->SpawnObject(ELWObjectKind::Car,C.HomeVehicle66,CarRecord->Position,CarRecord->Rotation));
  if(RV)Home45=RV->BunkPosition66(C.HomeBunk66)+FVector(0,0,80);
 }
 if(!RVHome66&&BunkerManager45)if(const auto* Job=Bunker45.Assignments.Find(C.Id))if(Bunker45.Utilities.Contains(*Job))Home45=BunkerManager45->UtilityPosition(*Job);if(!C.Following&&!RVHome66&&C.Station.IsNone()&&(!bSafehouse||FMath::Abs(GetActorLocation().Z-Home45.Z)>380)){if(Found&&Found!=Speaker)Found->Destroy();continue;}
 if(!Found){FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;const auto* Station=RPG.Settlements.Find(C.Station);FVector Pos=C.Following?GetActorLocation()-GetActorForwardVector()*200:Station?Station->Center+FVector(C.Bedroom*85,700,0):Home45;auto* N=GetWorld()->SpawnActor<ALWResident>(Pos,FRotator::ZeroRotator,Params);if(N){N->ConfigureResident(C.Id,TEXT("recruit"),C.Name,C.Voice);N->SettlementId=C.Station;if(RVHome66)N->TickRVHome66(this);}}}
}
void ALWCharacter::Talk(ALWResident* N){if(N&&Settlement82){FName Home=Settlement82->HomeClaim(N->ResidentId);if(!Home.IsNone()){Settlement82->Open(Home);Settlement82->Panel=1;Settlement82->SelectedResident=N->ResidentId;return;}}if(Campaign76&&Campaign76->Talk(N))return;if(N&&N->NpcRole==TEXT("story")&&Story){Story->SpeakTo(N);return;}if(N&&N->IsTownHostile()){Notify(TEXT("THEY ARE HOSTILE"));return;}if(N&&N->NpcRole==TEXT("encounter")){if(auto* E=Cast<ALWEncounterScene>(N->Shop))E->Use(this);return;}if(!N||N->NpcRole==TEXT("civilian")||!CanAct())return;ClosePanels();Speaker=N;RPGPanel=4;CancelReload();SetMenuInput(true);QuestEvent(TEXT("talk"),N->NpcRole);BuildDialogue(TEXT("root"));}
void ALWCharacter::BuildDialogue(FName Node){
 if(!Speaker)return;FString SpokenText;DialogueNode=Node;DialogueChoices.Empty();DialogueActions.Empty();
 auto Choice=[&](FString T,FName A){DialogueChoices.Add(T);DialogueActions.Add(A);};
 if(LWCanada68::Dialogue(this))return;
 if(BuildSettlementDialogue(Node))return;
 if(Node==TEXT("root")){
 DialogueText=TEXT("We survive by looking after each other. What do you need?");
 Choice(TEXT("What is this place?"),TEXT("rumors"));
 if(!Speaker->SettlementId.IsNone())Choice(TEXT("Settlement affairs."),TEXT("town"));
 if(Speaker->NpcRole==TEXT("warden"))Choice(TEXT("Let us talk about contracts."),TEXT("contracts"));
 if(Speaker->NpcRole==TEXT("merchant"))Choice(TEXT("Show me your supplies."),TEXT("trade"));
 if(Speaker->NpcRole==TEXT("medic"))Choice(TEXT("Treat my injuries. [40 credits]"),TEXT("heal"));
 if(Speaker->NpcRole==TEXT("recruit")){bool Hired=RPG.Crew.ContainsByPredicate([&](const auto& C){return C.Id==Speaker->ResidentId;});Choice(Hired?TEXT("Come with me."):FString::Printf(TEXT("Join my crew. [%d credits]"),FMath::RoundToInt(200*FMath::Max(.2f,1-Stat(TEXT("hire"))))),Hired?TEXT("follow"):TEXT("hire"));if(Hired)Choice(TEXT("Return home."),TEXT("home"));}
 if(RPG.Attributes.IsValidIndex(3)&&RPG.Attributes[3]>=5&&!RPG.Persuaded.Contains(Speaker->ResidentId))Choice(TEXT("[PRESENCE 5] We should share what we know."),TEXT("persuade"));
 Choice(TEXT("Goodbye."),TEXT("bye"));
 }else if(Node==TEXT("rumors")){DialogueText=TEXT("This is a waystation on the old road network. Wardens post work, merchants sell supplies, and drifters sometimes sign on. Shelter 01 has room for ten. Use your journal to follow a contract.");Choice(TEXT("Tell me about the dangers."),TEXT("dangers"));Choice(TEXT("Back."),TEXT("root"));}
 else if(Node==TEXT("dangers")){DialogueText=TEXT("Raiders watch the roads. Dogs hunt by sound. The pale mannequins move when nobody watches. Bring someone you trust, and never stand beside a fuel pump in a firefight.");Choice(TEXT("Back."),TEXT("root"));}
 else if(Node==TEXT("contracts")){
 DialogueText=TEXT("Six active contracts at a time. Deliver supplies here, and return when your work is finished.");
 const auto All=ULWRPGCatalog::Get()->Quests.FilterByPredicate([](const auto& Q){return !Q.Id.ToString().StartsWith(TEXT("landmark_"));});for(int I=JournalPage*5;I<FMath::Min(All.Num(),JournalPage*5+5);I++){const auto& Q=All[I];const auto* S=RPG.Quests.FindByPredicate([&](const auto& X){return X.Id==Q.Id;});if(S&&S->Rewarded)continue;bool Locked=!Q.Prerequisite.IsNone()&&!RPG.Quests.ContainsByPredicate([&](const auto& X){return X.Id==Q.Prerequisite&&X.Rewarded;});Choice(Q.Title+(S?TEXT(" [ACTIVE]"):Locked?TEXT(" [LOCKED]"):TEXT(" [AVAILABLE]")),FName(*(TEXT("quest:")+Q.Id.ToString())));}
 Choice(TEXT("More contracts."),TEXT("more"));Choice(TEXT("Back."),TEXT("root"));
 }else if(Node.ToString().StartsWith(TEXT("quest:"))){FName Id(*Node.ToString().Mid(6));const auto* Q=ULWRPGCatalog::Get()->Quests.FindByPredicate([&](const auto& X){return X.Id==Id;});if(!Q){BuildDialogue(TEXT("root"));return;}SpokenText=Q->Description;DialogueText=Q->Description+FString::Printf(TEXT(" Reward: %d XP / %d credits."),Q->XP,Q->Credits);const auto* Progress=RPG.Quests.FindByPredicate([&](const auto& X){return X.Id==Id;});bool Active=Progress!=nullptr;bool Locked=!Q->Prerequisite.IsNone()&&!RPG.Quests.ContainsByPredicate([&](const auto& X){return X.Id==Q->Prerequisite&&X.Rewarded;});if(Locked){const auto* Pre=ULWRPGCatalog::Get()->Quests.FindByPredicate([&](const auto& X){return X.Id==Q->Prerequisite;});DialogueText+=TEXT(" Complete and claim: ")+(Pre?Pre->Title:Q->Prerequisite.ToString());}else if(Q->Objectives.IsValidIndex(Progress?Progress->Stage:0))DialogueText+=TEXT(" Objective: ")+Q->Objectives[Progress?Progress->Stage:0].Text;if(!Locked)Choice(Active?TEXT("Deliver supplies / claim reward."):TEXT("Accept this contract."),FName(*((Active?FString(TEXT("turn:")):FString(TEXT("accept:")))+Id.ToString())));Choice(TEXT("Back."),TEXT("contracts"));}
 Speaker->Say(SpokenText.IsEmpty()?DialogueText:SpokenText);
}
void ALWCharacter::ChooseDialogue(int32 I){if(DialogueActions.IsValidIndex(I)&&LWAviation84::UseService(this,DialogueActions[I]))return;
 if(Service66){ChooseService66(I);return;}
 if(RestBed&&DialogueActions.IsValidIndex(I)){FName A=DialogueActions[I];auto* B=RestBed.Get();ClosePanels();if(A==TEXT("bed_spawn"))SetBedSpawn(B);else if(A==TEXT("bed_sleep"))SleepInBed();return;}
 if(EncounterSpeaker){EncounterSpeaker->Choose(this,I);return;}
 if(!Speaker||!DialogueActions.IsValidIndex(I)||FVector::Dist(Speaker->GetActorLocation(),GetActorLocation())>550)return;FName A=DialogueActions[I];FString S=A.ToString();
 if(LWCanada68::Action(this,A))return;
 if(SettlementAction(A))return;
 if(A==TEXT("bye")){ClosePanels();return;}
 if(A==TEXT("trade")){auto* N=Speaker.Get();FName Id(*(TEXT("shop_")+N->ResidentId.ToString()));if(!World->Containers.Contains(Id)){FLWContainerRecord R;R.Id=Id;R.bTrader=true;R.Context=TEXT("trader");R.Position=N->GetActorLocation();for(auto Item:LWLoot::Roll(TEXT("trader"),int32(FCrc::StrCrc32(*Id.ToString()))))LWItems::Place(R.Items,Item,12,12);World->Containers.Add(Id,R);}if(!IsValid(N->Shop)){N->Shop=World->SpawnObject(ELWObjectKind::Trader,Id,N->GetActorLocation());N->Shop->SetActorHiddenInGame(true);N->Shop->SetActorEnableCollision(false);N->Shop->SetActorTickEnabled(false);}N->Shop->SetActorLocation(N->GetActorLocation());World->Containers.FindChecked(Id).Position=N->GetActorLocation();OpenContainer(N->Shop);return;}
 if(A==TEXT("heal")){if(Money>=40){Money-=40;Health=MaxHealth();RequestSave40();}else Notify(TEXT("NOT ENOUGH CREDITS"));BuildDialogue(TEXT("root"));return;}
 if(A==TEXT("hire")){Recruit(Speaker);BuildDialogue(TEXT("root"));return;}
 if(A==TEXT("follow")||A==TEXT("home")){SetCompanion(Speaker->ResidentId,A==TEXT("follow"));ClosePanels();return;}
 if(A==TEXT("persuade")){if(RPG.Attributes[3]>=5&&!RPG.Persuaded.Contains(Speaker->ResidentId)){RPG.Persuaded.Add(Speaker->ResidentId);GainXP(80);RequestSave40();}BuildDialogue(TEXT("rumors"));return;}
 if(A==TEXT("more")){JournalPage=(JournalPage+1)%3;BuildDialogue(TEXT("contracts"));return;}
 if(S.StartsWith(TEXT("accept:"))){if(!AcceptQuest(FName(*S.Mid(7))))Notify(TEXT("CONTRACT LOCKED OR ALREADY ACCEPTED"));BuildDialogue(TEXT("contracts"));return;}
 if(S.StartsWith(TEXT("turn:"))){if(!TurnInQuest(FName(*S.Mid(5))))Notify(TEXT("OBJECTIVES OR SUPPLIES STILL MISSING"));BuildDialogue(TEXT("contracts"));return;}
 BuildDialogue(A);
}

