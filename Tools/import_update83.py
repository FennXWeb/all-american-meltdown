"""Neutral factory paint for the electric motorhome. No model/audio reimport."""
import unreal as u
lib=u.EditorAssetLibrary
ml=u.MaterialEditingLibrary
name='M_EV83_CoachPearl'
path='/Game/Materials/'+name
mat=lib.load_asset(path) if lib.does_asset_exist(path) else u.AssetToolsHelpers.get_asset_tools().create_asset(name,'/Game/Materials',u.Material,u.MaterialFactoryNew())
ml.delete_all_material_expressions(mat)
color=ml.create_material_expression(mat,u.MaterialExpressionVectorParameter)
color.set_editor_property('parameter_name','PaintTint')
color.set_editor_property('default_value',u.LinearColor(.72,.74,.75,1))
ml.connect_material_property(color,'RGB',u.MaterialProperty.MP_BASE_COLOR)
rough=ml.create_material_expression(mat,u.MaterialExpressionScalarParameter)
rough.set_editor_property('parameter_name','Roughness')
rough.set_editor_property('default_value',.38)
ml.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS)
# Painted composite panels are dielectric, not exposed tinted metal. Keep glass,
# chrome, solar panels and the player's purchased garage finishes independent.
for prop,value in [(u.MaterialProperty.MP_METALLIC,0),(u.MaterialProperty.MP_SPECULAR,.25)]:
 node=ml.create_material_expression(mat,u.MaterialExpressionConstant)
 node.set_editor_property('r',value)
 ml.connect_material_property(node,'',prop)
mat.set_editor_property('two_sided',True)
ml.set_material_usage(mat,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES)
ml.recompile_material(mat)
lib.save_loaded_asset(mat)
u.log('UPDATE83_PAINT_SAVED '+path)
u.SystemLibrary.quit_editor()
