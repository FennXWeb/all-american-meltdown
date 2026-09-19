"""V17 models, laser material and audio slots. No packaging."""
import unreal as u,runpy
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary;tools=u.AssetToolsHelpers.get_asset_tools();ml=u.MaterialEditingLibrary
path='/Game/Materials/M_LaserV17'
m=lib.load_asset(path) if lib.does_asset_exist(path) else tools.create_asset('M_LaserV17','/Game/Materials',u.Material,u.MaterialFactoryNew())
ml.delete_all_material_expressions(m);m.set_editor_property('shading_model',u.MaterialShadingModel.MSM_UNLIT);m.set_editor_property('blend_mode',u.BlendMode.BLEND_ADDITIVE)
v=ml.create_material_expression(m,u.MaterialExpressionConstant3Vector);v.constant=u.LinearColor(18,.035,.015,1);ml.connect_material_property(v,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
a=ml.create_material_expression(m,u.MaterialExpressionConstant);a.r=.65;ml.connect_material_property(a,'',u.MaterialProperty.MP_OPACITY);ml.recompile_material(m);lib.save_loaded_asset(m)
exec(compile((root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','ModelsV17').replace('models_v5_manifest','models_v17_manifest'),'import_v17','exec'))
runpy.run_path(str(root/'Tools/build_audio_v2.py'),run_name='__main__')
u.log('AAM_V17_CONTENT_COMPLETE')
