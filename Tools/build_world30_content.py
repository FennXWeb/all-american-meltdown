"""Merge new content entries only. Never overwrites existing designer choices or packages."""
from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve()
lib=u.EditorAssetLibrary
exec(compile((root/'Tools/build_map30_content.py').read_text(),'map30','exec'))
cat=lib.load_asset('/Game/Data/DA_ItemCatalog')
if cat:
 rows=list(cat.items)
 for item in u.get_default_object(u.LWItemCatalog).items:
  if str(item.id)=='gas_can' and not any(str(old.id)=='gas_can' for old in rows):rows.append(item)
 cat.items=rows
 lib.save_loaded_asset(cat)
for path in lib.list_assets('/Game/Data',recursive=True):
 asset=lib.load_asset(path)
 if isinstance(asset,u.LWLootTable):
  rows=list(asset.presets)
  defaults=u.get_default_object(u.LWLootTable).presets
  for row in rows:
   native=next((r for r in defaults if str(r.context)==str(row.context)),None)
   if not native:continue
   entries=list(row.entries)
   for entry in native.entries:
    if str(entry.item_id)=='gas_can' and not any(str(e.item_id)=='gas_can' for e in entries):entries.append(entry)
   row.entries=entries
  asset.presets=rows
  lib.save_loaded_asset(asset)
 if isinstance(asset,u.LWSpawnTable):
  rows=list(asset.get_editor_property("POIs"))
  for row in u.get_default_object(u.LWSpawnTable).get_editor_property("POIs"):
   if row.get_editor_property("POIType")>=58 and not any(r.get_editor_property("POIType")==row.get_editor_property("POIType") for r in rows):rows.append(row)
  asset.set_editor_property("POIs",rows)
  lib.save_loaded_asset(asset)
u.log('AAM_WORLD30_CONTENT_COMPLETE')
