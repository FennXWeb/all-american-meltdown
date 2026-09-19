#include "LWCharacter.h"
#include "LWAppearance.h"
#include "LWCreator35.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
void ALWCharacter::BuildSurvivorBody(){
 LWCreator35::Cleanup(this);
 for(auto& C:SurvivorParts)if(C)C->DestroyComponent();SurvivorParts.Empty();
 if(!SurvivorRoot){SurvivorRoot=NewObject<USceneComponent>(this);SurvivorRoot->SetupAttachment(GetRootComponent());SurvivorRoot->RegisterComponent();}
 SurvivorRoot->SetRelativeLocation(FVector(-17,0,-GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));
 LWAppearance::Build(this,SurvivorRoot,Identity,SurvivorParts,true);
}
void ALWCharacter::TickSurvivorBody(float Dt){
 if(!SurvivorRoot)return;
 SurvivorRoot->SetRelativeLocation(FVector(-17,0,-GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));
 SurvivorRoot->SetRelativeRotation(FRotator(Prone54?-80.f:0.f,FMath::FindDeltaAngleDegrees(GetActorRotation().Yaw,GetControlRotation().Yaw),0));
 for(auto& C:SurvivorParts)if(C&&(C->ComponentTags.Contains(TEXT("LeftLeg"))||C->ComponentTags.Contains(TEXT("RightLeg")))){
 C->SetVisibility(false,false);TArray<USceneComponent*> VisualChildren;C->GetChildrenComponents(false,VisualChildren);for(auto* Child:VisualChildren)Child->SetVisibility(bStarted&&!Vehicle&&!SittingChair&&Health>0);float Swing=FMath::Sin(GetWorld()->GetTimeSeconds()*9)*FMath::Min(24.f,float(GetVelocity().Size2D())*.06f)*(C->ComponentTags.Contains(TEXT("LeftLeg"))?1:-1);C->SetRelativeRotation(FRotator(Sliding54?65.f:Swing*(Prone54?.35f:1.f),0,0));}
}
