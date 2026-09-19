#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LWGeneration.h"
#include "LWWorldObject.h"
#include "LWVehicleState.h"
#include "LWEncounter.h"
#include "LWRoads33.h"
#include "LWWorld.generated.h"

class UInstancedStaticMeshComponent;
class UProceduralMeshComponent;
class UMaterialInterface;
class UStaticMesh;
class ALWZombie;
class USoundAttenuation;
class USoundBase;
class UAudioComponent;
class UReverbEffect;

struct FLWSurfaceBatch26 {TArray<FVector> Vertices,Normals;TArray<FVector2D> UV;TArray<int32> Triangles;};

UCLASS()
class LETHALWORLD_API ALWChunk : public AActor
{
    GENERATED_BODY()
public:
    ALWChunk();
    virtual void Tick(float Dt) override;
    UPROPERTY() TObjectPtr<class ALWWorld> LightingWorld;
    UPROPERTY() TArray<TObjectPtr<class UPointLightComponent>> StreetLights;
    TArray<LWRoads33::FJunction> RoadJunctions;
    struct FSignalLamp {int Junction=0;FVector2D Approach;TWeakObjectPtr<class UStaticMeshComponent> Lens[3];};
    TArray<FSignalLamp> SignalLamps;
    void BuildRoads33(class ALWWorld* W,const TArray<LWGen::FRoad>& Roads,const TArray<LWGen::FSite>& Sites);
    void TickTraffic33();
    FIntPoint Coordinate;
    UPROPERTY() TObjectPtr<UProceduralMeshComponent> Terrain;
    UPROPERTY() TMap<FName,TObjectPtr<UInstancedStaticMeshComponent>> Batches;
    UPROPERTY() TArray<TObjectPtr<AActor>> Residents;
    void Add(class ALWWorld* World,FName Mesh,FName Material,FVector Local,FVector Scale=FVector::OneVector,FRotator Rotation=FRotator::ZeroRotator,bool Collision=true);
    void Box(class ALWWorld* World,FName Material,FVector Local,FVector Size,FRotator Rotation=FRotator::ZeroRotator,bool Collision=true);
    void Generate(class ALWWorld* World);
    FBox2D MajorBounds=FBox2D(ForceInit);
    bool BuildingSurfaces=false;
    TMap<int32,TArray<TArray<FVector2D>>> SlabFootprints;
    TMap<FName,FLWSurfaceBatch26> SurfaceData;
    UPROPERTY() TMap<FName,TObjectPtr<UProceduralMeshComponent>> SurfaceMeshes;
    void AddSlab(ALWWorld* W,FName Mat,FVector P,FVector Size,FRotator R,bool Collision);
    void FlushSurfaces();
    void BuildingUnderground(class ALWWorld* W,const LWGen::FSite& S);
    void BuildingAirport(class ALWWorld* World,const LWGen::FSite& Site);
    void BuildingLandmark(class ALWWorld* World,const LWGen::FSite& Site);
    void BuildingDungeon(class ALWWorld* World,const LWGen::FSite& Site);
    void BuildingCasino(class ALWWorld* World,const LWGen::FSite& Site);
    void BuildingTower(class ALWWorld* World,const LWGen::FSite& Site);
    void StreetLight(class ALWWorld* World,FVector Local,FRotator Rotation,bool Neon=false);
    void BuildingV18(class ALWWorld* World,const LWGen::FSite& Site);
    void Building(class ALWWorld* World,const LWGen::FSite& Site);
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
};

UCLASS(Config=Game)
class LETHALWORLD_API ALWWorld : public AActor
{
    GENERATED_BODY()
public:
    ALWWorld();
    struct FPendingCard37 {TWeakObjectPtr<ALWChunk> Chunk;LWGen::FSite Site;};
    TArray<FPendingCard37> PendingCards37;
    UPROPERTY() FLWEncounterState Encounters;
    UPROPERTY() TMap<FName,TObjectPtr<ALWEncounterScene>> LiveEncounters;
    UPROPERTY(Config,EditAnywhere,Category="Encounters") bool EnableEncounters=true;
    float EncounterClock=0;
    void TickEncounters(float Dt,class ALWCharacter* P);
    void SnapshotEncounters();void ClearEncounterActors();
    ALWEncounterScene* SpawnEncounter(FName Type,FVector Position,float Yaw=0);
    bool EncounterLocation(FVector Position,float Radius=750)const;

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    UPROPERTY(Config,EditAnywhere,Category="Generation") int32 Seed=198706;
    UPROPERTY(Config,EditAnywhere,Category="Generation",meta=(ClampMin=1,ClampMax=4)) int32 RenderRadius=2;
    UPROPERTY() TMap<FIntPoint,TObjectPtr<ALWChunk>> Chunks;
    UPROPERTY() TMap<FName,TObjectPtr<UStaticMesh>> Meshes;
    UPROPERTY() TMap<FName,TObjectPtr<UMaterialInterface>> Materials;
    UPROPERTY() TMap<FName,TObjectPtr<USoundBase>> Sounds;
    UPROPERTY() TObjectPtr<USoundAttenuation> Attenuation;
    UPROPERTY() TObjectPtr<USoundAttenuation> LoudAttenuation;
    UPROPERTY() TObjectPtr<UAudioComponent> Wind;
    UPROPERTY() TObjectPtr<UAudioComponent> Drone;
    UPROPERTY() TObjectPtr<UReverbEffect> IndoorReverb;
    UPROPERTY() TObjectPtr<class AStaticMeshActor> Sky;
    UPROPERTY() TMap<FName,FLWContainerRecord> Containers;
    UPROPERTY() TMap<FName,int32> PropStates;
    UPROPERTY() TMap<FName,FLWVehicleRecord> Vehicles;
    UPROPERTY() TArray<TObjectPtr<AActor>> BunkerParts;
    UPROPERTY() TObjectPtr<class ULWAudioCatalog> AudioCatalog;
    UPROPERTY() TObjectPtr<UAudioComponent> Music;
    FName MusicState;
    float MusicRetryClock=0;
    TSharedPtr<struct FStreamableHandle> AudioWarm40;void WarmAudio40();float Maintenance40=0;
    TArray<TWeakObjectPtr<AActor>> BloodEffects40;
    float CombatMusicHold=0;float MusicScan40=0;bool MusicThreat40=false;
    void RestoreBunkerContainers();void CreateBunker();void CreateBunkerV4();void SpawnSettlement(ALWChunk* Chunk,FIntPoint Region);
    static FVector BedroomPosition(int32 I){int F=FMath::Clamp(I/10,0,9);int R=I%10;return FVector(-1700+(-1200+(R%5)*600),1700+(R<5?-1:1)*600,-1870-F*460);}
    void SpawnRecords(ALWChunk* Chunk);
    void SpawnSiteLoot(ALWChunk* Chunk,const LWGen::FSite& Site);
    void EnsureSiteContainer(FName Id,const LWGen::FSite& Site,FVector Position,int LockOverride=-1);
    ALWWorldObject* SpawnObject(ELWObjectKind Kind,FName Id,FVector Position,FRotator Rotation=FRotator::ZeroRotator);
    FName DropGear(FVector Position,const TArray<FLWItemInstance>& Items);
    static FVector BunkerDoorPosition(){return FVector(-1700,1700,12);}
    static FVector BunkerSpawn(){return FVector(-1700,1550,-1870);}
    static bool IsSafePosition(FVector P){return (FMath::Abs(P.X+1700)<1900&&FMath::Abs(P.Y-1700)<1550&&P.Z>-6800&&P.Z<-1520)||(FMath::Abs(P.X+1700)<2500&&FMath::Abs(P.Y-1700)<2450&&P.Z>-7180&&P.Z<-6600);}
    TSet<uint32> KilledZombies;
    TArray<FIntPoint> Queue;
    FIntPoint LastCenter=FIntPoint(MAX_int32,MAX_int32);
    UPROPERTY() TObjectPtr<class ULWSpawnTable> SpawnTable;
    void SpawnRoamingBoss(class ALWChunk* C);
    void SpawnEnemies(ALWChunk* C,const LWGen::FSite* Site,FVector2D Footprint=FVector2D::ZeroVector,int Floors=1,float FloorHeight=360);
    int32 ZombieCount=0;
    float UpdateClock=0;
    float SurfaceWetness=0;
    UPROPERTY() TArray<TObjectPtr<class UMaterialInstanceDynamic>> WetMaterials;
    void InitWetMaterials();void UpdateWetness(float Dt);
    int32 TownSetting=1,POISetting=1,TerrainSetting=1,Difficulty=1;
    void ApplyGenerationSettings();
    float TimeOfDay=6.f;
    int32 DayNumber=1;
    UPROPERTY(Config,EditAnywhere,Category="Weather") float DayLengthMinutes=96;
    UPROPERTY() TObjectPtr<class ADirectionalLight> SunLight;
    UPROPERTY() TObjectPtr<class ADirectionalLight> MoonLight;
 UPROPERTY() TObjectPtr<class UStaticMeshComponent> MoonDisc;
 UPROPERTY() TObjectPtr<class UStaticMeshComponent> SunDisc;
    UPROPERTY() TObjectPtr<class ASkyLight> AmbientLight;
    UPROPERTY() TObjectPtr<class AExponentialHeightFog> WeatherFog;
    UPROPERTY() TObjectPtr<class UMaterialInstanceDynamic> WeatherSky;
    UPROPERTY() TObjectPtr<UInstancedStaticMeshComponent> Rain;
    UPROPERTY() TObjectPtr<UAudioComponent> RainAudio;
    float RainAmount=0,CloudAmount=0,WeatherClock=0;
    int32 WeatherType=0;
    UPROPERTY() TObjectPtr<class UInstancedStaticMeshComponent> StormParticles;
    UPROPERTY() TObjectPtr<class UInstancedStaticMeshComponent> Tornado;
    FVector StormOrigin=FVector::ZeroVector;bool TornadoActive=false;
    float VisibilityRange=6000,ExposureClock=0; int32 WeatherOverride=-1;
    void TickSevereWeather(float Dt,class ALWCharacter* P);
    int64 LastThunder=-1;
    void TickWeather(float Dt,class ALWCharacter* Player);
    bool bWasIndoors=false;
    FString LocationName=TEXT("OUTER EXCLUSION ZONE");
    UStaticMesh* Mesh(FName Name);
    UMaterialInterface* Material(FName Name);
    UAudioComponent* Sound(FName Name,FVector Position,float Volume=1,float Pitch=1,bool Loud=false);
    void Noise(FVector Position,float Radius);
    void Stream(FVector Position,bool Immediate=false);
    void Reset();
    float HeightAt(FVector2D Position) const;
    static ALWWorld* Get(const UObject* Context);
};

