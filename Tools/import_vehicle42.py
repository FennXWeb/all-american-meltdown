import unreal as u
from pathlib import Path
import json
root=Path(u.Paths.project_dir()).resolve();lib=u.EditorAssetLibrary;ml=u.MaterialEditingLibrary;tools=u.AssetToolsHelpers.get_asset_tools()
for f in sorted((root/'ArtSource/TexturesV42').glob('*.png')):
 t=u.AssetImportTask();t.filename=str(f);t.destination_path='/Game/Art/Textures';t.automated=True;t.replace_existing=True;t.save=True;tools.import_asset_tasks([t])
 tex=lib.load_asset('/Game/Art/Textures/'+f.stem);tex.set_editor_property('srgb',True);lib.save_loaded_asset(tex)
 name=f.stem[2:];path='/Game/Materials/M_'+name
 mat=lib.load_asset(path) if lib.does_asset_exist(path) else tools.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew())
 ml.delete_all_material_expressions(mat);mat.set_editor_property('two_sided',True);ml.set_material_usage(mat,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES)
 sample=ml.create_material_expression(mat,u.MaterialExpressionTextureSample);sample.texture=tex;ml.connect_material_property(sample,'RGB',u.MaterialProperty.MP_BASE_COLOR)
 rough=ml.create_material_expression(mat,u.MaterialExpressionScalarParameter);rough.set_editor_property('parameter_name','Roughness');rough.set_editor_property('default_value',.32 if 'Paint' in name else .18 if 'Chrome' in name else .5 if 'Metal' in name else .8);ml.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS)
 metal=ml.create_material_expression(mat,u.MaterialExpressionConstant);metal.r=.8 if 'Chrome' in name or 'Metal' in name else .35 if 'Paint' in name else 0;ml.connect_material_property(metal,'',u.MaterialProperty.MP_METALLIC)
 if 'Dial' in name or 'Lamp' in name or 'Amber' in name:
  gain=ml.create_material_expression(mat,u.MaterialExpressionScalarParameter);gain.set_editor_property('parameter_name','LampPower');gain.set_editor_property('default_value',.55 if 'Dial' in name else .08)
  mul=ml.create_material_expression(mat,u.MaterialExpressionMultiply);ml.connect_material_expressions(sample,'RGB',mul,'A');ml.connect_material_expressions(gain,'',mul,'B');ml.connect_material_property(mul,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 ml.recompile_material(mat);lib.save_loaded_asset(mat)
p='/Game/Materials/M_V42_Glass';mat=lib.load_asset(p) if lib.does_asset_exist(p) else tools.create_asset('M_V42_Glass','/Game/Materials',u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(mat)
mat.set_editor_property('blend_mode',u.BlendMode.BLEND_TRANSLUCENT);mat.set_editor_property('two_sided',True);mat.set_editor_property('translucency_lighting_mode',u.TranslucencyLightingMode.TLM_SURFACE_PER_PIXEL_LIGHTING)
c=ml.create_material_expression(mat,u.MaterialExpressionConstant3Vector);c.constant=u.LinearColor(.065,.11,.13,1);ml.connect_material_property(c,'',u.MaterialProperty.MP_BASE_COLOR)
for value,prop in [(.18,u.MaterialProperty.MP_OPACITY),(.13,u.MaterialProperty.MP_ROUGHNESS),(.3,u.MaterialProperty.MP_METALLIC)]:
 n=ml.create_material_expression(mat,u.MaterialExpressionConstant);n.r=value;ml.connect_material_property(n,'',prop)
ml.set_material_usage(mat,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES);ml.recompile_material(mat);lib.save_loaded_asset(mat)
exec(compile((root/'Tools/import_models_v5.py').read_text().replace('ModelsV5','ModelsV42').replace('models_v5_manifest','models_v42_manifest'),'import42','exec'),{'__name__':'__main__'})
u.log('VEHICLE42_IMPORT_DONE; run finish_vehicle42_lods.py in a full editor process')
