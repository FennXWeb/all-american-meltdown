"""V14 content: camper atlas, original meshes, spawn editor and additive attachment loot."""
from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary;tools=u.AssetToolsHelpers.get_asset_tools();ml=u.MaterialEditingLibrary
path='/Game/Art/Textures/T_CamperAtlasV14'
if not lib.does_asset_exist(path):
 t=u.AssetImportTask();t.filename=str(root/'ArtSource/RV/T_CamperAtlasV14.png');t.destination_path='/Game/Art/Textures';t.automated=True;t.save=True;tools.import_asset_tasks([t])
tex=lib.load_asset(path)
if not tex:raise RuntimeError('Camper atlas missing')
for name,cell in [('CamperPearlV14',0),('CamperWalnutV14',1),('CamperLeatherV14',2),('CamperStoneV14',3)]:
 dest='/Game/Materials/M_'+name
 if lib.does_asset_exist(dest):continue
 m=tools.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew())
 uv=ml.create_material_expression(m,u.MaterialExpressionTextureCoordinate);uv.u_tiling=.488;uv.v_tiling=.488
 offset=ml.create_material_expression(m,u.MaterialExpressionConstant2Vector);offset.r=(cell%2)*.5+.006;offset.g=(cell//2)*.5+.006
 add=ml.create_material_expression(m,u.MaterialExpressionAdd);ml.connect_material_expressions(uv,'',add,'A');ml.connect_material_expressions(offset,'',add,'B')
 sample=ml.create_material_expression(m,u.MaterialExpressionTextureSample);sample.texture=tex;ml.connect_material_expressions(add,'',sample,'UVs');ml.connect_material_property(sample,'RGB',u.MaterialProperty.MP_BASE_COLOR)
 rough=ml.create_material_expression(m,u.MaterialExpressionScalarParameter);rough.set_editor_property('parameter_name','Roughness');rough.set_editor_property('default_value',.38 if cell in [0,3] else .7);ml.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS)
 ml.recompile_material(m);lib.save_loaded_asset(m)
if not lib.does_asset_exist('/Game/Materials/M_CamperGlassV14'):
 m=tools.create_asset('M_CamperGlassV14','/Game/Materials',u.Material,u.MaterialFactoryNew());m.set_editor_property('blend_mode',u.BlendMode.BLEND_TRANSLUCENT);m.set_editor_property('two_sided',True)
 color=ml.create_material_expression(m,u.MaterialExpressionConstant3Vector);color.constant=u.LinearColor(.10,.17,.17,1);ml.connect_material_property(color,'',u.MaterialProperty.MP_BASE_COLOR)
 opacity=ml.create_material_expression(m,u.MaterialExpressionConstant);opacity.r=.16;ml.connect_material_property(opacity,'',u.MaterialProperty.MP_OPACITY);ml.recompile_material(m);lib.save_loaded_asset(m)
code=(root/'Tools/import_models_v5.py').read_text(encoding='utf-8').replace("'ModelsV5'","'ModelsV14'").replace('models_v5_manifest.json','models_v14_manifest.json').replace('LW_V5','LW_V14')
exec(compile(code,'import_models_v14.py','exec'),{'__name__':'__main__'})
for name in ['CamperSeatV14','CamperBedV14','CamperKitchenV14','CamperSinkV14','CamperFridgeV14']:
 mesh=lib.load_asset('/Game/Art/Meshes/SM_'+name)
 body=mesh.get_editor_property('body_setup');body.set_editor_property('collision_trace_flag',u.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE);lib.save_loaded_asset(mesh)
if not lib.does_asset_exist('/Game/Data/DA_EnemySpawns'):
 factory=u.DataAssetFactory();factory.set_editor_property('data_asset_class',u.LWSpawnTable)
 a=tools.create_asset('DA_EnemySpawns','/Game/Data',u.LWSpawnTable,factory);lib.save_loaded_asset(a)
# Preserve hand-authored existing rows. Add the new items from native defaults only.
cat=lib.load_asset('/Game/Data/DA_ItemCatalog')
if cat:
 defaults=u.get_default_object(u.LWItemCatalog).get_editor_property('items');items=list(cat.get_editor_property('items'));ids={str(x.id) for x in items}
 for d in defaults:
  if str(d.id).startswith('att_') and str(d.id) not in ids:items.append(d)
 cat.set_editor_property('items',items);lib.save_loaded_asset(cat)
for asset in lib.list_assets('/Game/Data',recursive=True):
 table=lib.load_asset(asset)
 if not isinstance(table,u.LWLootTable):continue
 presets=list(table.get_editor_property('presets'))
 for p in presets:
  if str(p.context) not in ['depot','military','trader']:continue
  rows=list(p.entries);ids={str(x.item_id) for x in rows}
  for name in ['light','laser','reflex','holo','scope4','scope8','vertical','angled']:
   if 'att_'+name in ids:continue
   e=u.LWLootEntry();e.item_id='att_'+name;e.weight=4;e.min_count=1;e.max_count=1;rows.append(e)
  p.entries=rows
 table.presets=presets;lib.save_loaded_asset(table)
u.log('LW_V14_CONTENT_COMPLETE')

