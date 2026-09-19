"""Import original V16 models and alpha logo; editor only, never cook/package."""
import unreal as u
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary;tools=u.AssetToolsHelpers.get_asset_tools();ml=u.MaterialEditingLibrary
t=u.AssetImportTask();t.filename=str(root/'ArtSource/Branding16/T_AllAmericanMeltdownAlpha.png');t.destination_path='/Game/Art/Textures';t.automated=True;t.replace_existing=True;t.save=True;tools.import_asset_tasks([t])
tex=lib.load_asset('/Game/Art/Textures/T_AllAmericanMeltdownAlpha');tex.set_editor_property('compression_settings',u.TextureCompressionSettings.TC_EDITOR_ICON);tex.set_editor_property('mip_gen_settings',u.TextureMipGenSettings.TMGS_NO_MIPMAPS);tex.set_editor_property('lod_group',u.TextureGroup.TEXTUREGROUP_UI);lib.save_loaded_asset(tex)
for name,col in [('SunV16',u.LinearColor(5,3.2,1.4,1)),('MoonV16',u.LinearColor(.65,.71,.78,1))]:
 path='/Game/Materials/M_'+name
 m=lib.load_asset(path) if lib.does_asset_exist(path) else tools.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew())
 ml.delete_all_material_expressions(m);m.set_editor_property('shading_model',u.MaterialShadingModel.MSM_UNLIT)
 v=ml.create_material_expression(m,u.MaterialExpressionConstant3Vector);v.constant=col
 # The moon has irregular darker mare / crater shading, generated in the material.
 if name=='MoonV16':
  n=ml.create_material_expression(m,u.MaterialExpressionNoise);n.set_editor_property('scale',5.0);normal=ml.create_material_expression(m,u.MaterialExpressionVertexNormalWS);ml.connect_material_expressions(normal,'',n,'Position')
  mul=ml.create_material_expression(m,u.MaterialExpressionMultiply);ml.connect_material_expressions(n,'',mul,'A');ml.connect_material_expressions(v,'',mul,'B');ml.connect_material_property(mul,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 else:ml.connect_material_property(v,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 ml.recompile_material(m);lib.save_loaded_asset(m)
exec(compile((root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','ModelsV16').replace('models_v5_manifest','models_v16_manifest'),'import_v16','exec'))
u.log('AAM_V16_CONTENT_COMPLETE')

