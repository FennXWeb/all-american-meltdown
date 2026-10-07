import unreal as u
import json,hashlib
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve()
source=root/'ArtSource/Dialogue59'
registry=json.loads((root/'Data/VoiceProfiles59.json').read_text(encoding='utf-8'))
rows={}
for batch in sorted(source.glob('*.json')):
    data=json.loads(batch.read_text(encoding='utf-8'))
    for row in data.get('lines',[]):
        assert row['key'] not in rows or rows[row['key']]==row,'Conflicting dialogue keys'
        rows[row['key']]=row
manifest=list(rows.values())
usage=json.loads((source/'usage.json').read_text(encoding='utf-8'))
assert len({p['id'] for p in registry['profiles']})==len(registry['profiles'])
assert all(p['voice_id'] and p['model_id'] for p in registry['profiles'])
assert all(p in {v['id'] for v in registry['profiles']} for p in registry.get('active_procedural_profiles',[]))
assert all(usage.get(r['key'],{}).get('state')=='complete' and (source/r['file']).exists() for r in manifest),'Incomplete generation; keep old library active'
lib=u.EditorAssetLibrary;tools=u.AssetToolsHelpers.get_asset_tools()
folder='/Game/Audio/Dialogue59'
cat=lib.load_asset(folder+'/DA_Dialogue59') if lib.does_asset_exist(folder+'/DA_Dialogue59') else None
if not cat:
    factory=u.DataAssetFactory();factory.set_editor_property('data_asset_class',u.LWDialogueCatalog59)
    cat=tools.create_asset('DA_Dialogue59',folder,u.LWDialogueCatalog59,factory)
profiles={}
for p in registry['profiles']:
    value=u.LWVoiceProfile59();value.set_editor_property('display_name',p['name']);value.set_editor_property('eleven_labs_voice_id',p['voice_id']);value.set_editor_property('female',p['female']);value.set_editor_property('model',p['model_id']);profiles[u.Name(p['id'])]=value
lines=dict(cat.get_editor_property('lines'))
stamp=root/'Saved/Dialogue59Imported.json';hashes=json.loads(stamp.read_text(encoding='utf-8')) if stamp.exists() else {}
for r in manifest:
    path=folder+'/'+r['key']
    digest=hashlib.sha256((source/r['file']).read_bytes()).hexdigest()
    changed=not lib.does_asset_exist(path) or hashes.get(r['key'])!=digest
    if changed:
        task=u.AssetImportTask();task.filename=str(source/r['file']);task.destination_path=folder;task.automated=True;task.replace_existing=True;task.save=True;tools.import_asset_tasks([task])
    hashes[r['key']]=digest
    wave=lib.load_asset(path);assert wave and wave.get_editor_property('duration')>0
    if changed:
        wave.set_editor_property('looping',False)
        wave.set_editor_property('enable_amplitude_envelope_analysis',True)
        lib.save_loaded_asset(wave)
    value=u.LWSpokenLine59();value.set_editor_property('text',r['text']);value.set_editor_property('profile',u.Name(r['profile']));value.set_editor_property('audio',wave);value.set_editor_property('duration',wave.get_editor_property('duration'));lines[r['key']]=value
cat.set_editor_property('profiles',profiles);cat.set_editor_property('lines',lines);cat.set_editor_property('cast',{u.Name(k):u.Name(v) for k,v in registry['cast'].items()})
active=registry.get('active_procedural_profiles',[])
cat.set_editor_property('procedural_profiles75',[u.Name(p) for p in active])
for gender,prop in [(False,'procedural_male75'),(True,'procedural_female75')]:
    matches=[p for p in registry['profiles'] if p['id'] in active and p['female']==gender]
    cat.set_editor_property(prop,u.Name(matches[0]['id'] if matches else 'None'))
cat.set_editor_property('procedural_replacements75',{u.Name(k):u.Name(v) for k,v in registry.get('procedural_replacements',{}).items()})
cat.set_editor_property('enabled',True);lib.save_loaded_asset(cat)
stamp.write_text(json.dumps(hashes,indent=2))
assert len(cat.get_editor_property('lines'))>=len(manifest)
u.log('DIALOGUE59_IMPORT_DONE profiles='+str(len(profiles))+' clips='+str(len(lines)))
