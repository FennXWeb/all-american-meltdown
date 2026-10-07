#include "LWGeography84.h"
#include "LWBoss48.h"
#include "LWWorld.h"
#include "LWZombie.h"
#include "LWResident.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
void ALWWorld::SpawnRoamingBoss(ALWChunk* C){
 if(!C)return;
 if(LWNY69::WaterDepth(FVector2D(C->GetActorLocation())+FVector2D(6400))>0)return;
 if(C&&LWGeography84::Canada(FVector2D(C->GetActorLocation())))return;
 if(!C)return;const uint32 Id=LWGen::Hash(C->Coordinate.X,C->Coordinate.Y,Seed,29010);const int K=LWBoss48::Kind(Id);
 if(K==INDEX_NONE||KilledZombies.Contains(Id))return;
 // Ordinary population must not consume the entire budget for these encounters.
 int Alive=0;for(TActorIterator<ALWZombie> I(GetWorld());I;++I){if(I->PersistentId==Id)return;if(!I->bDead&&int(I->Kind)>=9&&int(I->Kind)<=11)++Alive;}if(Alive>=1)return;
 const FVector2D Center(C->GetActorLocation()+FVector(LWGen::ChunkSize*.5,LWGen::ChunkSize*.5,0));
 TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Gather(Center,Seed,Roads,Sites);
 auto* Player=UGameplayStatics::GetPlayerPawn(this,0);FRandomStream Rand(Id^0x4819u);
 for(int Attempt=0;Attempt<24;++Attempt){const FVector2D XY=LWBoss48::Candidate(C->Coordinate,Rand);const float Yaw=Rand.FRandRange(0,360);
  if(!LWBoss48::Clear(XY,K,Yaw,Roads,Sites))continue;
  if(Player&&FVector2D::Distance(XY,FVector2D(Player->GetActorLocation()))<(K==9?4500:9000))continue;
  const float Expected=HeightAt(XY);FHitResult Ground;FCollisionQueryParams Query(SCENE_QUERY_STAT(BossSpawn48));
  if(!GetWorld()->LineTraceSingleByChannel(Ground,FVector(XY,Expected+600),FVector(XY,Expected-600),ECC_WorldStatic,Query)||Ground.ImpactNormal.Z<.9f||FMath::Abs(Ground.ImpactPoint.Z-Expected)>80)continue;
  const float Radius=LWBoss48::Radius(K),Half=LWBoss48::HalfHeight(K);FVector At=Ground.ImpactPoint+FVector(0,0,Half+12);
  if(GetWorld()->OverlapBlockingTestByChannel(At,FQuat::Identity,ECC_Pawn,FCollisionShape::MakeCapsule(Radius,Half),Query))continue;
  FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
  auto* Z=GetWorld()->SpawnActor<ALWZombie>(At,FRotator(0,Yaw,0),Params);if(!Z)continue;
  Z->PersistentId=Id;Z->ConfigureKind(ELWEnemyKind(K));Z->SetActorLocation(Ground.ImpactPoint+FVector(0,0,Z->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()+12),false,nullptr,ETeleportType::TeleportPhysics);Z->Home=Z->Interest=Z->GetActorLocation();Z->EnsureLegendary();C->Residents.Add(Z);ZombieCount++;
  UE_LOG(LogTemp,Display,TEXT("BOSS48_SPAWN kind=%d id=%u chunk=%d,%d position=%s"),K,Id,C->Coordinate.X,C->Coordinate.Y,*Z->GetActorLocation().ToString());return;
 }
}
