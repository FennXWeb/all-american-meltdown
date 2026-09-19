import unreal as u
lib=u.MaterialEditingLibrary
colors={"RoadGreen33":(.025,.16,.08),"RoadRed33":(.5,.018,.012),"RoadOchre33":(.7,.42,.035),"RoadBlack33":(.018,.021,.02),"TrafficOff33":(.025,.027,.021),"TrafficRed33":(1,.012,.005),"TrafficAmber33":(1,.45,.005),"TrafficGreen33":(.008,1,.09)}
for name,color in colors.items():
 path='/Game/Materials/M_'+name
 mat=u.load_asset(path) or u.AssetToolsHelpers.get_asset_tools().create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew())
 lib.delete_all_material_expressions(mat)
 c=lib.create_material_expression(mat,u.MaterialExpressionConstant3Vector);c.set_editor_property('constant',u.LinearColor(*color))
 lib.connect_material_property(c,'',u.MaterialProperty.MP_BASE_COLOR)
 if name.startswith('Traffic') and name!='TrafficOff33':
  k=lib.create_material_expression(mat,u.MaterialExpressionConstant);k.set_editor_property('r',4)
  mul=lib.create_material_expression(mat,u.MaterialExpressionMultiply);lib.connect_material_expressions(c,'',mul,'A');lib.connect_material_expressions(k,'',mul,'B');lib.connect_material_property(mul,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 r=lib.create_material_expression(mat,u.MaterialExpressionConstant);r.set_editor_property('r',.72);lib.connect_material_property(r,'',u.MaterialProperty.MP_ROUGHNESS)
 if name=='RoadRed33':mat.set_editor_property('two_sided',True)
 lib.recompile_material(mat);u.EditorAssetLibrary.save_loaded_asset(mat)
u.log('ROAD33_MATERIALS_COMPLETE 8')
