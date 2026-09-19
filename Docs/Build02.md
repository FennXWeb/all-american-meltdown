# Lethal World 0.2 — Shelter Protocol

The Windows package passed 24 automation tests and 463 integrated gameplay checks. [Validation report and screenshots](Validation02.md).

This update connects survival to physical equipment: weapons, loaded magazines, loose ammunition, armor, supplies, world containers, trader stock, and bunker storage use persistent item identities.

## Starting out

Launch `Builds/Windows/LethalWorld.exe`. Your starting kit contains a crowbar, pump shotgun, revolver, armor, a backpack, and supplies. Press **B** to plot a route to Shelter 01. Use **E** on the bunker airlock. Inside, use the secured storage chest to access the bat, sniper, SMG, combat rifle, LMG, magazines, and practice ammunition supplied with this build.

The bunker prevents damage and stops hunger/thirst depletion. Stored items remain secure when you die. All carried items—including equipped armor, guns, their loaded magazines, and loose rounds—go into a recoverable gear bag at your death location. Respawn returns you to the bunker without replacing lost equipment. Death is saved immediately; loading the survivor record does not restore the old carried kit. Credits remain in the survivor's account.

## Controls

| Action | Input |
| --- | --- |
| Walk / look | WASD / mouse |
| Sprint / crouch / jump | Shift / Ctrl / Space |
| Shoot or melee / aim | Left / right mouse |
| Crowbar / bat / shotgun / revolver | 1 / 2 / 3 / 4 |
| Sniper / SMG / combat rifle / LMG | 5 / 6 / 7 / 8 |
| Cycle equipped weapons | Mouse wheel |
| Reload / change automatic fire mode | R / V |
| Interact / flashlight | E / F |
| Inventory / map | I / Tab |
| Route to bunker | B |
| Pause / close panel | Escape |
| Save / load survivor record | F5 / F9 |

## Inventory and ammunition

Drag items between grids or onto compatible equipment slots. **R** rotates the held item's footprint. Empty cells in irregular footprints remain usable. Right-click a carried item to equip or consume it; select a carried weapon or magazine and press **U** to unload; **Delete** drops selected carried gear outside the bunker. Container grids scroll when their contents exceed the visible rows.

Primary, secondary, sidearm, melee, armor, helmet, rig, backpack, and tool slots enforce item compatibility. Equipment can be moved back into the backpack. A gun must be carried and equipped before its hotkey selects it. Open inventory and map panels do not protect you from enemies; use the bunker for safe organization.

Magazine-fed guns retain separate magazine instances and a separately tracked chamber. Reload exchanges actual compatible magazines. Partially used magazines retain their remaining rounds. Drag compatible loose ammunition onto a magazine in the inventory to fill it; incompatible ammunition is rejected and capacity is enforced. A loaded magazine travels with its weapon when the weapon moves into storage, a trader's stock, or a dropped bag. Unload it before managing it separately.

The shotgun loads individual shells. The revolver tracks all six chambers, including spent cases; reloading ejects spent cases, retains unfired rounds, and inserts new rounds individually. The sniper cycles its bolt; the automatic weapons have different rates, handling, sounds, and staged reload motions. The LMG uses an individually tracked detachable belt box filled with loose 7.62 rounds.

## Loot, trading, and navigation

Fuel stops, motels, clinics, depots, diners, roads, military tables, and traders have separate weighted loot presets. Containers retain their state across chunk unloads and saves. Emptying a container and returning does not generate a fresh roll.

Wandering exchange robots patrol settlement streets. One is available in the starting settlement for testing; other regions have an 18% spawn chance. Approach and press **E** to open trade. Drag stock into your inventory to buy, or carried items into the stock grid to sell. The transaction checks credits and available space before changing ownership. Stock persists; this version does not periodically restock merchants.

The minimap shows nearby roads and landmarks. The full map supports waypoints and displays a road-following route. Amber ground markings guide you in the world and update as you move. Routes connect crossings, T junctions, service streets, and driveways. Very long trips use successive bounded regional routes toward the original destination.

## Settings

Field Settings includes sensitivity from **0.025 to 5.000**, master volume, camera decay, resolution presets, and windowed/fullscreen/borderless modes. Borderless follows the desktop resolution. Applied video changes revert after 15 seconds unless confirmed. Input and presentation settings are saved independently of the survivor checkpoint.

## Authoring tools

In Unreal Editor, open **Tools → Lethal World Audio Manager**. Expand a slot and drag a SoundWave, SoundCue, or MetaSound Source onto its Source field. The catalog includes weapon actions, impacts, footsteps, interface sounds, ambience, traders, the bunker, and menu/exploration/combat music. Volume, pitch, looping, and attenuation are editable per slot. Save the catalog, then restart play to audition the mix. **Tools → Validate Lethal World Audio** checks the assignments. SoundCue/MetaSound loops must also be configured in their source graphs.

The catalog is `/Game/Audio/DA_AudioCatalog`. The original 16 slots and 30 new slots have original placeholder audio. Music switches between menu, exploration, and nearby combat. Walls apply occlusion and low-pass filtering; shelter changes ambience and reverb. Replacing the source does not require editing C++.

Edit `/Game/Data/DA_LootTable` to change per-context roll counts, empty-container chance, tier weights, rarity multipliers, stack ranges, guarantees, unique-item rules, total caps, and hazard scaling. Edit `/Game/Data/DA_ItemCatalog` to change prices, ammunition compatibility, equipment compatibility, stack limits, and rectangular or irregular footprints. Keep stable item IDs for existing saves.

## Rebuilding

- `Tools/Build.ps1` compiles the editor target.
- `Tools/Build.ps1 -Content` imports the 0.2 mesh, audio, item, and loot assets.
- `Tools/Build.ps1 -Package` builds and packages Windows.
- `Tools/Build.ps1 -RebuildOriginalContent` deliberately regenerates the original content first; this can replace original source audio imports, so keep artist replacements assigned to their own assets.
- `Tools/Verify.ps1` runs the generation, inventory, loot, navigation, and gameplay checks.

Source generators are included under `Tools`. The 23 new meshes have documented pivots and assembly transforms in `ArtSource/ModelsV2/README.md`. Reloads and robot motion use animated static components; these are original procedural animations, not skeletal animation clips. Zombie corpses use seven simulated bodies connected by six physics joints.

This remains a single-player procedural prototype. The backpack is a shared grid rather than nested containers inside every worn bag. There is no multiplayer, weapon modification bench, driving, or authored campaign in this version.
