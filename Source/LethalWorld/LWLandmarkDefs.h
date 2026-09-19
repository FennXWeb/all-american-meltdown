#pragma once
#include "CoreMinimal.h"
namespace LWLandmarks {
constexpr int Airport=32,First=33,Count=20;
struct FDefinition {const TCHAR* Name;const TCHAR* Story;const TCHAR* Steps[3];int Layout;int Mechanic;const TCHAR* Weapon;};
inline const FDefinition& Get(int T){static const FDefinition D[]={
 {TEXT("LIBERTY LAST BROADCAST"),TEXT("The final uncensored news bulletin is still trapped in a dead transmitter."),{TEXT("Restore the transmitter"),TEXT("Tune the emergency frequency"),TEXT("Broadcast the recovered bulletin")},0,0,TEXT("rifle")},
 {TEXT("MERCY SEED VAULT"),TEXT("A botanist sealed the last viable crop samples behind an irrigation interlock."),{TEXT("Inspect the seed archive"),TEXT("Supply three bottles of water"),TEXT("Restart the germination chamber")},1,1,TEXT("shotgun")},
 {TEXT("ATLAS OBSERVATORY"),TEXT("The observatory recorded something crossing the exclusion zone after the bombs."),{TEXT("Power the tracking console"),TEXT("Align the observation array"),TEXT("Decode the orbital recording")},0,2,TEXT("sniper")},
 {TEXT("PRESIDENTIAL NIGHT TRAIN"),TEXT("Richardson's abandoned evacuation train carried sealed orders and a private arsenal."),{TEXT("Search the staff carriage"),TEXT("Unlock the command carriage"),TEXT("Open the presidential compartment")},2,0,TEXT("desert_eagle")},
 {TEXT("HOOVER MEMORIAL SPILLWAY"),TEXT("An emergency crew died trying to keep the flood gates open."),{TEXT("Inspect the flood control log"),TEXT("Route turbine power"),TEXT("Release the spillway interlock")},3,2,TEXT("lmg")},
 {TEXT("SUNKEN TREASURY"),TEXT("Emergency currency was moved here, then deliberately sealed away."),{TEXT("Start the drainage pump"),TEXT("Recover the treasury ledger"),TEXT("Break the reserve seal")},1,3,TEXT("m4")},
 {TEXT("CHAPEL OF THE ASHES"),TEXT("A caretaker left a bell sequence to guide survivors to hidden relief supplies."),{TEXT("Read the caretaker's inscription"),TEXT("Ring the memorial sequence"),TEXT("Open the reliquary")},4,2,TEXT("revolver")},
 {TEXT("THE FINAL PICTURE SHOW"),TEXT("The last evacuation message was hidden on a film reel at a private drive-in."),{TEXT("Find the emergency reel"),TEXT("Power the projector"),TEXT("Survive the noise and collect the cache")},3,4,TEXT("doublebarrel")},
 {TEXT("COLDWATER LIGHTHOUSE"),TEXT("The keeper kept a signal burning long after rescue stopped coming."),{TEXT("Read the keeper's log"),TEXT("Repair the generator with three scrap"),TEXT("Relight the beacon")},0,5,TEXT("sniper")},
 {TEXT("THE GLASS CONSERVATORY"),TEXT("A corporate food experiment outlived the people monitoring its containment."),{TEXT("Inspect the containment controls"),TEXT("Open the quarantine chamber"),TEXT("Defeat the escaped specimen")},1,6,TEXT("flamethrower")},
 {TEXT("IRONJAW ARENA"),TEXT("An automated fight promoter still pays challengers who survive its final program."),{TEXT("Register as a challenger"),TEXT("Begin the arena program"),TEXT("Defeat the arena champions")},4,7,TEXT("minigun")},
 {TEXT("APOLLO ROCKET GARDEN"),TEXT("A civilian space museum concealed a military telemetry station."),{TEXT("Restore exhibit power"),TEXT("Decode the launch telemetry"),TEXT("Open the flight director's locker")},3,2,TEXT("missile_launcher")},
 {TEXT("WEATHER CROWN"),TEXT("A research station can briefly override the region's emergency weather controls."),{TEXT("Restore the weather receiver"),TEXT("Calibrate the storm array"),TEXT("Choose a weather override")},0,8,TEXT("m4")},
 {TEXT("THE SILENT COURTHOUSE"),TEXT("The last public inquiry named the corporations responsible. Its evidence survived."),{TEXT("Recover the hearing transcript"),TEXT("Authenticate the evidence archive"),TEXT("Publish or suppress the findings")},4,9,TEXT("desert_eagle")},
 {TEXT("MUSEUM OF PROSPERITY"),TEXT("Corporate exhibits celebrate a golden age that never reached ordinary citizens."),{TEXT("Inspect the corporate exhibit"),TEXT("Recover the curator's correction"),TEXT("Unlock the hidden collection")},4,0,TEXT("revolver")},
 {TEXT("TITAN GRAVE EXCAVATION"),TEXT("Excavators uncovered a growth experiment, then abandoned their equipment overnight."),{TEXT("Recover the survey notes"),TEXT("Activate the buried transponder"),TEXT("Defeat the excavation guardian")},3,6,TEXT("lmg")},
 {TEXT("BLACK BOX RIDGE"),TEXT("A government courier aircraft went down before the borders were sealed."),{TEXT("Recover the flight recorder"),TEXT("Decode the cargo manifest"),TEXT("Open the courier's strongbox")},3,3,TEXT("smg")},
 {TEXT("NEON MIRAGE MOTOR COURT"),TEXT("Every room was booked to the same name on the night of the evacuation."),{TEXT("Read the guest register"),TEXT("Find the real room sequence"),TEXT("Open the missing guest's safe")},1,2,TEXT("sawedoff")},
 {TEXT("SWITCHBACK RADIO TELESCOPE"),TEXT("An unattended receiver has been collecting a repeating signal from outside the wall."),{TEXT("Repair the receiver"),TEXT("Align the three channels"),TEXT("Record the distant transmission")},0,2,TEXT("sniper")},
 {TEXT("REDWOOD EMERGENCY ARK"),TEXT("A shelter caretaker is waiting for someone to restore the life-support relay."),{TEXT("Inspect the shelter log"),TEXT("Repair life support with three scrap"),TEXT("Release the caretaker's reserve")},1,5,TEXT("rifle")}
 };return D[FMath::Clamp(T-First,0,Count-1)];}
inline bool Unique(int T){return T>=First&&T<First+Count;}
inline FVector2D Size(int T){if(T==Airport)return {30000,22000};return Get(T).Layout==2?FVector2D(8200,3000):Get(T).Layout==3?FVector2D(8200,7200):FVector2D(5800,5800);}
}
