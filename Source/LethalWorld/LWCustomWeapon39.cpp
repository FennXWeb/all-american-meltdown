#include "LWCharacter.h"
#include "LWWeaponMods.h"
#include "LWMystic62.h"
#include "LWWorld.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
void ALWCharacter::ConfigureCustom39(){for(auto& C:CustomMeshes39)if(C)C->DestroyComponent();CustomMeshes39.Empty();if(Mystic62){for(auto& C:Mystic62->Drops)if(C)C->DestroyComponent();Mystic62->DestroyComponent();Mystic62=nullptr;}const auto* G=ActiveGun();if(!G||!World)return;
 if(G->Camo62==9){auto* Body=World->Mesh(FName(*FString::Printf(TEXT("Mystic62_%02d"),Weapon)));if(Body)WeaponMesh->SetStaticMesh(Body);for(auto* C:{MovingPart.Get(),SecondPart.Get()})if(C){auto* M=World->Material(FName(*FString::Printf(TEXT("MysticMoving62_%d"),LWArsenal62::Theme(Weapon))));if(M)for(int I=0;I<C->GetNumMaterials();I++)C->SetMaterial(I,M);}Mystic62=NewObject<ULWMystic62>(this);Mystic62->SetupAttachment(WeaponMesh);Mystic62->RegisterComponent();Mystic62->Initialize(World,Weapon);
 }else if(G->Camo62>0&&G->Camo62<9){auto* M=World->Material(FName(*(G->Camo62>=7?FString::Printf(TEXT("Camo62_%d"),G->Camo62):FString::Printf(TEXT("WeaponSkin24_2_%02d"),G->Camo62-1))));if(M)for(auto* C:{WeaponMesh.Get(),MovingPart.Get(),SecondPart.Get()})if(C)for(int I=0;I<C->GetNumMaterials();I++)C->SetMaterial(I,M);}
}
void ALWCharacter::UpdateCustom39(){}
