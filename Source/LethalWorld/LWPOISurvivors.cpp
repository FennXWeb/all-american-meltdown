#include "LWStreaming68.h"
#include "LWPOISurvivors.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWResident.h"
#include "LWWorldTextComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
FString LWPOISurvivors::Name(uint32 SiteId,int Role){
 // SettlerName requires a real settlement and mutates its roster. Roadside survivors
 // instead use a pure site/role identity, independent of encounter/load order.
 static const TCHAR* First[]={TEXT("Mara"),TEXT("Elias"),TEXT("June"),TEXT("Otis"),TEXT("Nadia"),TEXT("Ruth"),TEXT("Tobias"),TEXT("Clara"),TEXT("Jasper"),TEXT("Sofia"),TEXT("Caleb"),TEXT("Wren"),TEXT("Hector"),TEXT("Iris"),TEXT("Dante"),TEXT("Mae"),TEXT("Nolan"),TEXT("Vera"),TEXT("Simon"),TEXT("Alma"),TEXT("Felix"),TEXT("Ada"),TEXT("Owen"),TEXT("Zara"),TEXT("Emmett"),TEXT("Tessa"),TEXT("Malik"),TEXT("Nell"),TEXT("Silas"),TEXT("Rosa"),TEXT("Victor"),TEXT("Lena")};
 static const TCHAR* Last[]={TEXT("Mercer"),TEXT("Holloway"),TEXT("Reyes"),TEXT("Bennett"),TEXT("Chen"),TEXT("Alvarez"),TEXT("Ward"),TEXT("Okafor"),TEXT("Sinclair"),TEXT("Patel"),TEXT("Brooks"),TEXT("Fletcher"),TEXT("Quinn"),TEXT("Navarro"),TEXT("Carter"),TEXT("Voss"),TEXT("Morrow"),TEXT("Kim"),TEXT("Sawyer"),TEXT("Hayes"),TEXT("Dalton"),TEXT("Flores"),TEXT("Turner"),TEXT("Reed"),TEXT("Lawson"),TEXT("Santos"),TEXT("Hale"),TEXT("Price"),TEXT("Dawson"),TEXT("Ellis"),TEXT("Sutton"),TEXT("Grant")};
 const uint32 H=LWGen::Hash(int32(SiteId),0,30021);const uint32 Detail=LWGen::Hash(int32(SiteId),Role,30022);
 // Distinct first names for all three roles at the same stop.
 return FString::Printf(TEXT("%s %c. %c. %s"),First[((H&31)+Role*11)%32],TCHAR('A'+Detail%26),TCHAR('A'+(Detail/26)%26),Last[(Detail>>12)&31]);
}
void LWPOISurvivors::Spawn(ALWChunk* C,ALWWorld* W,const LWGen::FSite& S,int Role68){
 if(C->Plan68&&Role68<0){for(int I=0;I<3;I++)C->Plan68->Population.Add([C,W,S,I](){LWPOISurvivors::Spawn(C,W,S,I);});return;}
 auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(W,0));const FRotator R(0,S.Yaw,0);
 const bool Expanded=LWPlaces::Expanded(S.Type);const float Y=-S.Size.Y*.5f+(Expanded?700.f:-180.f);
 auto At=[&](FVector V){return FVector(S.Position,12)+R.RotateVector(V);};
 // Publish pending collision before asking physics for a supporting floor.
 C->FlushSurfaces();
 const TCHAR* Roles[]={TEXT("merchant"),TEXT("recruit"),TEXT("warden")};
 // An existing authored NPC cast takes precedence over this optional shared stop.
 for(auto A:C->Residents)if(auto* N=Cast<ALWResident>(A)){
  if(N->ResidentId.ToString().StartsWith(FString::Printf(TEXT("poi30_%u_resident_"),S.Id)))continue;
  const FVector Local=R.UnrotateVector(N->GetActorLocation()-FVector(S.Position,12));
  if(FMath::Abs(Local.X)<S.Size.X*.5&&FMath::Abs(Local.Y)<S.Size.Y*.5){UE_LOG(LogTemp,Display,TEXT("POI30_SURVIVOR_SKIP site=%u authored=%s"),S.Id,*N->ResidentId.ToString());return;}
 }
 for(int I=Role68<0?0:Role68;I<(Role68<0?3:Role68+1);I++){
  FName Id(*FString::Printf(TEXT("poi30_%u_resident_%d"),S.Id,I));
  if(P&&P->RPG.Crew.ContainsByPredicate([&](const auto& Crew){return Crew.Id==Id;})){UE_LOG(LogTemp,Display,TEXT("POI30_SURVIVOR_SKIP site=%u role=%d hired"),S.Id,I);continue;}
  bool Exists=false;for(TActorIterator<ALWResident> N(W->GetWorld());N;++N)if(N->ResidentId==Id){Exists=true;break;}if(Exists){UE_LOG(LogTemp,Display,TEXT("POI30_SURVIVOR_SKIP site=%u role=%d already_present"),S.Id,I);continue;}
  // Try nearby apron positions deterministically; never force a pawn into geometry.
  FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
  ALWResident* N=nullptr;FVector Candidate;
  for(int Attempt=0;Attempt<24&&!N;Attempt++){
   const float Along=Attempt==0?0.f:((Attempt%6)-2)*90.f;
   Candidate=At(FVector(-320+I*320+(Attempt/6)*110,Y+Along,180));
   FHitResult Floor;FCollisionQueryParams Query;
   if(!W->GetWorld()->LineTraceSingleByChannel(Floor,Candidate,Candidate-FVector(0,0,350),ECC_WorldStatic,Query)||Floor.ImpactNormal.Z<.85f||Floor.ImpactPoint.Z>140||Floor.ImpactPoint.Z<-32)continue;
   Candidate.Z=Floor.ImpactPoint.Z+94; // Resident capsule half-height 88 + 6cm clearance.
   if(W->GetWorld()->OverlapBlockingTestByChannel(Candidate,FQuat::Identity,ECC_Pawn,FCollisionShape::MakeCapsule(29,88),Query))continue;
   N=W->GetWorld()->SpawnActor<ALWResident>(Candidate,R,Params);
  }
  if(N){N->ConfigureResident(Id,Roles[I],Name(S.Id,I),I);N->ActivitySpots={N->Home,N->Home};C->Residents.Add(N);UE_LOG(LogTemp,Display,TEXT("POI30_SURVIVOR_SPAWN site=%u role=%d position=%s"),S.Id,I,*N->GetActorLocation().ToString());}
  else UE_LOG(LogTemp,Warning,TEXT("POI30_SURVIVOR_BLOCKED site=%u type=%d role=%d final_position=%s"),S.Id,S.Type,I,*Candidate.ToString());
 }
 if(Role68>=0&&Role68!=2)return;
 auto* Sign=NewObject<ULWWorldTextComponent>(C);Sign->SetupAttachment(C->GetRootComponent());Sign->SetRelativeLocation(At(FVector(0,Y+(Expanded?400:-90),290))-C->GetActorLocation());Sign->SetRelativeRotation(R+FRotator(0,-90,0));Sign->SetText(FText::FromString(TEXT("SURVIVOR STOP / TRADE / CREW / CONTRACTS")));Sign->SetWorldSize(27);Sign->SetHorizontalAlignment(EHTA_Center);Sign->RegisterComponent();
}
