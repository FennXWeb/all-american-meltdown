import unreal as u
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary;tools=u.AssetToolsHelpers.get_asset_tools();ml=u.MaterialEditingLibrary
for name,col,rough,metal in [('CasinoGold21',(.65,.43,.12),.32,.75),('CasinoVelvet21',(1,1,1),.95,0),('CasinoMarble21',(1,1,1),.38,.05),('Carbon21',(1,1,1),.45,.3),('SuperOrange21',(.95,.24,.015),.2,.5)]:
 source=root/'ArtSource/TexturesV21'/('T_'+name+'.png');tex=None
 if source.exists():
  if not lib.does_asset_exist('/Game/Art/Textures/T_'+name):
   task=u.AssetImportTask();task.filename=str(source);task.destination_path='/Game/Art/Textures';task.automated=True;task.save=True;tools.import_asset_tasks([task])
  tex=lib.load_asset('/Game/Art/Textures/T_'+name)
 path='/Game/Materials/M_'+name
 if lib.does_asset_exist(path):
  if name not in ['CasinoMarble21','CasinoVelvet21']:continue
  m=lib.load_asset(path);ml.delete_all_material_expressions(m)
 else:m=tools.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew())
 m.set_editor_property('used_with_instanced_static_meshes',True);m.set_editor_property('two_sided',True)
 c=ml.create_material_expression(m,u.MaterialExpressionConstant3Vector);c.constant=u.LinearColor(*col,1)
 if tex:
  t=ml.create_material_expression(m,u.MaterialExpressionTextureSample);t.texture=tex
  if name in ['CasinoMarble21','CasinoVelvet21']:
   pos=ml.create_material_expression(m,u.MaterialExpressionWorldPosition);normal=ml.create_material_expression(m,u.MaterialExpressionVertexNormalWS)
   uv=ml.create_material_expression(m,u.MaterialExpressionCustom);uv.set_editor_property('code','float3 a=abs(N);return (a.z>a.x&&a.z>a.y?P.xy:a.x>a.y?P.yz:P.xz)/200;');uv.set_editor_property('output_type',u.CustomMaterialOutputType.CMOT_FLOAT2)
   inputs=[]
   for key in ['P','N']:
    ci=u.CustomInput();ci.set_editor_property('input_name',key);inputs.append(ci)
   uv.set_editor_property('inputs',inputs);ml.connect_material_expressions(pos,'',uv,'P');ml.connect_material_expressions(normal,'',uv,'N');ml.connect_material_expressions(uv,'',t,'UVs')
  mul=ml.create_material_expression(m,u.MaterialExpressionMultiply);ml.connect_material_expressions(t,'RGB',mul,'A');ml.connect_material_expressions(c,'',mul,'B');ml.connect_material_property(mul,'',u.MaterialProperty.MP_BASE_COLOR)
 else:ml.connect_material_property(c,'',u.MaterialProperty.MP_BASE_COLOR)
 for v,prop in [(rough,u.MaterialProperty.MP_ROUGHNESS),(metal,u.MaterialProperty.MP_METALLIC)]:
  n=ml.create_material_expression(m,u.MaterialExpressionConstant);n.r=v;ml.connect_material_property(n,'',prop)
 ml.recompile_material(m);lib.save_loaded_asset(m)
exec(compile((root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','ModelsV21').replace('models_v5_manifest','models_v21_manifest'),'import_v21','exec'),{'__name__':'__main__'})
cat=lib.load_asset('/Game/Data/DA_EnemySpawns')
if cat:
 rows=list(cat.get_editor_property('POIs'))
 if not any(r.get_editor_property('POIType')==21 for r in rows):
  r=u.LWSpawnRow();r.set_editor_property('POIType',21);r.set_editor_property('MinCount',0);r.set_editor_property('MaxCount',0);rows.append(r);cat.set_editor_property('POIs',rows);lib.save_loaded_asset(cat)
u.log('AAM_CASINO21_CONTENT_COMPLETE')
