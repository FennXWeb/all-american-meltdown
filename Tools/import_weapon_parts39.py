from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve()
script=(root/'Tools/import_models_v5.py').read_text().replace("'ModelsV5'","'WeaponParts39'").replace('models_v5_manifest','models_39_manifest')
exec(compile(script,'parts39_import','exec'),{'__name__':'__main__'})
u.log('WEAPON_PARTS39_IMPORTED 50')

# Preserve designer tuning while exposing the new parts in existing catalog assets.
def add_catalog_entries():
    default=u.new_object(u.LWItemCatalog)
    catalog=u.EditorAssetLibrary.load_asset('/Game/Data/DA_ItemCatalog')
    if catalog:
        entries=list(catalog.get_editor_property('items'))
        ids={str(x.get_editor_property('id')) for x in entries}
        for item in default.get_editor_property('items'):
            if str(item.get_editor_property('category'))=='WeaponPart' and str(item.get_editor_property('id')) not in ids:
                entries.append(item)
        catalog.set_editor_property('items',entries)
        u.EditorAssetLibrary.save_loaded_asset(catalog)
    loot=u.EditorAssetLibrary.load_asset('/Game/Data/DA_LootTable')
    if loot:
        import json
        parts=json.loads((root/'Data/WeaponParts39.json').read_text())
        presets=list(loot.get_editor_property('presets'))
        for preset in presets:
            if str(preset.get_editor_property('context')).lower() not in ('depot','military','trader'):
                continue
            entries=list(preset.get_editor_property('entries'))
            ids={str(e.get_editor_property('item_id')) for e in entries}
            for part in parts:
                if part['id'] in ids: continue
                entry=u.LWLootEntry()
                entry.set_editor_property('item_id',part['id'])
                entry.set_editor_property('weight',.65)
                entry.set_editor_property('min_count',1)
                entry.set_editor_property('max_count',1)
                entry.set_editor_property('b_unique',True)
                entry.set_editor_property('max_per_container',1)
                entries.append(entry)
            preset.set_editor_property('entries',entries)
        loot.set_editor_property('presets',presets)
        u.EditorAssetLibrary.save_loaded_asset(loot)
    u.log('WEAPON_PARTS39_CATALOGS_UPDATED')
add_catalog_entries()

