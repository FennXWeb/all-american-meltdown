// Included inside BuildV17Smoke; exercises production controls against isolated fixtures.
if(FParse::Param(FCommandLine::Get(),TEXT("LWComedy50Smoke"))){
 auto Add=[this](FString N,double D,FLWV2Action B,FLWV2Action E=FLWV2Action()){V2->Steps.Add({N,D,120,MoveTemp(B),MoveTemp(E),[](ALWCharacter&){
#if WITH_EDITOR
 return !GShaderCompilingManager||!GShaderCompilingManager->IsCompiling();
#else
 return true;
#endif
 }});};
 Add(TEXT("comedy setup"),1,[](ALWCharacter& P){P.NewGame();P.RPG.Story.Enabled=false;P.ClosePanels();P.LeaveSafehouse();P.World->EnableEncounters=false;P.World->TimeOfDay=12;P.World->SetActorTickEnabled(false);P.Health=10000;P.bCrust=false;for(TActorIterator<ALWZombie> I(P.GetWorld());I;++I)I->Destroy();auto* F=P.GetWorld()->SpawnActor<ALWChunk>();F->Box(P.World,TEXT("Concrete"),FVector(20000,20000,4990),FVector(40000,40000,20));LWV2Teleport(P,FVector(20000,20000,5090));P.Controller->SetControlRotation(FRotator(0,0,0));for(TActorIterator<ADirectionalLight> I(P.GetWorld());I;++I){I->SetActorRotation(FRotator(-55,-40,0));I->GetLightComponent()->SetIntensity(5);}});
 for(FName Id:{TEXT("giant_glock"),TEXT("questionable_ak"),TEXT("finger_guns"),TEXT("budget_cut"),TEXT("tenbarrel")}){
  Add(Id.ToString()+TEXT(" equip"),.4,[this,Id](ALWCharacter& P){P.CancelReload();P.StopAttack();P.Inventory.Empty();auto G=LWItems::Make(Id);const auto& D=LWItems::Def(Id);G.Slot=D.EquipSlots[0];P.Inventory.Add(G);P.ActiveWeaponId=G.Id;P.Equip(D.WeaponIndex);P.AttackTimer=0;P.bUIAttackHeld=false;P.BudgetBrace50=0;
   if(!D.AmmoType.IsNone())P.GiveItem(D.AmmoType,20);if(!D.MagazineType.IsNone()){auto M=LWItems::Make(D.MagazineType);M.Rounds=D.Capacity;LWItems::Place(P.Inventory,M,12,10);}
   Check(P.ActiveGun()!=nullptr,TEXT("new gun equips from catalog"));Check(P.WeaponMesh->GetStaticMesh()!=nullptr,TEXT("dedicated gun model loaded"));P.Reload();Check(P.bReloading,TEXT("new gun reload starts"));P.UpdateReload(10);Check(!P.bReloading,TEXT("reload completes"));P.SyncAmmoHUD();Check(P.Weapon==19||P.Shells>0,TEXT("real ammunition loaded"));
   auto Bar=LWItems::Make(TEXT("crowbar"));Bar.Slot=TEXT("Melee");P.Inventory.Add(Bar);P.NextWeapon();Check(P.Weapon==0,TEXT("wheel wraps to melee"));P.NextWeapon();Check(P.Weapon==D.WeaponIndex,TEXT("wheel selects new weapon indices"));P.Inventory.RemoveAll([](const auto& I){return I.Definition==TEXT("crowbar");});
  },[this,Id](ALWCharacter&){CaptureV2(*(TEXT("Comedy50_")+Id.ToString()));});
  Add(Id.ToString()+TEXT(" fire"),.3,[this,Id](ALWCharacter& P){int Before=P.Shells;P.StopAttack();P.AttackTimer=0;P.Attack();if(P.Weapon==20){Check(P.Shells==Before,TEXT("plastic rifle waits for support hand"));P.UpdateWeapon(.3f);P.UpdateWeapon(.01f);}P.StopAttack();P.SyncAmmoHUD();if(P.Weapon==19)Check(P.Shells==Before,TEXT("finger shot needs no ammo"));else Check(P.Shells<Before,TEXT("shot consumes inventory ammunition"));if(P.Weapon==16){Check(P.Shells==0,TEXT("ten barrels expend all ten shells"));Check(P.Recoiling50&&!P.CanAct(),TEXT("shotgun launches player into physics ragdoll"));}});
  if(Id==TEXT("tenbarrel"))Add(TEXT("ragdoll recovery"),9,[](ALWCharacter&){},[this](ALWCharacter& P){Check(!P.Recoiling50&&P.CanAct(),TEXT("player recovers movement after landing"));Check(P.GetActorLocation().X<19500,TEXT("recoil sends player backwards at severe force"));});
  if(Id==TEXT("questionable_ak")){
   Add(TEXT("AK throw pose"),.1,[this](ALWCharacter& P){P.AttackTimer=0;P.Reload();P.UpdateReload(.6f);P.UpdateWeapon(0);Check(P.bReloading,TEXT("AK receiver reload can retain existing magazine"));P.SetActorTickEnabled(false);},[this](ALWCharacter&){CaptureV2(TEXT("Comedy50_AKThrow"));});
   Add(TEXT("AK magazine conservation"),.1,[this](ALWCharacter& P){auto Mag=P.ActiveGun()->LoadedMagazine;int Before=P.Shells;P.SetActorTickEnabled(true);P.UpdateReload(10);P.SyncAmmoHUD();Check(P.ActiveGun()->LoadedMagazine==Mag,TEXT("receiver replacement keeps magazine identity"));Check(P.Shells==Before,TEXT("comedic reload never generates ammunition"));});
  }
 }
 Add(TEXT("seat travel limits"),.5,[this](ALWCharacter& P){for(const auto& Spec:LWTraffic::Specs()){FLWVehicleRecord R;R.VIN=FGuid::NewGuid();R.Model=Spec.Id;R.Position=FVector(22000,20000,5075);R.Unlocked=R.Hotwired=true;FName Id(*(FString(TEXT("qa50_"))+Spec.Id));P.World->Vehicles.Add(Id,R);auto* C=Cast<ALWVehicle>(P.World->SpawnObject(ELWObjectKind::Car,Id,R.Position));if(!RequireV2(C!=nullptr,TEXT("seat test car spawned")))continue;C->SetActorTickEnabled(false);auto Base=C->DriverEye();for(int I=0;I<100;++I)C->AdjustSeat50(1,1,.1f);Check(C->SeatOffset50.Equals(FVector(20,0,15)),TEXT("seat forward and height upper limits"));for(int I=0;I<100;++I)C->AdjustSeat50(-1,-1,.1f);Check(C->SeatOffset50.Equals(FVector(-24,0,-15)),TEXT("seat rearward and height lower limits"));Check(C->DriverEye()==Base,TEXT("camera adjustment does not move physical seat"));C->Destroy();}});
 return;
}
