"""Reimport and validate the three aircraft hulls; preserve audio and materials."""
from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir())
code=(root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','Aviation84').replace('models_v5_manifest','models_aviation84_manifest')
code=code.replace("records = manifest['assets']", "records = [r for r in manifest['assets'] if r['name'] in ['AV84_JetShell','AV84_AirbusShell','AV84_AirbusLuxuryShell']]")
exec(compile(code,'aviation84-hulls','exec'),{'__name__':'__main__'})
sub=u.get_editor_subsystem(u.StaticMeshEditorSubsystem)
for name in ['JetShell','AirbusShell','AirbusLuxuryShell']:
 mesh=u.EditorAssetLibrary.load_asset('/Game/Art/Meshes/SM_AV84_'+name);mesh.get_editor_property('body_setup').set_editor_property('collision_trace_flag',u.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE)
 opts=u.EditorScriptingMeshReductionOptions();opts.auto_compute_lod_screen_size=False;levels=[]
 for ratio,screen in [(1,1),(.6,.14),(.25,.035)]:
  q=u.EditorScriptingMeshReductionSettings();q.percent_triangles=ratio;q.screen_size=screen;levels.append(q)
 opts.reduction_settings=levels;sub.set_lods(mesh,opts);u.EditorAssetLibrary.save_loaded_asset(mesh)
u.log('AVIATION84_HULLS_PASS')
u.SystemLibrary.quit_editor()
