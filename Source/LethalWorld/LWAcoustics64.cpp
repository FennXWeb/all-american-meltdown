#include "LWAcoustics64.h"
#include "LWCharacter.h"
#include "LWVehicle.h"
#include "LWResident.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/ReverbEffect.h"
#include "GameFramework/Pawn.h"

namespace LWAcoustics64
{
FTransmission Transmission(float Coverage, float Thickness, float Cabin, bool InteriorSource)
{
    Coverage=FMath::Clamp(Coverage,0.f,1.f);
    const float Heavy=FMath::Clamp(Thickness/180.f,0.f,1.f);
    FTransmission R;
    R.Gain=FMath::Lerp(1.f,FMath::Lerp(.34f,.12f,Heavy),Coverage);
    R.Cutoff=Coverage<=0?20000.f:FMath::Exp(FMath::Lerp(FMath::Loge(20000.f),FMath::Loge(FMath::Lerp(1900.f,650.f,Heavy)),Coverage));
    if(!InteriorSource){Cabin=FMath::Clamp(Cabin,0.f,1.f);R.Gain*=FMath::Lerp(1.f,.38f,Cabin);R.Cutoff=FMath::Min(R.Cutoff,FMath::Lerp(20000.f,1600.f,Cabin));}
    R.Gain=FMath::Max(.045f,R.Gain); // retain a quiet warning through several barriers
    return R;
}
int RoomPreset(bool Roof,int Walls,float Volume,bool Cabin)
{
    if(Cabin)return 0;
    if(!Roof||Walls<3)return -1;
    return Volume<65?1:Volume<260?2:Volume<1200?3:4;
}
}

bool ULWAcoustics64::DoesSupportWorldType(EWorldType::Type T)const{return T==EWorldType::Game||T==EWorldType::PIE;}
void ULWAcoustics64::Initialize(FSubsystemCollectionBase& C)
{
    Super::Initialize(C);
    const float Decays[]={.18f,.48f,.95f,1.75f,2.8f};
    for(int I=0;I<5;++I){auto* R=NewObject<UReverbEffect>(this);R->DecayTime=Decays[I];R->Gain=I==0?.12f:.35f;R->GainHF=I==0?.24f:.56f;R->DecayHFRatio=.62f;R->Density=.8f;R->Diffusion=.88f;R->ReflectionsDelay=.006f+I*.008f;R->ReflectionsGain=.12f;R->LateDelay=.012f+I*.011f;R->LateGain=I==0?.3f:1.1f;Rooms.Add(R);}
}
void ULWAcoustics64::Deinitialize(){UGameplayStatics::DeactivateReverbEffect(this,TEXT("LWAcoustics64"));Sources.Empty();Rooms.Empty();Super::Deinitialize();}
void ULWAcoustics64::RefreshListener()
{
    Listener=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
    if(auto* P=Listener.Get())Ear=P->Camera?P->Camera->GetComponentLocation():P->GetPawnViewLocation();
    if(BudgetFrame!=GFrameCounter){BudgetFrame=GFrameCounter;TraceBudget=48;}
}
bool ULWAcoustics64::Trace(FHitResult& Hit,FVector Start,FVector End,AActor* Ignore,AActor* Other)
{
    if(TraceBudget<=0)return false;
    --TraceBudget;
    FCollisionQueryParams Q(SCENE_QUERY_STAT(LWAcoustics64),false,Ignore);if(Other)Q.AddIgnoredActor(Other);
    if(Listener.IsValid()&&Listener->Vehicle)Q.AddIgnoredActor(Listener->Vehicle);
    bool Blocked=GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Q);
    // Living actors should not acoustically behave as solid room walls.
    if(Blocked&&Cast<APawn>(Hit.GetActor())&&TraceBudget>0){Q.AddIgnoredActor(Hit.GetActor());--TraceBudget;Blocked=GetWorld()->LineTraceSingleByChannel(Hit,Start,End,ECC_Visibility,Q);}
    return Blocked;
}
LWAcoustics64::FTransmission ULWAcoustics64::Probe(FVector L,FVector S,AActor* LA,AActor* SA,float Cabin,bool Interior)
{
    float Coverage=0,Thickness=0;
    const FVector Side=FVector::CrossProduct((S-L).GetSafeNormal(),FVector::UpVector).GetSafeNormal();
    for(int I=0;I<3;++I){const FVector Offset=I==0?FVector::ZeroVector:Side*(I==1?55.f:-55.f)+FVector(0,0,25);FHitResult H;
        if(Trace(H,L+Offset,S+Offset,LA,SA)&&H.Distance<FVector::Distance(L,S)-20){Coverage+=1.f/3; if(I==0){FHitResult Back;if(Trace(Back,S,L,LA,SA))Thickness=FVector::Distance(H.ImpactPoint,Back.ImpactPoint);}}
    }
    return LWAcoustics64::Transmission(Coverage,Thickness,Cabin,Interior);
}
void ULWAcoustics64::MeasureRoom()
{
    auto* P=Listener.Get();if(!P)return;
    auto* V=P->Vehicle.Get();const bool Cabin=V&&V->Spec().Seats>1;
    int Next=0;
    if(!Cabin){
        const FVector Directions[]={FVector(1,0,0),FVector(-1,0,0),FVector(0,1,0),FVector(0,-1,0),FVector(0,0,1),FVector(0,0,-1),FVector(.707,.707,0),FVector(-.707,.707,0),FVector(.707,-.707,0),FVector(-.707,-.707,0)};
        float Dist[10];int Walls=0;bool Roof=false;
        for(int I=0;I<10;++I){FHitResult H;const bool Hit=Trace(H,Ear,Ear+Directions[I]*2500,P);Dist[I]=Hit?H.Distance:2500;if(I==4)Roof=Hit;if(I<4||I>5)Walls+=Hit?1:0;}
        const float Volume=(Dist[0]+Dist[1])*(Dist[2]+Dist[3])*(Dist[4]+Dist[5])/1000000.f;
        Next=LWAcoustics64::RoomPreset(Roof,Walls,Volume,false);
    }
    if(Next!=CandidateRoom){CandidateRoom=Next;StableRoom=1;}else ++StableRoom;
    if(Next!=CurrentRoom&&(StableRoom>=2||Cabin)){
        CurrentRoom=Next;
        if(Next<0)UGameplayStatics::DeactivateReverbEffect(this,TEXT("LWAcoustics64"));
        else UGameplayStatics::ActivateReverbEffect(this,Rooms[Next],TEXT("LWAcoustics64"),2,Next==0?.18f:.32f+Next*.06f,.8f);
    }
}
void ULWAcoustics64::Track(UAudioComponent* A,FName Slot,const FSoundAttenuationSettings& Base,bool Local)
{
    if(!A)return;RefreshListener();
    FSource S;S.Audio=A;S.Base=Base;S.Slot=Slot;S.Local=Local;
    // Bound storage as well as traces. Unmanaged overflow retains native attenuation/occlusion.
    if(Sources.Num()>=256){Sources.RemoveAll([](const FSource& X){return !X.Audio.IsValid()||!X.Audio->IsPlaying();});if(Sources.Num()>=256)return;}
    Sources.Add(MoveTemp(S));Process(Sources.Last(),true);
}
void ULWAcoustics64::Process(FSource& S,bool Initial)
{
    auto* A=S.Audio.Get();auto* P=Listener.Get();if(!A||!P||A->bIsFadingOut)return;
    auto Settings=S.Base;Settings.bEnableOcclusion=false;
    Settings.bEnableReverbSend=true;Settings.ReverbSendMethod=EReverbSendMethod::Manual;
    Settings.ManualReverbSendLevel=CurrentRoom<0?0:S.Local?.3f:.22f;
    if(S.Local){Settings.bAttenuate=false;Settings.bSpatialize=false;A->AdjustAttenuation(Settings);return;}
    auto* V=P->Vehicle.Get();float Cabin=V&&V->Spec().Seats>1?(V->CamperDoorOpen?.35f:1.f):0;
    AActor* Emitter=A->GetAttachParent()?A->GetAttachParent()->GetOwner():nullptr;
    // Cabin controls/radio stay clear. Engine, tire, weapon and exterior NPC sound does not.
    const FString Name=S.Slot.ToString();const bool Control=Name==TEXT("CarRadio")||Name==TEXT("CarSignal")||Name==TEXT("CarWipers")||Name.Contains(TEXT("Switch"))||Name.Contains(TEXT("Glove"));
    const bool OwnVehicle=V&&Emitter==V&&(Name.StartsWith(TEXT("Vehicle42_"))||Name.StartsWith(TEXT("EV74_"))||Name.StartsWith(TEXT("Car"))||Name.StartsWith(TEXT("Heli")));
    if(OwnVehicle){
        // Your own engine reaches the cabin through its mounts as well as the air.
        // Distance to the rear engine in a long coach must not make it nearly inaudible.
        Settings.bAttenuate=false;Settings.bSpatialize=false;Settings.bAttenuateWithLPF=true;
        const bool Motor=Name.StartsWith(TEXT("Vehicle42_"))||Name.Contains(TEXT("Motor"))||Name.Contains(TEXT("Engine"))||Name.Contains(TEXT("Load"));
        const float Cutoff=Control?18000.f:Motor?4600.f:6800.f;
        S.Cutoff=Cutoff;Settings.LPFFrequencyAtMin=Settings.LPFFrequencyAtMax=Cutoff;
        A->AdjustAttenuation(Settings);A->AdjustVolume(Initial?0.f:.18f,Control?1.f:.86f);return;
    }
    const auto* Resident=Cast<ALWResident>(Emitter);
    const bool Interior=(Resident&&V&&Resident->Riding==V)||(Control&&V&&FVector::DistSquared(A->GetComponentLocation(),V->GetActorLocation())<FMath::Square(V->Spec().HalfLength+150.f));
    // Ignore the occupied car in visibility probes; its shell transmission is accounted for separately.
    auto T=LWAcoustics64::Transmission(0,0,Cabin,Interior);
    if(TraceBudget>=8)T=Probe(Ear,A->GetComponentLocation(),P,Emitter?Emitter:V,Cabin,Interior);
    else Settings.bEnableOcclusion=S.Base.bEnableOcclusion;
    S.Cutoff=Initial?T.Cutoff:FMath::Lerp(S.Cutoff,T.Cutoff,.65f);
    // Air absorption and obstruction combine on the audio thread without touching RPM/slot gain.
    Settings.bAttenuateWithLPF=true;
    Settings.LPFFrequencyAtMin=FMath::Min(S.Base.LPFFrequencyAtMin,S.Cutoff);
    Settings.LPFFrequencyAtMax=FMath::Min(S.Base.LPFFrequencyAtMax,S.Cutoff);
    Settings.ManualReverbSendLevel*=FMath::Sqrt(T.Gain);
    A->AdjustAttenuation(Settings);
    A->AdjustVolume(Initial?0.f:.18f,T.Gain);
}
void ULWAcoustics64::Tick(float Dt)
{
    RefreshListener();if(!Listener.IsValid())return;
    RoomClock-=Dt;SourceClock-=Dt;
    if(RoomClock<=0&&TraceBudget>=20){RoomClock=.5f;MeasureRoom();}
    if(SourceClock>0)return;SourceClock=.05f;
    // Round-robin guarantees eventual updates; no scene-wide component enumeration or asset loads.
    for(int I=0;I<6&&!Sources.IsEmpty()&&TraceBudget>=8;++I){Cursor%=Sources.Num();auto& S=Sources[Cursor];if(!S.Audio.IsValid()||!S.Audio->IsPlaying()){Sources.RemoveAtSwap(Cursor);continue;}Process(S,false);++Cursor;}
}
