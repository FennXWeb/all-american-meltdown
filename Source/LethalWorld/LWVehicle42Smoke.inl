void ALWGameMode::BuildVehicle42Smoke(ALWCharacter& Initial){
 struct FState{TWeakObjectPtr<ALWVehicle> Car;};auto S=MakeShared<FState>();
 auto Add=[this](FString Name,double Delay,FLWV2Action Begin,FLWV2Action End=FLWV2Action()){V2->Steps.Add({Name,Delay,150,MoveTemp(Begin),MoveTemp(End),[](ALWCharacter&){return true;}});};
 Add(TEXT("vehicle42 setup"),2,[](ALWCharacter& P){P.NewGame();P.RPG.Story.Enabled=false;P.ClosePanels();P.LeaveSafehouse();P.World->EnableEncounters=false;P.World->TimeOfDay=12;P.World->SetActorTickEnabled(false);P.bCrust=false;P.Health=10000;P.GetCharacterMovement()->SetMovementMode(MOVE_Flying);for(TActorIterator<ALWVehicle> I(P.GetWorld());I;++I)I->Destroy();P.World->Vehicles.Empty();for(TActorIterator<ALWZombie> I(P.GetWorld());I;++I)I->Destroy();auto* F=P.GetWorld()->SpawnActor<ALWChunk>();F->Box(P.World,TEXT("Concrete"),FVector(20000,20000,4990),FVector(40000,40000,20));for(TActorIterator<ADirectionalLight> I(P.GetWorld());I;++I){I->SetActorRotation(FRotator(-55,-40,0));I->GetLightComponent()->SetIntensity(5);}});
 for(const auto& Spec:LWTraffic::Specs()){
  const FName Model(Spec.Id);
  Add(FString(TEXT("exterior "))+Spec.Id,1,[this,S,Model](ALWCharacter& P){
   if(auto* Old=S->Car.Get()){Old->Exit(true);Old->Destroy();}
   P.ClosePanels();P.GetCharacterMovement()->SetMovementMode(MOVE_Flying);
   const FVector At(20000,20000,5075);FLWVehicleRecord R;R.VIN=FGuid::NewGuid();R.Model=Model;R.Position=At;R.Unlocked=R.Hotwired=true;R.FuelLitres=35;R.FuelLootInitialized=true;
   const FName Id(*(FString(TEXT("qa42_"))+Model.ToString()));P.World->Vehicles.Add(Id,R);auto* C=Cast<ALWVehicle>(P.World->SpawnObject(ELWObjectKind::Car,Id,At));S->Car=C;if(!RequireV2(C!=nullptr,TEXT("vehicle spawned")))return;C->SetActorTickEnabled(false);C->SetActorLocation(At);
   Check(C->Body->GetStaticMesh()->GetName()==(Model==TEXT("rv")?FString(TEXT("SM_RV66_Shell")):FString(TEXT("SM_Vehicle42_"))+Model.ToString()),TEXT("new body model active"));
   Check(C->GaugeNeedles42.Num()==(Model==TEXT("dirtbike")?2:4),TEXT("working instrument set present"));
   Check(C->Body->GetStaticMesh()->bAllowCPUAccess,TEXT("body supports persistent dents"));
   const float L=C->Spec().HalfLength;const FVector Eye=At+FVector(L*1.5f+150,-L*1.8f-150,C->Spec().Height*.9f+160);const FVector Look=At+FVector(0,0,C->Spec().Height*.42f-75);
   LWV2Teleport(P,Eye-FVector(0,0,66));P.Controller->SetControlRotation((Look-Eye).Rotation());
  },[this,Model](ALWCharacter& P){CaptureV2(*(FString(TEXT("Vehicle42_"))+Model.ToString()+TEXT("_Exterior")));});
  Add(FString(TEXT("cockpit "))+Spec.Id,1,[this,S,Model](ALWCharacter& P){
   auto* C=S->Car.Get();if(!C)return;LWV2Teleport(P,C->GetActorLocation()+FVector(0,-C->Spec().HalfWidth-90,20));if(!RequireV2(C->Enter(&P),TEXT("player enters driver seat")))return;P.SeatYaw=0;P.SeatPitch=-12;P.TickVehicleSeat();C->EngineOn=true;C->Speed=C->Spec().MaxSpeed*.5f;
   for(int i=0;i<50;++i)C->TickDriveAudio(.05f,false,.7f);C->TickInstruments42(2);
   if(FParse::Param(FCommandLine::Get(),TEXT("LWVehicle42GeneratedAudio"))){
    for(int Layer=0;Layer<3;++Layer){
     const TCHAR* Suffix=Layer==0?TEXT("Idle"):Layer==1?TEXT("Low"):TEXT("High");
     const FName Slot(*(FString(TEXT("Vehicle42_"))+Model.ToString()+TEXT("_")+Suffix));FLWResolvedAudioSlot Resolved;
     Check(P.World->AudioCatalog&&P.World->AudioCatalog->ResolveSlot(Slot,Resolved,false)&&Resolved.Sound&&Resolved.Sound->GetPathName().StartsWith(TEXT("/Game/Audio/Vehicles42/"))&&Resolved.Settings.bLoop,TEXT("generated engine layer resolves without fallback"));
     Check(C->DriveAudio.IsValidIndex(Layer)&&IsValid(C->DriveAudio[Layer])&&C->DriveAudio[Layer]->IsPlaying(),TEXT("generated engine layer is playing"));
    }
   }
   Check(C->EngineRPM>800&&C->AudioGear>=1,TEXT("engine RPM follows load and gear"));
   const float SpeedAngle=C->GaugeNeedles42[0]->GetRelativeRotation().Roll;C->Speed=0;C->TickInstruments42(.1f);Check(FMath::Abs(C->GaugeNeedles42[0]->GetRelativeRotation().Roll-SpeedAngle)>5,TEXT("speedometer follows speed"));
   C->Headlights=true;C->Signal=-1;C->SignalClock=0;C->TickInstruments42(.1f);Check(C->GaugeLamps42[0]->IsVisible()&&C->GaugeLamps42[1]->IsVisible(),TEXT("indicator and headlight telltales work"));
   if(Model!=TEXT("dirtbike")){C->Record()->FuelLitres=0;C->TickInstruments42(.1f);Check(FMath::Abs(C->GaugeNeedles42[2]->GetRelativeRotation().Roll-130)<1,TEXT("fuel needle reads empty"));C->Record()->FuelLitres=35;}
   if(Model==TEXT("rv")){
    C->PlayerSeat=-2;C->CabinEye=FVector(LWTraffic::FrontOffset(C->Spec())-474,20,85);P.SeatYaw=90;P.SeatPitch=0;P.TickVehicleSeat();P.Camera->SetWorldRotation(FRotator(0,90,0));
    Check(C->CamperFocus(&P)==TEXT("fridge"),TEXT("rebuilt RV fridge retains its interaction trace"));
    C->PlayerSeat=-1;P.SeatYaw=0;P.SeatPitch=-12;P.TickVehicleSeat();
   }
   C->Speed=0;C->EngineOn=false;C->StopDriveAudio();
  },[this,Model](ALWCharacter& P){CaptureV2(*(FString(TEXT("Vehicle42_"))+Model.ToString()+TEXT("_Cockpit")));});
 }
 Add(TEXT("finish"),1,[S](ALWCharacter& P){if(auto* C=S->Car.Get())C->Exit(true);});
}
