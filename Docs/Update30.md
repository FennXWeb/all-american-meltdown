# World and exploration update

Implemented in Unreal source and editor assets. No cooking or packaging.

- Generated transparent atlas for all 64 POI types, used on the map and minimap. Discovery still gates visibility. Atlas: `ContentSource/Map30/T_POIAtlas30.png`; Unreal asset: `/Game/Art/Textures/T_POIAtlas30`.
- Seeded place names remain stable after streaming and loading. Map hover includes a survey address to distinguish matching names or hashes. Authored one-off landmark names remain intact. The HUD and discovery alert use the shorter name. Previously discovered sites receive updated names when revisited without another XP reward.
- Last driven vehicle appears on both maps, including after streaming out. Taking a passenger seat does not overwrite it; a destroyed last vehicle is labeled as a wreck.
- Reusable 20 L gas cans: equip one in Tool, park and stop the engine, exit, then use the car to refuel. Intact gas pumps refill carried gas cans and flamethrower canisters. Empty cans are retained. Fuel capacity and consumption vary with vehicle size; dashboard and item inspection show remaining fuel.
- Companion boarding prioritizes the player's available seats. Overflow convoy drivers can start their vehicles immediately. Vehicle footprints include long RV overhangs; obstructed spawns seek clear space and remain hidden while retrying if no valid space exists.
- Companion walking reacts to doors, stairs, changing obstacles and stalled routes; combat includes threat selection and friendly-fire checks. Combat music requires a relevant visible enemy, rather than distance or alert alone.
- Weapon inspection identifies compatible ammunition and magazine/feed type. Reflex/holographic ADS aligns the actual sight aperture with the firing ray. World labels turn to their readable face when viewed from behind.
- Six added POIs: shopping mall, prison, church, car dealership, hardware store and theme park. Theme park rides are static scenery. Survivor stops reuse trading, recruitment and contract dialogue. Dungeon finales have a three-star legendary guardian.

See `POIExpansion30.md`, `VehicleFuelIntegration.md`, `WeaponAmmoSightTextFix.md`, and `CompanionNavigation.md` for implementation details and limitations. Integrated validation results are recorded below.

## Icon provenance
Built-in image generation was used. The original RGBA output was copied intact into the project; transparency was preserved. Reimport with `Tools/build_map30_content.py` using Unreal Python. No external image API key is required.

Prompt: Create a production game UI map icon atlas for All American Meltdown, PS1 post-apocalyptic survival game. One square RGBA transparent sprite sheet, exact 8 columns x 8 rows, equal cells with generous empty margins, no text, labels, grid lines or background. Monochrome ivory bold ink stamp pictograms with subtle distressed pixels inside strokes and consistent chunky line weight. Exact ordered icons left to right then next row:
1. Fuel pump, motel bed, medical clinic, freight depot, diner cup, arcade joystick, apartments, ranch.
2. Cottage, townhouse, market cart, plaza storefronts, furniture chair, warehouse forklift, burger, drive-in.
3. Parking, police badge, clothing hanger, tavern mug, financial tower, casino cards/dice, pumpworks, quarantine.
4. Remand prison, bullion vault, microphone, steelworks, microscope, missile silo, train interchange, arcology.
5. Airport, broadcast antenna, seed vault, observatory, presidential train, dam, treasury, ash chapel.
6. Film projector, lighthouse, conservatory, arena fists, rocket museum, weather radar, courthouse, museum.
7. Excavation, crashed aircraft, motel key, telescope dish, emergency ark, bunker hatch, cave, tunnel.
8. Residential tower, medical tower, shopping mall, prison watchtower, church, dealership car, hardware tools, ferris wheel.
Avoid shadows or colored backgrounds. Preserve exactly these 64 cells in order with no merged icons, transparent alpha between all icons, game-ready rather than a presentation.

## Integrated validation (September 9, 2026)
The editor target compiled successfully (`Saved/BuildWorld30c.log`). The integrated automation run passed all 67 tests (`Saved/World30FinalReport/index.json`). Rendered vehicle checks passed 64/64 and rendered POI checks passed 133/133, including actual survivor trade/recruitment/contracts, all six layouts, generated map icons, and the three-star guardian reward gate. Twenty POI/map screenshots are available in `Saved/ScreenshotsV17`. Companion navigation and combat-awareness checks passed 39/39 (`Saved/NavStackCornerTrace.log`) after the final editor build (`Saved/BuildNavStackCorner.log`). The final fix validates clearance before skipping a path corner, preventing repeated stairwell-wall collisions. Diagnostics are opt-in. Combined coverage: 67 automated tests and 236 rendered checks, all passing in their final runs. No build was cooked or packaged.

