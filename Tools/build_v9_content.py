"""Import original 0.9 assets and weather materials; never cook or package."""
from pathlib import Path
import runpy
import unreal as u
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary;ml=u.MaterialEditingLibrary;assets=u.AssetToolsHelpers.get_asset_tools()
for p in (root/'ArtSource/TexturesV9').glob('*.png'):
 task=u.AssetImportTask();task.filename=str(p);task.destination_path='/Game/Art/Textures';task.automated=True;task.replace_existing=True;task.save=True;assets.import_asset_tasks([task]);t=lib.load_asset('/Game/Art/Textures/'+p.stem);t.set_editor_property('filter',u.TextureFilter.TF_NEAREST)
 if p.stem.endswith('_N'):t.set_editor_property('compression_settings',u.TextureCompressionSettings.TC_NORMALMAP);t.set_editor_property('srgb',False)
 lib.save_loaded_asset(t)
def node(m,cls):return ml.create_material_expression(m,cls)
def scalar(m,name,value=0):
 n=node(m,u.MaterialExpressionScalarParameter);n.set_editor_property('parameter_name',name);n.set_editor_property('default_value',value);return n
def custom(m,code,inputs,kind=u.CustomMaterialOutputType.CMOT_FLOAT1):
 c=node(m,u.MaterialExpressionCustom);c.set_editor_property('code',code);c.set_editor_property('output_type',kind);ins=[]
 for name,source,pin in inputs:
  i=u.CustomInput();i.set_editor_property('input_name',name);ins.append(i)
 c.set_editor_property('inputs',ins)
 for name,source,pin in inputs:ml.connect_material_expressions(source,pin,c,name)
 return c
def material(name):
 path='/Game/Materials/M_'+name;m=lib.load_asset(path) if lib.does_asset_exist(path) else assets.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(m);return m
def constant(m,value,prop):n=node(m,u.MaterialExpressionConstant);n.r=value;ml.connect_material_property(n,'',prop)
def texture(m,name,uv=None):
 n=node(m,u.MaterialExpressionTextureSample);n.texture=lib.load_asset('/Game/Art/Textures/T_'+name)
 if uv:ml.connect_material_expressions(uv,'',n,'UVs')
 return n
def finish(m):ml.set_material_usage(m,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES);ml.recompile_material(m);lib.save_loaded_asset(m)
for name in ['BoutiqueStoneV9','BoutiqueWallV9','VelvetV9','BoutiqueSignV9','WetAsphaltV9','WetConcreteV9','WetEarthV9','VehiclePaintV9']:
 m=material(name);p=node(m,u.MaterialExpressionWorldPosition);n=node(m,u.MaterialExpressionVertexNormalWS);wet=scalar(m,'WeatherWet')
 uv=None
 if name!='BoutiqueSignV9':uv=custom(m,'float3 a=abs(N);return (a.z>a.x&&a.z>a.y?P.xy:a.x>a.y?P.yz:P.xz)/200;', [('P',p,''),('N',n,'')],u.CustomMaterialOutputType.CMOT_FLOAT2)
 source=name[3:-2] if name.startswith('Wet') else name;tex=texture(m,source,uv)
 effective=custom(m,'return W*saturate(N.z)*step(0,P.z);' if name.startswith('Wet') else 'return W;', [('W',wet,''),('N',n,''),('P',p,'')])
 color=custom(m,'return C*lerp(1,.55,W);',[('C',tex,'RGB'),('W',effective,'')],u.CustomMaterialOutputType.CMOT_FLOAT3)
 if name=='VehiclePaintV9':
  tint=node(m,u.MaterialExpressionVectorParameter);tint.set_editor_property('parameter_name','PaintTint');tint.set_editor_property('default_value',u.LinearColor(.35,.4,.4,1));color=custom(m,'return C*T;',[('C',color,''),('T',tint,'RGB')],u.CustomMaterialOutputType.CMOT_FLOAT3)
 ml.connect_material_property(color,'',u.MaterialProperty.MP_BASE_COLOR)
 rough=custom(m,'return lerp(.72,.09,W);',[('W',effective,'')]);ml.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS)
 normal_path='/Game/Art/Textures/T_'+source+'_N'
 if lib.does_asset_exist(normal_path):
  normal=texture(m,source+'_N',uv);normal.set_editor_property('sampler_type',u.MaterialSamplerType.SAMPLERTYPE_NORMAL);ml.connect_material_property(normal,'RGB',u.MaterialProperty.MP_NORMAL)
 finish(m)
for name in ['WindshieldV9','PuddleV9']:
 m=material(name);m.set_editor_property('blend_mode',u.BlendMode.BLEND_TRANSLUCENT);m.set_editor_property('two_sided',True)
 wet=scalar(m,'WeatherWet');t=texture(m,'DropsV9' if name=='WindshieldV9' else 'PuddleV9');uv=node(m,u.MaterialExpressionTextureCoordinate)
 c=node(m,u.MaterialExpressionConstant3Vector);c.constant=u.LinearColor(.12,.16,.17,1);ml.connect_material_property(c,'',u.MaterialProperty.MP_BASE_COLOR)
 if name=='WindshieldV9':
  wiping=scalar(m,'Wiping');sweep=scalar(m,'Sweep');opacity=custom(m,'float clear=Wiping*step(UV.y,.82)*step(abs(UV.x-.5),.48);return .015+M*W*.78*(1-clear*.97);',[('M',t,'R'),('W',wet,''),('Wiping',wiping,''),('UV',uv,''),('Sweep',sweep,'')]);constant(m,1.018,u.MaterialProperty.MP_REFRACTION)
 else:opacity=custom(m,'return M*smoothstep(.08,.85,W)*.82;',[('M',t,'R'),('W',wet,'')])
 ml.connect_material_property(opacity,'',u.MaterialProperty.MP_OPACITY);constant(m,.07,u.MaterialProperty.MP_ROUGHNESS);constant(m,.8,u.MaterialProperty.MP_SPECULAR);finish(m)
# Reuse the existing strict FBX axis, unit, pivot and material validation.
code=(root/'Tools/import_models_v5.py').read_text().replace("'ModelsV5'","'ModelsV9'").replace('models_v5_manifest.json','models_v9_manifest.json').replace('LW_V5','LW_V9')
exec(compile(code,'import_models_v9.py','exec'),{'__name__':'__main__'})
for name in ['ClothesRackV9','BoutiqueBenchV9','DisplayPlinthV9']:
 m=lib.load_asset('/Game/Art/Meshes/SM_'+name)
 for i,slot in enumerate(m.get_editor_property('static_materials')):
  source=str(slot.get_editor_property('imported_material_slot_name'))
  if source=='M_Cloth':m.set_material(i,lib.load_asset('/Game/Materials/M_VelvetV9'))
  if name=='DisplayPlinthV9' and source=='M_Bone':m.set_material(i,lib.load_asset('/Game/Materials/M_BoutiqueStoneV9'))
 lib.save_loaded_asset(m)
u.log('LW_V9_CONTENT_COMPLETE')
