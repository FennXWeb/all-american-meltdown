#include "LWCharacter.h"
#include "LWHUD.h"
#include "LWWorld.h"
#include "LWWorldObject.h"
#include "LWZombie.h"
#include "LWWeaponMods.h"
#include "LWPlayerInput51.h"
#include "Camera/CameraComponent.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"

ALWWorldObject* ALWCharacter::QuickContainer54()const{
 if(!CanAct()||bAim||bReloading||Sliding54||!World||!Camera)return nullptr;
 // Re-trace on each action: cached focus can be up to 160 ms old.
 FCollisionQueryParams Q(SCENE_QUERY_STAT(LWQuickLoot54),false,this);FHitResult H;
 const FVector Start=Camera->GetComponentLocation();
 if(!GetWorld()->LineTraceSingleByChannel(H,Start,Start+Camera->GetForwardVector()*330,ECC_Visibility,Q))return nullptr;
 auto* O=Cast<ALWWorldObject>(H.GetActor());if(auto* Z=Cast<ALWZombie>(H.GetActor());Z&&Z->bDead)O=Z->LootProxy;
 if(!IsValid(O)||(O->Kind!=ELWObjectKind::Container&&O->Kind!=ELWObjectKind::Stash)||IsLocked(O))return nullptr;
 if(O->Kind==ELWObjectKind::Stash)return bSafehouse?O:nullptr;
 auto* R=World->Containers.Find(O->RecordId);return R&&!R->bTrader?O:nullptr;
}
int ALWCharacter::TakeAll54(){
 if(!bInventory||!IsValid(OpenObject)||OpenObject->Kind==ELWObjectKind::Trader||IsLocked(OpenObject))return 0;
 int Panel=OpenObject->Kind==ELWObjectKind::Stash?1:2;auto* Source=ItemsFor(Panel);if(!Source)return 0;
 if(Panel==2){auto* R=World->Containers.Find(OpenObject->RecordId);if(!R||R->bTrader)return 0;}
 TArray<FGuid> IDs;for(const auto& I:*Source)if(I.Slot!=TEXT("Loaded"))IDs.Add(I.Id);
 int Taken=0;for(FGuid Id:IDs)if(LWItems::QuickTransfer(*Source,Inventory,Id,12,LWItems::InventoryHeight(Inventory))){++Taken;RecordSettlementTransfer(Panel,0,0,false);}
 if(Taken){SyncAmmoHUD();World->Sound(TEXT("InventoryMove"),GetActorLocation());RequestSave40();}
 const bool Remains=Source->ContainsByPredicate([](const auto& I){return I.Slot!=TEXT("Loaded");});
 if(Remains)Notify(TEXT("INVENTORY FULL - SOME ITEMS LEFT"));else if(!Taken)Notify(TEXT("EMPTY"),1);
 return Taken;
}
bool ALWCharacter::TakeQuick54(bool All){
 auto* O=QuickContainer54();if(!O)return false;
 if(QuickRecord54!=O->RecordId){QuickRecord54=O->RecordId;QuickIndex54=0;}
 const auto Previous=OpenObject;OpenObject=O;bInventory=true;
 bool Result=false;if(All)Result=TakeAll54()>0;else{
  int Panel=O->Kind==ELWObjectKind::Stash?1:2;auto* Items=ItemsFor(Panel);TArray<FGuid> IDs;
  if(Items)for(const auto& I:*Items)if(I.Slot!=TEXT("Loaded"))IDs.Add(I.Id);
  if(IDs.Num()){QuickIndex54=FMath::Clamp(QuickIndex54,0,IDs.Num()-1);Result=QuickTransferItem(Panel,IDs[QuickIndex54]);}
 }
 bInventory=false;OpenObject=Previous;return Result;
}
void ALWCharacter::QuickLootAll54(){if(QuickContainer54())TakeQuick54(true);}
void ALWCharacter::QuickLootOpen54(){if(auto* O=QuickContainer54())OpenContainer(O);}
void ALWCharacter::QuickLootStep54(int D){
 auto* O=QuickContainer54();if(!O)return;if(QuickRecord54!=O->RecordId){QuickRecord54=O->RecordId;QuickIndex54=0;}
 const auto* R=World->Containers.Find(O->RecordId);const auto* Items=O->Kind==ELWObjectKind::Stash?&Stash:R?&R->Items:nullptr;
 int N=0;if(Items)for(const auto& I:*Items)N+=I.Slot!=TEXT("Loaded");QuickIndex54=N?(QuickIndex54+D+N)%N:0;
}
void ALWHUD::QuickLoot54(ALWCharacter* P){
 auto* O=P->QuickContainer54();if(!O){P->QuickRecord54=NAME_None;return;}
 if(P->QuickRecord54!=O->RecordId){P->QuickRecord54=O->RecordId;P->QuickIndex54=0;}
 auto* R=P->World->Containers.Find(O->RecordId);const auto* Items=O->Kind==ELWObjectKind::Stash?&P->Stash:R?&R->Items:nullptr;if(!Items)return;
 TArray<const FLWItemInstance*> List;for(const auto& I:*Items)if(I.Slot!=TEXT("Loaded"))List.Add(&I);
 P->QuickIndex54=FMath::Clamp(P->QuickIndex54,0,FMath::Max(0,List.Num()-1));
 const float X=Canvas->ClipX/Scale*.5f+65,Y=285;const FLinearColor Ink(.025,.035,.025,.9),Gold(.95,.72,.32),Bone(.85,.87,.73);
 const int Rows=FMath::Clamp(List.Num(),1,5);const float Footer=Y+42+Rows*26;
 Rect(X,Y,375,80+Rows*26,Ink);Text(R&&!R->Settlement.IsNone()?TEXT("CONTAINER / OWNED"):TEXT("CONTAINER"),X+12,Y+10,1,Gold);
 const int First=FMath::Max(0,P->QuickIndex54-3);
 for(int I=First;I<FMath::Min(First+5,List.Num());++I){float Row=Y+36+(I-First)*26;if(I==P->QuickIndex54)Rect(X+6,Row-3,363,24,FLinearColor(.18,.22,.13,.9));const auto& Item=*List[I];
  FString Name=Item.CustomName39.IsEmpty()?LWItems::Def(Item.Definition).DisplayName.ToString():Item.CustomName39;Name=Name.Left(31);if(Item.Count>1)Name+=FString::Printf(TEXT(" (%d)"),Item.Count);
  Text(Name,X+14,Row,.88f,LWItems::Def(Item.Definition).Category==TEXT("Weapon")?LWMods::TierColor(Item.WeaponTier):Bone);
 }
 if(List.IsEmpty())Text(TEXT("EMPTY"),X+14,Y+40,.9f,Bone);
 auto Key=[&](FKey Logical){auto* K=ULWPlayerInput51::Get51(this);if(K){K->Load51();for(const auto& B:K->Bindings)if(B.Logical==Logical)return B.Physical.GetDisplayName().ToString().ToUpper();}return Logical.GetDisplayName().ToString().ToUpper();};
 Text(Key(EKeys::E)+TEXT(" TAKE  ")+Key(EKeys::Y)+TEXT(" TAKE ALL  ")+Key(EKeys::L)+TEXT(" TRANSFER"),X+12,Footer,.8f,Gold);
 Text(TEXT("WHEEL SELECT"),X+12,Footer+20,.7f,Bone);
}
