import unreal as u
import json
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve();source=root/'ArtSource/AudioV44'
rows=json.loads((source/'manifest.json').read_text())['sounds']
assert len(rows)==75 and all((source/r['file']).exists() for r in rows), 'Incomplete batch'
lib=u.EditorAssetLibrary;tools=u.AssetToolsHelpers.get_asset_tools()
cat=lib.load_asset('/Game/Audio/DA_AudioCatalog');slots=dict(cat.get_editor_property('slots'));groups={}
for row in rows:
 task=u.AssetImportTask();task.filename=str(source/row['file']);task.destination_path='/Game/Audio/NPC44';task.automated=True;task.replace_existing=True;task.save=True
 tools.import_asset_tasks([task]);sound=lib.load_asset('/Game/Audio/NPC44/'+row['slot'])
 assert sound and sound.get_editor_property('duration')>0
 sound.set_editor_property('looping',False);lib.save_loaded_asset(sound)
 groups.setdefault(row['group'],[]).append(sound)
for name,tracks in groups.items():
 slot=slots.get(u.Name(name),u.LWAudioSlot())
 slot.set_editor_property('source',tracks[0]);slot.set_editor_property('tracks',tracks)
 slot.set_editor_property('loop',False);slot.set_editor_property('music',False)
 slot.set_editor_property('attenuation',u.LWAudioAttenuation.SPATIAL)
 slot.set_editor_property('description',f'{len(tracks)} generated NPC variations. Random track per vocalization.')
 slots[u.Name(name)]=slot
for old,new in {'Speech0':'HumanMale44','Speech1':'HumanMale44','Speech2':'HumanFemale44','RaiderVoice':'HumanMale44'}.items():slots[u.Name(old)]=slots[u.Name(new)]
cat.set_editor_property('slots',slots);lib.save_loaded_asset(cat)
for name,tracks in groups.items():
 actual=cat.get_editor_property('slots')[u.Name(name)]
 assert len(actual.get_editor_property('tracks'))==(10 if name.startswith('Human') else 5),name
u.log('NPC44_IMPORT_DONE clips=75 groups=13')
