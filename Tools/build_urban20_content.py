"""Additive urban materials and tower spawn profile. Preserves existing catalog rows."""
import unreal as u
lib=u.EditorAssetLibrary; tools=u.AssetToolsHelpers.get_asset_tools(); ml=u.MaterialEditingLibrary
for name,col,rough,metal,emission in [('UrbanGlass20',(.07,.13,.17),.23,.55,.08),('UrbanLitGlass20',(.20,.16,.09),.35,.35,.8),('NeonPink20',(1,.025,.16),.5,0,5),('LampWarm20',(1,.57,.18),.5,0,4)]:
 path='/Game/Materials/M_'+name
 if lib.does_asset_exist(path):
  m=lib.load_asset(path);m.set_editor_property("used_with_instanced_static_meshes",True);ml.recompile_material(m);lib.save_loaded_asset(m);continue
 m=tools.create_asset('M_'+name,'/Game/Materials',u.Material,u.MaterialFactoryNew())
 m.set_editor_property("used_with_instanced_static_meshes",True)
 c=ml.create_material_expression(m,u.MaterialExpressionConstant3Vector);c.constant=u.LinearColor(*col,1)
 ml.connect_material_property(c,'',u.MaterialProperty.MP_BASE_COLOR)
 for val,prop in [(rough,u.MaterialProperty.MP_ROUGHNESS),(metal,u.MaterialProperty.MP_METALLIC)]:
  n=ml.create_material_expression(m,u.MaterialExpressionConstant);n.r=val;ml.connect_material_property(n,'',prop)
 e=ml.create_material_expression(m,u.MaterialExpressionConstant);e.r=emission
 mul=ml.create_material_expression(m,u.MaterialExpressionMultiply);ml.connect_material_expressions(c,'',mul,'A');ml.connect_material_expressions(e,'',mul,'B');ml.connect_material_property(mul,'',u.MaterialProperty.MP_EMISSIVE_COLOR)
 ml.recompile_material(m);lib.save_loaded_asset(m)
cat=lib.load_asset('/Game/Data/DA_EnemySpawns')
if cat:
 rows=list(cat.get_editor_property('POIs'))
 if not any(r.get_editor_property('POIType')==20 for r in rows):
  row=u.LWSpawnRow();row.set_editor_property('POIType',20);row.set_editor_property('MinCount',2);row.set_editor_property('MaxCount',6);row.set_editor_property('IndoorChance',.85);row.set_editor_property('ZombieWeight',65);row.set_editor_property('RaiderWeight',35);row.set_editor_property('DogWeight',0);rows.append(row)
  cat.set_editor_property('POIs',rows);lib.save_loaded_asset(cat)
for name in ['PolicePaintV18','ArcadeMuralV18']:
 m=lib.load_asset('/Game/Materials/M_'+name)
 if m:
  m.set_editor_property('used_with_instanced_static_meshes',True);ml.recompile_material(m);lib.save_loaded_asset(m)
u.log('AAM_URBAN20_CONTENT_COMPLETE')
