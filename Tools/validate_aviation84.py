"""Blender validation of forward cockpit visibility against opaque hull triangles."""
import bpy,math
from mathutils import Vector
from mathutils.bvhtree import BVHTree
for name,x,z in [('Jet',7.9,2.81),('Airbus',14.1,4.31),('AirbusLuxury',14.1,4.31)]:
 o=bpy.data.objects['SM_AV84_'+name+'Shell'];faces=[list(p.vertices) for p in o.data.polygons if 'Glass' not in o.data.materials[p.material_index].name];tree=BVHTree.FromPolygons([v.co for v in o.data.vertices],faces)
 for yaw in [-12,0,12]:
  for pitch in [-4,0,8]:
   a,b=math.radians(yaw),math.radians(pitch);direction=Vector((math.cos(b)*math.cos(a),math.cos(b)*math.sin(a),math.sin(b)));hit,normal,index,distance=tree.ray_cast(Vector((x,-.55,z)),direction,20)
   assert hit is None, (name,yaw,pitch,hit,distance)
 print('COCKPIT84_CLEAR',name)
