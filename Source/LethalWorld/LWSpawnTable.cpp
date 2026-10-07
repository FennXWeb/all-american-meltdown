#include "LWGeography84.h"
#include "LWCampaign76.h"
#include "LWStreaming68.h"
#include "LWSpawnTable.h"
#include "LWWorld.h"
#include "LWZombie.h"
#include "LWCharacter.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
ULWSpawnTable::ULWSpawnTable(){for(int T=0;T<LWPlaces::Count;T++){auto& R=POIs.AddDefaulted_GetRef();R.POIType=T;R.MinCount=1;R.MaxCount=(T==6||T==10||T==11||T==12||T==13)?5:3;R.ZombieWeight=T==17?30:70;R.RaiderWeight=T==17?65:T==13?45:15;R.DogWeight=T==14||T==16?35:10;if(T>=64){R.MinCount=3;R.MaxCount=8;R.RogueWeight=T==64?45:T==65?30:5;R.RaiderWeight=40;R.ZombieWeight=25;}if(T==18||T==19||T==21)R.MinCount=R.MaxCount=0;if(LWPlaces::Expanded(T)){R.MinCount=T==58||T==59?5:T==63?4:2;R.MaxCount=T==58||T==59||T==63?8:4;R.RaiderWeight=T==59?65:T==61||T==62?35:10;R.ZombieWeight=T==59?30:70;R.IndoorChance=T==63?.35f:.85f;}}}
void ALWWorld::SpawnEnemies(ALWChunk* C,const LWGen::FSite* Site,FVector2D Footprint,int Floors,float FloorHeight){
 if(!Site&&LWNY69::WaterDepth(FVector2D(C->GetActorLocation())+FVector2D(6400))>0)return;
 if(C&&LWGeography84::Canada(FVector2D(C->GetActorLocation())))return;
 if(C&&C->Plan68&&!C->Plan68->RunningPopulation&&Site){const auto Copy=*Site;C->Plan68->Population.Add([this,C,Copy,Footprint,Floors,FloorHeight](){SpawnEnemies(C,&Copy,Footprint,Floors,FloorHeight);});return;}
 if(!SpawnTable)SpawnTable=LoadObject<ULWSpawnTable>(nullptr,TEXT("/Game/Data/DA_EnemySpawns.DA_EnemySpawns"));if(!SpawnTable)SpawnTable=NewObject<ULWSpawnTable>(this);
 if(auto* StoryPlayer=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));StoryPlayer&&StoryPlayer->RPG.Campaign76.Started&&Site&&LWCampaign76::Reserved(Site->Position))return;
 if(!C||Site&&(Site->Friendly||Site->Type==18||Site->Type==19||Site->Type==2&&Site->Position.Size()<10000))return;
 FLWSpawnRow Row;if(Site){const auto* R=SpawnTable->POIs.FindByPredicate([&](const auto& E){return E.POIType==Site->Type;});if(!R){if(Site->Type>=64){Row.RaiderWeight=45;Row.RogueWeight=35;Row.MinCount=3;Row.MaxCount=8;}else if(!LWPlaces::IsTower(Site->Type))return;Row.MinCount=2;Row.MaxCount=6;Row.IndoorChance=.85f;}else Row=*R;}
 else {Row.MinCount=1;Row.MaxCount=2;Row.IndoorChance=0;Row.ZombieWeight=55;Row.RaiderWeight=5;Row.DogWeight=40;Row.MooseWeight=35;Row.TitanWeight=2;Row.DeathclawWeight=12;Row.ScorpionWeight=20;Row.KarenWeight=0;Row.BearWeight=25;Row.HornetWeight=25;Row.RogueWeight=4;}
 if(Site&&LWPlaces::IsTower(Site->Type)&&Site->AccessibleFloors==0)Row.IndoorChance=0;
 const uint32 Base=Site?Site->Id:LWGen::Hash(C->Coordinate.X,C->Coordinate.Y,Seed,0xe771a);
 if(Site&&!Site->SettlementBuilding&&!Site->Friendly&&Row.MaxCount>0&&!LWPlaces::IsTower(Site->Type)&&!LWDungeons::IsDungeon(Site->Type)){
 const int Mood=LWGen::Hash(int32(Base),Site->Type,Seed,53010)%100;
 if(Mood<12)return;
 if(Mood<34){Row.MinCount=1;Row.MaxCount=2;}else if(Mood<65){Row.MinCount=3;Row.MaxCount=5;}else{Row.MinCount=6;Row.MaxCount=FMath::Min(12,6+int(Footprint.Size()/1200));if(Mood>88){Row.RaiderWeight=100;Row.ZombieWeight=Row.DogWeight=Row.MooseWeight=Row.TitanWeight=Row.DeathclawWeight=Row.ScorpionWeight=Row.KarenWeight=Row.BearWeight=Row.HornetWeight=Row.RogueWeight=0;}}
 }
 FRandomStream R(Base^uint32(Seed)^0xb281ffu);if(!Site&&R.FRand()>FMath::Clamp(SpawnTable->WildernessChance,0.f,1.f))return;
 float Sum=0;for(float Weight:{Row.ZombieWeight,Row.RaiderWeight,Row.DogWeight,Row.MooseWeight,Row.TitanWeight,Row.DeathclawWeight,Row.ScorpionWeight,Row.KarenWeight,Row.HornetWeight,Row.BearWeight,Row.RogueWeight})if(FMath::IsFinite(Weight))Sum+=FMath::Max(0.f,Weight);if(Sum<=0)return;
 if(Row.MaxCount<=0)return;
 const int Count=FMath::Clamp(R.RandRange(FMath::Clamp(Row.MinCount,0,12),FMath::Clamp(FMath::Max(Row.MinCount,Row.MaxCount),0,12))+(Difficulty==2?1:Difficulty==0?-1:0),0,12);
 auto* Player=UGameplayStatics::GetPlayerPawn(this,0);
 for(int I=0;I<Count&&ZombieCount<FMath::Clamp(SpawnTable->MaxAlive,0,150);I++){
 const uint32 Id=Base^(0x65a3b271u*uint32(I+1));if(KilledZombies.Contains(Id))continue;
 for(int Try=0;Try<32;Try++){
 const float Weights[]={Row.ZombieWeight,Row.RaiderWeight,Row.DogWeight,0,Row.MooseWeight,Row.TitanWeight,Row.DeathclawWeight,Row.ScorpionWeight,Row.KarenWeight,0,0,0,Row.HornetWeight,Row.BearWeight,Row.RogueWeight};float Total=0;for(float Weight:Weights)Total+=FMath::IsFinite(Weight)?FMath::Max(0.f,Weight):0;float Roll=R.FRand()*Total;int Pick=0;for(int J=0;J<15;J++){Roll-=FMath::IsFinite(Weights[J])?FMath::Max(0.f,Weights[J]):0;if(Roll<=0){Pick=J;break;}}ELWEnemyKind Kind=ELWEnemyKind(Pick);
 FVector At;bool Indoor=Site&&(Pick<4||Pick==14)&&R.FRand()<Row.IndoorChance;
 if(Site){FVector Local(R.FRandRange(-Footprint.X*.5+110,Footprint.X*.5-110),R.FRandRange(-Footprint.Y*.5+110,Footprint.Y*.5-110),0);if(!Indoor){Local.X=FMath::Sign(Local.X)*(Footprint.X*.5+R.FRandRange(180,600));}At=FVector(Site->Position,12)+FRotator(0,Site->Yaw,0).RotateVector(Local);At.Z+=Indoor?24+R.RandRange(0,FMath::Max(0,Floors-1))*FloorHeight+160:200;}
 else{At=C->GetActorLocation()+FVector(R.FRandRange(600,LWGen::ChunkSize-600),R.FRandRange(600,LWGen::ChunkSize-600),0);At.Z=HeightAt(FVector2D(At))+200;TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Gather(FVector2D(At),Seed,Roads,Sites);bool Near=false;for(const auto& S:Sites)if(FVector2D::Distance(S.Position,FVector2D(At))<S.Size.Size()*.5+1800)Near=true;if(Near)continue;}
 if(LWStory::Reserved(FVector2D(At))||IsSafePosition(At)||Player&&FVector::Dist2D(At,Player->GetActorLocation())<1100)continue;
 FHitResult H;FCollisionQueryParams Q;if(!GetWorld()->LineTraceSingleByChannel(H,At,At-FVector(0,0,350),ECC_Visibility,Q)||H.ImpactNormal.Z<.85f)continue;
 // Reject furniture tops and roofs; only the intended floor is valid.
 const float Expected=Indoor?At.Z-160:HeightAt(FVector2D(At));if(FMath::Abs(H.ImpactPoint.Z-Expected)>38)continue;
 const float Half[]={88,88,88,88,130,152,135,70,92,456,1368,150,50,100,94},Radius[]={40,40,40,40,100,65,90,70,65,195,585,150,48,80,38};
 FVector ClearAt=H.ImpactPoint+FVector(0,0,Half[Pick]+6);if(GetWorld()->OverlapBlockingTestByChannel(ClearAt,FQuat::Identity,ECC_Pawn,FCollisionShape::MakeCapsule(Radius[Pick],FMath::Max(Radius[Pick],Half[Pick])),Q))continue;
 At=H.ImpactPoint+FVector(0,0,94);
 const FRotator Rotation(0,R.FRandRange(0,360),0);auto Spawn=[this,C,At,Rotation,Id,Kind](){
 if(ZombieCount>=FMath::Clamp(SpawnTable->MaxAlive,0,150)||KilledZombies.Contains(Id))return;
 FActorSpawnParameters P;P.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;auto* Z=GetWorld()->SpawnActor<ALWZombie>(At,Rotation,P);if(!Z)return;
 Z->PersistentId=Id;Z->ConfigureKind(Kind);Z->Home=At;C->Residents.Add(Z);ZombieCount++;
 };if(C->Plan68)C->Plan68->Population.Add(MoveTemp(Spawn));else Spawn();break;
 }
 }
}
