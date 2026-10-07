#include "LWGeography84.h"
#include "LWEncounter.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWZombie.h"
#include "LWResident.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Misc/CommandLine.h"
#include "Misc/Crc.h"

void ALWWorld::SnapshotEncounters(){for(auto& E:LiveEncounters)if(IsValid(E.Value))E.Value->Snapshot();}
void ALWWorld::ClearEncounterActors(){for(auto& E:LiveEncounters)if(IsValid(E.Value))E.Value->Destroy();LiveEncounters.Empty();}
bool ALWWorld::EncounterLocation(FVector At,float Radius)const{
 auto Reject=[&](int Why){static int Count=0;if(Count++<80&&FParse::Param(FCommandLine::Get(),TEXT("LWUpdate53Smoke")))UE_LOG(LogTemp,Display,TEXT("ENCOUNTER53_REJECT reason=%d xyz=%s"),Why,*At.ToString());return false;};
 if(LWGeography84::Canada(FVector2D(At))||LWStory::Reserved(FVector2D(At),Radius)||FVector::Dist2D(At,BunkerDoorPosition())<6000||!Chunks.Contains(LWGen::ChunkAt(FVector2D(At))))return Reject(1);
 TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Gather(FVector2D(At),Seed,Roads,Sites);
 for(const auto& S:Sites){const FVector2D L=(FVector2D(At)-S.Position).GetRotated(-S.Yaw);const FVector2D Outside(FMath::Max(0.,FMath::Abs(L.X)-S.Size.X*.5),FMath::Max(0.,FMath::Abs(L.Y)-S.Size.Y*.5));if(Outside.Size()<Radius+200)return Reject(2);}
 for(const auto& R:Roads)if(LWGen::DistanceToSegment(FVector2D(At),R)<R.Width*.5+Radius)return Reject(3);
 for(FVector2D O:{FVector2D(Radius,0),FVector2D(-Radius,0),FVector2D(0,Radius),FVector2D(0,-Radius)})if(FMath::Abs(HeightAt(FVector2D(At)+O)-At.Z)>100)return Reject(4);
 for(const auto& E:Encounters.Records)if(E.Value.Stage<2&&FVector::Dist2D(E.Value.Position,At)<4500)return Reject(5);
 FCollisionQueryParams Q(NAME_None,false,this);if(GetWorld()->OverlapBlockingTestByChannel(At+FVector(0,0,200),FQuat::Identity,ECC_Pawn,FCollisionShape::MakeBox(FVector(Radius,Radius,160)),Q))return Reject(6);return true;
}
ALWEncounterScene* ALWWorld::SpawnEncounter(FName Type,FVector At,float Yaw){
 if(LWGeography84::Canada(FVector2D(At)))return nullptr;const auto* D=ULWEncounterCatalog::Get()->Find(Type);if(!D||!D->Enabled)return nullptr;
 FLWEncounterRecord R;R.Id=FName(*FString::Printf(TEXT("enc_%d_%d"),Seed,++Encounters.Serial));R.Type=Type;R.Position=At;R.Yaw=Yaw;R.Created=Encounters.Elapsed;R.Expires=R.Created+FMath::Max(120.f,D->Lifetime);R.EscortPosition=At+FRotator(0,Yaw,0).RotateVector(FVector(120,170,100));R.Goal=At+FRotator(0,Yaw,0).RotateVector(FVector(2600,0,0));R.Goal.Z=HeightAt(FVector2D(R.Goal))+100;
 Encounters.Records.Add(R.Id,R);Encounters.LastType.Add(Type,Encounters.Elapsed);Encounters.LastPosition=At;Encounters.Recent.Add(Type);while(Encounters.Recent.Num()>6)Encounters.Recent.RemoveAt(0);
 auto* A=GetWorld()->SpawnActor<ALWEncounterScene>(At,FRotator(0,Yaw,0));if(A){LiveEncounters.Add(R.Id,A);A->Initialize(this,R.Id);}return A;
}
void ALWWorld::TickEncounters(float Dt,ALWCharacter* P){
 if(!P||!P->bStarted||P->bMenu||P->Health<=0)return;Encounters.Elapsed+=FMath::Min(Dt,1.f);EncounterClock-=Dt;if(EncounterClock>0)return;EncounterClock=1;
 TArray<FName> Remove;
 for(auto& E:LiveEncounters){if(!IsValid(E.Value)){Remove.Add(E.Key);continue;}const auto* R=E.Value->Record();if(!R){E.Value->Destroy();Remove.Add(E.Key);continue;}
 float Distance=FMath::Min(FVector::Dist2D(P->GetActorLocation(),R->Position),FVector::Dist2D(P->GetActorLocation(),R->EscortPosition));if(Distance>16000||(R->Stage>=2&&E.Value->LivingEnemies()==0&&Distance>4500)){E.Value->Destroy();Remove.Add(E.Key);}}
 for(FName Id:Remove)LiveEncounters.Remove(Id);
 int Active=0,Threats=0;for(auto& E:Encounters.Records){auto& R=E.Value;const auto* D=ULWEncounterCatalog::Get()->Find(R.Type);if(!D)continue;
 if((R.Stage==0&&FVector::Dist2D(P->GetActorLocation(),R.Position)>22000)||(R.Stage<2&&Encounters.Elapsed>R.Expires)){R.Stage=3;R.Result=TEXT("The opportunity passed.");}
 if(R.Stage<2){Active++;Threats+=LWEncounters::Hostile(*D);}
 bool Nearby=FVector::Dist2D(P->GetActorLocation(),R.Position)<(R.Stage<2?12000:4000);
 if(Nearby&&!LiveEncounters.Contains(E.Key)&&(R.Stage<2||(R.Stage==2&&Containers.Contains(R.Id)&&!Containers[R.Id].Items.IsEmpty()))){auto* A=GetWorld()->SpawnActor<ALWEncounterScene>(R.Position,FRotator(0,R.Yaw,0));if(A){LiveEncounters.Add(E.Key,A);A->Initialize(this,E.Key);}}
 }
 // Bound history without forgetting anti-repeat clocks or storyline unlocks.
 if(Encounters.Records.Num()>256){TArray<FName> Old;for(const auto& E:Encounters.Records)if(E.Value.Stage>=2&&!LiveEncounters.Contains(E.Key))Old.Add(E.Key);Old.Sort([&](FName A,FName B){return Encounters.Records[A].Created<Encounters.Records[B].Created;});for(FName Id:Old){if(Encounters.Records.Num()<=256)break;Containers.Remove(Id);for(int I=0;I<12;I++)KilledZombies.Remove(FCrc::StrCrc32(*(Id.ToString()+FString::FromInt(I))));Encounters.Records.Remove(Id);}}
 if(Encounters.Serial==0)Encounters.NextAttempt=FMath::Min(Encounters.NextAttempt,Encounters.Elapsed+35.0);
 if(!EnableEncounters||Encounters.Elapsed<Encounters.NextAttempt)return;
 Encounters.NextAttempt=Encounters.Elapsed+8;
 if(P->bSafehouse||(P->bIndoors&&!P->Vehicle)||P->RPGPanel||P->bInventory||Active>=FMath::Clamp(ULWEncounterCatalog::Get()->MaxActive,1,3)||ZombieCount>115)return;
 bool Combat=false;for(TActorIterator<ALWZombie> I(GetWorld());I;++I)if(!Cast<ALWResident>(*I)&&!I->bDead&&I->Alert>0&&FVector::DistSquared(I->GetActorLocation(),P->GetActorLocation())<FMath::Square(3800.f)){Combat=true;break;}if(Combat)return;
 if(Encounters.Serial>0&&FVector::Dist2D(P->GetActorLocation(),Encounters.LastPosition)<1600)return;
 const auto* D=LWEncounters::Select(*ULWEncounterCatalog::Get(),Encounters,Seed,P->RPG.Level,TimeOfDay,RainAmount,Threats==0&&P->Health>=45);if(!D)return;
 TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Gather(FVector2D(P->GetActorLocation()),Seed,Roads,Sites);Roads.RemoveAll([&](const auto& Road){return Road.Width<500||LWGen::DistanceToSegment(FVector2D(P->GetActorLocation()),Road)>(P->Vehicle?22000:18000);});if(Roads.IsEmpty())return;
 FRandomStream Rand(LWGen::Hash(Encounters.Serial,Encounters.Elapsed/20,Seed,8251));
 int PassedDistance=0,PassedType=0,PassedView=0;
 for(int Try=0;Try<128;Try++){const auto& Road=Roads[Rand.RandRange(0,Roads.Num()-1)];if(Road.Width<500)continue;FVector2D Dir=(Road.B-Road.A).GetSafeNormal(),N(-Dir.Y,Dir.X);FVector2D Closest;LWGen::DistanceToSegment(FVector2D(P->GetActorLocation()),Road,&Closest);const float Len=(Road.B-Road.A).Size();const float T=FMath::Clamp(float(FVector2D::DotProduct(Closest-Road.A,Dir))+Rand.FRandRange(-8500,8500),0.f,Len);FVector2D XY=Road.A+Dir*T+N*(Road.Width*.5+Rand.FRandRange(1100,3200))*(Rand.RandRange(0,1)?1:-1);if(Try>=96&&(D->Setting==ELWEncounterSetting::Camp||D->Setting==ELWEncounterSetting::Grave||D->Setting==ELWEncounterSetting::Supply))XY=FVector2D(P->GetActorLocation())+FVector2D(Rand.FRandRange(8000,15000),0).GetRotated(Rand.FRandRange(0,360));FVector At(XY,HeightAt(XY));float Dist=FVector::Dist2D(At,P->GetActorLocation());if(Dist<(P->Vehicle?5000:2800)||Dist>(P->Vehicle?20000:16000))continue;PassedDistance++;
 if(D->NearPOI>=0&&!Sites.ContainsByPredicate([&](const auto& S){return S.Type==D->NearPOI&&FVector2D::Distance(S.Position,XY)<10000;}))continue;
 PassedType++;
 // Never assemble a scene in an unobstructed view of the player.
 FVector Eye=P->Camera->GetComponentLocation();if(FVector::DotProduct((At-Eye).GetSafeNormal(),P->Camera->GetForwardVector())>-.15f){FHitResult Hit;FCollisionQueryParams Q(NAME_None,false,P);if(!GetWorld()->LineTraceSingleByChannel(Hit,Eye,At+FVector(0,0,180),ECC_Visibility,Q))continue;}
 PassedView++;if(!EncounterLocation(At))continue;
 if(SpawnEncounter(D->Id,At,FMath::RadiansToDegrees(FMath::Atan2(Dir.Y,Dir.X)))){float Min=FMath::Max(45.f,ULWEncounterCatalog::Get()->MinInterval),Max=FMath::Max(Min,ULWEncounterCatalog::Get()->MaxInterval);Encounters.NextAttempt=Encounters.Elapsed+Rand.FRandRange(Min,Max)/FMath::Clamp(ULWEncounterCatalog::Get()->FrequencyMultiplier,.25f,4.f);if(D->Task==ELWEncounterTask::Aid||D->Task==ELWEncounterTask::Trade||D->Task==ELWEncounterTask::Rescue||D->Task==ELWEncounterTask::Escort||D->Task==ELWEncounterTask::Repair){const FVector Offset=At-P->GetActorLocation();const TCHAR* Direction=FMath::Abs(Offset.X)>FMath::Abs(Offset.Y)?(Offset.X>0?TEXT("north"):TEXT("south")):(Offset.Y>0?TEXT("east"):TEXT("west"));P->Notify(FString::Printf(TEXT("A voice calls out to the %s."),Direction),5);Sound(D->Cue,At,1.f);}P->RequestSave40();}break;
 }
 if(FParse::Param(FCommandLine::Get(),TEXT("LWUpdate53Smoke")))UE_LOG(LogTemp,Display,TEXT("ENCOUNTER53_ATTEMPT roads=%d distance=%d type=%d view=%d serial=%d"),Roads.Num(),PassedDistance,PassedType,PassedView,Encounters.Serial);
}
