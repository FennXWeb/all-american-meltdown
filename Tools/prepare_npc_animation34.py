"""Retain vertex data for nearby NPC facial and articulated limb animation. No packaging."""
import unreal as u
count = 0
for family in ('Resident', 'ResidentFemale', 'Zombie', 'Raider'):
    for part in ('Head', 'Arm', 'Leg'):
        path = f'/Game/Art/Meshes/SM_{family}{part}32'
        mesh = u.EditorAssetLibrary.load_asset(path)
        if not isinstance(mesh, u.StaticMesh):
            raise RuntimeError(f'Missing animation mesh: {path}')
        mesh.set_editor_property('allow_cpu_access', True)
        u.EditorAssetLibrary.save_loaded_asset(mesh)
        count += 1
u.log(f'NPC34_CPU_MESHES={count}')

# Unlit cavity avoids the studio/world lights turning the mouth interior skin-pink.
path = '/Game/Materials/M_NPCMouth34'
mat = u.EditorAssetLibrary.load_asset(path) if u.EditorAssetLibrary.does_asset_exist(path) else None
if not mat:
    mat = u.AssetToolsHelpers.get_asset_tools().create_asset('M_NPCMouth34', '/Game/Materials', u.Material, u.MaterialFactoryNew())
mat.set_editor_property('shading_model', u.MaterialShadingModel.MSM_UNLIT)
mat.set_editor_property('two_sided', True)
u.MaterialEditingLibrary.delete_all_material_expressions(mat)
color = u.MaterialEditingLibrary.create_material_expression(mat, u.MaterialExpressionConstant3Vector)
color.set_editor_property('constant', u.LinearColor(.003, .001, .001, 1))
u.MaterialEditingLibrary.connect_material_property(color, '', u.MaterialProperty.MP_EMISSIVE_COLOR)
u.MaterialEditingLibrary.recompile_material(mat)
u.EditorAssetLibrary.save_loaded_asset(mat)
u.log('NPC34_MOUTH_MATERIAL_READY')
