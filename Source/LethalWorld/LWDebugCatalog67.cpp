#include "LWDebugMenu67.h"
#include "LWConsole47.h"
#include "LWInventory.h"
#include "LWVehicleSpec.h"
#include "LWPOITypes.h"

TArray<FLWDebugRow67> LWDebug67::Catalog(ELWDebugSection67 Section){
 TArray<FLWDebugRow67> Out;
 auto Add=[&](FString Id,FString Name,FString Category,FString Detail,FString Command,int Min=1,int Max=1,int Default=1,bool HasAmount=false){
  auto E=MakeShared<FLWDebugEntry67>();E->Id=MoveTemp(Id);E->Name=MoveTemp(Name);E->Category=MoveTemp(Category);E->Details=MoveTemp(Detail);E->Command=MoveTemp(Command);E->Min=Min;E->Max=Max;E->Default=Default;E->HasAmount=HasAmount;Out.Add(E);
 };
 if(Section==ELWDebugSection67::Cheats){
  Add(TEXT("money"),TEXT("Add credits"),TEXT("Progression"),TEXT("Add credits to your balance. This change is saved normally."),TEXT("money"),1,1000000000,1000,true);
  Add(TEXT("heal"),TEXT("Restore all vitals"),TEXT("Survival"),TEXT("Refill health, stamina, food and water."),TEXT("heal"));
  Add(TEXT("god"),TEXT("Toggle god mode"),TEXT("Survival"),TEXT("Toggle damage immunity for this session. The current state is shown below."),TEXT("god"));
  Add(TEXT("xp"),TEXT("Add experience"),TEXT("Progression"),TEXT("Gain experience through the normal levelling system."),TEXT("xp"),1,100000,1000,true);
  Add(TEXT("skillpoints"),TEXT("Add skill points"),TEXT("Progression"),TEXT("Add unspent perk points."),TEXT("skillpoints"),1,1000,10,true);
  Add(TEXT("time"),TEXT("Set time of day"),TEXT("World"),TEXT("Choose an hour from 0 (midnight) to 23. The day cycle resumes when you close this menu."),TEXT("time"),0,23,12,true);
 }else if(Section==ELWDebugSection67::Items){
  for(const auto& D:LWItems::All47()){
   FString Detail=FString::Printf(TEXT("Inventory footprint: %d x %d\nMaximum stack: %d\nValue: %d credits"),D.Width,D.Height,D.MaxStack,D.Price);
   if(!D.AmmoType.IsNone())Detail+=TEXT("\nAmmunition: ")+LWItems::Def(D.AmmoType).DisplayName.ToString()+TEXT(" (")+D.AmmoType.ToString()+TEXT(")");
   if(!D.MagazineType.IsNone())Detail+=TEXT("\nMagazine: ")+LWItems::Def(D.MagazineType).DisplayName.ToString()+TEXT(" (")+D.MagazineType.ToString()+TEXT(")");
   Detail+=TEXT("\n\nItems go into your inventory. If there is not enough room, nothing is added.");
   Add(D.Id.ToString(),D.DisplayName.ToString(),D.Category.ToString(),Detail,TEXT("give ")+D.Id.ToString(),1,1000,1,true);
  }
 }else if(Section==ELWDebugSection67::NPCs){
  for(const auto& N:LWCheats47::NPCs60())Add(N.Id,N.Name,N.Kind<0?TEXT("Friendly"):N.Kind>=9&&N.Kind<=11?TEXT("Boss"):TEXT("Hostile"),N.Kind<0?TEXT("Spawn a friendly NPC ahead of you. Companion candidates can be recruited through dialogue. Cheat-spawned NPCs last for this session."):TEXT("Spawn hostile NPCs ahead of you. They become active when you close the menu. Cheat-spawned NPCs last for this session."),FString(TEXT("spawnnpc "))+N.Id,1,20,1,true);
 }else if(Section==ELWDebugSection67::Vehicles){
  for(const auto& V:LWTraffic::Specs()){
   FString Group=FString(V.Id)==TEXT("helicopter")?TEXT("Aircraft"):FString(V.Feature)==TEXT("turret")?TEXT("Armed"):V.Height>=245?TEXT("Heavy / utility"):TEXT("Road vehicles");
   Add(V.Id,V.Name,Group,FString::Printf(TEXT("Seats: %d\nCargo grid: %d x %d\nMaximum speed: %.0f km/h\nLength: %.1f m\nFeature: %s\n\nSpawns ahead of you, unlocked and hotwired. Requires clear space outside the bunker."),V.Seats,V.CargoW,V.CargoH,V.MaxSpeed*.036f,V.HalfLength*.02f,V.Feature),FString(TEXT("spawnvehicle "))+V.Id);
  }
 }else{
  for(int I=0;I<LWPlaces::Count;I++){
   FString Group=LWLandmarks::Unique(I)?TEXT("Unique landmarks"):LWDungeons::IsDungeon(I)?TEXT("Dungeons"):LWPlaces::Underground(I)?TEXT("Underground"):LWPlaces::IsTower(I)?TEXT("Towers"):I==LWPlaces::Tavern?TEXT("Settlements"):I==7||I==8||I==9||I==6?TEXT("Residential"):TEXT("Public / commercial");
   const FVector2D Size=LWPlaces::Size(I);
   Add(FString::FromInt(I),LWPlaces::Name(I),Group,FString::Printf(TEXT("Type: %s\nSite size: %.0f x %.0f m\n\nFind the nearest generated location within the selected radius and set a waypoint. Does not spawn a duplicate building or reveal unrelated map locations."),LWPlaces::Event(I),Size.X*.01,Size.Y*.01),FString::Printf(TEXT("locate %d"),I),1,20,5,true);
  }
 }
 return Out;
}
TArray<FLWDebugRow67> LWDebug67::Filter(const TArray<FLWDebugRow67>& Entries,const FString& Query,const FString& Category,int Sort,bool Descending){
 TArray<FString> Words;Query.TrimStartAndEnd().ParseIntoArrayWS(Words);TArray<FLWDebugRow67> Out;
 for(const auto& E:Entries){if(!Category.IsEmpty()&&E->Category!=Category)continue;const FString Haystack=E->Name+TEXT(" ")+E->Id+TEXT(" ")+E->Category+TEXT(" ")+E->Details;bool Match=true;for(const auto& W:Words)if(!Haystack.Contains(W,ESearchCase::IgnoreCase)){Match=false;break;}if(Match)Out.Add(E);}
 Out.Sort([=](const auto& A,const auto& B){
  const FString& L=Sort==1?A->Id:Sort==2?A->Category:A->Name;const FString& R=Sort==1?B->Id:Sort==2?B->Category:B->Name;
  int C=Sort==1&&L.IsNumeric()&&R.IsNumeric()?FCString::Atoi(*L)-FCString::Atoi(*R):L.Compare(R,ESearchCase::IgnoreCase);
  if(C==0)C=A->Name.Compare(B->Name,ESearchCase::IgnoreCase);if(C==0)C=A->Id.Compare(B->Id,ESearchCase::CaseSensitive);return Descending?C>0:C<0;
 });return Out;
}
FString LWDebug67::Command(const FLWDebugEntry67& E,int32 Amount){return E.Command+(E.HasAmount?FString::Printf(TEXT(" %d"),FMath::Clamp(Amount,E.Min,E.Max)):FString());}
