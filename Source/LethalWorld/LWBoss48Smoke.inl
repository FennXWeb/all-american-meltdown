#include "LWBoss48.h"
void ALWGameMode::BuildBoss48Smoke(ALWCharacter& Initial){
 auto Add=[this](FString N,FLWV2Action B){V2->Steps.Add({N,.5,120,MoveTemp(B),FLWV2Action(),[](ALWCharacter&){return true;}});};
 Add(TEXT("large enemy spawn integration"),[this](ALWCharacter& P){P.NewGame();P.EnterSafehouse();for(TActorIterator<ALWZombie> I(GetWorld());I;++I)if(int(I->Kind)>=9)I->Destroy();
 const int Before=P.World->ZombieCount;P.World->ZombieCount=100;int Found=0;
 for(int Kind=9;Kind<=11;++Kind){bool Spawned=false;
  for(int Index=0;Index<2000&&!Spawned;++Index){FIntPoint Coord(60+Index%50,60+Index/50);uint32 Id=LWGen::Hash(Coord.X,Coord.Y,P.World->Seed,29010);if(LWBoss48::Kind(Id)!=Kind)continue;
   FVector Origin(Coord.X*LWGen::ChunkSize,Coord.Y*LWGen::ChunkSize,0);TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Gather(FVector2D(Origin)+FVector2D(6400),P.World->Seed,Roads,Sites);FRandomStream R(Id^0x4819u);FVector2D At;bool Clear=false;
   for(int Try=0;Try<24;++Try){At=LWBoss48::Candidate(Coord,R);float Yaw=R.FRandRange(0,360);if(LWBoss48::Clear(At,Kind,Yaw,Roads,Sites)){Clear=true;break;}}if(!Clear)continue;
   auto* C=GetWorld()->SpawnActor<ALWChunk>(Origin,FRotator::ZeroRotator);C->Coordinate=Coord;float Ground=P.World->HeightAt(At);C->Box(P.World,TEXT("Concrete"),FVector(6400,6400,Ground-20),FVector(20000,20000,40));
   P.World->SpawnRoamingBoss(C);ALWZombie* Boss=nullptr;for(auto& A:C->Residents)if(auto* Z=Cast<ALWZombie>(A))Boss=Z;
   if(Boss){Check(int(Boss->Kind)==Kind,TEXT("rolled large enemy kind materializes"));Check(FMath::Abs(Boss->GetActorLocation().Z-Boss->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()-Ground-12)<1,TEXT("scaled boss rests above ground"));if(Kind>=10)Check(Boss->Stars>=1,TEXT("rare boss is legendary"));int N=C->Residents.Num();P.World->SpawnRoamingBoss(C);Check(C->Residents.Num()==N,TEXT("duplicate spawn blocked"));P.World->KilledZombies.Add(Id);Boss->Destroy();P.World->SpawnRoamingBoss(C);Check(C->Residents.Num()==N,TEXT("killed boss never respawns"));Spawned=true;++Found;}
   C->Destroy();
  }Check(Spawned,*FString::Printf(TEXT("kind %d can spawn with full ordinary population"),Kind));
 }P.World->ZombieCount=Before;Check(Found==3,TEXT("all larger-than-titan enemy types spawn"));});
}
