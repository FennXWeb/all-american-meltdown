#include "LWEncounter.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWZombie.h"
#include "LWResident.h"
#include "LWNavigation.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Misc/Crc.h"
FLWEncounterRecord* ALWEncounterScene::Record()const{return World?World->Encounters.Records.Find(RecordId):nullptr;}
const FLWEncounterDefinition* ALWEncounterScene::Definition()const{auto* R=Record();return R?ULWEncounterCatalog::Get()->Find(R->Type):nullptr;}
void ALWEncounterScene::Initialize(ALWWorld* W,FName Id){
 World=W;RecordId=Id;Kind=ELWObjectKind::Container;auto* D=Definition();auto* R=Record();if(!D||!R){Destroy();return;}
 const TCHAR* Main[]={TEXT("Crate"),TEXT("WorkbenchV4"),TEXT("Crate"),TEXT("CabinetV3"),TEXT("RadioV4"),TEXT("Crate"),TEXT("Crate"),TEXT("WorkbenchV4")};Body->SetStaticMesh(W->Mesh(Main[int(D->Setting)]));
 auto Prop=[&](FName Mesh,FVector At,FVector Scale=FVector(1),FName Mat=NAME_None){FVector Global=GetActorTransform().TransformPosition(At);At.Z+=W->HeightAt(FVector2D(Global))-GetActorLocation().Z;Part(Mesh,At,Scale,FRotator::ZeroRotator,Mat);};
 auto Box=[&](FVector At,FVector Size,FName Mat){Prop(TEXT("Cube"),At,Size/100,Mat);};
 switch(D->Setting){
 case ELWEncounterSetting::Camp:
  Prop(TEXT("MotelBedV3"),FVector(-310,220,0),FVector(.75));Prop(TEXT("ChairV3"),FVector(220,-150,0));Box(FVector(-230,320,250),FVector(600,420,8),TEXT("Cloth"));for(int X:{-480,20})for(int Y:{150,480})Box(FVector(X,Y,125),FVector(8,8,250),TEXT("Steel"));break;
 case ELWEncounterSetting::Motor:
  Prop(TEXT("Vehicle_sedan_V9"),FVector(-200,340,0));for(int X:{-332,-68})for(int Y:{255,425})Prop(TEXT("SedanWheelV5"),FVector(X,Y,32));Prop(TEXT("Barrel"),FVector(320,280,0));break;
 case ELWEncounterSetting::Checkpoint:
  for(int Side:{-1,1}){Box(FVector(Side*410,250,60),FVector(310,55,120),TEXT("Concrete"));Box(FVector(Side*410,250,126),FVector(280,58,8),TEXT("Lane"));}Prop(TEXT("ChairV3"),FVector(-200,170,0));break;
 case ELWEncounterSetting::Medical:
  Prop(TEXT("ClinicBedV3"),FVector(-270,290,0));Prop(TEXT("DinerTableV9"),FVector(280,300,0));Box(FVector(300,300,105),FVector(50,40,50),TEXT("Bone"));Box(FVector(300,277,105),FVector(10,2,36),TEXT("Red"));Box(FVector(300,277,105),FVector(32,2,9),TEXT("Red"));break;
 case ELWEncounterSetting::Radio:
  Box(FVector(-330,300,260),FVector(12,12,520),TEXT("Steel"));for(int I=0;I<3;I++)Box(FVector(-330,300,320+I*80),FVector(220-I*40,10,10),TEXT("Steel"));Box(FVector(-330,300,15),FVector(130,130,30),TEXT("Concrete"));Prop(TEXT("Crate"),FVector(300,300,0));break;
 case ELWEncounterSetting::Grave:
  for(int I=0;I<3;I++){Box(FVector(-360+I*210,330,55),FVector(12,12,110),TEXT("DoorWoodV7"));Box(FVector(-360+I*210,330,85),FVector(65,12,12),TEXT("DoorWoodV7"));Box(FVector(-360+I*210,230,4),FVector(140,230,8),TEXT("Earth"));}break;
 case ELWEncounterSetting::Supply:
  for(int I=0;I<4;I++)Prop(I%2?TEXT("Barrel"):TEXT("Crate"),FVector(-360+I*210,320,0),FVector(.8+I*.08));break;
 case ELWEncounterSetting::Utility:
  Box(FVector(-300,320,80),FVector(210,150,160),TEXT("Steel"));for(int I=0;I<4;I++)Box(FVector(-300,241,35+I*26),FVector(160,5,8),TEXT("Rubber"));Prop(TEXT("Barrel"),FVector(340,320,0));break;
 }
 Beacon=NewObject<UPointLightComponent>(this);Beacon->SetupAttachment(Root);Beacon->SetRelativeLocation(FVector(-80,0,170));Beacon->SetIntensity(2800);Beacon->SetAttenuationRadius(1100);Beacon->SetCastShadows(false);Beacon->SetLightColor(D->Task==ELWEncounterTask::Hazard?FLinearColor(1,.15,.03):FLinearColor(1,.64,.27));Beacon->RegisterComponent();
 Box(FVector(-80,0,85),FVector(6,6,170),TEXT("Steel"));Box(FVector(-80,0,171),FVector(20,20,18),TEXT("Bone"));
 if(D->Task!=ELWEncounterTask::Cache&&D->Task!=ELWEncounterTask::Ambush&&D->Task!=ELWEncounterTask::Hazard){
  FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
  Witness=GetWorld()->SpawnActor<ALWResident>(R->EscortPosition,GetActorRotation(),Params);if(Witness){Witness->ConfigureResident(FName(*(Id.ToString()+TEXT("_witness"))),TEXT("encounter"),D->Title,FCrc::StrCrc32(*Id.ToString())%3);Witness->Shop=this;Witness->SetActorTickEnabled(false);Witness->GetCharacterMovement()->DisableMovement();Owned.Add(Witness);}
 }
 if(R->Stage<2&&(R->EnemiesSpawned||(D->Enemies>0&&D->Task!=ELWEncounterTask::Toll)))SpawnEnemies();
}
FString ALWEncounterScene::Prompt()const{auto* D=Definition();auto* R=Record();if(!D||!R)return TEXT("");return R->Stage==2?TEXT("[E] ")+D->Title+TEXT(" / SUPPLIES"):TEXT("[E] ")+D->Title;}
void ALWEncounterScene::Use(ALWCharacter* P){if(!P||!P->CanAct())return;auto* R=Record();if(!R)return;if(R->Stage==2&&World->Containers.Contains(RecordId)){P->OpenContainer(this);return;}P->ClosePanels();P->EncounterSpeaker=this;P->RPGPanel=4;P->CancelReload();P->bAim=P->bSprint=false;P->SetMenuInput(true);if(IsValid(Witness))Witness->SetActorRotation((P->GetActorLocation()-Witness->GetActorLocation()).Rotation());Dialogue(P);}
void ALWEncounterScene::Dialogue(ALWCharacter* P){auto* D=Definition();auto* R=Record();if(!D||!R)return;P->DialogueChoices.Empty();P->DialogueText=R->Stage>=2?R->Result:R->Stage==1?Objective():D->Intro;
 if(R->Stage==0&&D->Task==ELWEncounterTask::Trade)P->DialogueText+=FString::Printf(TEXT(" Offer: %d %s and %d credits."),D->RewardCount,*LWItems::Def(D->RewardItem).DisplayName.ToString(),D->Credits);
 if(R->Stage>=2){P->DialogueChoices.Add(TEXT("Leave."));return;}
 if(R->Stage==1){P->DialogueChoices={TEXT("Keep going."),TEXT("Abandon this encounter.")};return;}
 FString Cost=D->Cost>0?(D->CostItem.IsNone()?FString::Printf(TEXT(" [%d credits]"),D->Cost):FString::Printf(TEXT(" [%d %s]"),D->Cost,*LWItems::Def(D->CostItem).DisplayName.ToString())):TEXT("");
 const TCHAR* Actions[]={TEXT("Give them supplies"),TEXT("Help with the repair"),TEXT("Accept the offer"),TEXT("Hold the position"),TEXT("Get them out"),TEXT("Open the case"),TEXT("Investigate the setup"),TEXT("Walk with them"),TEXT("Take the bounty"),TEXT("Monitor the transmission"),TEXT("Stay a while"),TEXT("Make the area safe"),TEXT("Recover usable parts"),TEXT("Pay the fee"),TEXT("Keep watch")};
 P->DialogueChoices.Add(FString(Actions[int(D->Task)])+Cost);
 if(D->Task==ELWEncounterTask::Repair||D->Task==ELWEncounterTask::Hazard)P->DialogueChoices.Add(TEXT("[INTELLECT 5] Improvise a repair."));
 else if(D->Task==ELWEncounterTask::Toll)P->DialogueChoices.Add(TEXT("[PRESENCE 5] Talk them down."));
 else if(D->Task==ELWEncounterTask::Salvage)P->DialogueChoices.Add(TEXT("[STRENGTH 5] Shift the heavy pieces."));
 else P->DialogueChoices.Add(TEXT("Mark this location on my map."));
 P->DialogueChoices.Add(D->Task==ELWEncounterTask::Toll?TEXT("Refuse and fight."):D->Task==ELWEncounterTask::Aid||D->Task==ELWEncounterTask::Trade?TEXT("Threaten them for the supplies."):TEXT("What is the reward?"));
 P->DialogueChoices.Add(TEXT("Leave it alone."));
 if(Witness){Witness->Say(D->Intro);Witness->SubtitleTime=0;}else World->Sound(D->Cue==TEXT("ZombieVoice")?FName(TEXT("Zombie")):D->Cue,GetActorLocation(),.6f);
}
bool ALWEncounterScene::Choose(ALWCharacter* P,int Choice){
 auto* D=Definition();auto* R=Record();if(!P||!D||!R||P->EncounterSpeaker!=this||P->Health<=0||FVector::Dist2D(P->GetActorLocation(),GetActorLocation())>600||Choice<0||Choice>=P->DialogueChoices.Num())return false;
 if(R->Stage>=2){P->ClosePanels();return true;}
 if(R->Stage==1){P->ClosePanels();if(Choice==1)Resolve(P,false,TEXT("You left before the work was done."));return true;}
 if(Choice==3){P->ClosePanels();Resolve(P,false,TEXT("You chose to leave it alone."));return true;}
 if(Choice==2&&D->Task!=ELWEncounterTask::Toll&&D->Task!=ELWEncounterTask::Aid&&D->Task!=ELWEncounterTask::Trade){P->DialogueText=FString::Printf(TEXT("For finishing the work: %d credits, %d XP, and %d %s. Supplies remain here for you to collect."),D->Credits,D->XP,D->RewardCount,*LWItems::Def(D->RewardItem).DisplayName.ToString());return true;}
 bool Skill=false;
 if(Choice==1){int Stat=D->Task==ELWEncounterTask::Toll?3:D->Task==ELWEncounterTask::Salvage?0:4;
 if(D->Task!=ELWEncounterTask::Repair&&D->Task!=ELWEncounterTask::Hazard&&D->Task!=ELWEncounterTask::Toll&&D->Task!=ELWEncounterTask::Salvage){P->SetWaypoint(FVector2D(R->Position));P->Notify(TEXT("LOCATION MARKED"));return true;}
 if(!P->RPG.Attributes.IsValidIndex(Stat)||P->RPG.Attributes[Stat]<5){P->Notify(TEXT("ATTRIBUTE TOO LOW"));return false;}Skill=true;}
 if(Choice==0&&D->Cost>0&&!R->Paid){if(D->CostItem.IsNone()){if(P->Money<D->Cost){P->Notify(TEXT("NOT ENOUGH CREDITS"));return false;}P->Money-=D->Cost;}else if(!P->ConsumeSupply(D->CostItem,D->Cost)){P->Notify(TEXT("MISSING SUPPLIES"));return false;}R->Paid=true;}
 R->Choice=Choice;R->Stage=1;R->Progress=Skill?D->WorkSeconds*.5f:0;P->ClosePanels();
 if(Choice==2){World->Encounters.Reputation=FMath::Max(-20,World->Encounters.Reputation-2);SpawnEnemies(FMath::Max(2,D->Enemies));if(Witness)Witness->Say(TEXT("You picked the wrong people."));}
 else if(D->Task==ELWEncounterTask::Aid||D->Task==ELWEncounterTask::Trade||D->Task==ELWEncounterTask::Cache||D->Task==ELWEncounterTask::Toll){Resolve(P,true,D->Outcome,D->Task==ELWEncounterTask::Aid?1:0);return true;}
 else if(D->Enemies>0)SpawnEnemies();
 if(D->Task==ELWEncounterTask::Signal)World->Noise(GetActorLocation(),4500);P->Notify(Objective(),6);P->RequestSave40();return true;
}
void ALWEncounterScene::SpawnEnemies(int Override){auto* R=Record();auto* D=Definition();if(!R||!D||!Enemies.IsEmpty())return;
 int Count=R->EnemiesSpawned?R->EnemyHealth.Num():FMath::Clamp(Override?Override:D->Enemies,0,10);bool Restore=R->EnemiesSpawned;
 if(!Restore){R->EnemyHealth.Init(100,Count);R->EnemyPositions.Init(FVector::ZeroVector,Count);R->EnemiesSpawned=true;}
 Enemies.SetNum(Count);FRandomStream Rand(FCrc::StrCrc32(*RecordId.ToString()));
 for(int I=0;I<Count;I++){float Angle=I*2*PI/FMath::Max(1,Count)+Rand.FRand()*.3f;FVector At=GetActorLocation()+FVector(FMath::Cos(Angle)*1100,FMath::Sin(Angle)*1100,0);At.Z=World->HeightAt(FVector2D(At))+100;if(Restore)At=R->EnemyPositions[I];if(R->EnemyHealth[I]<=0)continue;
 FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;auto* Z=GetWorld()->SpawnActor<ALWZombie>(At,FRotator::ZeroRotator,Params);if(!Z)continue;
 World->ZombieCount++;Z->PersistentId=FCrc::StrCrc32(*(RecordId.ToString()+FString::FromInt(I)));Z->ConfigureKind(ELWEnemyKind((Override||R->Choice==2)?1:FMath::Clamp(D->EnemyKind,0,2)));Z->Health=Restore?R->EnemyHealth[I]:Z->Health;Z->Home=Z->Interest=GetActorLocation();Z->Alert=20;Enemies[I]=Z;}
 Snapshot();
}
int ALWEncounterScene::LivingEnemies()const{int N=0;for(ALWZombie* Z:Enemies)N+=IsValid(Z)&&!Z->bDead;return N;}
void ALWEncounterScene::Snapshot(){auto* R=Record();if(!R)return;for(int I=0;I<Enemies.Num();I++)if(R->EnemyHealth.IsValidIndex(I)){auto* Z=Enemies[I].Get();R->EnemyHealth[I]=IsValid(Z)&&!Z->bDead?Z->Health:0;if(IsValid(Z))R->EnemyPositions[I]=Z->GetActorLocation();}if(IsValid(Witness))R->EscortPosition=Witness->GetActorLocation();}
void ALWEncounterScene::Resolve(ALWCharacter* P,bool Success,const FString& Result,int Rep){auto* D=Definition();auto* R=Record();if(!D||!R||R->Stage>=2)return;R->Stage=Success?2:3;R->Result=Result;
 if(Success&&!R->Rewarded){R->Rewarded=true;World->Sound(TEXT("EncounterResolved"),GetActorLocation(),.5f);World->Encounters.Completed.Add(D->Id);World->Encounters.Reputation=FMath::Clamp(World->Encounters.Reputation+Rep,-20,20);P->Money+=FMath::Max(0,D->Credits);P->GainXP(FMath::Max(0,D->XP));
 FLWContainerRecord Loot;Loot.Id=RecordId;Loot.Context=TEXT("encounter");Loot.Position=GetActorLocation();if(D->RewardCount>0&&!D->RewardItem.IsNone()){auto Item=LWItems::Make(D->RewardItem,FMath::Clamp(D->RewardCount,1,LWItems::Def(D->RewardItem).MaxStack));Item.Id=FGuid::NewDeterministicGuid(RecordId.ToString()+TEXT("reward"));LWItems::Place(Loot.Items,Item,12,12);}World->Containers.Add(RecordId,Loot);P->QuestEvent(TEXT("encounter"),D->Id);}
 if(Witness)Witness->Say(Success?TEXT("Thank you. We will remember this."):TEXT("Then we will find another way."));P->Notify(D->Title+TEXT("\n")+Result,7);if(P->EncounterSpeaker==this)P->ClosePanels();P->RequestSave40();
}
FString ALWEncounterScene::Objective()const{auto* D=Definition();auto* R=Record();if(!D||!R)return TEXT("");if(R->Stage>=2)return R->Result;
 if(LivingEnemies()>0)return FString::Printf(TEXT("%s: %d threats remaining. Keep the site intact: %d%%."),*D->Title,LivingEnemies(),FMath::RoundToInt(R->Integrity));
 if(D->Task==ELWEncounterTask::Escort)return TEXT("Stay beside the traveler until the next roadside rendezvous.");
 return FString::Printf(TEXT("%s: stay nearby and keep watch. %d / %d seconds."),*D->Title,FMath::FloorToInt(R->Progress),FMath::CeilToInt(D->WorkSeconds));}
void ALWEncounterScene::Tick(float Dt){
 AActor::Tick(Dt);auto* R=Record();auto* D=Definition();auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));if(!P||!R||!D||P->bMenu||!P->bStarted||P->Health<=0)return;
 if(P->EncounterSpeaker==this&&(FVector::Dist2D(P->GetActorLocation(),GetActorLocation())>600||R->Stage>=2))P->ClosePanels();
 float Dist=FVector::Dist2D(P->GetActorLocation(),GetActorLocation());TalkClock-=Dt;Pulse+=Dt;
 if(Beacon)Beacon->SetIntensity((D->Task==ELWEncounterTask::Hazard?2200+1800*FMath::Sin(Pulse*7):2400+350*FMath::Sin(Pulse*2))*(R->Stage>=2?.35f:1));
 if(Dist<1700&&!R->Seen){R->Seen=true;P->Notify(D->Title,5);World->Sound(D->Cue==TEXT("ZombieVoice")?FName(TEXT("Zombie")):D->Cue,GetActorLocation(),.55f);P->RequestSave40();}
 if(IsValid(Witness)){Witness->SubtitleTime=FMath::Max(0.f,Witness->SubtitleTime-Dt);if(Dist<900&&TalkClock<=0&&R->Stage<2){Witness->Say(R->Stage==1?TEXT("Stay close. We are not done yet."):D->Task==ELWEncounterTask::Memorial?TEXT("Would you sit with me a moment?"):TEXT("Over here. Could use a hand."));TalkClock=25;}}
 if(R->Stage==0&&D->Enemies>0&&D->Task!=ELWEncounterTask::Toll&&Dist<1500){R->Stage=1;P->Notify(Objective(),5);P->RequestSave40();}
 if(R->Stage!=1)return;
 Snapshot();if(R->Integrity<=0){Resolve(P,false,TEXT("The equipment was destroyed."));return;}if(Dist>3500&&D->Task!=ELWEncounterTask::Escort)return;
 int Living=LivingEnemies();if(D->Task==ELWEncounterTask::Defense&&Dist<1800&&!P->bSafehouse&&!P->Vehicle)R->Progress+=Dt;
 if((D->Task==ELWEncounterTask::Rescue||D->Task==ELWEncounterTask::Defense)&&Living){for(ALWZombie* Z:Enemies)if(IsValid(Z)&&!Z->bDead&&FVector::Dist2D(Z->GetActorLocation(),GetActorLocation())<550)R->Integrity-=Dt*1.5f;if(R->Integrity<=0){Resolve(P,false,TEXT("The site was overrun."));return;}}
 if(D->Task==ELWEncounterTask::Hazard){Think-=Dt;if(Pulse>8&&Dist<2000){Pulse=0;World->Sound(TEXT("EncounterWarning"),GetActorLocation(),.55f);}FVector Hazard=GetActorTransform().TransformPosition(FVector(340,320,0));if(FVector::Dist2D(P->GetActorLocation(),Hazard)<190&&Think<=0){Think=1;UGameplayStatics::ApplyDamage(P,5,nullptr,this,nullptr);}}
 if(D->Task==ELWEncounterTask::Escort&&Witness){
  float Gap=FVector::Dist2D(P->GetActorLocation(),Witness->GetActorLocation());if(Gap>750||P->Vehicle||P->bSafehouse)return;
  if(EscortRoute.IsEmpty())EscortRoute=LWNavigation::FindPath(FVector2D(Witness->GetActorLocation()),FVector2D(R->Goal),World->Seed);
  while(EscortRoute.IsValidIndex(RouteStep)&&FVector2D::Distance(FVector2D(Witness->GetActorLocation()),EscortRoute[RouteStep])<100)RouteStep++;
  if(!EscortRoute.IsValidIndex(RouteStep)){if(FVector::Dist2D(Witness->GetActorLocation(),R->Goal)<180)Resolve(P,true,D->Outcome,1);return;}
  FVector Goal(EscortRoute[RouteStep],World->HeightAt(EscortRoute[RouteStep])+100);FVector Dir=(Goal-Witness->GetActorLocation()).GetSafeNormal2D();FVector Next=Witness->GetActorLocation()+Dir*125*Dt;Next.Z=World->HeightAt(FVector2D(Next))+100;FHitResult Hit;Witness->SetActorLocation(Next,true,&Hit);Witness->SetActorRotation(Dir.Rotation());for(int I=3;I<7;I++)Witness->Parts[I]->SetRelativeRotation(FRotator(FMath::Sin(Pulse*8+(I%2)*PI)*20,0,0));R->EscortPosition=Witness->GetActorLocation();return;
 }
 if(Living>0)return;
 if(D->Enemies>0||R->Choice==2){if(D->Task!=ELWEncounterTask::Defense){Resolve(P,true,D->Outcome,R->Choice==2?0:1);return;}}
 if(D->Task!=ELWEncounterTask::Defense&&Dist<450&&!P->RPGPanel&&!P->bInventory&&!P->Vehicle&&!P->bSafehouse&&!P->bTrigger&&P->AttackTimer<=0)R->Progress+=Dt;
 if(R->Progress>=FMath::Max(1.f,D->WorkSeconds))Resolve(P,true,D->Outcome,1);
}
float ALWEncounterScene::TakeDamage(float Damage,const FDamageEvent&,AController*,AActor*){auto* R=Record();if(!R||R->Stage!=1)return 0;R->Integrity=FMath::Max(0.f,R->Integrity-Damage*.25f);return Damage;}
void ALWEncounterScene::EndPlay(const EEndPlayReason::Type Why){Snapshot();if(IsValid(Witness))Witness->Shop=nullptr;for(ALWZombie* Z:Enemies)if(IsValid(Z))Z->Destroy();for(AActor* A:Owned)if(IsValid(A))A->Destroy();Super::EndPlay(Why);}
