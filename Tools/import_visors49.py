import unreal as u,json
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve()
source=(root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','ModelsV49').replace('models_v5_manifest','models_v49_manifest')
exec(compile(source,'visors49_import','exec'),{'__name__':'__main__'})
sub=u.get_editor_subsystem(u.StaticMeshEditorSubsystem)
for row in json.loads((root/'ArtSource/ModelsV49/models_v49_manifest.json').read_text())['assets']:
 mesh=u.EditorAssetLibrary.load_asset(row['asset']);mesh.set_editor_property('allow_cpu_access',True)
 opts=u.StaticMeshReductionOptions();settings=[]
 for percent,screen in [(1.,1.),(.5,.35),(.2,.12)]:
  s=u.StaticMeshReductionSettings();s.percent_triangles=percent;s.screen_size=screen;settings.append(s)
 opts.reduction_settings=settings;opts.auto_compute_lod_screen_size=False
 assert sub.set_lods(mesh,opts)>=0
 u.EditorAssetLibrary.save_loaded_asset(mesh)
u.log('VISORS49_IMPORTED');u.SystemLibrary.quit_editor()
