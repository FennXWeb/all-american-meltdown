#include "LWSiteIdentity.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWZombie.h"
#include "LWResident.h"
#include "LWVehicle.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
bool ALWCharacter::DiscoverPlace(uint32 Id,FVector Entrance,const FString& Name){
 if(RPG.KnownPlaces.Contains(int64(Id))){RPG.PlaceNames.Add(int64(Id),Name);return false;}RPG.KnownPlaces.Add(int64(Id),Entrance);RPG.PlaceNames.Add(int64(Id),Name);FName Legacy(*FString::Printf(TEXT("site_%u"),Id));if(RPG.Discoveries.Contains(Legacy))return false;RPG.Discoveries.Add(Legacy);GainXP(35);DiscoveryTitle=Name;int32 AddressStart;if(DiscoveryTitle.FindChar(TCHAR('/'),AddressStart))DiscoveryTitle=DiscoveryTitle.Left(AddressStart).TrimEnd();DiscoveryAlert=5;World->Sound(TEXT("LevelUp"),GetActorLocation(),.35f);PersistWorldChange();return true;
}
void ALWCharacter::TickDiscovery(float Dt){
 DiscoveryAlert=FMath::Max(0.f,DiscoveryAlert-Dt);DiscoveryClock-=Dt;if(DiscoveryClock>0||bSafehouse)return;DiscoveryClock=.75f;
 for(TActorIterator<ALWWorldObject> O(GetWorld());O;++O)if(O->Kind==ELWObjectKind::Trader&&!RPG.Discoveries.Contains(O->RecordId)&&FVector::DistSquared(GetActorLocation(),O->GetActorLocation())<FMath::Square(900.f)){FHitResult H;FCollisionQueryParams Q(NAME_None,false,this);if(!GetWorld()->LineTraceSingleByChannel(H,GetActorLocation()+FVector(0,0,50),O->GetActorLocation()+FVector(0,0,80),ECC_Visibility,Q)||H.GetActor()==*O){RPG.Discoveries.Add(O->RecordId);PersistWorldChange();}}
 const FIntPoint Region(LWGen::FloorDiv(GetActorLocation().X+LWGen::RegionSize*.5,LWGen::RegionSize),LWGen::FloorDiv(GetActorLocation().Y+LWGen::RegionSize*.5,LWGen::RegionSize));
 if(LWGen::HasTown(Region,World->Seed)){FVector2D Hub=LWGen::Hub(Region,World->Seed)+FVector2D(0,-5000);if(FVector2D::Distance(FVector2D(GetActorLocation()),Hub)<2200)DiscoverPlace(LWGen::Hash(Region.X,Region.Y,World->Seed,26001),FVector(Hub+FVector2D(0,1600),0),EnsureSettlement(Region).Name);}
 TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Gather(FVector2D(GetActorLocation()),World->Seed,Roads,Sites);
 for(const auto& S:Sites){if(S.SettlementBuilding)continue;if(RPG.KnownPlaces.Contains(int64(S.Id))){RPG.PlaceNames.Add(int64(S.Id),LWSites::Name(S));continue;}const FVector2D Q=(FVector2D(GetActorLocation())-S.Position).GetRotated(-S.Yaw);
 if(FMath::Abs(Q.X)<S.Size.X*.5+250&&FMath::Abs(Q.Y)<S.Size.Y*.5+250&&FMath::Abs(GetActorLocation().Z)<1500){DiscoverPlace(S.Id,FVector(LWGen::Entrance(S),0),LWSites::Name(S));break;}}
}
bool ALWCharacter::FastTravel(){
 if(bStoryLocked||(RPG.Story.Enabled&&RPG.Story.Stage>=20&&RPG.Story.Stage<=26)){Notify(TEXT("ESCAPE FORT RESOLUTE BEFORE FAST TRAVELING"));return false;}
 if(!bMap||!bStarted||Health<=0||Vehicle||GetWorld()->GetTimeSeconds()-LastCombatTime<25){Notify(TEXT("CANNOT TRAVEL NOW"));return false;}
 if(!bWaypoint){Notify(TEXT("MARK A DISCOVERED LOCATION FIRST"));return false;}
 FVector Goal;bool Found=false;double Best=600;
 const FVector2D Bunker(ALWWorld::BunkerDoorPosition());if(FVector2D::Distance(Waypoint,Bunker)<600){Goal=ALWWorld::BunkerDoorPosition()+FVector(0,350,0);Found=true;Best=FVector2D::Distance(Waypoint,Bunker);}
 for(const auto& E:RPG.KnownPlaces){double D=FVector2D::Distance(Waypoint,FVector2D(E.Value));if(D<Best){Best=D;Goal=E.Value;Found=true;}}
 if(!Found){Notify(TEXT("FAST TRAVEL REQUIRES A DISCOVERED LOCATION"));return false;}
 auto Threat=[&](FVector At){for(TActorIterator<ALWZombie> Z(GetWorld());Z;++Z)if(!Z->bDead&&!Cast<ALWResident>(*Z)&&FVector::DistSquared(Z->GetActorLocation(),At)<FMath::Square(Z->Alert>0?6500.f:1800.f))return true;for(TActorIterator<ALWResident> N(GetWorld());N;++N)if(N->IsTownHostile()&&FVector::DistSquared(N->GetActorLocation(),At)<FMath::Square(1800.f))return true;return false;};
 if(!bSafehouse&&Threat(GetActorLocation())){Notify(TEXT("ENEMIES NEARBY"));return false;}
 TArray<TWeakObjectPtr<ALWResident>> Followers;for(TActorIterator<ALWResident> N(GetWorld());N;++N)if(RPG.Crew.ContainsByPredicate([&](const auto& C){return C.Following&&C.Id==N->ResidentId;}))Followers.Add(*N);
 const FVector Old=GetActorLocation();World->Stream(Goal,true);FVector Arrival;bool Clear=false;FCollisionQueryParams Q(NAME_None,false,this);
 for(int I=0;I<32&&!Clear;I++){float Angle=I*2.39996f;FVector Try=Goal+FVector(FMath::Cos(Angle),FMath::Sin(Angle),0)*(100+I*35);FHitResult H;
 if(GetWorld()->LineTraceSingleByChannel(H,Try+FVector(0,0,1600),Try-FVector(0,0,1000),ECC_Visibility,Q)&&H.ImpactNormal.Z>.75f){Try=H.ImpactPoint+FVector(0,0,GetCapsuleComponent()->GetScaledCapsuleHalfHeight()+5);if(!GetWorld()->OverlapBlockingTestByChannel(Try,FQuat::Identity,ECC_Pawn,FCollisionShape::MakeCapsule(34,90),Q)&&!Threat(Try)){Arrival=Try;Clear=true;}}}
 if(!Clear){World->Stream(Old,true);Notify(TEXT("DESTINATION IS NOT SAFE"));return false;}
 const float Hours=FMath::Clamp(FVector::Dist2D(Old,Arrival)/400000.f,.25f,8.f);StandFromChair(true);ClosePanels();CancelReload();GetCharacterMovement()->StopMovementImmediately();SetActorLocation(Arrival,false,nullptr,ETeleportType::TeleportPhysics);bSafehouse=false;
 for(int I=0;I<Followers.Num();I++)if(auto* N=Followers[I].Get()){FVector At=Arrival+FVector(-160-I*70,150,0);FHitResult H;if(GetWorld()->LineTraceSingleByChannel(H,At+FVector(0,0,500),At-FVector(0,0,500),ECC_WorldStatic,Q)){At.Z=H.ImpactPoint.Z+100;if(!GetWorld()->OverlapBlockingTestByChannel(At,FQuat::Identity,ECC_Pawn,FCollisionShape::MakeCapsule(32,88),Q)){N->SetActorLocation(At,false,nullptr,ETeleportType::TeleportPhysics);N->Home=At;N->Path.Empty();}}}
 World->TickWeather(Hours*World->DayLengthMinutes*60/24,this);Hunger=FMath::Max(5.f,Hunger-Hours*2);Thirst=FMath::Max(5.f,Thirst-Hours*3);ClearWaypoint();Notify(TEXT("TRAVEL COMPLETE"));RequestSave40();return true;
}
