#include "LWNPCLife.h"
#include "LWZombie.h"
#include "LWCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
void ULWNPCLife::CreatureAnimation51(float Dt,ALWCharacter* P){
 if(NPC->bDead||NPC->SeveredMask||NPC->bCrawling||NPC->HeadlessTime>0||FVector::DistSquared(NPC->GetActorLocation(),P->GetActorLocation())>FMath::Square(14000.f))return;
 const auto Kind=NPC->Kind;
 // An observed, dormant mannequin must stay absolutely still, including its head and breathing.
 if(Kind==ELWEnemyKind::Mannequin&&!NPC->bAggressive&&NPC->IsObserved(P)){AnimationState=TEXT("Frozen");return;}
 if(!CreatureReady51){CreatureReady51=true;CreaturePhase51=FMath::Fmod(float(NPC->PersistentId),37.f);for(int I=0;I<7;I++){CreatureRest51[I]=NPC->Parts[I]->GetRelativeLocation();CreaturePose51[I]=FRotator::ZeroRotator;}}
 Dt=FMath::Min(Dt,.1f);float Speed=NPC->GetVelocity().Size2D(),Move=FMath::Clamp(Speed/220.f,0.f,1.f);const bool Giant=Kind==ELWEnemyKind::Titan||Kind==ELWEnemyKind::Behemoth||Kind==ELWEnemyKind::Colossus;
 const bool Quad=Kind==ELWEnemyKind::Dog||Kind==ELWEnemyKind::Moose;
 // Distance-derived gait: large feet cover metres per step instead of tiny fast shuffles.
 const float StepLength=Giant?150.f*NPC->GetActorScale3D().Z:Kind==ELWEnemyKind::Deathclaw?170.f:Kind==ELWEnemyKind::Dog?95.f:120.f;
 CreaturePhase51+=Dt*(Speed>15?Speed/StepLength*PI:.15f);
 NPC->CreatureFootfall54(CreaturePhase51,P);float T=CreaturePhase51,B=FMath::Sin(GetWorld()->GetTimeSeconds()*(Giant?.65f:1.6f)+NPC->PersistentId%13),Attack=FMath::Clamp(FMath::Max(NPC->CreatureWindup,NPC->AttackWindup)*1.7f,0.f,1.f);
 AnimationState=Attack>0?TEXT("Windup"):NPC->CreatureCharge>0?TEXT("Charge"):Move>.15f?TEXT("Locomotion"):TEXT("Watch / breathe");
 FRotator Goal[7];for(auto& G:Goal)G=FRotator::ZeroRotator;
 Goal[0].Roll=B*(Giant?1.5f:.6f)+FMath::Sin(T)*Move*(Giant?2.8f:1.f);
 Goal[1].Yaw=FMath::Sin(T*.37f)*7*(1-Move);Goal[1].Pitch=B*1.5f;
 for(int S=0;S<2;++S){float Stride=FMath::Sin(T+S*PI);Goal[3+S].Pitch=-Stride*Move*(Giant?17:28);Goal[5+S].Pitch=Stride*Move*(Giant?32:30);Goal[5+S].Roll=Giant?(S?1:-1)*4:0;}
 if(Quad){for(int I=3;I<7;++I)Goal[I].Pitch=FMath::Sin(T+((I==3||I==6)?0:PI))*Move*29;Goal[1].Pitch+=(1-Move)*FMath::Max(0.f,FMath::Sin(T*.24f))*17-Attack*19;Goal[0].Pitch=NPC->CreatureCharge>0?-5:0;}
 if(Giant){Goal[0].Pitch+=Move*5-Attack*9;Goal[3].Pitch-=Attack*68;Goal[4].Pitch-=Attack*52;Goal[1].Pitch+=Attack*8;}
 if(Kind==ELWEnemyKind::Deathclaw){
  const bool Charge=NPC->CreatureCharge>0;float Blend=FMath::FInterpTo(Quadruped54,Charge?1.f:0.f,Dt,12);Quadruped54=Blend;
  const FVector Offsets[]={FVector(34,0,-71),FVector(46,0,-74),FVector(-15,0,-55),FVector(42,0,-75),FVector(42,0,-75),FVector(0,0,-55),FVector(0,0,-55)};
  for(int I=0;I<7;++I)NPC->Parts[I]->SetRelativeLocation(CreatureRest51[I]+Offsets[I]*Blend);
  for(int I=3;I<7;++I){float Crawl=(I>=5?-55.f:0.f)+FMath::Sin(T+((I==3||I==6)?0:PI))*22*Move;Goal[I].Pitch=FMath::Lerp(Goal[I].Pitch,Crawl,Blend);}Goal[1].Pitch=FMath::Lerp(Goal[1].Pitch,-15.f,Blend);
  Goal[0].Pitch=8+Move*7-Blend*65;Goal[3].Roll=-14;Goal[4].Roll=14;Goal[3].Pitch-=Attack*65;Goal[4].Pitch-=Attack*35;Goal[2].Yaw=FMath::Sin(T*.6f)*8;}
 if(Kind==ELWEnemyKind::Scorpion){Goal[2].Pitch=-Attack*24+B*4;for(int I=3;I<7;++I){Goal[I]=FRotator(0,(I==5?180.f:I==3?-30.f:I==4?30.f:0.f)+FMath::Sin(T+I*1.7f)*(Move*14+2),0);}Goal[3].Yaw-=Attack*25;Goal[4].Yaw+=Attack*25;}
 if(Kind==ELWEnemyKind::Karen){Goal[3]=FRotator(-70+B*2,-2,0);Goal[4]=FRotator(-70-B*2,2,0);Goal[5]=Goal[6]=FRotator(60,0,0);Goal[0].Pitch=Move*4+Attack*12;Goal[1].Yaw=FMath::Sin(T*.41f)*10;}
 if(Kind==ELWEnemyKind::Mannequin){Goal[1].Roll=8;Goal[3].Pitch-=NPC->bAggressive?35:5;Goal[4].Pitch-=NPC->bAggressive?35:0;Goal[0].Roll=0;}
 if(Kind==ELWEnemyKind::WorldEater){if(NPC->BurrowPhase!=0)return;for(int I=0;I<7;++I)Goal[I]=FRotator(FMath::Sin(T*1.7f-I*.65f)*4,FMath::Sin(T*.9f-I*.45f)*6,0);}
 // Carry/throw poses remain owned by the giant's combat animation.
 if(NPC->HeldCar49.IsValid())return;
 for(int I=0;I<7;++I){CreaturePose51[I]=FMath::RInterpTo(CreaturePose51[I],Goal[I],Dt,Giant?4.f:9.f);NPC->Parts[I]->SetRelativeRotation(CreaturePose51[I]);}
}
