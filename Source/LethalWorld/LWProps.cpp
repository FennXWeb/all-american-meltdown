#include "LWCharacter.h"
#include "Misc/Crc.h"
#include "LWWorldObject.h"
#include "LWVehicleSpec.h"
#include "LWWorld.h"
#include "LWLootTable.h"
#include "Components/StaticMeshComponent.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void ALWWorldObject::ConfigureProp()
{
    bChanged=World->PropStates.FindRef(RecordId)!=0;Body->SetCollisionObjectType(ECC_WorldDynamic);
    if(Kind==ELWObjectKind::MannequinDisplay){
     Body->SetStaticMesh(World->Mesh(TEXT("MannequinTorso32")));Body->SetRelativeLocation(FVector(0,0,63));
     const TCHAR* N[]={TEXT("MannequinHead32"),TEXT("MannequinPelvis32"),TEXT("MannequinArm32"),TEXT("MannequinArm32"),TEXT("MannequinLeg32"),TEXT("MannequinLeg32")};
     const FVector V[]={FVector(0,0,91),FVector(0,0,12),FVector(0,-18,58),FVector(0,18,58),FVector(0,-9,-1),FVector(0,9,-1)};
     for(int I=0;I<6;I++)Part(N[I],V[I],FVector(1));return;
    }
    if(Kind==ELWObjectKind::Car){
        Body->SetStaticMesh(World->Mesh(TEXT("Wreck")));
        if(!World->Containers.Contains(RecordId)){
            FLWContainerRecord R;R.Id=RecordId;R.Context=TEXT("road");R.Position=GetActorLocation();auto VIN=FGuid::NewDeterministicGuid(RecordId.ToString(),uint64(uint32(World->Seed)));const auto& Spec=LWTraffic::Get(LWTraffic::Choose(FCrc::StrCrc32(*VIN.ToString())));R.Width=Spec.CargoW;R.Height=Spec.CargoH;
            auto* Player=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
            for(auto Item:LWLoot::Roll(R.Context,int32(FCrc::StrCrc32(*RecordId.ToString())^uint32(World->Seed)),Player?Player->Stat(TEXT("loot")):0)){if(Item.Slot==TEXT("Loaded"))R.Items.Add(Item);else LWItems::Place(R.Items,Item,R.Width,R.Height);}World->Containers.Add(RecordId,R);
        }
    }
    if(Kind==ELWObjectKind::Door){Body->SetStaticMesh(World->Mesh(TEXT("POIDoor")));DoorAngle=bChanged?100:0;Body->SetRelativeRotation(FRotator(0,DoorAngle,0));}
    if(Kind==ELWObjectKind::Window){Body->SetStaticMesh(World->Mesh(TEXT("Cube")));Body->SetMaterial(0,World->Material(TEXT("WindowGlass")));if(bChanged){Body->SetVisibility(false);Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);}}
    if(Kind==ELWObjectKind::FuelPump){Body->SetStaticMesh(World->Mesh(TEXT("FuelPump")));if(bChanged)for(int I=0;I<Body->GetNumMaterials();I++)Body->SetMaterial(I,World->Material(TEXT("Rubber")));}
    if(Kind==ELWObjectKind::Sign){Body->SetStaticMesh(World->Mesh(TEXT("SignFrameV3")));Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);}
}
void ALWWorldObject::SignFace(int32 Type)
{
    const TCHAR* Names[]={TEXT("SignGasV3"),TEXT("SignMotelV3"),TEXT("SignClinicV3"),TEXT("SignDepotV3"),TEXT("SignDinerV3")};
    auto* Face=NewObject<UProceduralMeshComponent>(this);Face->SetupAttachment(Root);
    TArray<FVector> V={FVector(7,100,-50),FVector(7,-100,-50),FVector(7,-100,50),FVector(7,100,50)};
    Face->CreateMeshSection(0,V,{0,1,2,0,2,3},{FVector(1,0,0),FVector(1,0,0),FVector(1,0,0),FVector(1,0,0)},
        {FVector2D(0,1),FVector2D(1,1),FVector2D(1,0),FVector2D(0,0)},TArray<FColor>(),TArray<FProcMeshTangent>(),false);
    Face->SetMaterial(0,World->Material(Names[FMath::Clamp(Type,0,4)]));Face->RegisterComponent();
}
float ALWWorldObject::TakeDamage(float D,const FDamageEvent& E,AController* DamageInstigator,AActor* Causer)
{
    if(D<=0||bChanged||!World||(Kind!=ELWObjectKind::Window&&Kind!=ELWObjectKind::FuelPump))return 0;
    bChanged=true;World->PropStates.Add(RecordId,1);
    const bool Pump=Kind==ELWObjectKind::FuelPump;
    World->Sound(Pump?TEXT("FuelExplosion"):TEXT("GlassBreak"),GetActorLocation(),1,1,Pump);
    World->Noise(GetActorLocation(),Pump?11000:2000);
    if(!Pump){Body->SetVisibility(false);Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);}
    else {
        for(int I=0;I<Body->GetNumMaterials();I++)Body->SetMaterial(I,World->Material(TEXT("Rubber")));
        // Mark spent before radial damage, so neighbouring pumps can safely chain once.
        UGameplayStatics::ApplyRadialDamageWithFalloff(this,170,15,GetActorLocation()+FVector(0,0,100),250,1050,1,nullptr,{this},this,DamageInstigator,ECC_Visibility);
    }
    for(int I=0;I<(Pump?10:12);I++){
        Part(TEXT("Cube"),FVector(FMath::FRandRange(-35.f,35.f),FMath::FRandRange(-35.f,35.f),FMath::FRandRange(30.f,160.f)),FVector(Pump?.6f:.045f),FRotator::MakeFromEuler(FVector(I*31,I*73,I*17)),Pump?TEXT("Glow"):TEXT("Steel"));
        auto* C=Details.Last().Get();C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        // World-sized fragments must not inherit a pane's nonuniform scale.
        C->SetWorldScale3D(Pump?FVector(.6f):FVector(.06f,.01f,.10f));
        if(!Pump){
            C->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
            C->SetWorldLocation(GetActorLocation()+FVector(FMath::FRandRange(-45.f,45.f),FMath::FRandRange(-45.f,45.f),FMath::FRandRange(-45.f,45.f)));
            C->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);C->SetCollisionObjectType(ECC_PhysicsBody);C->SetCollisionResponseToAllChannels(ECR_Ignore);C->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);
            C->SetSimulatePhysics(true);C->SetMassOverrideInKg(NAME_None,.03f,true);C->AddImpulse(FVector(FMath::FRandRange(-150.f,150.f),FMath::FRandRange(-150.f,150.f),70),NAME_None,true);
        }
    }
    EffectTime=Pump?1.6f:2.f;return D;
}
