import unreal as u
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary;ml=u.MaterialEditingLibrary;tools=u.AssetToolsHelpers.get_asset_tools()
for name,color,rough in [('CanadaArmor51',(.62,.67,.64),.46),('CanadaRed51',(.48,.035,.035),.55),('CanadaLeaf51',(.11,.34,.085),.9),('CanadaGrass51',(.12,.29,.075),.96)]:
 path='/Game/Materials/M_'+name
 mat=lib.load_asset(path) if lib.does_asset_exist(path) else tools.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew())
 ml.delete_all_material_expressions(mat)
 tint=ml.create_material_expression(mat,u.MaterialExpressionConstant3Vector);tint.constant=u.LinearColor(*color,1)
 # Existing grain texture gives the new palette surface detail without the rust patches of the wasteland materials.
 tex=ml.create_material_expression(mat,u.MaterialExpressionTextureSample);tex.texture=lib.load_asset('/Game/Art/Textures/T_Concrete')
 if tex.texture and name not in ['CanadaLeaf51','CanadaGrass51']:
  des=ml.create_material_expression(mat,u.MaterialExpressionDesaturation);ml.connect_material_expressions(tex,'RGB',des,'Input')
  mul=ml.create_material_expression(mat,u.MaterialExpressionMultiply);ml.connect_material_expressions(tint,'',mul,'A');ml.connect_material_expressions(des,'',mul,'B');ml.connect_material_property(mul,'',u.MaterialProperty.MP_BASE_COLOR)
 else:ml.connect_material_property(tint,'',u.MaterialProperty.MP_BASE_COLOR)
 r=ml.create_material_expression(mat,u.MaterialExpressionConstant);r.r=rough;ml.connect_material_property(r,'',u.MaterialProperty.MP_ROUGHNESS)
 mat.set_editor_property('two_sided',name=='CanadaLeaf51');ml.set_material_usage(mat,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES);ml.recompile_material(mat);lib.save_loaded_asset(mat)
source=(root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','ModelsV51').replace('models_v5_manifest','models_v51_manifest')
exec(compile(source,'border51_import','exec'),{'__name__':'__main__'})
for name in ['MenuDistant51','MenuForeground51']:
 task=u.AssetImportTask();task.filename=str(root/'ArtSource/MenuV51'/f'{name}.png');task.destination_path='/Game/Art/Menu51';task.destination_name=name;task.automated=True;task.replace_existing=True;task.save=True;tools.import_asset_tasks([task]);tex=lib.load_asset('/Game/Art/Menu51/'+name);tex.set_editor_property('compression_settings',u.TextureCompressionSettings.TC_EDITOR_ICON);tex.set_editor_property('lod_group',u.TextureGroup.TEXTUREGROUP_UI);tex.set_editor_property('never_stream',True);lib.save_loaded_asset(tex)
u.log('UPDATE51_IMPORTED');u.SystemLibrary.quit_editor()
