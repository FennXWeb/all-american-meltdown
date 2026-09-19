# Build 0.10 - Road Encounters

56 authored encounters, 15 gameplay mechanisms and eight dressed scene families. Play in Unreal Engine 5.8. No package is produced.

## Discovery and pacing

Travel outdoors along roads, on foot or in a vehicle. After two minutes of active play the director considers opportunities, then spaces successful spawns roughly 90-180 seconds apart. It permits two unresolved opportunities and one dangerous opportunity at a time. Interiors, the bunker, nearby combat, heavy enemy populations and repeated travel through the same location suppress new spawns. Driving uses a larger spawn distance. Unaccepted opportunities far behind you retire so they do not block new events.

Scenes occupy clear roadside ground away from buildings and the bunker, outside unobstructed player view. Weather, daylight, level, recent history and previous outcomes influence selection. Low health suppresses new threats. Unavailable suitable ground or conditions postpone events rather than forcing a spawn.

Use E on the central object or traveler. Choices can spend supplies, trade credits, use attributes, start work, mark a route, threaten someone or decline. Escorts walk the road graph and wait when you fall behind. Threats can attack before you speak. Defenses and rescues can be overrun. Hazard lights mark dangerous ground. Work/watch tasks progress nearby while menus are closed and you are not firing.

## Consequences

Helping people improves road reputation and can unlock connected stories. Extortion lowers reputation and provokes armed resistance rather than giving free supplies. Low reputation makes dangerous encounters more likely. Credits and XP are awarded only once. Physical rewards remain in the encounter object even if the backpack is full.

Saves preserve choice, work progress, integrity, surviving enemies and health, escort position, rewards and missed opportunities. Distant scenes unload and reconstruct from saved records. Expiration uses active play time, not real time or skipped sleeping hours. New Survivor clears encounter state. History is bounded to 256 records; story unlocks and cooldowns persist, but very old distant encounter loot can retire with its record.

Open Contracts, then Road Journal to review discovered encounters and route to them. Discovered unresolved encounters also appear on the map and minimap. Unknown encounters are not revealed.

## Editing

The Unreal Data Asset `/Game/Data/DA_Encounters` exposes definitions, weights, conditions, costs, rewards, enemy composition, timing and prerequisites, plus director intervals and active-count limit. The importer preserves existing designer edits. Reset To Native Defaults explicitly replaces the definitions.

`Data/Encounters.json` authors native defaults. Run `Tools/generate_encounter_catalog.py` and compile after changing it. `Tools/build_v10_content.py` creates the editor catalog if missing and adds original RadioStatic, EncounterWarning and EncounterResolved audio slots to the existing audio manager. Scenes reuse existing low-poly assets in new arrangements.

The feature is single-player. Enemies use existing zombie, raider and dog AI; there are no new mannequin spawns. Escorts use swept collision along the road graph and do not teleport around obstacles. Combat and work share reusable mechanisms, while the 56 definitions provide authored situations, costs, rewards, conditions and story links.

## Catalog


| # | Encounter | Gameplay | Conditions / follow-up |
|---|---|---|---|
| 1 | The Last Canteen | Aid | any suitable roadside |
| 2 | No More Bandages | Aid | near matching POI |
| 3 | Cold Supper | Aid | night |
| 4 | The Bitter Well | Aid | any suitable roadside |
| 5 | A Debt on the Road | Aid | after The Last Canteen |
| 6 | Waiting Out the Rain | Aid | rain |
| 7 | Broken Axle | Repair | any suitable roadside |
| 8 | Dead Air | Repair | any suitable roadside |
| 9 | The Warm Medicine | Repair | near matching POI |
| 10 | Water Under Pressure | Repair | any suitable roadside |
| 11 | Ground Fault | Repair | rain |
| 12 | Last Box of Shells | Trade | any suitable roadside |
| 13 | Night Pharmacy | Trade | night |
| 14 | The Traveling Locksmith | Trade | any suitable roadside |
| 15 | Ration Exchange | Trade | day |
| 16 | Weight in Metal | Trade | any suitable roadside |
| 17 | Keep the Signal Alive | Defense | level 3, after Dead Air |
| 18 | Firelight Siege | Defense | night |
| 19 | The Last Stretcher | Defense | after No More Bandages |
| 20 | Rear Guard | Defense | level 4 |
| 21 | The Caged Scout | Rescue | level 2 |
| 22 | Cornered Runner | Rescue | any suitable roadside |
| 23 | Under the Machinery | Rescue | near matching POI |
| 24 | No Vacancy | Rescue | near matching POI |
| 25 | Undelivered | Cache | any suitable roadside |
| 26 | Blue Tape | Cache | level 3, near matching POI |
| 27 | What the Rain Revealed | Cache | rain |
| 28 | The Scout's Promise | Cache | after The Caged Scout |
| 29 | A Voice Too Calm | Ambush | level 2 |
| 30 | Open Invitation | Ambush | any suitable roadside |
| 31 | The Whistle | Ambush | any suitable roadside |
| 32 | Silent Procession | Ambush | night, level 3 |
| 33 | Lost Surveyor | Escort | any suitable roadside |
| 34 | Walking Wounded | Escort | any suitable roadside |
| 35 | The Last Passenger | Escort | night |
| 36 | A Second Chance | Escort | after Under the Machinery |
| 37 | The Alpha Pack | Hunt | level 3 |
| 38 | Long Shadows | Hunt | level 4 |
| 39 | End of Shift | Hunt | level 3, near matching POI |
| 40 | Numbers at Midnight | Signal | night |
| 41 | A Reply at Last | Signal | after Keep the Signal Alive |
| 42 | Fallen Weather Station | Signal | rain |
| 43 | Names on Tin | Memorial | any suitable roadside |
| 44 | One More Birthday | Memorial | night |
| 45 | A Pair of Boots | Memorial | any suitable roadside |
| 46 | Yellow Water | Hazard | level 2 |
| 47 | Live Wire | Hazard | rain |
| 48 | Pressure Leak | Hazard | any suitable roadside |
| 49 | Heat in the Drums | Hazard | level 2 |
| 50 | The Engine Picker | Salvage | any suitable roadside |
| 51 | Under the Canvas | Salvage | any suitable roadside |
| 52 | Flat-Pack Apocalypse | Salvage | near matching POI |
| 53 | Private Road | Toll | level 2 |
| 54 | Quarantine Fee | Toll | level 3 |
| 55 | Counting the Living | Survey | day |
| 56 | The Falling Lights | Survey | night |
