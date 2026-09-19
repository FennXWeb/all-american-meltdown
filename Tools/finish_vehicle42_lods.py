import unreal as u,json
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary;sub=u.get_editor_subsystem(u.StaticMeshEditorSubsystem)
assert sub,'Requires full editor initialization'
for row in json.loads((root/'ArtSource/ModelsV42/models_v42_manifest.json').read_text())['assets']:
 mesh=lib.load_asset(row['asset']);mesh.set_editor_property('allow_cpu_access',True)
 if row['name'].startswith('Camper42') or row['name'] in ('Seat42','Steering42'):
  mesh.get_editor_property('body_setup').set_editor_property('collision_trace_flag',u.CollisionTraceFlag.CTF_USE_COMPLEX_AS_SIMPLE)
 opts=u.StaticMeshReductionOptions();settings=[]
 for percent,screen in [(1.,1.),(.5,.35),(.20,.12)]:
  setting=u.StaticMeshReductionSettings();setting.percent_triangles=percent;setting.screen_size=screen;settings.append(setting)
 opts.reduction_settings=settings;opts.auto_compute_lod_screen_size=False
 result=sub.set_lods(mesh,opts)
 assert result>=0,row['name']
 lib.save_loaded_asset(mesh)
u.log('VEHICLE42_LODS_DONE')
u.SystemLibrary.quit_editor()
