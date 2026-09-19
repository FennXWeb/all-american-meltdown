import bpy,math
from pathlib import Path
from mathutils import Vector
root=Path('X:/LethalWorld');bpy.ops.wm.open_mainfile(filepath=str(root/'ArtSource/Characters35/AllAmericanMeltdown_Characters35.blend'))
for o in bpy.data.objects:o.hide_render=True
for sex,y,top,hair in [('Male',-.63,3,3),('Female',.63,2,6)]:
 for name,at in [(sex+f'Top{top}35',(0,y,1.51)),(sex+'Waist035',(0,y,1.00)),(sex+'AnatomicalHead35',(0,y,1.74)),(f'Hair{hair:02}35',(0,y,1.74)),(f'Sleeve{top}35',(0,y-.18,1.46)),(f'Sleeve{top}35',(0,y+.18,1.46)),('NPCLeg035',(0,y-.09,.87)),('NPCLeg035',(0,y+.09,.87))]:
  src=bpy.data.objects['SM_'+name];o=src.copy();o.data=src.data.copy();bpy.context.collection.objects.link(o);o.location=at;o.hide_render=False
bpy.ops.mesh.primitive_plane_add(size=200);floor=bpy.context.object;mat=bpy.data.materials.new('StudioFloor');mat.diffuse_color=(.12,.13,.14,1);floor.data.materials.append(mat)
world=bpy.context.scene.world;world.use_nodes=True;world.node_tree.nodes['Background'].inputs[0].default_value=(.18,.2,.23,1);world.node_tree.nodes['Background'].inputs[1].default_value=.6
for loc,energy,size in [((3,-4,5),800,5),((2,4,3),500,4),((-3,1,4),850,3)]:
 bpy.ops.object.light_add(type='AREA',location=loc);l=bpy.context.object;l.data.energy=energy;l.data.shape='DISK';l.data.size=size;l.rotation_euler=(Vector((0,0,1))-l.location).to_track_quat('-Z','Y').to_euler()
bpy.ops.object.camera_add(location=(4.9,-2.8,2.45));cam=bpy.context.object;cam.rotation_euler=(Vector((0,0,1.0))-cam.location).to_track_quat('-Z','Y').to_euler();cam.data.type='ORTHO';cam.data.ortho_scale=2.75
sc=bpy.context.scene;sc.camera=cam;sc.render.engine='CYCLES';sc.cycles.samples=24;sc.render.resolution_x=1400;sc.render.resolution_y=1100;sc.render.resolution_percentage=100;sc.render.filepath=str(root/'ArtSource/Characters35/ModelReview35.png');bpy.ops.render.render(write_still=True)
