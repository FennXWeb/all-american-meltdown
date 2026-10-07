#if WITH_DEV_AUTOMATION_TESTS
if(FParse::Param(FCommandLine::Get(),TEXT("LWLoading79Smoke"))){
 auto Overlay=MakeShared<TSharedPtr<SWidget>>();
 for(int32 Art=0;Art<6;++Art)Add(FString::Printf(TEXT("79 loading artwork %d"),Art),.7,[Overlay,Art](ALWCharacter& P){
  auto* View=P.GetWorld()->GetGameViewport();if(Overlay->IsValid())View->RemoveViewportWidgetContent(Overlay->ToSharedRef());
  *Overlay=LWLoading79::Preview(TEXT("Loading world"),Art*18.+5.);View->AddViewportWidgetContent(Overlay->ToSharedRef(),10000);
 },[this,Art](ALWCharacter&){CaptureV2(*FString::Printf(TEXT("Loading79_Art%d"),Art));});
 Add(TEXT("79 accessible loading"),.7,[Overlay](ALWCharacter& P){auto* View=P.GetWorld()->GetGameViewport();View->RemoveViewportWidgetContent(Overlay->ToSharedRef());*Overlay=LWLoading79::Preview(TEXT("Restoring survivor"),55,true,true);View->AddViewportWidgetContent(Overlay->ToSharedRef(),10000);},[this](ALWCharacter&){CaptureV2(TEXT("Loading79_Accessible"));});
 Add(TEXT("79 movie runs while main thread blocked"),.5,[this,Overlay](ALWCharacter& P){
  P.GetWorld()->GetGameViewport()->RemoveViewportWidgetContent(Overlay->ToSharedRef());Overlay->Reset();
  const auto Before=LWLoading79::Telemetry();double FinishedAt=0;
  {FLWLoadingScope45 Loading(TEXT("Loading world"));Check(Loading.Active,TEXT("real movie-player loading screen started"));FPlatformProcess::Sleep(20.f);FinishedAt=FPlatformTime::Seconds();}
  const auto After=LWLoading79::Telemetry();Check(After.Paints>Before.Paints+30,TEXT("loading widget paints while game thread is blocked"));Check(After.Art>=Before.Art+2,TEXT("artwork rotates during blocked load"));Check(After.Message>=Before.Message+2,TEXT("tips rotate during blocked load"));Check(FPlatformTime::Seconds()-FinishedAt<3,TEXT("loading ends without waiting for next tip"));
  const double ShortStart=FPlatformTime::Seconds();{FLWLoadingScope45 Loading(TEXT("Entering underground"));FPlatformProcess::Sleep(.25f);}
  const auto Next=LWLoading79::Telemetry();Check(Next.Art>After.Art&&Next.Message>After.Message,TEXT("short consecutive loads advance instead of repeating"));Check(FPlatformTime::Seconds()-ShortStart<3,TEXT("short loading screen has no artificial reading delay"));
 });
 Add(TEXT("79 new survivor uses loading screen"),1,[this](ALWCharacter& P){
  const auto Before=LWLoading79::Telemetry();P.BeginWorldSetup();P.WorldSetupClick(TEXT("createworld"));P.bGod47=true;P.EndOpening(true);P.World->EnableEncounters=false;
  Check(LWLoading79::Telemetry().Paints>Before.Paints,TEXT("actual new-game initialization rendered loading artwork"));
 },[this](ALWCharacter& P){Check(P.bStarted&&P.Campaign76&&P.Campaign76->SceneOpen,TEXT("new survivor resumes into opening dialogue"));Check(Cast<APlayerController>(P.Controller)->bShowMouseCursor,TEXT("loading screen preserves conversation mouse input"));CaptureV2(TEXT("Loading79_Resumed"));});
 for(auto& Step:V2->Steps)Step.Ready=[Frames=0](ALWCharacter&)mutable{return ++Frames>=20;};
 return;
}
#endif
