"""Import the 35 character wardrobe, customization materials and vertex masks. No packaging."""
import json
from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve();src=root/'ArtSource/Characters35';lib=u.EditorAssetLibrary;ml=u.MaterialEditingLibrary
for name in ['T_CharacterMaterials35','T_Tattoos35']:
 task=u.AssetImportTask();task.filename=str(src/(name+'.png'));task.destination_path='/Game/Art/Textures';task.automated=True;task.save=True;task.replace_existing=True
 u.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task]);tex=lib.load_asset('/Game/Art/Textures/'+name);tex.set_editor_property('max_texture_size',2048);tex.set_editor_property('filter',u.TextureFilter.TF_BILINEAR);lib.save_loaded_asset(tex)
atlas=lib.load_asset('/Game/Art/Textures/T_CharacterMaterials35');tattoos=lib.load_asset('/Game/Art/Textures/T_Tattoos35')
data=json.loads((src/'models_35_manifest.json').read_text())
for key,(tile,color) in data['materials'].items():
 name='M_'+key;path='/Game/Materials/'+name
 mat=lib.load_asset(path) if lib.does_asset_exist(path) else u.AssetToolsHelpers.get_asset_tools().create_asset(name,'/Game/Materials',u.Material,u.MaterialFactoryNew())
 ml.delete_all_material_expressions(mat);mat.set_editor_property('two_sided',True)
 def node(kind):return ml.create_material_expression(mat,kind)
 def link(a,p,b,q):
  if not ml.connect_material_expressions(a,p,b,q):raise RuntimeError('Invalid material connection '+str(type(b))+' '+q)
 tint=node(u.MaterialExpressionVectorParameter);tint.set_editor_property('parameter_name','Tint');tint.set_editor_property('default_value',u.LinearColor(1,1,1,1))
 if tile>=0:
  uv=node(u.MaterialExpressionTextureCoordinate);uv.u_tiling=3 if key=='Hair35' else 4;uv.v_tiling=1 if key=='Hair35' else 4
  repeat=node(u.MaterialExpressionFrac);link(uv,'',repeat,'')
  cell=node(u.MaterialExpressionConstant2Vector);cell.r=.48;cell.g=.23
  tileuv=node(u.MaterialExpressionMultiply);link(repeat,'',tileuv,'A');link(cell,'',tileuv,'B')
  offset=node(u.MaterialExpressionConstant2Vector);offset.r=(tile%2)*.5+.01;offset.g=(tile//2)*.25+.01
  add=node(u.MaterialExpressionAdd);link(tileuv,'',add,'A');link(offset,'',add,'B')
  sample=node(u.MaterialExpressionTextureSample);sample.texture=atlas;link(add,'',sample,'UVs');base=sample;pin='RGB'
  if key=='Skin35':
   gray=node(u.MaterialExpressionDotProduct);link(sample,'RGB',gray,'A');weights=node(u.MaterialExpressionConstant3Vector);weights.constant=u.LinearColor(.299,.587,.114,1);link(weights,'',gray,'B');base=gray;pin=''
 elif tile<=-2:
  fuv=node(u.MaterialExpressionTextureCoordinate);fuv.u_tiling=.498;fuv.v_tiling=.498
  off=node(u.MaterialExpressionConstant2Vector);off.r=.001 if tile==-2 else .501;off.g=.001
  add=node(u.MaterialExpressionAdd);link(fuv,'',add,'A');link(off,'',add,'B')
  fs=node(u.MaterialExpressionTextureSample);fs.texture=lib.load_asset('/Game/Art/Textures/T_CharacterFaces32');link(add,'',fs,'UVs')
  gray=node(u.MaterialExpressionDotProduct);link(fs,'RGB',gray,'A');weights=node(u.MaterialExpressionConstant3Vector);weights.constant=u.LinearColor(.299,.587,.114,1);link(weights,'',gray,'B')
  level=node(u.MaterialExpressionConstant);level.r=.36
  base=node(u.MaterialExpressionLinearInterpolate);link(level,'',base,'A');link(gray,'',base,'B');mix=node(u.MaterialExpressionConstant);mix.r=.28;link(mix,'',base,'Alpha');pin=''
 else:
  base=node(u.MaterialExpressionConstant3Vector);base.constant=u.LinearColor(*color,1);pin=''
 if key=='Skin35' or key.startswith('Face'):
  # Identical neutral endpoint on skin and face avoids a hard ear/neck colour boundary.
  neutral=node(u.MaterialExpressionConstant);neutral.r=.36
  blend=node(u.MaterialExpressionLinearInterpolate);link(neutral,'',blend,'A');link(base,pin,blend,'B')
  if key.startswith('Face'):
   coverage=node(u.MaterialExpressionVertexColor);link(coverage,'R',blend,'Alpha')
  else:
   micro=node(u.MaterialExpressionConstant);micro.r=.06;link(micro,'',blend,'Alpha')
  base=blend;pin=''
 if key=='Hair35':
  lift=node(u.MaterialExpressionConstant);lift.r=.3
  detail=node(u.MaterialExpressionAdd);link(base,pin,detail,'A');link(lift,'',detail,'B');base=detail;pin=''
 result=node(u.MaterialExpressionMultiply);link(base,pin,result,'A');link(tint,'',result,'B')
 if key=='Skin35' or key.startswith('Face'):
  vertex=node(u.MaterialExpressionVertexColor)
  guv=node(u.MaterialExpressionTextureCoordinate);guv.coordinate_index=1
  ink=node(u.MaterialExpressionTextureSample);ink.texture=tattoos;link(guv,'',ink,'UVs')
  opacity=node(u.MaterialExpressionScalarParameter);opacity.set_editor_property('parameter_name','TattooOpacity');opacity.set_editor_property('default_value',.85)
  mask=node(u.MaterialExpressionMultiply);link(ink,'A',mask,'A');link(vertex,'B',mask,'B')
  alpha=node(u.MaterialExpressionMultiply);link(mask,'',alpha,'A');link(opacity,'',alpha,'B')
  inkcolor=node(u.MaterialExpressionConstant3Vector);inkcolor.constant=u.LinearColor(.013,.011,.009,1)
  tattoo=node(u.MaterialExpressionLinearInterpolate);link(result,'',tattoo,'A');link(inkcolor,'',tattoo,'B');link(alpha,'',tattoo,'Alpha')
  glove=node(u.MaterialExpressionVectorParameter);glove.set_editor_property('parameter_name','GloveTint');glove.set_editor_property('default_value',u.LinearColor(.06,.05,.04,1))
  amount=node(u.MaterialExpressionScalarParameter);amount.set_editor_property('parameter_name','GloveAmount');amount.set_editor_property('default_value',0)
  coverage=node(u.MaterialExpressionMultiply);link(vertex,'G',coverage,'A');link(amount,'',coverage,'B')
  result=node(u.MaterialExpressionLinearInterpolate);link(tattoo,'',result,'A');link(glove,'',result,'B');link(coverage,'',result,'Alpha')
 ml.connect_material_property(result,'',u.MaterialProperty.MP_BASE_COLOR)
 rough=node(u.MaterialExpressionConstant);rough.r=.28 if key in ['Eye35','Iris35'] else .6 if key=='Skin35' else .83;ml.connect_material_property(rough,'',u.MaterialProperty.MP_ROUGHNESS)
 spec=node(u.MaterialExpressionConstant);spec.r=.2;ml.connect_material_property(spec,'',u.MaterialProperty.MP_SPECULAR)
 ml.recompile_material(mat);lib.save_loaded_asset(mat)
stagepath='/Game/Materials/M_CreatorBackdrop35'
stage=lib.load_asset(stagepath) if lib.does_asset_exist(stagepath) else u.AssetToolsHelpers.get_asset_tools().create_asset('M_CreatorBackdrop35','/Game/Materials',u.Material,u.MaterialFactoryNew())
ml.delete_all_material_expressions(stage)
c=ml.create_material_expression(stage,u.MaterialExpressionConstant3Vector);c.constant=u.LinearColor(.055,.064,.066,1);ml.connect_material_property(c,'',u.MaterialProperty.MP_BASE_COLOR)
r=ml.create_material_expression(stage,u.MaterialExpressionConstant);r.r=1;ml.connect_material_property(r,'',u.MaterialProperty.MP_ROUGHNESS)
ml.recompile_material(stage);lib.save_loaded_asset(stage)
script=(root/'Tools/import_models_v5.py').read_text().replace("'ModelsV5'","'Characters35'").replace('models_v5_manifest','models_35_manifest')
script=script.replace('        task = u.AssetImportTask()', "        existing = LIB.load_asset(DESTINATION + '/SM_' + record['name']) if LIB.does_asset_exist(DESTINATION + '/SM_' + record['name']) else None\n        if existing and sorted(str(slot.get_editor_property('imported_material_slot_name')) for slot in existing.get_editor_property('static_materials')) != sorted(record['material_slots']):\n            existing.set_editor_property('static_materials', [])\n        task = u.AssetImportTask()")
script=script.replace('data.combine_meshes = True','data.combine_meshes = True\n        data.vertex_color_import_option = u.VertexColorImportOption.REPLACE')
exec(compile(script,'characters35_import','exec'),{'__name__':'__main__'})
for record in data['assets']:
 mesh=lib.load_asset(record['asset']);mesh.set_editor_property('allow_cpu_access',True);lib.save_loaded_asset(mesh)
u.log('CHARACTERS35_IMPORTED '+str(len(data['assets'])))
