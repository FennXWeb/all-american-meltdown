"""Full editor import, physical-scale validation, LODs and query collision."""
import unreal as u,json
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve();src=root/'ArtSource/RV66';data=json.loads((src/'models_rv66_manifest.json').read_text());lib=u.EditorAssetLibrary;ml=u.MaterialEditingLibrary;at=u.AssetToolsHelpers.get_asset_tools();sub=u.get_editor_subsystem(u.StaticMeshEditorSubsystem)
def connect(a,output,b,pin):
 if not ml.connect_material_expressions(a,output,b,pin):raise RuntimeError('Material connection failed: '+str(a.get_class().get_name())+' -> '+str(b.get_class().get_name())+' / '+pin)
 return True
def connect_property(a,output,prop):
 if not ml.connect_material_property(a,output,prop):raise RuntimeError('Material output connection failed: '+str(prop))
 return True
for key,color in data['palette'].items():
 name='M_RV66_'+key;path='/Game/Materials/'+name
 mat=(lib.load_asset(path) if lib.does_asset_exist(path) else None) or at.create_asset(name,'/Game/Materials',u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(mat)
 def node(cls):return ml.create_material_expression(mat,cls)
 def scalar(v):n=node(u.MaterialExpressionConstant);n.r=v;return n
 c=node(u.MaterialExpressionConstant3Vector);c.constant=u.LinearColor(*color,1)
 texname={'Walnut':'Wood','Ivory':'Leather','Linen':'Fabric','Pearl':'Metal','Quartz':'Vinyl'}.get(key)
 if texname:
  tex=node(u.MaterialExpressionTextureSample);tex.texture=lib.load_asset('/Game/Art/Textures/T_V42_'+texname)
  des=node(u.MaterialExpressionDesaturation)
  if not connect(tex,'RGB',des,''):raise RuntimeError('Unable to connect RV surface texture to desaturation')
  add=node(u.MaterialExpressionAdd);connect(des,'',add,'A');connect(scalar(.55),'',add,'B')
  mult=node(u.MaterialExpressionMultiply);connect(add,'',mult,'A');connect(c,'',mult,'B');base=mult
 else:base=c
 connect_property(base,'',u.MaterialProperty.MP_BASE_COLOR)
 rough=node(u.MaterialExpressionScalarParameter);rough.set_editor_property('parameter_name','Roughness');rough.set_editor_property('default_value',.20 if key in ['Chrome','Brass','Quartz'] else .34 if key=='Pearl' else .78);connect_property(rough,'',u.MaterialProperty.MP_ROUGHNESS)
 connect_property(scalar(.8 if key in ['Chrome','Brass'] else .15 if key=='Pearl' else 0),'',u.MaterialProperty.MP_METALLIC)
 if key in ['Water','Screen','Light']:
  mult=node(u.MaterialExpressionMultiply);connect(c,'',mult,'A');connect(scalar(1.6 if key=='Light' else .7),'',mult,'B');connect_property(mult,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 if key=='Screen':
  uv=node(u.MaterialExpressionTextureCoordinate);pan=node(u.MaterialExpressionPanner);pan.set_editor_property('speed_y',.18);connect(uv,'',pan,'Coordinate')
  tex=node(u.MaterialExpressionTextureSample);tex.texture=lib.load_asset('/Game/Art/Textures/T_V42_Fabric');connect(pan,'',tex,'UVs')
  connect(tex,'RGB',mult,'B')
  power=node(u.MaterialExpressionScalarParameter);power.set_editor_property('parameter_name','Power');power.set_editor_property('default_value',0)
  on=node(u.MaterialExpressionMultiply);connect(mult,'',on,'A');connect(power,'',on,'B');connect_property(on,'',u.MaterialProperty.MP_EMISSIVE_COLOR);connect_property(on,'',u.MaterialProperty.MP_BASE_COLOR)
  live=node(u.MaterialExpressionScalarParameter);live.set_editor_property('parameter_name','Live');live.set_editor_property('default_value',0)
  view=node(u.MaterialExpressionTextureSampleParameter2D);view.set_editor_property('parameter_name','SceneView');view.texture=lib.load_asset('/Game/Art/Textures/T_V42_Fabric')
  mix=node(u.MaterialExpressionLinearInterpolate);connect(mult,'',mix,'A');connect(view,'RGB',mix,'B');connect(live,'',mix,'Alpha');connect(mix,'',on,'A')
 mat.set_editor_property('two_sided',True);ml.set_material_usage(mat,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES);ml.recompile_material(mat);lib.save_loaded_asset(mat)
exec(compile((root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','RV66').replace('models_v5_manifest','models_rv66_manifest'),'rv66import','exec'),{'__name__':'__main__'})
for r in data['assets']:
 mesh=lib.load_asset('/Game/Art/Meshes/SM_'+r['name']);mesh.set_editor_property('allow_cpu_access',True)
 bs=mesh.get_editor_property('body_setup');bs.set_editor_property('collision_trace_flag',u.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE)
 opts=u.EditorScriptingMeshReductionOptions();opts.auto_compute_lod_screen_size=False;levels=[]
 for ratio,screen in [(1,1),(.5,.25),(.2,.09)]:q=u.EditorScriptingMeshReductionSettings();q.percent_triangles=ratio;q.screen_size=screen;levels.append(q)
 opts.reduction_settings=levels;sub.set_lods(mesh,opts);lib.save_loaded_asset(mesh)
u.log('RV66_IMPORT_PASS '+str(len(data['assets'])));u.SystemLibrary.quit_editor()
