# Build 0.9

Open `LethalWorld.uproject` in Unreal Engine 5.8 and play in the editor. This update does not create a packaged executable.

## Vehicles and weather

All eleven vehicle shells are rebuilt, with wheel openings, body trim, mounted mirrors, lights, bumpers, and interiors. Seats and wheels use each vehicle profile; the SUV has three rows. The shared cockpit includes gauges, pedals, dashboard, console and supported rear-view mirror. Vehicle classes retain their existing handling, cargo, seat counts and special controls.

Flashlight intensity is increased to 180,000 and headlights to 900,000, with longer illumination range. Headlight emitters sit ahead of the bodywork. Press F on foot for the flashlight, H while driving for headlights, and C for wipers.

Rain accumulates on the windshield and obstructs the view. Wipers clear the accumulation; switching them off lets rain build up again. Outdoor pavement, terrain and vehicle paint darken and become glossy when wet. Road puddles appear with accumulated moisture and fade gradually after rain. Surface moisture is saved with the world. This is a lightweight material system, not a fluid simulation; interior walls do not receive a universal wet coating.

## Buildings and interaction

VANTA ATELIER is a new luxury clothing store with original racks, display plinths, velvet seating, stone and wall textures, and a custom sign. Mannequin enemies spawn only in this POI and share their appearance with static display mannequins. Aggro behavior remains unchanged.

Diners and fast-food restaurants use new pedestal tables, upholstered booths, a six-burner range, extractor hood, sink, refrigerator and service counter. Detached kitchen/bathroom tile panels were removed.

Use E on a bed to sleep four hours. Sleeping restores health and stamina, consumes food and water, and requires enemies to be clear of the area outside the bunker. Use E on a chair to sit; E or Space stands up. Seated saves retain the standing position to prevent loading inside furniture.

## New worlds

First-time deployment and New Survivor open a setup screen. Click the seed field and type up to nine digits, or select Random Seed. Select settlement density, building density, terrain roughness and difficulty, then Create World. Creating a world replaces the active survivor save.

The seed and settings persist. Roads can branch into curved residential side lanes and small hamlets. Existing chunk streaming, road clearance, roadside parcels and connected highways remain in use. Easy/Normal/Hard affect incoming damage, survival consumption and ambient enemy counts. Settings change generation cache identity, avoiding stale regions when creating a differently configured world.

Existing saves load with standard settings. Because generation and POI layouts changed, a new world gives the most consistent layout; old saved car/container coordinates are preserved.

## Development

- `Tools/make_models_v9.py`: Blender source for 19 models.
- `Tools/make_textures_v9.py`: original procedural textures and sign.
- `Tools/build_v9_content.py`: Unreal asset/material import, no packaging.
- `Tools/Build.ps1 -Content`: editor compilation and content imports.
- `Tools/Verify.ps1`: automation and gameplay checks, including V9.

Vehicle movement continues to use the existing swept chassis/steering simulation. The models retain the game's deliberately low-poly, degraded visual style.
