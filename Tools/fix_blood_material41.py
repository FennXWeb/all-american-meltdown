import unreal as u
m=u.EditorAssetLibrary.load_asset('/Game/Materials/M_Blood22')
if not m:
    raise RuntimeError('Blood material missing')
u.MaterialEditingLibrary.set_material_usage(m,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES)
u.MaterialEditingLibrary.recompile_material(m)
u.EditorAssetLibrary.save_loaded_asset(m)
assert m.get_editor_property('used_with_instanced_static_meshes')
u.log('AAM_BLOOD41_MATERIAL_OK')
