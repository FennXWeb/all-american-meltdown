"""Custom interior kit. Metres, floor-rooted furniture and explicit wall/ceiling pivots."""
import bpy,bmesh,math,json,random
from pathlib import Path
from mathutils import Vector,Matrix
ROOT=Path('X:/LethalWorld');OUT=ROOT/'ArtSource/Interiors65';OUT.mkdir(exist_ok=True)
bpy.ops.object.select_all(action='SELECT');bpy.ops.object.delete(use_global=False);bpy.context.preferences.filepaths.save_version=0
random.seed(65);parts=[];records=[];mats={}
names=['Wood','Upholstery','Paint','Steel','Acoustic','Glass','Leather','Rubber','Carpet','Terrazzo','Cardboard','Ceramic','Plaster','Navy','Brass','Grille']
atlas=bpy.data.images.load(str(OUT/'T_Interiors65.png'))
for i,n in enumerate(names):
 m=bpy.data.materials.new('M_'+n+'65');m.use_nodes=True;bs=m.node_tree.nodes.get('Principled BSDF');tex=m.node_tree.nodes.new('ShaderNodeTexImage');tex.image=atlas;uv=m.node_tree.nodes.new('ShaderNodeTexCoord');mapping=m.node_tree.nodes.new('ShaderNodeVectorMath');mapping.operation='MULTIPLY_ADD';mapping.inputs[1].default_value=(.242,.242,1);mapping.inputs[2].default_value=((i%4)*.25+.004,1-(i//4+1)*.25+.004,0);m.node_tree.links.new(uv.outputs['UV'],mapping.inputs[0]);m.node_tree.links.new(mapping.outputs[0],tex.inputs['Vector']);m.node_tree.links.new(tex.outputs['Color'],bs.inputs['Base Color']);bs.inputs['Roughness'].default_value=.36 if n in ['Steel','Glass','Brass','Ceramic'] else .8;bs.inputs['Metallic'].default_value=.7 if n in ['Steel','Brass'] else 0;mats[n]=m
def active(o):
 bpy.ops.object.select_all(action='DESELECT');o.select_set(True);bpy.context.view_layer.objects.active=o
def mesh(v,f,mat):
 d=bpy.data.meshes.new(mat);d.from_pydata(v,[],f);d.update();d.materials.append(mats[mat]);o=bpy.data.objects.new(mat,d);bpy.context.collection.objects.link(o);parts.append(o);return o
def box(p,s,mat='Wood',r=.008):
 bpy.ops.mesh.primitive_cube_add(size=1,location=p);o=bpy.context.object;o.scale=s;bpy.ops.object.transform_apply(location=False,rotation=False,scale=True);o.data.materials.append(mats[mat]);parts.append(o)
 if r:
  b=o.modifiers.new('Soft manufactured edges','BEVEL');b.width=r;b.segments=3;bpy.ops.object.modifier_apply(modifier=b.name)
 return o
def tube(points,r=.015,mat='Steel',sides=10):
 v=[];f=[]
 for j,p in enumerate(points):
  p=Vector(p);d=(Vector(points[min(j+1,len(points)-1)])-Vector(points[max(j-1,0)])).normalized();a=d.cross(Vector((0,0,1)))
  if a.length<.01:a=d.cross(Vector((0,1,0)))
  a.normalize();b=d.cross(a)
  v += [p+r*(a*math.cos(k*math.tau/sides)+b*math.sin(k*math.tau/sides)) for k in range(sides)]
 for j in range(len(points)-1):
  for k in range(sides):f.append((j*sides+k,j*sides+(k+1)%sides,(j+1)*sides+(k+1)%sides,(j+1)*sides+k))
 f += [tuple(range(sides-1,-1,-1)),tuple((len(points)-1)*sides+k for k in range(sides))];return mesh(v,f,mat)
def lathe(p,profile,mat='Steel',sides=24):
 v=[(p[0]+r*math.cos(k*math.tau/sides),p[1]+r*math.sin(k*math.tau/sides),p[2]+z) for z,r in profile for k in range(sides)];f=[]
 for j in range(len(profile)-1):
  for k in range(sides):f.append((j*sides+k,j*sides+(k+1)%sides,(j+1)*sides+(k+1)%sides,(j+1)*sides+k))
 f += [tuple(range(sides-1,-1,-1)),tuple((len(profile)-1)*sides+k for k in range(sides))];return mesh(v,f,mat)
def cushion(p,s,mat='Upholstery'):
 # Sculpted super-ellipsoid: broad faces, eased corners, tailored seams.
 v=[];f=[];rings=10;segments=32
 for j in range(rings+1):
  t=-math.pi/2+math.pi*(.01+.98*j/rings);z=math.copysign(abs(math.sin(t))**.45,math.sin(t));rr=abs(math.cos(t))**.45
  for k in range(segments):
   a=k*math.tau/segments;x=math.copysign(abs(math.cos(a))**.4,math.cos(a));y=math.copysign(abs(math.sin(a))**.4,math.sin(a));v.append((p[0]+s[0]*rr*x/2,p[1]+s[1]*rr*y/2,p[2]+s[2]*z/2))
 for j in range(rings):
  for k in range(segments):f.append((j*segments+k,j*segments+(k+1)%segments,(j+1)*segments+(k+1)%segments,(j+1)*segments+k))
 f += [tuple(range(segments-1,-1,-1)),tuple(rings*segments+k for k in range(segments))];return mesh(v,f,mat)
def handle(x,y,z,w=.13):tube([(x-w/2,y+.018,z),(x-w/2,y-.025,z),(x+w/2,y-.025,z),(x+w/2,y+.018,z)],.009)
def legs(w,d,h,mat='Wood'):
 for x in [-w/2+.04,w/2-.04]:
  for y in [-d/2+.04,d/2-.04]:tube([(x*1.06,y*1.06,.02),(x,y,h)],.023,mat)
def books(p,n=7):
 for i in range(n):
  w=random.uniform(.018,.038);h=random.uniform(.18,.26);x=p[0]+i*.036;box((x,p[1],p[2]+h/2),(w,.16,h),'Navy' if i%3 else 'Leather',.002);box((x,p[1]-.083,p[2]+h*.8),(w*.8,.003,.015),'Brass',.001)
def bottles(p,n=4):
 for i in range(n):lathe((p[0]+i*.07,p[1],p[2]),[(0,.028),(.015,.032),(.11,.032),(.14,.014),(.18,.014)],'Glass' if i%2 else 'Ceramic',12)
def wheels(w,d,z=.065):
 for x in [-w/2+.06,w/2-.06]:
  for y in [-d/2+.06,d/2-.06]:
   o=lathe((0,0,0),[(-.016,.05),(.016,.05)],'Rubber',16);o.rotation_euler.x=math.pi/2;o.location=(x,y,z)
def desk():
 box((0,0,.79),(1.4,.65,.055));legs(1.3,.56,.77,'Steel');box((.44,.03,.42),(.38,.52,.61),'Paint');
 for z in [.28,.48,.68]:box((.44,-.24,z),(.35,.018,.18),'Paint');handle(.44,-.26,z)
 box((0,.23,.52),(1.25,.025,.4),'Paint')
def desktop():
 box((0,.12,.028),(.32,.2,.055),'Rubber');tube([(0,.17,.03),(0,.18,.18)],.025);box((0,.19,.3),(.5,.065,.32),'Paint',.015);box((0,.153,.3),(.45,.008,.265),'Glass',.004);box((0,-.13,.018),(.41,.14,.035),'Rubber',.006)
 for i in range(11):
  for j in range(3):box((-.18+i*.035,-.175+j*.041,.039),(.027,.028,.006),'Paint',.001)
 cushion((.29,-.11,.025),(.06,.105,.045),'Rubber');books((-.53,.1,0),4);bottles((.44,.15,0),1)
def chair():
 legs(.43,.46,.44,'Steel');cushion((0,0,.455),(.49,.49,.095),'Navy');cushion((0,.205,.76),(.45,.09,.52),'Navy');tube([(-.19,.2,.25),(-.2,.23,.93)],.016);tube([(.19,.2,.25),(.2,.23,.93)],.016)
def sofa(w=1.9):
 legs(w-.15,.7,.18);box((0,.02,.27),(w,.79,.24));cushion((0,.32,.7),(w,.2,.78));
 for x in [-w/2+.09,w/2-.09]:cushion((x,0,.58),(.19,.83,.48),'Leather')
 n=3 if w>1 else 1
 for i in range(n):cushion((-(w-.3)/2+(i+.5)*(w-.3)/n,-.06,.46),((w-.32)/n,.62,.19));cushion((-(w-.3)/2+(i+.5)*(w-.3)/n,.23,.79),((w-.35)/n,.22,.54))
def table(w,d,h):legs(w-.12,d-.12,h-.04);box((0,0,h-.025),(w,d,.05));
def cabinet(w=1.05,h=1,d=.45):
 box((0,.04,h/2),(w,d-.08,h),'Paint');box((0,0,h+.015),(w+.04,d+.03,.03),'Terrazzo');
 for x in [-w/4,w/4]:box((x,-d/2,h/2),(w/2-.025,.035,h-.06));handle(x+(.1 if x<0 else -.1),-d/2-.03,h*.72,.055)
def shelf(book=False,tools=False):
 for x in [-.56,.56]:box((x,0,.95),(.05,.39,1.9),'Wood' if book else 'Paint')
 box((0,.18,.95),(1.12,.025,1.9),'Wood' if book else 'Paint')
 for z in [.04,.49,.94,1.39,1.84]:
  box((0,0,z),(1.13,.42,.04),'Wood' if book else 'Steel')
  if z<1.8:
   if book:books((-.49,-.02,z+.02),random.randint(15,24))
   elif tools:
    for x in [-.38,0,.38]:box((x,0,z+.13),(.3,.27,.19),'Rubber',.022);handle(x,-.15,z+.13)
   else:
    bottles((-.46,-.02,z+.025),6);box((.22,0,z+.14),(.37,.3,.23),'Cardboard')
def export(name,kind='floor',interaction=''):
 global parts
 bpy.ops.object.select_all(action='DESELECT')
 for o in parts:o.select_set(True)
 bpy.context.view_layer.objects.active=parts[0];bpy.ops.object.join();o=bpy.context.object;o.name='SM_'+name;o.data.transform(o.matrix_world);o.matrix_world=Matrix.Identity(4)
 bm=bmesh.new();bm.from_mesh(o.data);bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces));bm.to_mesh(o.data);bm.free();active(o);bpy.ops.object.mode_set(mode='EDIT');bpy.ops.mesh.select_all(action='SELECT');bpy.ops.uv.smart_project(angle_limit=1.2,island_margin=.005);bpy.ops.object.mode_set(mode='OBJECT')
 if name=='Artwork65':
  for poly in o.data.polygons:
   for idx in poly.loop_indices:
    v=o.data.vertices[o.data.loops[idx].vertex_index].co;o.data.uv_layers.active.data[idx].uv=((v.x+.415)/.83,(v.z-.095)/.65)
 # Weighted corner normals keep planar cabinetry crisp while curved upholstery stays smooth.
 for p in o.data.polygons:p.use_smooth=True
 mod=o.modifiers.new('Weighted manufactured normals','WEIGHTED_NORMAL');mod.keep_sharp=True;bpy.ops.object.modifier_apply(modifier=mod.name)
 tris=sum(len(p.vertices)-2 for p in o.data.polygons)
 if tris>16000:
  dec=o.modifiers.new('Interior triangle budget','DECIMATE');dec.ratio=16000/tris;bpy.ops.object.modifier_apply(modifier=dec.name)
 coords=[v.co for v in o.data.vertices];bounds={'min':[min(v[i] for v in coords) for i in range(3)],'max':[max(v[i] for v in coords) for i in range(3)]}
 c=o.copy();c.data=o.data.copy();bpy.context.collection.objects.link(c)
 for v in c.data.vertices:v.co.y*=-1
 bm=bmesh.new();bm.from_mesh(c.data);bmesh.ops.reverse_faces(bm,faces=list(bm.faces));bm.to_mesh(c.data);bm.free();active(c);bpy.ops.export_scene.fbx(filepath=str(OUT/(name+'.fbx')),use_selection=True,object_types={'MESH'},axis_forward='-Y',axis_up='Z',bake_anim=False,mesh_smooth_type='FACE');bpy.data.objects.remove(c,do_unlink=True)
 records.append(dict(name=name,bounds=bounds,triangles=sum(len(p.vertices)-2 for p in o.data.polygons),kind=kind,interaction=interaction));parts=[];o.hide_render=True;print('INTERIOR65 '+name,flush=True)

desk();export('Desk65',interaction='storage')
desktop();export('Desktop65','surface')
desk();before=len(parts);desktop()
for o in parts[before:]:o.location.z+=.82
export('Workstation65',interaction='storage')
chair();export('Chair65',interaction='chair')
sofa();export('Sofa65',interaction='chair')
sofa(.9);export('Armchair65',interaction='chair')
table(1.15,.6,.43);export('CoffeeTable65')
table(1.6,.85,.78);export('DiningTable65')
box((0,0,.21),(1.55,2.1,.27));legs(1.45,1.95,.17);cushion((0,0,.43),(1.52,2.08,.22),'Acoustic');cushion((0,-.2,.55),(1.54,1.64,.08));box((0,1.02,.67),(1.62,.08,1.04))
for x in [-.4,.4]:cushion((x,.71,.61),(.64,.42,.14),'Acoustic')
export('Bed65',interaction='bed')
cabinet(.52,.59,.42);export('Nightstand65',interaction='storage')
cabinet(1.55,.85,.44);export('Sideboard65',interaction='storage')
cabinet();export('Cabinet65',interaction='storage')
shelf(True);export('Bookcase65',interaction='storage')
shelf();export('Shelf65',interaction='storage')
shelf(False,True);export('ToolShelf65',interaction='storage')
box((0,0,.92),(.5,.49,1.84),'Paint');box((0,-.255,.94),(.46,.03,1.74),'Navy');handle(.13,-.285,.9,.05)
for z in [1.49,1.54,1.59]:box((0,-.279,z),(.29,.005,.017),'Grille',.001)
export('Locker65',interaction='storage')
cabinet(1.5,.99,.7);box((0,-.33,.65),(1.53,.05,.65),'Wood');export('Counter65',interaction='storage')
box((0,0,.91),(.76,.69,1.82),'Paint',.04)
for z,h in [(.64,1.16),(1.54,.54)]:box((0,-.365,z),(.71,.055,h),'Paint',.024);tube([(.24,-.42,z-.15),(.24,-.45,z+.15)],.015,'Steel')
export('Fridge65',interaction='storage')
cabinet(.76,.9,.67)
for x in [-.21,.21]:
 for y in [-.18,.18]:lathe((x,y,.945),[(0,.13),(.008,.13),(.012,.09)],'Grille')
box((0,-.36,.45),(.54,.025,.37),'Glass');handle(0,-.4,.73,.5)
for x in [-.25,-.09,.09,.25]:lathe((x,-.38,.82),[(0,.023),(.023,.023)],'Rubber',12)
export('Stove65',interaction='cooker')
cabinet(.82,.89,.57);lathe((0,0,.93),[(0,.22),(.016,.23),(.02,.18),(-.12,.08)],'Steel');tube([(.26,.2,.94),(.26,.2,1.2),(.17,.15,1.25),(0,.12,1.25),(0,.06,1.19)],.012);export('Sink65',interaction='sink')
lathe((0,0,0),[(0,.16),(.08,.18),(.25,.13),(.32,.24),(.43,.27),(.45,.23),(.36,.16)],'Ceramic');box((0,.22,.65),(.42,.19,.49),'Ceramic',.04);box((0,.22,.905),(.44,.22,.03),'Ceramic',.02);export('Toilet65')
cabinet(1.05,.72,.31);export('WallCabinet65','wall','storage')
for name in ['Washer65','Dryer65']:
 box((0,0,.45),(.68,.63,.9),'Paint',.025);o=lathe((0,0,0),[(0,.24),(.025,.245),(.04,.21)],'Steel');o.rotation_euler.x=math.pi/2;o.location=(0,-.335,.43);o=lathe((0,0,0),[(0,.195),(.012,.195)],'Glass');o.rotation_euler.x=math.pi/2;o.location=(0,-.38,.43);box((0,-.325,.8),(.61,.025,.12),'Grille');export(name,interaction='storage')
box((0,0,1.02),(.72,.66,2.04),'Rubber');
for z in [.2,.4,.6,.8,1,1.2,1.4,1.6,1.8]:
 box((0,-.335,z),(.64,.015,.17),'Grille',.003);box((-.25,-.35,z),(.02,.008,.03),'Glass',.001);handle(.24,-.35,z,.055)
export('ServerRack65',interaction='storage')
for name in ['ExamCart65','ToolTrolley65','CleaningCart65']:
 wheels(.65,.47);legs(.64,.45,.82,'Steel')
 for z in [.16,.51,.85]:box((0,0,z),(.68,.5,.035),'Steel');box((0,.235,z+.06),(.65,.015,.13),'Paint')
 tube([(-.3,.23,.82),(-.3,.3,1.03),(.3,.3,1.03),(.3,.23,.82)],.019);bottles((-.22,0,.88),4);box((.12,0,.65),(.36,.33,.21),'Cardboard');export(name,interaction='storage')
wheels(1.0,.68);box((0,0,.18),(1,.7,.07),'Carpet')
for x in [-.45,.45]:tube([(x,-.28,.18),(x,-.28,1.6),(x,0,1.73),(x,.28,1.6),(x,.28,.18)],.025,'Brass')
tube([(-.45,0,1.73),(.45,0,1.73)],.025,'Brass')
for i in range(3):box((-.25+i*.25,0,.4+i*.04),(.22,.49,.36),'Leather',.035);handle(-.25+i*.25,-.26,.52+i*.04)
export('LuggageCart65',interaction='storage')
lathe((0,0,0),[(0,.18),(.04,.2),(.62,.24),(.66,.24),(.66,.20),(.53,.19)],'Steel');export('WasteBin65')
lathe((0,0,0),[(0,.2),(.05,.23),(.48,.3),(.52,.31),(.52,.26),(.46,.25)],'Terrazzo')
for i in range(12):
 a=i*math.tau/12;p=(math.cos(a)*.04,math.sin(a)*.04,.46);tube([p,(math.cos(a)*.1,math.sin(a)*.1,.9),(math.cos(a)*.32,math.sin(a)*.32,1.1)],.007,'Wood');v=[(0,0,.87),(math.cos(a)*.38,math.sin(a)*.38,1.04),(math.cos(a)*.3-math.sin(a)*.075,math.sin(a)*.3+math.cos(a)*.075,.96)];mesh(v,[(0,1,2)],'Upholstery')
export('Planter65')
lathe((0,0,0),[(0,.07),(.025,.085),(.36,.085),(.42,.04)],'Brass');box((0,0,.455),(.13,.04,.04),'Rubber');tube([(.04,0,.44),(.12,0,.4),(.11,0,.13)],.012,'Rubber');export('Extinguisher65','wall')
for i in range(12):box((-.48+i*.087,0,.37),(.072,.1,.65),'Paint',.027)
tube([(-.53,0,.12),(.53,0,.12)],.022);tube([(-.53,0,.61),(.53,0,.61)],.022);export('Radiator65')
for name in ['Noticeboard65','Frame65','Mirror65']:
 box((0,0,.42),(.9,.045,.72),'Wood');box((0,-.03,.42),(.83,.012,.65),'Glass' if name=='Mirror65' else 'Cardboard' if name=='Noticeboard65' else 'Carpet',.002)
 if name=='Noticeboard65':
  for i in range(7):box((-.31+(i%3)*.28,-.043,.22+(i//3)*.21),(.2,.006,.16),'Acoustic',.001)
 export(name,'wall')
o=lathe((0,0,0),[(0,.2),(.055,.2)],'Paint');o.rotation_euler.x=math.pi/2;o.location=(0,0,.2);o=lathe((0,0,0),[(0,.185),(.005,.185)],'Acoustic');o.rotation_euler.x=math.pi/2;o.location=(0,-.058,.2)
for i in range(12):a=i*math.tau/12;box((math.sin(a)*.155,-.07,.2+math.cos(a)*.155),(.012,.006,.026),'Rubber',.001)
tube([(0,-.08,.2),(.11,-.08,.28)],.008,'Rubber');tube([(0,-.08,.2),(-.02,-.08,.33)],.01,'Rubber');export('Clock65','wall')
box((0,0,.1),(.42,.07,.2),'Paint');box((0,-.044,.1),(.36,.015,.15),'Ceramic');export('ExitLight65','wall')
box((0,0,0),(.57,.57,.04),'Paint');
for i in range(12):box((0,-.25+i*.045,-.025),(.52,.013,.012),'Steel',.001)
export('Vent65','ceiling')
box((0,0,0),(1.18,1.18,.028),'Acoustic',.002)
for s in [-1,1]:box((s*.598,0,-.012),(.018,1.2,.012),'Paint',.001);box((0,s*.598,-.012),(1.2,.018,.012),'Paint',.001)
export('CeilingTile65','ceiling')
tube([(0,0,0),(0,0,-.45)],.006,'Rubber');lathe((0,0,-.65),[(0,.24),(.13,.16),(.22,.04)],'Brass');lathe((0,0,-.645),[(0,.22),(.01,.22)],'Acoustic');export('Pendant65','ceiling')
box((0,0,-.025),(1.2,.23,.05),'Paint')
for y in [-.055,.055]:tube([(-.54,y,-.075),(.54,y,-.075)],.022,'Acoustic',12)
export('TubeLight65','ceiling')
box((0,0,-.19),(2,.42,.38),'Steel',.04)
for x in [-.96,0,.96]:box((x,0,-.19),(.027,.45,.42),'Paint',.01)
export('Duct65','ceiling')
for y in [-.12,.12]:tube([(-1,y,-.13),(1,y,-.13)],.037,'Paint')
for x in [-.9,.9]:box((x,0,-.14),(.025,.4,.025),'Steel')
export('Pipe65','ceiling')
for y in [-.16,.16]:box((0,y,-.15),(2,.03,.14),'Steel')
for i in range(14):box((-.96+i*.148,0,-.215),(.028,.33,.025),'Steel')
for y in [-.09,-.03,.03,.09]:tube([(-1,y,-.19),(1,y,-.19)],.012,'Rubber')
export('CableTray65','ceiling')
cushion((0,0,.012),(2.2,1.6,.024),'Carpet');export('Rug65','surface')
box((0,0,.013),(1.25,.75,.026),'Rubber',.01);export('Mat65','surface')
books((-.15,0,0),9);export('Books65','surface')
bottles((-.12,0,0),5);export('Bottles65','surface')
for i in range(6):o=box((i*.015,0,.004+i*.003),(.22,.3,.003),'Acoustic',.001);o.rotation_euler.z=i*.09
export('Papers65','surface')
box((0,0,.28),(.7,.55,.56),'Wood',.012)
for x in [-.29,.29]:box((x,-.29,.28),(.07,.04,.58),'Wood');box((x,.29,.28),(.07,.04,.58),'Wood')
handle(0,-.31,.36,.2);export('Crate65',interaction='storage')
tube([(0,0,.03),(0,0,1.75)],.03,'Brass')
for i in range(4):a=i*math.pi/2;tube([(0,0,.05),(math.cos(a)*.3,math.sin(a)*.3,.02)],.02,'Brass');tube([(0,0,1.5),(math.cos(a)*.22,math.sin(a)*.22,1.73)],.016,'Brass')
export('CoatRack65')
box((0,0,.16),(.56,.39,.32),'Paint',.022);box((-.045,-.207,.16),(.4,.014,.235),'Glass');handle(.17,-.225,.16,.03);export('Microwave65','surface')
cabinet(1.05,.9,.52);box((-.22,0,1.1),(.3,.31,.35),'Rubber',.025);lathe((-.22,-.08,.96),[(0,.085),(.12,.075)],'Glass');bottles((.2,0,.93),3);export('CoffeeStation65',interaction='storage')
tube([(0,0,0),(0,0,-.29)],.018,'Brass');lathe((0,0,-.38),[(0,.11),(.12,.14)],'Brass')
for i in range(5):a=i*math.tau/5;v=[(.1,0,-.29),(.68,.06,-.29),(.7,.2,-.29),(.17,.12,-.29)];v=[(x*math.cos(a)-y*math.sin(a),x*math.sin(a)+y*math.cos(a),z) for x,y,z in v];o=mesh(v,[(0,1,2,3)],'Wood');m=o.modifiers.new('Blade thickness','SOLIDIFY');m.thickness=.012;active(o);bpy.ops.object.modifier_apply(modifier=m.name)
export('CeilingFan65','ceiling')
for x in [-.45,0,.45]:box((x,0,.065),(.1,1,.13),'Wood')
for i in range(7):box((0,-.46+i*.15,.145),(1.15,.12,.025),'Wood')
for x in [-.3,.3]:box((x,0,.39),(.5,.65,.46),'Cardboard')
export('Pallet65',interaction='storage')
for x in [-.6,.6]:tube([(x,0,.02),(x,0,1.65)],.021);tube([(x,-.3,.02),(x,.3,.02)],.024)
tube([(-.6,0,1.65),(.6,0,1.65)],.02)
for i in range(9):
 x=-.49+i*.12;cushion((x,0,1.07),(.08,.4,.92),'Navy' if i%2 else 'Leather');tube([(x,0,1.55),(x,0,1.7)],.006)
export('ClothesRail65',interaction='storage')
for x in [-.6,.6]:tube([(x,0,0),(x,0,.4)],.025);tube([(x,-.28,0),(x,.28,0)],.025)
box((0,0,.4),(1.7,.05,.07),'Steel')
for x in [-.55,0,.55]:cushion((x,0,.48),(.52,.52,.09),'Navy');cushion((x,.23,.75),(.52,.09,.5),'Navy')
export('WaitingBench65',interaction='chair')
# Restaurant booth: a sprung upholstered bench, high back, plinth and chrome welt.
box((0,.22,.58),(1.18,.14,1.12),'Wood',.025);box((0,0,.17),(1.02,.53,.27),'Rubber',.035)
cushion((0,-.035,.44),(1.13,.62,.19),'Leather');cushion((0,.19,.84),(1.12,.15,.67),'Leather')
for x in [-.565,.565]:tube([(x,-.28,.43),(x,.25,.43),(x,.25,1.13)],.014,'Steel')
for x in [-.32,0,.32]:tube([(x,.10,.61),(x,.11,1.10)],.003,'Wood',6)
export('Booth65',interaction='chair')
lathe((0,0,0),[(0,.28),(.025,.31),(.06,.26),(.085,.08),(.71,.052),(.735,.21)],'Steel',32)
box((0,0,.767),(1.16,.74,.054),'Wood',.07)
for y in [-.365,.365]:tube([(-.50,y,.761),(.50,y,.761)],.008,'Steel')
export('CafeTable65')
mesh([(-.415,-.044,.095),(.415,-.044,.095),(.415,-.044,.745),(-.415,-.044,.745)],[(0,1,2,3)],'Paint');export('Artwork65','wall')
(OUT/'manifest.json').write_text(json.dumps({'assets':records},indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'Interiors65.blend'));print('INTERIORS65_PASS',len(records),flush=True)
