if(FParse::Param(FCommandLine::Get(),TEXT("LWUpdate83Smoke"))){
 struct FState83{TWeakObjectPtr<ALWVehicle> RV;TWeakObjectPtr<ACameraActor> Camera;TWeakObjectPtr<ALWChunk> Road;};auto S=MakeShared<FState83>();
 auto HUD=[](ALWCharacter& P){return Cast<ALWHUD>(Cast<APlayerController>(P.Controller)->GetHUD());};
 auto Click=[this,HUD](ALWCharacter& P,FName Id){
  auto* H=HUD(P);const auto* C=H->Controls55.FindByPredicate([&](const auto& V){return V.Id==Id;});if(!RequireV2(C!=nullptr,TEXT("responsive button exists")))return;
  auto* V=GetWorld()->GetGameViewport()->GetGameViewport();auto Widget=V->GetViewportWidget().Pin();if(!RequireV2(Widget.IsValid(),TEXT("viewport input available")))return;
  const auto& G=Widget->GetCachedGeometry();const FVector2D Local=C->Center*H->Scale+H->Origin55;const auto* Hit=H->GetHitBoxAtCoordinates(Local,true);Check(Hit&&Hit->GetName()==Id,TEXT("button hit target matches rendered location"));
  const FVector2D Point=G.LocalToAbsolute(Local);TSet<FKey> Keys;Keys.Add(EKeys::LeftMouseButton);FPointerEvent Down(0,0,Point,Point,Keys,EKeys::LeftMouseButton,0,FModifierKeysState());V->OnMouseButtonDown(G,Down);Keys.Empty();FPointerEvent Up(0,0,Point,Point,Keys,EKeys::LeftMouseButton,0,FModifierKeysState());V->OnMouseButtonUp(G,Up);
 };
 Add(TEXT("83 isolated fixture"),3,[this,S](ALWCharacter& P){
  P.NewGame();P.EndOpening(true);P.ClosePanels();if(P.Campaign76){P.Campaign76->Pause();P.RPG.Campaign76.Paused=true;}P.bStoryLocked=false;P.bMenu=false;P.bStarted=true;P.bGod47=true;P.World->EnableEncounters=false;P.World->SetActorTickEnabled(false);P.SetActorTickEnabled(false);P.GetCharacterMovement()->DisableMovement();P.World->TimeOfDay=12;P.World->TickWeather(0,&P);P.bCrust=false;P.bSafehouse=false;P.SetActorHiddenInGame(true);P.SetActorLocation(FVector(20000,23000,50300));P.SetMenuInput(false);
  auto* PC=Cast<APlayerController>(P.Controller);PC->ConsoleCommand(TEXT("r.ScreenPercentage 100"));PC->ConsoleCommand(TEXT("DisableAllScreenMessages"));
  auto* Floor=GetWorld()->SpawnActor<ALWChunk>();Floor->SyncCollision68=true;Floor->Box(P.World,TEXT("Concrete"),FVector(20000,20000,49990),FVector(18000,18000,20));
  FLWVehicleRecord R;R.Model=TEXT("solstice_rv");R.Position=FVector(20000,20000,50075);R.VIN=FGuid::NewGuid();R.Unlocked=R.Hotwired=true;R.FuelLootInitialized=true;P.World->Vehicles.Add(TEXT("rv83"),R);S->RV=Cast<ALWVehicle>(P.World->SpawnObject(ELWObjectKind::Car,TEXT("rv83"),R.Position));
  if(!RequireV2(S->RV.IsValid(),TEXT("electric RV spawned")))return;auto* V=S->RV.Get();V->SetActorTickEnabled(false);V->SpawnPlacementPending=false;V->SetActorHiddenInGame(false);
  int Painted=0;for(int I=0;I<V->Body->GetNumMaterials();I++)if(auto* M=V->Body->GetMaterial(I))if(M->GetName().Contains(TEXT("EV83_CoachPearl"))){FLinearColor Tint;Check(M->GetVectorParameterValue(FMaterialParameterInfo(TEXT("PaintTint")),Tint)&&FMath::Abs(Tint.R-Tint.G)<.03&&FMath::Abs(Tint.G-Tint.B)<.03,TEXT("electric RV factory coat is neutral, not pink"));Painted++;}Check(Painted>0,TEXT("electric coach uses corrected factory material"));
  S->Camera=GetWorld()->SpawnActor<ACameraActor>();auto* C=S->Camera.Get();C->GetCameraComponent()->SetFieldOfView(65);C->SetActorLocation(V->GetActorLocation()+FVector(900,1450,650));C->SetActorRotation((V->GetActorLocation()+FVector(-300,0,130)-C->GetActorLocation()).Rotation());PC->SetViewTarget(C);PC->GetHUD()->bShowHUD=false;
 },[this](ALWCharacter&){CaptureV2(TEXT("Update83_ElectricRV"));});
 Add(TEXT("83 signal collision"),2,[this,S](ALWCharacter& P){
  auto* C=GetWorld()->SpawnActor<ALWChunk>(FVector(0,0,60000),FRotator::ZeroRotator);S->Road=C;C->SyncCollision68=true;C->Coordinate={0,0};C->BuildingSurfaces=true;
  C->Box(P.World,TEXT("Earth"),FVector(6400,6400,-10),FVector(12800,12800,20));
  TArray<LWGen::FRoad> R{{{1000,6400},{11800,6400},1680,true},{{6400,1000},{6400,11800},780,false}};C->BuildRoads33(P.World,R,{});C->FlushSurfaces();
  Check(C->SignalLamps.Num()==4,TEXT("four working signal heads retained"));int Parts=0,Colliders=0;TArray<UPrimitiveComponent*> All;C->GetComponents(All);
  for(auto* Part:All)if(Part->ComponentHasTag(TEXT("TrafficSignal83"))){Parts++;Check(Part->GetCollisionObjectType()==ECC_Destructible,TEXT("signal excluded from static/dynamic suspension queries"));if(Part->GetCollisionEnabled()!=ECollisionEnabled::NoCollision){Colliders++;Check(Part->GetCollisionResponseToChannel(ECC_WorldDynamic)==ECR_Ignore&&Part->GetCollisionResponseToChannel(ECC_Visibility)==ECR_Block,TEXT("signal permits vehicles and retains bullet collision"));}}
  Check(Parts>12&&Colliders>0,TEXT("signal collision rules cover mast arm and housing"));
  FCollisionObjectQueryParams Types;Types.AddObjectTypesToQuery(ECC_WorldStatic);Types.AddObjectTypesToQuery(ECC_WorldDynamic);FCollisionQueryParams Q;
  for(const auto& L:C->SignalLamps)if(L.Lens[0].IsValid()){FVector Start=L.Lens[0]->GetComponentLocation()-FVector(L.Approach*16,0)+FVector(0,0,300);FHitResult Hit;GetWorld()->LineTraceSingleByObjectType(Hit,Start,Start-FVector(0,0,1500),Types,Q);UE_LOG(LogTemp,Display,TEXT("PROBE83 hit=%d actor=%s component=%s z=%.1f type=%d"),Hit.bBlockingHit,*GetNameSafe(Hit.GetActor()),*GetNameSafe(Hit.GetComponent()),Hit.ImpactPoint.Z,Hit.GetComponent()?int(Hit.GetComponent()->GetCollisionObjectType()):-1);Check(Hit.bBlockingHit&&Hit.ImpactPoint.Z<60025,TEXT("suspension sees road below signal, never signal roof"));Check(L.Lens[2]->GetComponentLocation().Z>60470,TEXT("lowest lens clears tall vehicles"));}
  // Isolate signal collision; generated highway guardrails are intentionally solid.
  FCollisionQueryParams SignalQuery;for(auto* Part:All)if(!Part->ComponentHasTag(TEXT("TrafficSignal83")))SignalQuery.AddIgnoredComponent(Part);
  for(const auto& B:C->Breakables60)if(B.Id.ToString().StartsWith(TEXT("roadprop60_signal_"))){FHitResult Hit;GetWorld()->SweepSingleByChannel(Hit,B.Anchor+FVector(-700,0,160),B.Anchor+FVector(700,0,160),FQuat::Identity,ECC_WorldDynamic,FCollisionShape::MakeBox(FVector(220,150,100)),SignalQuery);Check(!Hit.bBlockingHit,TEXT("wide vehicle chassis passes through signal mast without obstruction"));}
  auto* Cam=S->Camera.Get();Cam->SetActorLocation(FVector(9900,3200,62000));Cam->SetActorRotation((FVector(6400,6400,60200)-Cam->GetActorLocation()).Rotation());
 },[this](ALWCharacter&){CaptureV2(TEXT("Update83_TrafficLights"));});
 Add(TEXT("83 menu setup"),.5,[S,HUD](ALWCharacter& P){Cast<APlayerController>(P.Controller)->SetViewTarget(&P);HUD(P)->bShowHUD=true;P.RPG.Crew.Empty();FLWCrewRecord Crew;Crew.Id=TEXT("crew83");Crew.Name=TEXT("Jamie Mercer");P.RPG.Crew.Add(Crew);P.SetMenuInput(true);});
 for(int Tab=0;Tab<7;Tab++)Add(FString::Printf(TEXT("83 tab %d"),Tab),.7,[Tab](ALWCharacter& P){P.SwitchTab(Tab);},[this,Tab,HUD](ALWCharacter& P){
  auto* H=HUD(P);int X=0,Y=0;Cast<APlayerController>(P.Controller)->GetViewportSize(X,Y);const float W=X/H->Scale;
  Check(FMath::IsNearlyZero(H->Origin55.X),TEXT("tab canvas uses complete screen width"));
  const auto* Last=H->Controls55.FindByPredicate([](const auto& C){return C.Id==TEXT("tab_6");});Check(Last&&Last->Center.X>W*.8,TEXT("tab navigation expands with viewport"));
  for(const auto& C:H->Controls55)Check(C.Center.X>=0&&C.Center.X<=W,TEXT("responsive control is on-screen"));
  if(Tab==0){const auto* Sort=H->Controls55.FindByPredicate([](const auto& C){return C.Id==TEXT("inv_sort55");});Check(Sort&&FMath::IsNearlyEqual(float(Sort->Center.X),690+FMath::Max(0.f,W-1280)*.5f,1.f),TEXT("inventory sort button follows expanded pane"));}
  if(Tab==2){const auto* Buy=H->Controls55.FindByPredicate([](const auto& C){return C.Id.ToString().StartsWith(TEXT("buy_"));});Check(Buy&&Buy->Center.X>W-200,TEXT("perk controls anchor to right edge"));}
  if(Tab==4){const auto* Crew=H->Controls55.FindByPredicate([](const auto& C){return C.Id==TEXT("crew_crew83");});Check(Crew&&Crew->Center.X>W-220,TEXT("crew controls anchor to right edge"));}
  CaptureV2(*FString::Printf(TEXT("Update83_Tab%d"),Tab));
 });
 Add(TEXT("83 container input setup"),.5,[this](ALWCharacter& P){P.SwitchTab(0);auto* O=GetWorld()->SpawnActor<ALWWorldObject>();O->SetActorLocation(P.GetActorLocation()+FVector(100,0,0));O->World=P.World;O->Kind=ELWObjectKind::Container;O->RecordId=TEXT("container83");P.OpenObject=O;auto& R=P.World->Containers.FindOrAdd(O->RecordId);R.Unlocked=true;R.Width=12;R.Height=10;R.Context=TEXT("Supplies");auto Item=LWItems::Make(TEXT("scrap"),17);LWItems::Place(R.Items,Item,12,10);},[this](ALWCharacter&){CaptureV2(TEXT("Update83_Container"));});
 Add(TEXT("83 take all mouse input"),.5,[Click](ALWCharacter& P){Click(P,TEXT("inv_all55"));},[this](ALWCharacter& P){Check(P.CountSupply(TEXT("scrap"))==17&&P.World->Containers[TEXT("container83")].Items.IsEmpty(),TEXT("single click on reflowed take-all transfers items"));Check(!P.bTrigger,TEXT("container click does not fire weapon"));});
 Add(TEXT("83 sort mouse input"),.5,[Click](ALWCharacter& P){Click(P,TEXT("inv_sort55"));},[this](ALWCharacter& P){Check(P.CountSupply(TEXT("scrap"))==17,TEXT("sort preserves transferred contents"));});
 Add(TEXT("83 cleanup"),.2,[S](ALWCharacter& P){P.ClosePanels();P.DrainSave40();if(S->RV.IsValid())S->RV->Destroy();if(S->Road.IsValid())S->Road->Destroy();});
 for(auto& Step:V2->Steps)Step.Ready=[Frames=0](ALWCharacter&)mutable{return ++Frames>=20;};
 return;
}
