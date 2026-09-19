#include "LWTradingCards36.h"
void ALWGameMode::BuildTradingCards36Smoke(ALWCharacter& Initial){
 auto Add=[this](const TCHAR* Label,double Delay,FLWV2Action Begin,FLWV2Action End=FLWV2Action()){V2->Steps.Add({Label,Delay,120,MoveTemp(Begin),MoveTemp(End),[](ALWCharacter&){return true;}});};
 Add(TEXT("Loose collectible"),3,[this](ALWCharacter& P){for(int I=0;I<100;I++)LWCollect36::Artwork(I);P.NewGame();P.RPG.Story.Enabled=false;P.OpeningMode=0;P.bMenu=false;P.ClosePanels();P.SetMenuInput(false);P.Health=100;P.RPG.TradingCards36.Empty();auto* Card=GetWorld()->SpawnActor<ALWTradingCard36>(P.GetActorLocation()+FVector(100,0,-50),FRotator::ZeroRotator);Card->Setup(0);Check(P.CanAct(),TEXT("pickup can act"));Card->Use(&P);Check(P.RPG.TradingCards36.Contains(0),TEXT("loose card acquired"));Check(LWCollect36::Bonus(P.RPG,TEXT("health"))==1,TEXT("pickup applies health"));auto* Duplicate=GetWorld()->SpawnActor<ALWTradingCard36>(P.GetActorLocation()+FVector(100,0,0),FRotator::ZeroRotator);Duplicate->Setup(0);Duplicate->Use(&P);Check(P.RPG.TradingCards36.Num()==1,TEXT("duplicate cannot stack"));P.SaveProgress();P.RPG.TradingCards36.Empty();P.LoadProgress();Check(P.RPG.TradingCards36.Contains(0),TEXT("disk save restores card"));});
 Add(TEXT("Seeded loose surface placement"),2,[this](ALWCharacter& P){
  auto* Chunk=GetWorld()->SpawnActor<ALWChunk>();LWGen::FSite S;S.Position=FVector2D(2000,0);S.Size=FVector2D(500,500);
  for(S.Id=1;S.Id<100000;S.Id++){uint32 H=LWGen::Hash(S.Id,36,P.World->Seed,36101);if(H%100<46&&H%13==0&&LWCollect36::Select(H^0xabc391u)==20)break;}
  auto* Surface=GetWorld()->SpawnActor<ALWWorldObject>(FVector(S.Position,4000),FRotator::ZeroRotator);Surface->Kind=ELWObjectKind::Furniture;Surface->Body->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube")));Surface->SetActorScale3D(FVector(3,3,1));Chunk->Residents.Add(Surface);
  LWCollect36::Spawn(Chunk,P.World,S);ALWTradingCard36* Found=nullptr;for(auto& A:Chunk->Residents)if(auto* C=Cast<ALWTradingCard36>(A))Found=C;
  Check(Found!=nullptr,TEXT("seeded POI produces loose pickup"));if(Found){Check(Found->CardIndex==20,TEXT("seed determines collectible identity"));Check(FMath::Abs(Found->GetActorLocation().Z-4050.4)<1,TEXT("card rests flush on exposed surface"));Found->Destroy();}
  Surface->Destroy();Chunk->Destroy();
 });
 Add(TEXT("Collection grid"),3,[this](ALWCharacter& P){P.ClosePanels();P.bMenu=false;for(int I:{0,1,2,3,4,5,6,7,8,9})LWCollect36::Collect(P.RPG,I);P.SwitchTab(5);Check(P.RPGPanel==6&&!P.CanAct(),TEXT("collection tab blocks combat"));P.CollectionSelection36=0;},[this](ALWCharacter& P){CaptureV2(TEXT("TradingCards36_Collection"));});
 Add(TEXT("Legendary artwork"),2,[](ALWCharacter& P){P.CollectionSelection36=9;},[this](ALWCharacter& P){CaptureV2(TEXT("TradingCards36_Legendary"));});
 Add(TEXT("Bonus summary"),2,[](ALWCharacter& P){P.CollectionSelection36=-1;},[this](ALWCharacter& P){CaptureV2(TEXT("TradingCards36_Bonuses"));P.ClosePanels();Check(P.RPGPanel==0,TEXT("collection closes normally"));});
 Add(TEXT("Asset completeness"),1,[this](ALWCharacter& P){if(!FParse::Param(FCommandLine::Get(),TEXT("TradingCardsPartial")))for(int I=0;I<100;I++)Check(LWCollect36::Artwork(I)!=nullptr,TEXT("unique card artwork resolves"));P.Respawn();Check(P.RPG.TradingCards36.Num()==10,TEXT("collection survives respawn"));});
 Add(TEXT("capture flush"),2,[](ALWCharacter& P){});
}
