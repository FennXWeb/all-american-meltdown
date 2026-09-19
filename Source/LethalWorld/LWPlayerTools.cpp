#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWWorldObject.h"
#include "Components/SceneComponent.h"
void ALWCharacter::SwitchTab(int32 Tab){if(Workbench39)return;if(bStoryLocked||!bStarted||bMenu||Health<=0||Tab<0||Tab>5)return;auto Storage=OpenObject;ClosePanels();OpenObject=IsValid(Storage)?Storage:nullptr;bInventory=Tab==0;bMap=Tab==1;RPGPanel=Tab==5?6:Tab>=2?Tab-1:0;CancelReload();bAim=bSprint=false;SetMenuInput(true);}
void ALWCharacter::EquipSlot(FName Slot){if(!CanAct())return;const auto* I=Inventory.FindByPredicate([Slot](const auto& V){return V.Slot==Slot;});if(!I){if(Slot==TEXT("Melee")){CancelReload();StopAttack();ActiveWeaponId.Invalidate();AttackTimer=AttackDuration=0;ConfigureWeaponParts();ConfigureAttachments();Notify(TEXT("UNARMED"));}else Notify(TEXT("SLOT EMPTY"));return;}ActiveWeaponId=I->Id;Equip(LWItems::Def(I->Definition).WeaponIndex);}
bool ALWCharacter::QuickTransferItem(int32 From,FGuid Id){if(!bInventory||!IsValid(OpenObject))return false;int To=From==0?(OpenObject->Kind==ELWObjectKind::Stash?1:2):0;auto* A=ItemsFor(From);auto* B=ItemsFor(To);if(!A||!B)return false;int W=12,H=To==0?LWItems::InventoryHeight(Inventory):14;if(To==2){auto* R=World->Containers.Find(OpenObject->RecordId);if(!R)return false;W=R->Width;H=R->Height;}
 if(OpenObject->Kind==ELWObjectKind::Trader){auto* I=A->FindByPredicate([Id](const auto& V){return V.Id==Id;});if(!I)return false;for(int R=0;R<2;R++)for(int Y=0;Y<H;Y++)for(int X=0;X<W;X++)if(LWItems::Fits(*B,*I,X,Y,R!=0,W,H))return MoveItem(From,To,Id,X,Y,R!=0);return false;}
 const auto BeforeA=*A,BeforeB=*B;
 if(!LWItems::QuickTransfer(*A,*B,Id,W,H)){Notify(TEXT("NOT ENOUGH SPACE"));return false;}if(!LWItems::ValidateEquipment(Inventory)){*A=BeforeA;*B=BeforeB;Notify(TEXT("EMPTY EXTRA BACKPACK ROWS / RIG WEAPON SLOT FIRST"));return false;}if(!FindItem(ActiveWeaponId)){ActiveWeaponId.Invalidate();WeaponRoot->SetVisibility(false,true);}RecordSettlementTransfer(From,To,0,false);SyncAmmoHUD();World->Sound(TEXT("InventoryMove"),GetActorLocation());RequestSave40();return true;
}
bool ALWCharacter::SortInventory(int32 Panel){if(!bInventory)return false;auto* Items=ItemsFor(Panel);if(!Items)return false;if(Panel==2&&OpenObject->Kind==ELWObjectKind::Trader)return false;int W=12,H=Panel==0?LWItems::InventoryHeight(Inventory):14;if(Panel==2){const auto& R=World->Containers.FindChecked(OpenObject->RecordId);W=R.Width;H=R.Height;}bool OK=LWItems::AutoSort(*Items,W,H);if(OK){World->Sound(TEXT("InventoryMove"),GetActorLocation());RequestSave40();}return OK;}

void ALWCharacter::TogglePlayerMenu(){if(bInventory||bMap||((RPGPanel>0&&RPGPanel<4)||RPGPanel==6))ClosePanels();else SwitchTab(0);}
