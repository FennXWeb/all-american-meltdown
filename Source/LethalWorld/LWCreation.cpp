#include "LWCampaign76.h"
#include "LWCharacter.h"
#include "LWCreator35.h"
#include "LWWorld.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "InputCoreTypes.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Camera/PlayerCameraManager.h"
void ALWCharacter::BeginOpening(){
 ClosePanels();bSettings=false;bWorldSetup=false;bMenu=true;bAim=bSprint=bTrigger=false;CancelReload();DraftIdentity=FLWIdentity();LWCreator35::Normalize(DraftIdentity);CreatorTab35=CreatorPage35=0;CreatorZoom35=0;StartingAttributes.Init(4,7);CreationMessage.Empty();PortraitYaw=0;OpeningPaused=false;NameEditing=false;
 if(World->Music)World->Music->Stop();
 OpeningMode=2;OpeningShot=0;OpeningSince=GetWorld()->GetRealTimeSeconds();
 OpeningScene=GetWorld()->SpawnActor<ALWOpeningScene>(FVector(0,0,80000),FRotator::ZeroRotator);OpeningScene->World=World;OpeningScene->Camera->PostProcessSettings=Camera->PostProcessSettings;OpeningScene->Portrait(DraftIdentity,PortraitYaw);
 if(auto* PC=Cast<APlayerController>(Controller)){PC->SetViewTarget(OpeningScene);}
 SetMenuInput(true);UGameplayStatics::SetGamePaused(this,true);
}
void ALWCharacter::UpdateOpening(){
 double Now=GetWorld()->GetRealTimeSeconds();
 if(OpeningMode==2&&OpeningScene&&CreatorDirty35&&Now-CreatorPreviewTime35>.12){
  LWCreator35::Normalize(DraftIdentity);OpeningScene->Portrait(DraftIdentity,PortraitYaw);
  if(CreatorZoom35>=1){OpeningScene->From=OpeningScene->To=CreatorZoom35<2?FVector(145,-45,177):FVector(250,-90,157);OpeningScene->Focus=CreatorZoom35<2?FVector(0,0,168):FVector(0,0,139);OpeningScene->Pose(0);}
  CreatorDirty35=false;CreatorPreviewTime35=Now;
 }
 if(OpeningMode&&OpeningScene)if(auto* PC=Cast<APlayerController>(Controller))if(PC->PlayerCameraManager)PC->PlayerCameraManager->UpdateCamera(0);
 if(OpeningMode!=1||!OpeningScene||OpeningPaused)return;
 float Age=Now-OpeningSince;if(Age>=18){OpeningShot++;OpeningSince=Now;if(OpeningShot>=LWOpening::Beats().Num()){OpeningClick(TEXT("intro_skip"));return;}OpeningScene->Stage(OpeningShot);Age=0;}OpeningScene->Pose(Age/18);
}
void ALWCharacter::OpeningClick(FName N){
 if(!OpeningMode)return;
 if(N==TEXT("intro_pause")&&OpeningMode==1){double Now=GetWorld()->GetRealTimeSeconds();if(!OpeningPaused)OpeningSince=Now-OpeningSince;else OpeningSince=Now-OpeningSince;OpeningPaused=!OpeningPaused;if(OpeningScene&&OpeningScene->Audio)OpeningScene->Audio->SetPaused(OpeningPaused);return;}
 if(N==TEXT("intro_skip")&&OpeningMode==1){OpeningMode=2;OpeningPaused=false;OpeningScene->Portrait(DraftIdentity,PortraitYaw);return;}
 if(N==TEXT("creator_back")){EndOpening(false);bWorldSetup=true;bSeedEdit=false;SetMenuInput(true);return;}
 if(OpeningMode!=2)return;
 if(LWCreator35::Click(this,N))return;
 if(N==TEXT("creator_done")){if(!LWOpening::ValidAllocation(StartingAttributes)){CreationMessage=TEXT("ASSIGN ALL 28 CATEGORY POINTS BEFORE STARTING.");return;}if(DraftIdentity.Name.TrimStartAndEnd().IsEmpty()){CreationMessage=TEXT("ENTER A NAME.");return;}EndOpening(true);return;}
 if(N==TEXT("name")){NameEditing=!NameEditing;return;}
 FString K=N.ToString();if(K.StartsWith(TEXT("plus_"))||K.StartsWith(TEXT("minus_"))){int I=FCString::Atoi(*K.Mid(K.StartsWith(TEXT("plus_"))?5:6));int Sum=0;for(int V:StartingAttributes)Sum+=V;if(StartingAttributes.IsValidIndex(I)){if(K.StartsWith(TEXT("plus_"))&&Sum<28&&StartingAttributes[I]<10)StartingAttributes[I]++;if(K.StartsWith(TEXT("minus_"))&&StartingAttributes[I]>1)StartingAttributes[I]--;}CreationMessage.Empty();return;}
 if(N==TEXT("reset_points")){StartingAttributes.Init(4,7);CreationMessage.Empty();return;}
 if(N==TEXT("skin"))DraftIdentity.Skin=(DraftIdentity.Skin+1)%6;
 if(N==TEXT("hair"))DraftIdentity.Hair=(DraftIdentity.Hair+1)%10;
 if(N==TEXT("haircolor"))DraftIdentity.HairColor=(DraftIdentity.HairColor+1)%4;
 if(N==TEXT("outfit"))DraftIdentity.Outfit=(DraftIdentity.Outfit+1)%4;
 if(N==TEXT("frame"))DraftIdentity.Frame=(DraftIdentity.Frame+1)%3;
 if(N==TEXT("face"))DraftIdentity.Face=(DraftIdentity.Face+1)%3;
 if(N==TEXT("body"))DraftIdentity.Body=1-DraftIdentity.Body;
 for(auto Pair:{TPair<FName,float*>(TEXT("height"),&DraftIdentity.Height),TPair<FName,float*>(TEXT("weight"),&DraftIdentity.Weight),TPair<FName,float*>(TEXT("shoulders"),&DraftIdentity.Shoulders)}){if(N==FName(*(Pair.Key.ToString()+TEXT("_up"))))*Pair.Value=FMath::Min(1.f,*Pair.Value+.1f);if(N==FName(*(Pair.Key.ToString()+TEXT("_down"))))*Pair.Value=FMath::Max(0.f,*Pair.Value-.1f);}
 if(N==TEXT("rotate"))PortraitYaw=FMath::Fmod(PortraitYaw+45,360);
 OpeningScene->Portrait(DraftIdentity,PortraitYaw);
}
void ALWCharacter::EndOpening(bool Commit){
 if(!OpeningMode)return;FLWIdentity Chosen=DraftIdentity;auto Stats=StartingAttributes;
 if(OpeningScene){OpeningScene->Destroy();OpeningScene=nullptr;}
 if(auto* PC=Cast<APlayerController>(Controller)){PC->SetViewTarget(this);}
 OpeningMode=0;NameEditing=false;OpeningPaused=false;
 if(!Commit)return;
 TGuardValue<bool> Guard(bLoadingSave,true);World->Difficulty=SetupDifficulty;
 NewGame();Identity=Chosen;RPG.Attributes=Stats;ApplyIdentity();ALWCampaign76::Ensure(this)->Begin(true);Message.Empty();MessageTime=0;
 // Commit one complete record after creation, not an intermediate outdoor spawn.
 bLoadingSave=false;CaptureMission37(TEXT("campaign76"),true);RequestSave40();
}
void ALWCharacter::ApplyIdentity(){
 LWCreator35::Normalize(Identity);
 BuildSurvivorBody();
 Identity.Skin=FMath::Clamp(Identity.Skin,0,5);Identity.Outfit=FMath::Clamp(Identity.Outfit,0,3);
 if(auto* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Materials/M_SurvivorTint")))for(auto* C:{Arms.Get(),LoadingHand.Get(),Weapon==19?WeaponMesh.Get():nullptr})if(C&&C->GetStaticMesh()){
 C->EmptyOverrideMaterials();const auto& Slots=C->GetStaticMesh()->GetStaticMaterials();for(int I=0;I<Slots.Num();I++){
 FString Name=Slots[I].MaterialInterface?Slots[I].MaterialInterface->GetName():FString();bool Sleeve=Name.Contains(TEXT("Cloth"))||Name.Contains(TEXT("Cotton35"))||Name.Contains(TEXT("Cotton61"));bool Skin=Name.Contains(TEXT("Skin"))||Name.Contains(TEXT("Flesh"));if(!Sleeve&&!Skin)continue;
 auto* Material=Slots[I].MaterialInterface.Get();auto* M=UMaterialInstanceDynamic::Create((Name.Contains(TEXT("32"))||Name.Contains(TEXT("35"))||Name.Contains(TEXT("61")))&&Material?Material:Base,C);M->SetVectorParameterValue(TEXT("Tint"),Sleeve?LWCreator35::Color(Identity.TopColor35):LWOpening::Skin(Identity.Skin)*1.5f);M->SetScalarParameterValue(TEXT("GloveAmount"),Identity.Gloves35?1:0);M->SetVectorParameterValue(TEXT("GloveTint"),LWCreator35::Color(Identity.ShoesColor35));C->SetMaterial(I,M);
 }}
}
void ALWCharacter::BindIdentityInput(UInputComponent* I){
 for(int N=0;N<26;N++){FInputKeyBinding B(FInputChord(FKey(FName(*FString::Chr('A'+N)))),IE_Pressed);B.bExecuteWhenPaused=true;B.bConsumeInput=false;B.KeyDelegate.GetDelegateForManualSet().BindLambda([this,N](){if(OpeningMode==2&&NameEditing&&DraftIdentity.Name.Len()<18)DraftIdentity.Name.AppendChar('A'+N);});I->KeyBindings.Add(MoveTemp(B));}
 for(FKey K:{EKeys::BackSpace,EKeys::SpaceBar}){FInputKeyBinding B(FInputChord(K),IE_Pressed);B.bExecuteWhenPaused=true;B.bConsumeInput=false;B.KeyDelegate.GetDelegateForManualSet().BindLambda([this,K](){if(OpeningMode==2&&NameEditing){if(K==EKeys::BackSpace&&!DraftIdentity.Name.IsEmpty())DraftIdentity.Name.LeftChopInline(1);else if(K==EKeys::SpaceBar&&DraftIdentity.Name.Len()<18)DraftIdentity.Name+=TEXT(" ");}});I->KeyBindings.Add(MoveTemp(B));}
}
