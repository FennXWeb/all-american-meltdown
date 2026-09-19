#include "HAL/IConsoleManager.h"
#include "LWGraphics33.h"
#include "LWHUD.h"
void ALWGameMode::BuildRoad33Smoke(ALWCharacter& Initial){
 auto Add=[this](const TCHAR* Name,double Delay,FLWV2Action Begin,FLWV2Action End=FLWV2Action()){V2->Steps.Add({Name,Delay,120,MoveTemp(Begin),MoveTemp(End),[](ALWCharacter&){
#if WITH_EDITOR
 return !GShaderCompilingManager||!GShaderCompilingManager->IsCompiling();
#else
 return true;
#endif
 }});};
 struct FFixture{TArray<ALWChunk*> Chunks;FVector Base;};auto F=MakeShared<FFixture>();
 Add(TEXT("Advanced graphics quality menu"),3,[this](ALWCharacter& P){P.NewGame();P.RPG.Story.Enabled=false;P.bSettings=true;P.SetMenuInput(true);auto* Blur=IConsoleManager::Get().FindConsoleVariable(TEXT("r.MotionBlurQuality"));const int Before=Blur?Blur->GetInt():-1;LWGraphics33::Change(15);Check(Blur&&Blur->GetInt()!=Before,TEXT("graphics toggle changes live renderer value"));LWGraphics33::Change(15);Check(Blur&&Blur->GetInt()==Before,TEXT("graphics toggle restores original value"));Cast<ALWHUD>(Cast<APlayerController>(P.Controller)->GetHUD())->GraphicsPage=1;for(int I=0;I<LWGraphics33::Count;I++)Check(!LWGraphics33::Label(I).IsEmpty(),TEXT("graphics option has a value"));},[this](ALWCharacter& P){CaptureV2(TEXT("Road33_GraphicsQuality"));});
 Add(TEXT("Advanced graphics effects menu"),2,[](ALWCharacter& P){Cast<ALWHUD>(Cast<APlayerController>(P.Controller)->GetHUD())->GraphicsPage=2;},[this](ALWCharacter& P){CaptureV2(TEXT("Road33_GraphicsEffects"));});
 Add(TEXT("Roads across chunk boundary"),5,[this,F](ALWCharacter& P){P.bSettings=P.bMenu=false;P.SetMenuInput(false);P.World->TimeOfDay=12;F->Base=FVector(0,0,60000);TArray<LWGen::FRoad> R{{{8000,5000},{18000,5000},2440,true},{{12800,1000},{12800,11000},1000,false},{{18000,5000},{21000,5600},2440,true},{{21000,5600},{24000,7100},2440,true},{{5000,8000},{10000,8000},780,false},{{7500,8000},{7500,11000},580,false}};
  for(int I=0;I<2;I++){auto* C=GetWorld()->SpawnActor<ALWChunk>(F->Base+FVector(I*12800,0,0),FRotator::ZeroRotator);C->Coordinate={I,0};C->BuildingSurfaces=true;C->Box(P.World,TEXT("Earth"),FVector(6400,6400,-10),FVector(12800,12800,20));C->BuildRoads33(P.World,R,{});C->FlushSurfaces();F->Chunks.Add(C);Check(C->SurfaceMeshes.Num()>=2,TEXT("chunk owns clipped pavement and shoulder"));}
  int Signals=0;for(auto* C:F->Chunks)Signals+=C->SignalLamps.Num();Check(Signals==4,TEXT("junction on boundary creates four heads exactly once"));
  P.SetActorLocation(F->Base+FVector(11500,2350,2100));P.GetCharacterMovement()->DisableMovement();P.Controller->SetControlRotation(FRotator(-30,55,0));
 },[this](ALWCharacter& P){CaptureV2(TEXT("Road33_Intersection"));});
 Add(TEXT("Live traffic phases"),3,[this,F](ALWCharacter& P){for(auto* C:F->Chunks){C->TickTraffic33();for(auto& S:C->SignalLamps)for(int I=0;I<3;I++)Check(S.Lens[I].IsValid()&&S.Lens[I]->GetMaterial(0),TEXT("traffic lenses have valid materials"));}P.SetActorLocation(F->Base+FVector(15900,4400,180));P.Controller->SetControlRotation(FRotator(0,180,0));},[this](ALWCharacter& P){CaptureV2(TEXT("Road33_Traffic"));});
 Add(TEXT("Cleanup"),1,[F](ALWCharacter& P){for(auto* C:F->Chunks)C->Destroy();P.GetCharacterMovement()->SetMovementMode(MOVE_Walking);P.EnterSafehouse();});
}
