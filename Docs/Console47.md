# Command console

Press the **tilde/backtick key (`~`)** to open or close the console. Type a command and press Enter. Escape also closes it (or clears the current typed line first). Gameplay pauses while it is open; an already-open pause menu remains paused after closing the console.

Use **Up/Down** for history, **Tab** for autocomplete, and **Page Up/Page Down** to scroll output. Type `help` for the in-game command list.

| Command | Effect |
| --- | --- |
| `debugmenu` | Open the searchable debug menu for cheats, items, NPCs, vehicles, and POIs. Alias: `debug`. See [Debug menu](DebugMenu67.md). |
| `money 5000` | Add 5,000 credits. Alias: `addmoney`. |
| `items` | List every item ID from the active catalog, including custom catalog entries. |
| `items shotgun` | Search item IDs, names, and categories. |
| `give medkit 5` | Add five medkits, subject to inventory space. Alias: `additem`. |
| `give revolver` | Add one .357 revolver. |
| `give ammo_357 60` | Add 60 loose .357 rounds. |
| `heal` | Restore health, stamina, food, and water. |
| `xp 1000` | Award XP using the existing progression system and XP bonuses. |
| `skillpoints 10` | Add ten perk points. |
| `god` | Toggle damage immunity. `god on` and `god off` explicitly set it. |
| `time 6` | Set the world clock to 06:00. Accepts integer hours 0–23. |
| `clear` | Clear console output. |

Game cheats require a started game and a living player. Invalid IDs and amounts are rejected. A failed item grant adds nothing; it does not overflow the grid or discard existing items. Item quantities are capped at 1,000 per command, credits at 1 billion, XP at 100,000, and skill points at 1,000. Weapons still need their normal ammunition and magazines.

Credits, inventory, XP, and clock changes follow normal game saving. God mode lasts for the current session and is not written into saves. Existing Unreal console commands remain available in build configurations that support them.

The console is installed explicitly in the game viewport so the game commands do not depend on Unreal's development-only default console creation. No packaged build is produced by this update.

Validation: editor target compiled successfully (Saved/Build47c.log). Both Console47 automation tests passed, and all 20 isolated runtime checks passed (Saved/Console47_Runtime.log). The console help screenshot was visually reviewed. Nothing was packaged.
