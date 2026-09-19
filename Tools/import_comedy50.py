import unreal as u
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary
ml=u.MaterialEditingLibrary;tools=u.AssetToolsHelpers.get_asset_tools()
for name,color,roughness in [('ComedySkin50',(.55,.34,.22),.78),('ComedyPlastic50',(.65,.69,.56),.27)]:
 path='/Game/Materials/M_'+name
 material=lib.load_asset(path) if lib.does_asset_exist(path) else tools.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew())
 ml.delete_all_material_expressions(material)
 c=ml.create_material_expression(material,u.MaterialExpressionVectorParameter);c.set_editor_property('parameter_name','Tint');c.set_editor_property('default_value',u.LinearColor(*color,1));ml.connect_material_property(c,'',u.MaterialProperty.MP_BASE_COLOR)
 r=ml.create_material_expression(material,u.MaterialExpressionConstant);r.r=roughness;ml.connect_material_property(r,'',u.MaterialProperty.MP_ROUGHNESS)
 ml.set_material_usage(material,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES);ml.recompile_material(material);lib.save_loaded_asset(material)
source=(root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','ModelsV50').replace('models_v5_manifest','models_v50_manifest')
exec(compile(source,'comedy50_import','exec'),{'__name__':'__main__'})
cat=lib.load_asset('/Game/Data/DA_ItemCatalog')
if cat:
 rows=list(cat.items)
 for d in u.get_default_object(u.LWItemCatalog).items:
  if not any(str(x.id)==str(d.id) for x in rows):rows.append(d)
 cat.items=rows;lib.save_loaded_asset(cat)
ids={f'part_action_{w}' for w in range(16,21)}|{'part_barrel_16','part_barrel_20','part_feed_17','part_feed_18','tenbarrel','giant_glock','questionable_ak','finger_guns','budget_cut','mag_giant50','mag_ak75'}
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
   if str(d.item_id) in ids and not any(str(e.item_id)==str(d.item_id) for e in entries):entries.append(d)
  row.entries=entries
 a.presets=rows;lib.save_loaded_asset(a)
u.log('COMEDY50_IMPORTED');u.SystemLibrary.quit_editor()
