import unreal as u
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve()
source=(root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','ModelsV56').replace('models_v5_manifest','models_v56_manifest')
exec(compile(source,'rv56_import','exec'),{'__name__':'__main__'})
for name in ['Vehicle42_rv','Cabin42_rv']:
 mesh=u.EditorAssetLibrary.load_asset('/Game/Art/Meshes/SM_'+name)
 mesh.set_editor_property('allow_cpu_access',True)
 u.EditorAssetLibrary.save_loaded_asset(mesh)
u.log('RV56_IMPORT_DONE')
