#include "LWNPCLife.h"
#include "ProceduralMeshComponent.h"
void ALWGameMode::BuildNPCLife34Smoke(ALWCharacter& Initial){
 auto Add=[this](const TCHAR* Label,double Delay,FLWV2Action Begin,FLWV2Action End=FLWV2Action()){V2->Steps.Add({Label,Delay,90,MoveTemp(Begin),MoveTemp(End),[](ALWCharacter&){return true;}});};
 struct FGallery{ALWOpeningScene* Scene=nullptr;ALWResident* A=nullptr;ALWResident* B=nullptr;ALWZombie* Mannequin=nullptr;FVector Vertex;};auto G=MakeShared<FGallery>();
 Add(TEXT("NPC animation gallery"),4,[this,G](ALWCharacter& P){
  P.NewGame();P.RPG.Story.Enabled=false;P.OpeningMode=0;P.bMenu=false;P.SetMenuInput(false);
  G->Scene=GetWorld()->SpawnActor<ALWOpeningScene>(FVector(0,0,80000),FRotator::ZeroRotator);G->Scene->World=P.World;
  G->Scene->Piece(TEXT("Cube"),TEXT("Concrete"),FVector(0,0,-10),FVector(1400,1400,20));G->Scene->Light(FVector(180,-250,300),FLinearColor(1,.9,.8),38000,1400);G->Scene->Light(FVector(150,250,250),FLinearColor(.7,.8,1),24000,1200);
  P.SetActorLocation(G->Scene->GetActorLocation()+FVector(350,0,100));P.GetCharacterMovement()->DisableMovement();P.SetActorHiddenInGame(true);
  G->A=GetWorld()->SpawnActor<ALWResident>(G->Scene->GetActorLocation()+FVector(0,-70,96),FRotator::ZeroRotator);G->A->ConfigureResident(TEXT("story_mara"),TEXT("story"),TEXT("Mara Vale"),0);
  G->B=GetWorld()->SpawnActor<ALWResident>(G->Scene->GetActorLocation()+FVector(0,70,96),FRotator::ZeroRotator);G->B->ConfigureResident(TEXT("story_jonah"),TEXT("story"),TEXT("Jonah Reed"),1);
  for(auto* R:{G->A,G->B}){R->SetActorTickEnabled(false);R->GetCharacterMovement()->DisableMovement();R->ChatterTime=100;}
  G->Mannequin=GetWorld()->SpawnActor<ALWZombie>(G->Scene->GetActorLocation()+FVector(0,350,96),FRotator::ZeroRotator);G->Mannequin->ConfigureKind(ELWEnemyKind::Mannequin);G->Mannequin->SetActorTickEnabled(false);G->Mannequin->GetCharacterMovement()->DisableMovement();
  G->Scene->From=G->Scene->To=FVector(410,-120,190);G->Scene->Focus=FVector(0,0,120);G->Scene->Camera->FieldOfView=48;G->Scene->Pose(0);Cast<APlayerController>(P.Controller)->SetViewTarget(G->Scene);
 },[this,G](ALWCharacter& P){
  for(auto* R:{G->A,G->B}){TArray<UProceduralMeshComponent*> Meshes;R->GetComponents(Meshes);Check(Meshes.Num()==8,TEXT("seven body visuals and mouth created"));Check(!R->Parts[1]->IsVisible(),TEXT("original head not double rendered"));Check(R->Parts[1]->GetCollisionEnabled()==ECollisionEnabled::QueryOnly,TEXT("head hit collision retained"));}
  Check(G->A->LifeAnimation->Gesture!=G->B->LifeAnimation->Gesture,TEXT("residents have independent gesture sequences"));Check(G->Mannequin->Parts[3]->GetRelativeRotation().IsNearlyZero(),TEXT("passive mannequin stays still"));CaptureV2(TEXT("NPC34_Idle"));
 });
 Add(TEXT("Conversation facial animation"),1,[G](ALWCharacter& P){P.Speaker=G->A;G->A->LifeAnimation->Speak(nullptr,TEXT("We need to get these supplies back to the settlement before the storm."));G->Scene->From=G->Scene->To=FVector(115,-70,182);G->Scene->Focus=FVector(0,-70,175);G->Scene->Camera->FieldOfView=40;G->Scene->Pose(0);},[this,G](ALWCharacter& P){Check(G->A->LifeAnimation->MouthOpen>.05f,TEXT("speech opens the mouth"));Check(G->A->LifeAnimation->AnimationState==TEXT("Speaking"),TEXT("conversation pose active"));CaptureV2(TEXT("NPC34_Speaking"));});
 Add(TEXT("Speech returns to listening"),5,[](ALWCharacter&){},[this,G](ALWCharacter& P){Check(G->A->LifeAnimation->MouthOpen<.05f,TEXT("mouth closes after speech"));Check(G->A->LifeAnimation->AnimationState==TEXT("Listening"),TEXT("listening pose active"));CaptureV2(TEXT("NPC34_Listening"));});
 Add(TEXT("Full voice facial shape"),.09,[G](ALWCharacter& P){G->A->LifeAnimation->Speak(nullptr,TEXT("Look out!"));G->A->LifeAnimation->Envelope(nullptr,.2f);},[this,G](ALWCharacter& P){Check(G->A->LifeAnimation->MouthOpen>.3f,TEXT("voice envelope drives jaw"));CaptureV2(TEXT("NPC34_VoiceEnvelope"));});
 Add(TEXT("Animation yields to injury"),1,[G](ALWCharacter& P){P.Speaker=nullptr;G->A->SeveredMask=1<<3;G->A->Parts[3]->SetRelativeRotation(FRotator(13,24,35));},[this,G](ALWCharacter& P){Check(G->A->Parts[3]->GetRelativeRotation().Equals(FRotator(13,24,35),.1),TEXT("severed limb pose not overwritten"));});
 Add(TEXT("Animated ragdoll"),1,[G](ALWCharacter& P){FDamageEvent Damage;G->B->Die(Damage,&P,200,nullptr);},[this,G](ALWCharacter& P){int Count=0;for(auto& Part:G->B->Parts)Count+=Part->IsSimulatingPhysics()?1:0;Check(Count==7,TEXT("animated character retains seven simulated ragdoll bodies"));});
 Add(TEXT("Animation teardown"),1,[G](ALWCharacter& P){G->A->Destroy();G->B->Destroy();G->Mannequin->Destroy();G->Scene->Destroy();P.SetActorHiddenInGame(false);Cast<APlayerController>(P.Controller)->SetViewTarget(&P);P.EnterSafehouse();});
}
