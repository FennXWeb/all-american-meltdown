import bpy,json,math
from pathlib import Path
out=Path('X:/LethalWorld/ArtSource/Character61')
bpy.ops.wm.open_mainfile(filepath=str(out/'Survivor61_Approval.blend'))
sc=bpy.context.scene
locks=[o for o in sc.objects if o.type=='MESH' and o.name.startswith('Hair61 | layered lock')]
def points(frame):
 sc.frame_set(frame);dg=bpy.context.evaluated_depsgraph_get();result=[]
 for ob in locks:
  ev=ob.evaluated_get(dg);me=ev.to_mesh();result.extend(ev.matrix_world@v.co for v in me.vertices);ev.to_mesh_clear()
 return result
base=points(1);moved=points(48);travel=max((a-b).length for a,b in zip(base,moved))
body=bpy.data.objects['Survivor61 | anatomical body']
uv=body.data.uv_layers['FaceUV'];indices=[l.index for l in body.data.loops if abs(body.data.vertices[l.vertex_index].co.z-1.669)<.001 and body.data.vertices[l.vertex_index].co.x>.13]
checks={
 '96_hair_locks':len(locks)==96,
 'hair_moves_on_mesh':travel>.0001,
 'bounded_hair_motion':travel<.05,
 'face_uv_eye_height':all(.54<uv.data[i].uv.y<.57 for i in indices) and bool(indices),
 'all_source_images_packed':all(im.packed_file is not None for im in bpy.data.images if im.source=='FILE'),
 'finite_vertices':all(math.isfinite(c) for o in sc.objects if o.type=='MESH' for v in o.data.vertices for c in v.co),
 'fbx_exists':(out/'Survivor61_Approval.fbx').stat().st_size>100000,
}
result={'checks':checks,'max_hair_displacement_m':travel,'all_passed':all(checks.values())}
(out/'validation.json').write_text(json.dumps(result,indent=2));print('CHARACTER61_VALIDATION',json.dumps(result))
if not result['all_passed']:raise RuntimeError('Character validation failed')
