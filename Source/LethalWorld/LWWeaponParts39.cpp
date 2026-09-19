#include "LWWeaponParts39.h"
#include "LWWeaponMods.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
namespace LWParts39 {
FName WeaponId(int32 I){static const FName Names[]={TEXT("crowbar"),TEXT("bat"),TEXT("shotgun"),TEXT("revolver"),TEXT("sniper"),TEXT("smg"),TEXT("rifle"),TEXT("lmg"),TEXT("doublebarrel"),TEXT("missile_launcher"),TEXT("minigun"),TEXT("sawedoff"),TEXT("desert_eagle"),TEXT("m4"),TEXT("taser"),TEXT("flamethrower"),TEXT("tenbarrel"),TEXT("giant_glock"),TEXT("questionable_ak"),TEXT("finger_guns"),TEXT("budget_cut")};return I>=0&&I<21?Names[I]:NAME_None;}
const TArray<FDefinition>& Definitions(){static const TArray<FDefinition> D=[](){TArray<FDefinition> Out;
#include "LWPartDefinitions39.inl"
 const TCHAR* Meshes[]={TEXT("TenBarrel50"),TEXT("GiantGlock50"),TEXT("QuestionableAK50"),TEXT("FingerGuns50"),TEXT("BudgetCut50")};
 const TCHAR* Names[]={TEXT("Ten-barrel receiver"),TEXT("Giant Glock frame"),TEXT("Questionable AK receiver"),TEXT("Finger gun hand"),TEXT("Budget plastic receiver")};
 for(int W=16;W<21;++W)Out.Add({FName(*FString::Printf(TEXT("part_action_%02d"),W)),Names[W-16],TEXT("Action"),Meshes[W-16],WeaponId(W),FVector(75,20,30),W==17?FVector(66,0,8):W==18?FVector(68,0,3):FVector(34,0,3),0,0,0,250});
 Out.Add({TEXT("part_barrel_16"),TEXT("Ten barrel cluster"),TEXT("Barrel"),TEXT("TenBarrels50"),NAME_None,FVector(72,21,12),FVector(72,0,3),0,0,0,500});
 Out.Add({TEXT("part_barrel_20"),TEXT("Flexible plastic barrel"),TEXT("Barrel"),TEXT("BudgetBarrel50"),NAME_None,FVector(42,8,8),FVector(41,0,0),0,0,0,50});
 Out.Add({TEXT("part_feed_17"),TEXT("Giant magazine housing"),TEXT("Feed"),TEXT("GiantMag50"),NAME_None,FVector(9,9,23),FVector::ZeroVector,0,0,0,80});
 Out.Add({TEXT("part_feed_18"),TEXT("Extended AK magazine housing"),TEXT("Feed"),TEXT("AKMag50"),NAME_None,FVector(17,6,33),FVector::ZeroVector,0,0,0,80});
 return Out;}();return D;}
const FDefinition* Find(FName Id){return Definitions().FindByPredicate([&](const auto& D){return D.Id==Id;});}
TArray<FLWItemDefinition> Items(){TArray<FLWItemDefinition> Out;for(const auto& P:Definitions()){
 FLWItemDefinition D;D.Id=P.Id;D.DisplayName=FText::FromString(P.Name);D.Category=TEXT("WeaponPart");D.Width=P.Group==TEXT("Barrel")?3:P.Group==TEXT("Action")?2:1;D.Height=P.Group==TEXT("Action")?2:1;D.Price=P.Price;
 D.Mesh=TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(FString::Printf(TEXT("/Game/Art/Meshes/SM_%s.SM_%s"),*P.Mesh.ToString(),*P.Mesh.ToString())));Out.Add(D);
 }return Out;}
FName Mesh(FName D){if(const auto* P=Find(D))return P->Mesh;if(LWItems::Def(D).Category==TEXT("Attachment"))return FName(*(D.ToString()+TEXT("V14")));return NAME_None;}
FLWAssemblyPart39 Part(const FLWItemInstance& I){FLWAssemblyPart39 P;P.Id=I.Id;P.Definition=I.Definition;P.Tier=I.WeaponTier;P.Modifier=I.LegendaryModifier;P.Skin=I.WeaponSkin;P.Durability=I.Durability;P.Rounds=I.Rounds;P.Chamber=I.Chamber;P.Cylinder=I.Cylinder;P.CylinderIndex=I.CylinderIndex;P.Transform=I.PartTransform39;return P;}
FLWItemInstance Item(const FLWAssemblyPart39& P){auto I=LWItems::Make(P.Definition);I.Id=P.Id;I.WeaponTier=P.Tier;I.LegendaryModifier=P.Modifier;I.WeaponSkin=P.Skin;I.Durability=P.Durability;I.Rounds=P.Rounds;I.Chamber=P.Chamber;I.Cylinder=P.Cylinder;I.CylinderIndex=P.CylinderIndex;I.PartTransform39=P.Transform;return I;}
TArray<FLWAssemblyPart39> Recipe(const FLWItemInstance& G){
 if(!G.Parts39.IsEmpty())return G.Parts39;
 TArray<FLWAssemblyPart39> Out;const int W=LWItems::Def(G.Definition).WeaponIndex;if(W<0||W>=21)return Out;
 auto Add=[&](FName Id,FVector Pos){auto I=LWItems::Make(Id);I.WeaponTier=G.WeaponTier;I.LegendaryModifier=LWMods::Modifier(&G);I.WeaponSkin=G.WeaponSkin;I.Durability=G.Durability;auto P=Part(I);P.Transform.SetLocation(Pos);Out.Add(P);};
 Add(FName(*FString::Printf(TEXT("part_action_%02d"),W)),FVector::ZeroVector);
 if(W>=16){
  if(W==16)Add(TEXT("part_barrel_16"),FVector(10,0,0));if(W==20)Add(TEXT("part_barrel_20"),FVector(24,0,3));
  if(W==17)Add(TEXT("part_feed_17"),FVector(-7,0,-12));if(W==18)Add(TEXT("part_feed_18"),FVector(10,0,-5));
  for(const auto& A:G.Attachments){auto P=Part(LWItems::Make(A.Value));P.Transform.SetLocation(LWMods::Position(W,A.Key));Out.Add(P);}return Out;
 }
 if(W>=2)Add(FName(*FString::Printf(TEXT("part_barrel_%02d"),W)),FVector(18,0,3));
 static const int Stock[]={-1,-1,4,-1,6,2,1,7,5,-1,7,5,-1,0,-1,3};
 static const int Grip[]={4,5,2,1,2,0,0,2,2,2,2,2,3,0,0,2};
 static const int Feed[]={-1,-1,0,1,2,2,2,4,3,5,4,3,2,2,5,5};
 if(Stock[W]>=0)Add(FName(*FString::Printf(TEXT("part_stock_%02d"),Stock[W])),FVector(-14,0,0));
 Add(FName(*FString::Printf(TEXT("part_grip_%02d"),Grip[W])),FVector(W<2?-8:0,0,-3));
 if(Feed[W]>=0)Add(FName(*FString::Printf(TEXT("part_feed_%02d"),Feed[W])),FVector(8,0,W==3?2:-5));
 for(const auto& A:G.Attachments){auto P=Part(LWItems::Make(A.Value));P.Transform.SetLocation(LWMods::Position(W,A.Key));Out.Add(P);}
 return Out;
}
bool Has(const FLWItemInstance* G,FName Id){return G&&G->Parts39.ContainsByPredicate([&](const auto& P){return P.Definition==Id;});}
float Bonus(const FLWItemInstance* G,int Stat){float V=0;if(G)for(const auto& P:G->Parts39)if(const auto* D=Find(P.Definition))V+=(Stat==0?D->Damage:Stat==1?D->Handling:D->Speed)*(1+.2f*FMath::Clamp(P.Tier,0,4));return FMath::Clamp(V,Stat==0?-.5f:-.35f,Stat==0?2.f:.65f);}
void Derive(FLWItemInstance& G){G.WeaponTier=0;G.LegendaryModifier=NAME_None;G.Attachments.Empty();for(const auto& P:G.Parts39){G.WeaponTier=FMath::Max(G.WeaponTier,P.Tier);if(P.Tier==4&&G.LegendaryModifier.IsNone())G.LegendaryModifier=P.Modifier;if(auto* D=Find(P.Definition);D&&P.Id==G.Action39)G.Definition=D->Weapon;}}
bool Validate(const FLWItemInstance& G,FString& Error){
 if(G.Parts39.IsEmpty()||G.Parts39.Num()>64){Error=TEXT("Use between 1 and 64 parts.");return false;}
 TSet<FGuid> Ids;bool Action=false,Barrel=false;
 for(const auto& P:G.Parts39){const auto* D=Find(P.Definition);if((!D&&LWItems::Def(P.Definition).Category!=TEXT("Attachment"))||!P.Id.IsValid()||Ids.Contains(P.Id)){Error=TEXT("Unknown or duplicate part.");return false;}Ids.Add(P.Id);
 const FVector T=P.Transform.GetLocation(),S=P.Transform.GetScale3D();if(P.Transform.ContainsNaN()||!P.Transform.GetRotation().IsNormalized()||T.GetAbsMax()>300||S.GetMin()<.25||S.GetMax()>3||P.Tier<0||P.Tier>4){Error=TEXT("Part transform is outside the bench's 6m workspace.");return false;}
 if(D&&(D->Group==TEXT("Barrel")||(D->Group==TEXT("Action")&&(D->Weapon==TEXT("giant_glock")||D->Weapon==TEXT("questionable_ak")||D->Weapon==TEXT("finger_guns")))))Barrel=true;if(P.Id==G.Action39&&D&&D->Group==TEXT("Action")&&D->Weapon==G.Definition)Action=true;
 }
 if(!Action){Error=TEXT("Choose an action to power this weapon.");return false;}
 if(LWItems::Def(G.Definition).WeaponIndex>=2&&!Barrel){Error=TEXT("Add a barrel or emitter. Any type can be used.");return false;}
 Error.Empty();return true;
}
bool ReleaseMagazine(TArray<FLWItemInstance>& Inventory,FLWItemInstance& G,FString& Error){
 if(!G.LoadedMagazine.IsValid())return true;const auto* Found=Inventory.FindByPredicate([&](const auto& I){return I.Id==G.LoadedMagazine&&I.Slot==TEXT("Loaded");});if(!Found){Error=TEXT("The loaded magazine is missing.");return false;}
 auto Next=Inventory;auto Mag=*Found;Next.RemoveAll([&](const auto& I){return I.Id==Mag.Id;});Mag.Slot=NAME_None;
 if(!LWItems::Place(Next,Mag,12,LWItems::InventoryHeight(Next))){Error=TEXT("Make room for the loaded magazine.");return false;}Inventory=MoveTemp(Next);G.LoadedMagazine.Invalidate();return true;
}
bool Store(TArray<FLWItemInstance>& Inventory,FLWItemInstance G,FString& Error){
 Derive(G);if(!Validate(G,Error))return false;
 if(Inventory.ContainsByPredicate([&](const auto& I){if(I.Id==G.Id)return true;for(const auto& P:G.Parts39){if(P.Id==I.Id)return true;for(const auto& Q:I.Parts39)if(P.Id==Q.Id)return true;}return false;})){Error=TEXT("A part is already owned outside this assembly.");return false;}
 auto Next=Inventory;if(!G.Slot.IsNone()&&LWItems::CanEquip(G,G.Slot)&&!Next.ContainsByPredicate([&](const auto& I){return I.Slot==G.Slot;})){Next.Add(G);}else{G.Slot=NAME_None;if(!LWItems::Place(Next,G,12,LWItems::InventoryHeight(Next))){Error=TEXT("Make room for the assembled weapon.");return false;}}
 Inventory=MoveTemp(Next);return true;
}
bool Dismantle(TArray<FLWItemInstance>& Inventory,FGuid Id,FString& Error){
 const auto* Found=Inventory.FindByPredicate([&](const auto& I){return I.Id==Id&&LWItems::Def(I.Definition).Category==TEXT("Weapon");});if(!Found){Error=TEXT("Select a weapon to dismantle.");return false;}auto G=*Found;auto Next=Inventory;Next.RemoveAll([&](const auto& I){return I.Id==Id;});if(!ReleaseMagazine(Next,G,Error))return false;
 auto Parts=Recipe(G);if(Parts.IsEmpty())return false;const FGuid Active=G.Action39.IsValid()?G.Action39:Parts[0].Id;
 for(auto& P:Parts){if(P.Id==Active){P.Rounds=G.Rounds;P.Chamber=G.Chamber;P.Cylinder=G.Cylinder;P.CylinderIndex=G.CylinderIndex;}auto I=Item(P);if(!LWItems::Place(Next,I,12,LWItems::InventoryHeight(Next))){Error=TEXT("Make room for the recovered parts.");return false;}}
 Inventory=MoveTemp(Next);Error.Empty();return true;
}
uint32 Fingerprint(const TArray<FLWItemInstance>& Items){TArray<uint8> Bytes;FMemoryWriter W(Bytes);FObjectAndNameAsStringProxyArchive A(W,false);A.ArIsSaveGame=true;for(const auto& I:Items)FLWItemInstance::StaticStruct()->SerializeItem(A,const_cast<FLWItemInstance*>(&I),nullptr);return FCrc::MemCrc32(Bytes.GetData(),Bytes.Num());}
TArray<FTransform> Barrels(const FLWItemInstance& G){TArray<FTransform> Out;for(const auto& P:G.Parts39)if(const auto* D=Find(P.Definition);D&&(D->Group==TEXT("Barrel")||(D->Group==TEXT("Action")&&(D->Weapon==TEXT("giant_glock")||D->Weapon==TEXT("questionable_ak")||D->Weapon==TEXT("finger_guns"))))){FTransform T=P.Transform;T.SetLocation(T.TransformPosition(D->Muzzle));Out.Add(T);}return Out;}
}
