"""Import editor assets only. Never cook or package."""
from pathlib import Path
import runpy
import unreal as u
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary;tools=u.AssetToolsHelpers.get_asset_tools();ml=u.MaterialEditingLibrary
for p in sorted((root/'ArtSource'/'TexturesV7').glob('*.png')):
 dest='/Game/Art/Textures/'+p.stem
 if lib.does_asset_exist(dest):continue
 task=u.AssetImportTask();task.filename=str(p);task.destination_path='/Game/Art/Textures';task.automated=True;task.save=True;tools.import_asset_tasks([task]);tex=lib.load_asset(dest);tex.set_editor_property('filter',u.TextureFilter.TF_NEAREST)
 if p.stem.endswith('_N'):tex.set_editor_property('compression_settings',u.TextureCompressionSettings.TC_NORMALMAP);tex.set_editor_property('srgb',False)
 lib.save_loaded_asset(tex)
for p in sorted((root/'ArtSource'/'TexturesV7').glob('*.png')):
 if p.stem.endswith('_N'):continue
 name=p.stem[2:];dest='/Game/Materials/M_'+name
 if lib.does_asset_exist(dest):
  m=lib.load_asset(dest)
  if lib.get_metadata_tag(m,'LW_V7_WORLD_UV')=='1':continue
  ml.delete_all_material_expressions(m)
 else:m=tools.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew())
 tex=ml.create_material_expression(m,u.MaterialExpressionTextureSample,-400,0);tex.texture=lib.load_asset('/Game/Art/Textures/'+p.stem);ml.connect_material_property(tex,'RGB',u.MaterialProperty.MP_BASE_COLOR)
 norm=ml.create_material_expression(m,u.MaterialExpressionTextureSample,-400,200);norm.texture=lib.load_asset('/Game/Art/Textures/'+p.stem+'_N');norm.sampler_type=u.MaterialSamplerType.SAMPLERTYPE_NORMAL;ml.connect_material_property(norm,'RGB',u.MaterialProperty.MP_NORMAL)
 if name!='PosterV7':
  uv=ml.create_material_expression(m,u.MaterialExpressionCustom,-650,0);uv.set_editor_property('output_type',u.CustomMaterialOutputType.CMOT_FLOAT2);uv.set_editor_property('code','float3 n=abs(N); return (n.z>n.x && n.z>n.y ? P.xy : n.x>n.y ? P.yz : P.xz)/200.0;')
  ins=[]
  for key in ['P','N']:
   inp=u.CustomInput();inp.set_editor_property('input_name',key);ins.append(inp)
  uv.set_editor_property('inputs',ins)
  pos=ml.create_material_expression(m,u.MaterialExpressionWorldPosition,-900,0);normal=ml.create_material_expression(m,u.MaterialExpressionVertexNormalWS,-900,150)
  ml.connect_material_expressions(pos,'',uv,'P');ml.connect_material_expressions(normal,'',uv,'N');ml.connect_material_expressions(uv,'',tex,'UVs');ml.connect_material_expressions(uv,'',norm,'UVs')
 rough=ml.create_material_expression(m,u.MaterialExpressionConstant,0,0);rough.r=.85;ml.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS)
 glow=ml.create_material_expression(m,u.MaterialExpressionMultiply,0,150);gain=ml.create_material_expression(m,u.MaterialExpressionConstant,0,250);gain.r=.08;ml.connect_material_expressions(gain,'',glow,'B');ml.connect_material_expressions(tex,'RGB',glow,'A');ml.connect_material_property(glow,'',u.MaterialProperty.MP_EMISSIVE_COLOR);ml.set_material_usage(m,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES);ml.recompile_material(m);lib.set_metadata_tag(m,'LW_V7_WORLD_UV','1');lib.save_loaded_asset(m)
runpy.run_path(str(root/'Tools'/'build_audio_v2.py'),run_name='__main__')
u.log('LW_V7_CONTENT_COMPLETE')
