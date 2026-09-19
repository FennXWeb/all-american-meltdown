void ALWGameMode::BuildSave43Smoke(ALWCharacter& Initial){
 auto Add=[this](const TCHAR* N,double Delay,FLWV2Action Begin){V2->Steps.Add({N,Delay,120,MoveTemp(Begin),FLWV2Action(),[](ALWCharacter&){return true;}});};
 Add(TEXT("save stress setup"),1,[](ALWCharacter& P){P.NewGame();P.RPG.Story.Enabled=false;P.DrainSave40();P.World->SetActorTickEnabled(false);P.SetActorTickEnabled(false);});
 Add(TEXT("large world serialization and ordered writes"),.1,[this](ALWCharacter& P){
  for(int I=0;I<3000;++I){auto& C=P.World->Containers.FindOrAdd(FName(*FString::Printf(TEXT("save43_%d"),I)));C.Items=P.Inventory;C.Position=FVector(I*100,0,0);}
  auto& R=P.MissionRecovery37.FindOrAdd(TEXT("stress43"));R.Start.Init(17,4*1024*1024);R.Checkpoint.Init(23,4*1024*1024);
  auto* Snapshot=P.MakeProgressSnapshot37();Snapshot->MissionRecovery37=P.MissionRecovery37;
  TArray<uint8> Baseline;const double B=FPlatformTime::Seconds();UGameplayStatics::SaveGameToMemory(Snapshot,Baseline);const double BaselineMs=(FPlatformTime::Seconds()-B)*1000;
  P.Money=431;P.SaveProgress();const double Begin=FPlatformTime::Seconds();P.TickSave40(0);const double CaptureMs=(FPlatformTime::Seconds()-Begin)*1000;
  Check(P.SaveWrite40.IsValid()&&P.SaveSnapshot43!=nullptr,TEXT("save dispatched with pinned immutable snapshot"));
  P.Money=432;P.World->Containers.FindChecked(TEXT("save43_0")).Items.Empty();
  Check(P.SaveSnapshot43->Money==431&&!P.SaveSnapshot43->WorldContainers.FindChecked(TEXT("save43_0")).Items.IsEmpty(),TEXT("in-flight snapshot is isolated from live mutations"));
  const double Requests=FPlatformTime::Seconds();for(int I=0;I<1000;++I)P.SaveProgress();const double RequestMs=(FPlatformTime::Seconds()-Requests)*1000;
  Check(RequestMs<10&&P.SaveUrgent43,TEXT("repeated explicit saves coalesce without waiting"));
  // Force GC while the worker owns the immutable snapshot.
  CollectGarbage(RF_NoFlags);
  P.DrainSave40();
  auto* Read=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromSlot(P.SaveSlot40(),0));
  Check(Read&&Read->Money==432,TEXT("queued latest save wins over earlier writer"));
  Check(Read&&Read->WorldContainers.FindChecked(TEXT("save43_0")).Items.IsEmpty(),TEXT("latest world changes survive queued write"));
  Check(Read&&Read->MissionRecovery37.FindChecked(TEXT("stress43")).Checkpoint.Num()==4*1024*1024,TEXT("large checkpoint payload round trips unchanged"));
  Check(!P.SaveWrite40.IsValid()&&!P.SaveSnapshot43&&!P.SaveUrgent43,TEXT("flush releases snapshot and drains writer"));
  Check(CaptureMs<BaselineMs,TEXT("game-thread capture cheaper than previous synchronous serialization"));
  UE_LOG(LogTemp,Display,TEXT("LW_SAVE43_BENCHMARK baseline_serialize_ms=%.3f capture_dispatch_ms=%.3f requests1000_ms=%.3f bytes=%d"),BaselineMs,CaptureMs,RequestMs,Baseline.Num());
  P.MissionRecovery37.Empty();P.World->Containers.Empty();P.SaveProgress();P.DrainSave40();
 });
}
