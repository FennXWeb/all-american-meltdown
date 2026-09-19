"""Import individually generated collector artwork. Does not cook or package."""
from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve();src=root/'ArtSource/TradingCards36';lib=u.EditorAssetLibrary;ml=u.MaterialEditingLibrary
files=[src/f'T_TradingCard{i:03}36.png' for i in range(1,101)]
partial='TradingCardsPartial' in u.SystemLibrary.get_command_line()
missing=[p.name for p in files if not p.is_file()]
if missing and not partial:raise RuntimeError('Missing card artwork: '+', '.join(missing))
textures=[]
for f in files:
 if not f.is_file():continue
 task=u.AssetImportTask();task.filename=str(f);task.destination_path='/Game/Art/Textures';task.automated=True;task.replace_existing=True;task.save=True
 u.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task]);tex=lib.load_asset('/Game/Art/Textures/'+f.stem);tex.set_editor_property('max_texture_size',1024);tex.set_editor_property('never_stream',False);lib.save_loaded_asset(tex);textures.append(tex)
if not textures:raise RuntimeError('No artwork available')
for name,art in [('M_TradingCard36',True),('M_TradingCardFrame36',False)]:
 path='/Game/Materials/'+name;mat=lib.load_asset(path) if lib.does_asset_exist(path) else u.AssetToolsHelpers.get_asset_tools().create_asset(name,'/Game/Materials',u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(mat);mat.set_editor_property('two_sided',True)
 if art:
  base=ml.create_material_expression(mat,u.MaterialExpressionTextureSampleParameter2D);base.set_editor_property('parameter_name','CardArt');base.texture=textures[0];pin='RGB'
 else:
  base=ml.create_material_expression(mat,u.MaterialExpressionVectorParameter);base.set_editor_property('parameter_name','Tint');base.set_editor_property('default_value',u.LinearColor(.75,.72,.6,1));pin=''
 ml.connect_material_property(base,pin,u.MaterialProperty.MP_BASE_COLOR)
 rough=ml.create_material_expression(mat,u.MaterialExpressionConstant);rough.r=.86;ml.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS)
 glow=ml.create_material_expression(mat,u.MaterialExpressionMultiply);ml.connect_material_expressions(base,pin,glow,'A');power=ml.create_material_expression(mat,u.MaterialExpressionConstant);power.r=.07;ml.connect_material_expressions(power,'',glow,'B');ml.connect_material_property(glow,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 ml.recompile_material(mat);lib.save_loaded_asset(mat)
u.log('TRADING_CARDS36_IMPORTED '+str(len(textures)))
