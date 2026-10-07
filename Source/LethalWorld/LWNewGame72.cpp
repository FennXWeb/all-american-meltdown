#include "LWSyracuse73.h"
#include "LWNewGame72.h"
#include "LWNewYork69.h"
namespace LWNewGame72 {
FVector2D StartXY(){return LWSyracuse73::Start();}
bool LegacySite(FVector P){for(int I=0;I<9;I++){const auto D=FVector2D(P)-LWStory::Site(I);if(FMath::Abs(D.X)<6500&&FMath::Abs(D.Y)<6500)return true;}return false;}
void RetireStory(FLWRPGState& RPG,TArray<FLWItemInstance>& Inventory,TArray<FLWItemInstance>& Stash,TArray<FLWItemInstance>& Overflow){
 const bool HadChapter=RPG.Story.Enabled||RPG.Story.Stage>0||RPG.Story.GearHeld;
 auto ReturnItem=[&](FLWItemInstance Item){
  if(Inventory.ContainsByPredicate([&](const auto& V){return V.Id==Item.Id;})||Stash.ContainsByPredicate([&](const auto& V){return V.Id==Item.Id;})||Overflow.ContainsByPredicate([&](const auto& V){return V.Id==Item.Id;}))return;
  Item.Slot=NAME_None;Item.LoadedMagazine.Invalidate(); // Return magazines separately without losing their rounds.
  if(!LWItems::Place(Inventory,Item,12,LWItems::InventoryHeight(Inventory))&&!LWItems::Place(Stash,Item,12,14))Overflow.Add(Item);
 };
 for(const auto& Item:RPG.Story.Confiscated)ReturnItem(Item);
 if(auto* Fort=RPG.Settlements.Find(TEXT("fort_resolute")))for(const auto& Listing:Fort->Listings)for(const auto& Item:Listing.Items)ReturnItem(Item);
 RPG.Settlements.Remove(TEXT("fort_resolute"));
 if(RPG.Respawn.Settlement==TEXT("fort_resolute")||(HadChapter&&LegacySite(RPG.Respawn.Position)))RPG.Respawn=FLWRespawnPoint();
 RPG.Crew.RemoveAll([](const auto& C){return C.Id.ToString().StartsWith(TEXT("story_"));});
 for(auto& C:RPG.Crew)if(C.Station==TEXT("fort_resolute"))C.Station=NAME_None;
 for(int I=0;I<9;I++){RPG.KnownPlaces.Remove(0xEF320000u+I);RPG.PlaceNames.Remove(0xEF320000u+I);}
 RPG.Story=FLWStoryState();
}
}
