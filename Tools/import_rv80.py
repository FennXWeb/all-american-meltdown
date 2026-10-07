from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve()
source=(root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','RV80').replace('models_v5_manifest','models_rv80_manifest')
exec(compile(source,'rv80_import','exec'),{'__name__':'__main__'})
for name in ['Interior','Joinery','SlideSeal','Radio','Rocker','Stalk','Ignition','ElectricLowerDash']:
 mesh=u.EditorAssetLibrary.load_asset('/Game/Art/Meshes/SM_RV80_'+name)
 mesh.set_editor_property('allow_cpu_access',True)
 mesh.get_editor_property('body_setup').set_editor_property('collision_trace_flag',u.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE)
 u.EditorAssetLibrary.save_loaded_asset(mesh)
u.log('RV80_IMPORT_OK')
u.SystemLibrary.quit_editor()
