import unreal as u
lib=u.EditorAssetLibrary
count=0
for p in lib.list_assets('/Game/Art',recursive=True):
 name=p.rsplit('/',1)[-1].removeprefix('SM_')
 if name.startswith('Vehicle_') or name.startswith('SupercarV21') or name.startswith('SedanShellV5'):
  a=lib.load_asset(p)
  if isinstance(a,u.StaticMesh):
   a.set_editor_property('allow_cpu_access',True);lib.save_loaded_asset(a);count+=1
u.log('AAM_V23_DENT_MESHES %d'%count)
if count<11:raise RuntimeError('Missing vehicle meshes for denting')
