#include "LWArsenal62.h"
#include "LWCharacter.h"
#include "LWWeaponMods.h"
#include "LWWeaponParts39.h"
namespace LWArsenal62 {
const TArray<FAttachment>& Attachments(){static const TArray<FAttachment> A={
 {TEXT("att_micro63"),TEXT("Optic"),TEXT("Micro reflex"),TEXT("Wide view / 1x dot"),TEXT("No magnification"),1,1,1,1},
 {TEXT("att_tube63"),TEXT("Optic"),TEXT("Enclosed red dot"),TEXT("1.5x circle-dot"),TEXT("Narrower peripheral view"),1,1,1,1},
 {TEXT("att_prism63"),TEXT("Optic"),TEXT("Prism 3x"),TEXT("3x chevron reticle"),TEXT("Reduced peripheral view"),1,1,1,1},
 {TEXT("att_combat63"),TEXT("Optic"),TEXT("Combat scope 6x"),TEXT("6x range ladder"),TEXT("Poor close-range view"),1,1,1,1},
 {TEXT("att_marksman63"),TEXT("Optic"),TEXT("Marksman scope 12x"),TEXT("12x precision mil-dot"),TEXT("Very narrow view"),1,1,1,1},
 {TEXT("att_suppressor62"),TEXT("Muzzle"),TEXT("Suppressor"),TEXT("Recoil -10%"),TEXT("Damage -5%"),.9f,1,.95f,1},
 {TEXT("att_brake62"),TEXT("Muzzle"),TEXT("Competition brake"),TEXT("Recoil -25%"),TEXT("Hip spread +8%"),.75f,1.08f,1,1},
 {TEXT("att_comp62"),TEXT("Muzzle"),TEXT("Compensator"),TEXT("Recoil -15%"),TEXT("Fire interval +5%"),.85f,1,1,1.05f},
 {TEXT("att_heavy62"),TEXT("Barrel"),TEXT("Heavy match barrel"),TEXT("Spread -20%, damage +5%"),TEXT("Fire interval +10%"),1,.8f,1.05f,1.1f},
 {TEXT("att_short62"),TEXT("Barrel"),TEXT("Compact barrel"),TEXT("Fire interval -8%"),TEXT("Spread +15%, damage -5%"),1,1.15f,.95f,.92f},
 {TEXT("att_fluted62"),TEXT("Barrel"),TEXT("Fluted barrel"),TEXT("Spread -10%"),TEXT("Recoil +8%"),1.08f,.9f,1,1},
 {TEXT("att_precision62"),TEXT("Stock"),TEXT("Precision stock"),TEXT("Spread -15%, recoil -10%"),TEXT("Fire interval +8%"),.9f,.85f,1,1.08f},
 {TEXT("att_lightstock62"),TEXT("Stock"),TEXT("Lightweight stock"),TEXT("Fire interval -6%"),TEXT("Recoil +15%"),1.15f,1,1,.94f},
 {TEXT("att_padded62"),TEXT("Stock"),TEXT("Padded stock"),TEXT("Recoil -18%"),TEXT("Spread +5%"),.82f,1.05f,1,1},
 {TEXT("att_tape62"),TEXT("RearGrip"),TEXT("Stippled grip tape"),TEXT("Spread -8%"),TEXT("Recoil +4%"),1.04f,.92f,1,1},
 {TEXT("att_rubber62"),TEXT("RearGrip"),TEXT("Rubber grip"),TEXT("Recoil -12%"),TEXT("Fire interval +4%"),.88f,1,1,1.04f},
 {TEXT("att_skeleton62"),TEXT("RearGrip"),TEXT("Skeleton grip"),TEXT("Fire interval -5%"),TEXT("Spread +8%"),1,1.08f,1,.95f}
 };return A;}
const FAttachment* Find(FName Id){return Attachments().FindByPredicate([&](const auto& A){return A.Id==Id;});}
const TCHAR* BaseMesh(int W){static const TCHAR* M[]={TEXT("Crowbar"),TEXT("Bat"),TEXT("Shotgun"),TEXT("Revolver"),TEXT("SniperBareV14"),TEXT("SMG"),TEXT("Rifle"),TEXT("LMG"),TEXT("DoubleBarrelV18"),TEXT("MissileLauncher24"),TEXT("Minigun24"),TEXT("SawedOff24"),TEXT("DesertEagle24"),TEXT("M424"),TEXT("Taser24"),TEXT("Flamethrower24"),TEXT("TenBarrel50"),TEXT("GiantGlock50"),TEXT("QuestionableAK50"),TEXT("FingerGuns50"),TEXT("BudgetCut50")};return M[FMath::Clamp(W,0,20)];}
TArray<TPair<FName,FVector>> MovingMeshes(int W){
 const TCHAR* Names[]={nullptr,nullptr,nullptr,TEXT("RevolverCylinder"),TEXT("SniperBolt"),TEXT("SMGMag"),TEXT("RifleMag"),TEXT("LMGCover"),TEXT("DoubleBarrelBarrelsV18"),TEXT("Missile24"),TEXT("MinigunBarrels24"),TEXT("SawedOffBarrels24"),TEXT("DeagleMag24"),TEXT("RifleMag"),TEXT("TaserCartridge24"),TEXT("FuelTank24"),TEXT("TenBarrels50"),TEXT("GiantMag50"),TEXT("AKMag50"),nullptr,TEXT("BudgetBarrel50")};
 const FVector At[]={FVector::ZeroVector,FVector::ZeroVector,FVector::ZeroVector,FVector(8,0,4.5),FVector(2,0,4),FVector(8.4,0,-4.7),FVector(9.7,0,-5.2),FVector(17.3,0,6.8),FVector(10,0,0),FVector(6,0,3),FVector(20,0,0),FVector(10,0,0),FVector(-3,0,-6),FVector(9.7,0,-5.2),FVector(13,0,3),FVector(6,-9,-8),FVector(10,0,0),FVector(-7,0,-12),FVector(10,0,-5),FVector::ZeroVector,FVector(24,0,3)};
 TArray<TPair<FName,FVector>> Out;if(W<0||W>20)return Out;if(Names[W])Out.Emplace(FName(Names[W]),At[W]);if(W==4)Out.Emplace(TEXT("SniperMag"),FVector(7,0,-4.7));if(W==7){Out.Emplace(TEXT("LMGBelt"),FVector(6.3,-6.5,2.1));Out.Emplace(TEXT("AmmoBox"),FVector(6,-14,-22));}if(W==10)Out.Emplace(TEXT("MinigunBox24"),FVector(2,12,-10));if(W==12)Out.Emplace(TEXT("DeagleSlide24"),FVector::ZeroVector);return Out;
}
FString MysticName(int W){static const TCHAR* N[]={TEXT("Gravebreaker"),TEXT("Extreme Slugger"),TEXT("Caldera"),TEXT("Hex Verdict"),TEXT("Absolute Zero"),TEXT("Venom Pulse"),TEXT("Obsidian Reign"),TEXT("Thunder Engine"),TEXT("Hell's Gate"),TEXT("Sunfall"),TEXT("Reactor Hydra"),TEXT("Blood Oath"),TEXT("Gilded Tyrant"),TEXT("Storm Sovereign"),TEXT("Tesla's Judgment"),TEXT("Toxic Furnace"),TEXT("Ten Hells"),TEXT("Titan's Decree"),TEXT("Cursed Exchange"),TEXT("Astral Hands"),TEXT("Liquid Assets")};return N[FMath::Clamp(W,0,20)];}
int Theme(int W){static const int T[]={2,4,0,2,3,1,0,5,0,6,1,4,6,5,5,1,0,6,2,7,1};return T[FMath::Clamp(W,0,20)];}
FString CamoName(int C,int W){static const TCHAR* N[]={TEXT("Factory"),TEXT("Shatter"),TEXT("Circuit"),TEXT("Tiger"),TEXT("Topographic"),TEXT("Hazard"),TEXT("Hex"),TEXT("Gold"),TEXT("Platinum")};return C==9?MysticName(W):N[FMath::Clamp(C,0,8)];}
static int Required(int C){static const int R[]={0,10,25,50,15,75,30,150,5,250};return R[FMath::Clamp(C,0,9)];}
static bool UsesHeads(FName W,int C){return (C==4||C==6)&&W!=TEXT("missile_launcher")&&W!=TEXT("crowbar")&&W!=TEXT("bat");}
static int Progress(const ALWCharacter* P,FName W,int C){return !P?0:W==TEXT("taser")?P->RPG.WeaponStuns62.FindRef(W):UsesHeads(W,C)?P->RPG.WeaponHeads62.FindRef(W):C==8?P->RPG.WeaponElites62.FindRef(W):P->RPG.WeaponKills62.FindRef(W);}
bool Unlocked(const ALWCharacter* P,FName W,int C){if(C<0||C>9)return false;return C==0||Progress(P,W,C)>=Required(C);}
FString Challenge(const ALWCharacter* P,FName W,int C){if(C==0)return TEXT("Available");return FString::Printf(TEXT("%d / %d %s"),Progress(P,W,C),Required(C),W==TEXT("taser")?TEXT("enemies stunned"):UsesHeads(W,C)?TEXT("headshot kills"):C==8?TEXT("legendary kills"):TEXT("kills"));}
float Stat(const FLWItemInstance* G,int I){float S=1;if(G)for(auto& A:G->Attachments)if(const auto* D=Find(A.Value)){if(LWMods::Compatible(G->Definition,A.Value))S*=I==0?D->Damage:I==1?D->Spread:I==2?D->Recoil:D->Rate;}return FMath::Clamp(S,.5f,1.8f);}
void Credit(ALWCharacter* P,FName W,bool Head,bool Elite,bool Stun){if(!P||W.IsNone())return;bool Before[10];for(int C=0;C<10;C++)Before[C]=Unlocked(P,W,C);if(Stun)++P->RPG.WeaponStuns62.FindOrAdd(W);else{++P->RPG.WeaponKills62.FindOrAdd(W);if(Head)++P->RPG.WeaponHeads62.FindOrAdd(W);if(Elite)++P->RPG.WeaponElites62.FindOrAdd(W);}for(int C=1;C<10;C++)if(!Before[C]&&Unlocked(P,W,C))P->Notify(TEXT("CAMO UNLOCKED: ")+CamoName(C,LWItems::Def(W).WeaponIndex),5);P->RequestSave40();}
void Migrate(TArray<FLWItemInstance>& Items){
 for(auto& G:Items){
  if(LWItems::Def(G.Definition).Category==TEXT("WeaponPart")||G.Definition.ToString().StartsWith(TEXT("part39_"))){int Count=FMath::Clamp(G.Count,1,100);G.Definition=TEXT("scrap");G.Count=Count;G.Slot=NAME_None;G.WeaponTier=0;G.LegendaryModifier=NAME_None;}
  if(!G.Parts39.IsEmpty()){for(const auto& Part:G.Parts39)if(LWItems::Def(Part.Definition).Category==TEXT("Attachment")&&LWMods::Compatible(G.Definition,Part.Definition))G.Attachments.Add(LWMods::Mount(Part.Definition),Part.Definition);G.Parts39.Empty();G.Action39.Invalidate();G.CustomName39.Empty();}
 }
}
}
