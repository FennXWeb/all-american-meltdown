#include "LWBorder51.h"
#include "LWTradingCards36.h"
#include "LWSiteIdentity.h"
#include "LWVehicle.h"
#include "LWResident.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWZombie.h"
#include "LWInteractable.h"
#include "LWAudioCatalog.h"
#include "LWThreatAwareness.h"
#include "ProceduralMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/AudioComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "Engine/TextureCube.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundWave.h"
#include "Sound/ReverbEffect.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/WorldSettings.h"

ALWChunk::ALWChunk()
{
    PrimaryActorTick.bCanEverTick=true;PrimaryActorTick.TickInterval=.5f;
    Terrain=CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Terrain")); SetRootComponent(Terrain);
    Terrain->bUseAsyncCooking=false;
    Terrain->SetCollisionProfileName(TEXT("BlockAll"));
}
void ALWChunk::Add(ALWWorld* W,FName MeshName,FName Mat,FVector P,FVector S,FRotator R,bool Collision)
{
    const FName Key(*(MeshName.ToString()+TEXT("_")+Mat.ToString()+(Collision?TEXT("_C"):TEXT("_N"))));
    UInstancedStaticMeshComponent* Batch=Batches.FindRef(Key);
    if(!Batch)
    {
        Batch=NewObject<UInstancedStaticMeshComponent>(this); Batch->SetupAttachment(RootComponent);
        Batch->SetStaticMesh(W->Mesh(MeshName));
        if(Mat==TEXT("Asphalt"))Batch->ComponentTags.Add(TEXT("RoadSurface"));
        if(!Mat.IsNone()) Batch->SetMaterial(0,W->Material(Mat));
        Batch->SetCollisionProfileName(Collision?TEXT("BlockAll"):TEXT("NoCollision"));
        Batch->SetCanEverAffectNavigation(false); Batch->RegisterComponent(); Batches.Add(Key,Batch);
    }
    Batch->AddInstance(FTransform(R,P,S));
}
void ALWChunk::Box(ALWWorld* W,FName Mat,FVector P,FVector Size,FRotator R,bool Collision)
{ if(Size.Z<=30&&Size.X>=150&&Size.Y>=150&&FMath::Abs(R.Pitch)<.01&&FMath::Abs(R.Roll)<.01)AddSlab(W,Mat,P,Size,R,Collision);else Add(W,TEXT("Cube"),Mat,P,Size/100.,R,Collision); }

void ALWChunk::Generate(ALWWorld* W)
{
    BuildingSurfaces=true;
    const FVector Origin=GetActorLocation();
    TArray<LWGen::FRoad> Roads; TArray<LWGen::FSite> Sites;
    LWGen::Gather(FVector2D(Origin)+FVector2D(LWGen::ChunkSize*.5),W->Seed,Roads,Sites);
    TArray<FVector> V,Normals; TArray<FVector2D> UV; TArray<int32> Tri;
    constexpr int32 Grid=24;
    for(int32 Y=0;Y<=Grid;Y++) for(int32 X=0;X<=Grid;X++)
    {
        const FVector2D Local(X*LWGen::ChunkSize/Grid,Y*LWGen::ChunkSize/Grid), P=FVector2D(Origin)+Local;
        const float H=LWGen::Height(P,Roads,Sites);
        V.Add(FVector(Local,H)); UV.Add(P/450.);
        const float DX=LWGen::Height(P+FVector2D(40,0),Roads,Sites)-LWGen::Height(P-FVector2D(40,0),Roads,Sites);
        const float DY=LWGen::Height(P+FVector2D(0,40),Roads,Sites)-LWGen::Height(P-FVector2D(0,40),Roads,Sites);
        Normals.Add(FVector(-DX,-DY,80).GetSafeNormal());
        if(X<Grid && Y<Grid) { const int32 I=Y*(Grid+1)+X; Tri.Append({I,I+Grid+1,I+1,I+1,I+Grid+1,I+Grid+2}); }
    }
    Terrain->CreateMeshSection(0,V,Tri,Normals,UV,TArray<FColor>(),TArray<FProcMeshTangent>(),true);
    Terrain->SetMaterial(0,W->Material(TEXT("Earth")));
    if(Origin.X>=LWBorder51::Strip){BuildRoads33(W,Roads,TArray<LWGen::FSite>());LWBorder51::Build(this,W);FlushSurfaces();return;}
    BuildRoads33(W,Roads,Sites);
    for(const LWGen::FSite& Site:Sites) if(!Site.SettlementBuilding&&LWGen::ChunkAt(Site.Position)==Coordinate) Building(W,Site);
    for(const auto& S:Sites)if(LWGen::ChunkAt(S.Position)==Coordinate){
        FRotator Facing(0,S.Yaw,0);FVector Front=FVector(S.Position,0)-Origin+Facing.RotateVector(FVector(0,-S.Size.Y*.5-150,0));
        if(!LWPlaces::IsTower(S.Type)){StreetLight(W,Front+Facing.RotateVector(FVector(S.Size.X*.5-100,0,0)),Facing);if(S.District==2||S.Type==5||S.Type==19)StreetLight(W,Front+FVector(0,0,310),Facing,true);}
        // Sidewalk furniture is owned by the parcel and stays outside its door approach.
        for(int Side:{-1,1}){FVector At=Front+Facing.RotateVector(FVector(Side*(S.Size.X*.5-280),0,0));Add(W,TEXT("WasteBinV13"),NAME_None,At,FVector(1),Facing);}
    }
    W->SpawnRecords(this);
    FRandomStream Rand(LWGen::Hash(Coordinate.X,Coordinate.Y,W->Seed,31));
    for(int32 I=0;I<42;I++)
    {
        const FVector2D Local(Rand.FRandRange(0,LWGen::ChunkSize),Rand.FRandRange(0,LWGen::ChunkSize));if(LWStory::Reserved(FVector2D(Origin)+Local))continue;
        const FVector2D P=FVector2D(Origin)+Local;
        bool Clear=true;
        for(const auto& R:Roads) if(LWGen::DistanceToSegment(P,R)<R.Width*.5+450) {Clear=false;break;}
        for(const auto& S:Sites) if((P-S.Position).Size()<S.Size.Size()*.5+250) {Clear=false;break;}
        if(!Clear) continue;
        const float Z=LWGen::Height(P,Roads,Sites);
        Add(W,I%4?TEXT("DeadTree"):TEXT("Rubble"),NAME_None,FVector(Local,Z),FVector(Rand.FRandRange(.55,1.6)),FRotator(0,Rand.FRandRange(0,360),0),I%4!=0);
    }
 BuildingSurfaces=false;FlushSurfaces();
 W->SpawnEnemies(this,nullptr);W->SpawnRoamingBoss(this);
}

void ALWChunk::EndPlay(const EEndPlayReason::Type Reason)
{ for(AActor* A:Residents) if(IsValid(A)) A->Destroy(); Super::EndPlay(Reason); }

ALWWorld::ALWWorld() { PrimaryActorTick.bCanEverTick=true;PrimaryActorTick.bTickEvenWhenPaused=true; }
void ALWWorld::EndPlay(const EEndPlayReason::Type Reason)
{
    if(RainAudio)RainAudio->Stop();if(Wind)Wind->Stop();if(Drone)Drone->Stop();if(Music)Music->Stop();
    UGameplayStatics::DeactivateReverbEffect(this,TEXT("Interior"));Super::EndPlay(Reason);
}
ALWWorld* ALWWorld::Get(const UObject* Context)
{ return Cast<ALWWorld>(UGameplayStatics::GetActorOfClass(Context,StaticClass())); }
UStaticMesh* ALWWorld::Mesh(FName N)
{
    if(UStaticMesh* M=Meshes.FindRef(N)) return M;
    FString Path=(N==TEXT("Sphere")||N==TEXT("Cylinder"))?FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"),*N.ToString(),*N.ToString()):N==TEXT("Cube")?TEXT("/Game/Art/Meshes/SM_UnitCube.SM_UnitCube"):FString::Printf(TEXT("/Game/Art/Meshes/SM_%s.SM_%s"),*N.ToString(),*N.ToString());
    UStaticMesh* M=LoadObject<UStaticMesh>(nullptr,*Path); Meshes.Add(N,M); return M;
}
UMaterialInterface* ALWWorld::Material(FName N)
{
    if(UMaterialInterface* M=Materials.FindRef(N)) return M;
    UMaterialInterface* M=LoadObject<UMaterialInterface>(nullptr,*FString::Printf(TEXT("/Game/Materials/M_%s.M_%s"),*N.ToString(),*N.ToString()));
    Materials.Add(N,M); return M;
}
void ALWWorld::BeginPlay()
{
    Super::BeginPlay(); GetWorld()->GetWorldSettings()->bEnableWorldBoundsChecks=false;
    Attenuation=NewObject<USoundAttenuation>(this);
    auto& A=Attenuation->Attenuation; A.bAttenuate=true; A.bSpatialize=true; A.AttenuationShapeExtents=FVector(140);
    A.FalloffDistance=6500; A.bEnableOcclusion=true; A.OcclusionTraceChannel=ECC_Visibility;
    A.OcclusionLowPassFilterFrequency=1100; A.OcclusionVolumeAttenuation=.24f; A.OcclusionInterpolationTime=.22f;
    A.bAttenuateWithLPF=true; A.LPFRadiusMin=600; A.LPFRadiusMax=6000; A.LPFFrequencyAtMin=18000; A.LPFFrequencyAtMax=2600;
    A.bEnableReverbSend=true; A.ReverbWetLevelMin=.2f; A.ReverbWetLevelMax=.55f; A.ReverbDistanceMax=5000;
    LoudAttenuation=DuplicateObject<USoundAttenuation>(Attenuation,this); LoudAttenuation->Attenuation.FalloffDistance=18000;
    LoudAttenuation->Attenuation.LPFRadiusMax=18000;
    LoudAttenuation->Attenuation.OcclusionVolumeAttenuation=.45f;
    IndoorReverb=NewObject<UReverbEffect>(this); IndoorReverb->DecayTime=1.8f; IndoorReverb->Density=.5f;
    IndoorReverb->Diffusion=.8f; IndoorReverb->Gain=.45f; IndoorReverb->GainHF=.42f; IndoorReverb->LateGain=1.5f;
    AudioCatalog=ULWAudioCatalog::GetDefaultCatalog();WarmAudio40();
    for(FName N:ULWAudioCatalog::GetDefaultSlotNames()){FLWResolvedAudioSlot S;if(ULWAudioCatalog::ResolveDefaultSlot(N,S))Sounds.Add(N,S.Sound);}
    Wind=Sound(TEXT("Wind"),FVector::ZeroVector,.7f);
    Drone=Sound(TEXT("Drone"),FVector::ZeroVector,.22f);
    if(Wind) Wind->SetLowPassFilterEnabled(true);
    ADirectionalLight* Sun=GetWorld()->SpawnActor<ADirectionalLight>(FVector(0,0,8000),FRotator(-26,-35,0));
    SunLight=Sun;Sun->GetLightComponent()->SetLightingChannels(true,true,false);Sun->GetLightComponent()->SetMobility(EComponentMobility::Movable);
    Sun->GetLightComponent()->SetIntensity(3.2f); Sun->GetLightComponent()->SetLightColor(FLinearColor(.82f,.83f,.64f));
    Cast<UDirectionalLightComponent>(Sun->GetLightComponent())->DynamicShadowDistanceMovableLight=16000;
    Cast<UDirectionalLightComponent>(Sun->GetLightComponent())->SetForwardShadingPriority(1);
    ADirectionalLight* Fill=GetWorld()->SpawnActor<ADirectionalLight>(FVector(0,0,1000),FRotator(-55,145,0));
    MoonLight=Fill;Fill->GetLightComponent()->SetLightingChannels(true,true,false);Fill->GetLightComponent()->SetMobility(EComponentMobility::Movable);
    Fill->GetLightComponent()->SetIntensity(.7f); Fill->GetLightComponent()->SetLightColor(FLinearColor(.39f,.52f,.48f));
    Fill->GetLightComponent()->SetCastShadows(false);
    ASkyLight* Ambient=GetWorld()->SpawnActor<ASkyLight>();
    AmbientLight=Ambient;
    Ambient->GetLightComponent()->SetMobility(EComponentMobility::Movable);
    Ambient->GetLightComponent()->SourceType=SLS_SpecifiedCubemap;
    Ambient->GetLightComponent()->SetCubemap(LoadObject<UTextureCube>(nullptr,TEXT("/Game/Art/Textures/T_AmbientSky.T_AmbientSky")));
    Ambient->GetLightComponent()->SetIntensity(.75f);
    Ambient->GetLightComponent()->SetLightColor(FLinearColor(.68f,.76f,.65f));
    Ambient->GetLightComponent()->RecaptureSky();
    AExponentialHeightFog* Fog=GetWorld()->SpawnActor<AExponentialHeightFog>();
    WeatherFog=Fog;
    Fog->GetComponent()->SetFogDensity(.012f); Fog->GetComponent()->SetFogHeightFalloff(.11f);
    Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(.19f,.235f,.20f));
    Fog->GetComponent()->SetStartDistance(1400); Fog->GetComponent()->SetFogMaxOpacity(.97f);
    Sky=GetWorld()->SpawnActor<AStaticMeshActor>();
    // Actor-level exclusion survives the engine mesh's asynchronous render/physics setup.
    Sky->SetActorEnableCollision(false);
    Sky->SetMobility(EComponentMobility::Movable);
    Sky->GetStaticMeshComponent()->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Game/Art/Meshes/SM_SkySphere.SM_SkySphere")));
    Sky->GetStaticMeshComponent()->SetMaterial(0,Material(TEXT("Sky")));
    Sky->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("NoCollision"));
    Sky->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Sky->GetStaticMeshComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
    Sky->GetStaticMeshComponent()->SetCastShadow(false);Sky->SetActorScale3D(FVector(3000));
    InitWetMaterials();ApplyGenerationSettings(); // Gameplay geometry is created when a survivor is started or restored.
}
UAudioComponent* ALWWorld::Sound(FName N,FVector P,float Volume,float Pitch,bool Loud)
{
    return ULWAudioCatalog::PlaySlot(this,N,P,Volume,Pitch,Loud,Attenuation,LoudAttenuation);
}
void ALWWorld::Noise(FVector P,float Radius)
{
    for(TActorIterator<ALWZombie> It(GetWorld());It;++It)
        if(!Cast<ALWResident>(*It)&&FVector::DistSquared(P,It->GetActorLocation())<FMath::Square(Radius)) It->Hear(P,Radius);
}
float ALWWorld::HeightAt(FVector2D P) const
{ const auto& N=LWGen::Neighborhood38(P,Seed);return LWGen::Height(P,N.Roads,N.Sites); }
void ALWWorld::Stream(FVector P,bool Immediate)
{
    const FIntPoint C=LWGen::ChunkAt(FVector2D(P));
    if(C!=LastCenter || Immediate)
    {
        LastCenter=C; Queue.Empty();
        TArray<FIntPoint> Remove;
        for(const auto& Entry:Chunks)
            if((FMath::Abs(Entry.Key.X-C.X)>RenderRadius+1 || FMath::Abs(Entry.Key.Y-C.Y)>RenderRadius+1)&&!Entry.Value->MajorBounds.IsInside(FVector2D(P))) Remove.Add(Entry.Key);
        for(FIntPoint K:Remove) { Chunks[K]->Destroy(); Chunks.Remove(K); }
        for(int32 Y=-RenderRadius;Y<=RenderRadius;Y++) for(int32 X=-RenderRadius;X<=RenderRadius;X++)
        { const FIntPoint K=C+FIntPoint(X,Y); if(!Chunks.Contains(K)) Queue.Add(K); }
        // Large airports can extend beyond the normal minimum streaming radius.
        TArray<LWGen::FRoad> NearbyRoads;TArray<LWGen::FSite> NearbySites;LWGen::Gather(FVector2D(P),Seed,NearbyRoads,NearbySites);
        for(const auto& Site:NearbySites)if(Site.Type==32){const FVector2D Delta=FVector2D(P)-Site.Position;if(FMath::Abs(Delta.X)<Site.Size.X*.5+4000&&FMath::Abs(Delta.Y)<Site.Size.Y*.5+4000){const auto SiteChunk=LWGen::ChunkAt(Site.Position);if(!Chunks.Contains(SiteChunk))Queue.AddUnique(SiteChunk);}}
        Queue.Sort([C](const FIntPoint& A,const FIntPoint& B){return (A-C).SizeSquared()<(B-C).SizeSquared();});
    }
    const int32 Count=Immediate?Queue.Num():FMath::Min(1,Queue.Num());
    for(int32 I=0;I<Count;I++)
    {
        const FIntPoint K=Queue[0]; Queue.RemoveAt(0);
        ALWChunk* Chunk=GetWorld()->SpawnActor<ALWChunk>(FVector(K.X*LWGen::ChunkSize,K.Y*LWGen::ChunkSize,0),FRotator::ZeroRotator);
        Chunk->Coordinate=K; Chunks.Add(K,Chunk); Chunk->Generate(this);
    }
}
void ALWWorld::Reset()
{
    PendingCards37.Empty();
    ClearEncounterActors();
    for(TActorIterator<ALWVehicle> I(GetWorld());I;++I)I->Destroy();
    for(TActorIterator<ALWResident> I(GetWorld());I;++I)I->Destroy();
    for(auto& Pair:Chunks) if(IsValid(Pair.Value)) Pair.Value->Destroy();
    Chunks.Empty(); Queue.Empty(); LastCenter=FIntPoint(MAX_int32,MAX_int32); ZombieCount=0;for(AActor* A:BunkerParts)if(auto* O=Cast<ALWWorldObject>(A))if(O->Kind==ELWObjectKind::Door)O->ConfigureProp();
}
void ALWWorld::Tick(float Dt)
{
    Super::Tick(Dt);
    ALWCharacter* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0)); if(!P) return;

    Maintenance40-=Dt;if(Maintenance40<=0){Maintenance40=.5f;
    for(AActor* A:BunkerParts)if(auto* O=Cast<ALWWorldObject>(A))if(O->RecordId.ToString().StartsWith(TEXT("bunker_storage_"))&&!Containers.Contains(O->RecordId)){FLWContainerRecord R;R.Id=O->RecordId;R.Context=TEXT("bunker");R.Position=O->GetActorLocation();Containers.Add(R.Id,R);}
    // A moved car may outlive its generating chunk. Recreate missing residents by current location.
    TArray<FName> Missing;TSet<FName> LiveVehicles;for(TActorIterator<ALWVehicle> I(GetWorld());I;++I)LiveVehicles.Add(I->RecordId);
    for(const auto& V:Vehicles)if(!V.Value.Stored45&&Chunks.Contains(LWGen::ChunkAt(FVector2D(V.Value.Position)))){if(!LiveVehicles.Contains(V.Key)){Missing.Add(V.Key);break;}}
    for(FName Id:Missing){const auto V=Vehicles.FindChecked(Id);if(auto* Car=SpawnObject(ELWObjectKind::Car,Id,V.Position,V.Rotation))if(auto* C=Chunks.FindRef(LWGen::ChunkAt(FVector2D(V.Position))).Get())C->Residents.Add(Car);}
    }
    if(Sky)Sky->SetActorLocation(P->GetActorLocation());
    if(P->bStarted){LWBorder51::Enforce(this,P);Stream(P->GetActorLocation());}
    if(Queue.IsEmpty()&&!PendingCards37.IsEmpty()){const auto Job=PendingCards37[0];PendingCards37.RemoveAt(0,EAllowShrinking::No);if(Job.Chunk.IsValid())LWCollect36::Spawn(Job.Chunk.Get(),this,Job.Site);}
    FName Mood=(!P->bStarted||P->bMenu)?TEXT("MenuMusic"):TEXT("ExploreMusic");
    if(P->bStarted&&!P->bMenu&&!P->bSafehouse&&P->Health>0&&!P->bStoryLocked){
        MusicScan40-=Dt;
        if(MusicScan40<=0){MusicScan40=.25f;MusicThreat40=false;
            TArray<ALWZombie*> Nearby;
            for(TActorIterator<ALWZombie> It(GetWorld());It;++It)if(FVector::DistSquared(It->GetActorLocation(),P->GetActorLocation())<FMath::Square(3500.)&&LWThreatAwareness::Engaged(*It,P))Nearby.Add(*It);
            Nearby.Sort([P](const ALWZombie& A,const ALWZombie& B){return FVector::DistSquared(A.GetActorLocation(),P->GetActorLocation())<FVector::DistSquared(B.GetActorLocation(),P->GetActorLocation());});
            // Bound expensive visibility tests. Only engaged threats on this floor qualify.
            for(int i=0;i<FMath::Min(8,Nearby.Num());++i)if(LWThreatAwareness::Visible(P,Nearby[i])){MusicThreat40=true;break;}
        }
        const bool Incoming=IsValid(P->CombatTarget)&&!P->CombatTarget->bDead&&GetWorld()->GetTimeSeconds()-P->LastCombatTime<3;
        CombatMusicHold=(MusicThreat40||Incoming)?7.f:FMath::Max(0.f,CombatMusicHold-Dt);
        if(CombatMusicHold>0)Mood=TEXT("CombatMusic");
    }else{CombatMusicHold=0;MusicScan40=0;MusicThreat40=false;}
    MusicRetryClock=FMath::Max(0.f,MusicRetryClock-Dt);
    if(Mood!=MusicState||((!IsValid(Music.Get())||!Music->IsPlaying())&&MusicRetryClock<=0)){
        if(IsValid(Music.Get()))Music->FadeOut(.8f,0);
        MusicState=Mood;Music=Sound(Mood,P->GetActorLocation(),.18f);if(Music)Music->FadeIn(1.1f,Music->VolumeMultiplier);MusicRetryClock=2;
    }
    if(!P->bStarted || P->bMenu || P->Health<=0) return;
    TickWeather(Dt,P);TickEncounters(Dt,P);
    UpdateClock+=Dt;
    if(UpdateClock<.25f) return; UpdateClock=0;
    const bool Indoor=P->bIndoors;
    if(Indoor!=bWasIndoors)
    {
        bWasIndoors=Indoor;
        if(Indoor) UGameplayStatics::ActivateReverbEffect(this,IndoorReverb,TEXT("Interior"),1,.42f,.5f);
        else UGameplayStatics::DeactivateReverbEffect(this,TEXT("Interior"));
    }
    if(Wind) { const FLWAudioSlot* Slot=AudioCatalog?AudioCatalog->Slots.Find(TEXT("Wind")):nullptr;const float SlotVolume=Slot&&FMath::IsFinite(Slot->Volume)?FMath::Clamp(Slot->Volume,0.f,4.f):1.f;Wind->SetLowPassFilterFrequency(Indoor?850:12000);Wind->SetVolumeMultiplier((Indoor?.10f:.45f)*(1+CloudAmount)*SlotVolume); }
    TArray<LWGen::FRoad> Roads; TArray<LWGen::FSite> Sites;
    LWGen::Gather(FVector2D(P->GetActorLocation()),Seed,Roads,Sites);
    LocationName=TEXT("OUTER EXCLUSION ZONE");
    const TCHAR* Names[]={TEXT("LAST LIGHT // FUEL STOP"),TEXT("THE VACANCY // MOTEL"),TEXT("CIVIL RELIEF STATION"),TEXT("SECTOR 09 // STORAGE"),TEXT("DEAD END // DINER")};
    float Best=4000;
    for(const auto& S:Sites) {float D=(S.Position-FVector2D(P->GetActorLocation())).Size(); if(D<Best) {Best=D; LocationName=LWSites::Label(S);}}
    if(P->GetActorLocation().Z<-7000)for(const auto& Site:Sites)if(LWPlaces::Underground(Site.Type)){const FVector2D Local(FRotator(0,-Site.Yaw,0).RotateVector(P->GetActorLocation()-FVector(Site.Position,0)));const FVector2D Ext=LWDungeons::Size(Site.Type)*.5;if(FMath::Abs(Local.X)<Ext.X&&FMath::Abs(Local.Y)<Ext.Y)LocationName=LWSites::Label(Site);}
    if(P->bSafehouse)LocationName=TEXT("SHELTER 01 // SECURED BUNKER");
    else if(FVector2D::Distance(FVector2D(P->GetActorLocation()),FVector2D(BunkerDoorPosition()))<800)LocationName=TEXT("SHELTER 01 // AIRLOCK APPROACH");
}
