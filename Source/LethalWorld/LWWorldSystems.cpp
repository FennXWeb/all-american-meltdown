#include "LWGeography84.h"
#include "LWVehicle.h"
#include "Misc/Crc.h"
#include "EngineUtils.h"
#include "LWCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "LWWorld.h"
#include "LWWorldObject.h"
#include "LWInventory.h"
#include "LWLootTable.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"

namespace {
void AddRecordItem(FLWContainerRecord& Record,FLWItemInstance Item)
{
    if(Item.Slot==TEXT("Loaded")){Record.Items.Add(Item);return;}
    Item.Slot=NAME_None;
    if(!LWItems::Place(Record.Items,Item,Record.Width,Record.Height)){
        const auto& D=LWItems::Def(Item.Definition);Record.Width=FMath::Max(Record.Width,Item.bRotated?D.Height:D.Width);
        Item.X=0;Item.Y=Record.Height;Record.Height+=FMath::Max(1,Item.bRotated?D.Width:D.Height);Record.Items.Add(Item);
    }
}
}

ALWWorldObject* ALWWorld::SpawnObject(ELWObjectKind Kind,FName Id,FVector P,FRotator R)
{
    FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    if(Kind==ELWObjectKind::Car){
        for(TActorIterator<ALWVehicle> I(GetWorld());I;++I)if(I->RecordId==Id)return nullptr;
        if(const auto* V=Vehicles.Find(Id)){auto* Player=UGameplayStatics::GetPlayerPawn(this,0);if(Player&&FVector::Dist2D(Player->GetActorLocation(),V->Position)>(LWTraffic::IsAircraft(V->Model)?350000.f:(RenderRadius+1)*LWGen::ChunkSize))return nullptr;P=V->Position;R=V->Rotation;}
        auto* Car=GetWorld()->SpawnActor<ALWVehicle>(P,R,Params);if(Car){Car->Configure(this,Kind,Id);Car->InitializeVehicle();}return Car;
    }
    ALWWorldObject* O=GetWorld()->SpawnActor<ALWWorldObject>(P,R,Params);
    if(O)O->Configure(this,Kind,Id);return O;
}
void ALWWorld::CreateBunker()
{
    auto Box=[&](FVector P,FVector Size,FName Mat){
        auto* A=GetWorld()->SpawnActor<AStaticMeshActor>(P,FRotator::ZeroRotator);A->SetMobility(EComponentMobility::Movable);
        A->GetStaticMeshComponent()->SetStaticMesh(Mesh(TEXT("Cube")));A->GetStaticMeshComponent()->SetMaterial(0,Material(Mat));
        A->SetActorScale3D(Size/100);A->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));BunkerParts.Add(A);};
    CreateBunkerV4();
    const FVector C(-1700,1700,-2000);
    const FVector Entry=BunkerDoorPosition();
    Box(Entry-FVector(0,0,9),FVector(1000,1000,20),TEXT("Concrete"));
    for(int S:{-1,1})Box(Entry+FVector(S*260,-110,165),FVector(80,440,330),TEXT("Concrete"));
    Box(Entry+FVector(0,-300,165),FVector(560,70,330),TEXT("Concrete"));
    Box(Entry+FVector(0,-110,330),FVector(640,510,50),TEXT("Rust"));
    BunkerParts.Add(SpawnObject(ELWObjectKind::BunkerEntrance,TEXT("bunker_door"),Entry,FRotator(0,90,0)));
    BunkerParts.Add(SpawnObject(ELWObjectKind::BunkerExit,TEXT("bunker_exit"),C+FVector(-1907,0,22)));

}
void ALWWorld::EnsureSiteContainer(FName Id,const LWGen::FSite& Site,FVector Position,int LockOverride)
{
 if(Containers.Contains(Id))return;
 uint32 H=FCrc::StrCrc32(*Id.ToString());FRandomStream Random(H);
 FLWContainerRecord R;R.Id=Id;R.Context=LWPlaces::Loot(Site.Type);R.Position=Position;
 R.LockTier=LockOverride>=0?FMath::Clamp(LockOverride,0,4):!Site.Friendly&&H%100<20?1+int(H%4):0;R.Unlocked=R.LockTier==0;
 auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
 float Quality=FMath::Clamp(float(Site.Position.Size()/1000000.)+(P?P->Stat(TEXT("loot")):0)+R.LockTier*.24f,0.f,1.f);
 // Empty shelves are valid. Contents are seeded once and persisted, never rerolled on streaming.
 if(R.LockTier||Random.FRand()>.32f)for(auto I:LWLoot::Roll(R.Context,int32(H),Quality))AddRecordItem(R,I);
 if(R.LockTier){const FName Bonus[]={TEXT("ammo_12g"),TEXT("medkit"),TEXT("rifle"),TEXT("lmg")};auto I=LWItems::Make(Bonus[R.LockTier-1],R.LockTier==1?12:R.LockTier==2?2:1);I.Id=FGuid::NewDeterministicGuid(Id.ToString()+TEXT("lock_bonus"));AddRecordItem(R,I);}
 Containers.Add(Id,R);
}
void ALWWorld::SpawnSiteLoot(ALWChunk* Chunk,const LWGen::FSite& Site)
{
 // Migrate previously generated loose crates into real furniture without losing saved contents.
 FName Flag(*FString::Printf(TEXT("site_%u_v8_migrated"),Site.Id));if(PropStates.Contains(Flag))return;
 FString Prefix=FString::Printf(TEXT("site_%u_"),Site.Id);FName Target;
 for(AActor* A:Chunk->Residents)if(auto* O=Cast<ALWWorldObject>(A))if(O->RecordId.ToString().StartsWith(Prefix+TEXT("v8_loot_"))){Target=O->RecordId;break;}
 if(Target.IsNone())return;
 TArray<FName> Old;for(const auto& Pair:Containers)if(Pair.Key.ToString().StartsWith(Prefix)&&!Pair.Key.ToString().Contains(TEXT("v8_loot_")))Old.Add(Pair.Key);
 for(FName Id:Old){auto Items=Containers.FindChecked(Id).Items;for(auto Item:Items)AddRecordItem(Containers.FindChecked(Target),Item);Containers.FindChecked(Id).Items.Empty();}
 PropStates.Add(Flag,1);
}
void ALWWorld::SpawnRecords(ALWChunk* Chunk)
{
    TArray<FName> Parked;for(const auto& Pair:Vehicles)if(!Pair.Value.Stored45&&LWGen::ChunkAt(FVector2D(Pair.Value.Position))==Chunk->Coordinate)Parked.Add(Pair.Key);
    for(FName Id:Parked){const auto V=Vehicles.FindChecked(Id);if(auto* Car=SpawnObject(ELWObjectKind::Car,Id,V.Position,V.Rotation))Chunk->Residents.Add(Car);}
    for(const auto& Pair:Containers)if(Pair.Value.bDropped&&LWGen::ChunkAt(FVector2D(Pair.Value.Position))==Chunk->Coordinate)
        if(auto* O=SpawnObject(ELWObjectKind::Container,Pair.Key,Pair.Value.Position))Chunk->Residents.Add(O);
    if(LWGeography84::Canada(FVector2D(Chunk->GetActorLocation())))return;
    const FVector2D P=FVector2D(Chunk->GetActorLocation())+FVector2D(LWGen::ChunkSize*.5);
    const FIntPoint Region(LWGen::FloorDiv(P.X+LWGen::RegionSize*.5,LWGen::RegionSize),LWGen::FloorDiv(P.Y+LWGen::RegionSize*.5,LWGen::RegionSize));
    for(int DY=-1;DY<=1;DY++)for(int DX=-1;DX<=1;DX++){
        const FIntPoint R=Region+FIntPoint(DX,DY);SpawnSettlement(Chunk,R);const uint32 H=LWGen::Hash(R.X,R.Y,Seed,99012);
        if(R!=FIntPoint::ZeroValue&&H%100>=18)continue;
        const FVector2D At=LWGen::Hub(R,Seed)+FVector2D(2800,2800);
        if(LWGen::ChunkAt(At)!=Chunk->Coordinate)continue;
        const FName Id(*FString::Printf(TEXT("trader_%d_%d"),R.X,R.Y));
        if(!Containers.Contains(Id)){
            FLWContainerRecord Record;Record.Id=Id;Record.Context=TEXT("trader");Record.bTrader=true;Record.Position=FVector(At,18);
            for(auto Item:LWLoot::Roll(TEXT("trader"),int32(H),.4f))AddRecordItem(Record,Item);
            Containers.Add(Id,Record);
        }
        if(auto* O=SpawnObject(ELWObjectKind::Trader,Id,FVector(At,18)))Chunk->Residents.Add(O);
    }
}
FName ALWWorld::DropGear(FVector P,const TArray<FLWItemInstance>& Items)
{
    const FName Id(*(FString(TEXT("drop_"))+FGuid::NewGuid().ToString(EGuidFormats::Digits)));
    FLWContainerRecord Record;Record.Id=Id;Record.Context=TEXT("death");Record.bDropped=true;Record.Width=12;Record.Height=24;
    Record.Position=FVector(P.X,P.Y,HeightAt(FVector2D(P))+25);
    // Death is never allowed to discard gear just because a custom footprint will not repack.
    for(auto Item:Items)AddRecordItem(Record,Item);
    Containers.Add(Id,Record);
    if(auto* O=SpawnObject(ELWObjectKind::Container,Id,Record.Position)){
        if(ALWChunk* C=Chunks.FindRef(LWGen::ChunkAt(FVector2D(P))))C->Residents.Add(O);
    }
    return Id;
}
