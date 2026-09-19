"""Import v3 models, original generated sign textures, glass material and SFX."""
from pathlib import Path
import runpy
import unreal as u
root=Path(u.Paths.project_dir()).resolve()
runpy.run_path(str(root/'Tools/import_models_v3.py'),run_name='__main__')
lib=u.EditorAssetLibrary;at=u.AssetToolsHelpers.get_asset_tools();ml=u.MaterialEditingLibrary
def material(name):
    path='/Game/Materials/M_'+name
    m=lib.load_asset(path) if lib.does_asset_exist(path) else at.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew())
    ml.delete_all_material_expressions(m)
    return m
def scalar(m,val,prop):
    c=ml.create_material_expression(m,u.MaterialExpressionConstant);c.r=val
    ml.connect_material_property(c,'',prop)
for name in ('Gas','Motel','Clinic','Depot','Diner'):
    asset='Sign'+name+'V3'
    task=u.AssetImportTask();task.filename=str(root/'ArtSource/SignsV3'/('T_'+asset+'.png'))
    task.destination_path='/Game/Art/Textures';task.destination_name='T_'+asset
    task.automated=True;task.replace_existing=True;task.save=True
    at.import_asset_tasks([task]);tex=lib.load_asset(task.destination_path+'/'+task.destination_name)
    if not tex:raise RuntimeError('Missing sign '+asset)
    tex.set_editor_property('filter',u.TextureFilter.TF_NEAREST);lib.save_loaded_asset(tex)
    m=material(asset);n=ml.create_material_expression(m,u.MaterialExpressionTextureSample);n.texture=tex
    ml.connect_material_property(n,'RGB',u.MaterialProperty.MP_BASE_COLOR)
    mul=ml.create_material_expression(m,u.MaterialExpressionMultiply)
    strength=ml.create_material_expression(m,u.MaterialExpressionConstant);strength.r=.18
    ml.connect_material_expressions(strength,'',mul,'B')
    ml.connect_material_expressions(n,'RGB',mul,'A');ml.connect_material_property(mul,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
    scalar(m,.82,u.MaterialProperty.MP_ROUGHNESS);scalar(m,.20,u.MaterialProperty.MP_METALLIC)
    ml.recompile_material(m);lib.save_loaded_asset(m)
m=material('WindowGlass');m.set_editor_property('blend_mode',u.BlendMode.BLEND_TRANSLUCENT);m.set_editor_property('two_sided',True)
c=ml.create_material_expression(m,u.MaterialExpressionConstant3Vector);c.constant=u.LinearColor(.20,.30,.26,1)
ml.connect_material_property(c,'',u.MaterialProperty.MP_BASE_COLOR)
scalar(m,.22,u.MaterialProperty.MP_OPACITY);scalar(m,.2,u.MaterialProperty.MP_ROUGHNESS)
ml.recompile_material(m);lib.save_loaded_asset(m)
runpy.run_path(str(root/'Tools/build_audio_v2.py'),run_name='__main__')
u.log('LW_V3_CONTENT_COMPLETE')
