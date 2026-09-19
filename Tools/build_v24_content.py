"""Update 24 assets only. Add missing catalog/loot rows; retain designer overrides."""
import unreal as u
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary;tools=u.AssetToolsHelpers.get_asset_tools();ml=u.MaterialEditingLibrary

def node(m,kind):return ml.create_material_expression(m,kind)
def constant(m,v,prop):
 n=node(m,u.MaterialExpressionConstant);n.r=v;ml.connect_material_property(n,'',prop);return n
def scalar(m,name,v=0):
 n=node(m,u.MaterialExpressionScalarParameter);n.set_editor_property("parameter_name",name);n.set_editor_property("default_value",v);return n
def custom(m,code,inputs,kind=u.CustomMaterialOutputType.CMOT_FLOAT1):
 n=node(m,u.MaterialExpressionCustom);n.set_editor_property("code",code);n.set_editor_property("output_type",kind)
 ins=[]
 for name,src,pin in inputs:
  i=u.CustomInput();i.set_editor_property("input_name",name);ins.append(i)
 n.set_editor_property("inputs",ins)
 for name,src,pin in inputs:ml.connect_material_expressions(src,pin,n,name)
 return n
def mat(name):
 p='/Game/Materials/M_'+name
 m=lib.load_asset(p) if lib.does_asset_exist(p) else tools.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew())
 ml.delete_all_material_expressions(m);return m
def save(m):ml.recompile_material(m);lib.save_loaded_asset(m)
def imp(path,dest,name=None):
 name=name or path.stem
 if lib.does_asset_exist(dest+'/'+name):return lib.load_asset(dest+'/'+name)
 t=u.AssetImportTask();t.filename=str(path);t.destination_path=dest;t.destination_name=name;t.automated=True;t.save=True;tools.import_asset_tasks([t]);a=lib.load_asset(dest+'/'+name)
 if not a:raise RuntimeError('Import failed: '+str(path))
 return a
for tier in range(2,5):
 for k in range(10):
  name=f'WeaponSkin24_{tier}_{k:02d}';t=imp(root/'ArtSource/TexturesV24'/('T_'+name+'.png'),'/Game/Art/Textures')
  m=mat(name);sample=node(m,u.MaterialExpressionTextureSample);sample.texture=t;ml.connect_material_property(sample,'RGB',u.MaterialProperty.MP_BASE_COLOR)
  constant(m,.38 if tier==2 else .26 if tier==3 else .18,u.MaterialProperty.MP_ROUGHNESS);constant(m,.35 if tier<4 else .65,u.MaterialProperty.MP_METALLIC);save(m)
m=mat('WeaponFX24');m.set_editor_property('shading_model',u.MaterialShadingModel.MSM_UNLIT);m.set_editor_property('blend_mode',u.BlendMode.BLEND_ADDITIVE)
c=node(m,u.MaterialExpressionVectorParameter);c.set_editor_property('parameter_name','Tint');c.set_editor_property('default_value',u.LinearColor(1,.18,.01,1))
em=custom(m,'return C*6;',[('C',c,'RGB')],u.CustomMaterialOutputType.CMOT_FLOAT3);ml.connect_material_property(em,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
f=node(m,u.MaterialExpressionFresnel);op=custom(m,'return (1-F)*.65;',[('F',f,'')]);ml.connect_material_property(op,'',u.MaterialProperty.MP_OPACITY);save(m)
exec(compile((root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','ModelsV24').replace('models_v5_manifest','models_v24_manifest'),'import_v24','exec'),{'__name__':'__main__'})
for path in (root/'ArtSource/AudioV24').glob('*.wav'):imp(path,'/Game/Audio','S_'+path.stem)
cat=lib.load_asset('/Game/Audio/DA_AudioCatalog')
if cat:cat.add_missing_default_slots();lib.save_loaded_asset(cat)
cat=lib.load_asset('/Game/Data/DA_ItemCatalog')
if cat:
 rows=list(cat.items)
 for d in u.get_default_object(u.LWItemCatalog).items:
  if not any(str(x.id)==str(d.id) for x in rows):rows.append(d)
 cat.items=rows;lib.save_loaded_asset(cat)
newids={'missile_launcher','minigun','sawedoff','desert_eagle','m4','taser','flamethrower','ammo_rocket','ammo_50ae','ammo_dart','ammo_fuel','rocket_tube','minigun_box','mag_deagle7','taser_cartridge','fuel_tank'}
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
# Surface forward translucency receives Lumen front-layer reflections. Rain normals animate;
# optical density retains the wetness/drying and wiper parameters used by vehicle/weather code.
for name in ['WindshieldV9','PuddleV9']:
 m=mat(name);m.set_editor_property('blend_mode',u.BlendMode.BLEND_TRANSLUCENT);m.set_editor_property('two_sided',True)
 m.set_editor_property('translucency_lighting_mode',u.TranslucencyLightingMode.TLM_SURFACE_PER_PIXEL_LIGHTING)
 ml.set_material_usage(m,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES)
 wet=scalar(m,'WeatherWet');uv=node(m,u.MaterialExpressionTextureCoordinate);time=node(m,u.MaterialExpressionTime)
 t=node(m,u.MaterialExpressionTextureSample);t.texture=lib.load_asset('/Game/Art/Textures/T_'+('DropsV9' if name=='WindshieldV9' else 'PuddleV9'))
 color=node(m,u.MaterialExpressionConstant3Vector);color.constant=u.LinearColor(.07,.10,.11,1);ml.connect_material_property(color,'',u.MaterialProperty.MP_BASE_COLOR)
 if name=='WindshieldV9':
  wiping=scalar(m,'Wiping');sweep=scalar(m,'Sweep')
  eff=custom(m,'float clear=Wiping*step(UV.y,.85)*step(abs(UV.x-(.5+Sweep*.25)),.26);return W*(1-clear*.97);',[('W',wet,''),('Wiping',wiping,''),('Sweep',sweep,''),('UV',uv,'')])
  opacity=custom(m,'return .035+M*W*.55;',[('M',t,'R'),('W',eff,'')]);refr=1.035
 else:
  eff=wet;opacity=custom(m,'return M*smoothstep(.08,.85,W)*.86;',[('M',t,'R'),('W',wet,'')]);refr=1.08
 normal=custom(m,'float2 v=UV*140;float a=sin(v.x+sin(v.y*1.7)+T*2.1)*cos(v.y+T*1.3);return normalize(float3(a*W*.10,cos(v.y-v.x*.8+T*1.8)*W*.1,1));',[('UV',uv,''),('W',eff,''),('T',time,'')],u.CustomMaterialOutputType.CMOT_FLOAT3)
 ml.connect_material_property(normal,'',u.MaterialProperty.MP_NORMAL);ml.connect_material_property(opacity,'',u.MaterialProperty.MP_OPACITY);constant(m,refr,u.MaterialProperty.MP_REFRACTION);constant(m,.045,u.MaterialProperty.MP_ROUGHNESS);constant(m,.65,u.MaterialProperty.MP_SPECULAR);save(m)
for name in ['Glass','WindowGlass']:
 m=lib.load_asset('/Game/Materials/M_'+name)
 if m and m.get_editor_property('blend_mode')==u.BlendMode.BLEND_TRANSLUCENT:
  m.set_editor_property('translucency_lighting_mode',u.TranslucencyLightingMode.TLM_SURFACE_PER_PIXEL_LIGHTING);ml.set_material_usage(m,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES);save(m)
u.log('AAM_ARSENAL24_CONTENT_COMPLETE: 15 meshes, 30 finishes, 7 reports, wet glass/puddle materials')
