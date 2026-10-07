# Gunsmith, mystic finishes and save slots

The furniture weapon workbench now edits complete weapons with fixed compatible mounts. Freeform assembly, transforming individual weapon parts and dismantling weapons are retired. Legacy part definitions remain readable solely for old save compatibility.

## Gunsmith

Select an owned weapon, then Attachments or Camos. Orbit with right mouse, pan with middle mouse and zoom with the wheel. The preview includes the original moving parts (magazines, cylinders, bolts and break-open barrels). Changes remain a draft until Apply; closing discards them. Undo/redo covers attachment and camo edits. A fingerprint prevents applying a stale draft over inventory changes.

Five attachments can be installed across optic, muzzle, barrel, stock, rear grip, underbarrel grip, light and laser mounts. Compatibility remains weapon-specific. Installing consumes the owned attachment; replacement/removal returns the previous one and refuses the operation if there is no inventory space. Twelve new attachments join the eight existing ones. New attachments appear in military, depot and trader loot pools. Their damage, recoil, spread and fire interval tradeoffs affect combat. Suppressors also reduce shot volume and AI noise radius.

Existing custom guns become their saved base weapon with compatible attachments, original identity, rarity, legendary modifier and ammunition retained. Loose structural parts become scrap. Migration covers inventory, stash, world containers and settlement consignment inventory. New structural part loot is filtered even from older authored loot tables; part IDs are hidden from the item console listing and legacy give requests produce scrap.

## Camos and mystics

Each weapon type has its own progress within that survivor's save. Challenge unlocks apply to all copies of that weapon. Factory finish is always available. Camos unlock at 10, 25, 50, 15 headshot kills, 75 kills, 30 headshot kills, 150 kills (Gold), five legendary kills (Platinum) and 250 kills (Mystic). Melee weapons and the missile launcher use ordinary kills for the headshot categories; the taser uses distinct enemies stunned. Progress is awarded once on enemy death, not by repeated hits on corpses; residents are excluded. Taser stun progress is awarded once per enemy instance. New loot no longer rolls random finishes; legacy finishes already on saved weapons remain readable.

There are 21 named mystic models, including Gravebreaker, Extreme Slugger, Caldera, Hex Verdict, Absolute Zero, Venom Pulse, Obsidian Reign, Thunder Engine, Hell's Gate, Sunfall, Reactor Hydra, Blood Oath, Gilded Tyrant, Storm Sovereign, Tesla's Judgment, Toxic Furnace, Ten Hells, Titan's Decree, Cursed Exchange, Astral Hands and Liquid Assets.

Mystics use replacement meshes with fitted conduits, cores, fractured armor, crystals or barbed wire, a generated eight-surface atlas, pulsing emissive materials and eight lightweight animated liquid drops or orbiting fragments. They are cosmetic: the weapon's ammo, firing and reload behavior remain intact. Extreme Slugger has an actual helical wire mesh and hooked barbs. Magma, acid and blood variants drip; other themes orbit small emissive fragments. Effects work in the workbench preview as well as in first person.

Editable source and FBXs: `ArtSource/Arsenal62/MysticArsenal62.blend`, `manifest.json`. Rebuild with `Tools/build_arsenal62.py`, import with `Tools/import_arsenal62.py`. No third-party game assets were used.

## Saving

Save Game and Load Game are available from the pause menu; Load Game is also available from the main menu. Twenty manual slots are independent of the single autosave slot. Rows show survivor name, level, day and timestamp. Existing slots require overwrite confirmation; loading warns about unsaved progress. Continue selects the newest save, with the legacy survivor file used when no new slots exist.

The existing detached snapshot/background serialization pipeline remains in use. Manual requests target an explicit slot without changing the autosave destination. Managed saves first write a temporary file, retain the previous file as `.bak`, then replace the destination. Metadata is a small sidecar, avoiding deserializing entire worlds just to draw the menu. The original legacy save file is not overwritten by new autosaves.

## Verification

Focused automation: `LethalWorld.Arsenal62`, `LethalWorld.Save43.DetachedValueSnapshot`. In-engine workflow: `-LWV17Smoke -LWUI46Smoke -LWArsenal62Smoke`. Test manual slots use the `AAM_Test62_` prefix. Runtime screenshots are under `Saved/ScreenshotsV17/Arsenal62_*`. Compile only the Editor target; do not package.

### Verified September 20, 2026

- Unreal 5.8 Development Editor build succeeded (`Saved/BuildArsenal62g.log`). No packaged build was created.
- All four focused automation tests passed (`Saved/Arsenal62TestsC.log`), including actual disk overwrite, previous-snapshot backup and manual/autosave isolation.
- The generated asset import validated all 34 mesh/material assignments and bounds (`Saved/Arsenal62ImportC.log`). The DX11 runtime no longer reports mystic material compile failures; magma and Extreme Slugger previews were visually inspected.
- The in-engine workflow checks attachment limits, replacement transactions, undo/redo, ammo preservation, UI fire suppression, per-weapon challenge unlocks, and saved camo/progress restoration. Screenshots: `Saved/ScreenshotsV17/Arsenal62_*`.
- Final in-engine pass: 46 checks, zero failures (`Saved/Arsenal62SmokeC.log`). Original user graphics/settings file restored after all test processes exited.
