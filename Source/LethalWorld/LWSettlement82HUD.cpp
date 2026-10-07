#include "LWSettlement82.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWVehicle.h"
#include "LWResident.h"
#include "LWGarage45.h"
#include "LWHUD.h"
#include "LWPlayerInput51.h"
#include "Engine/Canvas.h"
#include "EngineUtils.h"
namespace {
const FLinearColor Ink82(.018,.032,.041,.97),Paper82(.9,.94,.9),Muted82(.52,.65,.68),Mint82(.32,.87,.68),Gold82(.97,.66,.33);
FName Key82(const TCHAR* Prefix,FName Id){return FName(*(FString(Prefix)+Id.ToString()));}
FString Short82(FString S,int Max=42){return S.Len()>Max?S.Left(Max-3)+TEXT("..."):S;}
const TCHAR* Job82(FName Use){if(Use==TEXT("farm"))return TEXT("3 food / cycle");if(Use==TEXT("scavenge"))return TEXT("20 scrap / cycle");if(Use==TEXT("recruit"))return TEXT("Faster radio recruitment");if(Use==TEXT("sell"))return TEXT("40 credits / cycle");if(Use==TEXT("contracts"))return TEXT("Collects available contracts");if(Use==TEXT("loot"))return TEXT("Workshop supplies / cycle");if(Use==TEXT("medical"))return TEXT("1 medkit / cycle");if(Use==TEXT("water"))return TEXT("4 water / cycle");return TEXT("Patrols and defends the settlement");}
}
void ALWSettlement82::Draw(ALWHUD& H){
 const float W=H.GetCanvas45()->ClipX/H.Scale;auto* C=State();
 auto Button=[&](FName Id,FString Label,float X,float Y,float Width=210,bool Active=false){H.Control55(Id,Label,X,Y,Width,38,Active);};
 if(Building&&!Palette){
  H.SurfacePanel55(24,24,450,92,Ink82);H.Text(Claiming?TEXT("PLACE SETTLEMENT FLAG"):C?C->Name.ToUpper():TEXT("BUILD"),42,40,1.05,Paper82);
  const auto* D=LWBuilding82::Catalog().IsValidIndex(SelectedCatalog)?&LWBuilding82::Catalog()[SelectedCatalog]:nullptr;
  H.Text(Claiming?TEXT("60 m claim radius"):D?D->Name:TEXT("Select a piece"),42,73,.92,Mint82);
  if(!Claiming){H.SurfacePanel55(W-276,24,252,112,Ink82);H.Text(FString::Printf(TEXT("SCRAP   %d"),Scrap()),W-257,41,1.1,Gold82);H.Text(FString::Printf(TEXT("SNAP %s   ROAD SNAP %s"),Snap?TEXT("ON"):TEXT("OFF"),RoadsSnap?TEXT("ON"):TEXT("OFF")),W-257,75,.68,Paper82);H.Text(FString::Printf(TEXT("%d / 600 PIECES"),C?C->Pieces.Num():0),W-257,104,.7,Muted82);}
  H.SurfacePanel55(24,588,W-48,109,Ink82);H.Rect(24,588,4,109,Valid?Mint82:FLinearColor(.95,.28,.22));H.Text(Short82(Message,110),43,606,.95,Valid?Mint82:Gold82);
  const bool Pad=ULWPlayerInput51::Get51(Player)&&ULWPlayerInput51::Get51(Player)->Controller58;H.Text(Pad?TEXT("A / RT PLACE   RB SELECT   X ROTATE   MENU CATALOG   LT CANCEL MOVE   LB DISMANTLE"):TEXT("LMB PLACE   [E] SELECT   R ROTATE   TAB CATALOG   RMB CANCEL MOVE   DELETE DISMANTLE"),43,639,.75,Paper82);
  H.Text(Pad?TEXT("LS FLY   RS LOOK   Y UP   RS CLICK DOWN   LS CLICK FAST   DPAD LEFT / RIGHT SNAP   DPAD UP / DOWN FOUNDATION HEIGHT   B BACK"):TEXT("WASD FLY   SPACE / CTRL HEIGHT   SHIFT FAST   G SNAP   B ROAD SNAP   PGUP / PGDN FOUNDATION HEIGHT   ESC BACK"),43,668,.68,Muted82);
  H.Rect(W*.5-5,360,10,1,Paper82);H.Rect(W*.5,355,1,10,Paper82);return;
 }
 if(Building&&Palette){
  H.Rect(0,0,W,720,FLinearColor(.003,.008,.012,.77));H.SurfacePanel55(24,24,W-48,672,Ink82);H.Text(TEXT("CONSTRUCTION"),44,42,1.35,Paper82);H.Text(FString::Printf(TEXT("%d SCRAP"),Scrap()),W-330,46,1.1,Gold82);Button(TEXT("s82_resume"),TEXT("Back to build"),W-230,91,180);
  const TCHAR* Cats[]={TEXT("Structure"),TEXT("Furniture"),TEXT("Utilities"),TEXT("Stations"),TEXT("Parking"),TEXT("Decor")};for(int I=0;I<6;I++)Button(FName(*FString::Printf(TEXT("s82_category_%d"),I)),Cats[I],44+I*(W-360)/6,91,(W-372)/6,Category==I);
  TArray<int> Entries;for(int I=0;I<LWBuilding82::Catalog().Num();I++)if(LWBuilding82::Catalog()[I].Category==Category)Entries.Add(I);const int Pages=FMath::Max(1,FMath::DivideAndRoundUp(Entries.Num(),8));Page=FMath::Clamp(Page,0,Pages-1);
  const float TileW=(W-444)*.5f;for(int J=0;J<8;J++){int Row=Page*8+J;if(!Entries.IsValidIndex(Row))break;int I=Entries[Row];const auto& D=LWBuilding82::Catalog()[I];float X=44+(J%2)*(TileW+12),Y=150+(J/2)*106;bool Active=I==SelectedCatalog;
   H.SurfacePanel55(X,Y,TileW,96,FLinearColor(.04,.073,.082,.96));H.Rect(X,Y,3,96,Active?Mint82:FLinearColor(.12,.23,.26));H.Text(FString::Printf(TEXT("%02d"),I+1),X+15,Y+12,.8,Muted82);H.Text(FString::Printf(TEXT("%d SCRAP"),D.Cost),X+TileW-108,Y+12,.75,Gold82);
   Button(FName(*FString::Printf(TEXT("s82_build_%d"),I)),D.Name,X+10,Y+43,TileW-20,Active);
  }
  const float X=W-348;H.Text(TEXT("SURFACE FINISHES"),X,155,.95,Mint82);H.Text(TEXT("Outside / wall front"),X,190,.76,Muted82);Button(TEXT("s82_out"),LWBuilding82::FinishName(Outside),X,212,290);Button(TEXT("s82_outcolor"),FString(TEXT("Color: "))+LWBuilding82::ColorName(OutsideColor),X,257,290);H.Rect(X+262,269,14,14,LWBuilding82::Color(OutsideColor));
  H.Text(TEXT("Inside / floor surface"),X,320,.76,Muted82);Button(TEXT("s82_in"),LWBuilding82::FinishName(Inside),X,342,290);Button(TEXT("s82_incolor"),FString(TEXT("Color: "))+LWBuilding82::ColorName(InsideColor),X,387,290);H.Rect(X+262,399,14,14,LWBuilding82::Color(InsideColor));
  if(!Moving.IsNone()){Button(TEXT("s82_paint"),TEXT("Repaint selection / 2 scrap"),X,444,290);Button(TEXT("s82_dismantle"),TEXT("Dismantle selection"),X,489,290);}
  H.Text(TEXT("New pieces use these finishes."),X,545,.72,Muted82);H.Text(TEXT("Each side can be different."),X,566,.72,Muted82);
  Button(TEXT("s82_catalogprev"),TEXT("Previous"),44,597,140);Button(TEXT("s82_catalognext"),FString::Printf(TEXT("%d / %d  Next"),Page+1,Pages),194,597,160);Button(TEXT("s82_exitbuild"),TEXT("Leave build mode"),W-290,620,230);H.Text(Short82(Message,105),44,653,.8,Gold82);return;
 }
 H.Rect(0,65,W,655,FLinearColor(.004,.012,.017,.96));H.SurfacePanel55(24,86,250,603,Ink82);H.Text(TEXT("YOUR SETTLEMENTS"),42,104,1.05,Paper82);
 TArray<FName> Claims;Player->RPG.Claims82.GetKeys(Claims);Claims.Sort([&](FName A,FName B){return Player->RPG.Claims82[A].Name<Player->RPG.Claims82[B].Name;});for(int I=0;I<Claims.Num();I++)Button(Key82(TEXT("s82_claim_"),Claims[I]),Player->RPG.Claims82[Claims[I]].Name,38,146+I*48,222,Claims[I]==Selected);
 Button(TEXT("s82_placeflag"),TEXT("Place a settlement flag"),38,625,222);
 const float X=302,Content=W-X-30;if(!C){H.Text(TEXT("A place to call home"),X,133,1.9,Paper82);H.Text(TEXT("Buy a settlement flag from a supplies merchant, or find one while scavenging."),X,204,.95,Muted82);H.Text(TEXT("Use the flag outdoors to claim land. Build beds and a HAM radio to attract residents."),X,236,.9,Muted82);H.Text(TEXT("Structures, furniture and stations use scrap. Supplies and vehicles persist when you leave."),X,284,.85,Muted82);H.Text(Message,X,370,.9,Gold82);return;}
 H.Text(C->Name,X,104,1.7,Paper82);H.Text(FString::Printf(TEXT("%d / %d RESIDENTS     %d SCRAP     %lld CREDITS"),C->Residents.Num(),LWBuilding82::Beds(*C),C->Scrap,C->Treasury),X,145,.92,Mint82);
 const TCHAR* Panels[]={TEXT("Overview"),TEXT("Residents & jobs"),TEXT("Vehicle parking"),TEXT("Contracts")};for(int I=0;I<4;I++)Button(FName(*FString::Printf(TEXT("s82_panel_%d"),I)),Panels[I],X+I*Content/4,180,Content/4-10,Panel==I);
 if(Panel==0){
  const bool Radio=C->Pieces.ContainsByPredicate([](const auto& P){return P.Catalog==TEXT("ham");});H.Text(TEXT("BUILD & SUPPLIES"),X,247,1,Mint82);Button(TEXT("s82_begin"),TEXT("Enter build mode"),X,280,Content*.47);Button(TEXT("s82_deposit"),TEXT("Deposit carried scrap"),X,326,Content*.47);Button(TEXT("s82_supplies"),TEXT("Open shared supplies"),X,372,Content*.47);Button(TEXT("s82_treasury"),TEXT("Collect settlement earnings"),X,418,Content*.47);Button(TEXT("s82_waypoint"),TEXT("Set waypoint"),X,464,Content*.47);
  const float Right=X+Content*.53;H.Text(TEXT("RECRUITMENT"),Right,247,1,Mint82);H.Text(!Radio?TEXT("Build a HAM radio to begin."):LWBuilding82::Beds(*C)<=C->Residents.Num()?TEXT("All beds are reserved."):C->Broadcast?TEXT("Broadcasting for new residents"):TEXT("Broadcast paused"),Right,286,.87,Paper82);H.Text(FString::Printf(TEXT("%d vacant beds"),FMath::Max(0,LWBuilding82::Beds(*C)-C->Residents.Num())),Right,320,.86,Muted82);Button(TEXT("s82_broadcast"),C->Broadcast?TEXT("Pause recruitment"):TEXT("Resume recruitment"),Right,358,Content*.47);
  H.Text(TEXT("Work completes every 5 minutes of play."),Right,428,.75,Muted82);H.Text(TEXT("Workers in your active crew pause their jobs."),Right,455,.75,Muted82);H.Text(TEXT("RVs and buses reserve three parking bays."),Right,482,.75,Muted82);
  H.Text(FString::Printf(TEXT("BUILD BUDGET   %d / 600"),C->Pieces.Num()),X,550,.8,Muted82);H.Rect(X,578,Content,6,FLinearColor(.1,.18,.2));H.Rect(X,578,Content*C->Pieces.Num()/600.f,6,Mint82);
 }else if(Panel==1){
  int Pages=FMath::Max(1,FMath::DivideAndRoundUp(C->Residents.Num(),6));ResidentPage=FMath::Clamp(ResidentPage,0,Pages-1);const float Left=Content*.48;
  for(int J=0;J<6;J++){int I=ResidentPage*6+J;if(!C->Residents.IsValidIndex(I))break;const auto& N=C->Residents[I];Button(Key82(TEXT("s82_resident_"),N.Id),N.Name,X,241+J*52,Left,N.Id==SelectedResident);}
  Button(TEXT("s82_peopleprev"),TEXT("Previous"),X,565,Left*.48);Button(TEXT("s82_peoplenext"),FString::Printf(TEXT("%d / %d  Next"),ResidentPage+1,Pages),X+Left*.52,565,Left*.48);
  const float Right=X+Content*.53;const auto* N=C->Residents.FindByPredicate([&](const auto& R){return R.Id==SelectedResident;});if(N){auto* CrewRecord=Player->RPG.Crew.FindByPredicate([&](const auto& R){return R.Id==N->Id;});const auto* Station=C->Pieces.FindByPredicate([&](const auto& P){return P.Id==N->Station;});const auto* D=Station?LWBuilding82::Find(Station->Catalog):nullptr;
   H.Text(N->Name,Right,246,1.2,Paper82);H.Text(CrewRecord&&CrewRecord->Following?TEXT("Traveling with you"):TEXT("At home"),Right,280,.9,Mint82);H.Text(D?D->Name:TEXT("Unassigned"),Right,316,.9,Paper82);H.Text(D?Job82(D->Use):TEXT("Choose a built work station below."),Right,342,.77,Muted82);
   Button(TEXT("s82_jobnext"),TEXT("Assign next available station"),Right,378,Content*.47);Button(TEXT("s82_jobclear"),TEXT("Clear work assignment"),Right,424,Content*.47);Button(TEXT("s82_follow"),TEXT("Add to active crew"),Right,488,Content*.47);Button(TEXT("s82_home"),TEXT("Return to this settlement"),Right,534,Content*.47);
  }else{H.Text(TEXT("Select a resident"),Right,263,1.2,Paper82);H.Text(TEXT("One worker per station."),Right,310,.85,Muted82);H.Text(TEXT("Build more stations to add more jobs."),Right,339,.8,Muted82);}
 }else if(Panel==2){
  TArray<FName> Lots;for(const auto& P:C->Pieces)if(P.Catalog==TEXT("parking"))Lots.Add(P.Id);if(!Lots.Contains(SelectedLot))SelectedLot=Lots.Num()?Lots[0]:NAME_None;
  if(Lots.IsEmpty())H.Text(TEXT("Build a parking lot to insure your vehicles."),X,266,1,Paper82);else{
   int Used=0;for(const auto& V:World->Vehicles)if(V.Value.HomeParking82==SelectedLot)Used+=LWGarage45::Bays(V.Value.Model);Button(TEXT("s82_lotnext"),FString::Printf(TEXT("Lot %d / %d    %d / 10 bays reserved"),Lots.IndexOfByKey(SelectedLot)+1,Lots.Num(),Used),X,242,Content*.58);Button(TEXT("s82_park"),TEXT("Register vehicle on lot"),X+Content*.62,242,Content*.38);
   int Row=0;for(const auto& V:World->Vehicles)if(V.Value.HomeParking82==SelectedLot){H.Text(FString::Printf(TEXT("%02d  %s"),V.Value.ParkingBay82+1,LWTraffic::Get(V.Value.Model).Name),X,305+Row*27,.85,Paper82);H.Text(V.Value.Exploded?TEXT("Replacement pending"):TEXT("Insured"),X+Content*.48,305+Row*27,.75,Mint82);H.Control55(Key82(TEXT("s82_release_"),V.Key),TEXT("Release"),X+Content*.77,298+Row*27,Content*.23,25);Row++;}
   H.Text(TEXT("Registering makes the car yours. Destruction returns it here with its cargo."),X,603,.77,Muted82);
  }
 }else{
  H.Text(TEXT("Collected by assigned contract-office workers"),X,245,.93,Muted82);for(int I=0;I<C->Contracts.Num();I++){const auto* Q=ULWRPGCatalog::Get()->Quests.FindByPredicate([&](const auto& R){return R.Id==C->Contracts[I];});if(Q)Button(Key82(TEXT("s82_contract_"),Q->Id),Q->Title,X,285+I*47,Content);}
  if(C->Contracts.IsEmpty())H.Text(TEXT("No new contracts waiting."),X,296,1,Paper82);
 }
 H.Text(Short82(Message,110),X,656,.82,Gold82);
}
bool ALWSettlement82::Action(FName Id){FString S=Id.ToString();if(!S.StartsWith(TEXT("s82_")))return false;auto* C=State();
 if(S.StartsWith(TEXT("s82_claim_"))){Selected=FName(*S.Mid(10));SelectedResident=SelectedLot=NAME_None;ResidentPage=0;Message.Empty();return true;}
 if(Id==TEXT("s82_placeflag")){BeginClaim();return true;}
 if(Id==TEXT("s82_exitbuild")){Close();Player->SwitchTab(6);return true;}
 if(Id==TEXT("s82_resume")){SetPalette(false);return true;}
 if(S.StartsWith(TEXT("s82_category_"))){Category=FMath::Clamp(FCString::Atoi(*S.Mid(13)),0,5);Page=0;return true;}
 if(S.StartsWith(TEXT("s82_build_"))){int I=FCString::Atoi(*S.Mid(10));if(LWBuilding82::Catalog().IsValidIndex(I)){SelectedCatalog=I;Moving=NAME_None;SetPalette(false);}return true;}
 if(Id==TEXT("s82_catalogprev")){Page=FMath::Max(0,Page-1);return true;}if(Id==TEXT("s82_catalognext")){Page++;return true;}
 if(Id==TEXT("s82_out")){Outside=(Outside+1)%7;return true;}if(Id==TEXT("s82_in")){Inside=(Inside+1)%7;return true;}if(Id==TEXT("s82_outcolor")){OutsideColor=(OutsideColor+1)%8;return true;}if(Id==TEXT("s82_incolor")){InsideColor=(InsideColor+1)%8;return true;}
 if(Id==TEXT("s82_paint")){Paint();return true;}if(Id==TEXT("s82_dismantle")){Dismantle();return true;}
 if(!C)return true;
 if(S.StartsWith(TEXT("s82_panel_"))){Panel=FMath::Clamp(FCString::Atoi(*S.Mid(10)),0,3);Message.Empty();return true;}
 if(Id==TEXT("s82_begin")){BeginBuild();return true;}if(Id==TEXT("s82_deposit")){Deposit();return true;}
 if(Id==TEXT("s82_waypoint")){Player->SetWaypoint(FVector2D(C->Center));Message=TEXT("Waypoint set.");return true;}
 if(Id==TEXT("s82_broadcast")){C->Broadcast=!C->Broadcast;Player->RequestSave40();return true;}
 if(Id==TEXT("s82_treasury")){if(FVector::Dist2D(Player->GetActorLocation(),C->Center)>C->Radius){Message=TEXT("Visit the settlement to collect earnings.");return true;}Player->Money+=C->Treasury;C->Treasury=0;Player->RequestSave40();Message=TEXT("Earnings collected.");return true;}
 if(Id==TEXT("s82_supplies")){
  if(FVector::Dist2D(Player->GetActorLocation(),C->Center)>C->Radius){Message=TEXT("Visit the settlement to access its supplies.");return true;}
  FName Store(*(C->Id.ToString()+TEXT("_supplies")));auto& R=World->Containers.FindOrAdd(Store);R.Id=Store;R.Context=TEXT("settlement82");R.Width=12;R.Height=24;R.Position=Player->GetActorLocation();R.Unlocked=true;
  auto* Proxy=Actors.FindRef(Store).Get();if(!Proxy){Proxy=GetWorld()->SpawnActor<ALWBuildPiece82>();Proxy->World=World;Proxy->Kind=ELWObjectKind::Container;Proxy->RecordId=Store;Proxy->Claim=C->Id;Proxy->SetActorHiddenInGame(true);Proxy->SetActorEnableCollision(false);Actors.Add(Store,Proxy);}Proxy->SetActorLocation(Player->GetActorLocation());Player->OpenContainer(Proxy);return true;
 }
 if(S.StartsWith(TEXT("s82_resident_"))){SelectedResident=FName(*S.Mid(13));return true;}
 if(Id==TEXT("s82_peopleprev")){ResidentPage=FMath::Max(0,ResidentPage-1);return true;}if(Id==TEXT("s82_peoplenext")){ResidentPage++;return true;}
 if(Id==TEXT("s82_follow")){Crew(SelectedResident,true);return true;}if(Id==TEXT("s82_home")){Crew(SelectedResident,false);return true;}if(Id==TEXT("s82_jobclear")){Assign(SelectedResident,NAME_None);return true;}
 if(Id==TEXT("s82_jobnext")){auto* N=C->Residents.FindByPredicate([&](const auto& R){return R.Id==SelectedResident;});if(!N)return true;TArray<FName> Jobs;for(const auto& P:C->Pieces)if(const auto* D=LWBuilding82::Find(P.Catalog);D&&D->Job()&&!C->Residents.ContainsByPredicate([&](const auto& R){return R.Id!=N->Id&&R.Station==P.Id;}))Jobs.Add(P.Id);if(Jobs.Num())Assign(N->Id,Jobs[(Jobs.IndexOfByKey(N->Station)+1)%Jobs.Num()]);else Message=TEXT("Build an unoccupied work station first.");return true;}
 if(Id==TEXT("s82_lotnext")){TArray<FName> Lots;for(const auto& P:C->Pieces)if(P.Catalog==TEXT("parking"))Lots.Add(P.Id);if(Lots.Num())SelectedLot=Lots[(Lots.IndexOfByKey(SelectedLot)+1)%Lots.Num()];return true;}
 if(Id==TEXT("s82_park")){const auto* Lot=C->Pieces.FindByPredicate([&](const auto& P){return P.Id==SelectedLot;});ALWVehicle* Pick=nullptr;float Best=MAX_flt;if(Lot)for(TActorIterator<ALWVehicle> V(GetWorld());V;++V){FVector Local=Lot->Transform.InverseTransformPosition(V->GetActorLocation());if(FMath::Abs(Local.X)>2000||FMath::Abs(Local.Y)>1300||FMath::Abs(Local.Z)>260||!V->Record()||V->Record()->HomeParking82==SelectedLot)continue;float D=FVector::DistSquared(V->GetActorLocation(),Player->GetActorLocation());if(D<Best){Best=D;Pick=*V;}}if(Pick)Park(Pick,SelectedLot);else Message=TEXT("Park an unregistered vehicle on the selected lot first.");return true;}
 if(S.StartsWith(TEXT("s82_release_"))){ReleaseCar(FName(*S.Mid(12)));return true;}
 if(S.StartsWith(TEXT("s82_contract_"))){FName Q(*S.Mid(13));if(C->Contracts.Contains(Q)&&Player->AcceptQuest(Q)){C->Contracts.Remove(Q);Message=TEXT("Contract added to your journal.");}return true;}
 return true;
}
