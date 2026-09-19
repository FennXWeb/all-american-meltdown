void ALWGameMode::BuildGear25Smoke(ALWCharacter& Initial){
 auto Add=[this](FString N,double W,FLWV2Action B,FLWV2Action E=FLWV2Action()){V2->Steps.Add({N,W,90,MoveTemp(B),MoveTemp(E),[](ALWCharacter&){
#if WITH_EDITOR
 return !GShaderCompilingManager||!GShaderCompilingManager->IsCompiling();
#else
 return true;
#endif
 }});};
 struct FState{TWeakObjectPtr<ALWChunk> Floor;TWeakObjectPtr<ALWVehicle> Bus;};auto S=MakeShared<FState>();
 Add(TEXT("gear fixtures"),2,[this,S](ALWCharacter& P){P.NewGame();P.LeaveSafehouse();P.World->EnableEncounters=false;P.World->TimeOfDay=12;P.World->TickWeather(0,&P);P.Health=10000;LWV2Teleport(P,FVector(20000,20000,5100));P.Controller->SetControlRotation(FRotator(0,0,0));auto* C=GetWorld()->SpawnActor<ALWChunk>();C->Box(P.World,TEXT("Concrete"),FVector(20000,20000,4990),FVector(8000,8000,20));S->Floor=C;
  for(FName Id:{TEXT("sling_pack"),TEXT("backpack"),TEXT("hiking_pack"),TEXT("military_pack"),TEXT("expedition_pack"),TEXT("night_vision")})Check(LWItems::Def(Id).Mesh.LoadSynchronous()!=nullptr,TEXT("gear model available"));
 });
 Add(TEXT("backpack transactional removal"),.2,[this](ALWCharacter& P){P.Inventory.Empty();auto B=LWItems::Make(TEXT("expedition_pack"));B.Slot=TEXT("Backpack");P.Inventory.Add(B);auto Ammo=LWItems::Make(TEXT("ammo_9mm"));Ammo.X=0;Ammo.Y=19;P.Inventory.Add(Ammo);P.bSafehouse=false;
  Check(!P.DropItem(B.Id)&&P.FindItem(B.Id),TEXT("cannot drop pack with extra rows occupied"));
  Check(!P.MoveItem(0,0,B.Id,2,0,false)&&P.FindItem(B.Id)->Slot==TEXT("Backpack"),TEXT("unequip overflow rolls back"));
  Check(!P.SortInventory(),TEXT("sort requires open inventory"));
 });
 Add(TEXT("rig slot and expanded inventory"),5,[this](ALWCharacter& P){P.ClosePanels();auto Rig=LWItems::Make(TEXT("rig"));LWItems::Place(P.Inventory,Rig,12,LWItems::InventoryHeight(P.Inventory));Check(P.EquipItem(Rig.Id),TEXT("rig equips"));auto Gun=LWItems::Make(TEXT("m4"));LWItems::Place(P.Inventory,Gun,12,LWItems::InventoryHeight(P.Inventory));Check(P.MoveItem(0,0,Gun.Id,0,0,false,TEXT("RigPrimary")),TEXT("equip extra primary"));P.EquipSniper();Check(P.ActiveWeaponId==Gun.Id,TEXT("hotkey 5 equips rig weapon"));Check(!P.DropItem(Rig.Id),TEXT("occupied rig cannot drop"));
  auto NV=LWItems::Make(TEXT("night_vision"));LWItems::Place(P.Inventory,NV,12,LWItems::InventoryHeight(P.Inventory));Check(P.EquipItem(NV.Id),TEXT("night vision equips helmet"));P.ToggleNightVision();Check(P.bNightVision&&P.NightVisionLight->IsVisible(),TEXT("night vision toggles"));P.ToggleNightVision();P.ToggleInventory();Check(P.SortInventory(),TEXT("sort expanded grid"));
 },[this](ALWCharacter&){CaptureV2(TEXT("Gear25_Inventory"));});
 Add(TEXT("night scene off"),3,[this,S](ALWCharacter& P){P.ClosePanels();P.World->TimeOfDay=0;P.World->TickWeather(0,&P);P.Flashlight->SetVisibility(false);LWV2Teleport(P,FVector(20000,20000,5090));P.Controller->SetControlRotation(FRotator(-4,0,0));for(int I=0;I<3;I++)S->Floor->Box(P.World,TEXT("Concrete"),FVector(20500+I*350,20000,5120),FVector(70,200,240));},[this](ALWCharacter&){CaptureV2(TEXT("Gear25_NightOff"));});
 Add(TEXT("night scene on"),3,[this](ALWCharacter& P){P.ToggleNightVision();Check(P.bNightVision,TEXT("night mode enabled"));},[this](ALWCharacter&){CaptureV2(TEXT("Gear25_NightOn"));});
 Add(TEXT("goggles removal"),.2,[this](ALWCharacter& P){auto* NV=P.Inventory.FindByPredicate([](const auto& I){return I.Definition==TEXT("night_vision");});Check(NV&&P.MoveItem(0,0,NV->Id,8,0,false),TEXT("goggles unequip"));P.TickNightVision();Check(!P.bNightVision&&!P.NightVisionLight->IsVisible(),TEXT("goggles off on unequip"));});
 Add(TEXT("school bus downhill"),1,[this,S](ALWCharacter& P){P.ClosePanels();P.World->TimeOfDay=12;P.World->TickWeather(0,&P);
  auto* C=S->Floor.Get();C->Box(P.World,TEXT("Asphalt"),FVector(20000,22000,7000),FVector(8000,1000,30),FRotator(-18,0,0));
  FLWVehicleRecord R;R.Model=TEXT("bus");R.Position=FVector(17700,22000,7822);R.Rotation=FRotator(-18,0,0);R.Unlocked=true;R.VIN=FGuid::NewGuid();P.World->Vehicles.Add(TEXT("gear25_bus"),R);
  auto* V=Cast<ALWVehicle>(P.World->SpawnObject(ELWObjectKind::Car,TEXT("gear25_bus"),R.Position,R.Rotation));S->Bus=V;Check(V!=nullptr,TEXT("school bus spawned"));if(!V)return;
  V->SetActorTickEnabled(false);V->Driver=&P;P.Vehicle=V;V->PlayerSeat=-1;V->EngineOn=true;V->Throttle=1;V->Speed=500;const FVector Start=V->GetActorLocation();
  for(int I=0;I<160;I++)V->Tick(.02f);
  Check(V->GetActorLocation().X>Start.X+1000,TEXT("bus sustains downhill movement"));Check(V->GetActorLocation().Z<Start.Z-250,TEXT("bus follows descending surface"));Check(V->Speed>400,TEXT("bus does not stall on downhill support"));
  // Steeper descent puts the leading wheel beyond the old 350cm trace.
  C->Box(P.World,TEXT("Asphalt"),FVector(20000,24000,9000),FVector(8000,1000,30),FRotator(-36,0,0));
  V->SetActorLocationAndRotation(FVector(17700,24000,10765),FRotator(-36,0,0),false,nullptr,ETeleportType::TeleportPhysics);V->Speed=500;const FVector SteepStart=V->GetActorLocation();
  for(int I=0;I<160;I++)V->Tick(.02f);
  Check(V->GetActorLocation().X>SteepStart.X+1000,TEXT("bus sustains steep downhill movement"));Check(V->GetActorLocation().Z<SteepStart.Z-600,TEXT("bus descends steep support"));Check(V->Speed>400,TEXT("steep downhill does not lose wheel contact"));
  V->Throttle=0;V->Speed=0;V->Driver=nullptr;P.Vehicle=nullptr;LWV2Teleport(P,V->GetActorLocation()+FVector(0,-1200,500));P.Controller->SetControlRotation(FRotator(-20,90,0));
 },[this](ALWCharacter&){CaptureV2(TEXT("Gear25_BusSlope"));});
 Add(TEXT("cleanup"),1,[S](ALWCharacter& P){if(S->Bus.IsValid())S->Bus->Destroy();if(S->Floor.IsValid())S->Floor->Destroy();P.World->Vehicles.Remove(TEXT("gear25_bus"));});
}
