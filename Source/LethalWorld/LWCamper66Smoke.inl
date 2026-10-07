if(FParse::Param(FCommandLine::Get(),TEXT("LWCamper66Smoke"))){
 struct FState66{TWeakObjectPtr<ALWVehicle> RV;TWeakObjectPtr<ALWWorldObject> Terminal;TWeakObjectPtr<ACameraActor> Camera;FGuid VIN;};auto S=MakeShared<FState66>();
 Add(TEXT("66 luxury coach setup"),2,[this,S](ALWCharacter& P){
  P.NewGame();P.ClosePanels();P.bMenu=false;P.bStarted=true;P.RPG.Story.Enabled=false;P.World->EnableEncounters=false;P.bGod47=true;P.World->SetActorTickEnabled(false);P.SetActorTickEnabled(false);P.GetCharacterMovement()->DisableMovement();P.World->TimeOfDay=12;P.World->TickWeather(0,&P);P.bCrust=false;
  auto* PC=Cast<APlayerController>(P.Controller);PC->ConsoleCommand(TEXT("r.ScreenPercentage 100"));PC->ConsoleCommand(TEXT("r.SetRes 1600x1000w"));PC->ConsoleCommand(TEXT("DisableAllScreenMessages"));PC->GetHUD()->bShowHUD=false;
  for(TActorIterator<ALWVehicle> V(GetWorld());V;++V)V->Destroy();P.World->Vehicles.Empty();P.SetActorLocation(FVector(20000,23000,50300));
  auto* Floor=GetWorld()->SpawnActor<ALWChunk>();Floor->SyncCollision68=true;Floor->Box(P.World,TEXT("Concrete"),FVector(20000,20000,49990),FVector(16000,16000,20));
  FLWVehicleRecord R;R.Model=TEXT("rv");R.Position=FVector(20000,20000,50075);R.VIN=FGuid::NewGuid();S->VIN=R.VIN;R.Unlocked=R.Hotwired=true;R.FuelLitres=70;R.FuelLootInitialized=true;P.World->Vehicles.Add(TEXT("rv66_test"),R);
  auto* V=Cast<ALWVehicle>(P.World->SpawnObject(ELWObjectKind::Car,TEXT("rv66_test"),R.Position));S->RV=V;if(!RequireV2(V!=nullptr,TEXT("luxury RV spawns")))return;V->SetActorTickEnabled(false);
  Check(!V->SpawnPlacementPending,TEXT("full coach fits supported ground"));Check(V->Spec().Seats==6&&V->Passengers.Num()==5,TEXT("six travel seats including driver"));Check(V->Body->GetStaticMesh()->GetName()==TEXT("SM_RV66_Shell"),TEXT("new coach shell active"));Check(V->GaugeNeedles42.Num()==4,TEXT("working gauges preserved"));Check(V->Body->GetStaticMesh()->bAllowCPUAccess,TEXT("shell retains dent deformation"));
  int Beds=0;TSet<FName> Seats;for(const auto& C:V->Details)if(IsValid(C)&&C->IsVisible()&&C->GetStaticMesh()){for(FName T:C->ComponentTags){Beds+=T.ToString().StartsWith(TEXT("bunk66_"));if(T.ToString().StartsWith(TEXT("seat_")))Seats.Add(T);}}
  Check(Beds==6,TEXT("two banks of three bunks"));Check(V->ShadesMesh66.Num()==15,TEXT("fifteen shades including windshield and door"));Check(V->Screens66.Num()==2,TEXT("two functional televisions"));
  S->Camera=GetWorld()->SpawnActor<ACameraActor>();S->Camera->GetCameraComponent()->SetFieldOfView(60);S->Camera->SetActorLocation(V->GetActorLocation()+FVector(900,1200,550));S->Camera->SetActorRotation((V->GetActorLocation()+FVector(0,0,110)-S->Camera->GetActorLocation()).Rotation());PC->SetViewTarget(S->Camera.Get());
 },[this](ALWCharacter&){CaptureV2(TEXT("RV66_Exterior"));});
 Add(TEXT("66 cabin circulation"),1,[this,S](ALWCharacter& P){auto* V=S->RV.Get();if(!V)return;P.SetActorLocation(V->GetActorLocation()+FVector(435,220,0));Check(V->Enter(&P,-2),TEXT("enter walkable cabin"));V->CabinEye=FVector(400,0,150);P.SeatYaw=180;V->CabinMove=FVector2D(1,0);for(int I=0;I<120;I++)V->TickCabinWalk(.05f);V->CabinMove=FVector2D::ZeroVector;Check(V->CabinEye.X< -385,TEXT("unbroken aisle reaches rear master bedroom"));
  auto Focus=[&](FVector Eye,FVector Target,FName Expected){const auto X=V->Root->GetComponentTransform();const FVector A=X.TransformPosition(Eye),B=X.TransformPosition(Target);P.Camera->SetWorldLocationAndRotation(A,(B-A).Rotation());Check(V->CamperFocus(&P)==Expected,*FString::Printf(TEXT("interaction ray reaches %s"),*Expected.ToString()));};
  Focus(FVector(-540,0,218),FVector(-540,-89,157),TEXT("faucet66"));Focus(FVector(-474,0,218),FVector(-474,91,164),TEXT("fridge"));Focus(FVector(-28,0,218),FVector(-28,117,172),TEXT("cabinlight66"));
  S->Camera->GetCameraComponent()->SetFieldOfView(90);S->Camera->SetActorLocation(V->Root->GetComponentTransform().TransformPosition(FVector(0,0,221)));S->Camera->SetActorRotation(V->GetActorRotation()+FRotator(-4,180,0));
 },[this](ALWCharacter&){CaptureV2(TEXT("RV66_Cabin"));});
 Add(TEXT("66 switches and extending rooms"),1,[this,S](ALWCharacter& P){auto* V=S->RV.Get();if(!V)return;
  V->UseCamper(&P,TEXT("cabinlight66"));Check(!V->Record()->CabinLights66,TEXT("lounge switch changes persisted lighting"));V->UseCamper(&P,TEXT("cabinlight66"));V->UseCamper(&P,TEXT("tv66"));V->UseCamper(&P,TEXT("faucet66"));V->TickLuxury66(.05f);Check(V->Record()->TV66==1&&V->Water66->IsVisible(),TEXT("TV broadcast and flowing faucet are active"));
  V->UseCamper(&P,TEXT("shade66_0"));for(int I=0;I<30;I++)V->TickLuxury66(.05f);Check(V->ShadesMesh66[0]->GetRelativeScale3D().Z>.8,TEXT("individual shade animates closed"));
  V->UseCamper(&P,TEXT("slides66"));V->UseCamper(&P,TEXT("awning66"));for(int I=0;I<140;I++)V->TickLuxury66(.05f);Check(V->SlideAlpha66>.99f&&V->AwningAlpha66>.99f,TEXT("both slide rooms and awning deploy"));
  Check(FMath::IsNearlyEqual(float(V->SeatLocation(1).Y),-165.f),TEXT("dinette seating follows slide room"));
  S->Camera->GetCameraComponent()->SetFieldOfView(60);S->Camera->SetActorLocation(V->GetActorLocation()+FVector(950,1650,630));S->Camera->SetActorRotation((V->GetActorLocation()+FVector(0,0,130)-S->Camera->GetActorLocation()).Rotation());
 },[this](ALWCharacter&){CaptureV2(TEXT("RV66_Camp"));});
 Add(TEXT("66 ignition interlock"),.5,[this,S](ALWCharacter& P){auto* V=S->RV.Get();if(!V)return;V->PlayerSeat=-1;V->Ignition();Check(!V->EngineOn&&V->PendingIgnition66,TEXT("ignition waits for automatic stow"));for(int I=0;I<25;I++)V->TickLuxury66(.05f);Check(!V->EngineOn&&V->SlideAlpha66>.1f,TEXT("engine cannot start partway through retraction"));for(int I=0;I<140;I++)V->TickLuxury66(.05f);Check(V->EngineOn&&V->SlideAlpha66<.001f&&V->AwningAlpha66<.001f,TEXT("engine starts only after equipment is stowed"));V->UseCamper(&P,TEXT("slides66"));Check(!V->Record()->Slides66,TEXT("engine must be off to deploy"));V->EngineOn=false;V->PlayerSeat=-2;});
 Add(TEXT("66 housing and storage"),1,[this,S](ALWCharacter& P){auto* V=S->RV.Get();if(!V)return;P.RPG.Crew.Empty();for(int I=0;I<7;I++){FLWCrewRecord C;C.Id=FName(*FString::Printf(TEXT("crew66_%d"),I));C.Name=FString::Printf(TEXT("Traveler %d"),I+1);C.Bedroom=I;P.RPG.Crew.Add(C);}
  for(int I=0;I<6;I++)Check(P.AssignRVBunk66(V,I,P.RPG.Crew[I].Id),TEXT("companion assigned to distinct bunk"));Check(!P.AssignRVBunk66(V,6,P.RPG.Crew[6].Id),TEXT("seventh bunk cannot be assigned"));Check(P.AssignRVBunk66(V,0,P.RPG.Crew[6].Id)&&P.RPG.Crew[0].HomeVehicle66.IsNone(),TEXT("replacing resident frees former assignment"));
  auto* N=GetWorld()->SpawnActor<ALWResident>(V->GetActorLocation(),FRotator::ZeroRotator);N->ConfigureResident(P.RPG.Crew[6].Id,TEXT("recruit"),TEXT("Traveler 7"),0);Check(N->TickRVHome66(&P)&&N->HomeRV66==V,TEXT("dismissed resident lives in moving motorhome"));P.RPG.Crew[6].Following=true;Check(!N->TickRVHome66(&P)&&!N->HomeRV66,TEXT("recruited resident leaves bunk to follow"));P.RPG.Crew[6].Following=false;N->TickRVHome66(&P);N->SetActorTickEnabled(false);
  P.SetBedSpawn(V);Check(V->Record()->Owned45&&P.RPG.Respawn.Vehicle==V->RecordId&&V->Record()->GarageBay45==-1,TEXT("master bed claims ownership without invalid garage allocation"));
  V->UseCamper(&P,TEXT("cargo_-1_0"));auto* First=P.OpenObject.Get();Check(First&&First->RecordId==V->RecordId,TEXT("original cargo inventory preserved"));P.ClosePanels();V->UseCamper(&P,TEXT("dining_storage"));Check(P.OpenObject&&P.OpenObject->RecordId==FName(TEXT("rv66_test_dining_storage")),TEXT("dining table has separate storage"));P.ClosePanels();
  P.OpenRVHome66(V,2);Check(P.Service66==V&&P.RPGPanel==4&&!P.bTrigger,TEXT("bunk assignment UI owns input"));Cast<APlayerController>(P.Controller)->GetHUD()->bShowHUD=true;
 },[this,S](ALWCharacter& P){CaptureV2(TEXT("RV66_Housing"));});
 Add(TEXT("66 persisted cabin"),1,[this,S](ALWCharacter& P){P.ClosePanels();Cast<APlayerController>(P.Controller)->GetHUD()->bShowHUD=false;auto* V=S->RV.Get();if(!V)return;P.DrainSave40();V->Record()->Slides66=true;V->Record()->TV66=2;P.SaveProgress();P.DrainSave40();auto* Save=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromSlot(P.SaveSlot40(),0));
  Check(Save!=nullptr,TEXT("isolated save written"));if(Save){const auto* R=Save->Vehicles.Find(V->RecordId);Check(R&&R->VIN==S->VIN&&R->Owned45&&R->Slides66&&R->TV66==2&&R->Shades66.Contains(0),TEXT("VIN ownership equipment and shades survive serialization"));Check(Save->RPG.Crew.ContainsByPredicate([](const auto& C){return C.HomeVehicle66==TEXT("rv66_test")&&C.HomeBunk66==2;}),TEXT("companion home assignment survives serialization"));}V->Record()->Slides66=false;
 });
 Add(TEXT("66 paid delivery"),1,[this,S](ALWCharacter& P){auto* V=S->RV.Get();if(!V)return;V->Exit(true);P.ClosePanels();auto* T=P.World->SpawnObject(ELWObjectKind::Furniture,TEXT("delivery66_test"),FVector(23000,20000,50000));T->UseType=TEXT("delivery66");T->Body->SetStaticMesh(P.World->Mesh(TEXT("RV66_DeliveryTerminal")));T->Route={FVector2D(23000,21500)};S->Terminal=T;P.SetActorLocation(T->GetActorLocation()+FVector(0,-150,90));P.Money=149;Check(!P.DeliverVehicle66(V->RecordId,T)&&P.Money==149,TEXT("insufficient funds do not move vehicle or charge"));P.Money=1000;V->EngineOn=true;Check(!P.DeliverVehicle66(V->RecordId,T)&&P.Money==1000,TEXT("running vehicle cannot be delivered"));V->EngineOn=false;
  auto& Cargo=P.World->Containers.FindChecked(V->RecordId);auto Item=LWItems::Make(TEXT("medkit"),2);LWItems::Place(Cargo.Items,Item,Cargo.Width,Cargo.Height);
  Check(P.DeliverVehicle66(V->RecordId,T),TEXT("owned vehicle delivered to supported clear parking pad"));Check(P.Money==850&&V->Record()->VIN==S->VIN,TEXT("single successful delivery charges once and preserves VIN"));Check(P.World->Containers.FindChecked(V->RecordId).Items.ContainsByPredicate([&](const auto& X){return X.Id==Item.Id;}),TEXT("delivered vehicle retains cargo items"));int Count=0;for(TActorIterator<ALWVehicle> C(GetWorld());C;++C)Count+=C->RecordId==V->RecordId;Check(Count==1,TEXT("delivery never duplicates loaded vehicle"));V->SetActorTickEnabled(false);
  P.OpenDelivery66(T);Check(P.Service66==T&&P.DialogueChoices.Num()>1,TEXT("delivery menu lists owned fleet"));Cast<APlayerController>(P.Controller)->GetHUD()->bShowHUD=true;
 },[this](ALWCharacter& P){CaptureV2(TEXT("RV66_Delivery"));});
 Add(TEXT("66 blocked and unloaded delivery"),1,[this,S](ALWCharacter& P){P.ClosePanels();Cast<APlayerController>(P.Controller)->GetHUD()->bShowHUD=false;auto* T=S->Terminal.Get();if(!T)return;
  FLWVehicleRecord R;R.Model=TEXT("sedan");R.VIN=FGuid::NewGuid();R.Owned45=R.Stored45=R.Unlocked=R.Hotwired=true;R.GarageBay45=0;R.Position=FVector(0,0,-6800);R.FuelLitres=30;P.World->Vehicles.Add(TEXT("stored66"),R);
  T->Route={FVector2D(22000,18000)};auto* Block=GetWorld()->SpawnActor<ALWChunk>();Block->SyncCollision68=true;Block->Box(P.World,TEXT("Concrete"),FVector(22800,18000,50200),FVector(5000,1600,400));const int Money=P.Money;Check(!P.DeliverVehicle66(TEXT("stored66"),T)&&P.Money==Money&&P.World->Vehicles.FindChecked(TEXT("stored66")).Stored45,TEXT("blocked delivery neither charges nor removes garage record"));Block->Destroy();
  Check(P.DeliverVehicle66(TEXT("stored66"),T),TEXT("unloaded garage vehicle can be called"));const auto& Delivered=P.World->Vehicles.FindChecked(TEXT("stored66"));Check(Delivered.VIN==R.VIN&&!Delivered.Stored45&&Delivered.GarageBay45==0&&P.Money==Money-150,TEXT("garage reservation and identity retained after delivery"));
 });
 Add(TEXT("66 authored parking lot service"),1,[this,S](ALWCharacter& P){auto* C=GetWorld()->SpawnActor<ALWChunk>();LWGen::FSite Site;Site.Type=16;Site.Id=660016;Site.Position=FVector2D(100000,100000);Site.Size=LWPlaces::Size(16);C->Building(P.World,Site);C->FlushSurfaces();ALWWorldObject* Terminal=nullptr;for(auto& A:C->Residents)if(auto* O=Cast<ALWWorldObject>(A))if(O->UseType==TEXT("delivery66"))Terminal=O;
  Check(Terminal&&Terminal->Route.Num()==1&&Terminal->Body->GetStaticMesh()->GetName()==TEXT("SM_RV66_DeliveryTerminal"),TEXT("procedural parking lots include service terminal and designated delivery pad"));C->Destroy();
  auto* V=S->RV.Get();if(!V)return;P.SetActorLocation(V->GetActorLocation());P.SetActorHiddenInGame(true);V->Record()->TV66=3;V->ScreenClock66=0;V->TickLuxury66(.05f);Check(V->TVCamera66&&V->TVTarget66,TEXT("working exterior television camera available"));V->Record()->Slides66=true;V->Record()->Awning66=false;for(int I=0;I<140;I++)V->TickLuxury66(.05f);
  S->Camera->GetCameraComponent()->SetFieldOfView(90);
  S->Camera->SetActorLocation(V->Root->GetComponentTransform().TransformPosition(FVector(-155,0,218)));S->Camera->SetActorRotation(V->GetActorRotation()+FRotator(-4,180,0));
 },[this](ALWCharacter&){CaptureV2(TEXT("RV66_LivingRoom"));});
 Add(TEXT("66 master suite"),1,[S](ALWCharacter& P){auto* V=S->RV.Get();if(!V)return;S->Camera->SetActorLocation(V->Root->GetComponentTransform().TransformPosition(FVector(-780,0,218)));S->Camera->SetActorRotation(V->GetActorRotation()+FRotator(-15,180,0));},[this](ALWCharacter&){CaptureV2(TEXT("RV66_MasterSuite"));});
 Add(TEXT("66 walk out after garbage collection"),1,[this,S](ALWCharacter& P){
  auto* V=S->RV.Get();if(!V)return;P.ClosePanels();P.SetActorHiddenInGame(false);Cast<APlayerController>(P.Controller)->SetViewTarget(&P);
  for(int Round=0;Round<3;Round++){
   P.SetActorLocation(V->GetActorTransform().TransformPosition(FVector(LWTraffic::FrontOffset(V->Spec())+35,220,100)));
   if(!RequireV2(V->Enter(&P,-2),TEXT("RV walk-in succeeds before exit regression")))return;
   V->CamperDoorOpen=true;V->CamperDoorAngle=-100;V->TickCamperEntry(0);V->EngineOn=false;V->Speed=0;
   // Enter places the player inside the open doorway. Reverse through that
   // opening instead of scripting a straight line through the copilot chair.
   P.SeatYaw=90;V->CabinMove=FVector2D(1,0);P.TickVehicleSeat();
   CollectGarbage(RF_NoFlags);
   if(!RequireV2(V->Headlamps.Num()==2&&IsValid(V->Headlamps[0])&&IsValid(V->Headlamps[1])&&V->Headlamps[0]->IsRegistered()&&V->Headlamps[1]->IsRegistered(),TEXT("both RV headlights survive interior rebuild and garbage collection")))return;
   Check(V->CabinLamps66.Num()==5&&!V->CabinLamps66.ContainsByPredicate([](const auto& L){return !IsValid(L);}),TEXT("new cabin fixtures survive garbage collection"));
   V->Headlights=Round!=1;
   // Exercise the complete vehicle tick, not a direct forced Exit call. Walking
   // over the threshold clears Driver in the middle of this same tick.
   for(int I=0;I<70&&P.Vehicle==V;I++){V->Tick(.05f);P.TickVehicleSeat();}
   Check(!P.Vehicle&&!V->Driver,TEXT("walking through open RV doorway completes exit"));
   Check(P.GetCharacterMovement()->MovementMode==MOVE_Walking&&P.GetCapsuleComponent()->GetCollisionEnabled()==ECollisionEnabled::QueryAndPhysics,TEXT("RV exit restores on-foot movement and collision"));
   Check(V->GetActorTransform().InverseTransformPosition(P.GetActorLocation()).Y>185,TEXT("walking exit places player outside coach"));
   CollectGarbage(RF_NoFlags);V->Tick(.05f);
   Check(IsValid(V->Headlamps[0])&&V->Headlamps[0]->IsVisible()==V->Headlights,TEXT("headlights still toggle after exit and cleanup"));
   P.GetCharacterMovement()->DisableMovement();
  }
 });
 return;
}
