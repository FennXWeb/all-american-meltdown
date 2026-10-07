#include "LWCharacter.h"
#include "LWWorld.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"

void ALWCharacter::BeginWorldSetup(){
 bWorldSetup=true;bSeedEdit=false;SetupDifficulty=FMath::Clamp(World->Difficulty,0,2);
 bMenu=true;bSettings=false;SetMenuInput(true);UGameplayStatics::SetGamePaused(this,true);
}
void ALWCharacter::WorldSetupClick(FName N){
 if(N==TEXT("difficulty"))SetupDifficulty=(SetupDifficulty+1)%3;
 if(N==TEXT("setupback")){bWorldSetup=false;bSeedEdit=false;}
 if(N==TEXT("createworld")){
 bWorldSetup=false;bSeedEdit=false;BeginOpening();
 }
}
void ALWCharacter::BindSeedInput(UInputComponent* Input){

}
void ALWWorld::ApplyGenerationSettings(){LWGen::TownDensity=TownSetting==0?22:TownSetting==2?65:43;LWGen::ParcelLevel=POISetting;LWGen::Ruggedness=TerrainSetting==0?.35f:TerrainSetting==2?1.8f:1.f;}
