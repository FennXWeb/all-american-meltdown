# Settlement building — update 82

## Getting started

Buy a **Settlement Flag** from a supplies merchant, or find one in roadside, motel, military, and depot loot. Existing merchant inventories receive a flag offer when opened if they have room. Right-click the flag in the inventory, or choose **Place a settlement flag** in **Tab → Settlements**. A valid placement consumes one flag and claims a 60-metre radius.

Claims cannot overlap, occupy existing POI grounds, obstruct roads, occupy deep water, or sit on campaign sites or the border checkpoint. Visit a claim on foot to build there. Management and resident assignments remain accessible through the Settlements tab while traveling; physical supplies and treasury collection require visiting the settlement.

## Construction

The 60-piece catalog includes foundations, upper floors, walls, hinged doorways, breakable window walls, roofs, stairs, porches, ramps, railings, furniture, lights, storage, workstations, decor, and insured parking. All construction uses scrap. New structures can have independent outside/inside materials and colors: brick, plaster, wallpaper, wood, tile, metal, and concrete in eight tints. Existing structural pieces can be repainted for two scrap.

Deposit carried scrap into the settlement stockpile. Construction spends stockpiled scrap first, then carried scrap. Moving existing pieces is free. Dismantling returns half of the original cost to the stockpile. Empty individual storage first; occupied bed capacity, dependent structures, and insured parking reservations prevent destructive removal.

| Keyboard/mouse in build mode | Action |
|---|---|
| WASD; Shift | Move camera; move faster |
| Mouse | Look |
| Space / Ctrl | Raise / lower camera |
| Left mouse | Place / confirm move |
| Interact key (default F) | Select existing piece |
| R | Rotate 15 degrees |
| Tab | Open / close catalog |
| G | Toggle structural/grid snapping |
| B | Toggle road-edge snapping |
| Page Up / Page Down | Adjust foundation height |
| Right mouse | Cancel move |
| Delete | Dismantle selected piece |
| T | Apply chosen finishes to selected structure |
| Escape | Cancel selection, return to catalog, or exit |

Structural snapping uses actual piece transforms. Foundations snap edge-to-edge or align outside a road shoulder. Walls attach to deck edges and other walls. Floors and roofs connect to walls, matching decks, and stairs. Wall decor mounts to a wall; ceiling fixtures mount underneath a floor or roof. Placement rejects intersections, occupied world space, unsupported pieces, and blocked door swing areas. A green/red preview reports the reason placement is allowed or rejected.

Controller: left stick moves, right stick looks, Y raises the camera, right-stick click lowers it, left-stick click accelerates, A/RT places, RB selects, X rotates, Menu opens the catalog, LT cancels a move, LB dismantles, D-pad left/right toggles snaps, D-pad up/down adjusts foundation height, and B goes back. Catalog and management support cursor, D-pad, and confirm navigation.

## Residents

Build beds and a HAM radio, then enable broadcasting. An unstaffed radio attracts one resident every eight minutes of active play while a bed is vacant. Recruitment workers shorten this interval, to a minimum of about 2 minutes 40 seconds. Every resident reserves one bed, including companions traveling with the player. Names and voice identities persist.

Each workstation accepts one assigned resident. A resident in the active crew suspends their settlement job. Production advances every five minutes of play, including when the settlement is unloaded; quitting the game or skipping time does not create unlimited offline output.

| Station | Output per work cycle |
|---|---|
| Farm plot | 3 food |
| Scrap sorting | 20 stockpiled scrap |
| Recruitment desk | Faster HAM recruitment |
| Trading counter | 40 credits in treasury |
| Contract office | One eligible contract, up to six pending |
| Expedition staging | Workshop loot in shared supplies |
| Medical station | 1 medkit |
| Water purification | 4 water |
| Guard post | Local defense assignment |

Use **Residents & jobs** to select a resident, assign or clear a station, add them to the active crew, or send them home. Existing companion limits still apply. Residents use the existing companion navigation, door handling, recovery, and settlement defense systems. Generated supplies are retained in shared storage; output pauses when an entire output cannot fit.

## Parking and persistence

A parking lot provides ten reserved bays. Ordinary vehicles reserve one; buses and either RV reserve three adjoining bays. Drive onto the lot, stop the engine, leave the vehicle with all passengers, and register it in **Vehicle parking**. Registration grants ownership and access, preserves cargo and modifications, and marks the vehicle on the map. The vehicle can then be driven normally.

After destruction, the same VIN, cargo, modifications, and paint are restored at the assigned lot after a short replacement delay. Occupied replacement space delays delivery. Releasing a vehicle removes this lot's insurance and frees its reservation; it does not delete the vehicle or its cargo. Transferring a car to the bunker garage releases its outdoor parking reservation.

Claims, transforms, supports, finishes, residents, jobs, reserved beds, recruitment/work timers, stockpiles, treasury, contracts, and parking ownership are included in existing save snapshots. Older saves default to no player-built settlements. Build objects stream through a bounded queue; resident hydration is staggered rather than spawning a whole population in one frame.

Current construction budgets: eight claims, 600 pieces per claim, 100 beds per claim. Shared supplies use a 12×24 inventory. World scrap drops now have greater weight and larger stacks, and scrap stacks hold 240.

## Developer entry points

- `LWSettlement82State.h`: save-only value types, nested in RPG state.
- `LWSettlement82Catalog.cpp`: catalog, finishes, footprint and snap geometry.
- `LWSettlement82Build.cpp`: camera/input, preview, support/overlap validation, transactions.
- `LWSettlement82Pieces.cpp`: construction geometry and reusable interactable furniture.
- `LWSettlement82Residents.cpp`: recruitment, job economy, residents and crew integration.
- `LWSettlement82Parking.cpp`: bay reservations and insured replacement.
- `LWSettlement82HUD.cpp`: catalog and Settlements tab.
- `Tools/import_settlement82.py`: mapped/tintable finishes and targeted loot/item asset updates.
- Automation group: `LethalWorld.Settlement82`.
- Rendered test: `-game -LWV17Smoke -LWUI46Smoke -LWSettlement82Smoke` (isolated automation saves).

Editor-target development only; no packaged build is produced.

## Verification — September 26, 2026

- Development Editor target: successful (`Saved/BuildSettlement82Verified.log`).
- Material and catalog import: seven mapped finishes plus merged flag/scrap rows (`Saved/ImportSettlement82b.log`).
- Rendered 1600×900 integration run: 91 checks, zero failures (`Saved/Settlement82Smoke.log`).
- Final 2560×1080 integration run: 99 checks, zero failures (`Saved/Settlement82Ultrawide.log`). Additional checks exercise actual placement input, snap selection, free movement of filled storage, dismantling safeguards/refunds, and blocked insurance delivery.
- Fourteen Unreal automation tests passed across Settlement82, Inventory, Save43, and Bunker45 (`Saved/Settlement82Regression.log`).
- Save round trips preserve construction, finishes, workers, beds, balances, timers, vehicle identity/upgrades/cargo, and open-door/broken-window state. Queued streaming restoration was exercised in-engine.

Rendered screenshots: [catalog](Update82/Settlement82_Catalog.png), [interior](Update82/Settlement82_Interior.png), [residents](Update82/Settlement82_Residents.png), [parking](Update82/Settlement82_Parking.png). These are engine captures of an isolated test fixture, not concept images. Visual review corrected stair rails, roof finish orientation and deck seams.

Coverage is automated and visual; it is not a sustained human playthrough or a performance benchmark of eight fully populated settlements. Physical controller feel and long-term economic balance still benefit from playtesting.
