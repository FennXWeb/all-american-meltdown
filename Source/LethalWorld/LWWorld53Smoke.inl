if(FParse::Param(FCommandLine::Get(),TEXT("LWUpdate53Smoke"))){
 auto Add=[this](FString Name,double Delay,FLWV2Action Begin,FLWV2Action End=FLWV2Action()){V2->Steps.Add({Name,Delay,180,MoveTemp(Begin),MoveTemp(End),[](ALWCharacter&){return true;}});};
 struct F53{TWeakObjectPtr<ALWChunk> Chunk;FVector Center;};auto S=MakeShared<F53>();
 Add(TEXT("World53 setup"),1,[this](ALWCharacter& P){P.NewGame();P.LeaveSafehouse();P.Health=10000;P.World->EnableEncounters=false;P.World->WeatherOverride=0;P.World->TimeOfDay=12;P.World->TickWeather(0,&P);});
 for(int Type:{18,13}){
 Add(FString::Printf(TEXT("Retail53 authored layout %d"),Type),3,[this,S,Type](ALWCharacter& P){if(S->Chunk.IsValid())S->Chunk->Destroy();LWGen::FSite Site;Site.Type=Type;Site.Id=530000+Type;Site.Position=FVector2D(-2000000-Type*100000,2000000);Site.Size=LWPlaces::Size(Type);if(Type==13)while(!LWPOISurvivors::Selected(Site,P.World->Seed))Site.Id++;
 auto* C=GetWorld()->SpawnActor<ALWChunk>();S->Chunk=C;C->Box(P.World,TEXT("Concrete"),FVector(Site.Position,-10),FVector(Site.Size+FVector2D(1200),20));C->BuildingSurfaces=true;C->Building(P.World,Site);C->BuildingSurfaces=false;C->FlushSurfaces();C->SetActorLocation(FVector(0,0,5000));S->Center=FVector(Site.Position,5000);
 int Storage=0,CastCount=0;for(auto A:C->Residents)if(IsValid(A)){A->AddActorWorldOffset(FVector(0,0,5000));if(auto* N=Cast<ALWResident>(A))CastCount++;if(auto* Z=Cast<ALWZombie>(A)){Z->SetActorTickEnabled(false);Z->GetCharacterMovement()->DisableMovement();}if(auto* O=Cast<ALWWorldObject>(A))Storage+=O->Kind==ELWObjectKind::Container;}
 Check(Storage>=4,TEXT("new retail fixtures provide usable storage"));if(Type==13)Check(CastCount==3,TEXT("warehouse can contain trader recruit and quest giver instead of hostiles"));
 for(FName Name:Type==18?TArray<FName>{TEXT("RoundRack53"),TEXT("GarmentRail53"),TEXT("CashRegister53"),TEXT("DisplayTable53")}:TArray<FName>{TEXT("PalletRack53"),TEXT("LoadedPallet53"),TEXT("PalletJack53"),TEXT("PackingBench53")})Check(P.World->Mesh(Name)&&P.World->Mesh(Name)->GetName().Contains(Name.ToString()),TEXT("new fixture resolves to imported mesh, not fallback cube"));
 P.ClosePanels();LWV2Teleport(P,S->Center+FVector(0,-Site.Size.Y*.5+220,120));P.GetCharacterMovement()->DisableMovement();P.Controller->SetControlRotation(FRotator(0,90,0));P.Camera->SetWorldRotation(FRotator(0,90,0));P.World->Sky->SetActorLocation(P.GetActorLocation());P.World->TickWeather(.016f,&P);
 },[this,Type](ALWCharacter&){CaptureV2(*FString::Printf(TEXT("World53_Interior%d"),Type));});
 }
 Add(TEXT("World53 natural encounter placement"),2,[this,S](ALWCharacter& P){if(S->Chunk.IsValid())S->Chunk->Destroy();P.ClosePanels();P.World->ClearEncounterActors();P.World->Encounters=FLWEncounterState();P.World->Encounters.Elapsed=1000;P.World->EnableEncounters=true;P.bIndoors=P.bSafehouse=false;P.RPG.Level=20;
 LWV2Teleport(P,FVector(51000,51000,200));P.World->Stream(P.GetActorLocation(),true);P.World->ZombieCount=0;P.GetCharacterMovement()->DisableMovement();P.Camera->SetWorldRotation(FRotator(0,0,0));
 UE_LOG(LogTemp,Display,TEXT("WORLD53_STATE started=%d menu=%d indoor=%d safe=%d panel=%d inventory=%d chunks=%d pos=%s"),P.bStarted,P.bMenu,P.bIndoors,P.bSafehouse,P.RPGPanel,P.bInventory,P.World->Chunks.Num(),*P.GetActorLocation().ToString());
 for(int Try=0;Try<80&&P.World->Encounters.Serial==0;Try++){P.World->Encounters.NextAttempt=0;P.World->TickEncounters(8,&P);}
 Check(P.World->Encounters.Serial>0,TEXT("director can place an eligible scene in real generated terrain"));P.World->EnableEncounters=false;UE_LOG(LogTemp,Display,TEXT("WORLD53_DONE failures=%d"),TestFailures);
 });return;
}
