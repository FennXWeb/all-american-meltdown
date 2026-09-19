import unreal as u
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary;tools=u.AssetToolsHelpers.get_asset_tools();ml=u.MaterialEditingLibrary
files=[(root/'ArtSource/Branding15/T_AllAmericanMeltdown.png','/Game/Art/Textures')]+[(p,'/Game/Audio') for p in (root/'ArtSource/AudioV15').glob('*.wav')]
for p,d in files:
 t=u.AssetImportTask();t.filename=str(p);t.destination_path=d;t.automated=True;t.replace_existing=True;t.save=True;tools.import_asset_tasks([t])
tex=lib.load_asset('/Game/Art/Textures/T_AllAmericanMeltdown');tex.set_editor_property('compression_settings',u.TextureCompressionSettings.TC_EDITOR_ICON);tex.set_editor_property('mip_gen_settings',u.TextureMipGenSettings.TMGS_NO_MIPMAPS);tex.set_editor_property('lod_group',u.TextureGroup.TEXTUREGROUP_UI);lib.save_loaded_asset(tex)
if not lib.does_asset_exist('/Game/Materials/M_SurvivorTint'):
 m=tools.create_asset('M_SurvivorTint','/Game/Materials',u.Material,u.MaterialFactoryNew());v=ml.create_material_expression(m,u.MaterialExpressionVectorParameter);v.set_editor_property('parameter_name','Tint');v.set_editor_property('default_value',u.LinearColor(.4,.25,.15,1));ml.connect_material_property(v,'',u.MaterialProperty.MP_BASE_COLOR);r=ml.create_material_expression(m,u.MaterialExpressionConstant);r.r=.8;ml.connect_material_property(r,'',u.MaterialProperty.MP_ROUGHNESS);ml.recompile_material(m);lib.save_loaded_asset(m)
for name,col,glow in [('IntroFire',u.LinearColor(1,.18,.015,1),True),('IntroSmoke',u.LinearColor(.065,.055,.045,1),False)]:
 if lib.does_asset_exist('/Game/Materials/M_'+name):continue
 m=tools.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew());v=ml.create_material_expression(m,u.MaterialExpressionConstant3Vector);v.constant=col
 ml.connect_material_property(v,'',u.MaterialProperty.MP_BASE_COLOR)
 if glow:ml.connect_material_property(v,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 ml.recompile_material(m);lib.save_loaded_asset(m)
c=lib.load_asset('/Game/Audio/DA_AudioCatalog');slots=dict(c.get_editor_property('slots'))
for name in ['IntroArchive','IntroUnrest','IntroAlarm','IntroBlast','IntroAftermath','IntroShelter']:
 if u.Name(name) in slots:continue
 s=u.LWAudioSlot();s.set_editor_property('source',lib.load_asset('/Game/Audio/S_'+name));s.set_editor_property('music',True);s.set_editor_property('volume',.6);s.set_editor_property('description','Opening cinematic cue. Replace Source or add Tracks. Timed to an 18 second scene.');slots[u.Name(name)]=s
c.set_editor_property('slots',slots);lib.save_loaded_asset(c)
src=(root/'Tools/import_models_v5.py').read_text(encoding='utf-8').replace('ModelsV5','ModelsV15').replace('models_v5_manifest','models_v15_manifest');exec(compile(src,'import_v15','exec'))
u.log('AAM_V15_CONTENT_COMPLETE')

