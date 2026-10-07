# Update 78 — interface, arrival area, wilderness and signage

## Player-facing changes

- Interaction prompts resolve the logical action to the player's current keyboard/controller binding once. The opening campaign prompt no longer turns the default interact key F into the flashlight key H. Combined prompts also respect remapping.
- Opening conversations own mouse input, release pointer capture and keep the cursor visible through the new-character handoff. Clicking dialogue continues to consume weapon input.
- Main/pause menus use generated roadside artwork, restrained motion and left-aligned navigation. Full-width backgrounds support ultrawide displays. Settings are grouped into Display, Audio/input, Quality, Effects, Performance, Controller and Interface; display confirmation, key remapping, controller settings and all graphics options remain available.
- Six survivor tabs share navigation and styling. The compact HUD retains health, stamina, food/water, money, weapon/ammo, equipment and heal shortcut, XP, compass, minimap, objectives, subtitles and alerts. New-game setup contains difficulty and character creation without geography filler.
- Bellwether's entrance has a nearby public road and four destinations: a diner, cottage, ranch house and fuel station. All four are within 70 metres of the starting building; the public road is within 30 metres. Parcel-aware routing avoids cutting through these buildings.
- Six deterministic wilderness scene layouts add tents, chairs, work supplies, pallets, barrels, picnic furniture and lootable crates. Placement rejects water, roads, buildings, story reservations and steep terrain. Loot uses persistent container records and does not refill on chunk reload.
- Roaming giant rolls per eligible chunk are now 0.15% behemoth, 0.02% colossus and 0.03% world eater, before terrain/clearance checks. At most one roaming giant may be active nearby. Authored encounters, dungeon bosses and explicit debug spawns retain their separate rules.
- POI labels, including Canadian buildings, no longer render floating 3D glyphs. Supported labels become textured framed signs using generated category artwork and each location's actual inscription. All four panel corners require wall support; panels shrink to fit or are suppressed if no valid mounting surface exists. Slot reels, road safety signs and live vehicle instruments keep their functional displays.

## Assets and implementation

`ArtSource/World78` contains the generated menu painting and 16-category enamel-sign atlas, editable Blender scene, shaped ridge tent and framed sign mesh. `Tools/build_world78.py` rebuilds meshes and `Tools/import_world78.py` imports assets. Runtime signs share cached material/texture resources by inscription, process at most one nearby label per frame and use the existing asynchronous asset warming path. Wilderness geometry/population uses staged chunk streaming.

Existing saves, character settings and user key bindings are preserved. Nearby terrain and POIs update when their chunks are reconstructed. No cooked or packaged build is produced.

## Verification

- Unreal 5.8 Development Editor compile: passed.
- Native automation: `LethalWorld.Update78.ActionPrompts`, `LethalWorld.Update78.StartingNeighborhood`, `LethalWorld.Boss48.RatesAndClearance`, `LethalWorld.Update76.Corridor` all passed. Log: `Saved/NativeWorld78Final.log`.
- Rendered UI/world regression: 72 checks passed at 1600 x 900 (`Saved/World78SmokeFinal.log`) and 72 at 2560 x 1080 (`Saved/World78Ultrawide.log`). This exercises the actual new-character handoff, pointer capture, dialogue mouse hitboxes, all settings categories, six survivor tabs, physical sign support and wilderness records.
- Screenshot review corrected stretched glyphs, a map-toolbar overlap and a tent material atlas input. The material importer now asserts the atlas connection succeeds. Final post-material 16:9 run: 72 checks passed (`Saved/World78ReleaseCheck.log`); corrected canvas texture visually verified.
- Reviewed menu/settings/skills/new-character layouts, pause overlay, exterior sign placement, nearby road access and wilderness camp geometry. Preview images: `MainMenu.jpg`, `NewSurvivor.jpg`, `Skills.jpg`.

The test processes exited and the original user settings were restored with matching file hashes.

The automated checks are not a complete manual playthrough of every POI or every possible input/device combination.
