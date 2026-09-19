from pathlib import Path
root=Path('Source/LethalWorld')
def edit(name,old,new):
 p=root/name;s=p.read_text();assert old in s,(name,old[:80]);p.write_text(s.replace(old,new))
edit('LWInventory.cpp','    Add(TEXT("battery"),','    Add(TEXT("lockpick"), TEXT("Lockpicks"), TEXT("Tool"), 1, 2, 12).MaxStack=20;\n    Add(TEXT("car_key"), TEXT("Vehicle Key"), TEXT("Tool"), 1, 1, 15);\n    Add(TEXT("battery"),')
for file in ['LWCharacter.cpp','LWSurvival.cpp','LWWorld.cpp','LWWorldSystems.cpp','LWWeapons.cpp','LWHUD.cpp']:
 p=root/file;s=p.read_text();p.write_text('#include "LWVehicle.h"\n'+s)
edit('LWCharacter.cpp','    I->BindKey(EKeys::K,','    I->BindKey(EKeys::G,IE_Pressed,this,&ALWCharacter::VehicleGlove);I->BindKey(EKeys::H,IE_Pressed,this,&ALWCharacter::VehicleLights);I->BindKey(EKeys::T,IE_Pressed,this,&ALWCharacter::VehicleRadio);I->BindKey(EKeys::C,IE_Pressed,this,&ALWCharacter::VehicleWipers);I->BindKey(EKeys::Z,IE_Pressed,this,&ALWCharacter::VehicleLeft);I->BindKey(EKeys::X,IE_Pressed,this,&ALWCharacter::VehicleRight);\n    I->BindKey(EKeys::K,')
edit('LWCharacter.cpp','Forward(float V){if(CanAct())','Forward(float V){if(Vehicle){Vehicle->Throttle=V;return;}if(CanAct())')
edit('LWCharacter.cpp','Right(float V){if(CanAct())','Right(float V){if(Vehicle){Vehicle->Steer=V;return;}if(CanAct())')
edit('LWCharacter.cpp','Turn(float V){if(CanAct())','Turn(float V){if(SecurityMode==1){PickAngle=FMath::Clamp(PickAngle+V*Sensitivity*4,-90.f,90.f);return;}if(Vehicle&&!bMenu&&!bInventory&&!bMap&&!RPGPanel&&!SecurityMode){SeatYaw=FMath::Clamp(SeatYaw+V*Sensitivity*2,-115.f,115.f);return;}if(CanAct())')
edit('LWCharacter.cpp','Look(float V){if(CanAct())','Look(float V){if(Vehicle&&!bMenu&&!bInventory&&!bMap&&!RPGPanel&&!SecurityMode){SeatPitch=FMath::Clamp(SeatPitch-V*Sensitivity*2,-65.f,55.f);return;}if(CanAct())')
edit('LWCharacter.cpp','ToggleFlashlight(){if(CanAct())','ToggleFlashlight(){if(Vehicle){Vehicle->Exit();return;}if(CanAct())')
edit('LWCharacter.cpp','ClosePanels(){RPGPanel','ClosePanels(){CancelSecurity();RPGPanel')
edit('LWCharacter.cpp','OpenContainer(ALWWorldObject* O){ClosePanels();','OpenContainer(ALWWorldObject* O){if(IsLocked(O)){StartLockpick(O);return;}ClosePanels();')
edit('LWCharacter.cpp','FocusPrompt()const{return ResidentFocus?','FocusPrompt()const{if(Vehicle)return Vehicle->CabinPrompt();return ResidentFocus?')
edit('LWCharacter.cpp','if(bInventory||bMap||RPGPanel){ClosePanels();return;}','if(bInventory||bMap||RPGPanel||SecurityMode){ClosePanels();return;}')
edit('LWCharacter.cpp','if(ResidentFocus)Talk(ResidentFocus);','if(Vehicle){Vehicle->Control(Vehicle->FocusControl());return;}if(ResidentFocus)Talk(ResidentFocus);')
edit('LWCharacter.cpp','!Enabled&&ActiveGun()!=nullptr','!Enabled&&!Vehicle&&ActiveGun()!=nullptr')
edit('LWCharacter.cpp','    RPG=FLWRPGState();','    if(Vehicle)Vehicle->Exit(true);\n    RPG=FLWRPGState();')
edit('LWCharacter.cpp','World->PropStates.Empty();World->Reset();','World->PropStates.Empty();World->Reset();World->Vehicles.Empty();')
edit('LWCharacter.cpp','    TickRPG(Dt);UpdateWeapon(Dt);TickRoute(Dt);','    TickVehicleSeat();TickSecurity(Dt);TickRPG(Dt);if(!Vehicle)UpdateWeapon(Dt);TickRoute(Dt);')
edit('LWWeapons.cpp','void ALWCharacter::Reload()\n{','void ALWCharacter::Reload()\n{\n    if(Vehicle){Vehicle->Ignition();return;}')
edit('LWSurvival.cpp','FString SaveSlot(){','FString SaveSlot(){if(FParse::Param(FCommandLine::Get(),TEXT("LWV5Smoke")))return TEXT("LethalWorld_AutomationV5");')
edit('LWSurvival.cpp','    GiveItem(TEXT("ammo_12g"),24);','    GiveItem(TEXT("lockpick"),8);\n    GiveItem(TEXT("ammo_12g"),24);')
edit('LWSurvival.cpp','    if(Panel==0)return &Inventory;','    if(Panel==0)return &Inventory;\n    if(IsLocked(OpenObject))return nullptr;')
edit('LWSurvival.cpp','    if(bDeadSaved)return;ClosePanels();','    if(bDeadSaved)return;if(Vehicle)Vehicle->Exit(true);ClosePanels();')
edit('LWSurvival.cpp','    ClosePanels();CancelReload();bSafehouse=true;','    if(Vehicle)Vehicle->Exit(true);ClosePanels();CancelReload();bSafehouse=true;')
edit('LWSurvival.cpp','    S->Version=2;S->RPG=RPG;','    S->Version=2;S->RPG=RPG;S->Vehicles=World->Vehicles;')
edit('LWSurvival.cpp','    UGameplayStatics::SetGamePaused(this,false);ClosePanels();CancelReload();World->Reset();','    UGameplayStatics::SetGamePaused(this,false);if(Vehicle)Vehicle->Exit(true);ClosePanels();CancelReload();World->Reset();World->Vehicles=S->Vehicles;')
# Save parked beside vehicle instead of inside a solid chassis on restoration.
edit('LWSurvival.cpp','    S->Inventory=Inventory;','    if(Vehicle){FVector At=Vehicle->GetActorLocation()+Vehicle->GetActorRightVector()*185;At.Z=World->HeightAt(FVector2D(At))+110;S->Position=At;}\n    S->Inventory=Inventory;')
edit('LWWorld.cpp','    for(TActorIterator<ALWResident> I(GetWorld());I;++I)I->Destroy();','    for(TActorIterator<ALWVehicle> I(GetWorld());I;++I)I->Destroy();\n    for(TActorIterator<ALWResident> I(GetWorld());I;++I)I->Destroy();')
edit('LWWorldObject.cpp','    switch(Kind){','    if(World)if(const auto* R=World->Containers.Find(RecordId))if(R->LockTier&&!R->Unlocked)return FString::Printf(TEXT("[E] PICK LOCK // TIER %d"),R->LockTier);\n    switch(Kind){')
edit('LWWorldObject.cpp','    if(!P||P->Health<=0)return;','    if(!P||P->Health<=0)return;\n    if(P->IsLocked(this)){P->StartLockpick(this);return;}')
# Routing is intercepted only for vehicle objects; all prior prop paths remain intact.
edit('LWWorldSystems.cpp','    ALWWorldObject* O=GetWorld()->SpawnActor<ALWWorldObject>(P,R,Params);\n    if(O)O->Configure(this,Kind,Id);return O;', '''    if(Kind==ELWObjectKind::Car){
        for(TActorIterator<ALWVehicle> I(GetWorld());I;++I)if(I->RecordId==Id)return nullptr;
        if(const auto* V=Vehicles.Find(Id)){auto* Player=UGameplayStatics::GetPlayerPawn(this,0);if(Player&&FVector::Dist2D(Player->GetActorLocation(),V->Position)>(RenderRadius+1)*LWGen::ChunkSize)return nullptr;P=V->Position;R=V->Rotation;}
        auto* Car=GetWorld()->SpawnActor<ALWVehicle>(P,R,Params);if(Car){Car->Configure(this,Kind,Id);Car->InitializeVehicle();}return Car;
    }
    ALWWorldObject* O=GetWorld()->SpawnActor<ALWWorldObject>(P,R,Params);
    if(O)O->Configure(this,Kind,Id);return O;''')
edit('LWWorldSystems.cpp','#include "LWVehicle.h"','#include "LWVehicle.h"\n#include "EngineUtils.h"')
edit('LWWorldSystems.cpp','void ALWWorld::SpawnRecords(ALWChunk* Chunk)\n{','''void ALWWorld::SpawnRecords(ALWChunk* Chunk)
{
    TArray<FName> Parked;for(const auto& Pair:Vehicles)if(LWGen::ChunkAt(FVector2D(Pair.Value.Position))==Chunk->Coordinate)Parked.Add(Pair.Key);
    for(FName Id:Parked){const auto V=Vehicles.FindChecked(Id);if(auto* Car=SpawnObject(ELWObjectKind::Car,Id,V.Position,V.Rotation))Chunk->Residents.Add(Car);}''')
edit('LWWorldSystems.cpp','Record.Context=Contexts[Site.Type];Record.Position=P;','''Record.Context=Contexts[Site.Type];Record.Position=P;
            uint32 SecurityHash=Site.Id^(uint32(I+1)*7919u);Record.LockTier=(!Site.Friendly&&SecurityHash%100<55)?1+int32(SecurityHash%4):0;''')
edit('LWWorldSystems.cpp','float(Site.Position.Size()/1000000.)+Bonus','float(Site.Position.Size()/1000000.)+Bonus+Record.LockTier*.24f')
edit('LWWorldSystems.cpp','            for(auto Item:Roll)AddRecordItem(Record,Item);','''            for(auto Item:Roll)AddRecordItem(Record,Item);
            if(Record.LockTier){const FName BonusItems[]={TEXT("ammo_12g"),TEXT("medkit"),TEXT("rifle"),TEXT("lmg")};auto Reward=LWItems::Make(BonusItems[Record.LockTier-1],Record.LockTier==1?12:Record.LockTier==2?2:1);Reward.Id=FGuid::NewDeterministicGuid(Id.ToString()+TEXT("lock_bonus"));AddRecordItem(Record,Reward);}
            auto Picks=LWItems::Make(TEXT("lockpick"),2);Picks.Id=FGuid::NewDeterministicGuid(Id.ToString()+TEXT("picks"));AddRecordItem(Record,Picks);''')
# Existing traders get a deterministic replenishment-free initial pick stock.
edit('LWWorldObject.cpp','        Body->SetCollisionObjectType(ECC_WorldDynamic);','''        if(auto* Stock=W->Containers.Find(Id))if(!Stock->Items.ContainsByPredicate([](const auto& I){return I.Definition==TEXT("lockpick");})&&!W->PropStates.Contains(FName(*(Id.ToString()+TEXT("_picks")))){auto Pick=LWItems::Make(TEXT("lockpick"),12);LWItems::Place(Stock->Items,Pick,Stock->Width,Stock->Height);W->PropStates.Add(FName(*(Id.ToString()+TEXT("_picks"))),1);}
        Body->SetCollisionObjectType(ECC_WorldDynamic);''')
edit('LWHUD.cpp','    if(RPGClick(P,N))return;','    if(P->SecurityMode){if(N==TEXT("security_close"))P->ClosePanels();else if(N.ToString().StartsWith(TEXT("wire_")))P->ChooseWire(FCString::Atoi(*N.ToString().Mid(5)));return;}\n    if(RPGClick(P,N))return;')
edit('LWHUD.cpp','    if(P->RPGPanel){','    if(P->SecurityMode){SecurityScreen(P);return;}\n    if(P->RPGPanel){')
edit('LWHUD.cpp','    MiniMap(P);RPGOverlay(P);','    MiniMap(P);RPGOverlay(P);if(P->Vehicle)VehicleOverlay(P);')
edit('LWHUD.cpp','BUILD 0.4    //    SURVIVOR NETWORK','BUILD 0.5    //    IGNITION')
