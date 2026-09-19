import unreal as u
lib=u.EditorAssetLibrary
refs=[('/Engine/BasicShapes/Cube','/Game/Art/Meshes/SM_UnitCube'),
      ('/Engine/BasicShapes/Sphere','/Game/Art/Meshes/SM_SkySphere'),
      ('/Engine/MapTemplates/Sky/DaylightAmbientCubemap','/Game/Art/Textures/T_AmbientSky')]
for source,target in refs:
    if lib.does_asset_exist(target):obj=lib.load_asset(target)
    else:
        source_obj=u.load_object(None,source+'.'+source.rsplit('/',1)[1])
        if not source_obj:raise RuntimeError('Could not load source '+source)
        obj=u.AssetToolsHelpers.get_asset_tools().duplicate_asset(target.rsplit('/',1)[1],target.rsplit('/',1)[0],source_obj)
    if not obj:raise RuntimeError('Could not prepare '+target)
    lib.save_loaded_asset(obj)
u.log('LW_RUNTIME_ASSETS_READY')
