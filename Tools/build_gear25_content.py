from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary
exec(compile((root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','ModelsV25').replace('models_v5_manifest','models_v25_manifest'),'gear25_import','exec'),{'__name__':'__main__'})
newids={'sling_pack','hiking_pack','military_pack','expedition_pack','night_vision'}
cat=lib.load_asset('/Game/Data/DA_ItemCatalog')
if cat:
 rows=list(cat.items)
 for d in u.get_default_object(u.LWItemCatalog).items:
  old=next((x for x in rows if str(x.id)==str(d.id)),None)
  if old is None:rows.append(d)
  elif str(d.id)=='backpack':old.mesh=d.mesh;old.container_width=12;old.container_height=24
 for d in rows:
  slots=list(d.equip_slots)
  if 'Primary' in [str(x) for x in slots] and 'RigPrimary' not in [str(x) for x in slots]:slots.append('RigPrimary');d.equip_slots=slots
 cat.items=rows;lib.save_loaded_asset(cat)
native=u.get_default_object(u.LWLootTable)
for path in lib.list_assets('/Game/Data',recursive=True):
 a=lib.load_asset(path)
 if not isinstance(a,u.LWLootTable):continue
 rows=list(a.presets)
 for row in rows:
  default=next((d for d in native.presets if str(d.context)==str(row.context)),None)
  if not default:continue
  entries=list(row.entries)
  for d in default.entries:
   if str(d.item_id) in newids and not any(str(e.item_id)==str(d.item_id) for e in entries):entries.append(d)
  row.entries=entries
 a.presets=rows;lib.save_loaded_asset(a)
u.log('AAM_GEAR25_CONTENT_COMPLETE')
