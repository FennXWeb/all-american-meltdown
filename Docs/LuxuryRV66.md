# Luxury motorhome and vehicle delivery

The existing `rv` vehicle type is rebuilt in place. Old vehicle IDs, VINs, keys, fuel,
upgrades, dents and cargo records are retained. No packaged build is produced.

## Cabin

- Six travel seats: driver, copilot, two opposing dinette seats and two lounge seats.
- Kitchen with induction hob, recessed sink and brass faucet, fridge, overhead
  cupboards, pantry and wardrobe. Use the hob with a food ration to eat a hot meal.
  Turn the faucet on for a visible water stream and filtered drinking water.
- Rear master bed supports sleeping and respawning. Setting it as the respawn point
  claims the vehicle and displays it on the owned-vehicle map layer. Claiming a bed
  does not consume bunker garage bays; garage allocation happens when storing it.
- Two banks of three companion bunks, with mattresses, quilts, pillows, guard rails
  and access ladders. Use each bunk to choose a crew member or clear its assignment.
  Assigning a bunk dismisses that companion to the RV. Recruiting them to follow
  preserves their home assignment for the next dismissal. Settlement/utility-room
  assignments replace the RV assignment. An unavailable or destroyed RV falls back
  to the companion's bunker bedroom until the vehicle is available again.
- Lounge and bedroom light circuits, ceiling downlights and ventilation grilles,
  framed art, outlets, tableware, upholstery piping and interior trim.
- Two televisions share channel state: civil radio/teletext, coach status/clock,
  live exterior security camera, then off. The exterior camera renders at 512×288,
  twice per second, only while selected and the player is near the RV.
- Fourteen independently operated pleated shades cover the side windows, slide
  rooms, cockpit, windshield and entrance door. Use a roller or shade to toggle it.
- Separate cargo hatches and furniture inventories. The first exterior hatch still
  uses the legacy shared vehicle cargo ID. Fridge, wardrobe and pantry IDs are kept.

## Camping and travel

Three switches beside the entrance operate lounge lighting, both slide rooms and
the side awning. A rear switch controls bedroom lighting. Park and turn the engine
off before extending equipment. Extension checks world clearance.

The dinette and lounge slide outward with their furniture, windows and blinds.
Ignition automatically retracts slide rooms and awning before the engine starts.
The same interlock protects companion and convoy driving. Press ignition again to
cancel a pending start. The awning takes about 4.4 seconds; slide rooms about 6.3.

## Paid vehicle delivery

Parking-lot POIs generate a dedicated terminal and reserved delivery area. Use the
terminal on foot and select an owned vehicle by model and VIN suffix. Delivery
costs **150 credits**, charged only after successful placement.

The service checks footprint support and full-height clearance, loaded vehicles,
occupants/drivers, motion, engine state, damage, active garage transfers and funds.
It can bring a loaded world vehicle or an unloaded/stored owned vehicle. The same
VIN, cargo and modifications are kept; existing garage reservations remain. Failed
delivery does not charge money or remove the vehicle from storage. The destination
receives a waypoint. Vehicles actively in use or destroyed cannot be called.

## Source and assets

- `Tools/build_rv66.py`: Blender geometry, authored in metres with +X forward.
- `ArtSource/RV66/LuxuryCoach66.blend`, individual FBXs and manifest: 27 models.
- `Tools/import_rv66.py`: materials, validated FBX bounds, query collision and three
  LOD levels; imported meshes keep CPU access for dent deformation.
- `Content/Art/Meshes/SM_RV66_*`, `Content/Materials/M_RV66_*`.
- Existing vehicle audio, instrument, wheel, door and windscreen systems are reused.
  Interior art uses the existing generated Interiors65 artwork collection.
- Cabin equipment persists in `FLWVehicleRecord`; companion homes persist in
  `FLWCrewRecord`. Missing properties on older saves take their default values.

## Validation

Editor target compilation and `LWCamper66Smoke` cover the assembled model, cabin
circulation, camping controls, ignition interlock, distinct storage, companion
housing, ownership, serialization, delivery failure/success and generated terminals.
Run with `-LWV17Smoke -LWUI46Smoke -LWCamper66Smoke` to isolate the test save.
Validation completed on September 20, 2026:

- `Saved/BuildRV66ExitFix.log`: editor target build succeeded.
- `Saved/RV66ImportValidated.log`: all 27 meshes imported with validated bounds,
  materials, collision and LOD settings.
- `Saved/Camper66ExitFix.log`: **73 checks, zero failures**, including actual
  aim traces to the faucet, fridge and wall switch; blocked delivery leaves credits
  and garage storage unchanged. Three complete doorway exits exercise vehicle
  ticks with forced garbage collection before and after each exit, headlights on
  and off, and restored on-foot movement/collision.
- `Saved/ScreenshotsV17/RV66_LivingRoom.png`, `RV66_MasterSuite.png`, `RV66_Camp.png`,
  `RV66_Housing.png` and `RV66_Delivery.png`: in-engine visual and UI captures.

The automated run uses a separate test save. Player graphics settings were restored
after it exited. No cook or package was run.

## RV exit crash correction

The interior rebuild previously removed all point lights except indicators and
emergency lights. Unreal spotlights inherit from point lights, so this also
destroyed the headlamps. Garbage collection cleared their references and the next
vehicle tick dereferenced a missing headlamp (the reported crash at
`LWVehicle.cpp:141`). Only cabin lights explicitly marked
`LegacyCamperCabinLight` are now removed. Headlight tick and upgrade updates also
validate each component before use. The regression checks that both registered
headlamps survive cleanup, rather than merely suppressing the crash.
