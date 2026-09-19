"""Create encounter catalog and add new audio slots. Preserve designer edits; never package."""
from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary;tools=u.AssetToolsHelpers.get_asset_tools()
cls=u.LWEncounterCatalog
if not lib.does_asset_exist('/Game/Data/DA_Encounters'):
 factory=u.DataAssetFactory();factory.set_editor_property('data_asset_class',cls)
 catalog=tools.create_asset('DA_Encounters','/Game/Data',cls,factory)
 if not catalog:raise RuntimeError('Could not create encounter catalog')
 lib.save_loaded_asset(catalog)
catalog=lib.load_asset('/Game/Data/DA_Encounters');events=catalog.get_editor_property('events')
if len(events)<50:raise RuntimeError('Expected at least 50 encounter definitions')
audio=lib.load_asset('/Game/Audio/DA_AudioCatalog');slots=audio.get_editor_property('slots')
for name in ['RadioStatic','EncounterWarning','EncounterResolved']:
 path='/Game/Audio/S_'+name
 if not lib.does_asset_exist(path):
  task=u.AssetImportTask();task.filename=str(root/'ArtSource/Audio'/('S_'+name+'.wav'));task.destination_path='/Game/Audio';task.automated=True;task.save=True;tools.import_asset_tasks([task])
 if u.Name(name) not in slots:
  slot=u.LWAudioSlot();slot.set_editor_property('source',lib.load_asset(path));slot.set_editor_property('volume',.65);slot.set_editor_property('attenuation',u.LWAudioAttenuation.SPATIAL);slots[u.Name(name)]=slot
  audio.set_editor_property('slots',slots)
lib.save_loaded_asset(audio)
u.log('LW_V10_CONTENT_COMPLETE events='+str(len(events)))
