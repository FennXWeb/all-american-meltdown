# Interface overhaul — update 55

## Shared presentation
All Canvas screen families now use a common control, panel and typography layer in `LWInterface55.cpp`. Existing gameplay transactions remain separate from presentation. This covers menus, settings, world setup, character creation, inventory/trading, map/minimap, progression, contracts, crew, dialogue, collections, card games, security minigames, workbench, bunker/garage, story overlays and the gameplay HUD. The engine developer console retains its command/history system.

- Generated menu painting with subtle camera drift, pointer parallax, ash and existing animated transparent title logo.
- Generated worn-metal panel surface, restrained warm accents and clearer monochrome typography. Semantic rarity, danger and item-placement colors remain intact.
- Frame-rate independent control hover/focus animation, animated active-tab indicator, interpolated health/stamina/XP bars and screen text entrance fading. Timers use real time so menus remain responsive while the world is paused.
- Shared spatial keyboard focus and Enter activation for registered controls. Pointer-driven tools retain direct manipulation. Bunker terminal retains its dedicated arrow navigation; creator typing and lockpick controls are not intercepted.
- Centered 1280x720 modal design space with scaled input and hit boxes, preserving aspect ratio on narrower and wider screens. Scene previews remain visible behind their panels.
- Reduced-motion and high-contrast settings saved in game user settings. Draggable sensitivity and volume sliders update immediately, with disk persistence on release rather than every frame. Enter confirms once per press.
- Long controls fit or truncate safely, with full-label tooltips. Crew is paginated to nine members per page, and dialogue choices to five. Dialogue uses a dedicated lower-screen layout. Inventory grids no longer show redundant row/column coordinates.
- Preloaded UI artwork/font and bounded transient animation state. No scene capture is added for decorative UI.
- Loading screen uses the same artwork and palette, an activity indicator and rotating survival tips; it does not invent percentage progress.

## Artwork
Built-in image generation was used. Source PNGs and exact prompts: `ArtSource/UI55/`. Imported Unreal textures: `/Game/Art/UI55/T_Menu55`, `/Game/Art/UI55/T_Surface55`. `Content/UI55/T_Menu55.png` is also staged as a runtime dependency for the threaded Slate loading screen. Reimport with `Tools/import_ui55.py`.

## Validation
- Editor Development build passed: `Saved/Build55Last.log`. No packaged build was produced.
- Generated texture import passed: `Saved/Import55Fixed.log`.
- The 18-phase UI sweep passed all 10 functional checks at 1280x720, forced 1024x768 and forced 2560x1080. Actual PNG dimensions were verified; the earlier automatically clamped runs are not used as aspect-ratio evidence.
- Logs: `Saved/UI55_1280x720.log`, `Saved/UI55_Force_1024x768.log`, `Saved/UI55_Force_2560x1080.log`.
- Final 1280x720 sweep passed 12 checks after adding real-container lockpicking assertions (`Saved/UI55Last.log`).
- Twenty dialogue/menu firing regression checks passed with the rebuilt loading screen enabled: `Saved/UIClick55.log`.
- Screenshots reviewed for menus/settings, inventory, map, skills, crew, collection, card games, terminal, dialogue, lockpicking, loaded weapon preview and character creation. Final captures: `Saved/UI55/<resolution>/`.
- Tests use the existing isolated automation save slot. No player save is replaced.

These are targeted input, rendering and layout checks. Pointer tools retain their specialized controls; keyboard focus supports registered buttons rather than providing full gamepad manipulation of inventory grids or 3D gizmos. The developer console keeps Unreal's command/history UI.
