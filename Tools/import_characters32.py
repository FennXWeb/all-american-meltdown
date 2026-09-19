"""Editor-only material/mesh import with explicit slot and unit validation. Never packages."""
import json
from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve();source=root/'ArtSource/Characters32'
data=json.loads((source/'models_32_manifest.json').read_text());lib=u.EditorAssetLibrary;ml=u.MaterialEditingLibrary
t=u.AssetImportTask();t.filename=str(source/'T_CharacterSurface32.png');t.destination_path='/Game/Art/Textures';t.automated=True;t.save=True;t.replace_existing=False
if not lib.does_asset_exist('/Game/Art/Textures/T_CharacterSurface32'):u.AssetToolsHelpers.get_asset_tools().import_asset_tasks([t])
tex=lib.load_asset('/Game/Art/Textures/T_CharacterSurface32');tex.set_editor_property('max_texture_size',1024);tex.set_editor_property('filter',u.TextureFilter.TF_BILINEAR);lib.save_loaded_asset(tex)
f=u.AssetImportTask();f.filename=str(source/'T_CharacterFaces32.png');f.destination_path='/Game/Art/Textures';f.automated=True;f.save=True;f.replace_existing=False
if not lib.does_asset_exist('/Game/Art/Textures/T_CharacterFaces32'):u.AssetToolsHelpers.get_asset_tools().import_asset_tasks([f])
faces=lib.load_asset('/Game/Art/Textures/T_CharacterFaces32');faces.set_editor_property('max_texture_size',1024);faces.set_editor_property('filter',u.TextureFilter.TF_BILINEAR);lib.save_loaded_asset(faces)
for name,(tile,color) in data['materials'].items():
 path='/Game/Materials/M_'+name
 mat=lib.load_asset(path) if lib.does_asset_exist(path) else u.AssetToolsHelpers.get_asset_tools().create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew())
 ml.delete_all_material_expressions(mat)
 tint=ml.create_material_expression(mat,u.MaterialExpressionVectorParameter);tint.set_editor_property('parameter_name','Tint');tint.set_editor_property('default_value',u.LinearColor(*color,1))
 result=tint;pin=''
 if tile>=0:
  q=tile-4 if tile>=4 else tile
  uv=ml.create_material_expression(mat,u.MaterialExpressionTextureCoordinate);uv.u_tiling=.498 if tile>=4 else .46;uv.v_tiling=.498 if tile>=4 else .46
  off=ml.create_material_expression(mat,u.MaterialExpressionConstant2Vector);off.r=(q%2)*.5+(.001 if tile>=4 else .02);off.g=(q//2)*.5+(.001 if tile>=4 else .02)
  add=ml.create_material_expression(mat,u.MaterialExpressionAdd);ml.connect_material_expressions(uv,'',add,'A');ml.connect_material_expressions(off,'',add,'B')
  sample=ml.create_material_expression(mat,u.MaterialExpressionTextureSample);sample.texture=faces if tile>=4 else tex;ml.connect_material_expressions(add,'',sample,'UVs')
  result=ml.create_material_expression(mat,u.MaterialExpressionMultiply);ml.connect_material_expressions(sample,'RGB',result,'A');ml.connect_material_expressions(tint,'',result,'B')
  if tile>=4:
   edge=ml.create_material_expression(mat,u.MaterialExpressionConstant3Vector);edge.constant=u.LinearColor(*((.30,.19,.12) if tile<6 else (.12,.065,.035)),1)
   mask=ml.create_material_expression(mat,u.MaterialExpressionVertexColor);blend=ml.create_material_expression(mat,u.MaterialExpressionLinearInterpolate)
   ml.connect_material_expressions(edge,'',blend,'A');ml.connect_material_expressions(sample,'RGB',blend,'B');ml.connect_material_expressions(mask,'R',blend,'Alpha');ml.connect_material_expressions(blend,'',result,'A')
 ml.connect_material_property(result,pin,u.MaterialProperty.MP_BASE_COLOR)
 rough=ml.create_material_expression(mat,u.MaterialExpressionScalarParameter);rough.set_editor_property('parameter_name','Roughness');rough.set_editor_property('default_value',.86 if name!='Eye32' else .42)
 ml.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS)
 spec=ml.create_material_expression(mat,u.MaterialExpressionConstant);spec.r=.18;ml.connect_material_property(spec,'',u.MaterialProperty.MP_SPECULAR)
 ml.recompile_material(mat);lib.save_loaded_asset(mat)
# Reuse the established validated FBX importer; only substitute this version's manifest.
script=(root/'Tools/import_models_v5.py').read_text().replace("'ModelsV5'","'Characters32'").replace('models_v5_manifest','models_32_manifest')
script=script.replace('data.combine_meshes = True','data.combine_meshes = True\n        data.vertex_color_import_option = u.VertexColorImportOption.REPLACE')
exec(compile(script,'characters32_import','exec'),{'__name__':'__main__'})
u.log('CHARACTERS32_IMPORTED')
