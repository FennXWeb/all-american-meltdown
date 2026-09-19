#include "LWNPCLife.h"
#include "LWCreator35.h"
#include "LWZombie.h"
#include "LWResident.h"
#include "LWCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"
#include "Engine/World.h"
#include "Materials/MaterialInterface.h"
#include "Kismet/GameplayStatics.h"
#include "KismetProceduralMeshLibrary.h"
#include "Misc/Crc.h"

namespace { TArray<TWeakObjectPtr<ULWNPCLife>> DetailQueue34; }

ULWNPCLife::ULWNPCLife(){PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.TickGroup=TG_PostPhysics;}
void ULWNPCLife::BeginPlay(){Super::BeginPlay();NPC=Cast<ALWZombie>(GetOwner());if(NPC)AddTickPrerequisiteActor(NPC);}
void ULWNPCLife::EndPlay(const EEndPlayReason::Type R){if(Speech.IsValid())Speech->OnAudioSingleEnvelopeValue.RemoveDynamic(this,&ULWNPCLife::Envelope);DetailQueue34.Remove(this);RestoreVisuals();Super::EndPlay(R);}
float ULWNPCLife::BlinkShape(float T){return T<0||T>.19f?0:FMath::Sin(PI*T/.19f);}
FVector ULWNPCLife::Bend(FVector V,float Joint,float Degrees){
 const float W=(1-FMath::SmoothStep(Joint-5.f,Joint+4.f,float(V.Z)));
 return FQuat(FVector::YAxisVector,FMath::DegreesToRadians(Degrees)*W).RotateVector(V-FVector(0,0,Joint))+FVector(0,0,Joint);
}
void ULWNPCLife::Envelope(const USoundWave*,float Value){VoiceLevel=FMath::Clamp(Value*6.f,0.f,1.f);EnvelopeClock=Clock;HasEnvelope=true;}
void ULWNPCLife::Speak(UAudioComponent* Audio,const FString& Text){
 if(Speech.IsValid())Speech->OnAudioSingleEnvelopeValue.RemoveDynamic(this,&ULWNPCLife::Envelope);
 Speech=Audio;HadSpeechAudio=Audio!=nullptr;HasEnvelope=false;VoiceLevel=0;VoiceEnd=Clock+FMath::Clamp(Text.Len()*.045f,1.f,5.f);
 if(Audio)Audio->OnAudioSingleEnvelopeValue.AddDynamic(this,&ULWNPCLife::Envelope);
 NextGesture=Clock; // A new sentence starts a new gesture, independent of subtitle persistence.
}
void ULWNPCLife::RestoreVisuals(){
 if(NPC)for(int I=0;I<Visuals.Num();I++)if(Visuals[I]){if(NPC->Parts.IsValidIndex(I))NPC->Parts[I]->SetVisibility(true,false);Visuals[I]->DestroyComponent();}
 if(Mouth)Mouth->DestroyComponent();Mouth=nullptr;Visuals.Empty();Originals.Empty();Sources.Empty();
}
void ULWNPCLife::BuildVisuals(){
 if(!NPC||NPC->Parts.Num()!=7)return;
 if(Visuals.Num()==7){bool Changed=false;for(int I:{0,1,2,3,4,5,6})Changed|=Sources[I].Get()!=NPC->Parts[I]->GetStaticMesh();if(!Changed)return;RestoreVisuals();}
 Visuals.SetNum(7);Originals.SetNum(7);Sources.SetNum(7);
 for(int I:{0,1,2,3,4,5,6}){
  auto* Part=NPC->Parts[I].Get();UStaticMesh* Mesh=Part->GetStaticMesh();Sources[I]=Mesh;
  if(!Mesh||!Mesh->bAllowCPUAccess)continue;
  auto* V=NewObject<UProceduralMeshComponent>(NPC);V->SetupAttachment(Part);V->SetCollisionEnabled(ECollisionEnabled::NoCollision);V->SetCanEverAffectNavigation(false);V->RegisterComponent();
  UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(Part,0,V,false);Visuals[I]=V;
  for(int S=0;S<V->GetNumSections();S++){V->SetMaterial(S,Part->GetMaterial(Mesh->GetRenderData()->LODResources[0].Sections[S].MaterialIndex));auto Section=*V->GetProcMeshSection(S);if(Mesh->GetName().EndsWith(TEXT("35")))LWCreator35::Surface(Section,I,NPC->Appearance35);V->SetProcMeshSection(S,Section);Originals[I].Add(Section);}
  Part->SetVisibility(false,false);
 }
 if(Visuals[1]){
  Mouth=NewObject<UProceduralMeshComponent>(NPC);Mouth->SetupAttachment(NPC->Parts[1]);Mouth->SetCollisionEnabled(ECollisionEnabled::NoCollision);Mouth->SetCanEverAffectNavigation(false);Mouth->RegisterComponent();
  Mouth->SetMaterial(0,LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Materials/M_NPCMouth34.M_NPCMouth34")));Mouth->SetCastShadow(false);
 }
}
void ULWNPCLife::Deform(float Dt,bool Talking,bool Angry){
 float Voice=0;
 if(Talking){if(HasEnvelope)Voice=Clock-EnvelopeClock<.2f?VoiceLevel:0;else Voice=.2f+.65f*FMath::Abs(FMath::Sin(Clock*19+Personality)*FMath::Sin(Clock*7.3f));}
 MouthOpen=FMath::FInterpTo(MouthOpen,Voice,Dt,18);
 for(int I:{1,3,4,5,6})if(Visuals.IsValidIndex(I)&&Visuals[I]){
  auto* V=Visuals[I].Get();for(int S=0;S<Originals[I].Num();S++){
   const auto& Base=Originals[I][S];TArray<FVector> Pos,Normals;TArray<FVector2D> UV;TArray<FProcMeshTangent> Tangents;
   Pos.Reserve(Base.ProcVertexBuffer.Num());Normals.Reserve(Base.ProcVertexBuffer.Num());UV.Reserve(Base.ProcVertexBuffer.Num());Tangents.Reserve(Base.ProcVertexBuffer.Num());
   for(const auto& Vertex:Base.ProcVertexBuffer){FVector P=Vertex.Position,N=Vertex.Normal;FProcMeshTangent T=Vertex.Tangent;
    if(I==1){
     // Masks are in centimetres, calibrated to the sculpted humanoid face. Back/neck stay rigid.
     const float Front=FMath::SmoothStep(2.f,7.f,float(P.X));const float Eye=FMath::Exp(-FMath::Square((FMath::Abs(P.Y)-3.5)/1.5)-FMath::Square((P.Z+3.5)/1.1))*Front;
     P.Z+=(-3.5-P.Z)*Blink*.94f*Eye;
     const float Brow=FMath::Exp(-FMath::Square((FMath::Abs(P.Y)-3.5)/2)-FMath::Square((P.Z+1.8)/1.7))*Front;
     P.Z+=Brow*(Angry?-.55f:.25f*FMath::Sin(Clock*2.1f))*ExpressionStrength;
     const float Jaw=Front*(1-FMath::SmoothStep(-12.f,-10.5f,float(P.Z)));P.Z-=MouthOpen*1.45f*Jaw;P.X-=MouthOpen*.35f*Jaw;
     const float Corner=Front*FMath::Exp(-FMath::Square((FMath::Abs(P.Y)-2.6)/1.3)-FMath::Square((P.Z+10.9)/1.6));P.Z+=Corner*(Angry?-.25f:.22f*FMath::Sin(Clock*.7f+Personality))*ExpressionStrength;
    }else{const float Joint=I<5?-27.f:-40.2f;const float Angle=I<5?Elbows[I-3]:Knees[I-5];const float W=(1-FMath::SmoothStep(Joint-5.f,Joint+4.f,float(P.Z)));FQuat Q(FVector::YAxisVector,FMath::DegreesToRadians(Angle)*W);P=Bend(P,Joint,Angle);N=Q.RotateVector(N);T.TangentX=Q.RotateVector(T.TangentX);}
    Pos.Add(P);Normals.Add(N);UV.Add(Vertex.UV0);Tangents.Add(T);
   }
   V->UpdateMeshSection_LinearColor(S,Pos,Normals,UV,TArray<FLinearColor>(),Tangents);
   V->SetMaterial(S,NPC->Parts[I]->GetMaterial(NPC->Parts[I]->GetStaticMesh()->GetRenderData()->LODResources[0].Sections[S].MaterialIndex));
  }
 }
 if(Mouth){
  Mouth->SetVisibility(MouthOpen>.06f);TArray<FVector> P;TArray<int32> Tri;TArray<FVector> N;TArray<FVector2D> UV;
  P.Add(FVector(9.15-MouthOpen*.12f,0,-10.8-MouthOpen*.65f));N.Add(FVector(1,0,0));UV.Add(FVector2D(.5,.5));
  for(int I=0;I<=16;I++){float A=I*2*PI/16;P.Add(FVector(9.15-MouthOpen*.12f-.45f*FMath::Abs(FMath::Cos(A)),FMath::Cos(A)*2.15f,-10.8-MouthOpen*.65f+FMath::Sin(A)*MouthOpen*.85f));N.Add(FVector(1,0,0));UV.Add(FVector2D(.5,.5));if(I<16)Tri.Append({0,I+1,I+2,0,I+2,I+1});}
  Mouth->CreateMeshSection_LinearColor(0,P,Tri,N,UV,TArray<FLinearColor>(),TArray<FProcMeshTangent>(),false);
 }
}
void ULWNPCLife::TickComponent(float Dt,ELevelTick Type,FActorComponentTickFunction* Function){
 Super::TickComponent(Dt,Type,Function);if(!NPC||NPC->Parts.Num()!=7)return;
 auto* Player=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));if(!Player||!Player->bStarted||Player->bMenu)return;
 auto* R=Cast<ALWResident>(NPC);const bool Human=R||NPC->Kind==ELWEnemyKind::Zombie||NPC->Kind==ELWEnemyKind::Raider;
 if(!Human){DetailQueue34.Remove(this);if(!Visuals.IsEmpty())RestoreVisuals();CreatureAnimation51(Dt,Player);return;}
 if(NPC->bDead||NPC->SeveredMask||NPC->bCrawling||NPC->HeadlessTime>0){DetailQueue34.Remove(this);AnimationState=TEXT("Physics / injury");if(Mouth)Mouth->SetVisibility(false);return;}
 if(!Initialized){Random.Initialize(R?FCrc::StrCrc32(*R->ResidentId.ToString()):NPC->PersistentId^FCrc::StrCrc32(*NPC->Home.ToString()));Personality=Random.FRandRange(0,2*PI);Clock=Personality;Phase=Personality;NextBlink=Clock+Random.FRandRange(.3f,4.f);for(int I=0;I<7;I++)Pose[I]=NPC->Parts[I]->GetRelativeRotation();Initialized=true;}
 Dt=FMath::Min(Dt,.1f);Clock+=Dt;const float Distance=FVector::Distance(NPC->GetActorLocation(),Player->GetActorLocation());
 if(Distance>5000){DetailQueue34.Remove(this);if(!Visuals.IsEmpty())RestoreVisuals();return;}
 if(PreviousHealth>=0&&NPC->Health<PreviousHealth)Flinch=1;PreviousHealth=NPC->Health;Flinch=FMath::Max(0.f,Flinch-Dt*3);
 if(Clock>=NextBlink){BlinkStart=Clock;NextBlink=Clock+Random.FRandRange(2.3f,6.5f);}Blink=BlinkShape(Clock-BlinkStart);
 const bool Talking=HadSpeechAudio?(Speech.IsValid()&&Speech->IsPlaying()):Clock<VoiceEnd;
 const bool Riding=R&&R->Riding;const bool Down=R&&R->DownTime>0;
 const bool Combat=NPC->AttackWindup>0||NPC->ReloadClock>0||(R?(R->FireTime>0||R->IsTownHostile()):NPC->HasVisual);
 const bool Listening=R&&Player->Speaker==R&&!Talking;
 const float Speed=Riding?0:NPC->GetVelocity().Size2D();SmoothedSpeed=FMath::FInterpTo(SmoothedSpeed,Speed,Dt,7);const float Move=FMath::Clamp(SmoothedSpeed/180.f,0.f,1.f);
 Phase+=Dt*(1.2f+SmoothedSpeed*.036f);if(!Riding&&!Down)NPC->CreatureFootfall54(Phase,Player);const float Breath=FMath::Sin(Clock*(1.65f+Personality*.06f));
 if(Clock>NextGesture){const int Old=Gesture;Gesture=(Old+1+Random.RandRange(0,10))%12;GestureStart=Clock;NextGesture=Clock+Random.FRandRange(Talking?2.1f:5.f,Talking?4.f:11.f);}
 const float Beat=FMath::Clamp(FMath::Sin(FMath::Clamp((Clock-GestureStart)/3.f,0.f,1.f)*PI),0.f,1.f)*GestureStrength*(1-Move);
 FRotator Goal[7];for(auto& A:Goal)A=FRotator::ZeroRotator;float E[2]={8,8},K[2]={};
 Goal[0].Roll=Breath*.65f+FMath::Sin(Phase)*Move*1.6f;Goal[0].Pitch=FMath::Clamp((Speed-PreviousSpeed)*.018f,-4.f,6.f)+Move*4;PreviousSpeed=Speed;
 for(int Side=0;Side<2;Side++){float S=FMath::Sin(Phase+Side*PI);Goal[3+Side].Pitch=-S*(NPC->Kind==ELWEnemyKind::Zombie&&!R?16:25)*Move;Goal[5+Side].Pitch=S*30*Move;K[Side]=FMath::Max(0.f,-S)*48*Move;E[Side]=12+Move*(Speed>220?48:12);}
 AnimationState=Speed>220?TEXT("Run"):Speed>10?TEXT("Walk"):TEXT("Idle");
 if(!Combat&&!Riding&&!Down&&Move<.5f){
  const float B=Beat*(Talking?1.f:.55f);const int Side=Gesture%2;
  // Twelve silhouettes: explain, offer, point, shrug, count, reassure, wipe brow,
  // check wrist, inspect hand, rub neck, stretch shoulder, fold arms.
  static const float Raise[12]={30,42,68,24,35,28,105,52,55,110,78,48};
  static const float BendAngle[12]={65,45,12,72,88,70,92,95,85,105,82,108};
  Goal[3+Side].Pitch+=Raise[Gesture]*B;Goal[3+Side].Roll+=(Side?1:-1)*(Gesture==3?35:14)*B;E[Side]+=BendAngle[Gesture]*B;
  if(Gesture==3||Gesture==11){Goal[4-Side].Pitch+=Raise[Gesture]*B;E[1-Side]+=BendAngle[Gesture]*B;Goal[4-Side].Roll+=(Side?-1:1)*22*B;}
  Goal[0].Yaw=FMath::Sin(Personality)*B*4;Goal[0].Roll+=FMath::Sin(Personality+1)*B*2;
  if(Talking){AnimationState=TEXT("Speaking");Goal[1].Pitch+=FMath::Sin(Clock*3.8f)*2.5f*Beat;}
  else if(Listening){AnimationState=TEXT("Listening");Goal[1].Pitch+=FMath::Max(0.f,FMath::Sin(Clock*1.7f))*3;}
 }
 if(Combat){AnimationState=TEXT("Combat");Goal[0].Pitch+=5;if(NPC->Gun&&NPC->Gun->IsVisible()){Goal[3].Pitch=Goal[4].Pitch=-65;E[0]=E[1]=0;}else if(NPC->AttackWindup>0){Goal[3]=NPC->Parts[3]->GetRelativeRotation();Goal[4]=NPC->Parts[4]->GetRelativeRotation();if(NPC->AttackStyle54==1){Goal[3].Roll=-55;Goal[4].Pitch=-20;Goal[0].Yaw=-18;}else if(NPC->AttackStyle54==2){Goal[3].Pitch=Goal[4].Pitch=-140;Goal[0].Pitch=-10;}E[0]=E[1]=0;}if(NPC->ReloadClock>0){Goal[3].Roll=FMath::Sin(Clock*9)*15;E[0]=35;}}
 if(Riding){AnimationState=TEXT("Seated");Goal[5].Pitch=Goal[6].Pitch=85;K[0]=K[1]=85;Goal[0].Pitch=3;Goal[3].Pitch=Goal[4].Pitch=R->SeatIndex==0?45:12;E[0]=E[1]=R->SeatIndex==0?35:65;}
 if(Down){AnimationState=TEXT("Wounded");Goal[0].Pitch=55;Goal[3].Pitch=35;E[0]=100;}
 GazeClock-=Dt;if(GazeClock<=0){GazeClock=Random.FRandRange(.35f,1.3f);FVector Target=NPC->GetActorLocation()+NPC->GetActorForwardVector()*500;
  if((Talking||Listening||Combat)&&Distance<1800)Target=(R&&R->CompanionThreat.IsValid()?R->CompanionThreat->GetActorLocation():Player->GetActorLocation())+FVector(0,0,55);
  FRotator Look=NPC->GetActorTransform().InverseTransformVectorNoScale(Target-NPC->Parts[1]->GetComponentLocation()).Rotation();Gaze=FRotator(FMath::Clamp(Look.Pitch,-12.f,12.f),FMath::Clamp(Look.Yaw,-42.f,42.f),0);if(!Talking&&!Listening&&!Combat)Gaze.Yaw=Random.FRandRange(-24,24);
 }
 if(!R&&NPC->Kind==ELWEnemyKind::Zombie){Goal[0].Roll+=FMath::Sin(Clock*.61f+Personality)*3;Goal[1].Roll+=FMath::Sin(Clock*.43f)*4;Goal[4].Pitch+=7+Personality*1.5f;}
 Goal[1]+=Gaze;Goal[0].Pitch-=Flinch*12;Goal[1].Roll+=Flinch*10;
 for(int I=0;I<7;I++){Pose[I]=FMath::RInterpTo(Pose[I],Goal[I],Dt,I==1?5.f:9.f);NPC->Parts[I]->SetRelativeRotation(Pose[I]);}
 for(int I=0;I<2;I++){Elbows[I]=FMath::FInterpTo(Elbows[I],-E[I],Dt,9);Knees[I]=FMath::FInterpTo(Knees[I],K[I],Dt,10);}
 DetailClock+=Dt;if(Distance>DetailDistance){DetailQueue34.Remove(this);if(!Visuals.IsEmpty())RestoreVisuals();return;}
 const float Interval=Distance<650?1.f/30:1.f/15;if(DetailClock<Interval)return;
 // Bound CPU skinning across a crowded settlement; independent phases avoid synchronized updates.
 DetailQueue34.RemoveAll([](const auto& C){return !C.IsValid();});DetailQueue34.AddUnique(this);
 static uint64 Frame=0;static int Updates=0;if(Frame!=GFrameCounter){Frame=GFrameCounter;Updates=0;}if(Updates>=6||DetailQueue34[0].Get()!=this)return;DetailQueue34.RemoveAt(0);Updates++;
 BuildVisuals();Deform(DetailClock,Talking,Combat||Flinch>0);DetailClock=0;
}
