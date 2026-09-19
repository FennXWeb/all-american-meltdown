#include "LWBunker45.h"
#include "Components/StaticMeshComponent.h"
#include "LWGarage45.h"
#include "LWHUD.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWVehicle.h"
#include "LWResident.h"
#include "EngineUtils.h"
#include "Engine/Canvas.h"
namespace {const TArray<FName> Jobs={TEXT("trade"),TEXT("salvage"),TEXT("recruit"),TEXT("contracts"),TEXT("medical"),TEXT("farm")};const TCHAR* JobNames[]={TEXT("Trading office"),TEXT("Salvage workshop"),TEXT("Recruitment office"),TEXT("Dispatch room"),TEXT("Infirmary"),TEXT("Hydroponics")};}
void ALWBunker45::Draw(ALWHUD& H){
 if(Player->BuildMode45){
  H.Rect(20,570,1020,130,FLinearColor(.02,.035,.02,.88));H.Text(TEXT("BUNKER BUILD MODE"),40,585,1.3);
  H.Text(TEXT("WASD MOVE   MOUSE LOOK   SPACE/CTRL HEIGHT   SHIFT FAST"),40,610,.9);
  H.Text(TEXT("E SELECT   R ROTATE   G SNAP   CLICK PLACE   DELETE DISMANTLE"),40,632,.9);
  H.Text(TEXT("RIGHT CLICK CANCEL PLACEMENT   ESC EXIT"),40,654,.9);
  H.Text(Message+TEXT("  ")+(Preview->IsVisible()?(ValidPlacement?TEXT("CLEAR"):TEXT("BLOCKED")):TEXT("")),40,678,.9);return;
 }
 Entries46.Empty();const float W=H.GetCanvas45()->ClipX/H.Scale;H.Rect(0,0,W,720,FLinearColor(.005f,.015f,.008f,1));for(int Y=0;Y<720;Y+=4)H.Rect(0,Y,W,1,FLinearColor(0,0,0,.22f));H.Rect(25,17,W-50,2,FLinearColor(.12f,.5f,.22f));
 H.Text(TEXT("SHELTER CONTROL"),50,30,2.6,FLinearColor(.35f,1.f,.48f));H.Text(FString::Printf(TEXT("%lld CREDITS   %d SCRAP   %d/%d RESIDENTS"),Player->Money,Player->CountSupply(TEXT("scrap")),Player->RPG.Crew.Num(),Player->Bunker45.Bedrooms()),50,77,.95);
 const TCHAR* Tabs[]={TEXT("FURNITURE"),TEXT("EXPANSIONS"),TEXT("ASSIGNMENTS"),TEXT("GARAGE")};
 for(int I=0;I<4;++I)H.Button(FName(*FString::Printf(TEXT("base_tab_%d"),I)),Tabs[I],50+I*270,112,260);
 H.Button(TEXT("base_close"),TEXT("CLOSE"),W-180,30,140);
 if(BuildQueue.Num())H.Text(FString::Printf(TEXT("CONSTRUCTION: %d sections remaining"),BuildQueue.Num()),50,163,1.0,FLinearColor(.8,.6,.2));
 else H.Text(Message,50,163,.95);
 auto B=[&](FName Id,FString Text,int Row,float X=50,float Width=800){H.Button(Id,Text,X,210+Row*48,Width);};
 if(Tab==0){
  H.Button(TEXT("base_edit"),TEXT("EDIT EXISTING FURNITURE"),900,210,320);
  H.Text(TEXT("Half the scrap is recovered\nwhen dismantling furniture.\nEmpty storage first.\n\nFree camera, rotation,\ngrid snapping and collision\npreview are available."),900,274,.9);
  for(int I=Page*8;I<FMath::Min(Page*8+8,Catalog().Num());++I){const auto& C=Catalog()[I];B(FName(*FString::Printf(TEXT("base_build_%d"),I)),FString::Printf(TEXT("%s / %d SCRAP"),*C.Name,C.Cost),I%8);}
 }else if(Tab==1){
  B(TEXT("base_buy_bedrooms"),Player->Bunker45.Bedrooms()>=100?FString(TEXT("BEDROOM CAPACITY: 100 / MAXIMUM")):FString::Printf(TEXT("ADD 10 BEDROOMS / %d CREDITS"),2000+Player->Bunker45.BedroomFloors*1000),0,50,570);
  B(TEXT("base_buy_garage"),Player->Bunker45.Garage?TEXT("GARAGE OWNED"):TEXT("10-BAY GARAGE / 12000 CREDITS"),1,50,570);
  H.Text(TEXT("Each expansion includes elevator access.\nBedroom capacity: 10 to 100.\nRVs and buses reserve three adjacent bays."),50,326,.95);
  const TCHAR* Effects[]={TEXT("Earn credits"),TEXT("Find supplies"),TEXT("Recruit residents"),TEXT("Find contracts"),TEXT("Heal you in the bunker"),TEXT("Grow food and water")};
  for(int I=0;I<6;++I){H.Button(FName(*(TEXT("base_buy_")+Jobs[I].ToString())),FString(JobNames[I])+(Player->Bunker45.Utilities.Contains(Jobs[I])?TEXT(" / OWNED"):TEXT(" / 3000")),650,210+I*63,570);H.Text(Effects[I],664,253+I*63,.8);}
 }else if(Tab==2){
  H.Text(TEXT("Select a resident to cycle available work assignments. Following companions do not work."),50,189,.85);
  for(int I=Page*8;I<FMath::Min(Page*8+8,Player->RPG.Crew.Num());++I){const auto& C=Player->RPG.Crew[I];FName Work=Player->Bunker45.Assignments.FindRef(C.Id);B(FName(*FString::Printf(TEXT("base_assign_%d"),I)),C.Name+TEXT(" / ")+(Work.IsNone()?TEXT("OFF DUTY"):Work.ToString().ToUpper()),I%8,50,1100);}
  H.Text(TEXT("Work pays every 5 minutes while playing. Up to 8 workers per utility room.\nSalvage and farm output goes to the supply locker beside the entrance."),50,602,.85);
 }else if(Tab==3){
  int I=0;for(const auto& Pair:World->Vehicles)if(Pair.Value.Owned45){const auto& R=Pair.Value;if(I<Page*8||I>=Page*8+8){I++;continue;}B(FName(*(TEXT("base_car_")+Pair.Key.ToString())),FString::Printf(TEXT("BAY %d / %s / %s"),R.GarageBay45+1,LWTraffic::Get(R.Model).Name,R.Exploded?TEXT("REPLACING"):R.Stored45?TEXT("STORED"):TEXT("OUTSIDE")),(I++)%8,50,680);}
  if(!GarageCar.IsNone()){const auto* Selected=World->Vehicles.Find(GarageCar);H.Text(Selected?FString(LWTraffic::Get(Selected->Model).Name)+TEXT(" / ")+Selected->VIN.ToString(EGuidFormats::Digits).Right(6):TEXT("SELECT A VEHICLE"),780,205,.9);B(TEXT("base_repair"),FString::Printf(TEXT("REPAIR / %d CREDITS"),Selected?FMath::CeilToInt((200-Selected->Health)*3+Selected->Dents.Num()*5):0),1,780,400);B(TEXT("base_mods"),TEXT("UPGRADES"),2,780,400);B(TEXT("base_paints"),TEXT("PAINT / 250 CREDITS"),3,780,400);B(TEXT("base_retrieve"),TEXT("SEND TO SURFACE"),4,780,400);B(TEXT("base_release"),TEXT("RELEASE OWNERSHIP"),5,780,400);}
  if(I==0)H.Text(TEXT("Store a vehicle using the surface lift.\nSwitch off the engine and exit before pressing the button."),50,235,1);
 }else if(Tab==4){
  const auto* R=World->Vehicles.Find(GarageCar);int I=0;for(const auto& M:LWGarage45::Mods())if(R&&LWGarage45::Compatible(M,R->Model)){
   if(I>=Page*8&&I<Page*8+8)B(FName(*(TEXT("base_mod_")+M.Id.ToString())),FString::Printf(TEXT("%s / %d / %s +%.0f%s%s"),*M.Name,M.Cost,*M.Stat.ToString(),M.Stat==TEXT("cargo")||M.Stat==TEXT("clearance")?M.Value:M.Value*100,M.Stat==TEXT("cargo")?TEXT(" rows"):M.Stat==TEXT("clearance")?TEXT(" cm"):TEXT("%"),R->Mods45.Contains(M.Id)?TEXT(" / INSTALLED"):TEXT("")),I%8,50,1130);I++;
  }H.Text(TEXT("A new upgrade replaces the previous upgrade in the same slot."),50,610,.9);
 }else if(Tab==5){
  const TCHAR* Names[]={TEXT("Silver"),TEXT("Patrol black"),TEXT("Industrial white"),TEXT("Coach pearl"),TEXT("School yellow"),TEXT("Van blue"),TEXT("Pickup red"),TEXT("Bike green"),TEXT("SUV charcoal"),TEXT("Muscle copper"),TEXT("Supercar orange")};
  for(int I=0;I<11;++I)H.Button(FName(*FString::Printf(TEXT("base_paint_%d"),I)),Names[I],50+(I/6)*560,210+(I%6)*52,530);
 }else if(Tab==6){
  TArray<int> Floors={0};for(int I=1;I<=Player->Bunker45.BedroomFloors;++I)Floors.Add(I);if(Player->Bunker45.Utilities.Num())Floors.Add(10);if(Player->Bunker45.Garage)Floors.Add(11);
  for(int I=0;I<Floors.Num();++I){int F=Floors[I];H.Button(FName(*FString::Printf(TEXT("base_floor_%d"),F)),F==0?TEXT("MAIN BUNKER"):F==10?TEXT("UTILITY ROOMS"):F==11?TEXT("GARAGE"):FString::Printf(TEXT("BEDROOM FLOOR %d"),F),50+(I/6)*550,210+(I%6)*52,520);}
 }
 H.Button(TEXT("base_prev"),TEXT("PREVIOUS"),50,659,190);H.Button(TEXT("base_next"),TEXT("NEXT"),260,659,190);H.Text(FString::Printf(TEXT("PAGE %d"),Page+1),485,670,.9);H.Text(TEXT("ARROWS: SELECT   ENTER: CONFIRM   ESC: EXIT"),680,675,.8,FLinearColor(.25f,.7f,.35f));
}
void ALWBunker45::Action(FName Id){
 const FString S=Id.ToString();
 if(Id==TEXT("base_close")){Close();return;}
 if(S.StartsWith(TEXT("base_tab_"))){Tab=FCString::Atoi(*S.Mid(9));Page=0;return;}
 if(Id==TEXT("base_prev")){Page=FMath::Max(0,Page-1);return;}if(Id==TEXT("base_next")){int Count=Tab==0?Catalog().Num():Tab==2?Player->RPG.Crew.Num():Tab==4?LWGarage45::Mods().Num():8;if(Tab==3){Count=0;for(const auto& V:World->Vehicles)Count+=V.Value.Owned45;}if(Tab==4){Count=0;const auto* R=World->Vehicles.Find(GarageCar);if(R)for(const auto& M:LWGarage45::Mods())Count+=LWGarage45::Compatible(M,R->Model);}Page=FMath::Max(0,FMath::Min((Count-1)/8,Page+1));return;}
 if(Id==TEXT("base_edit")){BeginEdit();return;}
 if(S.StartsWith(TEXT("base_build_"))){BeginEdit(FCString::Atoi(*S.Mid(11)));return;}
 if(S.StartsWith(TEXT("base_buy_"))){BuyExpansion(FName(*S.Mid(9)));return;}
 if(S.StartsWith(TEXT("base_assign_"))){int I=FCString::Atoi(*S.Mid(12));if(!Player->RPG.Crew.IsValidIndex(I))return;auto& C=Player->RPG.Crew[I];TArray<FName> Available={NAME_None};for(FName Job:Jobs)if(Player->Bunker45.Utilities.Contains(Job))Available.Add(Job);FName Old=Player->Bunker45.Assignments.FindRef(C.Id);FName Next=Available[(Available.Find(Old)+1)%Available.Num()];if(Next.IsNone())Player->Bunker45.Assignments.Remove(C.Id);else {Player->Bunker45.Assignments.Add(C.Id,Next);C.Following=false;C.Station=NAME_None;}Player->RequestSave40();return;}
 if(S.StartsWith(TEXT("base_car_"))){GarageCar=FName(*S.Mid(9));return;}
 if(Id==TEXT("base_repair")){Repair();return;}if(Id==TEXT("base_mods")){Tab=4;Page=0;return;}if(Id==TEXT("base_paints")){Tab=5;Page=0;return;}
 if(Id==TEXT("base_retrieve")){Retrieve(GarageCar);return;}
 if(Id==TEXT("base_release")){if(Retrieve(GarageCar))if(auto* R=World->Vehicles.Find(GarageCar)){R->Owned45=false;R->GarageBay45=-1;Player->RequestSave40();}return;}
 if(S.StartsWith(TEXT("base_mod_"))){Install(FName(*S.Mid(9)));return;}
 if(S.StartsWith(TEXT("base_paint_"))){Paint(FCString::Atoi(*S.Mid(11)));return;}
 if(S.StartsWith(TEXT("base_floor_"))){GoFloor(FCString::Atoi(*S.Mid(11)));return;}
}

