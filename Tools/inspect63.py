import bpy,json
bpy.ops.wm.open_mainfile(filepath='X:/LethalWorld/ArtSource/Characters61/Characters61_Runtime.blend')
o=next(o for o in bpy.data.objects if o.name.startswith('SM_MaleAnatomicalHead35'))
for i,m in enumerate(o.data.materials):
 ids={v for p in o.data.polygons if p.material_index==i for v in p.vertices}
 if ids:
  vv=[o.data.vertices[j].co for j in ids];print('MAT',m.name,len(ids),[sum(v[k] for v in vv)/len(vv) for k in range(3)],flush=True)
print('UV',[(x.name,x.active_render) for x in o.data.uv_layers])

