#include "LWFuel.h"
#include "LWCharacter.h"
#include "LWWorldObject.h"

FLWItemDefinition LWFuel::GasCanDefinition(){
 FLWItemDefinition D;D.Id=TEXT("gas_can");D.DisplayName=FText::FromString(TEXT("Gas Can (20 L)"));
 D.Category=TEXT("Tool");D.Width=2;D.Height=3;D.Price=80;D.MaxStack=1;
 D.Capacity=CanCapacity;D.EquipSlots.Add(TEXT("Tool"));return D;
}
FLWItemInstance LWFuel::MakeGasCan(int32 Litres){
 auto I=LWItems::Make(TEXT("gas_can"));if(I.Id.IsValid())I.Rounds=FMath::Clamp(Litres,0,CanCapacity);return I;
}
int32 LWFuel::RefillGasCans(ALWCharacter* P,ALWWorldObject* Pump){
 if(!IsValid(P)||!P->CanAct()||!IsValid(Pump)||Pump->World!=P->World||Pump->Kind!=ELWObjectKind::FuelPump||Pump->bChanged||FVector::Dist(Pump->GetActorLocation(),P->GetActorLocation())>400)return 0;
 int32 Added=0;for(auto& I:P->Inventory)if(I.Definition==TEXT("gas_can")&&I.Count==1){
  const int32 Before=FMath::Clamp(I.Rounds,0,CanCapacity);Added+=CanCapacity-Before;I.Rounds=CanCapacity;
 }return Added;
}
