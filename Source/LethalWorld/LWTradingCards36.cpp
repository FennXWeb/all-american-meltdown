#include "LWTradingCards36.h"
#include "LWCharacter.h"
#include "LWZombie.h"
#include "LWWorld.h"
#include "LWWorldObject.h"
#include "LWAudioCatalog.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
namespace LWCollect36 {
const TArray<FCard>& Deck(){static const TArray<FCard> Cards={
#include "LWTradingCardDefs36.inl"
};return Cards;}
float Bonus(const FLWRPGState& S,FName Effect){float V=0;for(int I:S.TradingCards36)if(Deck().IsValidIndex(I)&&Effect==FName(Deck()[I].Effect))V+=Deck()[I].Value;return V;}
bool Collect(FLWRPGState& S,int I){if(!Deck().IsValidIndex(I)||S.TradingCards36.Contains(I))return false;S.TradingCards36.Add(I);return true;}
int Select(uint32 Seed,int Quality){FRandomStream R(Seed);float Total=0;const float Normal[]={16,8,3,1,.35f},Dangerous[]={5,5,4,2,1};const float* Weights=Quality>0?Dangerous:Normal;for(const auto& C:Deck())Total+=Weights[C.Rarity];float Pick=R.FRand()*Total;for(const auto& C:Deck()){Pick-=Weights[C.Rarity];if(Pick<=0)return C.Index;}return 99;}
const TCHAR* RarityName(int I){const TCHAR* N[]={TEXT("COMMON"),TEXT("UNCOMMON"),TEXT("RARE"),TEXT("EPIC"),TEXT("LEGENDARY")};return N[FMath::Clamp(I,0,4)];}
FLinearColor RarityColor(int I){const FLinearColor C[]={FLinearColor(.7,.72,.67),FLinearColor(.3,.76,.35),FLinearColor(.25,.55,1),FLinearColor(.72,.37,.93),FLinearColor(1,.65,.16)};return C[FMath::Clamp(I,0,4)];}
FString Benefit(const FCard& C){FString Label=C.Effect;Label=Label.ToUpper();if(Label==TEXT("GUN"))Label=TEXT("FIREARM DAMAGE");if(Label==TEXT("MELEE"))Label=TEXT("MELEE DAMAGE");if(Label==TEXT("ARMOR"))Label=TEXT("DAMAGE RESISTANCE");if(Label==TEXT("BARTER"))Label=TEXT("MERCHANT DISCOUNT");if(Label==TEXT("RECOVERY"))Label=TEXT("STAMINA RECOVERY");if(Label==TEXT("RELOAD"))Label=TEXT("RELOAD SPEED");if(Label==TEXT("XP"))Label=TEXT("EXPERIENCE");bool Flat=FName(C.Effect)==TEXT("health")||FName(C.Effect)==TEXT("stamina");return FString::Printf(TEXT("+%s%s %s"),*FString::SanitizeFloat(C.Value*(Flat?1:100)),Flat?TEXT(""):TEXT("%"),*Label);}
UTexture2D* Artwork(int I){
 if(!Deck().IsValidIndex(I))return nullptr;
 const FSoftObjectPath Path(FString::Printf(TEXT("/Game/Art/Textures/T_TradingCard%03d36.T_TradingCard%03d36"),I+1,I+1));
 // Retain handles so repeatedly visiting a collection page cannot evict/reload its artwork.
 static TMap<int,TSharedPtr<FStreamableHandle>> Requests;
 if(!Requests.Contains(I)){TArray<FSoftObjectPath> Assets={Path,FSoftObjectPath(TEXT("/Game/Materials/M_TradingCard36.M_TradingCard36")),FSoftObjectPath(TEXT("/Game/Materials/M_TradingCardFrame36.M_TradingCardFrame36"))};Requests.Add(I,UAssetManager::GetStreamableManager().RequestAsyncLoad(Assets,FStreamableDelegate()));}
 if(!Requests[I]||!Requests[I]->HasLoadCompleted())return nullptr;
 return Cast<UTexture2D>(Path.ResolveObject());
}
void Spawn(ALWChunk* Chunk,ALWWorld* W,const LWGen::FSite& S){
 if(!IsValid(Chunk)||!IsValid(W))return;uint32 H=LWGen::Hash(S.Id,36,W->Seed,36101);if(H%100>=46)return;int Index=Select(H^0xabc391u,LWDungeons::IsDungeon(S.Type)||LWPlaces::Underground(S.Type)||S.Type==21?1:0);
 auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(W,0));if(P&&P->RPG.TradingCards36.Contains(Index))return;
 FRandomStream R(H);TArray<FVector> Candidates;
 for(auto& A:Chunk->Residents)if(auto* O=Cast<ALWWorldObject>(A)){if(O->Kind!=ELWObjectKind::Container&&O->Kind!=ELWObjectKind::Furniture)continue;FVector Local=FRotator(0,S.Yaw,0).UnrotateVector(O->GetActorLocation()-FVector(S.Position,0));if(FMath::Abs(Local.X)>S.Size.X*.5||FMath::Abs(Local.Y)>S.Size.Y*.5)continue;FBox Bounds=O->GetComponentsBoundingBox();float Height=Bounds.Max.Z-O->GetActorLocation().Z;if(Height<20||Height>135)continue;Candidates.Add(FVector(Bounds.GetCenter().X,Bounds.GetCenter().Y,Bounds.Max.Z+5));}
 for(int I=0;I<12;I++){FVector2D At=I==0?LWGen::Entrance(S):S.Position+FVector2D(R.FRandRange(-S.Size.X*.36,S.Size.X*.36),R.FRandRange(-S.Size.Y*.36,S.Size.Y*.36));Candidates.Add(FVector(At,145));}
 FCollisionQueryParams Q(NAME_None,false);if(P)Q.AddIgnoredActor(P);for(auto& Resident:Chunk->Residents)if(Cast<ALWZombie>(Resident))Q.AddIgnoredActor(Resident);
 for(int I=0;I<Candidates.Num();I++){int J=(I+int(H%FMath::Max(1,Candidates.Num())))%Candidates.Num();FVector At=Candidates[J];FHitResult Hit;if(!W->GetWorld()->LineTraceSingleByChannel(Hit,At,At-FVector(0,0,180),ECC_Visibility,Q)||Hit.Normal.Z<.92f)continue;if(auto* O=Cast<ALWWorldObject>(Hit.GetActor());O&&O->Kind==ELWObjectKind::Car)continue;
  FVector Place=Hit.ImpactPoint+FVector(0,0,.4);bool Flat=true;for(FVector Offset:{FVector(6,9,0),FVector(-6,9,0),FVector(6,-9,0),FVector(-6,-9,0)}){FHitResult Edge;if(!W->GetWorld()->LineTraceSingleByChannel(Edge,Place+Offset+FVector(0,0,5),Place+Offset-FVector(0,0,5),ECC_Visibility,Q)||FMath::Abs(Edge.ImpactPoint.Z-Hit.ImpactPoint.Z)>1.5f){Flat=false;break;}}if(!Flat)continue;
  auto* Card=W->GetWorld()->SpawnActor<ALWTradingCard36>(Place,FRotator(0,S.Yaw+R.FRandRange(-22,22),0));if(Card){Card->Setup(Index);Chunk->Residents.Add(Card);}return;
 }
}
}
ALWTradingCard36::ALWTradingCard36(){PrimaryActorTick.bCanEverTick=true;PrimaryActorTick.TickInterval=1;Mesh->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube")));Mesh->SetRelativeScale3D(FVector(.16,.24,.006));Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);Mesh->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);Mesh->SetCanEverAffectNavigation(false);Picture=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardFace"));Picture->SetupAttachment(Mesh);Picture->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Plane.Plane")));Picture->SetRelativeScale3D(FVector(.91,.91,1));Picture->SetRelativeLocation(FVector(0,0,51));Picture->SetCollisionEnabled(ECollisionEnabled::NoCollision);Picture->SetCanEverAffectNavigation(false);Picture->SetCastShadow(false);}
void ALWTradingCard36::Setup(int I){if(CardIndex!=FMath::Clamp(I,0,99))ArtReady=false;CardIndex=FMath::Clamp(I,0,99);auto* Art=LWCollect36::Artwork(CardIndex);if(!Art)return;if(auto* Base=Cast<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/Materials/M_TradingCard36.M_TradingCard36")).ResolveObject())){auto* M=UMaterialInstanceDynamic::Create(Base,this);M->SetTextureParameterValue(TEXT("CardArt"),Art);ArtReady=true;M->SetVectorParameterValue(TEXT("RarityTint"),LWCollect36::RarityColor(LWCollect36::Deck()[CardIndex].Rarity));Picture->SetMaterial(0,M);}if(auto* Base=Cast<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/Materials/M_TradingCardFrame36.M_TradingCardFrame36")).ResolveObject())){auto* Frame=UMaterialInstanceDynamic::Create(Base,this);Frame->SetVectorParameterValue(TEXT("Tint"),LWCollect36::RarityColor(LWCollect36::Deck()[CardIndex].Rarity));Mesh->SetMaterial(0,Frame);}}
FString ALWTradingCard36::Prompt()const{return FString(TEXT("[E] COLLECT "))+LWCollect36::Deck()[CardIndex].Name;}
void ALWTradingCard36::Use(ALWCharacter* P){if(!P||!P->CanAct()||FVector::Dist(P->GetActorLocation(),GetActorLocation())>400)return;if(LWCollect36::Collect(P->RPG,CardIndex)){const auto& C=LWCollect36::Deck()[CardIndex];P->Notify(FString(TEXT("CARD COLLECTED: "))+C.Name+TEXT(" / ")+LWCollect36::Benefit(C),7);P->CollectionSelection36=CardIndex;ULWAudioCatalog::PlaySlot(P,TEXT("CardDeal"),GetActorLocation());P->RequestSave40();}Destroy();}
void ALWTradingCard36::Tick(float Dt){Super::Tick(Dt);if(!ArtReady)Setup(CardIndex);if(auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));P&&P->RPG.TradingCards36.Contains(CardIndex))Destroy();}
