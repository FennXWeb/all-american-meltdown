#include "LWWorldObject.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/PointLightComponent.h"
#include "LWWorldTextComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ALWWorldObject::ALWWorldObject()
{
    PrimaryActorTick.bCanEverTick=true;
    Root=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));SetRootComponent(Root);
    Body=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));Body->SetupAttachment(Root);
    Body->SetCollisionProfileName(TEXT("BlockAll"));Body->SetMobility(EComponentMobility::Movable);
}
void ALWWorldObject::Part(FName Mesh,FVector P,FVector S,FRotator R,FName Material)
{
    auto* C=NewObject<UStaticMeshComponent>(this);C->SetupAttachment(Root);C->SetStaticMesh(World->Mesh(Mesh));
    C->SetRelativeLocation(P);C->SetRelativeScale3D(S);C->SetRelativeRotation(R);
    if(!Material.IsNone())C->SetMaterial(0,World->Material(Material));
    C->SetCollisionProfileName(TEXT("BlockAll"));C->RegisterComponent();Details.Add(C);
}
void ALWWorldObject::Configure(ALWWorld* W,ELWObjectKind InKind,FName Id)
{
    World=W;Kind=InKind;RecordId=Id;
    if(Kind>=ELWObjectKind::Door){ConfigureProp();return;}
    if(Kind==ELWObjectKind::Trader)
    {
        const FName PickStockFlag(*(Id.ToString()+TEXT("_picks")));
        if(auto* Stock=W->Containers.Find(Id))if(!W->PropStates.Contains(PickStockFlag)){
            auto Pick=LWItems::Make(TEXT("lockpick"),12);if(LWItems::Place(Stock->Items,Pick,Stock->Width,Stock->Height))W->PropStates.Add(PickStockFlag,1);
        }
        Body->SetCollisionObjectType(ECC_WorldDynamic);
        Body->SetStaticMesh(W->Mesh(TEXT("TraderBody32")));
        Part(TEXT("TraderHead32"),FVector(-1.7,0,106.1),FVector(1));
        Part(TEXT("TraderWheel32"),FVector(0,-32,26.5),FVector(1));
        Part(TEXT("TraderWheel32"),FVector(0,32,26.5),FVector(1));
        const FVector2D P(GetActorLocation());
        const FIntPoint Region(LWGen::FloorDiv(P.X+LWGen::RegionSize*.5,LWGen::RegionSize),LWGen::FloorDiv(P.Y+LWGen::RegionSize*.5,LWGen::RegionSize));
        const FVector2D H=LWGen::Hub(Region,W->Seed);
        Route={H+FVector2D(-4700,2800),H+FVector2D(4700,2800),H+FVector2D(0,2800),H,H+FVector2D(0,2800)};
        RouteIndex=1;
    }
    else if(Kind==ELWObjectKind::Container)
    {
        if(Id.ToString().StartsWith(TEXT("bunker_storage_"))&&!W->Containers.Contains(Id)){FLWContainerRecord R;R.Id=Id;R.Position=GetActorLocation();R.Context=TEXT("bunker");W->Containers.Add(Id,R);}
        const auto* Record=W->Containers.Find(Id);
        Body->SetStaticMesh(W->Mesh(Record&&Record->bDropped?TEXT("LootBag"):TEXT("Crate")));
        // Exclude pickups from WorldStatic/WorldDynamic vehicle terrain object queries.
        if(Record&&Record->bDropped){Body->SetCollisionObjectType(ECC_PhysicsBody);Body->SetCollisionEnabled(ECollisionEnabled::QueryOnly);Body->SetCollisionResponseToAllChannels(ECR_Ignore);Body->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);Body->SetGenerateOverlapEvents(false);Body->SetCanEverAffectNavigation(false);}
    }
    else if(Kind==ELWObjectKind::Stash)
    {
        Body->SetStaticMesh(W->Mesh(TEXT("LockerV4")));
    }
    else
    {
        Body->SetStaticMesh(W->Mesh(TEXT("BunkerDoorFrame")));
        Part(TEXT("BunkerDoor"),FVector(22,-73.5,14.5),FVector(1));
        Part(TEXT("BunkerDoorWheel"),FVector(38.6,0,123),FVector(1));
        // Solid hatch is an airlock interaction: the protected chamber is below the terrain.
        Part(TEXT("Cube"),FVector(-12,0,125),FVector(.35,3.0,2.7),FRotator::ZeroRotator,TEXT("Concrete"));
    }
    if(Kind==ELWObjectKind::Container)return;
    auto* Label=NewObject<ULWWorldTextComponent>(this);Label->SetupAttachment(Root);
    Label->SetRelativeLocation(FVector(40,0,Kind==ELWObjectKind::Trader?205:155));Label->SetRelativeRotation(FRotator::ZeroRotator);
    Label->SetHorizontalAlignment(EHTA_Center);Label->SetWorldSize(16);Label->SetTextRenderColor(FColor(215,183,103));
    Label->SetText(FText::FromString(Kind==ELWObjectKind::Trader?TEXT("CREDIT EXCHANGE"):Kind==ELWObjectKind::Stash?TEXT("SECURED STORAGE"):TEXT("SHELTER 01 // AIRLOCK")));Label->RegisterComponent();
    auto* Light=NewObject<UPointLightComponent>(this);Light->SetupAttachment(Root);Light->SetRelativeLocation(FVector(80,0,160));
    Light->SetIntensity(Kind==ELWObjectKind::Container?180:900);Light->SetAttenuationRadius(450);Light->SetLightColor(FLinearColor(.55,.8,.6));Light->SetCastShadows(false);Light->RegisterComponent();
}
FString ALWWorldObject::Prompt()const
{
    if(World)if(const auto* R=World->Containers.Find(RecordId))if(R->LockTier&&!R->Unlocked)return FString::Printf(TEXT("[E] PICK LOCK // TIER %d"),R->LockTier);
    switch(Kind){
    case ELWObjectKind::Furniture:if(UseType==TEXT("base45"))return TEXT("[E] SHELTER CONTROL");if(UseType==TEXT("garage45"))return TEXT("[E] GARAGE CONTROL");if(UseType==TEXT("surface45"))return TEXT("[E] STORE VEHICLE");if(UseType==TEXT("lift45"))return TEXT("[E] SELECT FLOOR");if(UseType.ToString().StartsWith(TEXT("call45_")))return TEXT("[E] CALL ELEVATOR");return UseType==TEXT("bed")?TEXT("[E] BED / REST / RESPAWN"):UseType==TEXT("chair")?TEXT("[E] SIT DOWN"):TEXT("[E] ")+UseType.ToString().ToUpper();
    case ELWObjectKind::Door:return bChanged?TEXT("[E] CLOSE DOOR"):TEXT("[E] OPEN DOOR");
    case ELWObjectKind::Car:return TEXT("[E] SEARCH VEHICLE");
    case ELWObjectKind::Window:return bChanged?TEXT(""):TEXT("BREAKABLE GLASS");
    case ELWObjectKind::FuelPump:return bChanged?TEXT("BURNT FUEL PUMP"):TEXT("[E] REFILL GAS CANS / FLAMETHROWER CANISTERS");
    case ELWObjectKind::MannequinDisplay:case ELWObjectKind::Sign:return TEXT("");
    case ELWObjectKind::Trader:return TEXT("[E] WANDERING EXCHANGE // TRADE");
    case ELWObjectKind::BunkerEntrance:return TEXT("[E] ENTER SAFEHOUSE BUNKER");
    case ELWObjectKind::BunkerExit:return TEXT("[E] LEAVE SAFEHOUSE");
    case ELWObjectKind::Stash:return TEXT("[E] SECURED STASH // GEAR SURVIVES DEATH");
    default:return TEXT("[E] SEARCH CONTAINER");}
}
void ALWWorldObject::Use(ALWCharacter* P)
{
    if(!P||P->Health<=0)return;
    if(P->IsLocked(this)){P->StartLockpick(this);return;}
    if(Kind==ELWObjectKind::FuelPump){P->RefillCanisters(this);return;}
    if(Kind==ELWObjectKind::Furniture){UseFurniture(P);return;}
    if(Kind==ELWObjectKind::BunkerEntrance){P->EnterSafehouse();return;}
    if(Kind==ELWObjectKind::BunkerExit){P->LeaveSafehouse();return;}
    if(Kind==ELWObjectKind::Door){CompanionOpenedDoor=false;ManualDoorUntil=GetWorld()->GetTimeSeconds()+2;bChanged=!bChanged;World->PropStates.Add(RecordId,bChanged?1:0);World->Sound(TEXT("DoorHinge"),GetActorLocation());return;}
    if(Kind==ELWObjectKind::MannequinDisplay||Kind==ELWObjectKind::Window||Kind==ELWObjectKind::FuelPump||Kind==ELWObjectKind::Sign)return;
    P->OpenContainer(this);
    if(Kind==ELWObjectKind::Trader)World->Sound(TEXT("TraderVoice"),GetActorLocation(),.8f);
}
void ALWWorldObject::Tick(float Dt)
{
    Super::Tick(Dt);
    if(Kind==ELWObjectKind::Door){TickCompanionDoor();DoorAngle=FMath::FInterpConstantTo(DoorAngle,bChanged?100.f:0.f,Dt,160.f);Body->SetRelativeRotation(FRotator(0,DoorAngle,0));return;}
    if(EffectTime>0){EffectTime-=Dt;for(UStaticMeshComponent* C:Details){if(Kind==ELWObjectKind::FuelPump)C->AddLocalOffset(FVector(0,0,Dt*90));if(Kind==ELWObjectKind::FuelPump)C->SetWorldScale3D(C->GetComponentScale()+FVector(Dt*(Kind==ELWObjectKind::FuelPump?.6f:.015f)));if(EffectTime<=0){C->SetSimulatePhysics(false);C->SetCollisionEnabled(ECollisionEnabled::NoCollision);C->SetVisibility(false);}}}
    if(Kind!=ELWObjectKind::Trader||!World||Route.IsEmpty())return;
    ALWCharacter* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
    if(!P||!P->bStarted||P->bMenu)return;
    if(P->OpenObject==this){if(Details.Num()){const auto Look=(P->GetActorLocation()-Details[0]->GetComponentLocation()).Rotation()-GetActorRotation();Details[0]->SetRelativeRotation(FMath::RInterpTo(Details[0]->GetRelativeRotation(),FRotator(FMath::Sin(GetWorld()->GetTimeSeconds()*3)*3,FMath::Clamp(Look.Yaw,-50.f,50.f),0),Dt,4));}return;}
    if(FVector::DistSquared(P->GetActorLocation(),GetActorLocation())>FMath::Square(20000.))return;
    const FVector2D Here(GetActorLocation()),D=Route[RouteIndex]-Here;
    if(D.Size()<85){RouteIndex=(RouteIndex+1)%Route.Num();return;}
    FVector Next=GetActorLocation()+FVector(D.GetSafeNormal()*Dt*105,0);Next.Z=18;
    FHitResult Hit;SetActorLocation(Next,true,&Hit);SetActorRotation(FMath::RInterpTo(GetActorRotation(),FVector(D,0).Rotation(),Dt,2));
    if(Details.Num()>=3){const float Roll=GetWorld()->GetTimeSeconds()*227;Details[1]->SetRelativeRotation(FRotator(Roll,0,0));Details[2]->SetRelativeRotation(FRotator(Roll,0,0));Details[0]->SetRelativeRotation(FRotator(FMath::Sin(GetWorld()->GetTimeSeconds()*1.7f)*2,FMath::Sin(GetWorld()->GetTimeSeconds()*.6f)*24,0));}
    VoiceTimer-=Dt;if(VoiceTimer<0){World->Sound(TEXT("TraderVoice"),GetActorLocation(),.4f);VoiceTimer=30;}
}
