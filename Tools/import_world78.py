"""Import update 78 art and materials; no cooking or packaging."""
from pathlib import Path
import unreal as u
src=Path('X:/LethalWorld/ArtSource/World78');dest='/Game/Art/World78';lib=u.EditorAssetLibrary;at=u.AssetToolsHelpers.get_asset_tools();ml=u.MaterialEditingLibrary
lib.make_directory(dest)
for name in ['T_Menu78','T_Signs78']:
 t=u.AssetImportTask();t.filename=str(src/(name+'.png'));t.destination_path=dest;t.automated=True;t.save=True;t.replace_existing=True;at.import_asset_tasks([t]);tex=lib.load_asset(dest+'/'+name)
 if name=='T_Menu78':tex.set_editor_property('lod_group',u.TextureGroup.TEXTUREGROUP_UI);tex.set_editor_property('compression_settings',u.TextureCompressionSettings.TC_EDITOR_ICON);tex.set_editor_property('never_stream',True)
 lib.save_loaded_asset(tex)
def material(name,color,rough=.65):
 m=lib.load_asset(dest+'/'+name) if lib.does_asset_exist(dest+'/'+name) else at.create_asset(name,dest,u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(m)
 c=ml.create_material_expression(m,u.MaterialExpressionConstant3Vector);c.constant=u.LinearColor(*color);ml.connect_material_property(c,'',u.MaterialProperty.MP_BASE_COLOR)
 q=ml.create_material_expression(m,u.MaterialExpressionConstant);q.r=rough;ml.connect_material_property(q,'',u.MaterialProperty.MP_ROUGHNESS)
 ml.set_material_usage(m,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES);return m
m=material('M_Sign78',(.08,.13,.13,1));n=ml.create_material_expression(m,u.MaterialExpressionTextureSampleParameter2D);n.set_editor_property('parameter_name','Paint');n.texture=lib.load_asset(dest+'/T_Signs78');ml.connect_material_property(n,'RGB',u.MaterialProperty.MP_BASE_COLOR)
ml.recompile_material(m);lib.save_loaded_asset(m)
colors={'Frame':(.07,.085,.09,1),'Canvas':(.22,.28,.16,1),'Rubber':(.025,.03,.025,1),'Rope':(.45,.39,.26,1)};mats={'M_Sign78':m}
for key,color in colors.items():
 m=material('M_'+key+'78',color,.85 if key!='Frame' else .48)
 if key=='Canvas':
  tex=lib.load_asset('/Game/Art/Campaign77/T_Campaign77');n=ml.create_material_expression(m,u.MaterialExpressionTextureSample);n.texture=tex
  uv=ml.create_material_expression(m,u.MaterialExpressionTextureCoordinate);mul=ml.create_material_expression(m,u.MaterialExpressionMultiply);mul.set_editor_property('const_b',.22);ml.connect_material_expressions(uv,'',mul,'A')
  add=ml.create_material_expression(m,u.MaterialExpressionAdd);v=ml.create_material_expression(m,u.MaterialExpressionConstant2Vector);v.r=.265;v.g=.265;ml.connect_material_expressions(mul,'',add,'A');ml.connect_material_expressions(v,'',add,'B');assert ml.connect_material_expressions(add,'',n,'UVs'), 'Canvas atlas UV input not connected';ml.connect_material_property(n,'RGB',u.MaterialProperty.MP_BASE_COLOR);m.set_editor_property('two_sided',True)
 ml.recompile_material(m);lib.save_loaded_asset(m);mats['M_'+key+'78']=m
for name in ['SignPanel78','TrailTent78']:
 t=u.AssetImportTask();t.filename=str(src/(name+'.fbx'));t.destination_path=dest;t.destination_name='SM_'+name;t.automated=True;t.replace_existing=True;t.save=False
 opts=u.FbxImportUI();opts.import_mesh=True;opts.import_materials=False;opts.import_textures=False;opts.import_as_skeletal=False;opts.mesh_type_to_import=u.FBXImportType.FBXIT_STATIC_MESH;opts.automated_import_should_detect_type=False
 d=opts.static_mesh_import_data;d.combine_meshes=True;d.auto_generate_collision=False;d.generate_lightmap_u_vs=False;d.convert_scene=True;d.convert_scene_unit=True;t.options=opts;t.factory=u.FbxFactory();at.import_asset_tasks([t]);mesh=lib.load_asset(dest+'/SM_'+name)
 for j,s in enumerate(mesh.get_editor_property('static_materials')):mesh.set_material(j,mats[str(s.get_editor_property('imported_material_slot_name'))])
 mesh.get_editor_property('body_setup').set_editor_property('collision_trace_flag',u.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE);lib.save_loaded_asset(mesh)
u.log('WORLD78_IMPORT_PASS');u.SystemLibrary.quit_editor()

