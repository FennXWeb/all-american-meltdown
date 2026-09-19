import unreal as u
import json
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve();source=root/'ArtSource/AudioV42';lib=u.EditorAssetLibrary;tools=u.AssetToolsHelpers.get_asset_tools();rows=json.loads((source/'manifest.json').read_text())['sounds']
missing=[r['file'] for r in rows if not (source/r['file']).exists()]
if missing:raise RuntimeError('Audio batch incomplete; not modifying the catalog: '+str(len(missing))+' missing')
cat=lib.load_asset('/Game/Audio/DA_AudioCatalog');slots=dict(cat.get_editor_property('slots'))
for row in rows:
 task=u.AssetImportTask();task.filename=str(source/row['file']);task.destination_path='/Game/Audio/Vehicles42';task.automated=True;task.replace_existing=True;task.save=True;tools.import_asset_tasks([task])
 sound=lib.load_asset('/Game/Audio/Vehicles42/'+row['slot']);sound.set_editor_property('looping',row['loop']);lib.save_loaded_asset(sound)
 slot=u.LWAudioSlot();slot.set_editor_property('source',sound);slot.set_editor_property('tracks',[sound]);slot.set_editor_property('loop',row['loop']);slot.set_editor_property('volume',.8);slot.set_editor_property('pitch',1.);slot.set_editor_property('attenuation',u.LWAudioAttenuation.SPATIAL);slot.set_editor_property('description','ElevenLabs vehicle library 42. '+row['prompt']);slots[u.Name(row['slot'])]=slot
cat.set_editor_property('slots',slots);lib.save_loaded_asset(cat)
for row in rows:
 slot=cat.get_editor_property('slots')[u.Name(row['slot'])]
 sound=lib.load_asset('/Game/Audio/Vehicles42/'+row['slot'])
 assert sound and sound.get_editor_property('duration')>0, row['slot']+' empty sound'
 assert bool(sound.get_editor_property('looping'))==row['loop'], row['slot']+' wave loop mismatch'
 assert bool(slot.get_editor_property('loop'))==row['loop'], row['slot']+' catalog loop mismatch'
 assert len(slot.get_editor_property('tracks'))==1, row['slot']+' missing track'
u.log('VEHICLE42_AUDIO_IMPORT_DONE '+str(len(rows)))
