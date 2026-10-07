import unreal as u,json
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary
table=lib.load_asset('/Game/Data/DA_EnemySpawns');rows=list(table.get_editor_property('POIs'))
for t,z,raider,robot,hornet,bear in [(64,10,45,45,0,0),(65,35,35,30,0,0),(66,40,55,5,0,0),(67,65,25,0,5,8)]:
 row=next((r for r in rows if r.get_editor_property('POIType')==t),None)
 if row is None:row=u.LWSpawnRow();row.set_editor_property('POIType',t);rows.append(row)
 for prop,value in {'MinCount':5 if t==64 else 2,'MaxCount':10 if t==64 else 5,'ZombieWeight':z,'RaiderWeight':raider,'RogueWeight':robot,'HornetWeight':hornet,'BearWeight':bear,'DogWeight':0,'TitanWeight':0,'DeathclawWeight':0,'ScorpionWeight':0,'KarenWeight':0,'IndoorChance':.65}.items():row.set_editor_property(prop,value)
table.set_editor_property('POIs',rows);lib.save_loaded_asset(table)
loot=lib.load_asset('/Game/Data/DA_LootTable');presets=list(loot.get_editor_property('Presets'))
for p in presets:
 if str(p.get_editor_property('Context')) not in ('military','depot','trader'):continue
 entries=list(p.get_editor_property('Entries'))
 if not any(str(e.get_editor_property('ItemId'))=='ammo_30mm' for e in entries):
  e=u.LWLootEntry();e.set_editor_property('ItemId',u.Name('ammo_30mm'));e.set_editor_property('Weight',8.0);e.set_editor_property('MinCount',2);e.set_editor_property('MaxCount',6);entries.append(e);p.set_editor_property('Entries',entries)
loot.set_editor_property('Presets',presets);lib.save_loaded_asset(loot)
sub=u.get_editor_subsystem(u.StaticMeshEditorSubsystem);assert sub
for row in json.loads((root/'ArtSource/ModelsV57/models_v57_manifest.json').read_text())['assets']:
 mesh=lib.load_asset(row['asset']);assert mesh
 opts=u.StaticMeshReductionOptions();settings=[]
 for percent,screen in [(1.,1.),(.55,.30),(.25,.10)]:
  s=u.StaticMeshReductionSettings();s.percent_triangles=percent;s.screen_size=screen;settings.append(s)
 opts.reduction_settings=settings;opts.auto_compute_lod_screen_size=False
 assert sub.set_lods(mesh,opts)>=0,row['name'];lib.save_loaded_asset(mesh)
u.log('EXPANSION57_FINISH_DONE 41 LOD sets, 4 spawn tables')
u.SystemLibrary.quit_editor()
