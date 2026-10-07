"""Import EV74 art and reusable synthesized electric powertrain loops. No packaging."""
import unreal as u,json,math,wave,struct
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve();src=root/'ArtSource/Electric74';data=json.loads((src/'models_electric74_manifest.json').read_text());lib=u.EditorAssetLibrary;ml=u.MaterialEditingLibrary;at=u.AssetToolsHelpers.get_asset_tools()
t=u.AssetImportTask();t.filename=str(src/'T_ElectricAtlas74.png');t.destination_path='/Game/Art/Textures';t.automated=True;t.replace_existing=True;t.save=True;at.import_asset_tasks([t]);atlas=lib.load_asset('/Game/Art/Textures/T_ElectricAtlas74')
for key,color in data['palette'].items():
 name='M_EV74_'+key;path='/Game/Materials/'+name;mat=(lib.load_asset(path) if lib.does_asset_exist(path) else None) or at.create_asset(name,'/Game/Materials',u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(mat)
 def node(c):return ml.create_material_expression(mat,c)
 def scalar(v):n=node(u.MaterialExpressionConstant);n.r=v;return n
 def link(a,p,b,q):
  if not ml.connect_material_expressions(a,p,b,q):raise RuntimeError('Bad shader link '+q)
 def prop(a,p,q):
  if not ml.connect_material_property(a,p,q):raise RuntimeError('Bad shader property')
 c=node(u.MaterialExpressionConstant3Vector);c.constant=u.LinearColor(*color,1);base=c
 if key in ['Solar','Leather','Titanium','Carbon']:
  quad={'Solar':(0,0),'Leather':(.5,0),'Titanium':(0,.5),'Carbon':(.5,.5)}[key]
  uv=node(u.MaterialExpressionTextureCoordinate);uv.set_editor_property('u_tiling',16 if key=='Leather' else 6 if key=='Carbon' else 3 if key=='Titanium' else 1);uv.set_editor_property('v_tiling',16 if key=='Leather' else 6 if key=='Carbon' else 3 if key=='Titanium' else 1);frac=node(u.MaterialExpressionFrac);link(uv,'',frac,'');mul=node(u.MaterialExpressionMultiply);link(frac,'',mul,'A');link(scalar(.48),'',mul,'B')
  off=node(u.MaterialExpressionConstant2Vector);off.r=quad[0]+.01;off.g=quad[1]+.01;add=node(u.MaterialExpressionAdd);link(mul,'',add,'A');link(off,'',add,'B');tex=node(u.MaterialExpressionTextureSample);tex.texture=atlas;link(add,'',tex,'UVs');base=tex
  if key in ['Leather','Carbon','Titanium']:
   blend=node(u.MaterialExpressionLinearInterpolate);link(c,'',blend,'A');link(tex,'RGB',blend,'B');link(scalar(.30 if key=='Leather' else .35),'',blend,'Alpha');base=blend
 if key=='Display':
  base=node(u.MaterialExpressionTextureSampleParameter2D);base.set_editor_property('parameter_name','DisplayTexture');base.texture=atlas
  mat.set_editor_property('shading_model',u.MaterialShadingModel.MSM_UNLIT);prop(base,'RGB',u.MaterialProperty.MP_EMISSIVE_COLOR)
 else:
  prop(base,'RGB' if key=='Solar' else '',u.MaterialProperty.MP_BASE_COLOR)
  rough=node(u.MaterialExpressionScalarParameter);rough.set_editor_property('parameter_name','Roughness');rough.set_editor_property('default_value',.22 if key in ['Titanium','Pearl','Carbon','Solar'] else .68);prop(rough,'',u.MaterialProperty.MP_ROUGHNESS)
  prop(scalar(.82 if key=='Titanium' else .5 if key in ['Pearl','Carbon','Solar'] else 0),'',u.MaterialProperty.MP_METALLIC)
  if key in ['Light','Red']:glow=node(u.MaterialExpressionMultiply);link(c,'',glow,'A');link(scalar(3.5),'',glow,'B');prop(glow,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 mat.set_editor_property('two_sided',True);ml.set_material_usage(mat,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES);ml.recompile_material(mat);lib.save_loaded_asset(mat)
# Tinted safety glass remains visible in clear weather and retains the game's
# water accumulation/wiper controls. It is not a dry, almost-invisible overlay.
path='/Game/Materials/M_EV74_Window';mat=lib.load_asset(path) if lib.does_asset_exist(path) else at.create_asset('M_EV74_Window','/Game/Materials',u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(mat)
mat.set_editor_property('blend_mode',u.BlendMode.BLEND_TRANSLUCENT);mat.set_editor_property('two_sided',True);mat.set_editor_property('translucency_lighting_mode',u.TranslucencyLightingMode.TLM_SURFACE_PER_PIXEL_LIGHTING)
c=node(u.MaterialExpressionConstant3Vector);c.constant=u.LinearColor(.028,.058,.065,1);prop(c,'',u.MaterialProperty.MP_BASE_COLOR)
uv=node(u.MaterialExpressionTextureCoordinate);drop=node(u.MaterialExpressionTextureSample);drop.texture=lib.load_asset('/Game/Art/Textures/T_DropsV9')
custom=node(u.MaterialExpressionCustom);custom.set_editor_property('code','float clear=Wiping*step(UV.y,.85)*step(abs(UV.x-(.5+Sweep*.25)),.26); return .20+Drops*WeatherWet*(1-clear*.97)*.5;');custom.set_editor_property('output_type',u.CustomMaterialOutputType.CMOT_FLOAT1)
inputs=[];sources=[('UV',uv,''),('Drops',drop,'R')]
for name in ['WeatherWet','Wiping','Sweep']:
 q=node(u.MaterialExpressionScalarParameter);q.set_editor_property('parameter_name',name);q.set_editor_property('default_value',0);sources.append((name,q,''))
for name,_,_ in sources:q=u.CustomInput();q.set_editor_property('input_name',name);inputs.append(q)
custom.set_editor_property('inputs',inputs)
for name,srcnode,pin in sources:link(srcnode,pin,custom,name)
prop(custom,'',u.MaterialProperty.MP_OPACITY);prop(scalar(.08),'',u.MaterialProperty.MP_ROUGHNESS);prop(scalar(.75),'',u.MaterialProperty.MP_SPECULAR);ml.recompile_material(mat);lib.save_loaded_asset(mat)
exec(compile((root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','Electric74').replace('models_v5_manifest','models_electric74_manifest'),'electric74import','exec'),{'__name__':'__main__'})
sub=u.get_editor_subsystem(u.StaticMeshEditorSubsystem)
for row in data['assets']:
 mesh=lib.load_asset('/Game/Art/Meshes/SM_'+row['name']);mesh.set_editor_property('allow_cpu_access',True);mesh.get_editor_property('body_setup').set_editor_property('collision_trace_flag',u.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE)
 opts=u.EditorScriptingMeshReductionOptions();opts.auto_compute_lod_screen_size=False;levels=[]
 for ratio,screen in [(1,1),(.55,.22),(.22,.075)]:q=u.EditorScriptingMeshReductionSettings();q.percent_triangles=ratio;q.screen_size=screen;levels.append(q)
 opts.reduction_settings=levels;sub.set_lods(mesh,opts);lib.save_loaded_asset(mesh)
# Integer-frequency harmonics meet exactly at the 4-second loop seam. No piston
# loops or random pitch shifts: runtime load and speed continuously drive gain/pitch.
cat=lib.load_asset('/Game/Audio/DA_AudioCatalog');slots=dict(cat.get_editor_property('slots'))
for name,base,loop in [('EV74_CoachMotor',70,True),('EV74_HyperMotor',110,True),('EV74_Ready',440,False)]:
 path=src/(name+'.wav');sr=48000;duration=4 if loop else .7;frames=[]
 for i in range(int(sr*duration)):
  t=i/sr
  if loop:y=.26*math.sin(2*math.pi*base*t)+.085*math.sin(2*math.pi*base*3*t)+.045*math.sin(2*math.pi*(base*8+2)*t)*(1+.2*math.sin(2*math.pi*2*t))
  else:y=.20*(math.sin(2*math.pi*base*t)+.5*math.sin(2*math.pi*base*1.5*t))*math.sin(math.pi*t/duration)**2
  frames.append(struct.pack('<h',int(max(-1,min(1,y))*32767)))
 with wave.open(str(path),'wb') as w:w.setnchannels(1);w.setsampwidth(2);w.setframerate(sr);w.writeframes(b''.join(frames))
 task=u.AssetImportTask();task.filename=str(path);task.destination_path='/Game/Audio/Electric74';task.destination_name=name;task.automated=True;task.replace_existing=True;task.save=True;at.import_asset_tasks([task]);sound=lib.load_asset('/Game/Audio/Electric74/'+name);sound.set_editor_property('looping',loop);lib.save_loaded_asset(sound)
 slot=u.LWAudioSlot();slot.set_editor_property('source',sound);slot.set_editor_property('tracks',[sound]);slot.set_editor_property('loop',loop);slot.set_editor_property('volume',.85);slot.set_editor_property('pitch',1.);slot.set_editor_property('attenuation',u.LWAudioAttenuation.SPATIAL);slot.set_editor_property('description','Original synthesized electric powertrain 74; phase-periodic continuous loop, speed/load controlled in runtime. Replace Source or Tracks to customize.');slots[u.Name(name)]=slot
cat.set_editor_property('slots',slots);lib.save_loaded_asset(cat)
u.log('ELECTRIC74_IMPORT_PASS '+str(len(data['assets'])));u.SystemLibrary.quit_editor()
