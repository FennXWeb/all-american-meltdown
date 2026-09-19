#include "LWChapter52.h"
#include "LWStory.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
FVector LWChapter52::Objective(FName A){
 if(A==TEXT("supplies"))return {-1500,-2000,85};if(A==TEXT("ashes"))return {-2300,500,105};
 if(A==TEXT("triage"))return {-2200,200,100};if(A==TEXT("pharmacy"))return {2000,1800,110};if(A==TEXT("trauma"))return {2100,2300,90};
 if(A==TEXT("ledger"))return {-2300,2200,110};if(A==TEXT("relay_a"))return {2500,0,80};if(A==TEXT("relay_b"))return {-2400,1300,95};
 if(A==TEXT("manifest"))return {2300,1900,110};if(A==TEXT("key"))return {-2400,2300,100};if(A==TEXT("rescue"))return {1350,2940,120};
 if(A==TEXT("breach_a"))return {-2500,600,100};if(A==TEXT("breach_b"))return {2500,1900,100};
 if(A==TEXT("fastener"))return {-2850,-2000,55};if(A==TEXT("breaker"))return {-2100,-2400,125};if(A==TEXT("gear"))return {-2500,550,90};
 if(A==TEXT("free"))return {2600,-1550,120};if(A==TEXT("shield_a"))return {-950,3100,110};if(A==TEXT("shield_b"))return {950,3100,110};if(A==TEXT("charter"))return {0,3350,110};return {0,-2300,100};
}
FVector LWChapter52::Recovery(int S){if(S>=21&&S<=22)return {-2500,-2200,110};if(S==23)return {-2500,-450,110};if(S==24||S==25)return {3300,-2600,110};if(S>=26)return {0,1750,110};return {0,-3600,110};}
bool LWChapter52::Destructible(FName A){return A==TEXT("breach_a")||A==TEXT("breach_b")||A==TEXT("shield_a")||A==TEXT("shield_b");}
FName LWChapter52::Prop(FName A){if(A==TEXT("supplies")||A==TEXT("trauma"))return TEXT("Crate");if(A==TEXT("radio"))return TEXT("RadioV4");if(A==TEXT("triage"))return TEXT("ClinicBedV3");if(A==TEXT("pharmacy")||A==TEXT("gear"))return TEXT("EvidenceCabinetV18");if(A==TEXT("relay_a"))return TEXT("Generator");if(A==TEXT("relay_b")||A==TEXT("breaker")||A==TEXT("free"))return TEXT("ControlPanel52");if(A==TEXT("rescue"))return TEXT("ControlPanel52");if(Destructible(A))return TEXT("Generator");if(A==TEXT("fastener"))return TEXT("Scrap52");if(A==TEXT("key"))return TEXT("Keys52");return TEXT("Dossier52");}
TArray<FVector> LWChapter52::Posts(int I,int S){
 switch(I){
 case 0:return {{-2100,450,110},{2300,2000,110},{3200,-2600,110}};
 case 1:return {{-3400,-3400,110},{3200,-3200,110},{-3300,2900,110},{3500,1800,110},{-2400,1500,110},{2400,1300,110}};
 case 2:return {{-2200,500,110},{2100,500,110},{1800,2400,110},{0,1800,110},{-2700,2500,110}};
 case 3:return {{-2300,2100,110},{2600,2300,110},{-1800,-1800,110},{1700,-700,110},{-2800,900,110},{2800,-2400,110}};
 case 4:return {{-2300,700,110},{2300,900,110},{800,3000,110},{-3100,-2200,110},{3200,2100,110}};
 case 5:return {{2400,2200,110},{-900,1100,110},{800,2200,110},{-1800,3000,110},{2200,-2500,110},{-3000,-1200,110},{0,-1700,110}};
 case 6:return {{-2400,2300,110},{1500,200,110},{1600,2300,110},{-2200,200,110},{2800,-2200,110},{-800,3200,110},{500,-800,110},{3200,1700,110}};
 case 7:return S==19?TArray<FVector>{{0,100,110},{-2800,2300,110},{2600,1800,110},{-2900,-500,110}}:TArray<FVector>{{-2500,-1500,110},{2300,-1300,110},{-2700,500,110},{2700,2000,110},{-900,2700,110},{1000,2500,110},{3100,-2700,110},{-3400,1700,110}};
 default:if(S==23)return {{-2400,550,110},{-3100,1500,110},{-1300,600,110}};if(S==24)return {{2600,-450,110},{3300,-600,110},{1800,150,110},{1200,1500,110},{-600,800,110}};if(S==26)return {{0,3100,110},{-900,2700,110},{950,2700,110},{-1600,1800,110},{1600,2000,110}};return {{-900,500,110},{900,900,110},{-2300,1700,110},{2400,1700,110},{-3300,2800,110},{3200,2800,110},{-1000,1900,110},{900,1800,110},{-1700,3200,110},{1700,3200,110}};
 }
}
FVector ALWStoryDirector::Objective52(FName A)const{return At(LWStory::Missions()[State().Stage].Site,LWChapter52::Objective(A));}
void ALWStoryDirector::Migrate52(){if(State().LayoutVersion52>=52)return;const FVector2D Old[]={{8000,10000},{24000,14000},{43000,14000},{62000,14000},{81000,14000},{81000,34000},{62000,34000},{43000,34000},{24000,34000}};
 int Relocate=INDEX_NONE;float Nearest=6500;const FVector Original=Player->GetActorLocation();
 for(int I=0;I<9;++I){if(Player->RPG.KnownPlaces.Contains(0xEF320000u+I))Player->RPG.KnownPlaces[0xEF320000u+I]=At(I,FVector(0,-4200,100));const float Dist=FVector::Dist2D(Original,FVector(Old[I],0));if(Dist<Nearest){Nearest=Dist;Relocate=I;}}
 if(Relocate!=INDEX_NONE)Player->SetActorLocation(At(Relocate,LWChapter52::Recovery(FMath::Clamp(State().Stage,0,29))),false,nullptr,ETeleportType::TeleportPhysics);
 if(auto* Fort=Player->RPG.Settlements.Find(TEXT("fort_resolute")))Fort->Center=At(8,FVector(0,-1800,100));if(Player->RPG.Respawn.Name==TEXT("Fort Resolute"))Player->RPG.Respawn.Position=At(8,FVector(0,-1800,100));State().LayoutVersion52=52;
}
void ALWStoryDirector::SafeRecovery52(){const int S=State().Stage,I=LWStory::Missions()[S].Site;CombatGrace52=GetWorld()->GetTimeSeconds()+4;
 if(I>=0&&FVector::Dist2D(Player->GetActorLocation(),At(I))<7000){Player->SetActorLocation(At(I,LWChapter52::Recovery(S)),false,nullptr,ETeleportType::TeleportPhysics);Player->GetCharacterMovement()->StopMovementImmediately();Player->Health=FMath::Max(Player->Health,Player->MaxHealth()*.75f);}
}
void ALWStoryEnemy::Tick(float Dt){if(!Director){Super::Tick(Dt);return;}auto* P=Director->Player.Get();if(!P||Director->Locked()||GetWorld()->GetTimeSeconds()<Director->CombatGrace52){GetCharacterMovement()->StopMovementImmediately();return;}
 // Do not wake an entire compound through its walls. Each room/patrol acquires its own target.
 if(Alert<=0&&FVector::Dist2D(P->GetActorLocation(),GetActorLocation())>2400)return;
 const int Count=FMath::Max(1,Director->Enemies.Num());const int Lead=int(GetWorld()->GetTimeSeconds()/1.4f)%Count;
 const int Slot=Director->Enemies.IndexOfByKey(this);if(Slot!=Lead&&Slot!=(Lead+1)%Count)AttackCooldown=FMath::Max(AttackCooldown,.35f);
 Super::Tick(Dt);
}
float ALWStoryNode::TakeDamage(float D,const FDamageEvent& E,AController* I,AActor* C){if(!Director||!LWChapter52::Destructible(Action)||D<=0||!Director->Available(Action))return 0;Durability52-=D;if(Durability52<=0){Director->Activate(Action);Body->SetVisibility(false);Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);}return D;}
