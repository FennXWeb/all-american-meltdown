"""Create a soft, turbulent vertex-colour particle material without external dependencies."""
import unreal as u
ml=u.MaterialEditingLibrary;lib=u.EditorAssetLibrary;at=u.AssetToolsHelpers.get_asset_tools()
path='/Game/Materials/M_Explosion68'
m=lib.load_asset(path) if lib.does_asset_exist(path) else at.create_asset('M_Explosion68','/Game/Materials',u.Material,u.MaterialFactoryNew())
ml.delete_all_material_expressions(m)
m.set_editor_property('blend_mode',u.BlendMode.BLEND_TRANSLUCENT)
m.set_editor_property('shading_model',u.MaterialShadingModel.MSM_UNLIT)
m.set_editor_property('two_sided',True)
v=ml.create_material_expression(m,u.MaterialExpressionVertexColor)
uv=ml.create_material_expression(m,u.MaterialExpressionTextureCoordinate)
c=ml.create_material_expression(m,u.MaterialExpressionCustom)
i=u.CustomInput();i.set_editor_property('input_name','UV');c.set_editor_property('inputs',[i]);c.set_editor_property('output_type',u.CustomMaterialOutputType.CMOT_FLOAT1)
c.set_editor_property('code', '''float2 p=UV*2-1; float2 q=p*3.1; float cloud=0; float weight=.57;
for(int octave=0;octave<3;octave++){
 float2 cell=floor(q), f=frac(q); f=f*f*(3-2*f);
 float a=frac(sin(dot(cell,float2(127.1,311.7)))*43758.5453);
 float b=frac(sin(dot(cell+float2(1,0),float2(127.1,311.7)))*43758.5453);
 float c=frac(sin(dot(cell+float2(0,1),float2(127.1,311.7)))*43758.5453);
 float d=frac(sin(dot(cell+1,float2(127.1,311.7)))*43758.5453);
 cloud+=lerp(lerp(a,b,f.x),lerp(c,d,f.x),f.y)*weight; q=q*2.13+11.7; weight*=.5;
}
return saturate((.92-length(p))*3.3)*smoothstep(.12,.8,cloud+.22);''')
ml.connect_material_expressions(uv,'',c,'UV')
mul=ml.create_material_expression(m,u.MaterialExpressionMultiply);ml.connect_material_expressions(c,'',mul,'A');ml.connect_material_expressions(v,'A',mul,'B')
ml.connect_material_property(mul,'',u.MaterialProperty.MP_OPACITY);ml.connect_material_property(v,'RGB',u.MaterialProperty.MP_EMISSIVE_COLOR)
ml.recompile_material(m);lib.save_loaded_asset(m);u.log('EFFECTS68_IMPORT_PASS');u.SystemLibrary.quit_editor()
