# Electric vehicles

Two additional vehicle IDs keep existing motorhomes and supercars intact:

| Vehicle | ID | Seats | Battery | Cargo | Garage bays |
| --- | --- | --- | --- | --- | --- |
| Solstice Electric Residence | `solstice_rv` | 6 | 420 kWh | 12 × 38 | 3 |
| Apex Electric Hypercar | `apex_ev` | 2 | 125 kWh | 8 × 7 | 1 |

Both participate in the normal weighted traffic selection, debug vehicle list,
ownership, delivery, keys, damage, cargo, and companion seat systems. Spawn directly
with `spawnvehicle solstice_rv` or `spawnvehicle apex_ev`.

## Driving and cabin

Set a destination on the map, then interact with the dashboard display to engage
autopilot. No companion driver is required. The shared road navigation controller
handles lanes, corners, traffic controls, obstructions, braking and recovery. It
stops if a route cannot be driven. Use the display again to request a controlled
stop. Moving vehicles cannot hand control abruptly to the driver seat.

In the Solstice, press the jump/stand action (Space by default) to walk around
during autonomous travel. WASD walks relative to the cabin; look controls remain
relative to the vehicle. Interact with an unoccupied passenger seat to sit.
Cabin lights, televisions, window shades, faucet and interior storage work during
travel. Sleeping, exterior cargo, doors, awning and slide-outs require parking.
Autopilot retracts camping equipment before departure.

Walking integrates in the cabin's coordinate frame, with bounded substeps,
acceleration/deceleration, capsule sweeps and wall sliding. The player is updated
after the vehicle's final transform each frame. Normal character gravity and
collision are disabled only while aboard, then restored on exit. Vehicle pitch
is filtered for the walking camera; world translation is not smoothed independently,
which would make the camera trail behind the moving interior.

The Solstice has a wider and longer living space, photovoltaic roof, ribbon lamps,
sensor fairings, sculpted front bodywork, new cockpit, perforated upholstery,
titanium trim, ambient strips, coffee station and compact vanity. It preserves the
master bed, six companion bunks, dinette, lounge, kitchen, TVs, awning and slides.
The Apex has an original sculpted monocoque, enclosed cockpit with separate glazing,
turbo-fan wheels, diffuser, rear wing, bucket seats and a steering yoke.

## Energy and display

Petrol is never consumed by the electric variants. Batteries are saved independently
of old fuel fields. Solar production follows the day cycle and cloud cover; a
covered roof does not charge. The Solstice roof provides up to 8.4 kW; the Apex's
smaller integrated solar surface provides up to 1.2 kW. Traction consumes energy,
braking recovers a limited amount, and garage servicing can recharge the battery.
Solar catch-up is bounded to 72 in-game hours; the game's compressed day makes
charging deliberately faster than real-time travel consumption.

The large dashboard redraws at 10 Hz while occupied. It displays actual speed,
gear, charge, solar output, power draw/regeneration, weather, world time, nearby
roads, destination distance, planned driving route and autopilot state. Nearby
road geometry is cached until the vehicle moves outside its map window.

## Art and sound source

`Tools/build_electric74.py` authors twelve meshes; `Electric74.blend`, FBXs and a
bounds/material manifest live in `ArtSource/Electric74`. Shared RV architecture
and furnishings are reused where their existing openings and interactions fit.
`Tools/import_electric74.py` validates imports and makes materials and LODs.
The revised meshes use split, weighted normals, fine upholstery detail, tinted
glass and continuous roof/floor panels. Screens and nearby-road geometry are
updated at bounded rates; the vehicle art is included in the startup asset bundle.

In-engine previews: [residence](../Saved/ScreenshotsV17/EV74_Residence.png),
[RV dashboard](../Saved/ScreenshotsV17/EV74_Dashboard.png),
[RV exterior](../Saved/ScreenshotsV17/EV74_CoachExterior.png),
[hypercar](../Saved/ScreenshotsV17/EV74_HyperExterior.png),
[hypercar cockpit](../Saved/ScreenshotsV17/EV74_HyperCockpit.png).

The built-in image-generation tool created
`ArtSource/Electric74/T_ElectricAtlas74.png` for this project. Final prompt:
"Use case: stylized-concept. Asset type: game-ready seamless material atlas for two
high-end semi-futuristic electric vehicles in a 2030 post-apocalyptic PS3-styled
game. Create a square texture image divided precisely into four equal quadrants
with no borders, each flat orthographic material viewed directly from above,
uniform diffuse lighting with no perspective or baked shadows. Top left:
extremely dark navy photovoltaic solar cells in a precision silver microgrid with
delicate blue iridescent cell texture. Top right: sumptuous warm ivory perforated
automotive leather with tiny regular perforations and very fine realistic leather
grain. Bottom left: cool satin brushed titanium with subtle long horizontal grain,
no scratches. Bottom right: charcoal black woven carbon fiber with rich tightly
woven diagonal structure. PBR albedo surface reference, exceptionally detailed,
understated expensive materials. No lettering, no logos, no objects, no background,
no frame."

Three original synthesized powertrain sounds are registered in the audio catalog:
`EV74_CoachMotor`, `EV74_HyperMotor`, and `EV74_Ready`. Motor loops use periodic
harmonics so they join without a seam, with continuous speed/load modulation and
cabin filtering. Existing tire, braking, wind, wiper and cabin effects are reused.
All three new slots support Source and Tracks replacements. No ElevenLabs credits
were used for this update.

## Validation

Automation: `LethalWorld.Update74.Electric` covers energy integration, vehicle
families, garage sizes and battery/identity/cabin serialization.
Unpackaged test mode: `-LWV17Smoke -LWUI46Smoke -LWElectric74Smoke` uses the isolated
automation save and exercises the assembled vehicles, moving interior, collision,
autopilot, instruments, energy and exits.

Validated September 23, 2026 (local time):

- Editor target compiled successfully: `Saved/BuildElectric74Final.log`.
- Twelve FBX imports passed bounds/material validation:
  `Saved/Electric74ImportPolish.log`.
- Electric/vehicle-profile automation: 2/2 passed in
  `Saved/Electric74Automation.log`.
- Electric gameplay: 52 checks, zero failures in `Saved/Electric74Verified.log`.
  Includes 30/60/144 FPS cabin movement, turning/translation drift below 0.5 cm,
  full vehicle-tick autonomous travel while walking, collision, normal interaction
  routing, safe seat handover, camping-equipment interlocks and forced-exit cleanup.
- Original RV regression: 73 checks, zero failures in
  `Saved/Electric74RVRegression.log`, including storage, companion bunks, delivery,
  save/load and repeated exits after garbage collection.
- Inspected all five in-engine previews above. The artificial test platform is
  not a new map or a replacement for road testing across every city.

No packaging was run. User graphics settings were restored from the pre-test
backup after all test editor processes exited.
