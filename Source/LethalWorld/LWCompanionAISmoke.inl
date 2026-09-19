// Included inside BuildCompanionNavSmoke after its navigation steps. Uses its Add helper.
// Assertions below observe World::Tick music selection and actual companion damage.
{
 struct FAIState {
  TWeakObjectPtr<ALWChunk> Room,Wall;
  TWeakObjectPtr<ALWZombie> Enemy;
  TWeakObjectPtr<ALWResident> Crew,Peaceful;
 };
 auto AI=MakeShared<FAIState>();
 Add(TEXT("AI/music isolated setup"),2,[this,AI](ALWCharacter& P){
  P.RPG.Crew.Empty();P.World->EnableEncounters=false;P.Health=10000;
  LWV2Teleport(P,FVector(20000,20000,5091));P.GetCharacterMovement()->DisableMovement();
  auto* Room=GetWorld()->SpawnActor<ALWChunk>();AI->Room=Room;
  Room->Box(P.World,TEXT("Concrete"),FVector(20000,20000,4988),FVector(4000,4000,24));
  auto* Z=GetWorld()->SpawnActor<ALWZombie>(FVector(20800,20000,5091),FRotator::ZeroRotator);AI->Enemy=Z;
  Z->SetActorTickEnabled(false);Z->GetCharacterMovement()->DisableMovement();Z->Health=1000;Z->Alert=0;
  Z->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
 },[this](ALWCharacter& P){Check(P.World->MusicState==TEXT("ExploreMusic"),TEXT("idle enemy leaves actual music in exploration"));});
 Add(TEXT("visible combat music positive control"),.5,[AI](ALWCharacter& P){
  AI->Enemy->Alert=12;AI->Enemy->Interest=P.GetActorLocation();
 },[this](ALWCharacter& P){Check(P.World->MusicState==TEXT("CombatMusic"),TEXT("visible engaged enemy selects actual combat music"));});
 Add(TEXT("blocked LOS grace"),.25,[this,AI](ALWCharacter& P){
  auto* Wall=GetWorld()->SpawnActor<ALWChunk>();AI->Wall=Wall;
  Wall->Box(P.World,TEXT("Concrete"),FVector(20400,20000,5200),FVector(40,1800,400));
 },[this,AI](ALWCharacter& P){
  Check(!LWThreatAwareness::Visible(&P,AI->Enemy.Get()),TEXT("wall physically blocks enemy LOS"));
  Check(P.World->MusicState==TEXT("CombatMusic"),TEXT("brief occlusion retains combat music"));
 });
 Add(TEXT("blocked LOS expires combat music"),2,[](ALWCharacter& P){},[this,AI](ALWCharacter& P){
  Check(AI->Enemy->Alert>0,TEXT("occluded enemy remains alerted"));
  Check(P.World->MusicState==TEXT("ExploreMusic"),TEXT("blocked LOS cannot sustain actual combat music"));
 });
 Add(TEXT("LOS restored combat music"),.5,[AI](ALWCharacter& P){AI->Wall->Destroy();},
  [this](ALWCharacter& P){Check(P.World->MusicState==TEXT("CombatMusic"),TEXT("restored LOS re-enters combat music"));});
 Add(TEXT("different floor with open LOS"),2,[AI](ALWCharacter& P){
  AI->Enemy->SetActorLocation(FVector(20800,20000,5491));
  // No wall/slab between them: isolates vertical relevance from the LOS filter.
 },[this,AI](ALWCharacter& P){
  Check(LWThreatAwareness::Visible(&P,AI->Enemy.Get()),TEXT("upper-floor fixture retains open LOS"));
  Check(P.World->MusicState==TEXT("ExploreMusic"),TEXT("different floor cannot sustain actual combat music"));
 });
 Add(TEXT("same floor restored"),.5,[AI](ALWCharacter& P){AI->Enemy->SetActorLocation(FVector(20800,20000,5091));},
  [this](ALWCharacter& P){Check(P.World->MusicState==TEXT("CombatMusic"),TEXT("same-floor return restores combat music"));});
 Add(TEXT("unrelated alert music"),2,[AI](ALWCharacter& P){AI->Enemy->Interest=P.GetActorLocation()+FVector(2500,0,0);},
  [this](ALWCharacter& P){Check(P.World->MusicState==TEXT("ExploreMusic"),TEXT("unrelated noise alert does not sustain combat music"));});
 Add(TEXT("peaceful resident targeting and friendly obstruction"),0,[this,AI](ALWCharacter& P){
  auto* N=GetWorld()->SpawnActor<ALWResident>(FVector(20000,20300,5091),FRotator::ZeroRotator);AI->Crew=N;
  N->ConfigureResident(TEXT("ai_smoke_crew"),TEXT("recruit"),TEXT("Guard"),0);
  auto* Friend=GetWorld()->SpawnActor<ALWResident>(FVector(20400,20300,5091),FRotator::ZeroRotator);AI->Peaceful=Friend;
  Friend->ConfigureResident(TEXT("ai_smoke_peaceful"),TEXT("civilian"),TEXT("Civilian"),0);
  for(auto* R:{N,Friend}){R->SetActorTickEnabled(false);R->GetCharacterMovement()->DisableMovement();}
  Friend->Alert=12;Friend->Interest=P.GetActorLocation();Friend->bAggressive=true;
  AI->Enemy->Alert=0;
  const float FriendlyHealth=Friend->Health;
  N->CompanionThreat=Friend;N->ThreatScanClock=1;N->FireTime=0;
  N->TickCompanionThreat(.01f,&P);
  Check(!N->CompanionThreat.IsValid()&&Friend->Health==FriendlyHealth,TEXT("cached peaceful resident is dropped without shooting"));
  N->ThreatScanClock=0;N->TickCompanionThreat(.01f,&P);
  Check(!N->CompanionThreat.IsValid()&&Friend->Health==FriendlyHealth,TEXT("scan rejects alerted peaceful resident"));
  auto* Z=AI->Enemy.Get();Z->SetActorLocation(FVector(20800,20300,5091));Z->Interest=P.GetActorLocation();Z->Alert=12;
  const float EnemyHealth=Z->Health;
  N->CompanionThreat=Z;N->ThreatScanClock=1;N->FireTime=0;N->TickCompanionThreat(.01f,&P);
  Check(Friend->Health==FriendlyHealth&&Z->Health==EnemyHealth,TEXT("friendly crossing muzzle prevents damage to either actor"));
  Friend->SetActorLocation(FVector(20400,21000,5091));N->ThreatScanClock=0;N->FireTime=0;N->TickCompanionThreat(.01f,&P);
  Check(Z->Health<EnemyHealth&&Friend->Health==FriendlyHealth,TEXT("clear hostile shot deals damage while peaceful resident stays unharmed"));
  Z->Alert=0;N->CompanionThreat.Reset();
 });
 Add(TEXT("peaceful resident music"),2,[](ALWCharacter& P){},[this,AI](ALWCharacter& P){
  Check(P.World->MusicState==TEXT("ExploreMusic"),TEXT("alerted peaceful resident cannot trigger combat music"));
  AI->Crew->Destroy();AI->Peaceful->Destroy();AI->Enemy->Destroy();AI->Room->Destroy();
  P.GetCharacterMovement()->SetMovementMode(MOVE_Walking);
 });
}
