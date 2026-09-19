"""Run inside UnrealEditor-Cmd with -run=pythonscript -script=... .
Imports the authored library, builds editable native material graphs, and saves the launch map.
"""
import unreal as u
from pathlib import Path
ROOT=Path(u.Paths.project_dir()).resolve()
ASSET=u.AssetToolsHelpers.get_asset_tools()
LIB=u.EditorAssetLibrary
ML=u.MaterialEditingLibrary
for folder in ['/Game/Art/Textures','/Game/Art/Meshes','/Game/Audio','/Game/Materials','/Game/Maps']:
    LIB.make_directory(folder)

def import_one(path,destination,mesh=False):
    task=u.AssetImportTask();task.filename=str(path);task.destination_path=destination
    task.automated=True;task.replace_existing=True;task.save=True
    if mesh:
        opts=u.FbxImportUI();opts.import_mesh=True;opts.import_materials=False;opts.import_textures=False
        opts.import_as_skeletal=False;opts.mesh_type_to_import=u.FBXImportType.FBXIT_STATIC_MESH
        opts.automated_import_should_detect_type=False
        opts.static_mesh_import_data.combine_meshes=True
        opts.static_mesh_import_data.auto_generate_collision=True
        opts.static_mesh_import_data.generate_lightmap_u_vs=False
        task.options=opts;task.factory=u.FbxFactory()
    ASSET.import_asset_tasks([task])
    return LIB.load_asset(destination+'/'+path.stem)

textures={}
for path in sorted((ROOT/'ArtSource'/'Textures').glob('*.png')):
    tex=import_one(path,'/Game/Art/Textures');textures[path.stem]=tex
    if not tex: raise RuntimeError('Failed texture '+str(path))
    tex.set_editor_property('filter',u.TextureFilter.TF_NEAREST)
    if path.stem=='T_Font':
        tex.set_editor_property('compression_settings',u.TextureCompressionSettings.TC_EDITOR_ICON)
        tex.set_editor_property('mip_gen_settings',u.TextureMipGenSettings.TMGS_NO_MIPMAPS)
        tex.set_editor_property('lod_group',u.TextureGroup.TEXTUREGROUP_UI)
    elif path.stem.endswith('_N'):
        tex.set_editor_property('compression_settings',u.TextureCompressionSettings.TC_NORMALMAP)
        tex.set_editor_property('srgb',False)
    LIB.save_loaded_asset(tex)

def node(mat,typ,x=0,y=0):return ML.create_material_expression(mat,typ,x,y)
def constant(mat,v,x=0,y=0):
    e=node(mat,u.MaterialExpressionConstant,x,y);e.r=v;return e
def color(mat,v):
    e=node(mat,u.MaterialExpressionConstant3Vector);e.constant=u.LinearColor(*v,1);return e
def connect(a,b,pin=''):
    ML.connect_material_expressions(a,'',b,pin)
def material(name):
    path='/Game/Materials/M_'+name
    existing=LIB.load_asset(path) if LIB.does_asset_exist(path) else None
    if existing:
        ML.delete_all_material_expressions(existing);return existing
    return ASSET.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew())

materials={}
for name in ['Concrete','Brick','Rust','Wood','Asphalt','Earth','Cloth','Skin','Rubber','Red','Steel','Bone']:
    m=material(name);m.set_editor_property('two_sided',False)
    tex=node(m,u.MaterialExpressionTextureSample,-600,0);tex.texture=textures['T_'+name]
    ML.connect_material_property(tex,'RGB',u.MaterialProperty.MP_BASE_COLOR)
    normal=node(m,u.MaterialExpressionTextureSample,-600,240);normal.texture=textures['T_'+name+'_N']
    normal.sampler_type=u.MaterialSamplerType.SAMPLERTYPE_NORMAL
    ML.connect_material_property(normal,'RGB',u.MaterialProperty.MP_NORMAL)
    rough=constant(m,.86 if name not in ['Rust','Steel'] else .69)
    ML.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS)
    metal=constant(m,.35 if name in ['Rust','Steel'] else 0)
    ML.connect_material_property(metal,'',u.MaterialProperty.MP_METALLIC)
    # A small diffuse lift retains low-poly readability without expensive GI.
    glow=node(m,u.MaterialExpressionMultiply);connect(tex,glow,'A');connect(constant(m,.10),glow,'B')
    ML.connect_material_property(glow,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
    # Camera-relative sub-centimetre vertex quantisation: subtle PS1 instability.
    wobble=node(m,u.MaterialExpressionCustom,-80,400);wobble.set_editor_property('output_type',u.CustomMaterialOutputType.CMOT_FLOAT3)
    wobble.set_editor_property('code','float3 p = P-C; float step = clamp(length(p)*0.00007, 0.12, 1.2); return (floor(p/step+0.5)*step-p)*0.65;')
    inputs=[]
    for n in ['P','C']:
        i=u.CustomInput();i.set_editor_property('input_name',n);inputs.append(i)
    wobble.set_editor_property('inputs',inputs)
    connect(node(m,u.MaterialExpressionWorldPosition),wobble,'P')
    connect(node(m,u.MaterialExpressionCameraPositionWS),wobble,'C')
    ML.connect_material_property(wobble,'',u.MaterialProperty.MP_WORLD_POSITION_OFFSET)
    ML.recompile_material(m);LIB.save_loaded_asset(m);materials['M_'+name]=m

for name,col,emissive in [('Glow',(.8,.37,.08),2),('Glass',(.035,.065,.053),.1),('Lane',(.6,.45,.18),.1)]:
    m=material(name);c=color(m,col)
    ML.connect_material_property(c,'',u.MaterialProperty.MP_BASE_COLOR)
    mul=node(m,u.MaterialExpressionMultiply);connect(c,mul,'A');connect(constant(m,emissive),mul,'B')
    ML.connect_material_property(mul,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
    ML.connect_material_property(constant(m,.75),'',u.MaterialProperty.MP_ROUGHNESS)
    ML.recompile_material(m);LIB.save_loaded_asset(m);materials['M_'+name]=m

for name in ['Gas','Motel','Clinic','Depot','Diner','Warning']:
    m=material('Sign'+name);tex=node(m,u.MaterialExpressionTextureSample);tex.texture=textures['T_Sign'+name]
    ML.connect_material_property(tex,'RGB',u.MaterialProperty.MP_BASE_COLOR)
    mul=node(m,u.MaterialExpressionMultiply);connect(tex,mul,'A');connect(constant(m,.14),mul,'B')
    ML.connect_material_property(mul,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
    ML.connect_material_property(constant(m,.95),'',u.MaterialProperty.MP_ROUGHNESS)
    ML.recompile_material(m);LIB.save_loaded_asset(m)

m=material('Sky');m.set_editor_property('two_sided',True)
m.set_editor_property('shading_model',u.MaterialShadingModel.MSM_UNLIT)
c=color(m,(.13,.18,.145));ML.connect_material_property(c,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
ML.recompile_material(m);LIB.save_loaded_asset(m)

m=material('Crust');m.set_editor_property('material_domain',u.MaterialDomain.MD_POST_PROCESS)
# UE 5.8 labels the post-tonemap location SCENE_COLOR_AFTER_TONEMAPPING.
try:m.set_editor_property('blendable_location',u.BlendableLocation.BL_SCENE_COLOR_AFTER_TONEMAPPING)
except AttributeError:m.set_editor_property('blendable_location',u.BlendableLocation.BL_AFTER_TONEMAPPING)
scene=node(m,u.MaterialExpressionSceneTexture,-600,0);scene.set_editor_property('scene_texture_id',u.SceneTextureId.PPI_POST_PROCESS_INPUT0)
uv=node(m,u.MaterialExpressionTextureCoordinate,-600,160)
time=node(m,u.MaterialExpressionTime,-600,320)
strength=node(m,u.MaterialExpressionScalarParameter,-600,480)
strength.set_editor_property('parameter_name','Strength');strength.set_editor_property('default_value',1)
custom=node(m,u.MaterialExpressionCustom,-200,0);custom.set_editor_property('output_type',u.CustomMaterialOutputType.CMOT_FLOAT3)
custom.set_editor_property('code','''
float2 px=floor(UV*float2(640,360));
float bayer=fmod(px.x+px.y*2,4)/4.0;
float grain=frac(sin(dot(px+floor(T*24),float2(12.9898,78.233)))*43758.5453)-0.5;
float3 c=max(Scene.rgb,0);
c=floor(c*31+(bayer-.5)*.65)/31;
c+=grain*.025;
float scan=1-fmod(px.y,2)*.025;
c*=scan;
return lerp(Scene.rgb,saturate(c),Strength);
''')
inputs=[]
for n in ['Scene','UV','T','Strength']:
    i=u.CustomInput();i.set_editor_property('input_name',n);inputs.append(i)
custom.set_editor_property('inputs',inputs)
connect(scene,custom,'Scene');connect(uv,custom,'UV');connect(time,custom,'T');connect(strength,custom,'Strength')
ML.connect_material_property(custom,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
ML.recompile_material(m);LIB.save_loaded_asset(m)

for path in sorted((ROOT/'ArtSource'/'Models').glob('*.fbx')):
    mesh=import_one(path,'/Game/Art/Meshes',True)
    if not mesh:raise RuntimeError('Failed mesh '+str(path))
    for i,slot in enumerate(mesh.get_editor_property('static_materials')):
        name=str(slot.get_editor_property('imported_material_slot_name'))
        if name in materials:mesh.set_material(i,materials[name])
        else:u.log_warning('Unknown material slot '+name+' on '+path.stem)
    LIB.save_loaded_asset(mesh)
    u.log('LW_MESH '+path.stem+' bounds='+str(mesh.get_bounding_box()))

for path in sorted((ROOT/'ArtSource'/'Audio').glob('*.wav')):
    sound=import_one(path,'/Game/Audio')
    if not sound:raise RuntimeError('Failed audio '+str(path))
    if path.stem in ['S_Wind','S_Drone','S_Generator']:sound.set_editor_property('looping',True)
    LIB.save_loaded_asset(sound)

level=u.get_editor_subsystem(u.LevelEditorSubsystem)
if not LIB.does_asset_exist('/Game/Maps/Wasteland'):
    if not level.new_level('/Game/Maps/Wasteland'):raise RuntimeError('Map creation failed')
    level.save_current_level()
LIB.save_directory('/Game',only_if_is_dirty=False,recursive=True)
for asset_path in LIB.list_assets('/Game/Materials',recursive=True,include_folder=False):
    mat=LIB.load_asset(asset_path)
    if mat.get_name() not in ['M_Crust','M_Sky']:
        ML.set_material_usage(mat,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES)
        ML.recompile_material(mat);LIB.save_loaded_asset(mat)
u.log('LW_CONTENT_BUILD_COMPLETE')
exec(compile((ROOT/'Tools'/'prepare_runtime_assets.py').read_text(),str(ROOT/'Tools'/'prepare_runtime_assets.py'),'exec'))
