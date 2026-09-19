#include "LWBunker45.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWWorldObject.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "DrawDebugHelpers.h"

void ALWBunker45::BeginEdit(int Index){if(!Player->bSafehouse||BuildQueue.Num()){Message=TEXT("Enter the bunker and wait for construction to finish.");return;}Player->BaseUI45=false;Player->BuildMode45=true;Player->bTrigger=false;Player->CancelReload();Player->GetCharacterMovement()->DisableMovement();BuildCamera->SetWorldLocation(Player->GetActorLocation()+FVector(0,0,90));BuildCamera->SetWorldRotation(Player->GetControlRotation());SelectedCatalog=Index;SelectedFurniture=NAME_None;Preview->SetVisibility(false);if(auto* PC=Cast<APlayerController>(Player->Controller)){PC->SetViewTarget(this);Player->SetMenuInput(false);}if(Catalog().IsValidIndex(Index)){Preview->SetStaticMesh(World->Mesh(Catalog()[Index].Model));Preview->SetWorldScale3D(FVector(1));Preview->SetMaterial(0,World->Material(TEXT("Glass")));Preview->SetVisibility(true);}}
void ALWBunker45::Select(){FHitResult H;FCollisionQueryParams Q(NAME_None,false,this);Q.AddIgnoredActor(Player);FVector A=BuildCamera->GetComponentLocation();if(!GetWorld()->LineTraceSingleByChannel(H,A,A+BuildCamera->GetForwardVector()*5000,ECC_Visibility,Q))return;for(auto& Pair:Furniture)if(Pair.Value==H.GetActor()&&!Pair.Value->IsHidden()){SelectedFurniture=Pair.Key;SelectedCatalog=-1;auto* M=Pair.Value->FindComponentByClass<UStaticMeshComponent>();Preview->SetStaticMesh(M->GetStaticMesh());Preview->SetWorldScale3D(Pair.Value->GetActorScale3D());Preview->SetMaterial(0,World->Material(TEXT("Glass")));Preview->SetVisibility(true);Yaw=Pair.Value->GetActorRotation().Yaw;Message=TEXT("Selected. Left click to move, Delete to dismantle.");return;}}
void ALWBunker45::TickBuild(float Dt){auto* PC=Cast<APlayerController>(Player->Controller);if(!PC)return;
 if(PC->WasInputKeyJustPressed(EKeys::Escape)){Close();return;}
 float X=0,Y=0;PC->GetInputMouseDelta(X,Y);FRotator R=BuildCamera->GetComponentRotation();R.Yaw+=X*.16f;R.Pitch=FMath::Clamp(R.Pitch-Y*.16f,-85.f,85.f);BuildCamera->SetWorldRotation(R);
 FVector Move=R.Vector()*float(PC->IsInputKeyDown(EKeys::W)-PC->IsInputKeyDown(EKeys::S))+R.RotateVector(FVector::RightVector)*float(PC->IsInputKeyDown(EKeys::D)-PC->IsInputKeyDown(EKeys::A))+FVector::UpVector*float(PC->IsInputKeyDown(EKeys::SpaceBar)-PC->IsInputKeyDown(EKeys::LeftControl));
 FVector At=BuildCamera->GetComponentLocation()+Move.GetClampedToMaxSize(1)*Dt*(PC->IsInputKeyDown(EKeys::LeftShift)?650:300);if(ALWWorld::IsSafePosition(At))BuildCamera->SetWorldLocation(At);
 if(PC->WasInputKeyJustPressed(EKeys::E))Select();if(PC->WasInputKeyJustPressed(EKeys::R))Yaw=FMath::UnwindDegrees(Yaw+15);if(PC->WasInputKeyJustPressed(EKeys::G))Grid=!Grid;
 if(PC->WasInputKeyJustPressed(EKeys::RightMouseButton)){SelectedFurniture=NAME_None;SelectedCatalog=-1;Preview->SetVisibility(false);}
 if(PC->WasInputKeyJustPressed(EKeys::Delete))Dismantle();
 if(!Preview->IsVisible()||!Preview->GetStaticMesh())return;
 FCollisionQueryParams Q(NAME_None,false,this);Q.AddIgnoredActor(Player);if(auto* A=Furniture.FindRef(SelectedFurniture))Q.AddIgnoredActor(A);
 FHitResult H;At=BuildCamera->GetComponentLocation();ValidPlacement=false;
 if(GetWorld()->LineTraceSingleByChannel(H,At,At+BuildCamera->GetForwardVector()*5000,ECC_Visibility,Q)&&H.ImpactNormal.Z>.85f&&ALWWorld::IsSafePosition(H.ImpactPoint)){
  FVector Spot=H.ImpactPoint;const FBox Bounds=Preview->GetStaticMesh()->GetBoundingBox();FVector Scale=Preview->GetComponentScale();if(Grid){Spot.X=FMath::GridSnap(Spot.X,20.);Spot.Y=FMath::GridSnap(Spot.Y,20.);}Spot.Z-=Bounds.Min.Z*Scale.Z;Spot.Z+=3;
  Placement=FTransform(FRotator(0,Yaw,0),Spot,Scale);Preview->SetWorldTransform(Placement);
  FVector Ext=Bounds.GetExtent()*Scale;Ext=Ext.ComponentMax(FVector(3))-FVector(2);FVector Center=Placement.TransformPosition(Bounds.GetCenter());
  ValidPlacement=!GetWorld()->OverlapBlockingTestByChannel(Center,Placement.GetRotation(),ECC_Pawn,FCollisionShape::MakeBox(Ext),Q);
  if(FMath::Abs(Spot.X-(Center().X+1700))<260&&FMath::Abs(Spot.Y-Center().Y)<270)ValidPlacement=false;
  for(auto* A:World->BunkerParts)if(auto* O=Cast<ALWWorldObject>(A))if(O->Kind==ELWObjectKind::Door&&FVector::Dist(O->GetActorLocation(),Spot)<180)ValidPlacement=false;
  for(auto* A:Built)if(auto* O=Cast<ALWWorldObject>(A))if(O->Kind==ELWObjectKind::Door&&FVector::Dist(O->GetActorLocation(),Spot)<180)ValidPlacement=false;
  DrawDebugBox(GetWorld(),Center,Ext,Placement.GetRotation(),ValidPlacement?FColor::Green:FColor::Red,false,0,0,2);
 }
 if(PC->WasInputKeyJustPressed(EKeys::LeftMouseButton))Place();
}
bool ALWBunker45::Place(){if(!Player->BuildMode45||!ValidPlacement)return false;FName Id=SelectedFurniture;FLWPlaced45 V;
 if(Catalog().IsValidIndex(SelectedCatalog)){const auto& C=Catalog()[SelectedCatalog];if(!Player->ConsumeSupply(TEXT("scrap"),C.Cost)){Message=TEXT("Not enough scrap in inventory.");return false;}V.Model=C.Model;V.Use=C.Use;V.Cost=C.Cost;Id=FName(*(TEXT("bunker_custom45_")+FGuid::NewGuid().ToString(EGuidFormats::Digits)));}
 else {auto* A=Furniture.FindRef(Id);if(!IsValid(A))return false;if(auto* Existing=Player->Bunker45.Furniture.Find(Id))V=*Existing;else{V=Originals.FindRef(Id);if(V.Cost<=0)V.Cost=12;}if(auto* O=Cast<ALWWorldObject>(A))V.Use=O->UseType;}
 V.Removed=false;V.Transform=Placement;Player->Bunker45.Furniture.Add(Id,V);ApplyFurniture();Player->RequestSave40();Message=TEXT("Placed");return true;}
bool ALWBunker45::Dismantle(){auto* A=Furniture.FindRef(SelectedFurniture);if(!Player->BuildMode45||!IsValid(A)||A->IsHidden())return false;
 if(const auto* C=World->Containers.Find(SelectedFurniture);C&&!C->Items.IsEmpty()){Message=TEXT("Empty this storage before dismantling it.");return false;}
 auto& V=Player->Bunker45.Furniture.FindOrAdd(SelectedFurniture);if(V.Removed)return false;V.Transform=A->GetActorTransform();if(V.Cost<=0)V.Cost=Originals.Contains(SelectedFurniture)?Originals[SelectedFurniture].Cost:12;V.Removed=true;
 Player->GiveItem(TEXT("scrap"),FMath::Max(1,V.Cost/2));A->SetActorHiddenInGame(true);A->SetActorEnableCollision(false);Preview->SetVisibility(false);SelectedFurniture=NAME_None;Player->RequestSave40();Message=TEXT("Dismantled. Half the scrap recovered.");return true;}
bool ALWBunker45::BuyExpansion(FName Id){if(BuildQueue.Num()){Message=TEXT("Construction is still in progress.");return false;}int Cost=0;
 if(Id==TEXT("bedrooms")){if(Player->Bunker45.BedroomFloors>=9)return false;Cost=2000+Player->Bunker45.BedroomFloors*1000;}
 else if(Id==TEXT("garage")){if(Player->Bunker45.Garage)return false;Cost=12000;}
 else {const TArray<FName> Allowed={TEXT("trade"),TEXT("salvage"),TEXT("recruit"),TEXT("contracts"),TEXT("medical"),TEXT("farm")};if(!Allowed.Contains(Id)||Player->Bunker45.Utilities.Contains(Id))return false;Cost=3000;}
 if(Player->Money<Cost){Message=TEXT("Not enough credits.");return false;}Player->Money-=Cost;
 if(Id==TEXT("bedrooms"))++Player->Bunker45.BedroomFloors;else if(Id==TEXT("garage"))Player->Bunker45.Garage=true;else Player->Bunker45.Utilities.Add(Id);
 Player->RequestSave40();Rebuild();Open(1);Message=TEXT("Construction underway. Elevator access included.");return true;
}
