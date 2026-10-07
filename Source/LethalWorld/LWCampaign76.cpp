#include "LWGeography84.h"
#include "LWCampaign76.h"
#include "LWCampaignProduction77.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWAppearance.h"
#include "LWDialogue59.h"
#include "LWNPCLife.h"
#include "LWWeaponEffect.h"
#include "Kismet/GameplayStatics.h"
#include "LWVehicle.h"
#include "LWCanada68.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Misc/Crc.h"

namespace {
FName Key76(const TCHAR* Prefix,FName Id){return FName(*(FString(Prefix)+Id.ToString()));}
FName EnemyHealth76(int I){return FName(*FString::Printf(TEXT("enemy_%d"),I));}
bool FixedWorkProp77(FName M){return M==TEXT("ControlPanel52")||M==TEXT("RelayConsole52")||M==TEXT("Sink65")||M==TEXT("Bed65");}
// FName table indices can change between launches. Persistent encounter IDs
// must depend on the authored name, not its process-local interned index.
int EnemyId76(FName Stage,int I){return int(FCrc::StrCrc32(*Stage.ToString())^0x76b00000u^(uint32(I+1)*0x19f03u));}
}
ALWCampaign76::ALWCampaign76(){PrimaryActorTick.bCanEverTick=true;RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("NarrativeRoot"));Production=CreateDefaultSubobject<ULWCampaignProduction77>(TEXT("CampaignProduction"));}
ALWCampaign76* ALWCampaign76::Ensure(ALWCharacter* P){if(!P||!P->World)return nullptr;if(!IsValid(P->Campaign76)){P->Campaign76=P->GetWorld()->SpawnActor<ALWCampaign76>();P->Campaign76->Player=P;P->Campaign76->World=P->World;}return P->Campaign76;}
FLWCampaign76State& ALWCampaign76::State()const{return Player->RPG.Campaign76;}
FVector ALWCampaign76::At(FVector V)const{const auto* S=LWCampaign76::Stage(State().Stage);return S?LWCampaign76::Frame(S->Site).TransformPosition(V):Player->GetActorLocation();}
FVector ALWCampaign76::Goal()const{if(State().Sailing77)return LWProduction77::FerryPose(LWProduction77::FerryDuration()).GetLocation();if(const auto* S=LWCampaign76::Stage(State().Stage)){for(const auto& A:S->Actions)if(LWCampaign76::Meets(State(),A.Needs)&&(!State().Rewards.Contains(Key76(TEXT("action_"),A.Id))||!A.Next.IsNone()||!A.Scene.IsNone()))return At(A.At+FVector(0,0,80));}return At(FVector(0,-220,90));}
void ALWCampaign76::Route(){if(Player&&State().Started)Player->SetWaypoint(FVector2D(Goal()));}
void ALWCampaign76::Begin(bool Fresh){
 if(!Player||!World)return;
 if(Fresh){Clear();State()=FLWCampaign76State();State().Started=true;State().Stage=TEXT("bell_lights");
  // Only an explicitly new campaign relocates the player. Loading an existing survivor never does.
  if(Player->Vehicle)Player->Vehicle->Exit(true);World->Reset();const FVector Start=At(FVector(0,-250,92));
  Player->SetActorLocation(Start,false,nullptr,ETeleportType::TeleportPhysics);World->Stream(Start,true);Player->bSafehouse=false;
  if(Player->Controller)Player->Controller->SetControlRotation(FRotator(0,LWCampaign76::Frame(TEXT("bellwether")).Rotator().Yaw+90,0));
 }
 SetActorTickEnabled(State().Started&&!State().Paused);if(!State().Started||State().Paused)return;
 Spawn();Route();Player->CaptureMission37(TEXT("campaign76"));Player->RequestSave40();
}
void ALWCampaign76::Sync(){
 if(!Player)return;for(const auto& Pair:People)if(IsValid(Pair.Value))State().Health.Add(Pair.Key,FMath::Max(0.f,Pair.Value->Health));
 for(const auto& N:Enemies)if(IsValid(N)){State().Health.Add(EnemyHealth76(N->Index),FMath::Max(0.f,N->Health));if(N->bDead)State().Defeated.Add(N->Index);}
 State().Values.Add(TEXT("credits"),int32(FMath::Min<int64>(MAX_int32,Player->Money)));State().Values.Add(TEXT("enemies_clear"),Fighting()?0:1);
}
void ALWCampaign76::Clear(){
 if(Production)Production->Clear();
 RestoreWeather();Sync();if(RoomSound){RoomSound->Stop();RoomSound->DestroyComponent();RoomSound=nullptr;}if(SceneSound){SceneSound->Stop();SceneSound->DestroyComponent();SceneSound=nullptr;}if(Heater){Heater->DestroyComponent();Heater=nullptr;}
 for(auto& A:Actors)if(IsValid(A))A->Destroy();Actors.Empty();People.Empty();Enemies.Empty();Carrier=nullptr;CarrierShutter=nullptr;CarrierTurret=nullptr;CarrierShell.Empty();
 for(auto& L:SceneLights)if(L)L->DestroyComponent();SceneLights.Empty();for(auto& M:Dressing)if(M)M->DestroyComponent();Dressing.Empty();SpawnedStage=NAME_None;WorkAction=NAME_None;WorkSeconds=0;
}
void ALWCampaign76::EndPlay(const EEndPlayReason::Type R){Pause();Clear();Super::EndPlay(R);}
void ALWCampaign76::SetStage(FName Id){
 const auto* Next=LWCampaign76::Stage(Id);if(!Next||State().Stage==Id)return;
 if(Id==TEXT("canada_arrival")&&State().Values.FindRef(TEXT("entry_route"))==3&&!State().Values.FindRef(TEXT("ferry_landed77"))){if(Production)Production->BeginSailing();return;}
 if(Id==TEXT("canada_arrival")&&!Travel(TEXT("canada")))return;
 if(Id==TEXT("homecoming")&&LWGeography84::Canada(FVector2D(Player->GetActorLocation()))&&!Travel(TEXT("crossing")))return;
 const auto* Old=LWCampaign76::Stage(State().Stage);Sync();Pause();
 const bool RouteRetreat=Old&&Id==TEXT("watertown_arrive")&&(Old->Site==TEXT("watertown")||Old->Site==TEXT("crossing")||Old->Site==TEXT("ferry"));
 if(Old&&!RouteRetreat&&Old->Mission!=Next->Mission&&!State().Completed.Contains(Old->Mission)){
  State().Completed.Add(Old->Mission);Player->GainXP(100);Player->Notify(Old->Title+TEXT(" completed"),5);
 }
 Clear();State().Work77=NAME_None;State().WorkSeconds77=0;State().Performance77=NAME_None;State().Stage=Id;State().Scene=State().AfterScene=NAME_None;State().Beat=0;State().Revision++;State().EncounterSeconds=0;State().Decisions.Remove(Key76(TEXT("arrived_"),Id));
 if(Id==TEXT("syracuse_complete")&&!State().Completed.Contains(Next->Mission)){State().Completed.Add(Next->Mission);Player->GainXP(100);State().Values.Add(TEXT("syracuse_finished"),1);}
 PrepareStage();Spawn();Route();Player->CaptureMission37(TEXT("campaign76"),true);Player->RequestSave40();
}
void ALWCampaign76::Spawn(){
 const auto* S=LWCampaign76::Stage(State().Stage);if(!S||SpawnedStage==S->Id)return;
 if(FVector::Dist2D(Player->GetActorLocation(),At(FVector::ZeroVector))>7000)return;
 // The parcel must have finished collision construction before people or work props appear.
 const auto* Chunk=World->Chunks.FindRef(LWGen::ChunkAt(FVector2D(LWCampaign76::Frame(S->Site).GetLocation()))).Get();if(!Chunk||!Chunk->Ready68)return;
 SpawnedStage=S->Id;StageDressing();FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
 for(int I=0;I<S->Cast.Num();I++){
  FName Id=S->Cast[I];const auto* Def=LWCampaign76::Person(Id);if(!Def||!CastAvailable(Id))continue;
  FVector Local((I%3-1)*270,150+(I/3)*220,92);
  if(Id==TEXT("lena"))Local=FVector(110,330,92);
  if(S->Id.ToString().StartsWith(TEXT("crossing_"))&&(Id==TEXT("milo")||Id==TEXT("tomas")||Id==TEXT("ruth")))Local=FVector(-360,340+I*70,92);
  if(S->Site==TEXT("transport")&&Id==TEXT("richardson"))Local=FVector(0,250,92);
  for(const auto& A:S->Actions)if(A.Id==Key76(TEXT("talk_"),Id)){Local=A.At+FVector(0,0,92);break;}
  auto* N=GetWorld()->SpawnActor<ALWCampaignPerson76>(At(Local),FRotator(0,-90,0),Params);if(!N)continue;
  N->Campaign=this;N->Person=Id;if(!Player->RPG.VoiceProfiles59.Contains(Key76(TEXT("c76_"),Id)))Player->RPG.VoiceProfiles59.Add(Key76(TEXT("c76_"),Id),Def->Voice);N->ConfigureResident(Key76(TEXT("c76_"),Id),TEXT("campaign76"),Def->Name,I,Def->Female?1:0);
  N->CustomName=Def->Name;N->Health=State().Health.Contains(Id)?FMath::Max(1.f,State().Health[Id]):150;N->MaximumHealth=150;if(Id==TEXT("richardson")){N->Health=FMath::Min(90.f,N->Health);N->MaximumHealth=90;}N->LegendaryInitialized=true;N->Home=N->GetActorLocation();
  State().Values.Add(Key76(TEXT("met_"),Id),1);N->ChatterTime=99999;N->SetActorRotation((Player->GetActorLocation()-N->GetActorLocation()).Rotation());Actors.Add(N);People.Add(Id,N);
 }
 int Count=S->Enemies;if(S->Id==TEXT("bell_attack")&&State().Values.FindRef(TEXT("watch_ready")))Count--;
 if(S->Id==TEXT("carrier_battle")&&State().Values.FindRef(TEXT("unit_surrender")))Count=FMath::Max(2,Count-2);
 if(S->Id.ToString().StartsWith(TEXT("crossing_"))&&State().Values.FindRef(TEXT("guard_support"))>0)Count=FMath::Max(2,Count-1);
 for(int I=0;I<Count;I++){
  const int Id=EnemyId76(S->Id,I);if(State().Defeated.Contains(Id))continue;
  const bool Bell=S->Id==TEXT("bell_attack");FVector V((I%2?1:-1)*(Bell?850:600),Bell?-1150-I*250:-550-I*160,92);
  if(Bell&&State().Values.FindRef(TEXT("gate_braced")))V.Y-=600;
  auto* N=GetWorld()->SpawnActor<ALWCampaignEnemy76>(At(V),FRotator(0,90,0),Params);if(!N)continue;
  N->Campaign=this;N->Index=Id;N->PersistentId=uint32(Id);N->ConfigureKind(ELWEnemyKind::Raider);N->CustomName=S->Id==TEXT("bell_attack")?TEXT("ARMED SEARCHER"):S->Id==TEXT("hill_fight")?TEXT("ROAD AMBUSHER"):S->Id==TEXT("loyal_final")?TEXT("RESISTANCE DEFENDER"):TEXT("DIRECTORATE ESCORT");N->Stars=0;N->LegendaryInitialized=true;
  N->MaximumHealth=90;N->Health=State().Health.Contains(EnemyHealth76(Id))?FMath::Max(1.f,State().Health[EnemyHealth76(Id)]):90;
  N->Home=N->Interest=At(FVector(-170,80,92));N->Alert=10;Actors.Add(N);Enemies.Add(N);World->ZombieCount++;
 }
 Refresh();AfterEffect();if(Production){Production->Director=this;Production->Build();Production->PlaceCast();}
 WorkAction=State().Work77;WorkSeconds=State().WorkSeconds77;
}
void ALWCampaign76::Refresh(){
 const auto* S=LWCampaign76::Stage(State().Stage);if(!S||SpawnedStage!=S->Id)return;
 for(auto& Actor:Actors)if(auto* N=Cast<ALWCampaignNode76>(Actor)){
  const auto* A=S->Actions.FindByPredicate([&](const auto& X){return X.Id==N->Action;});
  const bool Active=A&&LWCampaign76::Meets(State(),A->Needs)&&(!State().Rewards.Contains(Key76(TEXT("action_"),A->Id))||!A->Next.IsNone()||!A->Scene.IsNone());N->Usable=Active;const bool Present=Active||(A&&FixedWorkProp77(A->Mesh));N->SetActorHiddenInGame(!Present);N->SetActorEnableCollision(Present);
 }
 for(const auto& A:S->Actions){const bool AbsentTalk=A.Mesh.IsNone()&&!CastAvailable(FName(*A.Id.ToString().Mid(5)));
  if(A.Mesh.IsNone()&&!AbsentTalk)continue;if(Actors.ContainsByPredicate([&](const auto& O){auto* N=Cast<ALWCampaignNode76>(O);return N&&N->Action==A.Id;}))continue;
  auto* M=World->Mesh(AbsentTalk?FName(TEXT("Dossier52")):A.Mesh);if(!M)continue;FVector V=A.At;V.Z+=FMath::Max(0.,-double(M->GetBoundingBox().Min.Z));if(A.Mesh==TEXT("Dossier52")||A.Mesh==TEXT("Keys52")||AbsentTalk)if(auto* Table=World->Mesh(TEXT("CoffeeTable65")))V.Z+=Table->GetBoundingBox().GetSize().Z+1;
  auto* N=GetWorld()->SpawnActor<ALWCampaignNode76>(At(V),LWCampaign76::Frame(S->Site).Rotator());if(!N)continue;
  N->Configure(World,ELWObjectKind::Sign,Key76(TEXT("c76_"),A.Id));for(auto& Part:N->Details)if(Part)Part->DestroyComponent();N->Details.Empty();N->SetActorTickEnabled(false);N->Body->SetStaticMesh(M);N->Body->EmptyOverrideMaterials();N->Body->SetCollisionProfileName(TEXT("BlockAll"));if(A.Mesh==TEXT("Case77")||A.Mesh==TEXT("Dossier52")||A.Mesh==TEXT("Keys52"))N->Body->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore);N->Campaign=this;N->Action=A.Id;N->Label=AbsentTalk?TEXT("Read the remaining working notes"):A.Label;
  const bool Active=LWCampaign76::Meets(State(),A.Needs)&&(!State().Rewards.Contains(Key76(TEXT("action_"),A.Id))||!A.Next.IsNone()||!A.Scene.IsNone());N->Usable=Active;const bool Present=Active||FixedWorkProp77(A.Mesh);N->SetActorHiddenInGame(!Present);N->SetActorEnableCollision(Present);Actors.Add(N);
 }
}
bool ALWCampaign76::Fighting()const{for(auto& E:Enemies)if(IsValid(E)&&!E->bDead)return true;for(const auto& P:People)if(IsValid(P.Value)&&!P.Value->bDead&&Hostile(P.Key))return true;return false;}
bool ALWCampaign76::CanApply(const TArray<FString>& E){
 int64 Cost=0;for(FString V:E)if(V.RemoveFromStart(TEXT("spend:")))Cost+=FCString::Atoi(*V);
 if(Player->Money<Cost){Player->Notify(TEXT("Not enough credits."));return false;}
 // GiveItem is transactional. Probe a copy and restore it before committing the
 // whole narrative action, so a full pack cannot swallow a one-time reward.
 TGuardValue<TArray<FLWItemInstance>> Probe(Player->Inventory,Player->Inventory);
 for(FString V:E)if(V.RemoveFromStart(TEXT("give:"))){FString Item,Count;if(V.Split(TEXT(":"),&Item,&Count)&&!Player->GiveItem(FName(*Item),FCString::Atoi(*Count))){Player->Notify(TEXT("Make room in your inventory before collecting this reward."));return false;}}
 return true;
}
void ALWCampaign76::Effects(const TArray<FString>& E,FName Once){
 if(!Once.IsNone()&&State().Rewards.Contains(Once)){
  TArray<FString> Repeatable;for(const FString& V:E)if(!V.Contains(TEXT("+="))&&!V.Contains(TEXT("-="))&&!V.StartsWith(TEXT("give:"))&&!V.StartsWith(TEXT("spend:"))&&!V.StartsWith(TEXT("credits_reward:"))&&V!=TEXT("execute_ally"))Repeatable.Add(V);
  LWCampaign76::Apply(State(),Repeatable);Sync();AfterEffect();return;
 }
 // Costs are checked by the action/choice before any effects are committed.
 TArray<FString> Flags;for(FString V:E){if(V.RemoveFromStart(TEXT("spend:"))){Player->Money-=FCString::Atoi(*V);continue;}
  if(V.RemoveFromStart(TEXT("credits_reward:"))){Player->Money+=FCString::Atoi(*V);continue;}
  if(V==TEXT("execute_ally")){const FName Names[]={TEXT("mara"),TEXT("della"),TEXT("hannah"),TEXT("ada"),TEXT("rook"),TEXT("ivo"),TEXT("tessa"),TEXT("vera")};int K=State().Values.FindRef(TEXT("condemned"))-1;if(K>=0&&K<8){auto* N=People.FindRef(Names[K]).Get();PersonDied(Names[K]);if(N&&!N->bDead){World->Sound(TEXT("RevolverFire"),Player->GetActorLocation()+FVector(0,0,55),.8f);FDamageEvent Damage;N->Die(Damage,Player,150,Player->Controller);}}continue;}
  if(V.RemoveFromStart(TEXT("give:"))){FString Item,Count;if(V.Split(TEXT(":"),&Item,&Count))Player->GiveItem(FName(*Item),FCString::Atoi(*Count));continue;}Flags.Add(V);}
 LWCampaign76::Apply(State(),Flags);if(!Once.IsNone())State().Rewards.Add(Once);State().Revision++;Sync();AfterEffect();Player->RequestSave40();
}
bool ALWCampaign76::Use(FName Id){
 if(!Player||!Player->CanAct()||State().Paused)return false;Sync();const auto* S=LWCampaign76::Stage(State().Stage);if(!S)return false;
 const auto* A=S->Actions.FindByPredicate([&](const auto& X){return X.Id==Id;});if(!A||!LWCampaign76::Meets(State(),A->Needs))return false;
 const bool Talk=A->Mesh.IsNone();if(!Talk&&State().Rewards.Contains(Key76(TEXT("action_"),Id))&&A->Next.IsNone()&&A->Scene.IsNone())return false;
 if(FVector::Dist(Player->GetActorLocation(),At(A->At+FVector(0,0,80)))>550)return false;
 if(Id==TEXT("treat_lena")&&!State().Values.FindRef(TEXT("clinic"))&&!State().Values.FindRef(TEXT("medical_ready"))&&Player->CountSupply(TEXT("medkit"))<1){Player->Notify(TEXT("A medkit or prepared clinic supplies are needed."),4);return true;}
 if(A->Work>0){WorkAction=Id;WorkSeconds=0;State().Work77=Id;State().WorkSeconds77=0;if(Production)Production->Work(Id,0,A->Work);Player->Notify(A->Label+TEXT("... Stay close to continue."),A->Work+1);return true;}
 CompleteAction(*A);return true;
}
void ALWCampaign76::CompleteAction(const FLWC76Action& A){
 State().Work77=NAME_None;State().WorkSeconds77=0;if(Production)Production->StopPerformance();
 if(!State().Rewards.Contains(Key76(TEXT("action_"),A.Id))&&!CanApply(A.Effects)){WorkAction=NAME_None;WorkSeconds=0;return;}
 if(A.Id==TEXT("canada_sleep")&&!Player->SleepInBed(4)){WorkAction=NAME_None;WorkSeconds=0;return;}
 if(A.Id==TEXT("treat_lena")){if(!State().Values.FindRef(TEXT("clinic"))&&!State().Values.FindRef(TEXT("medical_ready"))&&!Player->ConsumeSupply(TEXT("medkit"),1)){WorkAction=NAME_None;return;}if(auto* Lena=People.FindRef(TEXT("lena")).Get())Lena->Health=150;}
 Effects(A.Effects,Key76(TEXT("action_"),A.Id));World->Sound(TEXT("Click"),At(A.At),.35f);WorkAction=NAME_None;WorkSeconds=0;
 if(!A.Scene.IsNone())Show(A.Scene,A.Next);else if(!A.Next.IsNone())SetStage(A.Next);else {Refresh();Route();}
 Player->CaptureMission37(TEXT("campaign76"));
}
bool ALWCampaign76::Talk(ALWResident* N){auto* Person=Cast<ALWCampaignPerson76>(N);if(!Person||Person->Campaign!=this)return false;
 if(!State().Scene.IsNone()&&!SceneOpen){Resume();return true;}
 if(Use(Key76(TEXT("talk_"),Person->Person)))return true;Player->Notify(Person->DisplayName+TEXT(": ")+(LWCampaign76::Stage(State().Stage)?LWCampaign76::Stage(State().Stage)->Goal:FString()),5);return true;
}
void ALWCampaign76::PersonDied(FName Id){if(!LWCampaign76::Alive(State(),Id))return;if(Id.ToString().StartsWith(TEXT("crowd77_")))State().Values.FindOrAdd(TEXT("civilian_losses"))++;State().Values.Add(Key76(TEXT("dead_"),Id),1);if(Id==TEXT("richardson"))State().Values.Add(TEXT("richardson_dead"),1);State().Health.Add(Id,0);State().Revision++;State().Journal.Add((LWCampaign76::Person(Id)?LWCampaign76::Person(Id)->Name:Id.ToString())+TEXT(" died."));Refresh();Player->RequestSave40();}
void ALWCampaign76::Tick(float Dt){
 Super::Tick(Dt);if(Player&&State().Paused)RestoreWeather();if(!Player||!World||!Player->bStarted||Player->bMenu||Player->OpeningMode||!State().Started||State().Paused||Player->Health<=0)return;
 const auto* S=LWCampaign76::Stage(State().Stage);if(!S)return;
 if(Production){Production->Director=this;if(State().Sailing77){Production->UpdateSailing(Dt);return;}}
 Sync();
 if(FVector::Dist2D(Player->GetActorLocation(),At(FVector::ZeroVector))>11000){if(!Actors.IsEmpty())Clear();return;}
 Spawn();if(SpawnedStage!=S->Id)return;if(Production)Production->Update(Dt);TickStage(Dt);
 if(Heater&&S->Site==TEXT("bellwether"))Heater->SetIntensity(State().Values.FindRef(TEXT("heater_fixed"))&&(State().Values.FindRef(TEXT("heat"))||State().Stage==TEXT("bell_warm"))?4500:200);
 if(S->Site==TEXT("bellwether")&&State().Values.FindRef(TEXT("heater_fixed"))&&!RoomSound)RoomSound=World->Sound(TEXT("Generator"),At(FVector(-710,430,100)),.12f);
 if(SceneOpen){Player->bTrigger=Player->bAim=Player->bSprint=false;Player->GetCharacterMovement()->StopMovementImmediately();BeatWait=FMath::Max(0.f,BeatWait-Dt);SceneClock+=Dt;return;}
 if(Player->IsUIOpen())return;
 if(!State().Scene.IsNone()){if(FVector::Dist(Player->GetActorLocation(),At(FVector(0,0,92)))<750)Show(State().Scene,State().AfterScene);return;}
 const FName ArrivalKey=Key76(TEXT("arrived_"),S->Id);
 if(!S->Arrival.IsNone()&&!State().Decisions.Contains(ArrivalKey)&&FVector::Dist(Player->GetActorLocation(),At(FVector(0,0,92)))<750){State().Decisions.Add(ArrivalKey);Show(S->Arrival);return;}
 if(!WorkAction.IsNone()){
  const auto* A=S->Actions.FindByPredicate([&](const auto& X){return X.Id==WorkAction;});
  if(!A||!LWCampaign76::Meets(State(),A->Needs)||FVector::Dist(Player->GetActorLocation(),At(A->At+FVector(0,0,80)))>400){WorkAction=State().Work77=NAME_None;WorkSeconds=State().WorkSeconds77=0;if(Production)Production->StopPerformance();Player->Notify(TEXT("Work interrupted."));}
  else {if(!Production||Production->WorkReady)WorkSeconds+=Dt;State().WorkSeconds77=WorkSeconds;if(Production)Production->Work(A->Id,WorkSeconds,A->Work);if(WorkSeconds>=A->Work){CompleteAction(*A);return;}}
 }
 if(S->Id==TEXT("bell_attack")){
  State().EncounterSeconds+=Dt;
  if(!State().Values.FindRef(TEXT("lena_treated"))&&LWCampaign76::Alive(State(),TEXT("lena"))){if(auto* N=People.FindRef(TEXT("lena")).Get()){N->Health-=Dt*(State().Values.FindRef(TEXT("clinic"))>=2?.45f:.8f);State().Health.Add(TEXT("lena"),N->Health);if(N->Health<=0){FDamageEvent E;N->TakeDamage(1,E,nullptr,nullptr);}}}
  if(State().EncounterSeconds>12&&!State().Values.FindRef(TEXT("warning_records"))){Effects({TEXT("warning_records")});Player->Notify(TEXT("Tessa: They are going for the intake book!"),6);}
  if(!Fighting()){
   if(!State().Values.FindRef(TEXT("bell_condition")))State().Values.Add(TEXT("bell_condition"),State().Values.FindRef(TEXT("bell_damage"))||!State().Values.FindRef(TEXT("fire_contained"))?2:1);
   // A surviving, untreated casualty remains a playable rescue after the shooting stops.
   if(!LWCampaign76::Alive(State(),TEXT("lena"))||State().Values.FindRef(TEXT("lena_treated")))SetStage(S->Next);
  }
 }else if(!S->Next.IsNone()&&!S->Needs.IsEmpty()&&LWCampaign76::Meets(State(),S->Needs))SetStage(S->Next);
}

void ALWCampaignNode76::Use(ALWCharacter* P){if(Campaign&&Campaign->Player==P)Campaign->Use(Action);}
FString ALWCampaignNode76::Prompt()const{return Usable?TEXT("[E] ")+Label:FString();}
void ALWCampaignPerson76::Tick(float Dt){
 if(Campaign&&!Campaign->SceneOpen&&!Campaign->State().Paused&&!bDead&&Campaign->Hostile(Person)){if(Gun)Gun->SetVisibility(true);ALWZombie::Tick(Dt);return;}
 ACharacter::Tick(Dt);if(!Campaign||!Campaign->Player||bDead)return;auto* P=Campaign->Player.Get();if(P->bMenu||P->Health<=0||Campaign->State().Paused)return;
 FireTime=FMath::Max(0.f,FireTime-Dt);SubtitleTime=FMath::Max(0.f,SubtitleTime-Dt);
 if(MoveTime>0){MoveTime-=Dt;const float Remaining=FVector::Dist2D(GetActorLocation(),Mark);const float Tolerance=Campaign->Production&&Campaign->Production->Performer.Get()==this?8.f:20.f;if(Remaining>Tolerance){
  // Companion formation navigation stops a metre away. Performances need the
  // final measured approach; CharacterMovement still sweeps against obstacles.
  if(!NavigateCompanion(Mark,110,Dt)&&Remaining<145){const FVector Direction=(Mark-GetActorLocation()).GetSafeNormal2D();SetActorRotation(FMath::RInterpTo(GetActorRotation(),Direction.Rotation(),Dt,5));AddMovementInput(Direction,FMath::Clamp(Remaining/65.f,.2f,1.f));}
 }else {MoveTime=0;GetCharacterMovement()->StopMovementImmediately();}}
 else if(Campaign->SceneOpen&&!ActorHasTag(TEXT("DirectedMotion77"))&&(!LifeAnimation||LifeAnimation->PerformancePose77==0)){const FVector Dir=P->GetActorLocation()-GetActorLocation();SetActorRotation(FMath::RInterpTo(GetActorRotation(),FRotator(0,Dir.Rotation().Yaw,0),Dt,3));}
 if(!Campaign->SceneOpen&&Campaign->Fighting()&&(Person==TEXT("ivo")||Person==TEXT("tessa")||Person==TEXT("imani")||Person==TEXT("rook")||Person==TEXT("hannah"))){if(Gun)Gun->SetVisibility(true);TickCompanionThreat(Dt,P);}
}
float ALWCampaignPerson76::TakeDamage(float D,const FDamageEvent& E,AController* I,AActor* C){
 if(bDead||D<=0||!Campaign||Campaign->SceneOpen||Campaign->State().Sailing77||(Person==TEXT("richardson")&&!Campaign->Hostile(Person)))return 0;BloodHit(E,C,D);Health-=D;
 Campaign->State().Health.Add(Person,FMath::Max(0.f,Health));if(Health<=0){Campaign->PersonDied(Person);Die(E,C,D,I);}return D;
}
void ALWCampaignEnemy76::Tick(float Dt){
 if(Campaign&&(Campaign->SceneOpen||Campaign->State().Paused))return;
 // One search team targets the intake desk. It abandons the task if engaged;
 // records burn only if a living attacker actually reaches and works at the desk.
 if(Campaign&&!bDead&&Campaign->State().Stage==TEXT("bell_attack")&&(Index&1)==0&&Health>=MaximumHealth&&!Campaign->State().Values.FindRef(TEXT("intake_saved"))&&!Campaign->State().Values.FindRef(TEXT("intake_burned"))&&FVector::Dist2D(GetActorLocation(),Campaign->Player->GetActorLocation())>350){
  ACharacter::Tick(Dt);const FVector Goal=Campaign->At(FVector(-170,80,92));if(FVector::Dist2D(GetActorLocation(),Goal)<190){Sabotage+=Dt;if(Sabotage>6){Campaign->Effects({TEXT("intake_burned"),TEXT("bell_damage+=1")});Campaign->Refresh();Campaign->Player->Notify(TEXT("The search team destroyed the intake book."),5);}}
  else {PathClock-=Dt;if(PathClock<=0){FindRoute(Goal);PathClock=1;}FVector Target=Path.Num()?Path[0]:Goal;if(FVector::Dist2D(GetActorLocation(),Target)<90&&Path.Num())Path.RemoveAt(0);AddMovementInput((Target-GetActorLocation()).GetSafeNormal2D());}return;
 }
 if(Campaign&&!bDead&&(Index%3)==0&&FVector::Dist2D(GetActorLocation(),Campaign->Player->GetActorLocation())>750){
  ALWCampaignPerson76* Target=nullptr;float Best=1800;
  for(const auto& Pair:Campaign->People)if(auto* N=Pair.Value.Get();N&&!N->bDead&&Pair.Key!=TEXT("richardson")&&!Campaign->Hostile(Pair.Key)){float D=FVector::Dist2D(GetActorLocation(),N->GetActorLocation());if(D<Best){FHitResult H;FCollisionQueryParams Q(NAME_None,false,this);GetWorld()->LineTraceSingleByChannel(H,GetActorLocation()+FVector(0,0,35),N->GetActorLocation()+FVector(0,0,20),ECC_Visibility,Q);if(H.GetActor()==N){Best=D;Target=N;}}}
  if(Target){ACharacter::Tick(Dt);AttackCooldown-=Dt;SetActorRotation(FRotator(0,(Target->GetActorLocation()-GetActorLocation()).Rotation().Yaw,0));
   if(AttackCooldown<=0){const FVector Muzzle=ALWWeaponEffect::GunMuzzle(this,Gun);const FVector Dir=ALWWeaponEffect::EnemyAim(Muzzle,Target->GetActorLocation()+FVector(0,0,20),0,Target->GetVelocity().Size2D(),1);FHitResult H;FCollisionQueryParams Q(NAME_None,false,this);GetWorld()->LineTraceSingleByChannel(H,Muzzle,Muzzle+Dir*3000,ECC_Visibility,Q);if(H.GetActor()==Target)UGameplayStatics::ApplyPointDamage(Target,7,Dir,H,nullptr,this,nullptr);ALWWeaponEffect::Gunfire(this,World,Muzzle,H.bBlockingHit?H.ImpactPoint:Muzzle+Dir*3000);AttackCooldown=.95f;}return;
  }
 }
 Super::Tick(Dt);
}
float ALWCampaignEnemy76::TakeDamage(float D,const FDamageEvent& E,AController* I,AActor* C){if(Campaign&&Campaign->SceneOpen)return 0;const float Applied=Super::TakeDamage(D,E,I,C);if(Campaign){Campaign->State().Health.Add(EnemyHealth76(Index),FMath::Max(0.f,Health));if(bDead)Campaign->State().Defeated.Add(Index);}return Applied;}
