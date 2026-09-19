# Vehicle / convoy handoff

Implemented without compiling, UHT, packaging, or modifying map UI.

## Map contract

- `ALWVehicle::LastDrivenVIN(const ALWWorld* W)` returns an `FGuid`; invalid means no recorded driver vehicle.
- `ALWVehicle::LastDrivenId(const ALWWorld* W)` returns the record key; `NAME_None` means none.
- Persistent storage: `World->Vehicles[Id].LastDriven`, alongside existing `VIN`, `Position`, `Rotation`, and `Model`. Existing world save/load copies this struct.
- Taking the driver seat clears other records' markers and marks this vehicle. Passenger entry does not change it. Exit, streaming unload, and destruction preserve the marker and last position. The map can choose how to present a wreck.
- No live actor is needed to resolve the marker. Old saves have no marker until the player takes a driver seat.

## Euler HUD contract (relay through coordinator)

- Definition `gas_can`, label `Gas Can (20 L)`, category `Tool`, footprint 2x3, stack limit 1, equipment slot `Tool`.
- `FLWItemInstance::Rounds` stores whole litres, 0..20; `LWFuel::CanCapacity` is 20. New cans start full. Empty cans remain reusable.
- Suggested descriptor: `Fuel: <Rounds> / 20 L`. This is not flamethrower ammunition or a magazine.
- Live vehicle: `FuelLitres()` / `FuelCapacity()`. Persistent record `FuelLitres` defaults to -1 for migration; initialized vehicles start at 65% of their model's capacity.
- Inventory defaults and new-can fill are already added to `LWInventory.cpp`; no additional catalog edit is needed.

## Fuel pump integration (now wired by coordinator)

Vehicle refilling already works: equip a gas can in Tool, switch off the parked vehicle's engine, exit, then use the vehicle. Transfer conserves whole litres, retains unused can contents, and saves inventory/world changes. A tank with less than one litre of space is considered full.

To refill the reusable cans at existing pumps, in `LWCombat29.cpp`:

1. Include `LWFuel.h`.
2. In `ALWCharacter::RefillCanisters`, after its existing validation, replace `int Fuel=0;` with `int Fuel=LWFuel::RefillGasCans(this,Pump);`.
3. Keep the existing flamethrower-tank refill loop and the `if(Fuel)` save/audio/HUD path. Adjust feedback to mention gas cans and flamethrower canisters.
4. Update the pump prompt in `LWWorldObject.cpp` to mention gas cans as well as flamethrower canisters.

The helper independently validates player state, pump integrity, world identity, and 400-unit reach; it returns added litres and leaves saving/feedback to the caller. The coordinator has now wired the character handler and pump prompt; the focused smoke uses the actual pump interaction.

## Implemented behavior

- Free usable player-car passenger seats retain companion priority even outside boarding range. Chauffeur dismissal no longer blocks passenger boarding.
- Overflow drivers unlock/hotwire and start immediately; no artificial 4/10-second wait. The owner can take over their own stopped convoy car.
- Companion navigation excludes hidden pending spawns and empty-fuel cars. AI obstacle sweeps detect other vehicles.
- Full oriented model footprints, including the RV's overhang, reserve space against live and unloaded vehicle records. Full-height obstruction checks and bounded relocation search avoid publishing overlapping vehicle spawns. No safe location leaves a hidden, non-colliding actor that retries every two simulation seconds.
- Fuel burns while running, scales with model size and movement/load, and stops propulsion at empty. Autopilot/convoy refuse fuel-less driving.
- RV cargo gets a full can; other vehicles get one on a deterministic 25% roll, once per record, when cargo space permits.

## Ownership and verification

Changed vehicle files: `LWVehicle.h/.cpp`, `LWVehicleAI.cpp`, `LWConvoy.cpp`, `LWVehicleState.h`.
New files: `LWFuel.h/.cpp`, `LWVehicleFuel.cpp`, `LWVehiclePlacement.cpp`.
Authorized inventory change: gas-can definition and initial contents in `LWInventory.cpp`.
Necessary shared edit: `LWSocial.cpp` boarding guards and overflow target guard only. This makes the actual companion movement obey player-seat priority and keeps passenger boarding available after chauffeur dismissal.

Source contract checks passed for persistence fields, map accessor, inventory registration, boarding/navigation priority, removed delay, and pending-spawn gate. No Unreal compile or runtime test was run. This workspace has no accessible Git repository, so no Git diff/check was available.

Coordinator runtime checks: separated RV/sedan and rotated spawns; crowded-spawn retry; distant companion approaching the player's free seat; full-car overflow departure; stopped convoy takeover; driver/passenger last-VIN behavior across save/load and streaming; fuel exhaustion in manual/autopilot/convoy driving; partial-can transfer, empty-can retention, and intact/burnt pump refill after the hook above.


## Vehicle30 focused runtime smoke (ready for coordinator compile)

Run after compiling the editor target, from PowerShell:

```powershell
& 'X:/LethalWorld/Tools/VerifyVehicle30.ps1'
```

The script does not build. It launches the verified local editor path `G:/Epic/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe`, project `X:/LethalWorld/LethalWorld.uproject`, map `/Game/Maps/Wasteland`, with `-game -LWV17Smoke -LWVehicle30Smoke -LWAudioSmoke -RenderOffscreen -windowed -ResX=1280 -ResY=720 -unattended -nop4 -nosplash -abslog=X:/LethalWorld/Saved/Vehicle30_Runtime.log`.

Both smoke flags are required. `LWV17Smoke` selects `AllAmericanMeltdown_AutomationV17`, keeping normal survivor saves untouched. Success marker: `LW_VEHICLE30_DONE failures=0 checks=...`. The script checks the marker and exit status and enforces a 240-second process timeout.

`LWVehicle30Smoke.inl` covers RV/rotated-sedan overlap using actual resolved transforms, manual driving/fuel exhaustion/no-fuel ignition, partial and full gas-can transactions, actual intact/burnt pump interaction, archive persistence and last-VIN accessors, player-seat priority, and full-car overflow engine start before any convoy tick. Driving fixtures explicitly reposition and sync vehicles onto the test floor after verifying spawn relocation.

The pending-spawn regression reserves the origin and every bounded-search sample using *other* saved records, verifies hidden/non-colliding state through a retry, removes those blockers, and retries while the pending car's own saved record remains. Production placement excludes `Pair.Key == RecordId`; the regression checks that its own saved position cannot keep it hidden. Genuine external blockers can still keep a car pending until space becomes available; the bounded search does not promise availability in a completely occupied area.

Registration edits: one declaration in `LWGameMode.h`, include and completion marker in `LWGameMode.cpp`, dispatch in `LWV17Smoke.inl`. No vehicle/state headers changed in this smoke follow-up. Source checks and PowerShell parsing only; no compile or runtime execution performed here.


## Stopped overflow rejoining follow-up

`TickSocial` now calls the convoy-owned `LWConvoy::RejoinPlayerVehicle` helper before returning for a valid overflow ride. No headers were edited for this follow-up. Both cars must be stationary (absolute speed at most 0.01 units/sec), nearby, on the same level, with an unobstructed line between seats. A valid player-car seat is selected before any source occupancy changes. Riders reattach directly using existing boarding transforms, so there is no unreserved/on-foot frame or terrain-exit teleport.

When a convoy driver transfers, a healthy following passenger is promoted inside that car first. If remaining passengers have no eligible replacement, the driver stays aboard. The final driver transfers only when the convoy car is empty and its engine/convoy state can be cleared. Each subsequent rider rechecks the player's seat availability, preventing excess transfers.

The Vehicle30 suite now exercises the actual `TickSocial` entry: passenger refusal with either car moving, stopped passenger transfer, full-player-car retention, refusal to abandon an incapacitated passenger, driver replacement, and final empty-car parking. The runtime command and success marker are unchanged. Source integration checks only; no compile or runtime execution in this follow-up.
