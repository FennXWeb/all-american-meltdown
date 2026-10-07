import unreal as u,json,math
from pathlib import Path
src=Path('X:/LethalWorld/ArtSource/Arsenal62');records=json.loads((src/'manifest.json').read_text());lib=u.EditorAssetLibrary;at=u.AssetToolsHelpers.get_asset_tools();ml=u.MaterialEditingLibrary
for d in ['/Game/Art/Textures','/Game/Art/Meshes','/Game/Materials']:lib.make_directory(d)
t=u.AssetImportTask();t.filename=str(src/'T_MysticAtlas62.png');t.destination_path='/Game/Art/Textures';t.destination_name='T_MysticAtlas62';t.automated=True;t.replace_existing=True;t.save=True;at.import_asset_tasks([t]);tex=lib.load_asset('/Game/Art/Textures/T_MysticAtlas62');tex.set_editor_property('filter',u.TextureFilter.TF_TRILINEAR);lib.save_loaded_asset(tex)
colors=[(1,.11,.008),(.1,1,.005),(.48,.025,1),(.03,.55,1),(.38,.003,.007),(.01,.12,1),(1,.55,.025),(.02,.8,.55)]
mats={}
for name in [f'Mystic62_{i}' for i in range(8)]+[f'MysticFX62_{i}' for i in range(8)]+[f'MysticMoving62_{i}' for i in range(8)]+['MysticWire62','Camo62_7','Camo62_8']:
 m=(lib.load_asset('/Game/Materials/M_'+name) if lib.does_asset_exist('/Game/Materials/M_'+name) else None) or at.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(m)
 def node(c):return ml.create_material_expression(m,c)
 def conn(a,b,p='',out=''):
  if not ml.connect_material_expressions(a,out,b,p):raise RuntimeError('Cannot connect '+str(a.get_class().get_name())+' to '+str(b.get_class().get_name())+' pin '+p)
 def c(v):n=node(u.MaterialExpressionConstant);n.set_editor_property('r',v);return n
 def v(col):n=node(u.MaterialExpressionConstant3Vector);n.set_editor_property('constant',u.LinearColor(*col,1));return n
 def v2(x,y):n=node(u.MaterialExpressionConstant2Vector);n.set_editor_property('r',x);n.set_editor_property('g',y);return n
 def op(cls,a,b):n=node(cls);conn(a,n,'A');conn(b,n,'B');return n
 if name in ['MysticWire62','Camo62_7','Camo62_8']:
  col=(.22,.23,.24) if name=='MysticWire62' else (.75,.42,.055) if name.endswith('7') else (.65,.72,.8);ml.connect_material_property(v(col),'',u.MaterialProperty.MP_BASE_COLOR);ml.connect_material_property(c(.9),'',u.MaterialProperty.MP_METALLIC);ml.connect_material_property(c(.27),'',u.MaterialProperty.MP_ROUGHNESS)
 else:
  theme=int(name.rsplit('_',1)[1]);fx=name.startswith('MysticFX');moving=name.startswith('MysticMoving');col=colors[theme]
  time=node(u.MaterialExpressionTime);sine=node(u.MaterialExpressionSine);sine.set_editor_property('period',3+theme*.27);conn(time,sine,'');pulse=op(u.MaterialExpressionAdd,op(u.MaterialExpressionMultiply,sine,c(.22)),c(.78))
  if fx:
   base=v(col);glow=op(u.MaterialExpressionMultiply,op(u.MaterialExpressionMultiply,base,pulse),c(3.5 if theme!=4 else .5));ml.connect_material_property(c(.15),'',u.MaterialProperty.MP_ROUGHNESS)
  else:
   sample=node(u.MaterialExpressionTextureSample);sample.texture=tex
   if moving:
    uv=node(u.MaterialExpressionTextureCoordinate);uv=op(u.MaterialExpressionMultiply,uv,v2(.238,.484));uv=op(u.MaterialExpressionAdd,uv,v2(theme%4*.25+.006,theme//4*.5+.008));conn(uv,sample,'UVs')
   base=sample;des=node(u.MaterialExpressionDesaturation);conn(sample,des,'');power=node(u.MaterialExpressionPower);conn(des,power,'Base');power.set_editor_property('const_exponent',4);glow=op(u.MaterialExpressionMultiply,op(u.MaterialExpressionMultiply,sample,power),pulse);ml.connect_material_property(c(.58),'',u.MaterialProperty.MP_ROUGHNESS)
  ml.connect_material_property(base,'',u.MaterialProperty.MP_BASE_COLOR);ml.connect_material_property(glow,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 ml.recompile_material(m);lib.save_loaded_asset(m);mats['M_'+name]=m
errors=[]
for r in records:
 task=u.AssetImportTask();task.filename=str(src/(r['name']+'.fbx'));task.destination_path='/Game/Art/Meshes';task.destination_name='SM_'+r['name'];task.automated=True;task.replace_existing=True;task.save=False
 opt=u.FbxImportUI();opt.import_mesh=True;opt.import_materials=False;opt.import_textures=False;opt.import_as_skeletal=False;opt.mesh_type_to_import=u.FBXImportType.FBXIT_STATIC_MESH;opt.automated_import_should_detect_type=False;d=opt.static_mesh_import_data;d.combine_meshes=True;d.auto_generate_collision=False;d.generate_lightmap_u_vs=False;d.convert_scene=True;d.convert_scene_unit=True;d.normal_import_method=u.FBXNormalImportMethod.FBXNIM_IMPORT_NORMALS;task.options=opt;at.import_asset_tasks([task]);mesh=lib.load_asset('/Game/Art/Meshes/SM_'+r['name'])
 if not mesh:errors.append(r['name']);continue
 for i,slot in enumerate(mesh.get_editor_property('static_materials')):
  n=str(slot.get_editor_property('imported_material_slot_name'));mat=mats.get(n)
  if mat:mesh.set_material(i,mat)
  else:errors.append(r['name']+' unknown material '+n)
 b=mesh.get_bounding_box();actual=[b.min.x,b.min.y,b.min.z,b.max.x,b.max.y,b.max.z];expected=[x*100 for x in r['bounds']['min']+r['bounds']['max']]
 if max(abs(a-b) for a,b in zip(actual,expected))>.15:errors.append(r['name']+' bounds')
 lib.save_loaded_asset(mesh)
if errors:raise RuntimeError(str(errors))
u.log('ARSENAL62_IMPORT_PASS '+str(len(records)))
