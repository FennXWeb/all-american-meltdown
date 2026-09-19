# Exploration, weather and combat — update 26

No game was cooked, staged or packaged. Changes target the Unreal Editor project.

## Playing

- Locations stay hidden on both maps until approached. First discovery shows the location name and awards 35 XP. Discovered entrances and names persist in the save. The bunker remains known from the start. Previously visited legacy sites gain map records when revisited without duplicate XP.
- Open the map, click a discovered location, then select **FAST TRAVEL**. The bunker is also eligible. Travel is blocked while in a vehicle, shortly after taking combat damage, near enemies, or when no safe arrival point is available. Travel advances the clock and consumes food and water. Arbitrary waypoints cannot be used as teleport destinations.
- Six added weather states: dust storm, tornado, severe fog, nuclear winter, ash snow and blizzard. Seeded weather periods control their frequency. Fog and storms shorten enemy visual detection range. Snow and dust have moving particles; winter exposure costs stamina and water outdoors. Tornadoes move through the landscape, pull and hurt exposed players and damage nearby vehicles. Buildings and enclosed vehicles provide shelter.

## Combat

All nine enemy archetypes now use staggered perception, facing checks, physical sight traces, last-seen contact memory, bounded search and navigation, and nearby warnings. Raiders use finite magazines, bursts, reload windows, flanking and evaluated cover positions. Animals use committed lunges or charges; scorpions circle and close; titans have slower heavy windups. Non-aggressive mannequins still freeze when observed, then pursue and attack when provoked. Hostile settlement residents use the raider behavior; friendly social and companion behavior remains in place.

Melee attacks have a windup and recheck distance, facing and obstruction when they land. Moving out of reach can evade them. Injury penalties no longer reduce an enemy to near-zero mobility, and injured enemies retain tactical behavior. Limb damage thresholds scale with body mass: 95 for limbs, 145 for heads before scaling. Generic living damage cannot randomly sever a limb; focused hits and explosions can. Weapon-arm loss still disables firearms. These are concrete AI improvements, not a claim of production AAA-quality AI or animation.

## Geometry and models

Thin horizontal construction slabs are clipped against earlier coplanar footprints before rendering. Overlapping floor/road/roof faces no longer draw over each other; undersides and collision remain present. Degenerate triangulation faces are discarded. This addresses a shared z-fighting cause, not a guarantee that every procedurally assembled prop is free of intersection.

53 rebuilt segmented meshes cover zombies, raiders, residents, mannequins, dogs, moose, titans, deathclaws, scorpions, scooter zombies and traders. Added anatomy, faces, fingers, clothing seams, pockets, armor and creature detailing use the existing material palette. Display mannequins use the same parts as their enemy counterparts. Runtime assets have a `26` suffix to avoid retained material slots on old imports; prior assets remain available.

Source: `Tools/make_npcs26.py`, `ArtSource/ModelsV26/NPCs26.blend`, and both model manifests. Import with `Tools/import_npcs26.py` in Unreal. The versioned import manifest retains original FBX filenames and recorded bounds. Import validates every material slot and bounds before saving the batch.

## Verification

Use `Tools/VerifyWorld26.ps1` for all 54 automation tests plus rendered update-specific checks. Test saves use the existing isolated smoke-test slot. Logs: `Saved/World26_Automation.log`, `Saved/World26_Runtime.log`; captures: `Saved/ScreenshotsV17/World26_*.png`.

Final validation: Editor compilation succeeded (`Saved/Build26f.log`); all 54 automation tests and 115 rendered runtime checks passed with zero failures. All 53 imported mesh bounds/material-slot checks passed (`Saved/NPC26ImportB.log`). Reviewed discovery, enemy, weather and police-floor captures. Runtime startup still logs two informational Chaos messages about four bad triangles; they did not fail the checks and should not be treated as proof that every generated collision mesh is clean. No packaged build was produced.
