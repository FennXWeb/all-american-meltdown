if(FParse::Param(FCommandLine::Get(),TEXT("LWMenu81Smoke"))){
 struct FState81{int Frames=0;FString OriginalName;int64 OriginalMoney=0;};auto S=MakeShared<FState81>();
 auto HUD=[](ALWCharacter& P){return Cast<ALWHUD>(Cast<APlayerController>(P.Controller)->GetHUD());};
 auto Click=[this,HUD](ALWCharacter& P,FName Id){
  auto* H=HUD(P);const auto* C=H->Controls55.FindByPredicate([&](const auto& Item){return Item.Id==Id;});
  if(!RequireV2(C!=nullptr,*FString::Printf(TEXT("button exists: %s"),*Id.ToString())))return;
  auto* V=GetWorld()->GetGameViewport()->GetGameViewport();auto W=V->GetViewportWidget().Pin();if(!W)return;
  const FGeometry& G=W->GetCachedGeometry();const FVector2D Local=C->Center*H->Scale+H->Origin55;
  auto* Hit=H->GetHitBoxAtCoordinates(Local,true);Check(Hit&&Hit->GetName()==Id,TEXT("topmost hitbox matches tile"));
  const FVector2D Point=G.LocalToAbsolute(Local);TSet<FKey> Keys;Keys.Add(EKeys::LeftMouseButton);
  FPointerEvent Down(0,0,Point,Point,Keys,EKeys::LeftMouseButton,0,FModifierKeysState());V->OnMouseButtonDown(G,Down);
  Keys.Empty();FPointerEvent Up(0,0,Point,Point,Keys,EKeys::LeftMouseButton,0,FModifierKeysState());V->OnMouseButtonUp(G,Up);
 };
 Add(TEXT("81 fixture and async preview"),2,[this,HUD,S](ALWCharacter& P){
  P.SetMenuInput(true);S->OriginalName=P.Identity.Name;S->OriginalMoney=P.Money;
  auto* Save=Cast<ULWSaveGame>(UGameplayStatics::CreateSaveGameObject(ULWSaveGame::StaticClass()));
  Save->Version=2;Save->Identity.Name=TEXT("Alex Mercer");Save->Identity.StyleVersion35=1;Save->Identity.Hair=4;Save->Identity.Top35=3;Save->Identity.Beard35=2;Save->RPG.Level=17;Save->Money=47825;Save->RPG.Canada68.CanadianDollars=645;Save->MenuLocation81=TEXT("Syracuse");Save->MenuMission81=TEXT("The Country We Left Behind");Save->DayNumber=12;
  Save->Position=FVector(LWNewGame72::StartXY(),250);Save->RPG.Campaign76.Paused=true;
  Check(UGameplayStatics::SaveGameToSlot(Save,LWSaves62::Manual(20),0),TEXT("isolated menu fixture written"));
  auto* H=HUD(P);if(!H->Menu81)H->Menu81=NewObject<ULWMainMenu81>(H);H->Menu81->Refresh(LWSaves62::Manual(20));
 },[this,HUD,S](ALWCharacter& P){
  auto* H=HUD(P);auto* M=H->Menu81.Get();
  Check(M&&M->Ready&&!M->Loading,TEXT("async preview ready"));if(!M||!M->Ready)return;
  Check(M->Credits==47825&&M->CanadianDollars==645&&M->Level==17,TEXT("saved balances and level"));
  Check(M->Identity.Name==TEXT("Alex Mercer")&&M->Place==TEXT("Syracuse")&&M->Mission==TEXT("The Country We Left Behind"),TEXT("saved identity location mission"));
  Check(P.Identity.Name==S->OriginalName&&P.Money==S->OriginalMoney&&!P.bStarted,TEXT("preview does not apply save or start game"));
  Check(M->Portrait&&M->Portrait->Parts.Num()>10&&M->Portrait->Target,TEXT("saved survivor rendered by real mesh capture"));
  Check(H->Skyline81&&H->Foreground81&&H->Mist81,TEXT("all generated artwork imported"));
  Check(H->Controls55.Num()==6,TEXT("main menu has six actions"));
  int VX=0,VY=0;Cast<APlayerController>(P.Controller)->GetViewportSize(VX,VY);
  for(const auto& C:H->Controls55)Check(C.Center.X>=0&&C.Center.X<=VX/H->Scale&&C.Center.Y>=0&&C.Center.Y<=720,TEXT("tile stays inside viewport"));
  CaptureV2(TEXT("Menu81_Continue"));
 });
 V2->Steps.Last().Ready=[HUD](ALWCharacter& P){return HUD(P)->Menu81&&HUD(P)->Menu81->Ready;};
 Add(TEXT("81 single click update notes"),.7,[Click](ALWCharacter& P){Click(P,TEXT("news81"));},[this,HUD,S](ALWCharacter& P){auto* H=HUD(P);Check(H->News81,TEXT("first click opens release notes"));Check(H->Controls55.Num()==5&&!H->Controls55.ContainsByPredicate([](const auto& C){return C.Id==TEXT("Start");}),TEXT("notes own navigation without clickthrough"));S->Frames=H->Menu81->Portrait->CaptureCount;CaptureV2(TEXT("Menu81_WhatsNew"));});
 Add(TEXT("81 notes history"),.6,[Click](ALWCharacter& P){Click(P,TEXT("release81_1"));},[this,HUD,S](ALWCharacter& P){auto* H=HUD(P);Check(H->Release81==1,TEXT("older release selected"));Check(H->Menu81->Portrait->CaptureCount==S->Frames,TEXT("hidden portrait does not render"));Check(H->UIKey55(EKeys::Escape)&&!H->News81,TEXT("escape closes release notes"));});
 Add(TEXT("81 keyboard and controller tiles"),.6,[](ALWCharacter&){},[this,HUD](ALWCharacter& P){auto* H=HUD(P);H->Focus55=TEXT("Start");Check(H->UIKey55(EKeys::Right)&&H->Focus55==TEXT("New"),TEXT("right arrow chooses nearest tile"));Check(H->UIKey55(EKeys::Gamepad_DPad_Right)&&H->Focus55==TEXT("Load62"),TEXT("controller right navigates tiles"));H->Focus55=TEXT("news81");Check(H->UIKey55(EKeys::Gamepad_FaceButton_Bottom)&&H->News81,TEXT("controller accept opens notes"));});
 Add(TEXT("81 controller back"),.5,[](ALWCharacter&){},[this,HUD](ALWCharacter& P){auto* PC=Cast<APlayerController>(P.Controller);FInputKeyEventArgs E;E.Key=EKeys::Gamepad_FaceButton_Right;E.Event=IE_Pressed;E.AmountDepressed=1;PC->InputKey(E);E.Event=IE_Released;E.AmountDepressed=0;PC->InputKey(E);Check(!HUD(P)->News81,TEXT("real controller back input closes notes"));ULWPlayerInput51::Get51(&P)->Controller58=false;});
 Add(TEXT("81 reduced motion"),.6,[HUD](ALWCharacter& P){HUD(P)->ReducedMotion55=true;},[HUD,S](ALWCharacter& P){S->Frames=HUD(P)->Menu81->Portrait->CaptureCount;});
 Add(TEXT("81 frozen capture"),.8,[](ALWCharacter&){},[this,HUD,S](ALWCharacter& P){Check(HUD(P)->Menu81->Portrait->CaptureCount==S->Frames,TEXT("reduced motion stops portrait capture"));HUD(P)->ReducedMotion55=false;});
 Add(TEXT("81 single click settings"),.6,[Click](ALWCharacter& P){Click(P,TEXT("Settings"));},[this](ALWCharacter& P){Check(P.bSettings&&!P.bTrigger,TEXT("settings opens without firing"));});
 Add(TEXT("81 settings back"),.6,[Click](ALWCharacter& P){Click(P,TEXT("Settings"));},[this](ALWCharacter& P){Check(!P.bSettings,TEXT("settings returns to tiles"));});
 Add(TEXT("81 load slots"),.6,[Click](ALWCharacter& P){Click(P,TEXT("Load62"));},[this](ALWCharacter& P){Check(P.SavePanel62==2,TEXT("load game reaches existing save slots"));P.ToggleMenu();});
 Add(TEXT("81 new survivor"),.6,[Click](ALWCharacter& P){Click(P,TEXT("New"));},[this](ALWCharacter& P){Check(P.bWorldSetup,TEXT("new survivor reaches difficulty and character creation"));P.ToggleMenu();});
 Add(TEXT("81 empty save state"),.7,[HUD](ALWCharacter& P){auto* M=HUD(P)->Menu81.Get();M->Revision++;M->StopPortrait();M->Ready=M->Loading=false;M->Error.Empty();M->Slot.Empty();},[this,HUD](ALWCharacter& P){Check(!HUD(P)->Controls55.ContainsByPredicate([](const auto& C){return C.Id==TEXT("Start");}),TEXT("empty Continue cannot be activated"));CaptureV2(TEXT("Menu81_NoSave"));});
 Add(TEXT("81 unreadable save state"),.7,[HUD](ALWCharacter& P){HUD(P)->Menu81->Refresh(TEXT("AAM_Test62_Missing81"));},[this,HUD](ALWCharacter& P){Check(!HUD(P)->Menu81->Ready&&!HUD(P)->Menu81->Error.IsEmpty(),TEXT("missing save fails safely with visible guidance"));});
 Add(TEXT("81 legacy fallback"),.5,[this](ALWCharacter& P){auto* Save=NewObject<ULWSaveGame>();Save->Position=FVector(LWNewGame72::StartXY(),250);Check(!LWMenu81::Location(*Save).IsEmpty()&&LWMenu81::Mission(Save->RPG)==TEXT("No active mission"),TEXT("older saves have location and mission fallback"));},[](ALWCharacter&){});
 Add(TEXT("81 reload exact fixture"),1,[HUD](ALWCharacter& P){HUD(P)->Menu81->Refresh(LWSaves62::Manual(20));},[](ALWCharacter&){});
 V2->Steps.Last().Ready=[HUD](ALWCharacter& P){return HUD(P)->Menu81->Ready;};
 Add(TEXT("81 continue the displayed survivor"),2,[Click](ALWCharacter& P){Click(P,TEXT("Start"));},[this,HUD](ALWCharacter& P){Check(P.bStarted&&P.Money==47825&&P.Identity.Name==TEXT("Alex Mercer"),TEXT("Continue loads exactly the displayed save"));Check(!HUD(P)->Menu81,TEXT("preview resources released after starting"));auto* Save=P.MakeProgressSnapshot37();Check(Save&&!Save->MenuLocation81.IsEmpty()&&!Save->MenuMission81.IsEmpty(),TEXT("future snapshots preserve precise menu labels"));P.World->EnableEncounters=false;});
 for(auto& Step:V2->Steps){Step.TimeoutSeconds=120;auto Ready=Step.Ready;Step.Ready=[Frames=0,Ready](ALWCharacter& P)mutable{return ++Frames>=20&&Ready(P);};}
 return;
}
