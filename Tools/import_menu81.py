"""Import generated layered main menu art; no maps or builds are packaged."""
from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve()
for name in ['T_Skyline81','T_Foreground81','T_Mist81']:
 task=u.AssetImportTask();task.filename=str(root/'ArtSource/Menu81'/f'{name}.png');task.destination_path='/Game/Art/Menu81';task.destination_name=name;task.automated=True;task.replace_existing=True;task.save=False
 u.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
 tex=u.EditorAssetLibrary.load_asset('/Game/Art/Menu81/'+name)
 if not tex:raise RuntimeError('Missing imported texture '+name)
 tex.set_editor_property('compression_settings',u.TextureCompressionSettings.TC_EDITOR_ICON)
 tex.set_editor_property('lod_group',u.TextureGroup.TEXTUREGROUP_UI)
 tex.set_editor_property('mip_gen_settings',u.TextureMipGenSettings.TMGS_NO_MIPMAPS)
 tex.set_editor_property('never_stream',True)
 u.EditorAssetLibrary.save_loaded_asset(tex)
path='/Game/Art/Menu81/M_Studio81'
mat=u.EditorAssetLibrary.load_asset(path) if u.EditorAssetLibrary.does_asset_exist(path) else u.AssetToolsHelpers.get_asset_tools().create_asset('M_Studio81','/Game/Art/Menu81',u.Material,u.MaterialFactoryNew())
mat.set_editor_property('shading_model',u.MaterialShadingModel.MSM_UNLIT)
u.MaterialEditingLibrary.delete_all_material_expressions(mat)
color=u.MaterialEditingLibrary.create_material_expression(mat,u.MaterialExpressionConstant3Vector)
color.set_editor_property('constant',u.LinearColor(.09,.13,.16,1))
u.MaterialEditingLibrary.connect_material_property(color,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
u.MaterialEditingLibrary.recompile_material(mat)
u.EditorAssetLibrary.save_loaded_asset(mat)
u.log('MENU81_IMPORT_OK')
u.SystemLibrary.quit_editor()
