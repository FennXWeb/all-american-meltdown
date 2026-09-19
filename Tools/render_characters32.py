import bpy,math
from pathlib import Path
from mathutils import Vector
root=Path(__file__).resolve().parents[1];out=root/'ArtSource/Characters32'
bpy.ops.wm.open_mainfile(filepath=str(out/'AllAmericanMeltdown_Characters32.blend'))
source={o.name:o for o in bpy.data.objects if o.type=='MESH'}
for o in source.values():o.hide_render=True
def add(name,at):
 o=source['SM_'+name].copy();o.data=o.data.copy();bpy.context.collection.objects.link(o);o.location=at;o.hide_render=False;return o
for j,k in enumerate(['Resident','ResidentFemale','Raider','Zombie']):
 y=(j-1.5)*.85
 for part,p in [('Torso',(0,y,1.48)),('Head',(0,y,1.76)),('Pelvis',(0,y,.97)),('Arm',(0,y-.18,1.43)),('Arm',(0,y+.18,1.43)),('Leg',(0,y-.09,.84)),('Leg',(0,y+.09,.84))]:add(k+part+'32',p)
 if j<3:add('Hair%02d32'%[2,5,1][j],(0,y,1.76))
bpy.ops.mesh.primitive_plane_add(size=200);floor=bpy.context.object;floor.name='Preview Floor';mat=bpy.data.materials.new('Studio');mat.diffuse_color=(.075,.085,.10,1);floor.data.materials.append(mat)
scene=bpy.context.scene;scene.render.engine='CYCLES';scene.cycles.samples=24
scene.world.color=(.16,.16,.16);scene.view_settings.view_transform='Standard';scene.render.resolution_x=1440;scene.render.resolution_y=1000;scene.render.resolution_percentage=100
for pos,power,size in [((3,-3,5),650,5),((1,4,3),400,4),((-3,0,4),550,3)]:
 bpy.ops.object.light_add(type='AREA',location=pos);l=bpy.context.object;l.data.energy=power;l.data.shape='DISK';l.data.size=size;l.rotation_euler=(Vector((0,0,1))-l.location).to_track_quat('-Z','Y').to_euler()
bpy.ops.object.camera_add(location=(5,-2.2,2.7));camera=bpy.context.object;scene.camera=camera;camera.rotation_euler=(Vector((0,0,1.05))-camera.location).to_track_quat('-Z','Y').to_euler();camera.data.type='ORTHO';camera.data.ortho_scale=4.2
scene.render.filepath=str(out/'Lineup32.png');bpy.ops.render.render(write_still=True)
camera.location=(2,-1.5,1.83);camera.rotation_euler=(Vector((0,-.425,1.64))-camera.location).to_track_quat('-Z','Y').to_euler();camera.data.ortho_scale=.54;scene.render.resolution_x=900;scene.render.resolution_y=900
scene.render.filepath=str(out/'Portrait32.png');bpy.ops.render.render(write_still=True)
