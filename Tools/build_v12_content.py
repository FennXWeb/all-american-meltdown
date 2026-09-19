"""Import generated card atlas and native UV materials. Does not package."""
from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary;tools=u.AssetToolsHelpers.get_asset_tools();ml=u.MaterialEditingLibrary
path='/Game/Art/Textures/T_CardAtlasV12'
if not lib.does_asset_exist(path):
 t=u.AssetImportTask();t.filename=str(root/'ArtSource/Cards/T_CardAtlasV12.png');t.destination_path='/Game/Art/Textures';t.automated=True;t.save=True;tools.import_asset_tasks([t])
tex=lib.load_asset(path)
if not tex:raise RuntimeError('Card atlas import failed')
tex.set_editor_property('filter',u.TextureFilter.TF_TRILINEAR);tex.set_editor_property('mip_gen_settings',u.TextureMipGenSettings.TMGS_SIMPLE_AVERAGE);tex.set_editor_property('lod_group',u.TextureGroup.TEXTUREGROUP_UI);lib.save_loaded_asset(tex)
for name,cell in [('CardBackV12',1),('CardFeltV12',7),('CardCourtV12',4)]:
 dest='/Game/Materials/M_'+name
 if lib.does_asset_exist(dest):continue
 m=tools.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew());m.set_editor_property('two_sided',True)
 uv=ml.create_material_expression(m,u.MaterialExpressionTextureCoordinate);uv.set_editor_property('u_tiling',.246);uv.set_editor_property('v_tiling',.496)
 offset=ml.create_material_expression(m,u.MaterialExpressionConstant2Vector);offset.set_editor_property('r',(cell%4)*.25+.002);offset.set_editor_property('g',(cell//4)*.5+.002)
 add=ml.create_material_expression(m,u.MaterialExpressionAdd);ml.connect_material_expressions(uv,'',add,'A');ml.connect_material_expressions(offset,'',add,'B')
 sample=ml.create_material_expression(m,u.MaterialExpressionTextureSample);sample.texture=tex;ml.connect_material_expressions(add,'',sample,'UVs');ml.connect_material_property(sample,'RGB',u.MaterialProperty.MP_BASE_COLOR)
 rough=ml.create_material_expression(m,u.MaterialExpressionConstant);rough.r=.9;ml.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS)
 ml.recompile_material(m);lib.save_loaded_asset(m)
u.log('LW_V12_CONTENT_COMPLETE')
audio=lib.load_asset('/Game/Audio/DA_AudioCatalog');slots=audio.get_editor_property('slots')
for name in ['CardDeal','CardShuffle','CardChips']:
 dest='/Game/Audio/S_'+name
 if not lib.does_asset_exist(dest):
  task=u.AssetImportTask();task.filename=str(root/'ArtSource/Audio'/('S_'+name+'.wav'));task.destination_path='/Game/Audio';task.automated=True;task.save=True;tools.import_asset_tasks([task])
 if u.Name(name) not in slots:
  slot=u.LWAudioSlot();slot.set_editor_property('source',lib.load_asset(dest));slot.set_editor_property('volume',.65);slots[u.Name(name)]=slot
  audio.set_editor_property('slots',slots)
lib.save_loaded_asset(audio)
