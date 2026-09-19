"""Update 18 additive assets; preserves artist audio assignments and catalog overrides."""
from pathlib import Path
import unreal as u,runpy
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary;tools=u.AssetToolsHelpers.get_asset_tools();ml=u.MaterialEditingLibrary
texture='/Game/Art/Textures/T_ArcadeMuralV18'
if not lib.does_asset_exist(texture):
 t=u.AssetImportTask();t.filename=str(root/'ArtSource/TexturesV18/T_ArcadeMuralV18.png');t.destination_path='/Game/Art/Textures';t.automated=True;t.save=True;tools.import_asset_tasks([t])
for name,color,source in [('ArcadeMuralV18',(1,1,1),texture),('CreatureHideV18',(.45,.32,.20),'/Game/Art/Textures/T_Skin'),('PolicePaintV18',(.30,.44,.55),'/Game/Art/Textures/T_Concrete')]:
 path='/Game/Materials/M_'+name
 if lib.does_asset_exist(path):continue
 m=tools.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew());tex=lib.load_asset(source)
 if not tex:raise RuntimeError('Missing texture '+source)
 sample=ml.create_material_expression(m,u.MaterialExpressionTextureSample);sample.texture=tex
 tint=ml.create_material_expression(m,u.MaterialExpressionConstant3Vector);tint.constant=u.LinearColor(*color,1)
 mul=ml.create_material_expression(m,u.MaterialExpressionMultiply);ml.connect_material_expressions(sample,'RGB',mul,'A');ml.connect_material_expressions(tint,'',mul,'B');ml.connect_material_property(mul,'',u.MaterialProperty.MP_BASE_COLOR)
 rough=ml.create_material_expression(m,u.MaterialExpressionConstant);rough.r=.78;ml.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS);ml.recompile_material(m);lib.save_loaded_asset(m)
exec(compile((root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','ModelsV18').replace('models_v5_manifest','models_v18_manifest'),'import_v18','exec'),{'__name__':'__main__'})
for name in ['ArcadeCabinetV18','PinballV18','AirHockeyV18','PoliceDispatchV18','EvidenceCabinetV18','CellBunkV18','ShopCounterV18','PlazaPlanterV18']:
 mesh=lib.load_asset('/Game/Art/Meshes/SM_'+name);body=mesh.get_editor_property('body_setup');body.set_editor_property('collision_trace_flag',u.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE);lib.save_loaded_asset(mesh)
runpy.run_path(str(root/'Tools/build_audio_v2.py'),run_name='__main__')
cat=lib.load_asset('/Game/Data/DA_ItemCatalog')
if cat:
 rows=list(cat.items)
 for d in u.get_default_object(u.LWItemCatalog).items:
  if str(d.id)=='doublebarrel' and not any(str(x.id)=='doublebarrel' for x in rows):rows.append(d)
 cat.items=rows;lib.save_loaded_asset(cat)
cat=lib.load_asset('/Game/Data/DA_RPGCatalog')
if cat:
 rows=list(cat.get_editor_property("perks"))
 for d in rows:
  if str(d.get_editor_property('id'))=='perk_23':d.set_editor_property('max_rank',max(d.get_editor_property('max_rank'),8))
 cat.set_editor_property("perks",rows);lib.save_loaded_asset(cat)
for path in lib.list_assets('/Game/Data',recursive=True):
 asset=lib.load_asset(path)
 if not isinstance(asset,u.LWLootTable):continue
 rows=list(asset.presets)
 for row in rows:
  if str(row.context) not in ['gas','depot','trader','motel']:continue
  items=list(row.entries)
  if not any(str(x.item_id)=='doublebarrel' for x in items):
   d=u.LWLootEntry();d.item_id='doublebarrel';d.weight=3;d.min_count=d.max_count=1;items.append(d)
  row.entries=items
 asset.presets=rows;lib.save_loaded_asset(asset)
u.log('AAM_V18_CONTENT_COMPLETE')
