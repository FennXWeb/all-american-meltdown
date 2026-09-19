# Large-enemy spawning fix

Behemoths, colossi, and world eaters previously shared a single chunk-center spawn attempt. Their combined chance was 1.15% before rejection, with colossi at 0.1% and world eaters at 0.05%. The point was rejected within 70 meters plus the bounding radius of any POI. Dense procedural development and the ordinary enemy cap could effectively suppress all larger enemies.

The revised outdoor spawning path:

- Rolls 8% behemoth, 1.5% colossus, and 0.75% world eater per streamed chunk, before placement checks.
- Tries up to 24 deterministic positions instead of only the chunk center.
- Checks rotated building footprints and roads with size-appropriate clearance; also checks the world eater's long body.
- Keeps safehouse, story locations, and friendly settlements clear and prevents spawning close to the player.
- Checks ground height, slope, and capsule clearance, then places the fully scaled enemy above the ground.
- Limits living roaming giants to three, independently of the ordinary enemy population cap.
- Preserves persistent IDs and killed-enemy records, prevents duplicates, and retains guaranteed legendary status for colossi and world eaters.

Existing saves are supported. The new rules apply when chunks are generated or streamed back in; a new game is not required. These remain outdoor encounters, not indoor POI spawns. Rates above are eligibility rolls, not guaranteed encounter rates: space and population checks still apply.

Validation: editor target compiled successfully (Saved/Build48.log). Spawn-rate/clearance automation passed, and all 18 runtime checks passed (Saved/Boss48_Runtime.log), including all three large enemy types spawning with an ordinary population of 100. Natural chunk generation also produced a behemoth and world eater in the test world. No build was packaged.
