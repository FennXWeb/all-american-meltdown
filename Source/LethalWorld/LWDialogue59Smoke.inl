if(FParse::Param(FCommandLine::Get(),TEXT("LWDialogue59Smoke"))){
 Add(TEXT("voice library and saved identity"),.5,[this](ALWCharacter& P){
  P.NewGame();P.RPG.Story.Enabled=false;P.World->SetActorTickEnabled(false);P.EnterSafehouse();P.ClosePanels();
  auto* C=ULWDialogueCatalog59::Get();Check(C&&C->Enabled&&C->Profiles.Num()==10,TEXT("six preserved and four new profiles enabled"));
  Check(C&&C->Lines.Num()>=949,TEXT("complete library and starter recordings imported"));
  auto* V=ULWDialogue59::Channel(&P);const FName A=V->Assign(TEXT("test_npc59"),false);Check(V->Assign(TEXT("test_npc59"),true)==A,TEXT("saved voice wins over appearance changes"));
  Check(P.World->Sound(TEXT("HumanMale44"),P.GetActorLocation())==nullptr,TEXT("old human gibberish is disabled"));
  P.RPG.VoiceProfiles59.Add(TEXT("migrated75"),TEXT("M02"));Check(V->Assign(TEXT("migrated75"),true)==FName(TEXT("M05")),TEXT("old voice maps to consistent active male voice even after appearance change"));Check(P.RPG.VoiceProfiles59[TEXT("migrated75")]==FName(TEXT("M02")),TEXT("original assignment stays archived in save"));
  for(const auto& L:LWDialogue75::Lines())for(FName Active:C->ProceduralProfiles75)Check(C->Lines.Contains(ULWDialogueCatalog59::Key(Active,L.Text)),TEXT("every active voice covers every new line"));
 });
 for(FName Profile:{FName(TEXT("M01")),FName(TEXT("M02")),FName(TEXT("M03")),FName(TEXT("M04")),FName(TEXT("M05")),FName(TEXT("F01")),FName(TEXT("F02")),FName(TEXT("F03")),FName(TEXT("F04")),FName(TEXT("F05"))}){
  Add(TEXT("play voice ")+Profile.ToString(),.6,[Profile](ALWCharacter& P){auto* V=ULWDialogue59::Channel(&P);ULWDialogueCatalog59::Get()->Cast.Add(TEXT("sample59"),Profile);P.RPG.VoiceProfiles59.Add(TEXT("sample59"),Profile);V->Say(TEXT("sample59"),false,TEXT("The roads are quiet. Stay close, and watch the rooftops."),true);},[this](ALWCharacter& P){auto* V=ULWDialogue59::Channel(&P);Check(IsValid(V->Audio)&&V->Audio->IsPlaying(),TEXT("generated voice plays after async load"));Check(!P.GetWorld()->GetSubsystem<ULWDialogueDirector75>()->CanSpeak(true),TEXT("in-flight or playing speech blocks overlapping chatter"));V->Stop();Check(!V->Busy(),TEXT("voice cancels cleanly"));ULWDialogueCatalog59::Get()->Cast.Remove(TEXT("sample59"));});
  V2->Steps.Last().Ready=[](ALWCharacter& P){auto* V=ULWDialogue59::Channel(&P);return IsValid(V->Audio)||!V->Busy();};V2->Steps.Last().TimeoutSeconds=15;
 }
 Add(TEXT("missing and interrupted dialogue"),.5,[this](ALWCharacter& P){auto* V=ULWDialogue59::Channel(&P);V->Say(TEXT("sample59"),false,TEXT("This line deliberately has no recording."));Check(!V->Busy(),TEXT("unrecorded dialogue does not wait forever"));V->Say(TEXT("sample59"),false,TEXT("There you are!"));V->Stop();},[this](ALWCharacter& P){Check(!ULWDialogue59::Channel(&P)->Busy(),TEXT("canceled async voice never starts late"));});
 Add(TEXT("spatial NPC speech"),.6,[](ALWCharacter& P){FActorSpawnParameters S;S.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;auto* R=P.GetWorld()->SpawnActor<ALWResident>(P.GetActorLocation()+FVector(180,0,0),FRotator::ZeroRotator,S);R->ConfigureResident(TEXT("test_spatial59"),TEXT("civilian"),TEXT("Voice Test"),0);R->ChatterTime=9999;P.Speaker=R;R->Say(TEXT("Supplies for sale. Fair prices."));},[this](ALWCharacter& P){auto* R=P.Speaker.Get();auto* V=R?ULWDialogue59::Channel(R):nullptr;Check(V&&IsValid(V->Audio)&&V->Audio->IsPlaying(),TEXT("resident speaks recorded dialogue"));if(V&&V->Audio){Check(V->Audio->GetAttachParent()==R->GetRootComponent(),TEXT("speech follows NPC attachment"));Check(V->Audio->AttenuationSettings==P.World->Attenuation,TEXT("speech uses world occlusion and attenuation"));Check(R->SubtitleTime>0&&R->LifeAnimation,TEXT("spoken line retains subtitles and facial animation component"));}if(R)R->Destroy();P.Speaker=nullptr;});
 V2->Steps.Last().Ready=[](ALWCharacter& P){auto* V=P.Speaker?ULWDialogue59::Channel(P.Speaker):nullptr;return !V||IsValid(V->Audio)||!V->Busy();};V2->Steps.Last().TimeoutSeconds=15;
 Add(TEXT("shared chatter quiet interval"),22,[](ALWCharacter& P){P.ClosePanels();});
 Add(TEXT("context chatter and conversation priority"),.5,[this](ALWCharacter& P){
  FActorSpawnParameters S;S.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
  auto* R=P.GetWorld()->SpawnActor<ALWResident>(P.GetActorLocation()+FVector(150,0,0),FRotator::ZeroRotator,S);R->ConfigureResident(TEXT("chatter75"),TEXT("recruit"),TEXT("Chatter Test"),0);R->ChatterTime=9999;R->SetActorTickEnabled(false);
  auto* V=ULWDialogue59::Channel(R);P.bInventory=true;Check(!V->Chatter75(R->ResidentId,false,TEXT("companion_travel")),TEXT("chatter stays silent while inventory is open"));P.bInventory=false;
  Check(V->Chatter75(R->ResidentId,false,TEXT("companion_travel")),TEXT("context line starts through the recorded channel"));P.Speaker=R;
 },[this](ALWCharacter& P){
  auto* R=P.Speaker.Get();auto* V=R?ULWDialogue59::Channel(R):nullptr;Check(V&&V->Audio&&V->Audio->IsPlaying(),TEXT("new companion recording plays"));
  Check(R&&!R->Subtitle.IsEmpty()&&R->SubtitleTime>0,TEXT("new chatter has timed subtitles"));
  if(R){Check(!V->Chatter75(R->ResidentId,false,TEXT("companion_combat"),true),TEXT("combat bark does not interrupt the speaker"));R->Destroy();}P.Speaker=nullptr;
 });
 V2->Steps.Last().Ready=[](ALWCharacter& P){auto* V=P.Speaker?ULWDialogue59::Channel(P.Speaker):nullptr;return !V||IsValid(V->Audio)||!V->Busy();};V2->Steps.Last().TimeoutSeconds=15;
 return;
}
