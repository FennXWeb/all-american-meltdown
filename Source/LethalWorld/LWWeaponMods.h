#pragma once
#include "LWInventory.h"
#include "LWWeaponParts39.h"
namespace LWMods {
inline int Tier(const FLWItemInstance* G){return G?FMath::Clamp(G->WeaponTier,0,4):0;}
inline const TCHAR* TierName(int T){static const TCHAR* Names[]={TEXT("COMMON"),TEXT("UNCOMMON"),TEXT("RARE"),TEXT("EPIC"),TEXT("LEGENDARY")};return Names[FMath::Clamp(T,0,4)];}
inline FLinearColor TierColor(int T){static const FLinearColor C[]={FLinearColor(.86f,.86f,.82f),FLinearColor(.28f,.85f,.36f),FLinearColor(.25f,.58f,1.f),FLinearColor(.76f,.38f,1.f),FLinearColor(1.f,.70f,.18f)};return C[FMath::Clamp(T,0,4)];}
inline FName Modifier(const FLWItemInstance* G){static const FName Types[]={TEXT("Two Shot"),TEXT("Life Steal"),TEXT("Toxic"),TEXT("Rapid Fire"),TEXT("Recoil Free"),TEXT("Explosive")};if(Tier(G)!=4)return NAME_None;for(auto N:Types)if(G->LegendaryModifier==N)return N;return Types[GetTypeHash(G->Id)%6];}
inline bool Effect(const FLWItemInstance* G,FName N){if(G&&!G->Parts39.IsEmpty())return G->Parts39.ContainsByPredicate([&](const auto& P){return P.Tier==4&&P.Modifier==N;});return Modifier(G)==N;}
inline float Interval(const FLWItemInstance* G){return (Effect(G,TEXT("Rapid Fire"))?.4f:1.f)*(1-LWParts39::Bonus(G,2));}
inline int Shots(const FLWItemInstance* G){return Effect(G,TEXT("Two Shot"))?2:1;}
inline void RollFinish(FLWItemInstance& G,FRandomStream& R){G.LegendaryModifier=NAME_None;G.WeaponSkin=-1;if(Tier(&G)==4){static const FName Types[]={TEXT("Two Shot"),TEXT("Life Steal"),TEXT("Toxic"),TEXT("Rapid Fire"),TEXT("Recoil Free"),TEXT("Explosive")};G.LegendaryModifier=Types[R.RandRange(0,5)];}if(Tier(&G)>=2&&R.FRand()<.45f)G.WeaponSkin=R.RandRange(0,9);}
inline FString SkinName(const FLWItemInstance* G){static const TCHAR* Names[]={TEXT("Shatter"),TEXT("Circuit"),TEXT("Tiger"),TEXT("Topographic"),TEXT("Hazard"),TEXT("Hex"),TEXT("Marble"),TEXT("Stars"),TEXT("Ember"),TEXT("Stripes")};return G&&Tier(G)>=2&&G->WeaponSkin>=0&&G->WeaponSkin<10?FString(TierName(Tier(G)))+TEXT(" ")+Names[G->WeaponSkin]:FString();}
inline FName Mount(FName A){if(A==TEXT("att_light"))return TEXT("Light");if(A==TEXT("att_laser"))return TEXT("Laser");if(A==TEXT("att_vertical")||A==TEXT("att_angled"))return TEXT("Grip");if(A==TEXT("att_reflex")||A==TEXT("att_holo")||A==TEXT("att_scope4")||A==TEXT("att_scope8"))return TEXT("Optic");return NAME_None;}
inline bool Compatible(FName Gun,FName A){int W=LWItems::Def(Gun).WeaponIndex;FName M=Mount(A);if(W<2||M.IsNone()||W==16||W==19||W==20)return false;if(W==3)return M==TEXT("Optic")&&A!=TEXT("att_scope8");if(W==8||W==9||W==10||W==11||W==14||W==15)return false;if(W==12)return M==TEXT("Optic")&&(A==TEXT("att_reflex")||A==TEXT("att_holo"));if(W==2)return M!=TEXT("Grip")&&A!=TEXT("att_scope8");if(A==TEXT("att_scope8"))return W==4||W==6||W==7||W==13;return true;}
inline bool Has(const FLWItemInstance* G,FName A){if(LWParts39::Has(G,A))return true;return G&&G->Attachments.FindRef(Mount(A))==A&&Compatible(G->Definition,A);}
inline float Damage(const FLWItemInstance* G){return (1.f+.12f*Tier(G))*(1+LWParts39::Bonus(G,0));}
inline float Spread(const FLWItemInstance* G,bool Aim){return (1-LWParts39::Bonus(G,1))*(1-.06f*Tier(G))*(Has(G,TEXT("att_laser"))&&!Aim?.7f:1)*(Has(G,TEXT("att_angled"))?.85f:1);}
inline float Recoil(const FLWItemInstance* G){return (Effect(G,TEXT("Recoil Free"))?0.f:1.f)*(1-LWParts39::Bonus(G,1))*(1-.06f*Tier(G))*(Has(G,TEXT("att_vertical"))?.72f:Has(G,TEXT("att_angled"))?.85f:1);}
inline float Fov(const FLWItemInstance* G){return Has(G,TEXT("att_scope8"))?11.f:Has(G,TEXT("att_scope4"))?22.f:Has(G,TEXT("att_holo"))?48.f:Has(G,TEXT("att_reflex"))?55.f:58.f;}
inline bool OpenSight(const FLWItemInstance* G){return Has(G,TEXT("att_reflex"))||Has(G,TEXT("att_holo"));}
// Authored in make_models_v14.py: base .8cm + half the window height.
inline float SightHeight(const FLWItemInstance* G){return Has(G,TEXT("att_holo"))?3.4f:2.85f;}
inline FVector Position(int W,FName M){const float Top[]={0,0,7,8,7.6f,6.7f,7.2f,10};const float Fore[]={0,0,70,26,84,32,59,75};W=W==18?6:W==17?3:W==13?6:W==12?3:FMath::Clamp(W,0,7);return M==TEXT("Optic")?FVector(W==3?13:8,0,Top[W]):FVector(M==TEXT("Grip")?Fore[W]*.52f:Fore[W],M==TEXT("Light")?5:M==TEXT("Laser")?-5:0,M==TEXT("Grip")?-3:2);}
inline void AlignOpenSight(const FLWItemInstance* G,int W,const FTransform& Mesh,FVector& Rest,FRotator& Rotation){
 FVector Local=Position(W,TEXT("Optic"))+FVector(0,0,SightHeight(G));if(G)for(const auto& P:G->Parts39)if(Mount(P.Definition)==TEXT("Optic")){Local=P.Transform.TransformPosition(FVector(0,0,SightHeight(G)));break;}const FVector Sight=Mesh.TransformPosition(Local);
 Rotation=Mesh.GetRotation().Inverse().Rotator();
 const FVector Aligned=Rotation.RotateVector(Sight);Rest.Y=-Aligned.Y;Rest.Z=-Aligned.Z;
}
inline int32 Price(const FLWItemInstance& I){int64 V=int64(LWItems::Def(I.Definition).Price)*I.Count;if(!I.Parts39.IsEmpty()){V=0;for(const auto& P:I.Parts39)V+=Price(LWParts39::Item(P));return int32(FMath::Min<int64>(V,MAX_int32));}if(LWItems::Def(I.Definition).Category==TEXT("WeaponPart"))V=V*(100+Tier(&I)*35)/100;if(LWItems::Def(I.Definition).Category==TEXT("Weapon")){V=V*(100+Tier(&I)*35)/100;for(const auto& A:I.Attachments)if(Compatible(I.Definition,A.Value))V+=LWItems::Def(A.Value).Price;}return int32(FMath::Clamp<int64>(V,0,MAX_int32));}
inline bool Detach(TArray<FLWItemInstance>& Items,FGuid Id,FName M){auto Next=Items;auto* G=Next.FindByPredicate([&](const auto& I){return I.Id==Id;});if(!G)return false;FName A=G->Attachments.FindRef(M);if(A.IsNone())return false;G->Attachments.Remove(M);auto Removed=LWItems::Make(A);if(!LWItems::Place(Next,Removed,12,LWItems::InventoryHeight(Next)))return false;Items=MoveTemp(Next);return true;}
}
