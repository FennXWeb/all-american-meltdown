#include "LWAircraft84.h"
#include "LWVehicle.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWResident.h"
#include "LWCanada68.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
namespace LWAviation84 {
void OpenService(ALWCharacter* P,FVector Where){if(!P||!P->World||P->Vehicle)return;const auto* Airport=Closest(FVector2D(Where));if(!Airport)return;ALWWorldObject* Terminal=nullptr;for(TActorIterator<ALWWorldObject> O(P->GetWorld());O;++O)if(O->UseType==TEXT("airfield84")&&FVector::DistSquared(O->GetActorLocation(),Where)<100){Terminal=*O;break;}if(!Terminal)return;
 P->ClosePanels();P->Service66=Terminal;P->RPGPanel=4;P->DialogueText=Airport->Name+TEXT(" / aircraft services");P->DialogueActions.Empty();P->DialogueChoices.Empty();TArray<FName> Ids;for(const auto& Pair:P->World->Vehicles)if(Pair.Value.Owned45&&LWTraffic::IsAircraft(Pair.Value.Model))Ids.Add(Pair.Key);Ids.Sort(FNameLexicalLess());for(FName Id:Ids){const auto& R=P->World->Vehicles[Id];P->DialogueChoices.Add(FString::Printf(TEXT("Recover / service %s · %s"),LWTraffic::Get(R.Model).Name,*R.VIN.ToString().Right(6)));P->DialogueActions.Add(FName(*(TEXT("field84:")+Id.ToString())));}if(Ids.IsEmpty())P->DialogueText+=TEXT("\nClaim an aircraft at its flight computer to use recovery and refuelling here.");P->DialogueActions.Add(TEXT("bye"));P->DialogueChoices.Add(TEXT("Leave"));P->SetMenuInput(true);
}
bool Recover(ALWCharacter* P,FName Id,const FRunway& Airport){if(!P||!P->World||P->Vehicle)return false;auto* R=P->World->Vehicles.Find(Id);if(!R||!R->Owned45||!LWTraffic::IsAircraft(R->Model))return false;
 ALWVehicle* Existing=nullptr;for(TActorIterator<ALWVehicle> V(P->GetWorld());V;++V)if(V->RecordId==Id){Existing=*V;break;}if(Existing&&!R->Exploded&&(Existing->Driver||Existing->EngineOn||!Existing->AircraftOnGround84||Existing->Speed>5||Existing->Passengers.ContainsByPredicate([](const auto& N){return IsValid(N);} ))){P->Notify(TEXT("AIRCRAFT IS IN USE"));return false;}
 FVector At;bool Clear=false;for(int I=0;I<12;I++){const FVector D=Airport.Direction(),Right(-D.Y,D.X,0);At=Airport.Apron+D*((I%3-1)*6000)+Right*(4500+(I/3)*6000);bool Occupied=false;for(const auto& Pair:P->World->Vehicles)if(Pair.Key!=Id&&!Pair.Value.Stored45&&!Pair.Value.Exploded&&FVector::Dist2D(Pair.Value.Position,At)<LWTraffic::Get(Pair.Value.Model).HalfWidth+LWTraffic::Get(R->Model).HalfWidth+600){Occupied=true;break;}if(!Occupied){Clear=true;break;}}
 if(!Clear){P->Notify(TEXT("APRON FULL / MOVE AN AIRCRAFT TO FREE A SPACE"));return false;}
 // Destroy first: EndPlay writes the old transform, so it must not overwrite the
 // recovered record after it has been moved. Individual compartment IDs stay intact.
 if(Existing){Existing->Destroy();Existing=nullptr;}R=P->World->Vehicles.Find(Id);R->Position=At;R->Rotation=Airport.Direction().Rotation();R->Health=200;R->FireRemaining=0;R->Exploded=false;R->Stored45=false;R->Gear84=true;R->FlightPhase84=0;R->Autopilot84=R->FlightEngine84=false;R->FlightVelocity84=FVector::ZeroVector;R->Thrust84=0;R->FuelLitres=Spec(R->Model).Capacity;R->Dents.Empty();
 auto* V=Cast<ALWVehicle>(P->World->SpawnObject(ELWObjectKind::Car,Id,R->Position,R->Rotation));if(!V){P->Notify(TEXT("AIRCRAFT RECOVERED TO THE MARKED APRON"));}else P->Notify(TEXT("AIRCRAFT RECOVERED / REFUELLED / REPAIRED"));P->SetWaypoint(FVector2D(At));P->PersistWorldChange();return true;
}
bool UseService(ALWCharacter* P,FName Action){if(!P)return false;const FString A=Action.ToString();if(P->Service66&&P->Service66->UseType==TEXT("airfield84")){if(A==TEXT("bye")){P->ClosePanels();return true;}if(A.StartsWith(TEXT("field84:"))){if(FVector::Dist(P->GetActorLocation(),P->Service66->GetActorLocation())>550){P->ClosePanels();return true;}const auto* Field=Closest(FVector2D(P->Service66->GetActorLocation()));if(Field&&Recover(P,FName(*A.Mid(8)),*Field))P->ClosePanels();return true;}}
 auto* V=P->Vehicle.Get();if(!V||!V->IsAircraft84()||!V->Record())return false;if(A==TEXT("bye")&&!P->RestBed&&!P->Speaker){P->ClosePanels();return true;}if(!A.StartsWith(TEXT("air84_")))return false;auto* R=V->Record();
 if(A==TEXT("air84_auto")){P->ClosePanels();V->ToggleAutopilot84();return true;}
 if(A==TEXT("air84_claim")){R->Owned45=true;R->Unlocked=R->Hotwired=true;V->MarkLastDriven();P->Notify(TEXT("AIRCRAFT CLAIMED / AIRPORT RECOVERY AVAILABLE"));P->PersistWorldChange();P->ClosePanels();return true;}
 if(A==TEXT("air84_engine")){P->ClosePanels();if(V->PlayerSeat!=-1)P->Notify(TEXT("USE THE PILOT SEAT TO CONTROL THE ENGINES"));else V->Ignition();return true;}
 if(A==TEXT("air84_brighter"))R->CabinBrightness84=FMath::Min(1.f,R->CabinBrightness84+.1f);if(A==TEXT("air84_dimmer"))R->CabinBrightness84=FMath::Max(0.f,R->CabinBrightness84-.1f);
 if(A==TEXT("air84_warm"))R->CabinColor84=FLinearColor(1,.80,.57);if(A==TEXT("air84_cool"))R->CabinColor84=FLinearColor(.65,.83,1);if(A==TEXT("air84_blue"))R->CabinColor84=FLinearColor(.12,.3,1);if(A==TEXT("air84_violet"))R->CabinColor84=FLinearColor(.68,.14,1);if(A==TEXT("air84_amber"))R->CabinColor84=FLinearColor(1,.34,.035);
 if(A==TEXT("air84_redplus"))R->CabinColor84.R=FMath::Min(1.f,R->CabinColor84.R+.1f);if(A==TEXT("air84_redminus"))R->CabinColor84.R=FMath::Max(0.f,R->CabinColor84.R-.1f);
 if(A==TEXT("air84_greenplus"))R->CabinColor84.G=FMath::Min(1.f,R->CabinColor84.G+.1f);if(A==TEXT("air84_greenminus"))R->CabinColor84.G=FMath::Max(0.f,R->CabinColor84.G-.1f);
 if(A==TEXT("air84_blueplus"))R->CabinColor84.B=FMath::Min(1.f,R->CabinColor84.B+.1f);if(A==TEXT("air84_blueminus"))R->CabinColor84.B=FMath::Max(0.f,R->CabinColor84.B-.1f);
 if(A==TEXT("air84_shadesopen"))R->Shades66.Empty();if(A==TEXT("air84_shadesclose"))for(int I=0;I<V->AircraftShades84.Num();I++)R->Shades66.Add(I);V->TickAircraftCabin84(0);P->RequestSave40();return true;
}
}
