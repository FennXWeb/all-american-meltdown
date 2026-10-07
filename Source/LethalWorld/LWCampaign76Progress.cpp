#include "LWCampaign76.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWVehicle.h"
#include "LWCanada68.h"
#include "LWLoading45.h"
#include "LWDialogue59.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/DamageEvents.h"

bool ALWCampaign76::CastAvailable(FName Id)const{
 const auto& V=State().Values;if(!LWCampaign76::Alive(State(),Id))return false;
 const auto* S=LWCampaign76::Stage(State().Stage);if(!S)return false;
 if(Id==TEXT("mara")&&V.FindRef(TEXT("mara_stayed"))&&S->Site!=TEXT("canada"))return false;
 if(S->Site==TEXT("canada")){
  if(Id==TEXT("tomas")&&!V.FindRef(TEXT("tomas_released")))return false;
  if((Id==TEXT("simon")||Id==TEXT("ruth"))&&(!V.FindRef(TEXT("witness_travels"))||V.FindRef(TEXT("witness_surrendered"))))return false;
 }
 if(Id==TEXT("simon")&&V.FindRef(TEXT("witness_surrendered"))&&S->Site!=TEXT("witness"))return false;
 if((Id==TEXT("simon")||Id==TEXT("ruth"))&&S->Site!=TEXT("witness")&&S->Site!=TEXT("freehold")&&!V.FindRef(TEXT("witness_travels")))return false;
 if(Id==TEXT("tomas")&&V.FindRef(TEXT("tomas_released"))&&S->Site==TEXT("labor"))return false;
 if(Id==TEXT("tomas")&&!V.FindRef(TEXT("tomas_released"))&&S->Site==TEXT("crossing"))return false;
 if(S->Id==TEXT("ally_example")&&Id!=TEXT("richardson")){
  const FName Candidates[]={TEXT("mara"),TEXT("della"),TEXT("hannah"),TEXT("ada"),TEXT("rook"),TEXT("ivo"),TEXT("tessa"),TEXT("vera")};const int K=V.FindRef(TEXT("condemned"))-1;return K>=0&&K<8&&Candidates[K]==Id;
 }
 return true;
}
bool ALWCampaign76::Hostile(FName Id)const{
 const auto S=State().Stage;
 if(S==TEXT("keene_fight")&&Id==TEXT("keene"))return true;
 if((S==TEXT("richardson_final")||S==TEXT("loyal_takeover"))&&Id==TEXT("richardson"))return true;
 if(S==TEXT("loyal_final")&&(Id==TEXT("rook")||Id==TEXT("della")||Id==TEXT("hannah")||Id==TEXT("ada")))return true;
 return false;
}
bool ALWCampaign76::Travel(FName Destination){
 if(!Player||!World)return false;
 if(Destination==TEXT("canada")){
  const bool ProcessedFerry=State().Values.FindRef(TEXT("ferry_landed77"))&&LWCanada68::Authorized(Player);
  if(!State().Values.FindRef(TEXT("humanitarian_permit"))||(!ProcessedFerry&&!LWCanada68::Deposit(Player))){Player->Notify(TEXT("Complete weapons processing before transfer."));return false;}
 }
 if(Player->Vehicle)Player->Vehicle->Exit(true);Player->CancelReload();Player->StandFromChair(true);
 const FVector To=LWCampaign76::Frame(Destination).TransformPosition(FVector(0,-300,92));
 {FLWLoadingScope45 Loading(Destination==TEXT("canada")?TEXT("Northbank transfer to Toronto"):TEXT("Returning through the American checkpoint"));Player->SetActorLocation(To,false,nullptr,ETeleportType::TeleportPhysics);World->Stream(To,true);}
 Player->GetCharacterMovement()->StopMovementImmediately();Player->bSafehouse=false;
 if(Destination==TEXT("crossing"))LWCanada68::Reclaim(Player);
 Player->RequestSave40();return true;
}
void ALWCampaign76::PrepareStage(){
 const FName Id=State().Stage;auto& V=State().Values;
 if(Id==TEXT("resistance_exit")||Id==TEXT("final_evacuation")||Id==TEXT("meridian_inside"))V.Remove(TEXT("crowd_clear77"));
 if(Id==TEXT("ally_example")&&!V.FindRef(TEXT("condemned"))){
  const FName Candidates[]={TEXT("mara"),TEXT("della"),TEXT("hannah"),TEXT("ada"),TEXT("rook"),TEXT("ivo"),TEXT("tessa"),TEXT("vera")};
  int Pick=0,Best=MIN_int32;for(int I=0;I<8;I++){if(!V.FindRef(FName(*(TEXT("met_")+Candidates[I].ToString())))||!LWCampaign76::Alive(State(),Candidates[I])||(I==0&&V.FindRef(TEXT("mara_stayed"))))continue;const int Trust=V.FindRef(FName(*(TEXT("trust_")+Candidates[I].ToString())))+(I==0?2:0);if(Trust>Best){Best=Trust;Pick=I+1;}}V.Add(TEXT("condemned"),Pick);
 }
 if(Id==TEXT("ending_resistance")||Id==TEXT("ending_loyal")||Id==TEXT("ending_other")){
  const FString Result=LWCampaign76::Ending(State());State().Ending=FName(*Result);
  const TCHAR* Flag=Result==TEXT("Open Country")?TEXT("ending_open"):Result==TEXT("New Uniforms")?TEXT("ending_uniforms"):Result==TEXT("A Victory of Ash")?TEXT("ending_ash"):Result==TEXT("First Citizen")?TEXT("ending_citizen"):Result==TEXT("Useful Until Dawn")?TEXT("ending_discarded"):Result==TEXT("The Inheritance")?TEXT("ending_inheritance"):TEXT("ending_other");V.Add(Flag,1);
  if((Result==TEXT("First Citizen")||Result==TEXT("The Inheritance"))&&!V.FindRef(TEXT("command_awarded"))){
   auto& Settlement=Player->RPG.Settlements.FindOrAdd(TEXT("c76_meridian"));Settlement.Name=TEXT("Meridian Regional Command");Settlement.Center=LWCampaign76::Frame(TEXT("meridian")).GetLocation();Settlement.Reputation=100;Settlement.Member=Settlement.Leader=true;Settlement.LastEconomyHour=Player->WorldHour();Settlement.Size=2;V.Add(TEXT("command_awarded"),1);Player->Money+=500;
  }
  Player->MissionRecovery37.Remove(TEXT("campaign76"));
 }
}
