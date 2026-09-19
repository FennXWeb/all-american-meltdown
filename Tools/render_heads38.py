import bpy,math
from pathlib import Path
from mathutils import Vector
r=Path('X:/LethalWorld');bpy.ops.wm.open_mainfile(filepath=str(r/'ArtSource/Characters35/AllAmericanMeltdown_HeadsHair38.blend'))
for o in bpy.data.objects:o.hide_render=True
for name in ['MaleAnatomicalHead35','Hair0335']:
 bpy.data.objects['SM_'+name].hide_render=False
for material in bpy.data.materials:
 if 'Hair' in material.name:material.diffuse_color=(.10,.047,.022,1)
 elif 'Face' in material.name or 'Skin' in material.name:material.diffuse_color=(.45,.29,.19,1)
world=bpy.context.scene.world;world.use_nodes=True;world.node_tree.nodes['Background'].inputs[0].default_value=(.13,.15,.17,1);world.node_tree.nodes['Background'].inputs[1].default_value=.6
for loc,power,size in [((1,-1,1),45,1),((.5,1,.4),20,1),((-1,.3,.8),40,.5)]:
 bpy.ops.object.light_add(type='AREA',location=loc);o=bpy.context.object;o.data.energy=power;o.data.size=size;o.rotation_euler=(Vector((0,0,-.03))-o.location).to_track_quat('-Z','Y').to_euler()
bpy.ops.object.camera_add(location=(.6,-.5,.13));cam=bpy.context.object;cam.rotation_euler=(Vector((0,0,-.065))-cam.location).to_track_quat('-Z','Y').to_euler();cam.data.type='ORTHO';cam.data.ortho_scale=.38
sc=bpy.context.scene;sc.camera=cam;sc.render.engine='CYCLES';sc.cycles.samples=24;sc.render.resolution_x=900;sc.render.resolution_y=900;sc.render.resolution_percentage=100;sc.render.filepath=str(r/'ArtSource/Characters35/HeadHairGeometry38.png');bpy.ops.render.render(write_still=True)
