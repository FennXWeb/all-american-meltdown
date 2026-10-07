# Roadside collisions, input fixes and world cheats

Vehicles moving at least 350 cm/s (12.6 km/h) can knock down street lamps, roadside signs, neon fixtures and traffic signals. Each prop is grouped independently from roads/buildings. Its collision switches off, lights go out, and its geometry falls away from the impact. Destroyed prop IDs use the existing saved world state, so streaming a chunk back in does not restore them. Very slow bumps and impacts with buildings/terrain do not destroy scenery. The swept query is shared by player-driven, companion-driven and thrown vehicles.

Tab opens/closes inventory; I remains an inventory alias with the default layout. M opens the map. Existing custom keybindings are retained. Shift+E is restored as the default contextual vehicle flip/unstick shortcut; Shift plus the configured interaction key also works. E still leans right when no vehicle is targeted.

Open the console with tilde:

| Command | Example |
|---|---|
| `npctypes` | Lists hostile types and friendly roles |
| `spawnnpc <type> [count]` | `spawnnpc bear 2`, `spawnnpc companion` |
| `vehicles` | Lists valid vehicle model IDs |
| `spawnvehicle <type>` | `spawnvehicle apc`, `spawnvehicle helicopter` |
| `pois` | Lists POI names, type names and numeric IDs |
| `locate <type_or_id> [radius_km]` | `locate casino`, `locate airport 10`, `locate 58 5` |

Spawn outdoors with clear space in front of the player. Counts are limited to 20 NPCs per command. Spawned vehicles are unlocked/hotwired and use normal vehicle persistence. Cheat NPC spawns are session actors; recruited companions use the existing crew system.

The locator searches generated region data without loading distant chunks. It checks one region per frame, also while the console pauses gameplay. The default radius is 5 km, configurable from 1–20 km. Results identify the nearest match **within that radius**, show its coordinates/distance and set a waypoint to the entrance. A new `locate` command cancels an unfinished search. `mall` matches both the plaza and shopping mall; use `shoppingmall`, `plaza`, or numeric IDs for an exact type.

Validation: the Unreal editor target compiled successfully (`Saved/Build60c.log`). All 22 gameplay smoke checks passed (`Saved/Smoke60b.log`), including inventory/map input, Shift+E vehicle recovery, NPC/vehicle spawning, roadside impacts, destruction persistence after regeneration, and a casino search with waypoint placement. User settings were restored after testing.

No packaged build is produced.
