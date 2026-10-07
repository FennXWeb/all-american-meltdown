#include "LWAcoustics64.h"
#include "Misc/AutomationTest.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWAcousticResponse64,"LethalWorld.Audio64.TransmissionAndRooms",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWAcousticResponse64::RunTest(const FString&)
{
    using namespace LWAcoustics64;
    auto Open=Transmission(0,0,0,false),Thin=Transmission(1,15,0,false),Thick=Transmission(1,180,0,false),Edge=Transmission(1.f/3,15,0,false),Car=Transmission(0,0,1,false),Radio=Transmission(0,0,1,true);
    TestEqual(TEXT("unobstructed volume"),Open.Gain,1.f);TestEqual(TEXT("unobstructed bandwidth"),Open.Cutoff,20000.f);
    TestTrue(TEXT("wall muffles and quiets"),Thin.Gain<.4f&&Thin.Cutoff<2000);
    TestTrue(TEXT("thick wall is stronger than thin wall"),Thick.Gain<Thin.Gain&&Thick.Cutoff<Thin.Cutoff);
    TestTrue(TEXT("doorway edge transmits more than full obstruction"),Edge.Gain>Thin.Gain&&Edge.Cutoff>Thin.Cutoff);
    TestTrue(TEXT("cabin muffles exterior source"),Car.Gain<.5f&&Car.Cutoff<2000);
    TestEqual(TEXT("cabin controls stay clear"),Radio.Gain,1.f);
    TestTrue(TEXT("open camper door transmits more"),Transmission(0,0,.35f,false).Gain>Car.Gain);
    TestEqual(TEXT("outdoor"),RoomPreset(false,8,100,false),-1);TestEqual(TEXT("awning"),RoomPreset(true,1,100,false),-1);
    TestEqual(TEXT("cabin"),RoomPreset(false,0,0,true),0);TestEqual(TEXT("small room"),RoomPreset(true,8,40,false),1);
    TestEqual(TEXT("medium room"),RoomPreset(true,8,120,false),2);TestEqual(TEXT("hall"),RoomPreset(true,8,800,false),3);TestEqual(TEXT("warehouse"),RoomPreset(true,8,2400,false),4);
    return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWAcousticGeometry64,"LethalWorld.Audio64.LiveGeometry",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWAcousticGeometry64::RunTest(const FString&)
{
    auto IV=UWorld::InitializationValues().AllowAudioPlayback(false).RequiresHitProxies(false).CreatePhysicsScene(true).CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false).SetTransactional(false);
    auto* W=UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&IV);
    auto* A=W->GetSubsystem<ULWAcoustics64>();if(!TestNotNull(TEXT("world acoustics created"),A)){W->DestroyWorld(false);return false;}
    auto* Wall=W->SpawnActor<AActor>();auto* B=NewObject<UBoxComponent>(Wall);Wall->SetRootComponent(B);B->SetBoxExtent(FVector(15,300,300));B->SetCollisionEnabled(ECollisionEnabled::QueryOnly);B->SetCollisionResponseToAllChannels(ECR_Block);B->RegisterComponent();Wall->SetActorLocation(FVector(500,0,200));
    const FVector L(0,0,200),S(1000,0,200);auto Blocked=A->Probe(L,S,nullptr,nullptr,0,false);
    TestTrue(TEXT("real wall obstructs source"),Blocked.Gain<.4f&&Blocked.Cutoff<2000);
    B->SetCollisionEnabled(ECollisionEnabled::NoCollision);auto Open=A->Probe(L,S,nullptr,nullptr,0,false);TestEqual(TEXT("opened/destroyed obstruction restores volume"),Open.Gain,1.f);
    B->SetCollisionEnabled(ECollisionEnabled::QueryOnly);auto Ignored=A->Probe(L,S,nullptr,Wall,0,false);TestEqual(TEXT("emitter geometry is ignored"),Ignored.Gain,1.f);
    W->DestroyWorld(false);return true;
}
#endif
