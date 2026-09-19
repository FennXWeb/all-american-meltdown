# Build 0.7 — Open Country

Editor/source update only. The project owner handles cooking and packaging.

## Combat and progression

Mannequins freeze when watched only while stalking. Damage or close proximity makes them aggressive; they then pursue at 5.1 m/s and attack while watched. Stagger, cover and bunker protection still apply.

Level gains play the original **LevelUp** chime, replaceable through the Audio Manager. The tracked contract now shows its current objective, count and progress bar on the HUD. Delivery counts reflect carried supplies. Tracking and objective progress persist; the Contracts tab identifies the tracked quest. Visit routing supports the new POI types, and return routing selects a populated waystation.

## Vehicles

W/S throttle and brake/reverse, A/D steer, Space brake, R ignition, G glovebox, F exit after stopping. Look at a dashboard control and press E. The extra center-console switch operates the vehicle's special equipment; police cars have a separate siren switch. Outside, G accesses cargo after unlocking.

| Vehicle | Seats, including driver | Cargo | Top speed km/h | Spawn weight | Equipment |
|---|---:|---|---:|---:|---|
| Sedan | 4 | 10×8 | 86 | 30 | Standard cabin |
| Box truck | 2 | 12×28 | 65 | 8 | Rear ramp; driving interlock |
| Police cruiser | 4 | 10×10 | 115 | 5 | Alternating red/blue lights and separate siren |
| RV | 6 | 12×18 | 68 | 5 | Parked rest uses a ration; bed and kitchenette |
| School bus | 11 | 12×22 | 61 | 4 | Passenger cabin and opening door |
| Van | 2 | 12×16 | 79 | 15 | Opening cargo door |
| Pickup | 4 | 12×12 | 94 | 13 | Open bed and tailgate |
| Dirt bike | 1 | 4×4 | 97 | 8 | Two-wheel frame and deployable stand |
| SUV | 7 | 12×12 | 94 | 10 | Switchable traction mode |
| Muscle car | 4 | 9×7 | 126 | 4 | Boosted throttle response |
| Supercar | 2 | 6×5 | 158 | 1 | Sport throttle response |

Weights are relative. Each profile also defines acceleration, reverse speed, wheelbase, steering lock, braking and grip. Steering ramps and recenters; steering lock reduces with speed, power tapers near top speed, coasting includes drag, and tire grip limits cornering acceleration. This remains a swept-chassis bicycle model with terrain probes, rather than a full Chaos suspension/tire simulation.

Companions use available passenger seats; overflow behavior from 0.6 remains. VINs, keys, locks, hotwiring, health, cargo and model identities persist. Existing saved vehicles remain sedans and occupied storage is never shrunk to lose items. Bodies use modular geometry around the working cockpit, with distinct work-vehicle cabs, cargo bodies and equipment. Sirens attract enemies.

## POIs and materials

The five original businesses are rebuilt: fuel stop, motel, clinic, depot and diner. Added: arcade, three-floor apartments, ranch house, cottage, townhouse, megamarket, strip mall, Homeworks furniture store, warehouse, fast-food restaurant, drive-in theater, parking lot and police station.

The new assembly path includes fitted doors, storefront glazing, service rooms, washrooms, baseboards, trim, wall notices and lighting. Apartments have stairs and genuine slab openings; houses have pitched roofs. Furnishings include arcade cabinets, beds, storage, stocked aisles, checkouts, furniture displays, warehouse racks, kitchens, dining tables, theater screens/speaker posts, parking bays, desks, lockers and holding-area bars. Existing furniture, loot, door and breakable-window interactions are retained.

Nine original surface sets with normal maps cover plaster, brick, wallpaper, parquet, tile, corrugated metal, wood doors, painted doors and notices. Architectural surfaces use world-scale mapping to prevent giant stretched textures.

## World layout

Regions provide deterministic streaming ownership. Strongly displaced hubs connect through curved roads, a continuous backbone, optional branches and diagonal links. About 43% of regions host towns; rural regions have fewer properties and longer undeveloped stretches. Driveways join actual road segments. Foundations reject roads and other parcels; settlement foundations also flatten terrain. Vegetation respects building footprints. Roadside vehicles are less frequent and placed outside the travel lane. The map marks only existing waystations.

This is still a region-owned road network, not a river-crossing or urban-planning simulation. Topology and building sizes have changed. Existing inventory/progression is preserved, but old parked vehicles and player positions keep their saved coordinates. A fresh world is best for evaluating generation; this update does not reset the user's save.

## Development

`Tools/make_v7_assets.py` generates the original texture/audio sources. `Tools/build_v7_content.py` imports the assets without packaging; `Tools/Build.ps1 -Content` includes it. Existing audio assignments are preserved. Close Unreal before rebuilding native classes.

`-LWV7Smoke -LWAudioSmoke` runs the live checks with an isolated save and screenshots. See [validation](Validation07.md) for results and limitations.
