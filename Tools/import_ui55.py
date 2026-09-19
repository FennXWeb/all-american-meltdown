import unreal as u
from pathlib import Path
for name in ['T_Menu55','T_Surface55']:
    task=u.AssetImportTask()
    task.filename=str(Path('X:/LethalWorld/ArtSource/UI55',name+'.png'))
    task.destination_path='/Game/Art/UI55'
    task.destination_name=name
    task.automated=True
    task.replace_existing=True
    task.save=True
    u.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    asset=u.EditorAssetLibrary.load_asset('/Game/Art/UI55/'+name)
    assert asset
    asset.set_editor_property('lod_group',u.TextureGroup.TEXTUREGROUP_UI)
    asset.set_editor_property('compression_settings',u.TextureCompressionSettings.TC_EDITOR_ICON)
    asset.set_editor_property('never_stream',True)
    u.EditorAssetLibrary.save_loaded_asset(asset)
u.log('UI55_IMPORT_PASS')
u.SystemLibrary.quit_editor()

