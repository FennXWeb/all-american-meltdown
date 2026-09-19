"""Chapter artwork import and four atlas materials. Editor only; never packages."""
from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary;ml=u.MaterialEditingLibrary
name='/Game/Art/Textures/T_Chapter31'
if not lib.does_asset_exist(name):
 t=u.AssetImportTask();t.filename=str(root/'ContentSource/Chapter31/T_Chapter31.png');t.destination_path='/Game/Art/Textures';t.automated=True;t.save=True;u.AssetToolsHelpers.get_asset_tools().import_asset_tasks([t])
tex=lib.load_asset(name)
if not tex:raise RuntimeError('Chapter art import failed')
tex.set_editor_property('filter',u.TextureFilter.TF_BILINEAR);tex.set_editor_property('srgb',True);lib.save_loaded_asset(tex)
for index in range(4):
 path='/Game/Materials/M_Chapter31_'+str(index)
 if lib.does_asset_exist(path):continue
 m=u.AssetToolsHelpers.get_asset_tools().create_asset('M_Chapter31_'+str(index),'/Game/Materials',u.Material,u.MaterialFactoryNew())
 m.set_editor_property('two_sided',True)
 uv=ml.create_material_expression(m,u.MaterialExpressionTextureCoordinate);uv.u_tiling=.48;uv.v_tiling=.48
 offset=ml.create_material_expression(m,u.MaterialExpressionConstant2Vector);offset.r=(index%2)*.5+.01;offset.g=(index//2)*.5+.01
 add=ml.create_material_expression(m,u.MaterialExpressionAdd);ml.connect_material_expressions(uv,'',add,'A');ml.connect_material_expressions(offset,'',add,'B')
 sample=ml.create_material_expression(m,u.MaterialExpressionTextureSample);sample.texture=tex;ml.connect_material_expressions(add,'',sample,'UVs');ml.connect_material_property(sample,'RGB',u.MaterialProperty.MP_BASE_COLOR)
 rough=ml.create_material_expression(m,u.MaterialExpressionConstant);rough.r=.93;ml.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS)
 emission=ml.create_material_expression(m,u.MaterialExpressionMultiply);gain=ml.create_material_expression(m,u.MaterialExpressionConstant);gain.r=.09;ml.connect_material_expressions(gain,'',emission,'B');ml.connect_material_expressions(sample,'RGB',emission,'A');ml.connect_material_property(emission,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 ml.recompile_material(m);lib.save_loaded_asset(m)
u.log('CHAPTER31_ART_COMPLETE')

