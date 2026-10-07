# Update 81 — main menu

The title screen uses a large Continue tile, a live render of the saved survivor and four smaller tiles for New survivor, Load game, Settings and What's new. Continue shows the saved level, credits, Canadian dollars, location and active mission. Quit remains separate. The existing pause menu and gameplay screens are retained.

## Behavior

- The preview reads the latest save asynchronously once, without applying it to the player or generating world chunks. Continue loads the displayed slot. Empty and unreadable saves leave Continue disabled; New survivor and Load game remain available.
- The portrait uses the existing character meshes, clothing, hair and appearance parameters from the save. It is captured at 480 × 600, at most 20 times per second, only while visible. Reduced Motion freezes it. Starting gameplay or destroying the HUD releases the preview actor.
- New saves store precise location and mission labels alongside the existing save payload. Old saves resolve their mission from quest state and their location from the nearest authored town (or bunker); no save conversion is required.
- Background art has independent skyline, mist and foreground layers with slow motion and smoothed cursor parallax. Reduced Motion disables these animations. Tile hover/focus transitions respect the same preference. High Contrast uses opaque tile panels.
- The 1170-unit tile layout centers inside the available viewport; artwork covers the full width. Existing keyboard and controller navigation, mouse first-click behavior and remapped controls are retained.
- What's new contains a selectable history of updates. Add future notes to `LWMenu81::Releases()` in `Source/LethalWorld/LWMainMenu81.cpp`. The notes panel owns its hitboxes and closes with Back, Escape or controller B.

## Art

Three original bitmap layers were generated using the built-in image generation tool. Source PNGs are in `ArtSource/Menu81`; imported UI textures are in `/Game/Art/Menu81`. Foreground and mist retain real transparency. The same folder contains `M_Studio81`, a neutral unlit portrait backdrop material. `Tools/import_menu81.py` repeats the import. No existing art was overwritten.

See [the generation prompts](Update81/ArtPrompts.md) for provenance.

## Validation

- Unreal 5.8 Development Editor build passed: `Saved/BuildMenu81Final.log`.
- Three UI textures and the portrait backdrop material imported: `Saved/ImportMenu81Final.log`.
- Final 1600 × 900 rendered run: **48 checks passed, zero failures** (`Saved/Menu81FinalSmoke.log`).
- Final 2560 × 1080 rendered run: **48 checks passed, zero failures** (`Saved/Menu81FinalWideSmoke.log`).
- Tests cover real viewport first-click input, keyboard/controller navigation (including controller Back), async preview data, safe empty/missing save behavior, reduced-motion capture throttling, submenu transitions, exact-slot Continue readback and preview cleanup.
- Final screenshots of the main menu, notes and empty-save states are in `Docs/Update81/`. The standard and ultrawide main-menu captures were visually inspected, along with the notes layout and empty-save state. The test survivor shown is an isolated fixture; normal play uses the actual saved identity.
- User settings were restored after Unreal exited and verified against the original backup hash.

Test entry point: `-game -LWV17Smoke -LWUI46Smoke -LWMenu81Smoke`. Save fixtures use the existing isolated automation prefix. Legacy location labels show the nearest authored town until a new save records an exact location.

No packaged build is produced.
