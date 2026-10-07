import unreal as u
from pathlib import Path
ROOT=Path(u.Paths.project_dir()).resolve();A=u.AssetToolsHelpers.get_asset_tools();L=u.EditorAssetLibrary;M=u.MaterialEditingLibrary
def task(path,dest,mesh=False):
 t=u.AssetImportTask();t.filename=str(path);t.destination_path=dest;t.automated=True;t.replace_existing=True;t.save=True
 if mesh:
  opts=u.FbxImportUI();opts.import_mesh=True;opts.import_materials=False;opts.import_textures=False;opts.import_as_skeletal=False;opts.mesh_type_to_import=u.FBXImportType.FBXIT_STATIC_MESH;opts.automated_import_should_detect_type=False;opts.static_mesh_import_data.combine_meshes=True;opts.static_mesh_import_data.auto_generate_collision=True;t.options=opts;t.factory=u.FbxFactory()
 A.import_asset_tasks([t]);asset=L.load_asset(dest+'/'+path.stem)
 if not asset:raise RuntimeError(str(path))
 return asset
tex=task(ROOT/'ArtSource/Syracuse73/T_SyracuseAtlas73.png','/Game/Art/Textures');tex.set_editor_property('lod_group',u.TextureGroup.TEXTUREGROUP_WORLD);L.save_loaded_asset(tex)
for name,x,y in [('ArtDeco73',0,0),('PalaceVelvet73',.5,0),('PalaceCeiling73',0,.5),('CampusStone73',.5,.5)]:
 path='/Game/Materials/M_'+name;mat=L.load_asset(path) if L.does_asset_exist(path) else A.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew());M.delete_all_material_expressions(mat)
 uv=M.create_material_expression(mat,u.MaterialExpressionTextureCoordinate);frac=M.create_material_expression(mat,u.MaterialExpressionFrac);mul=M.create_material_expression(mat,u.MaterialExpressionMultiply);scale=M.create_material_expression(mat,u.MaterialExpressionConstant);scale.r=.495;M.connect_material_expressions(scale,"",mul,"B")
 offset=M.create_material_expression(mat,u.MaterialExpressionConstant2Vector);offset.r=x+.0025;offset.g=y+.0025;add=M.create_material_expression(mat,u.MaterialExpressionAdd);sample=M.create_material_expression(mat,u.MaterialExpressionTextureSample);sample.texture=tex
 for src,dst,pin in [(uv,frac,''),(frac,mul,'A'),(mul,add,'A'),(offset,add,'B'),(add,sample,'UVs')]:
  if not M.connect_material_expressions(src,'',dst,pin):raise RuntimeError('Invalid material connection '+pin)
 M.connect_material_property(sample,'RGB',u.MaterialProperty.MP_BASE_COLOR);ambient=M.create_material_expression(mat,u.MaterialExpressionMultiply);ambient_scale=M.create_material_expression(mat,u.MaterialExpressionConstant);ambient_scale.r=.045;M.connect_material_expressions(sample,'RGB',ambient,'A');M.connect_material_expressions(ambient_scale,'',ambient,'B');M.connect_material_property(ambient,'',u.MaterialProperty.MP_EMISSIVE_COLOR);rough=M.create_material_expression(mat,u.MaterialExpressionConstant);rough.r=.78;M.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS);M.set_material_usage(mat,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES);M.recompile_material(mat);L.save_loaded_asset(mat)
for name,rgb,roughness in [('Ice73',(.52,.64,.69),.18),('Projection73',(.64,.61,.54),1),('Turf73',(.045,.16,.035),1),('Roof73',(.18,.20,.22),.85),('LineWhite73',(.7,.7,.66),.8),('BluePaint73',(.02,.10,.38),.55),('RedPaint73',(.55,.025,.025),.55)]:
 path='/Game/Materials/M_'+name;mat=L.load_asset(path) if L.does_asset_exist(path) else A.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew());M.delete_all_material_expressions(mat)
 color=M.create_material_expression(mat,u.MaterialExpressionConstant3Vector);color.constant=u.LinearColor(*rgb,1);M.connect_material_property(color,'',u.MaterialProperty.MP_BASE_COLOR)
 rough=M.create_material_expression(mat,u.MaterialExpressionConstant);rough.r=roughness;M.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS);M.set_material_usage(mat,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES);M.recompile_material(mat);L.save_loaded_asset(mat)
for p in (ROOT/'ArtSource/Syracuse73/Models').glob('*.fbx'):
 mesh=task(p,'/Game/Art/Meshes',True)
 for i,slot in enumerate(mesh.static_materials):
  name=str(slot.material_slot_name).split('.')[0].removeprefix('M_');mat=L.load_asset('/Game/Materials/M_'+name)
  if not mat:raise RuntimeError('Missing mesh material '+name)
  mesh.set_material(i,mat)
 L.save_loaded_asset(mesh)
u.log('SYRACUSE73_IMPORT_COMPLETE')

for name in ['RoadGreen33','RoadOchre33','RoadBlack33']:
 mat=L.load_asset('/Game/Materials/M_'+name)
 if mat:M.set_material_usage(mat,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES);M.recompile_material(mat);L.save_loaded_asset(mat)
