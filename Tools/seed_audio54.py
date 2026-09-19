import unreal as u
cat=u.EditorAssetLibrary.load_asset('/Game/Audio/DA_AudioCatalog')
assert cat
added=cat.add_missing_default_slots()
u.EditorAssetLibrary.save_loaded_asset(cat)
for name in ['Slide54','Cloth54','Climb54','Land54','TitanStep54','BehemothStep54','ColossusStep54','GiantSlam54','DeathclawStep54','DeathclawCharge54','ScorpionStep54']:
    slot=cat.get_editor_property('slots')[u.Name(name)]
    assert slot.get_editor_property('source')
u.log('AUDIO54_CATALOG_READY added='+str(added))
u.SystemLibrary.quit_editor()
