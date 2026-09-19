"""Seed the editable loot table and item catalog from their native C++ defaults.

Run after compiling the module, inside Unreal's Python environment, for example:
  UnrealEditor-Cmd.exe LethalWorld.uproject -run=pythonscript \
    -script="Tools/build_loot_v2.py" -unattended -nop4

Creates /Game/Data/DA_LootTable and /Game/Data/DA_ItemCatalog if absent. Existing
designer edits are preserved. To deliberately replace edits, pass --reset-loot
and/or --reset-catalog in the script arguments. No map, project setting, or other
asset is changed. The script reads reflected defaults rather than duplicating
the C++ balance tables. Runtime has native fallbacks before either asset exists.

Cook /Game/Data along with the game's runtime data when packaging: the optional
runtime lookup uses a path, so a map reference is not created by this script.
"""

import argparse
import math

import unreal as u


DATA_PATH = "/Game/Data"
EXPECTED_CONTEXTS = {"gas", "motel", "clinic", "depot", "diner", "road", "military", "trader"}


def _native_class(name):
    cls = getattr(u, name, None)
    if cls is None:
        raise RuntimeError(
            f"unreal.{name} is unavailable. Compile the LethalWorld module with "
            "the inventory and loot files, then run this script in Unreal Python."
        )
    return cls


def _seed_asset(name, cls, properties, reset):
    """Return (asset, changed); never save an untouched asset."""
    library = u.EditorAssetLibrary
    path = f"{DATA_PATH}/{name}"
    exists = library.does_asset_exist(path)
    if exists:
        asset = library.load_asset(path)
        if asset is None or not isinstance(asset, cls):
            raise RuntimeError(f"{path} exists but is not a {cls.__name__}; leaving it untouched.")
        if not reset:
            u.log(f"Loot v2: preserving existing {path}")
            return asset, False
    else:
        factory = u.DataAssetFactory()
        factory.set_editor_property("data_asset_class", cls)
        asset = u.AssetToolsHelpers.get_asset_tools().create_asset(name, DATA_PATH, cls, factory)
        if asset is None:
            raise RuntimeError(f"Failed to create {path}")

    defaults = u.get_default_object(cls)
    for prop in properties:
        asset.set_editor_property(prop, defaults.get_editor_property(prop))
    return asset, True


def _name(value):
    return str(value).lower()


def _validate_loot(table, catalog, strict):
    """Check seeded defaults; warn about existing edits without rewriting them."""
    issues = []
    # Inventory merges asset overrides onto native definitions by Id.
    definitions = {
        _name(item.get_editor_property("id")): item
        for item in u.get_default_object(_native_class("LWItemCatalog")).get_editor_property("items")
    }
    definitions.update({
        _name(item.get_editor_property("id")): item
        for item in catalog.get_editor_property("items")
    })
    seen_contexts = set()
    for preset in table.get_editor_property("presets"):
        context = _name(preset.get_editor_property("context"))
        if context in seen_contexts:
            issues.append(f"Duplicate context {context}: runtime uses the first preset.")
        seen_contexts.add(context)
        empty = preset.get_editor_property("empty_chance")
        if not math.isfinite(empty) or not 0 <= empty <= 1:
            issues.append(f"{context}: empty chance must be finite and in [0,1].")
        pools = [("regular", preset.get_editor_property("entries"))]
        pools.extend(
            (_name(group.get_editor_property("name")), group.get_editor_property("entries"))
            for group in preset.get_editor_property("guaranteed_groups")
        )
        for group_name, entries in pools:
            for entry in entries:
                item_id = _name(entry.get_editor_property("item_id"))
                label = f"{context}/{group_name}/{item_id}"
                if item_id not in definitions:
                    issues.append(f"{label}: item is missing from the catalog and native defaults.")
                weight = entry.get_editor_property("weight")
                if not math.isfinite(weight) or weight < 0:
                    issues.append(f"{label}: invalid weight (runtime disables this row).")
                low = entry.get_editor_property("min_count")
                high = entry.get_editor_property("max_count")
                if low < 1 or high < low:
                    issues.append(f"{label}: expected 1 <= min_count <= max_count.")
                if entry.get_editor_property("max_per_container") < 0:
                    issues.append(f"{label}: max_per_container must be nonnegative.")
    if strict and not EXPECTED_CONTEXTS.issubset(seen_contexts):
        issues.append(f"Native contexts missing: {sorted(EXPECTED_CONTEXTS - seen_contexts)}")
    fallback = _name(table.get_editor_property("default_context"))
    if fallback not in seen_contexts:
        issues.append(f"Fallback context {fallback} is absent; unknown contexts will be empty.")
    if strict and issues:
        raise RuntimeError("Loot default validation failed:\n" + "\n".join(issues))
    for issue in issues:
        u.log_warning(f"Loot v2: {issue}")
    return len(seen_contexts)


def main(reset_loot=False, reset_catalog=False):
    loot_class = _native_class("LWLootTable")
    catalog_class = _native_class("LWItemCatalog")
    u.EditorAssetLibrary.make_directory(DATA_PATH)
    catalog, catalog_changed = _seed_asset("DA_ItemCatalog", catalog_class, ("items",), reset_catalog)
    table, table_changed = _seed_asset(
        "DA_LootTable", loot_class,
        ("presets", "rarity_multipliers", "default_context"), reset_loot,
    )
    count = _validate_loot(table, catalog, strict=table_changed)
    for asset, changed in ((catalog, catalog_changed), (table, table_changed)):
        if changed:
            if not u.EditorAssetLibrary.save_loaded_asset(asset, only_if_is_dirty=False):
                raise RuntimeError(f"Failed to save {asset.get_path_name()}")
            u.log(f"Loot v2: saved {asset.get_path_name()}")
    u.log(f"Loot v2 ready: {count} editable contexts; existing edits preserved unless explicitly reset.")
    return table, catalog


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--reset-loot", action="store_true", help="Replace existing loot edits with native defaults.")
    parser.add_argument("--reset-catalog", action="store_true", help="Replace existing item edits with native defaults.")
    args, _ = parser.parse_known_args()
    main(reset_loot=args.reset_loot, reset_catalog=args.reset_catalog)
