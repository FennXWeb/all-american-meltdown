import unreal as u
for i in range(1,101):
    tex=u.EditorAssetLibrary.load_asset(f'/Game/Art/Textures/T_TradingCard{i:03}36')
    if not tex: raise RuntimeError(f'Missing card {i}')
    tex.set_editor_property('never_stream',False)
    u.EditorAssetLibrary.save_loaded_asset(tex)
u.log('CARD37_STREAMING_ENABLED 100')
