import unreal as u
lib=u.EditorAssetLibrary;ml=u.MaterialEditingLibrary
path='/Game/Materials/M_Blood22'
m=lib.load_asset(path) if lib.does_asset_exist(path) else u.AssetToolsHelpers.get_asset_tools().create_asset('M_Blood22','/Game/Materials',u.Material,u.MaterialFactoryNew())
ml.delete_all_material_expressions(m)
c=ml.create_material_expression(m,u.MaterialExpressionConstant3Vector);c.constant=u.LinearColor(.09,.001,.003,1);ml.connect_material_property(c,'',u.MaterialProperty.MP_BASE_COLOR)
r=ml.create_material_expression(m,u.MaterialExpressionConstant);r.r=.65;ml.connect_material_property(r,'',u.MaterialProperty.MP_ROUGHNESS)
ml.set_material_usage(m,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES)
m.set_editor_property('two_sided',True);ml.recompile_material(m);lib.save_loaded_asset(m)
u.log('AAM_V22_CONTENT_COMPLETE')
