# Character, combat and streaming fixes

New games now start with a crowbar in the melee slot and a .357 revolver in the sidearm slot. Its six chambers are loaded. There are no spare supplies, starter credits or stocked personal stash. Existing version 2 saves retain their inventory.

## Characters

The male and female anatomical heads use continuous face materials with a smooth vertex mask at the temples, ears and neck. Reduced baked photographic contrast lets the geometry and world lighting shape the face. The duplicate neck tube was removed and the anatomical neck edge extended evenly into the collar.

All 19 non-bald hairstyles now have separate tapered strand geometry, uneven tips, exposed temples and swept flow for the parted/slicked styles. Hair retains the existing colour choices and gains texture visibility in dark colours. The revised heads are approximately 7,900 triangles each; most hairstyles are approximately 3,800 triangles.

Source: `Tools/make_characters38.py`, `ArtSource/Characters35/AllAmericanMeltdown_HeadsHair38.blend`. Import the 21 revised meshes with `Tools/import_characters38.py`. The complete character-library scripts also contain the geometry and material changes. Existing wardrobe, style IDs and asset paths remain compatible.

## Repeated world-generation work

- A bounded neighborhood cache stores the final road/POI overlap filtering. Terrain queries reuse its arrays directly instead of gathering and filtering nine regions for every sample.
- A bounded route cache reuses road topology while keeping each query's projections separate. Seed, bounds and generation settings are part of the key.
- Active waypoints retain and trim a usable route. They reroute when the player leaves the corridor or approaches an intermediate destination, rather than rebuilding every 750 cm.
- Ground-route samples have a fixed budget and refresh after meaningful movement.
- The region cache evicts incrementally instead of discarding its entire contents.
- Story compounds survive objective changes. Mercy's burned layout and the fortress's cell/surrender changes still invalidate their geometry.

These changes address repeated CPU work. They do not establish a guarantee against every hitch from first-use asset loading, shader compilation or construction of a large new chunk.

## Gunfire and story allies

Story characters now advance their firing cooldowns and participate in the infected tutorial, Mercy defense and fortress assault. Their existing line-of-sight checks still prevent firing through a friendly crossing the shot.

Enemy gunshots originate at the weapon mesh's muzzle. Loud spatial sounds retain their distance and occlusion effects, with a longer low-pass range and a less extreme occluded volume reduction. Catalog tracks and custom attenuation overrides remain supported.

Enemy firing spread increases with shooter and target movement. Hits still require a real collision trace. Gunfire has a brief illuminated muzzle flash and a thin, fading moving trail ending at the actual obstruction; shotgun shots show one representative trail to avoid visual clutter. Player gunfire keeps its existing listener-relative sound and does not play a second sound for the new effects.

Hostile settlement NPCs use the same muzzle presentation and imperfect aim. Their misses still produce sound and a visible shot. A settler crossing their line of fire cancels the shot.

## Verification

- Editor target compiled successfully (`Saved/Build38Final.log`); no packaged build.
- All 79 Unreal automation tests passed again against the final build (`Saved/Tests38Final.log`).
- Route benchmark: 5.951 ms for the initial graph and 0.333 ms with reused topology. Query changes and cache eviction preserved the original path.
- 480 cached terrain samples plus comparison with the original height calculation completed in 6.239 ms, with identical heights.
- At 18 m, a 20,000-shot simulation hit a torso-sized target 36.6% of the time while stationary and 18.8% with shooter/target movement.
- Gameplay and creator screenshots are produced by `-LWV17Smoke -LWRevision38Smoke -LWAudioSmoke` in the isolated automation save slot. Audio checks run with a real device transiently muted and therefore verify component creation/placement, not a listening assessment.
- Final targeted gameplay check: 10 checks passed (`Saved/Revision38Final.log`), including repeated story-ally damage, unchanged compound identity across objectives, muzzle audio location and both shot effects.
- Full Chapter 1 progression: 112 checks passed (`Saved/Story38Regression.log`), including captivity, gear recovery, final boss and settlement ownership.
