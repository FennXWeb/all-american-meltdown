if(FParse::Param(FCommandLine::Get(),TEXT("LWCharacters61Smoke"))){
 struct FState61{TWeakObjectPtr<ALWResident> NPC;FVector HairStart;};auto State=MakeShared<FState61>();
 Add(TEXT("Character61 modular assets"),.5,[this](ALWCharacter& P){
  Cast<APlayerController>(P.Controller)->ConsoleCommand(TEXT("r.ScreenPercentage 100"));
  for(const TCHAR* Sex:{TEXT("Male"),TEXT("Female")}){
   auto* Head=LWAppearance::Mesh61(FString(Sex)+TEXT("AnatomicalHead35"));Check(Head&&Head->bAllowCPUAccess&&Head->GetPathName().Contains(TEXT("Models63")),TEXT("new head with CPU animation data"));
   for(int I=0;I<9;I++){auto* Mesh=LWAppearance::Mesh61(FString::Printf(TEXT("%sTop%d35"),Sex,I));Check(Mesh&&Mesh->GetPathName().Contains(TEXT("Models63")),TEXT("new wardrobe choice resolves"));}
  }
  for(int I=1;I<20;I++){auto* Mesh=LWAppearance::Mesh61(FString::Printf(TEXT("Hair%02d35"),I));Check(Mesh&&Mesh->bAllowCPUAccess&&Mesh->GetPathName().Contains(TEXT("Models63")),TEXT("strand hairstyle resolves"));}
 });
 Add(TEXT("Character61 male creator"),5,[this](ALWCharacter& P){P.BeginOpening();P.OpeningClick(TEXT("intro_skip"));P.RPG.Story.Enabled=false;P.DraftIdentity.Skin=0;P.DraftIdentity.Top35=0;P.DraftIdentity.TopColor35=2;P.DraftIdentity.BottomColor35=5;P.DraftIdentity.Hair=3;P.DraftIdentity.HairColor=3;P.CreatorZoom35=0;P.CreatorDirty35=true;},[this,State](ALWCharacter& P){
  CaptureV2(TEXT("Characters61_Male"));TArray<ULWHair61*> Hair;P.OpeningScene->GetComponents(Hair);Check(Hair.Num()==1,TEXT("creator has one live hair component"));if(Hair.Num())State->HairStart=Hair[0]->Displacement61;
 });
 Add(TEXT("Character61 hair response"),3,[](ALWCharacter& P){P.CreatorZoom35=1;P.CreatorDirty35=true;},[this,State](ALWCharacter& P){TArray<ULWHair61*> Hair;P.OpeningScene->GetComponents(Hair);Check(Hair.Num()==1,TEXT("creator rebuild cleans old hair"));if(Hair.Num()){Check(Hair[0]->Displacement61.Size()>KINDA_SMALL_NUMBER,TEXT("live spring hair moves"));Check(Hair[0]->ValidateDeformation61(),TEXT("weighted strands deform while roots stay fixed"));Check(Hair[0]->Displacement61.Size()<=2.01f,TEXT("hair displacement bounded"));}CaptureV2(TEXT("Characters61_MaleFace"));});
 Add(TEXT("Character61 female creator"),4,[](ALWCharacter& P){P.DraftIdentity.Body=1;P.DraftIdentity.Skin=0;P.DraftIdentity.Beard35=0;P.DraftIdentity.Hair=5;P.DraftIdentity.Top35=2;P.DraftIdentity.TopColor35=5;P.DraftIdentity.Bottom35=1;P.CreatorZoom35=0;P.CreatorDirty35=true;},[this](ALWCharacter& P){CaptureV2(TEXT("Characters61_Female"));});
 Add(TEXT("Character61 female face"),3,[](ALWCharacter& P){P.CreatorZoom35=1;P.CreatorDirty35=true;},[this](ALWCharacter& P){CaptureV2(TEXT("Characters61_FemaleFace"));});
 Add(TEXT("Character61 hats and bald"),2,[](ALWCharacter& P){P.DraftIdentity.Headwear35=2;P.CreatorDirty35=true;},[this](ALWCharacter& P){TArray<ULWHair61*> Hair;P.OpeningScene->GetComponents(Hair);Check(Hair.IsEmpty(),TEXT("headwear suppresses hair without orphan components"));CaptureV2(TEXT("Characters61_Hat"));});
 Add(TEXT("Character61 NPC and first person"),4,[this,State](ALWCharacter& P){P.DraftIdentity.Headwear35=0;P.OpeningClick(TEXT("creator_done"));P.bGod47=true;P.World->EnableEncounters=false;P.World->SetActorTickEnabled(false);P.RPG.Story.Enabled=false;auto* R=GetWorld()->SpawnActor<ALWResident>(P.GetActorLocation()+FVector(190,0,0),FRotator(0,180,0));R->ConfigureResident(TEXT("character61_test"),TEXT("story"),TEXT("Character Review"),1);R->SetActorTickEnabled(false);R->Say(TEXT("Ready when you are."));State->NPC=R;P.GiveItem(TEXT("revolver"));P.Equip(3);
  Check(P.Arms->GetStaticMesh()&&P.Arms->GetStaticMesh()->GetPathName().Contains(TEXT("Models63")),TEXT("first person hand uses new materials"));for(auto& C:R->Parts)Check(C->GetStaticMesh()&&C->GetStaticMesh()->GetPathName().Contains(TEXT("Models63")),TEXT("NPC uses approved body library"));
 },[this](ALWCharacter& P){CaptureV2(TEXT("Characters61_InGame"));});
 Add(TEXT("Character61 ragdoll compatibility"),2,[this,State](ALWCharacter& P){if(State->NPC.IsValid()){FDamageEvent E;State->NPC->Die(E,&P,100,nullptr);}},[this,State](ALWCharacter& P){int Bodies=0;if(State->NPC.IsValid())for(auto& C:State->NPC->Parts)Bodies+=C->IsSimulatingPhysics();Check(Bodies==7,TEXT("all seven character bodies simulate on death"));CaptureV2(TEXT("Characters61_Ragdoll"));});
 return;
}
