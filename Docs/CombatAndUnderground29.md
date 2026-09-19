# Combat and underground expansion — update 29

Editor source changes only. No build has been cooked or packaged.

## Controls and ammunition
- **R** loads a loose missile directly into an empty launcher.
- **R** replaces a depleted taser battery. One battery provides ten shots.
- **Middle mouse** performs a short gun bash: 8 damage, 8 stamina, a 0.55-second recovery, and modest knockback reduced against heavy enemies. Menus, conversations, vehicles, and safehouse protection block it.
- **E** at an intact gas pump refills carried flamethrower canisters, including the installed canister. Exploded pumps cannot supply fuel.
- Old missile canisters and taser ammunition are converted on load. Live rounds remain in the weapon; spare ammunition is repacked. Overflow is preserved in a recovery bag.
- New loot rolls substitute loose missiles/batteries for obsolete launcher/taser magazines, including rolls from older loot-table assets.

## Enemies and body loot
- Dead enemies can be searched with **E**. A persistent loot bag remains when the temporary ragdoll expires or its chunk unloads.
- Legendary rarity is seeded per enemy: 4% one star, 1.5% two stars, 0.5% three stars. Each star increases health, damage, damage resistance, severing resistance, and rewards.
- Every starred enemy drops a tier-four weapon with a legendary modifier, plus supplies scaling with stars. Searching again does not regenerate loot.
- The most recently damaged enemy, or an enemy actively attacking you, appears at the top of the HUD with its name, legendary stars, and health.
- Behemoths use a body three times the size of a titan. Colossi are three times the behemoth's size. The World Eater is an enormous segmented worm.
- Colossi and World Eaters always have at least one star. Their wilderness spawn rolls are rare, seeded, avoid POIs and the starting area, and respect recorded kills.
- The World Eater sinks underground, becomes hidden and invulnerable while buried, selects a new clear position, warns with disturbed ground and sound, and emerges before attacking. Heavy melee attacks have a visible/audible windup.

## Locations
- **Deepwell Survival Bunker:** two levels of shelter quarters, life support, guarded storage, and a secured reserve.
- **Fracture Caverns:** two levels of rock-lined excavations and abandoned pump works, with mutated guards and an expedition cache.
- **Interstate Service Tunnels:** three levels of connected utility corridors, rail sidings, substations, and an emergency transport reserve.
- Underground entrances use **E** to descend; an exit inside returns to the surface. Nearby active companions travel with the player. These are portal transitions, not walkable holes through the terrain.
- Each underground layout is seeded and connected, with security objectives, enemies, loot, stairs, and a final reward. Dungeon activation accounts for negative world height.
- Towers now include financial, residential, and medical variants. Residential levels contain separate studios with usable beds and kitchen fixtures. Added storage, furniture, room divisions, signs, lighting, and ceiling details. Inaccessible floors have solid slabs; accessible floors retain stair openings and landings.
- Tower and casino glazing is individually breakable and saved. Facade bands, hotel partitions, and door headers have been fitted to close structural gaps.

## Verification
Verified: clean Editor rebuild succeeded; all 61 Unreal automation tests passed; the final rendered run passed 100 gameplay checks with zero failures. Bosses, underground interiors, tower rooms, and the casino were also reviewed in screenshots. Final logs: `Saved/Build29k.log`, `Saved/Combat29_Automation.log`, and `Saved/Combat29_FinalRuntime.log`.

`Tools/VerifyCombat29.ps1` runs the full Unreal automation suite followed by an offscreen gameplay scenario using an isolated automation save. Captures are written to `Saved/ScreenshotsV17` and reports to `Saved/Combat29Report`.

Generation changes apply when chunks are generated again. Existing discovered-place records may still describe the prior layout; a new seeded world provides the cleanest comparison.
