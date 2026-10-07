if(FParse::Param(FCommandLine::Get(),TEXT("LWNY69Smoke"))){
 Add(TEXT("NY69 setup"),1,[this](ALWCharacter& P){P.NewGame();P.RPG.Story.Enabled=false;P.World->EnableEncounters=false;P.World->SetActorTickEnabled(false);P.SetActorTickEnabled(false);P.ClosePanels();P.bMenu=false;P.bSafehouse=false;P.bGod47=true;UGameplayStatics::SetGamePaused(&P,false);P.SetMenuInput(false);P.GetCharacterMovement()->DisableMovement();P.World->TimeOfDay=12;P.World->WeatherOverride=0;P.World->TickWeather(.1,&P);P.WeaponRoot->SetVisibility(false,true);});
 for(int Type:{68,69}){
  Add(FString::Printf(TEXT("NY69 landmark %d"),Type),3,[this,Type](ALWCharacter& P){
   const auto* S=LWNY69::Sites().FindByPredicate([&](const auto& A){return A.Type==Type;});Check(S!=nullptr,TEXT("authored landmark exists"));if(!S)return;
   P.SetActorLocation(FVector(S->Position,130));P.World->Stream(P.GetActorLocation(),true);
   auto* C=P.World->Chunks.FindRef(LWGen::ChunkAt(S->Position)).Get();Check(C&&C->Ready68,TEXT("landmark construction and collision finish"));
   FHitResult Hit;FCollisionQueryParams Q(NAME_None,false,&P);Check(GetWorld()->LineTraceSingleByChannel(Hit,FVector(S->Position,Type==69?780:160),FVector(S->Position,Type==69?580:-100),ECC_Visibility,Q),TEXT("walkable ground floor at landmark centre"));
   P.SetActorLocation(FVector(S->Position,0)+FVector(-37000,-38000,22000));P.Controller->SetControlRotation((FVector(S->Position,500)-P.GetPawnViewLocation()).Rotation());P.World->Sky->SetActorLocation(P.GetActorLocation());P.World->TickWeather(.1,&P);
  },[this,Type](ALWCharacter& P){CaptureV2(Type==68?TEXT("NY69_Highmark"):TEXT("NY69_DestinyExterior"));});
 }
 Add(TEXT("NY69 Destiny gallery"),2,[this](ALWCharacter& P){const auto* S=LWNY69::Sites().FindByPredicate([](const auto& A){return A.Type==69;});if(!S)return;const auto& D=LWDestiny71::Data();P.SetActorLocation(FVector(S->Position+D.Atrium+FVector2D(-1500,-1500),720));P.Controller->SetControlRotation((FVector(S->Position+D.Atrium,1400)-P.GetPawnViewLocation()).Rotation());P.World->Sky->SetActorLocation(P.GetActorLocation());P.World->TickWeather(.1,&P);int Containers=0;for(TActorIterator<ALWWorldObject> I(GetWorld());I;++I)if(I->RecordId.ToString().StartsWith(TEXT("destiny71_")))Containers++;Check(Containers>100,TEXT("retail stores have lootable stock and usable seating"));},[this](ALWCharacter&){CaptureV2(TEXT("NY69_DestinyGallery"));});
 Add(TEXT("NY69 atlas"),1,[this](ALWCharacter& P){P.SwitchTab(1);if(auto* H=Cast<ALWHUD>(Cast<APlayerController>(P.Controller)->GetHUD())){H->MapZoom=.0004;H->MapPan=FVector2D(50000,1050000)-FVector2D(P.GetActorLocation());}},[this](ALWCharacter&){CaptureV2(TEXT("NY69_Map"));});
 return;
}
