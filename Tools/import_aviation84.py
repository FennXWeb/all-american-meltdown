"""Import original aviation meshes, atlas materials, turbine/warning and VFX material. Never packages."""
import unreal as u,json,math,wave,struct,random
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve();src=root/'ArtSource/Aviation84';data=json.loads((src/'models_aviation84_manifest.json').read_text());lib=u.EditorAssetLibrary;ml=u.MaterialEditingLibrary;at=u.AssetToolsHelpers.get_asset_tools()
t=u.AssetImportTask();t.filename=str(src/'T_AviationAtlas84.png');t.destination_path='/Game/Art/Textures';t.automated=True;t.replace_existing=True;t.save=True;at.import_asset_tasks([t]);atlas=lib.load_asset('/Game/Art/Textures/T_AviationAtlas84')
def node(c):return ml.create_material_expression(mat,c)
def scalar(v):n=node(u.MaterialExpressionConstant);n.r=v;return n
def link(a,p,b,q):
 if not ml.connect_material_expressions(a,p,b,q):raise RuntimeError('Shader input '+q)
def prop(a,p,q):
 if not ml.connect_material_property(a,p,q):raise RuntimeError('Shader output')
for key,color in data['palette'].items():
 name='M_AV84_'+key;path='/Game/Materials/'+name;mat=(lib.load_asset(path) if lib.does_asset_exist(path) else None) or at.create_asset(name,'/Game/Materials',u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(mat)
 c=node(u.MaterialExpressionConstant3Vector);c.constant=u.LinearColor(*color,1);base=c
 if key in ['Leather','Carpet','Pearl','Walnut']:
  quad={'Leather':(0,0),'Carpet':(.5,0),'Pearl':(0,.5),'Walnut':(.5,.5)}[key]
  uv=node(u.MaterialExpressionTextureCoordinate);uv.u_tiling=8;uv.v_tiling=8;frac=node(u.MaterialExpressionFrac);link(uv,'',frac,'');mul=node(u.MaterialExpressionMultiply);link(frac,'',mul,'A');link(scalar(.47),'',mul,'B');off=node(u.MaterialExpressionConstant2Vector);off.r=quad[0]+.015;off.g=quad[1]+.015;add=node(u.MaterialExpressionAdd);link(mul,'',add,'A');link(off,'',add,'B');tex=node(u.MaterialExpressionTextureSample);tex.texture=atlas;link(add,'',tex,'UVs');blend=node(u.MaterialExpressionLinearInterpolate);link(c,'',blend,'A');link(tex,'RGB',blend,'B');link(scalar(.5),'',blend,'Alpha');base=blend
 prop(base,'',u.MaterialProperty.MP_BASE_COLOR);prop(scalar(.13 if key in ['Metal','Glass'] else .4 if key=='Pearl' else .62), '',u.MaterialProperty.MP_ROUGHNESS);prop(scalar(.8 if key in ['Metal','Gold'] else 0),'',u.MaterialProperty.MP_METALLIC)
 if key=='Glass':mat.set_editor_property('blend_mode',u.BlendMode.BLEND_TRANSLUCENT);mat.set_editor_property('translucency_lighting_mode',u.TranslucencyLightingMode.TLM_SURFACE_PER_PIXEL_LIGHTING);prop(scalar(.22),'',u.MaterialProperty.MP_OPACITY)
 if key=='Light':glow=node(u.MaterialExpressionMultiply);link(c,'',glow,'A');link(scalar(3),'',glow,'B');prop(glow,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 mat.set_editor_property('two_sided',True);ml.set_material_usage(mat,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES);ml.recompile_material(mat);lib.save_loaded_asset(mat)
exec(compile((root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','Aviation84').replace('models_v5_manifest','models_aviation84_manifest'),'aviation84import','exec'),{'__name__':'__main__'})
sub=u.get_editor_subsystem(u.StaticMeshEditorSubsystem)
for row in data['assets']:
 mesh=lib.load_asset('/Game/Art/Meshes/SM_'+row['name']);mesh.get_editor_property('body_setup').set_editor_property('collision_trace_flag',u.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE);opts=u.EditorScriptingMeshReductionOptions();opts.auto_compute_lod_screen_size=False;levels=[]
 for ratio,screen in [(1,1),(.6,.14),(.25,.035)]:q=u.EditorScriptingMeshReductionSettings();q.percent_triangles=ratio;q.screen_size=screen;levels.append(q)
 opts.reduction_settings=levels;sub.set_lods(mesh,opts);lib.save_loaded_asset(mesh)
# Phase-continuous turbine layer: harmonics and band-limited broadband bypass,
# not a car engine pitch shifted. Runtime smoothly drives load and spool speed.
cat=lib.load_asset('/Game/Audio/DA_AudioCatalog');slots=dict(cat.slots)
for name,loop in [('AircraftTurbine84',True),('AircraftWarning84',False)]:
 path=src/(name+'.wav');sr=24000;duration=4 if loop else 1.4;rng=random.Random(84);noise=[(rng.randrange(25,3500),rng.random()*math.tau,rng.uniform(.001,.006)) for _ in range(110)];frames=[]
 for i in range(int(sr*duration)):
  t=i/sr
  if loop:y=.10*math.sin(math.tau*125*t)+.035*math.sin(math.tau*1500*t)+sum(math.sin(math.tau*f*t+p)*a for f,p,a in noise)
  else:y=.23*(math.sin(math.tau*880*t)+.2*math.sin(math.tau*1760*t))*max(0,math.sin(math.pi*(t%.7)/.7))**4
  frames.append(struct.pack('<h',int(max(-1,min(1,y))*32767)))
 with wave.open(str(path),'wb') as w:w.setnchannels(1);w.setsampwidth(2);w.setframerate(sr);w.writeframes(b''.join(frames))
 task=u.AssetImportTask();task.filename=str(path);task.destination_path='/Game/Audio/Aviation84';task.destination_name=name;task.automated=True;task.replace_existing=True;task.save=True;at.import_asset_tasks([task]);sound=lib.load_asset('/Game/Audio/Aviation84/'+name);sound.set_editor_property('looping',loop);lib.save_loaded_asset(sound)
 slot=u.LWAudioSlot();slot.source=sound;slot.tracks=[sound];slot.loop=loop;slot.volume=1.;slot.pitch=1.;slot.attenuation=u.LWAudioAttenuation.SPATIAL;slot.description='Original aviation 84 synthesized '+('seamless turbine loop' if loop else 'cockpit master warning')+'; replace Source/Tracks in the audio catalog.';slots[u.Name(name)]=slot
cat.slots=slots;lib.save_loaded_asset(cat)
# Fixed-topology particles use procedural turbulent density instead of flat disks.
name='M_Explosion84';path='/Game/Materials/'+name;mat=lib.load_asset(path) if lib.does_asset_exist(path) else at.create_asset(name,'/Game/Materials',u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(mat);mat.set_editor_property('blend_mode',u.BlendMode.BLEND_TRANSLUCENT);mat.set_editor_property('shading_model',u.MaterialShadingModel.MSM_UNLIT);mat.set_editor_property('two_sided',True)
uv=node(u.MaterialExpressionTextureCoordinate);time=node(u.MaterialExpressionTime);vc=node(u.MaterialExpressionVertexColor);density=node(u.MaterialExpressionCustom);density.set_editor_property('output_type',u.CustomMaterialOutputType.CMOT_FLOAT1)
density.set_editor_property('code','float2 p=UV*2-1; float n=sin(p.x*19+sin(p.y*17+T*2))*sin(p.y*23-sin(p.x*13-T*3)); float b=sin(p.x*39+T*3)*sin(p.y*37-T*2); return saturate((1-length(p))*(1.5+n*.5+b*.22));')
inputs=[]
for n in ['UV','T']:q=u.CustomInput();q.set_editor_property('input_name',n);inputs.append(q)
density.set_editor_property('inputs',inputs);link(uv,'',density,'UV');link(time,'',density,'T');alpha=node(u.MaterialExpressionMultiply);link(density,'',alpha,'A');link(vc,'A',alpha,'B');prop(alpha,'',u.MaterialProperty.MP_OPACITY);prop(vc,'',u.MaterialProperty.MP_EMISSIVE_COLOR);ml.recompile_material(mat);lib.save_loaded_asset(mat)
name='M_FarTerrain84';path='/Game/Materials/'+name;mat=lib.load_asset(path) if lib.does_asset_exist(path) else at.create_asset(name,'/Game/Materials',u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(mat);vc=node(u.MaterialExpressionVertexColor);prop(vc,'',u.MaterialProperty.MP_BASE_COLOR);prop(scalar(1),'',u.MaterialProperty.MP_ROUGHNESS);ml.recompile_material(mat);lib.save_loaded_asset(mat)
name='M_CloudBed84';path='/Game/Materials/'+name;mat=lib.load_asset(path) if lib.does_asset_exist(path) else at.create_asset(name,'/Game/Materials',u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(mat);mat.set_editor_property('blend_mode',u.BlendMode.BLEND_TRANSLUCENT);mat.set_editor_property('two_sided',True)
pos=node(u.MaterialExpressionWorldPosition);time=node(u.MaterialExpressionTime);density=node(u.MaterialExpressionCustom);density.set_editor_property('output_type',u.CustomMaterialOutputType.CMOT_FLOAT1);density.set_editor_property('code','float2 p=P.xy*.000008+float2(T*.004,T*.002); float n=0,amp=.55; for(int k=0;k<5;k++){float2 i=floor(p),f=frac(p);f=f*f*(3-2*f);float a=frac(sin(dot(i,float2(127.1,311.7)))*43758.5453);float b=frac(sin(dot(i+float2(1,0),float2(127.1,311.7)))*43758.5453);float c=frac(sin(dot(i+float2(0,1),float2(127.1,311.7)))*43758.5453);float d=frac(sin(dot(i+1,float2(127.1,311.7)))*43758.5453);n+=lerp(lerp(a,b,f.x),lerp(c,d,f.x),f.y)*amp;p=p*2.03+19; amp*=.5;} return saturate((n-.26)*2.7);')
inputs=[]
for n in ['P','T']:q=u.CustomInput();q.set_editor_property('input_name',n);inputs.append(q)
density.set_editor_property('inputs',inputs);link(pos,'',density,'P');link(time,'',density,'T');prop(density,'',u.MaterialProperty.MP_OPACITY);color=node(u.MaterialExpressionConstant3Vector);color.constant=u.LinearColor(.7,.75,.8,1);prop(color,'',u.MaterialProperty.MP_BASE_COLOR);prop(scalar(1),'',u.MaterialProperty.MP_ROUGHNESS);ml.recompile_material(mat);lib.save_loaded_asset(mat)
# Altitude-aware atmosphere. Camera-facing direction keeps the darker zenith and
# faint stars stable as the player walks around a banked aircraft.
name='M_AviationSky84';path='/Game/Materials/'+name;mat=lib.load_asset(path) if lib.does_asset_exist(path) else at.create_asset(name,'/Game/Materials',u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(mat);mat.set_editor_property('shading_model',u.MaterialShadingModel.MSM_UNLIT);mat.set_editor_property('two_sided',True)
c=node(u.MaterialExpressionVectorParameter);c.set_editor_property('parameter_name','SkyColor');c.set_editor_property('default_value',u.LinearColor(.29,.38,.46,1))
a=node(u.MaterialExpressionScalarParameter);a.set_editor_property('parameter_name','ThinAir84');a.set_editor_property('default_value',0)
v=node(u.MaterialExpressionCameraVectorWS);sky=node(u.MaterialExpressionCustom);sky.set_editor_property('output_type',u.CustomMaterialOutputType.CMOT_FLOAT3)
sky.set_editor_property('code','float h=saturate(-V.z); float3 upper=lerp(float3(.055,.085,.15),float3(.0005,.001,.004),pow(h,.42)); float2 star=floor(V.xy/max(.05,abs(V.z))*440); float hash=frac(sin(dot(star,float2(127.1,311.7)))*43758.5453); float stars=step(.9995,hash)*smoothstep(.25,.75,h)*.12; return lerp(C,upper+stars,A);')
inputs=[]
for n in ['C','A','V']:q=u.CustomInput();q.set_editor_property('input_name',n);inputs.append(q)
sky.set_editor_property('inputs',inputs);link(c,'',sky,'C');link(a,'',sky,'A');link(v,'',sky,'V');prop(sky,'',u.MaterialProperty.MP_EMISSIVE_COLOR);ml.recompile_material(mat);lib.save_loaded_asset(mat)
u.log('AVIATION84_IMPORT_PASS '+str(len(data['assets'])));u.SystemLibrary.quit_editor()
