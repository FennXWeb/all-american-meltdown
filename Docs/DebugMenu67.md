# Debug menu

While playing, press **~**, enter **`debugmenu`**, and press Enter. **`debug`** is an alias. Escape, ~, the Close button, or controller B closes the menu. The world pauses while it is open; closing restores the previous pause/menu state.

The five tabs share a searchable, sortable list:

- **Cheats:** credits, health/stamina/food/water, god mode, XP, skill points, and time of day.
- **Items:** every active inventory definition, including ammunition, magazines, equipment, and attachments. Select a quantity and add it to your inventory. Item details include grid size, stacking, price, and compatible ammunition/magazine where applicable.
- **NPCs:** all enemy types plus survivors, merchants, medics, and companion candidates. Spawn 1–20 ahead of the player, outside the bunker.
- **Vehicles:** every vehicle model, with seating, cargo, size, speed, and feature details. Spawn outside the bunker with clear space ahead; the vehicle starts unlocked and hotwired.
- **POIs:** all generated POI types, including dungeons and unique landmarks. Select a search radius (1–20 km) to locate the nearest matching site and set a waypoint. This searches the seeded world without creating duplicate buildings. Large searches take longer and can be cancelled; closing the menu cancels its outstanding search.

Search matches names, IDs, categories, and details, ignoring case. Multiple search words must all match. Combine search with a category filter, or use Reset filters. Click **Name**, **ID**, or **Category** to sort; click again to reverse the order. Numeric POI IDs sort numerically.

Select an entry and use its action button, double-click a row, or press **Ctrl+Enter**. **Ctrl+F** focuses search. The amount field accepts typing as well as dragging, and is bounded to the existing console command limits. Results and errors appear at the bottom and in console history.

Actions use the existing cheat implementation: inventory grants respect available space, NPC spawns last for the session, vehicles and progression changes follow normal saves, and god mode is session-only. Input is captured by the menu so searching, clicking, or using its controls cannot attack or drive underneath it.

## Implementation and checks

`LWDebugCatalog67` builds lists from the runtime item/vehicle/POI catalogs. NPCs share one catalog with the text console. `LWDebugMenu67` owns a virtualized Slate list and restores input/pause state on close. POI results arrive through the console's existing incremental locator.

Focused automation: `LethalWorld.Debug67` covers catalog completeness, filtering, sorting, unique IDs, and bounded command construction. Runtime smoke: `-LWV17Smoke -LWUI46Smoke -LWDebug67Smoke` exercises console launch, Slate text input, grants, spawning, pause/input restoration, asynchronous POI results, and cancellation using an isolated test world.

Validated on September 21, 2026: editor build succeeded (`Saved/BuildDebug67f.log`), both focused automation tests passed (`Saved/Debug67Automation.log`), and all 32 runtime checks passed (`Saved/Debug67SmokeVerified.log`). Native Slate typing, mouse movement/clicks, Ctrl+F, Ctrl+Enter, and Escape were exercised. Screenshots of the cheats, items, vehicles, and completed POI result were reviewed. Test runs used isolated saves; original graphics settings were restored. No build was packaged.
