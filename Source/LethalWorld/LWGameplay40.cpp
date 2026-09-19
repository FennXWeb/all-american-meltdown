#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWSaveGame.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Async/Async.h"
#include "Engine/World.h"
#include "ProfilingDebugging/CpuProfilerTrace.h"
void ALWCharacter::RequestSave40(){if(!bStarted||bLoadingSave||OpeningMode||bWorldSetup)return;if(!SaveDirty40)SaveAge40=0;SaveDirty40=true;SaveQuiet40=0;}
bool ALWCharacter::FinishSave43(){
 const bool OK=SaveWrite40.Get();SaveWrite40=TFuture<bool>();SaveSnapshot43=nullptr;
 if(!OK){SaveDirty40=true;SaveQuiet40=SaveAge40=0;Notify(TEXT("SAVE FAILED // CHECK DISK SPACE"),5);}return OK;
}
void ALWCharacter::StartSave43(){
 if(SaveWrite40.IsValid()||!World||!CheckpointJobs43.IsEmpty())return;
 TRACE_CPUPROFILER_EVENT_SCOPE(LW_SaveCapture43);
 const double Start=FPlatformTime::Seconds();
 SaveSnapshot43=MakeProgressSnapshot37();SaveSnapshot43->MissionRecovery37=MissionRecovery37;
 LastSaveCaptureMs43=(FPlatformTime::Seconds()-Start)*1000;
 const FString Slot=SaveSlot40();auto* Snapshot=SaveSnapshot43.Get();
 SaveDirty40=SaveUrgent43=false;SaveQuiet40=SaveAge40=0;++AutoSaves40;
 // Detached, immutable value-only snapshot. UPROPERTY pins it until game-thread completion.
 SaveWrite40=Async(EAsyncExecution::ThreadPool,[Snapshot,Slot](){
  TRACE_CPUPROFILER_EVENT_SCOPE(LW_SaveSerializeAndWrite43);
  const double Start=FPlatformTime::Seconds();TArray<uint8> Bytes;
  const bool OK=UGameplayStatics::SaveGameToMemory(Snapshot,Bytes)&&UGameplayStatics::SaveDataToSlot(Bytes,Slot,0);
  UE_LOG(LogTemp,Display,TEXT("LW_SAVE43 worker_ms=%.3f bytes=%d success=%d"),(FPlatformTime::Seconds()-Start)*1000,Bytes.Num(),OK);return OK;
 });
}
void ALWCharacter::DrainSave40(){
 FinishCheckpoints43(true);
 // Only load/quit boundaries wait. Ordinary saves never wait for an older writer.
 if(SaveWrite40.IsValid()&&!FinishSave43())return;
 if(SaveUrgent43&&IsValid(World)&&bStarted){StartSave43();if(SaveWrite40.IsValid())FinishSave43();}
}
void ALWCharacter::TickSave40(float Dt){
 FinishCheckpoints43();
 if(SaveWrite40.IsValid()){if(!SaveWrite40.IsReady())return;FinishSave43();}
 if(!SaveDirty40||!bStarted||!World||bLoadingSave||OpeningMode||bWorldSetup)return;
 SaveQuiet40+=Dt;SaveAge40+=Dt;
 // Keep snapshot copying/serialization out of reload, attack and interaction bursts.
 const bool Busy=Traversing54||Sliding54||bReloading||bTrigger||AttackTimer>0||HurtFlash>.1f||World->CombatMusicHold>0;
 if(!SaveUrgent43&&(SaveQuiet40<3||Busy)&&SaveAge40<30)return;
 StartSave43();
}
void ALWCharacter::EndPlay(const EEndPlayReason::Type Reason){FinishCheckpoints43(true);if(SaveDirty40&&IsValid(World)&&bStarted)SaveProgress();DrainSave40();Super::EndPlay(Reason);}
void ALWCharacter::LeanLeftStart40(){if(CanAct())LeanLeft40=true;}
void ALWCharacter::LeanLeftStop40(){LeanLeft40=false;}
void ALWCharacter::LeanRightStart40(){if(CanAct())LeanRight40=true;}
void ALWCharacter::LeanRightStop40(){LeanRight40=false;}
void ALWCharacter::TickLean40(float Dt){if(!Camera||!LeanPivot40)return;
 const bool Allowed=CanAct()&&!bSprint&&!GetCharacterMovement()->IsFalling();if(!Allowed)LeanLeft40=LeanRight40=false;
 const float Target=Allowed?float(LeanRight40)-float(LeanLeft40):0;
 LeanAmount40=FMath::FInterpTo(LeanAmount40,Target,Dt,10.f);
 Camera->ClearAdditiveOffset();const FVector Base=Camera->GetComponentLocation()-GetActorTransform().TransformVectorNoScale(LeanPivot40->GetRelativeLocation());
 const FVector LocalRight=GetActorTransform().InverseTransformVectorNoScale(GetControlRotation().RotateVector(FVector::RightVector));
 FVector Offset=LocalRight*(LeanAmount40*42);const FVector End=Base+GetActorTransform().TransformVectorNoScale(Offset);FHitResult Hit;FCollisionQueryParams Params(SCENE_QUERY_STAT(LWLeanClearance),false,this);
 if(GetWorld()->SweepSingleByChannel(Hit,Base,End,FQuat::Identity,ECC_Visibility,FCollisionShape::MakeSphere(12),Params)){Offset*=FMath::Max(0.f,Hit.Time-.04f);}
 LeanPivot40->SetRelativeLocation(Offset);const float Actual=Offset.Size()/42*(LeanAmount40<0?-1:1);Camera->AddAdditiveOffset(FTransform(FRotator(0,0,Actual*9)),0);
}
void ALWCharacter::QuickHeal40(){if(!CanAct())return;if(Health>=MaxHealth()){Notify(TEXT("HEALTH FULL"),1.5f);return;}const auto* Kit=Inventory.FindByPredicate([](const auto& I){return I.Definition==TEXT("medkit")&&I.Count>0&&I.Slot.IsNone();});if(!Kit){Notify(TEXT("NO MEDKIT"),1.5f);return;}const FGuid Id=Kit->Id;CancelReload();StopAttack();UseItem(Id);Notify(TEXT("MEDKIT USED"),1.5f);}
