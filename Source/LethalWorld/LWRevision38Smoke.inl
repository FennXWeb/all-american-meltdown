#include "LWWeaponEffect.h"
#include "LWAudioCatalog.h"
#include "Components/AudioComponent.h"
void ALWGameMode::BuildRevision38Smoke(ALWCharacter& Initial){
 struct FState{TWeakObjectPtr<ALWStoryEnemy> Enemy;TWeakObjectPtr<ALWStoryPerson> Ally;float Health=0;};auto S=MakeShared<FState>();
 auto Add=[this](const TCHAR* Name,double Delay,FLWV2Action Begin,FLWV2Action End=FLWV2Action()){V2->Steps.Add({Name,Delay,120,MoveTemp(Begin),MoveTemp(End),[](ALWCharacter&){return true;}});};
 Add(TEXT("New-game loadout"),2,[this](ALWCharacter& P){
  P.NewGame();Check(P.Inventory.Num()==2,TEXT("only two starting items"));Check(P.Stash.IsEmpty()&&P.Money==0,TEXT("empty stash and zero credits"));
  auto* Rev=P.Inventory.FindByPredicate([](const auto& I){return I.Definition==TEXT("revolver");});
  Check(Rev&&Rev->Cylinder.Num()==6&&Rev->Cylinder[0]==1,TEXT("revolver starts loaded"));
  Check(P.Inventory.ContainsByPredicate([](const auto& I){return I.Definition==TEXT("crowbar")&&I.Slot==TEXT("Melee");}),TEXT("crowbar equipped in melee"));
  if(P.Story)P.Story->FinishScene();
 });
 Add(TEXT("Story compound and defense"),6,[this,S](ALWCharacter& P){
  auto* D=ALWStoryDirector::Ensure(&P);P.RPG.Story.Enabled=true;P.RPG.Story.Stage=7;D->Applied=-1;D->ApplyStage();if(D->InScene)D->FinishScene();
  auto* Before=D->Sites.FindRef(1).Get();P.RPG.Story.Stage=8;D->Applied=-1;D->ApplyStage();if(D->InScene)D->FinishScene();
  Check(Before&&D->Sites.FindRef(1).Get()==Before,TEXT("same compound survives objective transition"));
  const FVector Pos=D->At(1,FVector(0,-3200,120));LWV2Teleport(P,Pos);P.Health=10000;
  auto* Ally=D->People.FindRef(TEXT("story_mara")).Get();auto* Enemy=D->Enemies.Num()?D->Enemies[0].Get():nullptr;
  Check(Ally&&Enemy,TEXT("defense has friendly and raider"));if(!Ally||!Enemy)return;
  Ally->SetActorLocation(Pos+FVector(250,100,0));Enemy->SetActorLocation(Pos+FVector(1100,100,0));Enemy->SetActorTickEnabled(false);Enemy->Health=Enemy->MaximumHealth=1000;Enemy->Alert=20;Enemy->Interest=Pos;
  S->Ally=Ally;S->Enemy=Enemy;S->Health=Enemy->Health;
  const FVector Muzzle=ALWWeaponEffect::GunMuzzle(Enemy,Enemy->Gun);
  auto* Audio=P.World->Sound(TEXT("RifleFire"),Muzzle,1,1,true);Check(Audio&&Audio->Sound&&Audio->VolumeMultiplier>0,TEXT("enemy gunshot creates audible audio component"));
  if(Audio)Check(Audio->GetComponentLocation().Equals(Muzzle,1),TEXT("gunshot plays at muzzle"));
  ALWWeaponEffect::Gunfire(Enemy,P.World,Muzzle,Pos,false);int Effects=0;for(TActorIterator<ALWWeaponEffect> FX(GetWorld());FX;++FX)if(FX->Mode>=3)++Effects;Check(Effects>=2,TEXT("muzzle flash and trail spawned"));
 },[this,S](ALWCharacter& P){Check(S->Enemy.IsValid()&&S->Enemy->Health<S->Health-20,TEXT("story ally repeatedly damages raider without player attacking"));CaptureV2(TEXT("Revision38_Defense"));});
 Add(TEXT("Face and hair front"),4,[](ALWCharacter& P){P.BeginOpening();P.OpeningClick(TEXT("intro_skip"));P.RPG.Story.Enabled=false;P.DraftIdentity.Skin=1;P.DraftIdentity.Hair=3;P.DraftIdentity.HairColor=3;P.DraftIdentity.Beard35=0;P.CreatorTab35=1;P.CreatorZoom35=1;P.PortraitYaw=0;P.CreatorDirty35=true;},[this](ALWCharacter& P){CaptureV2(TEXT("Revision38_FaceFront"));});
 Add(TEXT("Face and hair profile"),3,[](ALWCharacter& P){P.PortraitYaw=75;P.CreatorDirty35=true;},[this](ALWCharacter& P){CaptureV2(TEXT("Revision38_Profile"));});
 Add(TEXT("Long hair"),3,[](ALWCharacter& P){P.DraftIdentity.Body=1;P.DraftIdentity.Hair=19;P.PortraitYaw=25;P.CreatorDirty35=true;},[this](ALWCharacter& P){CaptureV2(TEXT("Revision38_LongHair"));});
 Add(TEXT("Capture flush"),2,[](ALWCharacter& P){});
}
