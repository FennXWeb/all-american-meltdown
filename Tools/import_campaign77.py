"""Import fitted production meshes, check measured bounds and build PBR materials."""
import unreal as u, json
from pathlib import Path
root=Path('X:/LethalWorld');src=root/'ArtSource/Campaign77';dest='/Game/Art/Campaign77'
data=json.loads((src/'manifest.json').read_text());lib=u.EditorAssetLibrary;ml=u.MaterialEditingLibrary;at=u.AssetToolsHelpers.get_asset_tools();lib.make_directory(dest)
textures={}
for name in ['T_Campaign77','T_SurfaceNormal77','T_SurfaceORM77']:
 t=u.AssetImportTask();t.filename=str(src/(name+'.png'));t.destination_path=dest;t.automated=True;t.save=True;t.replace_existing=True;at.import_asset_tasks([t]);tex=lib.load_asset(dest+'/'+name)
 if name.endswith('Normal77'):tex.set_editor_property('compression_settings',u.TextureCompressionSettings.TC_NORMALMAP);tex.set_editor_property('srgb',False)
 elif name.endswith('ORM77'):tex.set_editor_property('srgb',False);tex.set_editor_property('compression_settings',u.TextureCompressionSettings.TC_MASKS)
 lib.save_loaded_asset(tex);textures[name]=tex
mats={}
for key in data['materials']:
 name='M_'+key+'77';m=lib.load_asset(dest+'/'+name) if lib.does_asset_exist(dest+'/'+name) else at.create_asset(name,dest,u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(m)
 for texname,channels,props,sampler in [('T_Campaign77',['RGB'],[u.MaterialProperty.MP_BASE_COLOR],u.MaterialSamplerType.SAMPLERTYPE_COLOR),('T_SurfaceNormal77',['RGB'],[u.MaterialProperty.MP_NORMAL],u.MaterialSamplerType.SAMPLERTYPE_NORMAL),('T_SurfaceORM77',['R','G','B'],[u.MaterialProperty.MP_AMBIENT_OCCLUSION,u.MaterialProperty.MP_ROUGHNESS,u.MaterialProperty.MP_METALLIC],u.MaterialSamplerType.SAMPLERTYPE_MASKS)]:
  n=ml.create_material_expression(m,u.MaterialExpressionTextureSample);n.texture=textures[texname];n.sampler_type=sampler
  for ch,prop in zip(channels,props):ml.connect_material_property(n,ch,prop)
 m.set_editor_property('two_sided',key=='Cloth');ml.set_material_usage(m,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES);ml.recompile_material(m);lib.save_loaded_asset(m);mats[name]=m
sub=u.get_editor_subsystem(u.StaticMeshEditorSubsystem);errors=[]
for r in data['assets']:
 t=u.AssetImportTask();t.filename=str(src/(r['name']+'.fbx'));t.destination_path=dest;t.destination_name='SM_'+r['name'];t.automated=True;t.replace_existing=True;t.replace_existing_settings=True;t.save=False
 opts=u.FbxImportUI();opts.import_mesh=True;opts.import_materials=False;opts.import_textures=False;opts.import_as_skeletal=False;opts.mesh_type_to_import=u.FBXImportType.FBXIT_STATIC_MESH;opts.automated_import_should_detect_type=False
 d=opts.static_mesh_import_data;d.combine_meshes=True;d.auto_generate_collision=False;d.generate_lightmap_u_vs=False;d.convert_scene=True;d.convert_scene_unit=True;d.normal_import_method=u.FBXNormalImportMethod.FBXNIM_IMPORT_NORMALS;t.options=opts;t.factory=u.FbxFactory();at.import_asset_tasks([t]);mesh=lib.load_asset(dest+'/SM_'+r['name'])
 if not mesh:errors.append(r['name']);continue
 for j,s in enumerate(mesh.get_editor_property('static_materials')):mesh.set_material(j,mats[str(s.get_editor_property('imported_material_slot_name'))])
 mesh.get_editor_property('body_setup').set_editor_property('collision_trace_flag',u.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE)
 b=mesh.get_bounding_box();actual=[b.min.x,b.min.y,b.min.z,b.max.x,b.max.y,b.max.z];expected=[v*100 for v in r['bounds']['min']+r['bounds']['max']]
 if max(abs(a-b) for a,b in zip(actual,expected))>.2:errors.append(r['name']+' invalid scale')
 opt=u.EditorScriptingMeshReductionOptions();opt.auto_compute_lod_screen_size=False;levels=[]
 for ratio,screen in [(1,1),(.55,.3),(.2,.1)]:q=u.EditorScriptingMeshReductionSettings();q.percent_triangles=ratio;q.screen_size=screen;levels.append(q)
 opt.reduction_settings=levels;sub.set_lods(mesh,opt);lib.set_metadata_tag(mesh,'CampaignProduction','77');lib.save_loaded_asset(mesh)
if errors:raise RuntimeError(str(errors))
u.log('CAMPAIGN77_IMPORT_PASS '+str(len(data['assets'])));u.SystemLibrary.quit_editor()
