"""Import revision 63 models, shared creature atlas and corrected iris material."""
import unreal as u,json,os
from pathlib import Path
root=Path('X:/LethalWorld');src=root/'ArtSource/Models63';data=json.loads((src/'manifest.json').read_text());dest='/Game/Art/Models63';lib=u.EditorAssetLibrary;ml=u.MaterialEditingLibrary;at=u.AssetToolsHelpers.get_asset_tools();lib.make_directory(dest)
t=u.AssetImportTask();t.filename=str(src/'T_Surface63.png');t.destination_path=dest;t.automated=True;t.save=True;t.replace_existing=True;at.import_asset_tasks([t]);atlas=lib.load_asset(dest+'/T_Surface63')
material={};tiles={'Skin':0,'Fur':1,'Hide':2,'Chitin':3,'Metal':4,'Cloth':5,'Bone':6,'Polymer':7}
for key in list(tiles)+['Steel','Rubber','Lens','Glow','Iris']:
 name='M_'+key+'63';m=lib.load_asset(dest+'/'+name) if lib.does_asset_exist(dest+'/'+name) else at.create_asset(name,dest,u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(m)
 def node(c):return ml.create_material_expression(m,c)
 def link(a,b,p='',out=''):
  if not ml.connect_material_expressions(a,out,b,p):raise RuntimeError('Invalid material pin '+p)
 def c(x):n=node(u.MaterialExpressionConstant);n.r=x;return n
 def v(x):n=node(u.MaterialExpressionConstant3Vector);n.constant=u.LinearColor(*x,1);return n
 def v2(x,y):n=node(u.MaterialExpressionConstant2Vector);n.r=x;n.g=y;return n
 def op(cls,a,b):n=node(cls);link(a,n,'A');link(b,n,'B');return n
 if key in tiles:
  tile=tiles[key];uv=node(u.MaterialExpressionTextureCoordinate);frac=node(u.MaterialExpressionFrac);link(uv,frac);mapped=op(u.MaterialExpressionAdd,op(u.MaterialExpressionMultiply,frac,v2(.244,.488)),v2(tile%4*.25+.003,tile//4*.5+.006));tex=node(u.MaterialExpressionTextureSample);tex.texture=atlas;link(mapped,tex,'UVs');base=tex
 elif key=='Iris':
  # Iris UVs address an actual iris crop, not an entire portrait. UE texture V points down.
  uv=node(u.MaterialExpressionTextureCoordinate);mapped=op(u.MaterialExpressionAdd,op(u.MaterialExpressionMultiply,uv,v2(.05,.05)),v2(.321,.417));tex=node(u.MaterialExpressionTextureSample);tex.texture=lib.load_asset('/Game/Art/Characters61/T_FaceMale61');link(mapped,tex,'UVs');base=tex
 else:base=v({'Steel':(.32,.36,.4),'Rubber':(.018,.019,.022),'Lens':(.025,.16,.21),'Glow':(2,.05,.012)}[key])
 tint=node(u.MaterialExpressionVectorParameter);tint.set_editor_property('parameter_name','Tint');tint.set_editor_property('default_value',u.LinearColor(1,1,1,1));base=op(u.MaterialExpressionMultiply,base,tint);ml.connect_material_property(base,'',u.MaterialProperty.MP_BASE_COLOR)
 ml.connect_material_property(c(.28 if key in ['Steel','Metal','Lens','Iris'] else .79),'',u.MaterialProperty.MP_ROUGHNESS)
 if key in ['Steel','Metal']:ml.connect_material_property(c(.65),'',u.MaterialProperty.MP_METALLIC)
 if key=='Glow':ml.connect_material_property(base,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 m.set_editor_property('two_sided',key in ['Fur','Hide']);ml.recompile_material(m);lib.save_loaded_asset(m);material[name]=m
errors=[]
for r in data['assets']:
 if os.environ.get('LW63_WAISTS') and not ('Waist' in r['name'] or r['name']=='TitanPelvis32'):continue
 if os.environ.get('LW63_GROUP') and r['group']!=os.environ['LW63_GROUP']:continue
 t=u.AssetImportTask();t.filename=str(src/(r['name']+'.fbx'));t.destination_path=dest;t.destination_name='SM_'+r['name'];t.automated=True;t.replace_existing=True;t.replace_existing_settings=True;t.save=False
 opts=u.FbxImportUI();opts.import_mesh=True;opts.import_materials=False;opts.import_textures=False;opts.import_as_skeletal=False;opts.mesh_type_to_import=u.FBXImportType.FBXIT_STATIC_MESH;opts.automated_import_should_detect_type=False;d=opts.static_mesh_import_data;d.combine_meshes=True;d.auto_generate_collision=r['group']!='attachment';d.generate_lightmap_u_vs=False;d.convert_scene=True;d.convert_scene_unit=True;d.normal_import_method=u.FBXNormalImportMethod.FBXNIM_IMPORT_NORMALS;d.vertex_color_import_option=u.VertexColorImportOption.REPLACE;t.options=opts;t.factory=u.FbxFactory();at.import_asset_tasks([t]);mesh=lib.load_asset(dest+'/SM_'+r['name'])
 if not mesh:errors.append(r['name']);continue
 for i,s in enumerate(mesh.get_editor_property('static_materials')):
  name=str(s.get_editor_property('imported_material_slot_name'));mat=material.get(name)
  if name=='M_Iris61':mat=material['M_Iris63']
  if not mat and name.endswith('61') and lib.does_asset_exist('/Game/Art/Characters61/'+name):mat=lib.load_asset('/Game/Art/Characters61/'+name)
  if not mat:errors.append(r['name']+' material '+name)
  else:mesh.set_material(i,mat)
 b=mesh.get_bounding_box();actual=[b.min.x,b.min.y,b.min.z,b.max.x,b.max.y,b.max.z];expected=[v*100 for v in r['bounds']['min']+r['bounds']['max']]
 if max(abs(a-b) for a,b in zip(actual,expected))>.1:errors.append(r['name']+' bounds')
 mesh.set_editor_property('allow_cpu_access',r['group']!='attachment');lib.set_metadata_tag(mesh,'VisualRevision','63');lib.save_loaded_asset(mesh)
if errors:raise RuntimeError(str(errors))
u.log('MODELS63_IMPORT_PASS '+str(len(data['assets'])))
