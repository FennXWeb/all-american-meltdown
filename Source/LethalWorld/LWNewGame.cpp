#include "LWCharacter.h"
#include "LWWorld.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"

void ALWCharacter::BeginWorldSetup(){
 bWorldSetup=true;bSeedEdit=false;SeedText=FString::FromInt(World->Seed);SetupTowns=World->TownSetting;SetupPOIs=World->POISetting;SetupTerrain=World->TerrainSetting;SetupDifficulty=World->Difficulty;
 bMenu=true;bSettings=false;SetMenuInput(true);UGameplayStatics::SetGamePaused(this,true);
}
void ALWCharacter::WorldSetupClick(FName N){
 if(N==TEXT("seed")){bSeedEdit=true;SeedText.Empty();}
 if(N==TEXT("randomseed")){SeedText=FString::FromInt(FMath::RandRange(1,999999999));bSeedEdit=false;}
 if(N==TEXT("towns"))SetupTowns=(SetupTowns+1)%3;
 if(N==TEXT("pois"))SetupPOIs=(SetupPOIs+1)%3;
 if(N==TEXT("terrain"))SetupTerrain=(SetupTerrain+1)%3;
 if(N==TEXT("difficulty"))SetupDifficulty=(SetupDifficulty+1)%3;
 if(N==TEXT("setupback")){bWorldSetup=false;bSeedEdit=false;}
 if(N==TEXT("createworld")){
 if(SeedText.IsEmpty()){Notify(TEXT("ENTER A SEED"));return;}
 bWorldSetup=false;bSeedEdit=false;BeginOpening();
 }
}
void ALWCharacter::BindSeedInput(UInputComponent* Input){
 const FKey Digits[]={EKeys::Zero,EKeys::One,EKeys::Two,EKeys::Three,EKeys::Four,EKeys::Five,EKeys::Six,EKeys::Seven,EKeys::Eight,EKeys::Nine};
 for(int I=0;I<10;I++){FInputKeyBinding B(FInputChord(Digits[I]),IE_Pressed);B.bExecuteWhenPaused=true;B.bConsumeInput=false;B.KeyDelegate.GetDelegateForManualSet().BindLambda([this,I](){if(bWorldSetup&&bSeedEdit&&SeedText.Len()<9)SeedText+=FString::FromInt(I);});Input->KeyBindings.Add(MoveTemp(B));}
 FInputKeyBinding Back(FInputChord(EKeys::BackSpace),IE_Pressed);Back.bExecuteWhenPaused=true;Back.KeyDelegate.GetDelegateForManualSet().BindLambda([this](){if(bWorldSetup&&bSeedEdit&&!SeedText.IsEmpty())SeedText.LeftChopInline(1);});Input->KeyBindings.Add(MoveTemp(Back));
}
void ALWWorld::ApplyGenerationSettings(){LWGen::TownDensity=TownSetting==0?22:TownSetting==2?65:43;LWGen::ParcelLevel=POISetting;LWGen::Ruggedness=TerrainSetting==0?.35f:TerrainSetting==2?1.8f:1.f;}
