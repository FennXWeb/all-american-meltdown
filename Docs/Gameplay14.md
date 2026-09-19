# Build 0.14 — encounters, equipment and vehicle life

## Equipment

Weapons retain a saved Common/Uncommon/Rare/Epic/Legendary tier. Each step adds 12% base damage and reduces recoil and spread by 6%; character skills still apply. Old items default to Common. Loot rolls determine tier independently of weapon model; higher hazard improves the tier roll. Trade prices include tier and installed attachments.

Drag an attachment onto a compatible gun in inventory or storage. Occupied mounts reject replacement until the old attachment is removed. Select a carried gun and press F1 (optic), F2 (light), F3 (laser), F4 (grip) to remove that mount. Removal requires room in the backpack and is atomic. Installed types are saved within their owning gun, so transfers, sorting, death drops, and container saves carry them together.

- Light and laser have independent side mounts. The mounted light follows the flashlight switch.
- Reflex, holographic, 4x and 8x optics occupy the optic mount; optics change aiming FOV and reticle.
- Vertical grip reduces recoil; angled grip reduces recoil and spread.
- Revolver: optics except 8x. Shotgun: optics except 8x, light, laser. SMG: all except 8x. Sniper, combat rifle, LMG: all eight. Melee has no mounts.
- Attachment loot is added to depot, military and trader tables. Existing user-customized rows are preserved by the importer.

## Enemy population

`/Game/Data/DA_EnemySpawns` is an editable data asset. Each POI type has its own minimum/maximum population, zombie/raider/dog weights and indoor probability. Wilderness groups appear in 16% of chunks by default, away from developed parcels. The maximum ordinary live population is 100. Setting a row maximum to zero disables it at every difficulty. Placement checks floor height, slopes, capsule clearance and player distance, with bounded attempts. Killed IDs persist. Friendly sites and taverns exclude ordinary enemies. Boutique mannequins keep their dedicated display-placement system.

The `POIType` keys in the spawn asset are:

| Key | POI | Key | POI |
| --- | --- | --- | --- |
| 0 | Fuel stop | 10 | Megamarket |
| 1 | Motel | 11 | Strip mall |
| 2 | Clinic | 12 | Furniture store |
| 3 | Depot | 13 | Warehouse |
| 4 | Diner | 14 | Fast food |
| 5 | Arcade | 15 | Drive-in theater |
| 6 | Apartments | 16 | Parking lot |
| 7 | Ranch house | 17 | Police station |
| 8 | Cottage | 18 | Luxury boutique (dedicated mannequins) |
| 9 | Townhouse | 19 | Tavern (no ordinary enemies) |

## Camper

The RV has one front driver seat and four passenger chairs behind it. The hollow shell, upholstered chairs, rear bed, walnut kitchen, sink, refrigerator and storage use original Blender-authored meshes. Generated material atlas: `ArtSource/RV/T_CamperAtlasV14.png`; original generated file retained in Codex generated_images. Prompt requested four flat swatches (pearl/champagne fiberglass, walnut, oatmeal upholstery, warm quartz), no labels or perspective. UV sampling uses inset quadrants; no raster postprocessing.

Press Space while parked to stand in the cabin; WASD walks through the aisle with furniture collision. Look at cabin fixtures and press E while parked: bed skips time using the existing sleep checks; stove consumes food for a hot meal; sink restores water; fridge, pantry and wardrobe open separate VIN-linked inventories. Main cargo remains available. These stores persist across streaming and saves.

## Seats and companion driver

Outside E chooses the nearest unoccupied seat. Inside, 1 is driver, 2–8 choose passenger seats; mouse wheel cycles all seats, including the bus. Seats can change only while stopped and after dismissing a companion driver.

With a waypoint, an active companion and a passenger seat, the companion boards the driver seat and waits for available passenger seats to fill. Cars still require a matching key or completed hotwire. The driver follows the generated road network with speed-sensitive lookahead, turn speed limits, stopping-distance obstacle sweeps and bounded reversing recovery. Drivers can pass stationary obstructions when a swept clearance check keeps the whole vehicle on a wide road; pedestrians cause braking. An impassable route ends in a controlled stop and driver disembarkation after bounded recovery attempts. Routes beyond the navigation horizon extend as intermediate destinations are reached. F asks the driver to stop, then disembark; the player stays seated and may press F again to exit.

No game package is produced by this change.
