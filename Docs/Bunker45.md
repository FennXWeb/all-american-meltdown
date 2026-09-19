# Bunker and vehicle update (45)

## Controls

Use **E** on **Shelter Control**, just inside the bunker entrance. The terminal has Furniture, Expansions, Assignments and Garage tabs. Furniture construction spends scrap carried in the inventory; expansions and vehicle work spend credits.

Choose a furniture item or **Edit Existing Furniture**:

- WASD moves the build camera; mouse looks around.
- Space / Left Ctrl raises / lowers the camera. Shift moves faster.
- E selects an existing furniture piece.
- R rotates 15 degrees; G toggles 20 cm grid snapping.
- Left click places or moves. Right click cancels the current selection.
- Delete dismantles the selection and returns half its scrap cost, rounded down (minimum one).
- Escape exits build mode.

Placement checks floor support, overlap, doors and the elevator opening. Storage must be empty before dismantling. Editing and menu input block weapon attacks. The click that opens build mode cannot place an item immediately.

Twenty furniture choices use the existing game meshes: beds, chairs, lockers, tables, desks, bookcases, pantry cabinets, sideboards, bedside drawers, stoves, sinks, water dispensers, weapon workbenches, radios, lamps, bins, crates, generators, desk accessories and dishes. Existing bunker furniture can also be moved or dismantled. Structural walls, entrance controls, elevator components and lighting infrastructure are protected.

## Expansions and work

Nine bedroom floors add ten bedrooms each, increasing capacity from 10 to 100. Floors cost 2,000 credits initially, increasing by 1,000 per purchase. Every purchased floor includes elevator access. Call the elevator from a landing, enter the platform, and use its control to select a purchased floor. Landing gates close while the platform is elsewhere.

Utility rooms cost 3,000 credits each. Click a resident in Assignments to cycle through purchased workplaces or off duty. Assigning a job recalls that resident from following or settlement duty. Up to eight workers contribute per room. Work advances only while playing, not while paused or offline.

| Room | Effect |
|---|---|
| Trading office | 35 credits per worker every five minutes |
| Salvage workshop | Supply rolls per worker every five minutes |
| Recruitment office | A new resident every twenty minutes, if bedrooms are available |
| Dispatch room | An eligible, unaccepted contract every ten minutes |
| Infirmary | Restores health while the player is in the bunker |
| Hydroponics | Food and water per worker every five minutes |

Supplies arrive in the locker beside the entrance. Workers assigned to distant bunker floors are represented by their saved crew records and appear when that floor is visited. This avoids simulating up to 100 residents across unloaded rooms.

## Garage

Buy the **10-bay garage** for 12,000 credits. The surface lift is south of the bunker entrance. Center a vehicle on the platform, switch off its engine, exit, and use **Store Vehicle**. Companions exit before transfer. The garage reserves three adjacent bays in one row for an RV or bus; other vehicles reserve one bay. Reservations remain occupied while an owned vehicle is outside.

Stored vehicles retain their VIN and all their existing storage records. Owned vehicles are marked on the map. A destroyed owned vehicle is replaced in its reserved garage bay after 30 seconds of active play, with its cargo, paint and upgrades retained. Use the garage terminal to send a stored vehicle to the surface; retrieval checks that the platform is clear. Release Ownership retrieves the vehicle and relinquishes its bay and insurance.

The garage offers repairs, eleven paint finishes and 50 upgrades:

- 30 universal upgrades: three choices each for engine, gearbox, brakes, tires, suspension, armor, fuel tank, cargo, lighting and efficiency.
- 20 exclusive upgrades: eight for the RV, four each for police cars and buses, and two each for dirt bikes and supercars.

A new upgrade replaces the installed upgrade in the same slot. Effects change acceleration, maximum speed, braking, grip, clearance, damage resistance, fuel capacity, cargo rows, headlight output or fuel efficiency. Compatible exclusive upgrades can combine with universal upgrades.

## Other changes

Engine layers are 1.8 times their previous gain (about +5.1 dB), retaining existing randomized audio choices and continuous crossfades. A Slate loading screen covers map loading and synchronous new-game/save restoration in standalone play.

Terrain recovery can lift a moving chassis clear of a road/terrain collision at a steep transition. It rejects obstructed destinations and structures along the recovery path. Stored cars cannot use spawn recovery to relocate from their bay onto the surface.

Companion recovery measures progress once per second. Prolonged obstruction, floor separation, or distance triggers a search for a clear, supported position near the player. It avoids the player's vehicle footprint and does not move seated or incapacitated companions. Existing navigation and door handling remain the primary movement system.

## Persistence and verification

Bunker purchases, custom furniture transforms/removals, worker assignments and work progress are included in the existing asynchronous save snapshots. Vehicle records persist ownership, bay reservations, mods, paint and replacement progress. Saves made during the surface transfer point to the final secured bay.

Editor target only; no packaging was performed. Runtime test switch: `-game -LWV17Smoke -LWBunker45Smoke -LWLoading45Smoke`. Automation tests: `LethalWorld.Bunker45`.

Verification completed: editor build succeeded; all 87 automation tests passed (`Saved/Tests45.log`), and all 45 in-engine checks passed (`Saved/Bunker45_FinalRuntime.log`). Offscreen screenshots are in `Saved/ScreenshotsV17/Bunker45_*.png`.
