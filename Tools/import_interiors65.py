"""Full editor Python: import, validate material/units, and generate interior LODs."""
import unreal as u,json
from pathlib import Path
root=Path('X:/LethalWorld');src=root/'ArtSource/Interiors65';data=json.loads((src/'manifest.json').read_text());dest='/Game/Art/Interiors65';lib=u.EditorAssetLibrary;ml=u.MaterialEditingLibrary;at=u.AssetToolsHelpers.get_asset_tools();lib.make_directory(dest)
t=u.AssetImportTask();t.filename=str(src/'T_Interiors65.png');t.destination_path=dest;t.automated=True;t.save=True;t.replace_existing=True;at.import_asset_tasks([t]);atlas=lib.load_asset(dest+'/T_Interiors65')
mats={};names=['Wood','Upholstery','Paint','Steel','Acoustic','Glass','Leather','Rubber','Carpet','Terrazzo','Cardboard','Ceramic','Plaster','Navy','Brass','Grille']
for i,key in enumerate(names):
 name='M_'+key+'65';m=(lib.load_asset(dest+'/'+name) if lib.does_asset_exist(dest+'/'+name) else None) or at.create_asset(name,dest,u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(m)
 def node(c):return ml.create_material_expression(m,c)
 def link(a,b,p=''):
  if not ml.connect_material_expressions(a,'',b,p):raise RuntimeError('material pin '+p)
 def c(x):n=node(u.MaterialExpressionConstant);n.r=x;return n
 def v2(x,y):n=node(u.MaterialExpressionConstant2Vector);n.r=x;n.g=y;return n
 def op(cls,a,b):n=node(cls);link(a,n,'A');link(b,n,'B');return n
 uv=node(u.MaterialExpressionTextureCoordinate);frac=node(u.MaterialExpressionFrac);link(uv,frac);mapped=op(u.MaterialExpressionAdd,op(u.MaterialExpressionMultiply,frac,v2(.242,.242)),v2(i%4*.25+.004,i//4*.25+.004));tex=node(u.MaterialExpressionTextureSample);tex.texture=atlas;link(mapped,tex,'UVs');ml.connect_material_property(tex,'RGB',u.MaterialProperty.MP_BASE_COLOR)
 ml.connect_material_property(c(.38 if key in ['Steel','Glass','Brass','Ceramic'] else .82),'',u.MaterialProperty.MP_ROUGHNESS)
 ml.connect_material_property(c(.65 if key in ['Steel','Brass'] else 0),'',u.MaterialProperty.MP_METALLIC)
 m.set_editor_property('two_sided',key=='Upholstery');ml.set_material_usage(m,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES);ml.recompile_material(m);lib.save_loaded_asset(m);mats[name]=m
# Dedicated artwork uses full-face UVs, with one material for each original print.
t=u.AssetImportTask();t.filename=str(src/'T_WallArt65.png');t.destination_path=dest;t.automated=True;t.save=True;t.replace_existing=True;at.import_asset_tasks([t]);art=lib.load_asset(dest+'/T_WallArt65')
for i in range(16):
 name='M_Art%02d_65'%i;m=(lib.load_asset(dest+'/'+name) if lib.does_asset_exist(dest+'/'+name) else None) or at.create_asset(name,dest,u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(m)
 uv=node(u.MaterialExpressionTextureCoordinate);mapped=op(u.MaterialExpressionAdd,op(u.MaterialExpressionMultiply,uv,v2(.244,.244)),v2(i%4*.25+.003,i//4*.25+.003));tex=node(u.MaterialExpressionTextureSample);tex.texture=art;link(mapped,tex,'UVs');ml.connect_material_property(tex,'RGB',u.MaterialProperty.MP_BASE_COLOR);ml.connect_material_property(c(.85),'',u.MaterialProperty.MP_ROUGHNESS);m.set_editor_property('two_sided',True);ml.set_material_usage(m,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES);ml.recompile_material(m);lib.save_loaded_asset(m)
sub=u.get_editor_subsystem(u.StaticMeshEditorSubsystem);errors=[]
for r in data['assets']:
 if lib.does_asset_exist(dest+'/SM_'+r['name']):continue
 t=u.AssetImportTask();t.filename=str(src/(r['name']+'.fbx'));t.destination_path=dest;t.destination_name='SM_'+r['name'];t.automated=True;t.replace_existing=True;t.replace_existing_settings=True;t.save=False
 opts=u.FbxImportUI();opts.import_mesh=True;opts.import_materials=False;opts.import_textures=False;opts.import_as_skeletal=False;opts.mesh_type_to_import=u.FBXImportType.FBXIT_STATIC_MESH;opts.automated_import_should_detect_type=False;d=opts.static_mesh_import_data;d.combine_meshes=True;d.auto_generate_collision=r['kind']=='floor';d.generate_lightmap_u_vs=False;d.convert_scene=True;d.convert_scene_unit=True;d.normal_import_method=u.FBXNormalImportMethod.FBXNIM_IMPORT_NORMALS;t.options=opts;t.factory=u.FbxFactory();at.import_asset_tasks([t]);mesh=lib.load_asset(dest+'/SM_'+r['name'])
 if not mesh:errors.append(r['name']);continue
 for j,s in enumerate(mesh.get_editor_property('static_materials')):
  name=str(s.get_editor_property('imported_material_slot_name'))
  if name not in mats:errors.append(r['name']+' '+name)
  else:mesh.set_material(j,mats[name])
 b=mesh.get_bounding_box();actual=[b.min.x,b.min.y,b.min.z,b.max.x,b.max.y,b.max.z];expected=[v*100 for v in r['bounds']['min']+r['bounds']['max']]
 if max(abs(a-b) for a,b in zip(actual,expected))>.1:errors.append(r['name']+' units or bounds')
 opts=u.EditorScriptingMeshReductionOptions();opts.auto_compute_lod_screen_size=False;levels=[]
 for ratio,screen in [(1,1),(.5,.35),(.18,.12)]:q=u.EditorScriptingMeshReductionSettings();q.percent_triangles=ratio;q.screen_size=screen;levels.append(q)
 opts.reduction_settings=levels;sub.set_lods(mesh,opts);lib.set_metadata_tag(mesh,'InteriorRevision','65');lib.save_loaded_asset(mesh)
if errors:raise RuntimeError(str(errors))
u.log('INTERIORS65_IMPORT_PASS '+str(len(data['assets'])));u.SystemLibrary.quit_editor()

