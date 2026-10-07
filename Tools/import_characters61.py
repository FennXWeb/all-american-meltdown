"""Versioned character assets/materials, checked before enabling runtime resolution."""
import unreal as u,json,os
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve();src=root/'ArtSource/Characters61';data=json.loads((src/'manifest.json').read_text());dest='/Game/Art/Characters61'
lib=u.EditorAssetLibrary;ml=u.MaterialEditingLibrary;assets=u.AssetToolsHelpers.get_asset_tools()
only=os.environ.get('LW_CHAR61_PREFIX','')
textures={}
for n in ['T_FaceMale61','T_FaceFemale61','T_Materials61','T_HairCards61']:
 if only:textures[n]=lib.load_asset(dest+'/'+n);continue
 t=u.AssetImportTask();t.filename=str(src/(n+'.png'));t.destination_path=dest;t.automated=True;t.save=True;t.replace_existing=True;assets.import_asset_tasks([t]);tex=lib.load_asset(dest+'/'+n);tex.set_editor_property('max_texture_size',2048);tex.set_editor_property('filter',u.TextureFilter.TF_TRILINEAR);lib.save_loaded_asset(tex);textures[n]=tex
mats={}
for key,(kind,value) in data['materials'].items():
 if only:
  name='M_'+key+'61';mats[name]=lib.load_asset(dest+'/'+name)
  if not mats[name]:raise RuntimeError('Full import required before a partial mesh import')
  continue
 name='M_'+key+'61';mat=lib.load_asset(dest+'/'+name) if lib.does_asset_exist(dest+'/'+name) else assets.create_asset(name,dest,u.Material,u.MaterialFactoryNew());ml.delete_all_material_expressions(mat);mat.set_editor_property('two_sided',True)
 def node(cls):return ml.create_material_expression(mat,cls)
 def link(a,p,b,q):
  if not ml.connect_material_expressions(a,p,b,q):raise RuntimeError('Material pin '+q)
 def const(v):
  c=node(u.MaterialExpressionConstant);c.r=v;return c
 def vec(v):
  c=node(u.MaterialExpressionConstant3Vector);c.constant=u.LinearColor(*v,1);return c
 def op(cls,a,b,ap='',bp=''):
  n=node(cls);link(a,ap,n,'A');link(b,bp,n,'B');return n
 uv=node(u.MaterialExpressionTextureCoordinate)
 if kind=='constant':base=vec(value);pin=''
 else:
  if kind=='atlas':
   frac=node(u.MaterialExpressionFrac);link(uv,'',frac,'');s=node(u.MaterialExpressionConstant2Vector);s.r=s.g=.47
   mul=op(u.MaterialExpressionMultiply,frac,s);off=node(u.MaterialExpressionConstant2Vector);off.r=(value%2)*.5+.015;off.g=(value//2)*.5+.015;uv=op(u.MaterialExpressionAdd,mul,off);tex=textures['T_Materials61']
  else:tex=textures['T_HairCards61'] if kind=='cards' else textures['T_FaceFemale61' if value==1 else 'T_FaceMale61']
  sample=node(u.MaterialExpressionTextureSample);sample.texture=tex;link(uv,'',sample,'UVs');base=sample;pin='RGB'
  if kind=='face':
   # Smooth skin endpoint at ears/neck; the existing creator supplies the facial mask.
   neutral=vec([.50,.32,.21]);mask=node(u.MaterialExpressionVertexColor);mix=node(u.MaterialExpressionLinearInterpolate);link(neutral,'',mix,'A');link(sample,'RGB',mix,'B');link(mask,'R',mix,'Alpha');base=mix;pin=''
  if kind=='cards':mat.set_editor_property('blend_mode',u.BlendMode.BLEND_MASKED);mat.set_editor_property('opacity_mask_clip_value',.22);ml.connect_material_property(sample,'A',u.MaterialProperty.MP_OPACITY_MASK)
 tint=node(u.MaterialExpressionVectorParameter);tint.set_editor_property('parameter_name','Tint');tint.set_editor_property('default_value',u.LinearColor(1,1,1,1))
 scale=[1/1.17,1/.84,1/.63] if key.startswith(('Skin','Face')) else [1,1,1] if kind in ['constant','eye'] else [4,4,4] if not key.startswith('Hair') else [8,8,8]
 base=op(u.MaterialExpressionMultiply,base,vec(scale),pin);base=op(u.MaterialExpressionMultiply,base,tint)
 if key.startswith(('Skin','Face')):
  tattoo=lib.load_asset('/Game/Art/Textures/T_Tattoos35');tu=node(u.MaterialExpressionTextureCoordinate);tu.coordinate_index=1;ts=node(u.MaterialExpressionTextureSample);ts.texture=tattoo;link(tu,'',ts,'UVs');vc=node(u.MaterialExpressionVertexColor)
  opacity=node(u.MaterialExpressionScalarParameter);opacity.set_editor_property('parameter_name','TattooOpacity');opacity.set_editor_property('default_value',.85);alpha=op(u.MaterialExpressionMultiply,ts,vc,'A','B');alpha=op(u.MaterialExpressionMultiply,alpha,opacity)
  blend=node(u.MaterialExpressionLinearInterpolate);link(base,'',blend,'A');link(vec([.01,.009,.008]),'',blend,'B');link(alpha,'',blend,'Alpha');base=blend
  ga=node(u.MaterialExpressionScalarParameter);ga.set_editor_property('parameter_name','GloveAmount');ga.set_editor_property('default_value',0);gc=node(u.MaterialExpressionVectorParameter);gc.set_editor_property('parameter_name','GloveTint');gc.set_editor_property('default_value',u.LinearColor(.05,.04,.03,1))
  alpha=op(u.MaterialExpressionMultiply,vc,ga,'G');blend=node(u.MaterialExpressionLinearInterpolate);link(base,'',blend,'A');link(gc,'',blend,'B');link(alpha,'',blend,'Alpha');base=blend
 ml.connect_material_property(base,'',u.MaterialProperty.MP_BASE_COLOR);ml.connect_material_property(const(.28 if key in ['Eye','Iris'] else .83),'',u.MaterialProperty.MP_ROUGHNESS);ml.connect_material_property(const(.15),'',u.MaterialProperty.MP_SPECULAR)
 ml.recompile_material(mat);lib.save_loaded_asset(mat);mats[name]=mat
errors=[];total=0
for rec in data['assets']:
 if only and not rec['name'].startswith(only):continue
 t=u.AssetImportTask();t.filename=str(src/rec['fbx']);t.destination_path=dest;t.destination_name='SM_'+rec['name'];t.automated=True;t.replace_existing=True;t.replace_existing_settings=True;t.save=False
 options=u.FbxImportUI();options.import_mesh=True;options.import_materials=False;options.import_textures=False;options.import_as_skeletal=False;options.mesh_type_to_import=u.FBXImportType.FBXIT_STATIC_MESH;options.automated_import_should_detect_type=False
 d=options.static_mesh_import_data;d.combine_meshes=True;d.auto_generate_collision=True;d.generate_lightmap_u_vs=False;d.convert_scene=True;d.convert_scene_unit=True;d.force_front_x_axis=False;d.transform_vertex_to_absolute=True;d.normal_import_method=u.FBXNormalImportMethod.FBXNIM_IMPORT_NORMALS;d.vertex_color_import_option=u.VertexColorImportOption.REPLACE;t.options=options;t.factory=u.FbxFactory();assets.import_asset_tasks([t])
 mesh=lib.load_asset(dest+'/SM_'+rec['name'])
 if not mesh:errors.append('missing '+rec['name']);continue
 for i,s in enumerate(mesh.get_editor_property('static_materials')):
  n=str(s.get_editor_property('imported_material_slot_name'))
  if n not in mats:errors.append(rec['name']+' material '+n)
  else:mesh.set_material(i,mats[n])
 box=mesh.get_bounding_box();actual=[box.min.x,box.min.y,box.min.z,box.max.x,box.max.y,box.max.z];expected=[v*100 for v in rec['bounds']['min']+rec['bounds']['max']];err=max(abs(a-b) for a,b in zip(actual,expected))
 if err>.06:errors.append(rec['name']+' bounds '+str(err))
 mesh.set_editor_property('allow_cpu_access',True);lib.set_metadata_tag(mesh,'CharacterRevision','61');lib.save_loaded_asset(mesh);total+=1
 u.log('CHAR61_IMPORTED '+rec['name'])
if errors:
 for e in errors:u.log_error(e)
 raise RuntimeError('Character61 validation: '+str(len(errors))+' failures')
u.log('CHARACTERS61_IMPORT_PASS '+str(total))
