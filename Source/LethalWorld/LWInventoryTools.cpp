#include "LWInventory.h"
namespace LWItems {
bool StackCompatible(const FLWItemInstance& A,const FLWItemInstance& B){return A.Id!=B.Id&&A.Definition==B.Definition&&Def(A.Definition).MaxStack>1&&A.Slot.IsNone()&&B.Slot.IsNone()&&A.WeaponTier==B.WeaponTier&&A.LegendaryModifier==B.LegendaryModifier&&A.WeaponSkin==B.WeaponSkin&&A.Attachments.IsEmpty()&&B.Attachments.IsEmpty()&&A.Durability==B.Durability&&A.VehicleId==B.VehicleId&&A.Rounds==B.Rounds&&A.Chamber==B.Chamber&&A.Cylinder==B.Cylinder&&A.CylinderIndex==B.CylinderIndex&&!A.LoadedMagazine.IsValid()&&!B.LoadedMagazine.IsValid();}
bool AutoSort(TArray<FLWItemInstance>& Items,int32 W,int32 H){
 auto Merged=Items;
 for(int I=0;I<Merged.Num();I++)for(int J=I+1;J<Merged.Num();J++)if(StackCompatible(Merged[I],Merged[J])){int N=FMath::Min(Merged[J].Count,Def(Merged[I].Definition).MaxStack-Merged[I].Count);Merged[I].Count+=N;Merged[J].Count-=N;}
 Merged.RemoveAll([](const auto& I){return I.Count<=0;});
 TArray<FLWItemInstance> Grid,Fixed;for(const auto& I:Merged)(I.Slot.IsNone()?Grid:Fixed).Add(I);
 for(int Pass=0;Pass<2;Pass++){
 Grid.Sort([Pass](const auto& A,const auto& B){const auto& DA=Def(A.Definition);const auto& DB=Def(B.Definition);int AA=Pass?FMath::Max(DA.Width,DA.Height):Footprint(A.Definition).Num(),BB=Pass?FMath::Max(DB.Width,DB.Height):Footprint(B.Definition).Num();if(AA!=BB)return AA>BB;if(DA.Category!=DB.Category)return DA.Category.LexicalLess(DB.Category);if(A.Definition!=B.Definition)return A.Definition.LexicalLess(B.Definition);return A.Id.ToString()<B.Id.ToString();});
 auto Packed=Fixed;bool FitsAll=true;for(auto I:Grid)if(!Place(Packed,I,W,H)){FitsAll=false;break;}if(FitsAll){Items=MoveTemp(Packed);return true;}}
 // Dense irregular layouts may not repack greedily; merging at original positions is always safe.
 Items=MoveTemp(Merged);return true;
}
bool QuickTransfer(TArray<FLWItemInstance>& From,TArray<FLWItemInstance>& To,FGuid Id,int32 W,int32 H){
 if(&From==&To)return false;auto A=From,B=To;auto* Item=A.FindByPredicate([Id](const auto& I){return I.Id==Id;});if(!Item||Item->Slot==TEXT("Loaded"))return false;
 for(auto& Other:B)if(StackCompatible(*Item,Other)){int N=FMath::Min(Item->Count,Def(Other.Definition).MaxStack-Other.Count);Other.Count+=N;Item->Count-=N;}
 if(Item->Count==0)A.RemoveAll([Id](const auto& I){return I.Id==Id;});else{
 bool Done=false;for(int R=0;R<2&&!Done;R++)for(int Y=0;Y<H&&!Done;Y++)for(int X=0;X<W&&!Done;X++)if(Fits(B,*Item,X,Y,R!=0,W,H))Done=Transfer(A,B,Id,X,Y,R!=0,W,H);
 if(!Done)return false;}
 From=MoveTemp(A);To=MoveTemp(B);return true;
}
}

int32 LWItems::BackpackRows(FName Id){
 if(Id==TEXT("sling_pack"))return 2;if(Id==TEXT("backpack"))return 4;
 if(Id==TEXT("hiking_pack"))return 6;if(Id==TEXT("military_pack"))return 8;if(Id==TEXT("expedition_pack"))return 10;return 0;
}
int32 LWItems::InventoryHeight(const TArray<FLWItemInstance>& Items){
 for(const auto& I:Items)if(I.Slot==TEXT("Backpack")&&I.Count==1)return 10+BackpackRows(I.Definition);return 10;
}
bool LWItems::ValidateEquipment(const TArray<FLWItemInstance>& Items){
 const int H=InventoryHeight(Items);const bool Rig=Items.ContainsByPredicate([](const auto& I){return I.Slot==TEXT("Rig")&&I.Definition==TEXT("rig");});
 for(const auto& I:Items){if(I.Slot==TEXT("RigPrimary")&&!Rig)return false;if(I.Slot.IsNone()&&!Fits(Items,I,I.X,I.Y,I.bRotated,12,H,I.Id))return false;}return true;
}
