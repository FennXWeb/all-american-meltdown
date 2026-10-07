"""Create independently tintable settlement finishes from existing mapped materials."""
import unreal as u
lib=u.EditorAssetLibrary
ml=u.MaterialEditingLibrary
sources=['/Game/Materials/M_Brick','/Game/Art/Interiors65/M_Plaster65',
         '/Game/Materials/M_WallpaperV7','/Game/Materials/M_Wood',
         '/Game/Materials/M_TileV7','/Game/Materials/M_Steel','/Game/Materials/M_Concrete']
for i,src in enumerate(sources):
    target='/Game/Materials/M_Build82_'+str(i)
    if lib.does_asset_exist(target):
        mat=lib.load_asset(target)
    else:
        mat=lib.duplicate_asset(src,target)
        if not mat:
            raise RuntimeError('Missing source finish: '+src)
        original=ml.get_material_property_input_node(mat,u.MaterialProperty.MP_BASE_COLOR)
        output=ml.get_material_property_input_node_output_name(mat,u.MaterialProperty.MP_BASE_COLOR)
        if not original:
            raise RuntimeError('Material has no base-color input: '+src)
        tint=ml.create_material_expression(mat,u.MaterialExpressionVectorParameter)
        tint.set_editor_property('parameter_name','Tint')
        tint.set_editor_property('default_value',u.LinearColor(1,1,1,1))
        multiply=ml.create_material_expression(mat,u.MaterialExpressionMultiply)
        ml.connect_material_expressions(original,output,multiply,'A')
        ml.connect_material_expressions(tint,'',multiply,'B')
        ml.connect_material_property(multiply,'',u.MaterialProperty.MP_BASE_COLOR)
        ml.recompile_material(mat)
        lib.save_loaded_asset(mat)
    if not isinstance(mat,u.Material):
        raise RuntimeError('Expected authored material: '+target)
    u.log('SETTLEMENT82_FINISH '+target)
u.log('SETTLEMENT82_IMPORT_OK 7 tintable mapped finishes')

# Preserve designer-edited loot tables; update only this feature's rows.
loot=lib.load_asset('/Game/Data/DA_LootTable')
native=u.get_default_object(u.LWLootTable)
native_presets={str(p.context):p for p in native.presets}
presets=list(loot.presets)
for preset in presets:
    context=str(preset.context)
    entries=list(preset.entries)
    for entry in entries:
        if str(entry.item_id)=='scrap':
            entry.weight=max(entry.weight,40)
            entry.min_count=max(entry.min_count,8)
            entry.max_count=max(entry.max_count,20)
            if entry.max_per_container>0:
                entry.max_per_container=max(entry.max_per_container,100)
    if context in ('road','depot','military','motel') and not any(str(e.item_id)=='settlement_flag' for e in entries):
        entries.extend(e for e in native_presets[context].entries if str(e.item_id)=='settlement_flag')
    preset.entries=entries
    groups=list(preset.guaranteed_groups)
    for group in groups:
        rows=list(group.entries)
        for entry in rows:
            if str(entry.item_id)=='scrap':
                entry.min_count=max(entry.min_count,8)
                entry.max_count=max(entry.max_count,20)
                if entry.max_per_container>0:
                    entry.max_per_container=max(entry.max_per_container,100)
        group.entries=rows
    if context=='trader':
        for group in native_presets[context].guaranteed_groups:
            if str(group.name) in ('settlement82','building82') and not any(str(g.name)==str(group.name) for g in groups):
                groups.append(group)
        preset.max_items=max(preset.max_items,28)
    preset.guaranteed_groups=groups
loot.presets=presets
lib.save_loaded_asset(loot)
items=lib.load_asset('/Game/Data/DA_ItemCatalog')
rows=list(items.items)
for row in rows:
    if str(row.id)=='scrap':
        row.max_stack=max(row.max_stack,240)
items.items=rows
lib.save_loaded_asset(items)
u.log('SETTLEMENT82_CATALOGS_OK flags and scrap merged without resetting loot')
u.SystemLibrary.quit_editor()
