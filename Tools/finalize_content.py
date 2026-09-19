import unreal as u
lib=u.EditorAssetLibrary
ml=u.MaterialEditingLibrary
for path in lib.list_assets('/Game/Materials',recursive=True,include_folder=False):
    mat=lib.load_asset(path)
    if mat.get_name() not in ['M_Crust','M_Sky']:
        ml.set_material_usage(mat,u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES)
        ml.recompile_material(mat);lib.save_loaded_asset(mat)
        u.log('LW_USAGE_OK '+mat.get_name())
u.log('LW_FINALIZE_COMPLETE')
