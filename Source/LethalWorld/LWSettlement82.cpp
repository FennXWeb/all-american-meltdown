#include "LWSettlement82.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWResident.h"
#include "LWVehicle.h"
#include "LWCampaign76.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "HAL/PlatformTime.h"

ALWSettlement82::ALWSettlement82(){PrimaryActorTick.bCanEverTick=true;RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));Camera=CreateDefaultSubobject<UCameraComponent>(TEXT("ConstructionCamera"));Camera->SetupAttachment(RootComponent);Camera->FieldOfView=80;Preview=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Preview"));Preview->SetupAttachment(RootComponent);Preview->SetCollisionEnabled(ECollisionEnabled::NoCollision);Preview->SetCastShadow(false);Preview->SetVisibility(false);}
ALWSettlement82* ALWSettlement82::Ensure(ALWCharacter* P){if(!P||!P->World)return nullptr;if(!IsValid(P->Settlement82)){P->Settlement82=P->GetWorld()->SpawnActor<ALWSettlement82>();if(P->Settlement82){P->Settlement82->Player=P;P->Settlement82->World=P->World;}}return P->Settlement82;}
FLWClaim82* ALWSettlement82::State(){return Player?Player->RPG.Claims82.Find(Selected):nullptr;}
const FLWClaim82* ALWSettlement82::State()const{return Player?Player->RPG.Claims82.Find(Selected):nullptr;}
void ALWSettlement82::Reset(){Close();for(auto& A:Actors)if(IsValid(A.Value))A.Value->Destroy();Actors.Empty();for(auto& N:People)if(IsValid(N.Value))N.Value->Destroy();People.Empty();Loaded.Empty();Queue.Empty();Selected=Moving=SelectedResident=SelectedLot=NAME_None;Poll=0;}
void ALWSettlement82::EndPlay(const EEndPlayReason::Type Reason){Reset();Super::EndPlay(Reason);}
void ALWSettlement82::Open(FName Id){if(!Player||!Player->bStarted||Player->Health<=0)return;Player->ClosePanels();if(!Id.IsNone()&&Player->RPG.Claims82.Contains(Id))Selected=Id;if(!State()&&Player->RPG.Claims82.Num())Selected=Player->RPG.Claims82.CreateConstIterator().Key();Player->RPGPanel=7;Player->CancelReload();Player->ConsumeUIAttack();Player->SetMenuInput(true);Panel=0;Page=ResidentPage=0;Message.Empty();}
void ALWSettlement82::Close(){if(!IsValid(Player))return;const bool WasBuilding=Building;Building=Palette=Claiming=false;Player->SettlementBuild82=false;Preview->SetVisibility(false);Moving=Support=NAME_None;if(WasBuilding){if(auto* PC=Cast<APlayerController>(Player->Controller))PC->SetViewTarget(Player);if(Player->Health>0)Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);Player->ConsumeUIAttack();Player->SetMenuInput(Player->bMenu||Player->RPGPanel!=0);}}
bool ALWSettlement82::BeginClaim(){
 if(!Player||!Player->bStarted||Player->Health<=0||Player->Vehicle||Player->bSafehouse||Player->bStoryLocked){Message=TEXT("Go outside on foot to place a settlement flag.");if(Player)Player->Notify(Message);return false;}
 if(Player->CountSupply(TEXT("settlement_flag"))<1){Message=TEXT("Find a settlement flag or buy one from a supplies merchant.");Player->Notify(Message);return false;}
 if(Player->RPG.Claims82.Num()>=8){Message=TEXT("Eight settlements is the current limit.");Player->Notify(Message);return false;}
 Player->ClosePanels();Claiming=true;Building=true;Palette=false;Player->SettlementBuild82=true;SelectedCatalog=-1;Moving=NAME_None;Height=0;Yaw=Player->GetActorRotation().Yaw;Camera->SetWorldLocation(Player->GetActorLocation()+FVector(0,0,220));Camera->SetWorldRotation(FRotator(-25,Yaw,0));
 Roads.Empty();Sites.Empty();LWGen::Gather(FVector2D(Player->GetActorLocation()),World->Seed,Roads,Sites);Player->CancelReload();Player->ConsumeUIAttack();Player->GetCharacterMovement()->DisableMovement();Cast<APlayerController>(Player->Controller)->SetViewTarget(this);Player->SetMenuInput(false);Preview->SetStaticMesh(World->Mesh(TEXT("Cube")));Preview->SetMaterial(0,World->Material(TEXT("Glass")));Preview->SetVisibility(true);WaitClick=true;Message=TEXT("Choose open land. Your flag claims a 60-metre radius.");return true;
}
bool ALWSettlement82::ValidateClaim(FVector At,FString& Why)const{
 auto Fail=[&](const TCHAR* S){Why=S;return false;};
 if(Player->RPG.Claims82.Num()>=8)return Fail(TEXT("Settlement limit reached."));
 if(FVector::Dist2D(Player->GetActorLocation(),At)>4000)return Fail(TEXT("Place the flag within 40 metres of you."));
 if(ALWWorld::IsSafePosition(At)||LWNY69::WaterDepth(FVector2D(At))>2||LWCampaign76::Reserved(FVector2D(At))||(LWGeography84::BorderNearest(FVector2D(At))-FVector2D(At)).Size()<18000)return Fail(TEXT("This area cannot be claimed."));
 if(FMath::Abs(At.Z-World->HeightAt(FVector2D(At)))>100)return Fail(TEXT("Place the flag on natural ground."));
 for(const auto& C:Player->RPG.Claims82)if(FVector::Dist2D(C.Value.Center,At)<C.Value.Radius+6000+500)return Fail(TEXT("Too close to another settlement."));
 for(const auto& R:Roads)if(LWGen::DistanceToSegment(FVector2D(At),R)<R.Width*.5+180)return Fail(TEXT("Keep the flag clear of the road."));
 for(const auto& S:Sites){auto D=(FVector2D(At)-S.Position).GetRotated(-S.Yaw);if(FMath::Abs(D.X)<S.Size.X*.5+400&&FMath::Abs(D.Y)<S.Size.Y*.5+400)return Fail(TEXT("Existing buildings and their grounds cannot be claimed."));}
 FCollisionQueryParams Q(NAME_None,false,this);Q.AddIgnoredActor(Player);if(GetWorld()->OverlapBlockingTestByChannel(At+FVector(0,0,135),FQuat::Identity,ECC_Pawn,FCollisionShape::MakeBox(FVector(40,40,130)),Q))return Fail(TEXT("Clear the space around the flag."));
 Why=TEXT("Place settlement flag");return true;
}
bool ALWSettlement82::ClaimAt(FVector At){
 if(!ValidateClaim(At,Message)||!Player->ConsumeSupply(TEXT("settlement_flag"),1))return false;
 FLWClaim82 C;C.Id=FName(*(TEXT("settlement82_")+FGuid::NewGuid().ToString(EGuidFormats::Digits)));C.Center=At;static const TCHAR* Names[]={TEXT("Hearthstead"),TEXT("New Prospect"),TEXT("Cedar Refuge"),TEXT("Morningstar"),TEXT("Ironwood"),TEXT("Crossing Haven"),TEXT("Pioneer Rest"),TEXT("New Horizon")};C.Name=Names[Player->RPG.Claims82.Num()%8];Selected=C.Id;Player->RPG.Claims82.Add(C.Id,C);
 auto& Town=Player->RPG.Settlements.FindOrAdd(C.Id);Town.Name=C.Name;Town.Center=At;Town.Member=Town.Leader=true;Town.Reputation=100;Town.LastEconomyHour=Player->WorldHour();
 Player->RequestSave40();Player->GainXP(100);Close();Open(C.Id);Message=TEXT("Build a bed and a HAM radio to welcome your first resident.");Stream();return true;
}
void ALWSettlement82::BeginBuild(int Index){
 const auto* C=State();if(!C||Player->Vehicle||Player->bSafehouse||Player->Health<=0||Player->bStoryLocked||FVector::Dist2D(Player->GetActorLocation(),C->Center)>C->Radius+200){Message=TEXT("Visit this settlement on foot to build.");return;}
 const FName Id=Selected;Player->ClosePanels();Selected=Id;Building=true;Claiming=false;Player->SettlementBuild82=true;Player->CancelReload();Player->ConsumeUIAttack();Player->GetCharacterMovement()->DisableMovement();Camera->SetWorldLocation(Player->GetActorLocation()+FVector(0,0,230));Camera->SetWorldRotation(FRotator(-25,Player->GetActorRotation().Yaw,0));Cast<APlayerController>(Player->Controller)->SetViewTarget(this);
 Roads.Empty();Sites.Empty();LWGen::Gather(FVector2D(C->Center),World->Seed,Roads,Sites);SelectedCatalog=Index>=0?Index:0;Moving=Support=NAME_None;Yaw=Player->GetActorRotation().Yaw;Height=0;Valid=false;SetPalette(Index<0);Message.Empty();
}
void ALWSettlement82::SetPalette(bool Show){Palette=Show;Player->SetMenuInput(Show);Player->ConsumeUIAttack();WaitClick=true;Preview->SetVisibility(!Show);}
int ALWSettlement82::Scrap()const{return Player->CountSupply(TEXT("scrap"))+(State()?State()->Scrap:0);}
bool ALWSettlement82::Spend(int Cost){auto* C=State();if(!C||Cost<0||Scrap()<Cost){Message=TEXT("Not enough scrap.");return false;}int Stored=FMath::Min(C->Scrap,Cost);if(!Player->ConsumeSupply(TEXT("scrap"),Cost-Stored))return false;C->Scrap-=Stored;return true;}
void ALWSettlement82::Deposit(){auto* C=State();if(!C||FVector::Dist2D(Player->GetActorLocation(),C->Center)>C->Radius){Message=TEXT("Visit the settlement to deposit scrap.");return;}int N=Player->CountSupply(TEXT("scrap"));if(Player->ConsumeSupply(TEXT("scrap"),N)){C->Scrap+=N;Message=FString::Printf(TEXT("Deposited %d scrap."),N);Player->RequestSave40();}}
void ALWSettlement82::Refresh(FName Id){
 const auto* C=State();if(!C)return;const auto* P=C->Pieces.FindByPredicate([&](const auto& X){return X.Id==Id;});if(!P)return;
 if(auto* Old=Actors.FindRef(Id).Get())Old->Destroy();auto* A=GetWorld()->SpawnActor<ALWBuildPiece82>();if(!A)return;A->Claim=C->Id;A->Build(this,*P);Actors.Add(Id,A);
}
void ALWSettlement82::Stream(){
 const FVector At=Player->GetActorLocation();
 for(auto& Pair:Player->RPG.Claims82){auto& C=Pair.Value;const bool Near=FVector::Dist2D(At,C.Center)<C.Radius+16000&&FMath::Abs(At.Z-C.Center.Z)<12000;
  if(Near&&!Loaded.Contains(C.Id)){Loaded.Add(C.Id);Queue.Add({C.Id,NAME_None});for(const auto& P:C.Pieces)Queue.Add({C.Id,P.Id});}
  if(!Near&&Loaded.Remove(C.Id)){Queue.RemoveAll([&](const auto& Q){return Q.Key==C.Id;});for(auto I=Actors.CreateIterator();I;++I)if(IsValid(I.Value())&&I.Value()->Claim==C.Id){I.Value()->Destroy();I.RemoveCurrent();}}
 }
}
void ALWSettlement82::Tick(float Dt){
 Super::Tick(Dt);if(!IsValid(Player)||!World||!Player->bStarted||Player->bMenu)return;
 if(Building){if(Player->Health<=0||Player->bStoryLocked){Close();return;}if(!Palette)TickBuild(Dt);}
 // Geometry and characters are hydrated over frames, not in one chunk-sized burst.
 const double Start=FPlatformTime::Seconds();int Count=0;
 while(!Queue.IsEmpty()&&Count++<3&&FPlatformTime::Seconds()-Start<.002){auto Job=Queue[0];Queue.RemoveAt(0,1,EAllowShrinking::No);const auto* C=Player->RPG.Claims82.Find(Job.Key);if(!C)continue;
  FLWConstruction82 Flag;Flag.Id=FName(*(C->Id.ToString()+TEXT("_flag")));Flag.Catalog=TEXT("flag");Flag.Transform=FTransform(C->Center);
  const auto* P=Job.Value.IsNone()?&Flag:C->Pieces.FindByPredicate([&](const auto& X){return X.Id==Job.Value;});if(!P||Actors.Contains(P->Id))continue;
  auto* A=GetWorld()->SpawnActor<ALWBuildPiece82>();if(A){A->Claim=C->Id;A->Build(this,*P);Actors.Add(P->Id,A);}
 }
 Poll+=Dt;if(Poll>=1){float Seconds=Poll;Poll=0;Stream();Economy(Seconds);TickParking(Seconds);}
}
