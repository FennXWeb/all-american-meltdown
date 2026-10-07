#include "LWCampaignProduction77.h"
#include "LWCampaign76.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWNPCLife.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

void ULWCampaignProduction77::StartCrowd(FName Kind){
 const bool Meridian=LWCampaign76::Stage(Director->State().Stage)->Site==TEXT("meridian");
 const TCHAR* Names[]={TEXT("Nora Ellis"),TEXT("Victor Hale"),TEXT("Amina Grant"),TEXT("Philip Reed"),TEXT("Celia Park"),TEXT("Owen Marlow"),TEXT("Rosa Bellamy"),TEXT("Jonah Ortiz"),TEXT("Amelia Brooks"),TEXT("Daniel Mercer")};
 for(int I=0;I<10;I++){
  const FName Id(*FString::Printf(TEXT("crowd77_%s_%d"),*Kind.ToString(),I));if(!LWCampaign76::Alive(Director->State(),Id))continue;
  FLWCrowd77 Entry;const float Side=I%2?1:-1;const float Row=Meridian?-2300+(I/4)*2300:0;
  Entry.Route={FVector(Side*(Meridian?1850:1800),Row+250*(I%4),92),FVector(Side*(Meridian?1450:1250),Row,92),FVector(Side*450,Row,92),FVector(Side*450,Meridian?-3050:-2100,92),FVector(0,Meridian?-3270:-2180,92),FVector(0,Meridian?-3700:-2600,92),FVector(Side*(250+I*55),Meridian?-3900:-2750,92)};
  if(Meridian&&Side>0&&Row<2300){
   // School residents start in the clear teaching aisle, then pass through
   // their classroom opening before joining the main concourse. Starting
   // beside a pupil chair can put a capsule on top of the desk collision.
   const float RoomX=I%4==1?1900:3300;
   Entry.Route={FVector(RoomX,Row-235,92),FVector(RoomX,Row,92),FVector(1450,Row,92),FVector(450,Row,92),FVector(450,-3050,92),FVector(0,-3270,92),FVector(0,-3700,92),FVector(250+I*55,-3900,92)};
  }
  if(!Meridian){
   // Ward/storage aisles flank the beds and shelves. Cross the partition at
   // its actual opening; generic 250cm rows put two people inside this wall.
   Entry.Route={FVector(Side*1950,-1500+(I/2)*750,92),FVector(Side*1950,0,92),FVector(Side*1250,0,92),FVector(Side*450,0,92),FVector(Side*450,-2100,92),FVector(0,-2180,92),FVector(0,-2600,92),FVector(Side*(250+I*55),-2750,92)};
  }
  const FName Progress(*FString::Printf(TEXT("route_%s"),*Id.ToString()));Entry.Next=FMath::Clamp(Director->State().Values.FindRef(Progress),0,Entry.Route.Num());Entry.Complete=Entry.Next>=Entry.Route.Num();Entry.Delay=I*1.2f;
  const FVector Start=Entry.Route[FMath::Clamp(Entry.Next-1,0,Entry.Route.Num()-1)];FActorSpawnParameters P;P.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
  auto* N=GetWorld()->SpawnActor<ALWCampaignPerson76>(Director->At(Start),FRotator(0,-90,0),P);if(!N)continue;
  N->Campaign=Director;N->Person=Id;N->ConfigureResident(Id,TEXT("campaign76"),Names[I],I,I%2);N->ChatterTime=99999;N->Health=Director->State().Health.Contains(Id)?Director->State().Health[Id]:120;N->MaximumHealth=120;N->LegendaryInitialized=true;N->GetCharacterMovement()->MaxWalkSpeed=I==0?85:125;
  if(N->Gun)N->Gun->SetVisibility(false);Entry.Person=N;Director->Actors.Add(N);Director->People.Add(Id,N);Crowd.Add(MoveTemp(Entry));
 }
}
bool ULWCampaignProduction77::CrowdReady()const{for(const auto& E:Crowd)if(IsValid(E.Person)&&!E.Person->bDead&&!E.Complete)return false;return true;}
void ULWCampaignProduction77::UpdateCrowd(float Dt){
 if(Director->SceneOpen||Director->Player->IsUIOpen())return;
 const FName Stage=Director->State().Stage;const bool Open=Stage==TEXT("final_evacuation")||Director->State().Values.FindRef(Stage==TEXT("resistance_exit")?TEXT("labor_exit_open"):TEXT("family_corridor"));
 if(Stage!=TEXT("final_evacuation")&&Stage!=TEXT("resistance_exit")&&Stage!=TEXT("meridian_inside"))return;
 if(!Open)return;
 for(int I=0;I<Crowd.Num();I++){
  auto& E=Crowd[I];auto* N=E.Person.Get();if(!IsValid(N)||N->bDead||E.Complete)continue;E.Delay-=Dt;if(E.Delay>0)continue;
  const FVector Target=Director->At(E.Route[E.Next]);const float Distance=FVector::Dist2D(N->GetActorLocation(),Target);
  if(Distance<85){E.Next++;E.Stalled=0;E.LastDistance=BIG_NUMBER;E.Complete=E.Next>=E.Route.Num();Director->State().Values.Add(FName(*FString::Printf(TEXT("route_%s"),*N->Person.ToString())),E.Next);if(E.Complete){N->MoveTime=0;N->GetCharacterMovement()->StopMovementImmediately();continue;}}
  bool Yield=false;
  // Queues form at doors rather than ten capsules forcing through together.
  for(int J=0;J<I;J++)if(auto* Other=Crowd[J].Person.Get();Other&&!Other->bDead&&!Crowd[J].Complete&&FVector::Dist2D(N->GetActorLocation(),Other->GetActorLocation())<105&&FVector::DotProduct((Target-N->GetActorLocation()).GetSafeNormal2D(),(Other->GetActorLocation()-N->GetActorLocation()).GetSafeNormal2D())>.2){Yield=true;break;}
  if(Yield){N->MoveTime=0;N->GetCharacterMovement()->StopMovementImmediately();continue;}
  N->Mark=Director->At(E.Route[E.Next]);N->MoveTime=2;
  E.Stalled=Distance>=E.LastDistance-1?E.Stalled+Dt:0;E.LastDistance=Distance;
  if(E.Stalled>2){N->ResetCompanionNavigation();E.Stalled=0;}
 }
 if(CrowdReady()&&!Director->State().Values.FindRef(TEXT("crowd_clear77"))){Director->Effects({TEXT("crowd_clear77")});Director->Refresh();Director->Player->Notify(TEXT("The surviving group has reached the receiving point."),5);}
}
