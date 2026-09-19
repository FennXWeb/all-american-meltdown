# Vehicle overhaul 42

## Geometry and interiors

All 11 types use a new vehicle shell and separate fitted cabin mesh: sedan, police cruiser, box truck, motorhome, school bus, cargo van, pickup, dirt bike, SUV, muscle car and supercar. Bodywork is authored from panel surfaces with shaped shoulders, actual wheel arch openings, front/rear fascia, bonnet surfaces, pillars, roof headers and fitted windshield endpoints. New tyres have shoulders, tread, rims, brake discs, hubs and spokes; the dirt bike has its own wire-spoke knobby wheels.

Cabins include moulded dashboards, vents, radio/climate controls, console, cupholders, shift lever, pedals, door cards, upholstery, seat seams, belts, headrests, headlining and visors. RV furnishings replace the existing meshes while retaining their individual storage, bed and doorway interactions. Existing seating, fuel, convoy, companion driving, damage and persistence systems remain in place.

Sources: Tools/make_vehicle42_models.py, Tools/make_vehicle42_cabin.py, Tools/make_vehicle42_textures.py. Blender sources and FBX manifests are in ArtSource/ModelsV42. Textures are in ArtSource/TexturesV42. Body shells are approximately 1,200–10,800 triangles, plus separate cabins and wheels; LODs reduce geometry to 50% and 20% at distance. This is deliberately a stylized older-console art target.

## Instruments and interactions

Cockpits have actual moving speedometer, tachometer, fuel and coolant needles (compact speed/tach instruments on the bike), a gear/reverse/neutral display, turn indicators, headlight and warning lamps. Fuel reads the persistent tank; temperature rises gradually with the engine running and is affected by engine damage. Headlamp and tail-light materials change brightness with the lights and brakes. Look at the steering wheel and press E to sound the horn.

## Audio structure

Three independent steady-RPM engine layers per vehicle (idle, low and high) use equal-power mixing, smoothed load/RPM, six gear ratios, shift hysteresis and brief shift attenuation. Engine sources attach near the engine; tire, brake/skid, wind and wiper layers respond independently. Occupants hear a filtered cabin mix. Layers remain playing during shifts, rather than restarting for every throttle change. Parked silent vehicles release their audio components.

Tools/vehicle_audio42.py prepares 61 ElevenLabs sounds, estimated 7,320 credits: 33 unique engine loops, 18 interaction sounds, six mechanical loops and four engine starters. It reads the local key without printing or storing it, writes a resumable usage ledger and does not automatically retry ambiguous requests. It converts MP3 to mono PCM WAV, normalizes level and crossfades loop seams. Tools/import_vehicle42_audio.py requires a complete batch before replacing vehicle catalog slots. All entries retain the catalog Tracks system for later replacement/variation.

The user approved generation on September 12, 2026 with a 7,800-credit cap. All 61 sounds were generated and imported into `/Game/Audio/Vehicles42`; the catalog now uses them. ElevenLabs response headers reported 1,830 credits total, below the conservative 7,320-credit estimate. Per-request charges are recorded in `ArtSource/AudioV42/usage.json`. Existing engine audio remains a fallback if a custom engine slot is removed. No API key, decoder or network generation code is used by the packaged game.

## Validation

Editor compilation succeeded (`Saved/BuildVehicle42Verified.log`). All 56 replacement meshes were imported, checked for bounds/material assignments, and given distance LODs; RV furniture, seats and steering meshes retain interaction collision. Final fitting import: `Saved/Vehicle42FitImport.log`.

Rendered checks passed for all 11 types: 99 checks, zero failures (`Saved/Vehicle42VerifiedSmoke.log`), including driver entry, instruments, persistent-dent mesh support and the replacement RV fridge interaction trace. Cockpit/exterior captures are in `Saved/ScreenshotsV17/Vehicle42_*`. Existing vehicle placement, fuel, persistence and convoy regression passed 64 checks (`Saved/Vehicle42LegacySmoke.log`). The full existing automation suite passed all 84 tests (`Saved/Tests42Final.log`). These are controlled regression scenes, not a long open-world playtest.

All 61 PCM WAV files passed duration, level, clipping and loop-boundary checks (`Saved/Vehicle42AudioValidation.json`). The Unreal import validated sound durations, wave/catalog loop flags and Tracks entries (`Saved/Vehicle42AudioImport.log`). These signal checks do not replace a subjective listening pass. No packaging was performed.

The generated-library runtime pass completed 165 checks with zero failures (`Saved/Vehicle42GeneratedAudioSmoke.log`), verifying each of the 33 engine layers resolves from the new asset directory without fallback and has an actively playing component. Editor compilation with these checks succeeded (`Saved/BuildVehicle42Audio.log`).
