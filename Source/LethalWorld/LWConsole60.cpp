#include "LWGeography84.h"
#include "LWConsole47.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWZombie.h"
#include "LWResident.h"
#include "LWVehicle.h"
#include "LWVehicleSpec.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Containers/Ticker.h"
namespace {
FString Clean60(FString S){return S.ToLower().Replace(TEXT("_"),TEXT("")).Replace(TEXT("-"),TEXT("")).Replace(TEXT(" "),TEXT("")).Replace(TEXT("\""),TEXT(""));}
FString POIList60(){FString S;for(int I=0;I<LWPlaces::Count;I++)S+=FString::Printf(TEXT("%d: %s (%s)\n"),I,LWPlaces::Name(I),LWPlaces::Event(I));return S;}
}
const TArray<LWCheats47::FNPC60>& LWCheats47::NPCs60(){
 static const TArray<FNPC60> Types={
  {TEXT("zombie"),TEXT("Zombie"),0,TEXT("")},{TEXT("raider"),TEXT("Raider"),1,TEXT("")},{TEXT("dog"),TEXT("Rabid dog"),2,TEXT("")},{TEXT("mannequin"),TEXT("Mannequin"),3,TEXT("")},
  {TEXT("moose"),TEXT("Mutated moose"),4,TEXT("")},{TEXT("titan"),TEXT("Titan"),5,TEXT("")},{TEXT("deathclaw"),TEXT("Deathclaw"),6,TEXT("")},{TEXT("scorpion"),TEXT("Giant scorpion"),7,TEXT("")},
  {TEXT("karen"),TEXT("Zombie Karen"),8,TEXT("")},{TEXT("behemoth"),TEXT("Behemoth"),9,TEXT("")},{TEXT("colossus"),TEXT("Colossus"),10,TEXT("")},{TEXT("worldeater"),TEXT("World Eater"),11,TEXT("")},
  {TEXT("hornet"),TEXT("Giant hornet"),12,TEXT("")},{TEXT("bear"),TEXT("Bear"),13,TEXT("")},{TEXT("rogueai"),TEXT("Rogue AI"),14,TEXT("")},
  {TEXT("survivor"),TEXT("Survivor"),-1,TEXT("civilian")},{TEXT("merchant"),TEXT("Merchant"),-1,TEXT("merchant")},{TEXT("medic"),TEXT("Medic"),-1,TEXT("medic")},{TEXT("companion"),TEXT("Companion candidate"),-1,TEXT("recruit")}
 };return Types;
}
FString ULWConsole47::WorldCommand60(const TArray<FString>& A){
 const FString Cmd=A[0].ToLower();
 if(Cmd==TEXT("npctypes")){FString S;for(const auto& N:LWCheats47::NPCs60())S+=FString(N.Id)+TEXT(" - ")+N.Name+TEXT("\n");return S;}
 if(Cmd==TEXT("vehicles")){FString S;for(const auto& V:LWTraffic::Specs())S+=FString(V.Id)+TEXT(" - ")+V.Name+TEXT("\n");return S;}
 if(Cmd==TEXT("pois"))return POIList60();
 auto* P=Player47();if(!P||!P->World||!P->bStarted||P->OpeningMode||P->bWorldSetup||P->Health<=0)return TEXT("Start or load a living character before using world cheats.");
 if(A.Num()<2)return TEXT("Usage: spawnnpc <type> [1-20], spawnvehicle <type>, locate <POI type or ID> [radius_km 1-50]. Use npctypes, vehicles, pois.");
 const FString Type=Clean60(A[1]);auto* W=P->World.Get();
 if(Cmd==TEXT("locate")){
  int64 Km=50;if(A.Num()>3||(A.Num()==3&&!LWCheats47::PositiveAmount(A[2],50,Km)))return TEXT("Usage: locate <type_or_id> [radius_km 1-50]");
  TSet<int> Types;int Numeric=-1;if(Type.IsNumeric()&&LexTryParseString(Numeric,*Type)&&Numeric>=0&&Numeric<LWPlaces::Count)Types.Add(Numeric);
  else for(int I=0;I<LWPlaces::Count;I++)if(Clean60(LWPlaces::Event(I))==Type||Clean60(LWPlaces::Name(I))==Type)Types.Add(I);
  if(Type==TEXT("shoppingmall"))Types.Add(58);if(Type==TEXT("stripmall")||Type==TEXT("plaza"))Types.Add(11);if(Type==TEXT("militarybase"))Types.Add(64);if(Type==TEXT("gunrange"))Types.Add(66);if(Type==TEXT("sportinggoods"))Types.Add(67);if(Type==TEXT("policestation"))Types.Add(17);if(Type==TEXT("clothingstore"))Types.Add(18);
  if(Type==TEXT("highmark"))Types.Add(68);if(Type==TEXT("destiny"))Types.Add(69);
  if(Types.IsEmpty())return TEXT("Unknown POI type. Use pois for names and numeric IDs.");
  if(!LWGeography84::Canada(FVector2D(P->GetActorLocation()))){
   ++LocateSerial60;const LWGen::FSite* Best=nullptr;double Distance=Km*100000.;for(const auto& Site:LWNY69::Sites())if(Types.Contains(Site.Type)){double D=FVector2D::Distance(FVector2D(P->GetActorLocation()),Site.Position);if(D<Distance){Distance=D;Best=&Site;}}
   if(!Best)return TEXT("No matching location in this search radius.");P->SetWaypoint(LWGen::Entrance(*Best));const FString Result=FString::Printf(TEXT("%s / %.2f km. Waypoint set."),*Best->PlaceName69,Distance/100000.);ReportLocate67(LocateSerial60,Result);return Result;
  }
  struct FSearch {TArray<FIntPoint> Cells;int Index=0,Serial=0,Seed=0;double Best=0;FVector2D Origin;LWGen::FSite Site;bool Found=false;};
  auto S=MakeShared<FSearch>();S->Serial=++LocateSerial60;S->Seed=W->Seed;S->Origin=FVector2D(P->GetActorLocation());S->Best=Km*100000.;
  const FIntPoint Center(LWGen::FloorDiv(S->Origin.X,LWGen::RegionSize),LWGen::FloorDiv(S->Origin.Y,LWGen::RegionSize));const int Range=FMath::CeilToInt(S->Best/LWGen::RegionSize)+2;
  for(int Y=-Range;Y<=Range;Y++)for(int X=-Range;X<=Range;X++)S->Cells.Add(Center+FIntPoint(X,Y));
  TWeakObjectPtr<ULWConsole47> Console(this);TWeakObjectPtr<ALWCharacter> Player(P);TWeakObjectPtr<ALWWorld> World(W);
  FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Console,Player,World,S,Types](float){
   if(!Console.IsValid()||!Player.IsValid()||!World.IsValid()||Console->LocateSerial60!=S->Serial||World->Seed!=S->Seed)return false;
   // One region per frame; never spawn actors or synchronously stream distant chunks.
   TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Region(S->Cells[S->Index++],S->Seed,Roads,Sites);
   for(const auto& Site:Sites)if(Types.Contains(Site.Type)){double D=FVector2D::Distance(S->Origin,Site.Position);if(D<S->Best){S->Best=D;S->Site=Site;S->Found=true;}}
   if(S->Index<S->Cells.Num())return true;
   if(S->Found){Player->SetWaypoint(LWGen::Entrance(S->Site));Console->ReportLocate67(S->Serial,FString::Printf(TEXT("Nearest match in search radius: %s, %.2f km. X=%.0f Y=%.0f. Waypoint set."),LWPlaces::Name(S->Site.Type),S->Best/100000.,S->Site.Position.X,S->Site.Position.Y));}
   else Console->ReportLocate67(S->Serial,TEXT("No matching POI in this radius. Increase the search radius and try again."));return false;
  }));return TEXT("Searching generated world locations; results will appear here. A new locate command cancels the previous search.");
 }
 if(P->bSafehouse)return TEXT("Leave the bunker before spawning NPCs or vehicles.");
 if(Cmd==TEXT("spawnvehicle")){
  if(A.Num()!=2)return TEXT("Usage: spawnvehicle <vehicle_id>");const auto* Spec=LWTraffic::Specs().FindByPredicate([&](const auto& S){return Clean60(S.Id)==Type;});if(!Spec)return TEXT("Unknown vehicle. Use vehicles.");
  const FRotator Rot(0,P->GetActorRotation().Yaw,0);FVector At=P->GetActorLocation()+Rot.Vector()*(Spec->HalfLength+450);At.Z=W->HeightAt(FVector2D(At))+75;
  FCollisionQueryParams Q(NAME_None,false,P);if(GetWorld()->OverlapBlockingTestByChannel(At+FVector(0,0,Spec->Height*.5f),Rot.Quaternion(),ECC_WorldDynamic,FCollisionShape::MakeBox(FVector(Spec->HalfLength+25,Spec->HalfWidth+25,Spec->Height*.5f)),Q))return TEXT("Not enough clear space in front of you for this vehicle.");
  const FName Id(*(TEXT("cheat60_")+FGuid::NewGuid().ToString(EGuidFormats::Digits)));FLWVehicleRecord R;R.Model=Spec->Id;R.VIN=FGuid::NewGuid();R.Position=At;R.Rotation=Rot;R.Unlocked=R.Hotwired=true;W->Vehicles.Add(Id,R);
  auto* Car=W->SpawnObject(ELWObjectKind::Car,Id,At,Rot);if(!Car){W->Vehicles.Remove(Id);return TEXT("Vehicle could not be spawned.");}P->RequestSave40();return FString(TEXT("Spawned "))+Spec->Name+TEXT(" (unlocked and hotwired).");
 }
 if(Cmd==TEXT("spawnnpc")){
  const auto* NPC=LWCheats47::NPCs60().FindByPredicate([&](const auto& N){return Type==N.Id;});
  if(!NPC)return TEXT("Unknown NPC. Use npctypes.");const int Kind=NPC->Kind;const bool Friendly=Kind<0;int64 Count=1;if(A.Num()>3||(A.Num()==3&&!LWCheats47::PositiveAmount(A[2],20,Count)))return TEXT("Usage: spawnnpc <type> [1-20]");
  int Spawned=0;for(int I=0;I<Count;I++){
   float Radius=Kind==10?650:Kind==9?300:Kind==11?800:60;FVector At=P->GetActorLocation()+P->GetActorForwardVector()*(Radius+450)+P->GetActorRightVector()*(I-(Count-1)*.5f)*(Radius*2+80);At.Z=W->HeightAt(FVector2D(At))+Radius+100;
   FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
   auto* N=Friendly?GetWorld()->SpawnActor<ALWResident>(At,FRotator::ZeroRotator,Params):GetWorld()->SpawnActor<ALWZombie>(At,FRotator::ZeroRotator,Params);if(!N)continue;
   N->PersistentId=GetTypeHash(FGuid::NewGuid());if(auto* R=Cast<ALWResident>(N)){const FName Id(*FString::Printf(TEXT("cheat_npc60_%u"),N->PersistentId));R->ConfigureResident(Id,FName(NPC->Role),FString::Printf(TEXT("Wanderer %u"),N->PersistentId%100000),N->PersistentId%3);}else N->ConfigureKind(ELWEnemyKind(Kind));
   At.Z=W->HeightAt(FVector2D(At))+N->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()+5;FRotator R=FRotator::ZeroRotator;if(!GetWorld()->FindTeleportSpot(N,At,R)){N->Destroy();continue;}N->SetActorLocation(At,false,nullptr,ETeleportType::TeleportPhysics);++Spawned;
  }return FString::Printf(TEXT("Spawned %d / %lld %s. NPC cheat spawns last for this session."),Spawned,Count,*A[1]);
 }return TEXT("Unknown world command.");
}
