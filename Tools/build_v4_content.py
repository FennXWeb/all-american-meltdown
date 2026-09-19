from pathlib import Path
import runpy
import unreal as u
root=Path(u.Paths.project_dir()).resolve()
for script in ['import_models_v4.py','build_audio_v2.py']:runpy.run_path(str(root/'Tools'/script),run_name='__main__')
p='/Game/Data/DA_RPGCatalog';lib=u.EditorAssetLibrary
if not lib.does_asset_exist(p):
 factory=u.DataAssetFactory();factory.set_editor_property('data_asset_class',u.LWRPGCatalog)
 asset=u.AssetToolsHelpers.get_asset_tools().create_asset('DA_RPGCatalog','/Game/Data',u.LWRPGCatalog,factory)
 lib.save_loaded_asset(asset)
u.log('LW_V4_CONTENT_COMPLETE')
