# Casino discovery and impacts

Casinos were restricted to rare city-edge parcels that could fail collision filtering. Friendly map markers also incorrectly called them WAYSTATION. Resorts now have gold CASINO RESORT markers, a FIND CASINO control on the full map, and an additional deterministic roadside reservation pass around towns. The locator checks a nine-by-nine region area and routes to the closest generated resort entrance. It reports if no resort is found rather than inventing a destination.

Reservations retain full road and building clearance, and fit inside their region to prevent neighboring reservations colliding. The existing road network and ordinary parcel IDs are preserved. Restart the game/editor session to regenerate loaded chunks; a new save is not required.

Vehicle chassis sweeps now hit pedestrians at speed, with damage, knockback, death ragdoll momentum and a small speed loss. Both forward and reverse impacts work. Parking-speed movement below approximately 5 km/h causes no damage. Passengers are excluded, walls shield pedestrians and a per-victim cooldown prevents repeated damage every frame. Player-driven and player convoy impacts retain player attribution, including settlement hostility/reputation consequences. Wheel support ignores characters and the chassis clears ragdoll debris.

Blood spray, irregular ground splashes and red wound caps accompany hits. Strong killing blows can detach heads, arms and legs by breaking the corresponding ragdoll constraint. Shooting or striking corpses can detach remaining limbs. Mannequins do not bleed. Effects are bounded to 48 simultaneous blood actors with a 25-second lifespan; corpses retain their existing 16-second lifetime. Companions, settlers and neutral civilians bleed, are knocked down, and recover using the existing downed-state system rather than being permanently dismembered.

Rebuildable material: Tools/build_v22_content.py. Verification runner: Tools/VerifyImpact22.ps1. No packaged build is produced.

Validation: Editor compilation succeeded. All 47 automation tests passed. Seed 198706 produced 62 resorts in 169 sampled regions (previously 10); seeds 1, 42, 198706 and 999999 produced 10, 6, 7 and 9 resorts respectively in the 25-region starting-area sample. Cross-region roads and parcel overlaps were checked. All 14 offscreen gameplay assertions passed, including an actual vehicle Tick crossing a pedestrian, ragdoll/dismemberment, corpse damage, low speed, reverse, repeated contact, passenger protection, civilian recovery and wall shielding.

Final visual pass: smaller, flattened wound caps and darker, rougher blood material inspected in Saved/ScreenshotsV17/Impact22_GoreCloseup.png. Final Editor build: Saved/Build22f.log (succeeded). Final content import: Saved/Impact22_FinalContent.log. Final runtime: Saved/Impact22_FinalRuntime.log, failures=0 checks=14.
