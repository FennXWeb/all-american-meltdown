# Weapon workbench (revision 39)

Use a weapon workbench in the bunker workshop, settlements, service bays, or other furnished POIs. Existing workbench interactions now open the builder. The new bench includes a vise, drawers, a tool rack, parts trays, and a task lamp.

## Controls

- Click a weapon in the left inventory list to edit it. Click a loose part to add it, or drag it into the preview. Filters and page buttons separate guns, structural parts, and attachments.
- Right drag orbits the preview, middle drag pans, and the wheel zooms. Frame All fits the complete assembly.
- Select a part directly in the 3D view or in the assembly list. Choose Move, Rotate, or Scale. Free moves in the camera plane, rotates across two axes, or scales uniformly; X/Y/Z constrain edits. The + and - buttons provide precise adjustments. Snap can be disabled.
- Use Action selects the receiver/core that determines ammunition, feed behavior, reload, fire mode, and the resulting equipment slot. It does not consume or duplicate the other actions in the build.
- Click Name to type a custom name; Enter finishes editing.
- Remove returns the selected part to the draft inventory. Dismantle returns all parts and the loaded magazine. Repair Armor retains the previous bench repair functionality.
- Apply commits the complete draft. Close or Escape discards uncommitted changes. Undo/Redo cover additions, removals, transformations, action changes, and dismantling.

## Assembly rules

The catalog contains 50 structural part designs covering all 16 existing weapon types: 16 actions, 14 barrel/emitter assemblies, 8 stocks, 6 grips, and 6 feed assemblies. The eight existing attachments can also be placed freely. Parts from different weapon families can be mixed, stacked, reversed, translated, rotated, and independently scaled.

One selected action controls ammunition and firing/reloading behavior. A gun requires an action and at least one barrel or emitter; melee actions do not require a barrel. Additional barrels discharge from their own positions and orientations, sharing the action’s cartridge. A launcher barrel on an SMG action still fires SMG ammunition. A build supports up to 64 parts in a 6-metre workspace; per-axis scale is 0.25–3.0.

Structural parts use Common, Uncommon, Rare, Epic, and Legendary rarities, the existing finish skins, and the six existing legendary effects. Distinct legendary effects combine on a custom weapon; duplicate copies of the same effect do not multiply that effect. Damage, handling, and cycle-speed bonuses also depend on the fitted parts and their rarity. Assembled resale value is based on the owned parts.

Dismantling preserves part identities, rarity, legendary effects, finishes, condition, transforms, and recoverable ammunition. Integral ammunition stays in the action; detachable magazines remain individual inventory items. Partial taser battery charges are preserved as charges. Full-inventory failures leave the inventory unchanged. If the real inventory changes while the workbench is open, Apply refuses the stale draft; reopen the bench to refresh it.

Parts appear in newly rolled depot, military, and trader loot. Existing saved containers retain their saved contents. Dismantling existing weapons is another source. Existing weapons and saves remain supported; custom assemblies serialize through inventory, storage, trading, death drops, and save/load.

## Authoring

- `Data/WeaponParts39.json` documents the generated catalog. `Tools/define_weapon_parts39.py` is the catalog generator and writes the C++ definition include.
- `Tools/make_weapon_parts39.py` authors the 50 FBX part assemblies in Blender; `Tools/import_weapon_parts39.py` imports them and appends missing item/loot catalog entries without resetting designer tuning.
- `Tools/make_weapon_bench39.py` and `Tools/import_weapon_bench39.py` author/import the furniture.
- Materials reuse the project’s worn-metal, wood, rubber, and weapon-finish materials.

## Part catalog

Validation: the Editor target compiled successfully (Saved/Build39Release.log), all 84 project automation tests passed (Saved/Tests39Release.log), and the rendered workbench session passed 26 checks (Saved/Workbench39Final.log). All 50 part meshes and the furniture mesh imported with material and bounds validation. Preview and equipped-weapon screenshots were visually inspected. No packaged build was created.

| ID | Part | Group |
|---|---|---|
| part_action_00 | Crowbar hook | Action |
| part_action_01 | Bat striking head | Action |
| part_action_02 | Pump shotgun action | Action |
| part_action_03 | .357 revolver frame | Action |
| part_action_04 | Sniper receiver | Action |
| part_action_05 | SMG receiver | Action |
| part_action_06 | Combat rifle receiver | Action |
| part_action_07 | LMG receiver | Action |
| part_action_08 | Double-barrel action | Action |
| part_action_09 | Missile launcher chassis | Action |
| part_action_10 | Minigun drive unit | Action |
| part_action_11 | Sawed-off action | Action |
| part_action_12 | .50 pistol frame | Action |
| part_action_13 | M4 receiver | Action |
| part_action_14 | Taser power module | Action |
| part_action_15 | Flamethrower valve body | Action |
| part_barrel_02 | Pump barrel and fore-end | Barrel |
| part_barrel_03 | .357 barrel | Barrel |
| part_barrel_04 | Precision rifle barrel | Barrel |
| part_barrel_05 | SMG barrel shroud | Barrel |
| part_barrel_06 | Combat rifle gas barrel | Barrel |
| part_barrel_07 | LMG heavy barrel | Barrel |
| part_barrel_08 | Twin shotgun barrels | Barrel |
| part_barrel_09 | Launch tube | Barrel |
| part_barrel_10 | Six-barrel rotor | Barrel |
| part_barrel_11 | Short twin barrels | Barrel |
| part_barrel_12 | .50 pistol barrel | Barrel |
| part_barrel_13 | M4 gas barrel | Barrel |
| part_barrel_14 | Taser electrodes | Barrel |
| part_barrel_15 | Flame nozzle | Barrel |
| part_stock_00 | Adjustable carbine stock | Stock |
| part_stock_01 | Fixed rifle stock | Stock |
| part_stock_02 | Folding skeleton stock | Stock |
| part_stock_03 | Padded survival stock | Stock |
| part_stock_04 | Pump shotgun stock | Stock |
| part_stock_05 | Walnut break-action stock | Stock |
| part_stock_06 | Precision cheek-rest stock | Stock |
| part_stock_07 | Heavy support stock | Stock |
| part_grip_00 | Ribbed pistol grip | Grip |
| part_grip_01 | Revolver walnut grip | Grip |
| part_grip_02 | Angled shoulder grip | Grip |
| part_grip_03 | Large pistol grip | Grip |
| part_grip_04 | Steel crowbar handle | Grip |
| part_grip_05 | Wrapped bat handle | Grip |
| part_feed_00 | Tubular feed assembly | Feed |
| part_feed_01 | Six-chamber cylinder | Feed |
| part_feed_02 | Detachable-magazine well | Feed |
| part_feed_03 | Break-action hinge | Feed |
| part_feed_04 | Belt feed and cover | Feed |
| part_feed_05 | Sealed pressure coupling | Feed |
