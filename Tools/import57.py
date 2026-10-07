"""Import generated atlas, validated meshes and metered ElevenLabs audio. Editor only."""
import unreal as u,json,sys
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary;tools=u.AssetToolsHelpers.get_asset_tools()
t=u.AssetImportTask();t.filename=str(root/'ArtSource/TexturesV57/T_Expansion57.png');t.destination_path='/Game/Art/Textures';t.automated=True;t.replace_existing=True;t.save=True;tools.import_asset_tasks([t]);tex=lib.load_asset('/Game/Art/Textures/T_Expansion57')
for name,xy in [('Armor57',(0,0)),('Fur57',(.5,0)),('Chitin57',(0,.5)),('Robot57',(.5,.5))]:
 path='/Game/Materials/M_'+name;mat=lib.load_asset(path) or tools.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew());u.MaterialEditingLibrary.delete_all_material_expressions(mat)
 uv=u.MaterialEditingLibrary.create_material_expression(mat,u.MaterialExpressionTextureCoordinate);uv.set_editor_property('u_tiling',.49);uv.set_editor_property('v_tiling',.49)
 offset=u.MaterialEditingLibrary.create_material_expression(mat,u.MaterialExpressionConstant2Vector);offset.set_editor_property('r',xy[0]+.005);offset.set_editor_property('g',xy[1]+.005)
 add=u.MaterialEditingLibrary.create_material_expression(mat,u.MaterialExpressionAdd);sample=u.MaterialEditingLibrary.create_material_expression(mat,u.MaterialExpressionTextureSample);sample.set_editor_property('texture',tex)
 u.MaterialEditingLibrary.connect_material_expressions(uv,'',add,'A');u.MaterialEditingLibrary.connect_material_expressions(offset,'',add,'B');u.MaterialEditingLibrary.connect_material_expressions(add,'',sample,'UVs');u.MaterialEditingLibrary.connect_material_property(sample,'RGB',u.MaterialProperty.MP_BASE_COLOR)
 rough=u.MaterialEditingLibrary.create_material_expression(mat,u.MaterialExpressionConstant);rough.set_editor_property('r',.84 if name=='Fur57' else .6);u.MaterialEditingLibrary.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS);u.MaterialEditingLibrary.recompile_material(mat);lib.save_loaded_asset(mat)
# Use existing strict FBX bounds / material-slot validation with a version-specific source.
source=(root/'Tools/import_models_v5.py').read_text().replace("'ModelsV5'","'ModelsV57'").replace('models_v5_manifest.json','models_v57_manifest.json')
exec(compile(source,'import_models57','exec'),{'__name__':'__main__'})
for row in json.loads((root/'ArtSource/ModelsV57/models_v57_manifest.json').read_text())['assets']:
 mesh=lib.load_asset(row['asset']);mesh.set_editor_property('allow_cpu_access',True);lib.save_loaded_asset(mesh)
source=root/'ArtSource/AudioV57';rows=json.loads((source/'manifest.json').read_text())['sounds'];assert len(rows)==42 and all((source/r['file']).exists() for r in rows)
cat=lib.load_asset('/Game/Audio/DA_AudioCatalog');slots=dict(cat.get_editor_property('slots'));groups={}
for row in rows:
 task=u.AssetImportTask();task.filename=str(source/row['file']);task.destination_path='/Game/Audio/Expansion57';task.automated=True;task.replace_existing=True;task.save=True;tools.import_asset_tasks([task]);sound=lib.load_asset('/Game/Audio/Expansion57/'+row['slot']);assert sound
 sound.set_editor_property('looping',row.get('loop',False));lib.save_loaded_asset(sound);groups.setdefault(row.get('group',row['slot']),[]).append((sound,row))
for name,entries in groups.items():
 tracks=[x[0] for x in entries];slot=slots.get(u.Name(name),u.LWAudioSlot());slot.set_editor_property('source',tracks[0]);slot.set_editor_property('tracks',tracks);slot.set_editor_property('loop',entries[0][1].get('loop',False));slot.set_editor_property('music',False);slot.set_editor_property('attenuation',u.LWAudioAttenuation.SPATIAL);slot.set_editor_property('description',f'Expansion57 / {len(tracks)} ElevenLabs variation(s)');slots[u.Name(name)]=slot
cat.set_editor_property('slots',slots);lib.save_loaded_asset(cat)
u.log('EXPANSION57_IMPORT_DONE models=41 clips=42 materials=4')
