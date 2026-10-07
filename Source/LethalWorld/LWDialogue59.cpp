#include "LWDialogue59.h"
#include "LWAcoustics64.h"
#include "Engine/World.h"
#include "LWCharacter.h"
#include "LWResident.h"
#include "LWNPCLife.h"
#include "LWWorld.h"
#include "LWWorldObject.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Misc/PackageName.h"
#include "Misc/SecureHash.h"
#include "Misc/Crc.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "UObject/StrongObjectPtr.h"

ULWDialogueCatalog59* ULWDialogueCatalog59::Get(){
 static TStrongObjectPtr<ULWDialogueCatalog59> Cached;static bool Checked=false;
 if(!Checked){Checked=true;if(FPackageName::DoesPackageExist(TEXT("/Game/Audio/Dialogue59/DA_Dialogue59")))Cached.Reset(LoadObject<ULWDialogueCatalog59>(nullptr,TEXT("/Game/Audio/Dialogue59/DA_Dialogue59.DA_Dialogue59")));}
 return Cached.Get();
}
FString ULWDialogueCatalog59::Normalize(const FString& Text){TArray<FString> Words;Text.ParseIntoArrayWS(Words);return FString::Join(Words,TEXT(" "));}
FString ULWDialogueCatalog59::Key(FName Profile,const FString& Text){const FTCHARToUTF8 Bytes(*Normalize(Text));FMD5 Hash;Hash.Update(reinterpret_cast<const uint8*>(Bytes.Get()),Bytes.Length());uint8 Digest[16];Hash.Final(Digest);return Profile.ToString()+TEXT("_")+BytesToHex(Digest,16).ToLower();}
FName ULWDialogueCatalog59::ProceduralProfile75(FName Identity,bool Female)const{
 TArray<FName> Candidates;for(FName Id:ProceduralProfiles75)if(const auto* P=Profiles.Find(Id);P&&P->Female==Female)Candidates.Add(Id);
 Candidates.Sort(FNameLexicalLess());return Candidates.IsEmpty()?DefaultProfile(Identity,Female):Candidates[FCrc::StrCrc32(*(Identity.ToString().ToLower()+TEXT("_voice75")))%Candidates.Num()];
}
FName ULWDialogueCatalog59::DefaultProfile(FName Id,bool Female){
 const FString S=Id.ToString().ToLower();
 if(S.Contains(TEXT("mara")))return TEXT("F01");if(S.Contains(TEXT("inez")))return TEXT("F02");if(S.Contains(TEXT("elsie")))return TEXT("F03");if(S.Contains(TEXT("voss")))return TEXT("F03");if(S.Contains(TEXT("tessa")))return TEXT("F01");
 if(S.Contains(TEXT("jonah")))return TEXT("M01");if(S.Contains(TEXT("tomas")))return TEXT("M02");if(S.Contains(TEXT("rusk")))return TEXT("M03");if(S.Contains(TEXT("mercer")))return TEXT("M02");if(Id==TEXT("narrator"))return TEXT("M01");
 // Frozen v1 assignment: never use transient actor names or randomized GetTypeHash.
 return FName(*FString::Printf(TEXT("%s%02u"),Female?TEXT("F"):TEXT("M"),1+FCrc::StrCrc32(*S)%3));
}
FName ULWDialogueCatalog59::StoryIdentity(const FString& Name){const FString S=Name.ToLower();for(const TCHAR* N:{TEXT("mara"),TEXT("inez"),TEXT("elsie"),TEXT("voss"),TEXT("tessa"),TEXT("jonah"),TEXT("tomas"),TEXT("rusk"),TEXT("mercer")})if(S.Contains(N))return FName(*(FString(TEXT("story_"))+N));if(S.Contains(TEXT("collector")))return TEXT("choir_collector");if(S.Contains(TEXT("sergeant")))return TEXT("heroes_sergeant");return TEXT("narrator");}
ULWDialogue59::ULWDialogue59(){PrimaryComponentTick.bCanEverTick=true;PrimaryComponentTick.TickInterval=.15f;}
void ULWDialogue59::TickComponent(float Dt,ELevelTick Type,FActorComponentTickFunction* Function){Super::TickComponent(Dt,Type,Function);if(auto* N=Cast<ALWZombie>(GetOwner());N&&N->bDead&&Busy())Stop();}
bool ULWDialogue59::Available(){const auto* C=ULWDialogueCatalog59::Get();return C&&C->Enabled;}
ULWDialogue59* ULWDialogue59::Channel(AActor* Owner){if(!IsValid(Owner))return nullptr;auto* C=Owner->FindComponentByClass<ULWDialogue59>();if(!C){C=NewObject<ULWDialogue59>(Owner);Owner->AddInstanceComponent(C);C->RegisterComponent();}return C;}
FName ULWDialogue59::Assign(FName Identity,bool Female){
 Catalog=ULWDialogueCatalog59::Get();auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
 // Keep the original saved identity, even when a reduced active cast replaces it.
 // This preserves archived assignments if the cast is expanded again later.
 if(P)if(auto* Saved=P->RPG.VoiceProfiles59.Find(Identity)){Profile=*Saved;if(Catalog&&!Catalog->Cast.Contains(Identity))if(auto* Replacement=Catalog->ProceduralReplacements75.Find(Profile))Profile=*Replacement;return Profile;}
 Profile=Catalog?(Catalog->Cast.Contains(Identity)?Catalog->Cast[Identity]:Catalog->ProceduralProfile75(Identity,Female)):ULWDialogueCatalog59::DefaultProfile(Identity,Female);
 if(P)P->RPG.VoiceProfiles59.Add(Identity,Profile);return Profile;
}
void ULWDialogue59::Stop(){++Serial;if(Pending){Pending->CancelHandle();Pending.Reset();}if(IsValid(Audio))Audio->Stop();Audio=nullptr;}
bool ULWDialogue59::Busy()const{return Pending.IsValid()||(IsValid(Audio)&&Audio->IsPlaying());}
void ULWDialogue59::EndPlay(const EEndPlayReason::Type Reason){Stop();Super::EndPlay(Reason);}
void ULWDialogue59::Say(FName Identity,bool Female,const FString& Text,bool Relative){
 Stop();Assign(Identity,Female);if(!Catalog||!Catalog->Enabled)return;
 const FString K=ULWDialogueCatalog59::Key(Profile,Text);const auto* Line=Catalog->Lines.Find(K);
 if(!Line||Line->Audio.IsNull()){static TSet<FString> Reported;if(!Reported.Contains(K)){Reported.Add(K);UE_LOG(LogTemp,Display,TEXT("DIALOGUE59_SUBTITLE_ONLY profile=%s key=%s"),*Profile.ToString(),*K);}return;}
 if(auto* Director=GetWorld()->GetSubsystem<ULWDialogueDirector75>()){Director->Reserve(NAME_None,Line->Duration,false);Director->Speaking(this);}
 const FSoftObjectPath Path=Line->Audio.ToSoftObjectPath();const uint32 Request=Serial;
 Pending=UAssetManager::GetStreamableManager().RequestAsyncLoad(Path,FStreamableDelegate::CreateWeakLambda(this,[this,Request,Path,Text,Relative](){
  if(Request!=Serial||!IsValid(GetOwner()))return;auto* Sound=Cast<USoundBase>(Path.ResolveObject());if(!Sound){UE_LOG(LogTemp,Warning,TEXT("DIALOGUE59_LOAD_FAILED %s"),*Path.ToString());Pending.Reset();return;}
  auto* N=Cast<ALWZombie>(GetOwner());if(N&&(N->bDead||N->Health<=0)){Pending.Reset();return;}
  Audio=NewObject<UAudioComponent>(GetOwner());Audio->bAutoActivate=false;Audio->bStopWhenOwnerDestroyed=true;Audio->bAutoDestroy=true;Audio->bAllowSpatialization=!Relative;Audio->SetSound(Sound);Audio->SetVolumeMultiplier(.85f);Audio->SetPitchMultiplier(1.f);
  if(!Relative){Audio->SetupAttachment(GetOwner()->GetRootComponent());Audio->SetRelativeLocation(FVector(0,0,65));if(N&&N->World)Audio->AttenuationSettings=N->World->Attenuation;else if(auto* Object=Cast<ALWWorldObject>(GetOwner());Object&&Object->World)Audio->AttenuationSettings=Object->World->Attenuation;}
  Audio->RegisterComponent();Audio->Play();if(!Relative)if(auto* Acoustics=GetWorld()->GetSubsystem<ULWAcoustics64>())Acoustics->Track(Audio,TEXT("Dialogue59"),Audio->AttenuationSettings?Audio->AttenuationSettings->Attenuation:FSoundAttenuationSettings());UE_LOG(LogTemp,Verbose,TEXT("DIALOGUE59_PLAY %s duration=%.2f playing=%d"),*Path.ToString(),Sound->GetDuration(),Audio->IsPlaying());if(N&&N->LifeAnimation)N->LifeAnimation->Speak(Audio,Text);
  if(auto* R=Cast<ALWResident>(GetOwner())){R->Subtitle=Text;R->SubtitleTime=Sound->GetDuration()+.35f;}
  Pending.Reset();
 }),FStreamableManager::AsyncLoadHighPriority);
}

bool ULWDialogue59::Chatter75(FName Identity,bool Female,FName Group,bool Combat){
 if(Busy()||!GetWorld()||!IsValid(GetOwner()))return false;
 auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
 if(!P||!P->bStarted||P->bMenu||P->Health<=0||P->Speaker||P->EncounterSpeaker||P->bStoryLocked||P->bInventory||P->bMap||P->RPGPanel>0)return false;
 if(FVector::DistSquared(P->GetActorLocation(),GetOwner()->GetActorLocation())>FMath::Square(Combat?1900.:950.))return false;
 if(auto* N=Cast<ALWZombie>(GetOwner());N&&(N->bDead||N->Health<=0))return false;
 auto* D=GetWorld()->GetSubsystem<ULWDialogueDirector75>();if(!D||!D->CanSpeak(Combat))return false;
 Assign(Identity,Female);if(!Catalog||!Catalog->Enabled)return false;
 if(!ChatterSeeded75){ChatterRandom75.Initialize(FCrc::StrCrc32(*(Identity.ToString()+TEXT("_chatter75"))));ChatterSeeded75=true;}
 const auto* Pick=ChatterHistory75.Choose(Group,ChatterRandom75,[&](const FLWChatterLine75& L){return !D->Recent(L.Id)&&Catalog->Lines.Contains(ULWDialogueCatalog59::Key(Profile,L.Text));});
 if(!Pick)return false;
 const auto& Line=Catalog->Lines.FindChecked(ULWDialogueCatalog59::Key(Profile,Pick->Text));
 Say(Identity,Female,Pick->Text);D->Reserve(Pick->Id,Line.Duration,Combat);return true;
}
