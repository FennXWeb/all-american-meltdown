# World population, major destinations and retail interiors — update 53

- Behemoth eligibility: 2% of chunk rolls (was 8%). Colossus: 0.3% (was 1.5%). World eater remains 0.75%. Existing clearance, player distance, persistence and active-boss limits still apply.
- Regional major destinations reserve their footprints before ordinary plots and roads. Four-by-four region districts distribute malls, prisons and a theme park/deep dungeon; a separate roll enables the rarer casino slot. Unique landmarks and the airport take precedence. Story and Canadian-border exclusions remain enforced.
- Ordinary POIs retain empty/quiet outcomes, small scavenger counts, groups, and raider-heavy populations. Friendly eligible stops now occur on 36% of their seeded rolls (was 28%), with merchants, recruitable companions and contract givers. Actual enemy counts still obey collision clearance and the global population budget.
- Encounter first-attempt default is 35 seconds. The catalog has a FrequencyMultiplier of 1.8, applying to existing custom interval settings; native 90–180 second intervals become 50–100 seconds. Failed attempts retry after 8 seconds. Candidate road segments are restricted to the player's vicinity; placement still checks terrain, obstacles, POI footprints and visibility. Story scenes and protected areas are not populated by this director.
- Ten original models in ArtSource/ModelsV53: circular garment rack, straight garment rail, folded-clothes display table, cash register, sales counter, industrial pallet rack, loaded pallet, pallet jack, packing bench and fitting mirror. Clothing has cut garment silhouettes rather than block garments. Source: Tools/make_retail53.py. Import: Tools/import_retail53.py.
- Clothing-store layout separates displays, checkout, wall rails and fitting rooms; mannequin display positions are varied. Warehouse layout separates bulk storage, receiving, packing and existing staff facilities. Storage-bearing new fixtures are lootable.

Connected multi-segment access roads are preserved during driveway cleanup. Encounter searches extend toward town outskirts; camps, graves and supply caches can also use clear open ground. Visibility, terrain and collision checks remain enforced. Intervals schedule attempts, not guaranteed spawns.

Validation completed:
- Editor compilation passed: Saved/Build53Placement.log.
- All ten imported meshes passed bounds and material validation: Saved/Import53.log.
- Five automation tests passed, covering boss rates/clearance, encounter catalog/eligibility/determinism, expanded POI reservations and major destination placement: Saved/Tests53Final.log.
- Live smoke test passed all 12 checks, including imported fixtures, lootable storage, three warehouse survivor roles and encounter placement on generated terrain: Saved/World53Verified.log.
- Clothing-store and warehouse screenshots were reviewed in Saved/ScreenshotsV17/World53_Interior18.png and World53_Interior13.png.

The initial driveway connectivity and encounter placement failures were corrected and rerun successfully. These are automated and smoke-test results, not an extended manual playthrough. Builds and tests were editor-only. No packaging.
