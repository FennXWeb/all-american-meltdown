import unreal as u,json,os
from pathlib import Path
root=Path('X:/LethalWorld');data=json.loads((root/'ArtSource/Models63/manifest.json').read_text());sub=u.get_editor_subsystem(u.StaticMeshEditorSubsystem);lib=u.EditorAssetLibrary
for r in data['assets']:
 if os.environ.get('LW63_WAISTS') and not ('Waist' in r['name'] or r['name']=='TitanPelvis32'):continue
 if r['group']=='attachment' or r['name'].startswith(('Hair','Beard')):continue
 mesh=lib.load_asset('/Game/Art/Models63/SM_'+r['name']);opts=u.StaticMeshReductionOptions();settings=[]
 for percent,screen in [(1.,1.),(.5,.35),(.18,.12)]:
  s=u.StaticMeshReductionSettings();s.percent_triangles=percent;s.screen_size=screen;settings.append(s)
 opts.reduction_settings=settings;opts.auto_compute_lod_screen_size=False
 if sub.set_lods(mesh,opts)<0:raise RuntimeError(r['name'])
 lib.save_loaded_asset(mesh)
u.log('MODELS63_LODS_PASS')
