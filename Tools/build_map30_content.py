"""Import the generated 64-cell POI atlas; never cooks or packages."""
from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve()
lib=u.EditorAssetLibrary
path='/Game/Art/Textures/T_POIAtlas30'
if not lib.does_asset_exist(path):
 task=u.AssetImportTask()
 task.filename=str(root/'ContentSource/Map30/T_POIAtlas30.png')
 task.destination_path='/Game/Art/Textures'
 task.automated=True
 task.save=True
 u.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
tex=lib.load_asset(path)
if not tex:raise RuntimeError('POI atlas import failed')
tex.set_editor_property('lod_group',u.TextureGroup.TEXTUREGROUP_UI)
tex.set_editor_property('mip_gen_settings',u.TextureMipGenSettings.TMGS_NO_MIPMAPS)
tex.set_editor_property('filter',u.TextureFilter.TF_BILINEAR)
tex.set_editor_property('compression_settings',u.TextureCompressionSettings.TC_EDITOR_ICON)
tex.set_editor_property('srgb',True)
lib.save_loaded_asset(tex)
u.log('AAM_MAP30_CONTENT_COMPLETE')
