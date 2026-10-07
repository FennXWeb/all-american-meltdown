import bpy,bmesh,json,math
from pathlib import Path
from mathutils import Matrix
root=Path('X:/LethalWorld/ArtSource/Models63');data=json.loads((root/'manifest.json').read_text())
bpy.ops.wm.open_mainfile(filepath=str(root/'Models63.blend'));bpy.context.preferences.filepaths.save_version=0
for r in data['assets']:
 if r['group']!='npc':continue
 budget=8000 if 'Head' in r['name'] else 6000 if 'Torso' in r['name'] else 4000
 if r['triangles']<=budget:continue
 o=next(o for o in bpy.context.scene.objects if o.name.split('.')[0]=='SM_'+r['name']);bpy.ops.object.select_all(action='DESELECT');o.select_set(True);bpy.context.view_layer.objects.active=o
 d=o.modifiers.new('NPC triangle budget','DECIMATE');d.ratio=budget/r['triangles'];bpy.ops.object.modifier_apply(modifier=d.name);r['triangles']=sum(len(p.vertices)-2 for p in o.data.polygons)
 coords=[v.co for v in o.data.vertices];r['bounds']={'min':[min(v[i] for v in coords) for i in range(3)],'max':[max(v[i] for v in coords) for i in range(3)]}
 c=o.copy();c.data=o.data.copy();bpy.context.collection.objects.link(c)
 for v in c.data.vertices:v.co.y*=-1
 bm=bmesh.new();bm.from_mesh(c.data);bmesh.ops.reverse_faces(bm,faces=list(bm.faces));bm.to_mesh(c.data);bm.free();bpy.ops.object.select_all(action='DESELECT');c.select_set(True);bpy.context.view_layer.objects.active=c
 bpy.ops.export_scene.fbx(filepath=str(root/(r['name']+'.fbx')),use_selection=True,object_types={'MESH'},axis_forward='-Y',axis_up='Z',global_scale=1,apply_unit_scale=True,bake_anim=False,mesh_smooth_type='FACE');bpy.data.objects.remove(c,do_unlink=True)
 print('BUDGET63',r['name'],r['triangles'],flush=True)
(root/'manifest.json').write_text(json.dumps(data,indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(root/'Models63.blend'))
