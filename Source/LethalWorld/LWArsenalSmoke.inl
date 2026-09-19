void ALWGameMode::BuildArsenalSmoke(ALWCharacter& Initial){
 auto Add=[this](FString N,double W,FLWV2Action B,FLWV2Action E=FLWV2Action()){V2->Steps.Add({N,W,90,MoveTemp(B),MoveTemp(E),[](ALWCharacter&){
#if WITH_EDITOR
 return !GShaderCompilingManager||!GShaderCompilingManager->IsCompiling();
#else
 return true;
#endif
 }});};
 struct FState{int Lighting=1;TWeakObjectPtr<ALWChunk> Floor;TWeakObjectPtr<ALWZombie> Enemy;};auto S=MakeShared<FState>();
 Add(TEXT("arsenal fixtures"),3,[this,S](ALWCharacter& P){P.NewGame();P.LeaveSafehouse();P.World->EnableEncounters=false;P.World->TimeOfDay=12;P.World->TickWeather(10,&P);P.Health=10000;LWV2Teleport(P,FVector(20000,20000,5100));P.Controller->SetControlRotation(FRotator(0,0,0));auto* C=GetWorld()->SpawnActor<ALWChunk>();C->Box(P.World,TEXT("Concrete"),FVector(20000,20000,4990),FVector(8000,8000,20));S->Floor=C;
  for(int T=2;T<5;T++)for(int K=0;K<10;K++)Check(P.World->Material(FName(*FString::Printf(TEXT("WeaponSkin24_%d_%02d"),T,K)))!=nullptr,TEXT("finish material available"));
 });
 for(FName Id:{TEXT("missile_launcher"),TEXT("minigun"),TEXT("sawedoff"),TEXT("desert_eagle"),TEXT("m4"),TEXT("taser"),TEXT("flamethrower")}){
 Add(*(Id.ToString()+TEXT(" reload")),.3,[this,Id](ALWCharacter& P){P.ClosePanels();P.CancelReload();P.StopAttack();P.Inventory.Empty();auto G=LWItems::Make(Id);const auto& D=LWItems::Def(Id);G.Slot=D.EquipSlots[0];G.WeaponTier=3;G.WeaponSkin=3;P.Inventory.Add(G);P.ActiveWeaponId=G.Id;P.Equip(D.WeaponIndex);P.AttackTimer=0;P.bUIAttackHeld=false;
  if(Id==TEXT("missile_launcher")||Id==TEXT("taser")){P.GiveItem(D.AmmoType,2);}else if(Id==TEXT("sawedoff")){auto A=LWItems::Make(TEXT("ammo_12g"),4);LWItems::Place(P.Inventory,A,12,10);}else{auto M=LWItems::Make(D.MagazineType);M.Rounds=FMath::Min(10,D.Capacity);LWItems::Place(P.Inventory,M,12,10);}
  P.Reload();Check(P.bReloading,*(Id.ToString()+TEXT(" starts reload")));P.UpdateReload(.5f);P.UpdateWeapon(0);Check(P.WeaponMesh->GetStaticMesh()!=nullptr,*(Id.ToString()+TEXT(" model loaded")));
 },[this,Id](ALWCharacter&){CaptureV2(*(Id.ToString()+TEXT("24_Reload")));});
 Add(*(Id.ToString()+TEXT(" ammunition")),.2,[this,Id](ALWCharacter& P){P.UpdateReload(10);Check(!P.bReloading,*(Id.ToString()+TEXT(" reload completes")));P.SyncAmmoHUD();Check(P.Shells>0,*(Id.ToString()+TEXT(" ammunition available")));int Before=P.Shells;P.StopAttack();P.AttackTimer=0;P.Attack();if(Id==TEXT("minigun")){P.UpdateWeapon(.75f);P.AttackTimer=0;P.Attack();}P.StopAttack();P.SyncAmmoHUD();Check(P.Shells<Before,*(Id.ToString()+TEXT(" firing consumes real ammo")));},[this,Id](ALWCharacter&){CaptureV2(*(Id.ToString()+TEXT("24_Held")));});
 }
 Add(TEXT("status and healing"),.2,[this,S](ALWCharacter& P){auto* Z=GetWorld()->SpawnActor<ALWZombie>(FVector(20700,20300,5090),FRotator::ZeroRotator);S->Enemy=Z;Z->Health=1000;Z->SetActorTickEnabled(false);FHitResult Hit(Z,Z->Parts[0],Z->Parts[0]->Bounds.Origin,FVector(-1,0,0));P.Health=40;
  ALWWeaponEffect::Impact(&P,Hit,FVector(1,0,0),10,TEXT("Life Steal"),14);Check(Z->ShockTime>0&&P.Health>40,TEXT("taser stun and lifesteal"));Check(Z->TickStatus(.1f),TEXT("stun blocks AI"));Z->ShockTime=0;ALWWeaponEffect::Impact(&P,Hit,FVector(1,0,0),10,TEXT("Toxic"),15);float HP=Z->Health;Z->TickStatus(.6f);Check(Z->Health<HP&&Z->PoisonTime>0&&Z->BurnTime>0,TEXT("toxic and burn apply damage over time"));
 });
 Add(TEXT("projectile blast"),1,[this,S](ALWCharacter& P){auto* Z=S->Enemy.Get();Z->SetActorLocation(FVector(21000,20000,5090));Z->Health=100;FVector From(20700,20000,5090);auto* Rocket=GetWorld()->SpawnActor<ALWWeaponEffect>(From,FRotator::ZeroRotator);Rocket->Initialize(&P,FVector(1,0,0),0,420);},[this,S](ALWCharacter&){Check(S->Enemy->Health<100,TEXT("travelling missile hits and damages target"));});
 Add(TEXT("reflective scene"),1,[this,S](ALWCharacter& P){S->Lighting=LWLighting::Mode();P.StopAttack();P.EquipSlot(TEXT("Melee"));P.Flashlight->SetVisibility(false);P.World->SurfaceWetness=1;P.World->UpdateWetness(0);LWV2Teleport(P,FVector(20000,19900,5090));P.Controller->SetControlRotation(FRotator(-10,0,0));
  S->Floor->Box(P.World,TEXT("PuddleV9"),FVector(20400,19900,5004),FVector(900,700,2));
  S->Floor->Box(P.World,TEXT("WindowGlass"),FVector(20700,19900,5130),FVector(5,700,260));
  for(int I=0;I<3;I++)S->Floor->Box(P.World,I%2?TEXT("Red"):TEXT("Glow"),FVector(21000,19650+I*250,5150),FVector(100,90,300));
 });
 for(int Mode=0;Mode<3;Mode++)Add(TEXT("lighting mode"),4,[this,Mode](ALWCharacter&){LWLighting::Apply(Mode);int Expected=Mode==2&&!LWLighting::HardwareAvailable()?1:Mode;Check(LWLighting::Mode()==Expected,TEXT("lighting selects supported mode"));auto* C=IConsoleManager::Get().FindConsoleVariable(TEXT("r.Lumen.HardwareRayTracing"));Check(C&&C->GetInt()==(Expected==2?1:0),TEXT("hardware ray tracing switch applied"));},[this,Mode](ALWCharacter&){CaptureV2(*FString::Printf(TEXT("Lighting24_%d"),Mode));});
 Add(TEXT("restore lighting"),.2,[S](ALWCharacter&){LWLighting::Apply(S->Lighting);});
 Add(TEXT("cleanup"),.2,[S](ALWCharacter& P){P.StopAttack();if(S->Enemy.IsValid())S->Enemy->Destroy();S->Floor->Destroy();});
}
