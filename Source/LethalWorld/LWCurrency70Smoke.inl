if(FParse::Param(FCommandLine::Get(),TEXT("LWCurrency70Smoke"))){
 Add(TEXT("Currency70 setup"),1,[this](ALWCharacter& P){P.NewGame();P.RPG.Story.Enabled=false;P.World->EnableEncounters=false;P.World->SetActorTickEnabled(false);P.SetActorTickEnabled(false);P.ClosePanels();P.bMenu=false;P.bSafehouse=false;P.bGod47=true;UGameplayStatics::SetGamePaused(&P,false);P.SetMenuInput(false);P.GetCharacterMovement()->DisableMovement();P.World->TimeOfDay=12;P.World->WeatherOverride=0;P.World->TickWeather(.1,&P);P.WeaponRoot->SetVisibility(false,true);P.RPG.Canada68.Passport=P.RPG.Canada68.Cleared=true;P.Money=10000;});
 Add(TEXT("Currency70 exchange"),3,[this](ALWCharacter& P){
  P.SetActorLocation(FVector(LWGen::CanadaCity68()+FVector2D(13500,-13500),150));P.World->Stream(P.GetActorLocation(),true);
  ALWResident* Clerk=nullptr;for(TActorIterator<ALWResident> N(GetWorld());N;++N)if(N->NpcRole==TEXT("exchange70")){Clerk=*N;break;}Check(Clerk!=nullptr,TEXT("exchange clerk spawned at Toronto bank"));if(!Clerk)return;
  P.SetActorLocation(Clerk->GetActorLocation()+FVector(-300,0,0));P.Speaker=Clerk;P.RPGPanel=4;P.SetMenuInput(true);P.BuildDialogue(TEXT("root"));P.Controller->SetControlRotation((Clerk->GetActorLocation()-P.GetActorLocation()).Rotation());P.World->Sky->SetActorLocation(P.GetActorLocation());P.World->TickWeather(.1,&P);
  Check(LWCurrency70::Exchange(&P,1000,true),TEXT("clerk converts credits to CAD"));Check(P.Money==9000&&P.RPG.Canada68.CanadianDollars>0,TEXT("exchange debits one wallet and credits the other"));Check(P.DialogueActions.Num()==7,TEXT("both conversion directions and denominations offered"));
 },[this](ALWCharacter&){CaptureV2(TEXT("Currency70_Exchange"));});
 Add(TEXT("Currency70 merchant"),2,[this](ALWCharacter& P){
  P.ClosePanels();ALWResident* Merchant=nullptr;for(TActorIterator<ALWResident> N(GetWorld());N;++N)if(N->NpcRole==TEXT("canada_shop68")){Merchant=*N;break;}Check(Merchant!=nullptr,TEXT("Toronto supply merchant exists"));if(!Merchant)return;
  P.SetActorLocation(Merchant->GetActorLocation()+FVector(-250,0,0));P.Speaker=Merchant;P.BuildDialogue(TEXT("root"));LWCanada68::Action(&P,TEXT("ca_trade"));Check(LWCurrency70::CanadianTrade(&P),TEXT("Canadian trader selects CAD wallet"));auto* Items=P.ItemsFor(2);if(!Items||Items->IsEmpty()){Check(false,TEXT("shop stock available"));return;}const FGuid Id=(*Items)[0].Id;const int Cost=LWCanada68::TradePrice(&P,(*Items)[0],true);
  P.Inventory.Empty();P.RPG.Canada68.CanadianDollars=0;const int64 Credits=P.Money;
  Check(!P.MoveItem(2,0,Id,0,0,false,NAME_None),TEXT("credits cannot purchase Canadian goods"));P.RPG.Canada68.CanadianDollars=Cost+100;
  Check(P.MoveItem(2,0,Id,0,0,false,NAME_None),TEXT("CAD purchases Canadian goods"));Check(P.RPG.Canada68.CanadianDollars==100&&P.Money==Credits,TEXT("purchase leaves credits untouched"));
  const int Gain=LWCanada68::TradePrice(&P,P.Inventory[0],false);Check(P.MoveItem(0,2,Id,0,0,false,NAME_None),TEXT("sell item back to Canadian merchant"));Check(P.RPG.Canada68.CanadianDollars==100+Gain&&P.Money==Credits,TEXT("sale pays CAD only"));
 },[this](ALWCharacter&){CaptureV2(TEXT("Currency70_Shop"));});
 Add(TEXT("Currency70 map"),1,[this](ALWCharacter& P){P.ClosePanels();P.SwitchTab(1);if(auto* H=Cast<ALWHUD>(Cast<APlayerController>(P.Controller)->GetHUD())){H->MapZoom=.0004;H->MapPan=FVector2D(1200000,650000)-FVector2D(P.GetActorLocation());}},[this](ALWCharacter&){CaptureV2(TEXT("Currency70_Map"));});
 return;
}
