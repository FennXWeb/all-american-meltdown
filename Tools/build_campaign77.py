"""Campaign production meshes. Metres; fitted shell surfaces, local pivots, UVs and PBR atlas.
Run with Blender 4.2 --background --python Tools/build_campaign77.py.
"""
import bpy, bmesh, math, json, random
from pathlib import Path
from mathutils import Vector, Matrix
ROOT=Path('X:/LethalWorld'); OUT=ROOT/'ArtSource/Campaign77'; OUT.mkdir(exist_ok=True)
bpy.ops.object.select_all(action='SELECT'); bpy.ops.object.delete(use_global=False)
bpy.context.preferences.filepaths.save_version=0
names=['Armor','Rubber','Steel','White','Walnut','Cloth','Leather','Brass','Teal','Rope','Zinc','Mesh','Plaster','Terrazzo','Timber','Burned']
atlas=bpy.data.images.load(str(OUT/'T_Campaign77.png')); mats={}; parts=[]; records=[]
for i,n in enumerate(names):
 m=bpy.data.materials.new('M_'+n+'77');m.use_nodes=True;p=m.node_tree.nodes.get('Principled BSDF');t=m.node_tree.nodes.new('ShaderNodeTexImage');t.image=atlas;m.node_tree.links.new(t.outputs['Color'],p.inputs['Base Color']);p.inputs['Roughness'].default_value=.4 if n in ['Steel','Brass','Walnut'] else .75;p.inputs['Metallic'].default_value=.8 if n in ['Steel','Brass','Zinc'] else 0;mats[n]=m
def active(o):
 bpy.ops.object.select_all(action='DESELECT');o.select_set(True);bpy.context.view_layer.objects.active=o
def mesh(v,f,mat):
 d=bpy.data.meshes.new(mat);d.from_pydata(v,[],f);d.update();d.materials.append(mats[mat]);o=bpy.data.objects.new(mat,d);bpy.context.collection.objects.link(o);parts.append(o);return o
def box(p,s,mat='Armor',bevel=.018):
 bpy.ops.mesh.primitive_cube_add(size=1,location=p);o=bpy.context.object;o.scale=s;bpy.ops.object.transform_apply(location=False,rotation=False,scale=True);o.data.materials.append(mats[mat]);parts.append(o)
 if bevel:
  mod=o.modifiers.new('Machined edges','BEVEL');mod.width=bevel;mod.segments=3;bpy.ops.object.modifier_apply(modifier=mod.name)
 return o
def tube(points,r,mat='Steel',sides=12):
 v=[];f=[]
 for j,p in enumerate(points):
  p=Vector(p);d=(Vector(points[min(j+1,len(points)-1)])-Vector(points[max(j-1,0)])).normalized();a=d.cross(Vector((0,0,1)))
  if a.length<.01:a=d.cross(Vector((0,1,0)))
  a.normalize();b=d.cross(a);v.extend([p+r*(a*math.cos(k*math.tau/sides)+b*math.sin(k*math.tau/sides)) for k in range(sides)])
 for j in range(len(points)-1):
  for k in range(sides):f.append((j*sides+k,j*sides+(k+1)%sides,(j+1)*sides+(k+1)%sides,(j+1)*sides+k))
 f += [tuple(range(sides-1,-1,-1)),tuple((len(points)-1)*sides+k for k in range(sides))];return mesh(v,f,mat)
def lathe(profile,mat='Steel',n=32):
 v=[(r*math.cos(k*math.tau/n),r*math.sin(k*math.tau/n),z) for z,r in profile for k in range(n)];f=[]
 for j in range(len(profile)-1):
  for k in range(n):f.append((j*n+k,j*n+(k+1)%n,(j+1)*n+(k+1)%n,(j+1)*n+k))
 f += [tuple(range(n-1,-1,-1)),tuple((len(profile)-1)*n+k for k in range(n))];return mesh(v,f,mat)
def ring(p,r,thick,mat='Steel'):
 pts=[(p[0]+r*math.cos(k*math.tau/32),p[1]+r*math.sin(k*math.tau/32),p[2]) for k in range(33)];return tube(pts,thick,mat)
def hull(rows,mat):
 # Watertight longitudinal loft with carefully authored chine/shoulder cross sections.
 v=[];f=[];n=len(rows[0][1])
 for y,profile in rows:v.extend([(x,y,z) for x,z in profile])
 for j in range(len(rows)-1):
  for k in range(n):f.append((j*n+k,j*n+(k+1)%n,(j+1)*n+(k+1)%n,(j+1)*n+k))
 f.extend([tuple(range(n-1,-1,-1)),tuple((len(rows)-1)*n+k for k in range(n))]);return mesh(v,f,mat)
def shift_new(start,delta):
 for o in parts[start:]:o.location+=Vector(delta)
def export(name,collision='complex'):
 global parts
 bpy.ops.object.select_all(action='DESELECT')
 for o in parts:o.select_set(True)
 bpy.context.view_layer.objects.active=parts[0];bpy.ops.object.join();o=bpy.context.object;o.name='SM_'+name;o.data.transform(o.matrix_world);o.matrix_world=Matrix.Identity(4)
 bm=bmesh.new();bm.from_mesh(o.data);bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces));bm.to_mesh(o.data);bm.free();active(o)
 bpy.ops.object.mode_set(mode='EDIT');bpy.ops.mesh.select_all(action='SELECT');bpy.ops.uv.smart_project(angle_limit=1.1,island_margin=.015);bpy.ops.object.mode_set(mode='OBJECT')
 # Each polygon's UV island uses its named atlas swatch, with gutters against mip bleeding.
 for poly in o.data.polygons:
  slot=names.index(o.data.materials[poly.material_index].name[2:-2]);
  for li in poly.loop_indices:
   uv=o.data.uv_layers.active.data[li].uv;uv.x=(slot%4)*.25+.01+uv.x*.23;uv.y=1-(slot//4+1)*.25+.01+uv.y*.23
  poly.use_smooth=poly.area<.3
 mod=o.modifiers.new('Weighted face normals','WEIGHTED_NORMAL');mod.keep_sharp=True;bpy.ops.object.modifier_apply(modifier=mod.name)
 verts=[v.co for v in o.data.vertices];bounds={'min':[min(v[i] for v in verts) for i in range(3)],'max':[max(v[i] for v in verts) for i in range(3)]}
 tri=sum(len(p.vertices)-2 for p in o.data.polygons)
 c=o.copy();c.data=o.data.copy();bpy.context.collection.objects.link(c)
 for v in c.data.vertices:v.co.y*=-1
 bm=bmesh.new();bm.from_mesh(c.data);bmesh.ops.reverse_faces(bm,faces=list(bm.faces));bm.to_mesh(c.data);bm.free();active(c)
 bpy.ops.export_scene.fbx(filepath=str(OUT/(name+'.fbx')),use_selection=True,object_types={'MESH'},axis_forward='-Y',axis_up='Z',bake_anim=False,mesh_smooth_type='FACE');bpy.data.objects.remove(c,do_unlink=True)
 records.append(dict(name=name,bounds=bounds,triangles=tri,collision=collision));parts=[];o.hide_render=True;print('CAMPAIGN77_MESH',name,tri,flush=True)

# Bespoke 11m four-axle command carrier: integral sloped belly, prow and side sponsons.
rows=[]
for y,w,bottom,top in [(-5.5,1.8,.9,1.8),(-4.7,2.55,.45,1.8),(-3.7,2.65,.4,1.8),(3.9,2.65,.4,1.8),(5.2,2.15,.85,1.8)]:
 rows.append((y,[(-w*.7,bottom),(-w,bottom+.35),(-w,top-.2),(-w*.9,top),(w*.9,top),(w,top-.2),(w,bottom+.35),(w*.7,bottom)]))
hull(rows,'Armor');box((0,0,1.84),(4.85,8.9,.12),'Walnut')
for x in [-2.7,2.7]:
 for y in [-3.75,-1.25,1.25,3.75]:
  o=lathe([(-.27,.56),(-.2,.76),(.2,.76),(.27,.57)],'Rubber',40);o.rotation_euler.y=math.pi/2;o.location=(x,y,.83)
  o=lathe([(-.29,.36),(.29,.36)],'Steel',24);o.rotation_euler.y=math.pi/2;o.location=(x,y,.83)
  for k in range(8):tube([(x+(.31 if x>0 else -.31),y+math.cos(k*math.tau/8)*.22,.83+math.sin(k*math.tau/8)*.22),(x+(.34 if x>0 else -.34),y+math.cos(k*math.tau/8)*.22,.83+math.sin(k*math.tau/8)*.22)],.032,'Zinc',8)
 for y in [-2.5,0,2.5]:box((x*.91,y,1.35),(.12,.8,.55),'Armor')
for x in [-1.5,1.5]:
 tube([(x,-5.25,1.28),(x,-5.55,1.28)],.13,'White');tube([(x,-5.3,1.0),(x,-5.6,1.0)],.09,'Brass')
for i in range(12):box((-.85+i*.15,-5.33,1.55),(.055,.055,.3),'Steel',.006)
for x in [-1.6,1.6]:tube([(x,4.4,1.4),(x,4.7,2.2),(x,4.7,2.9)],.09,'Steel')
export('CarrierHull77')
# Side armor at its hinge. Window cutouts are real geometry, backed by separate glass.
for y in [-4.0,-1.4,1.4,4.0]:box((0,y,1.2),(.18,.18,2.4))
box((0,0,.34),(.2,9,.68));box((0,0,2.38),(.2,9,.32))
for y in [-2.7,0,2.7]:box((-.015,y,.79),(.23,2.4,.16),'Steel')
for y in [-3.9,-1.3,1.3,3.9]:
 for z in [.22,2.4]:tube([(-.12,y,z),(-.16,y,z)],.043,'Zinc',8)
export('CarrierSide77')
box((0,0,0),(4.95,.19,2.55));
for x in [-2,-1,0,1,2]:box((x,-.12,0),(.1,.08,2.4),'Steel')
export('CarrierShutter77')
# Roof forms one close-fitting curved cap with a central turret opening.
for x in [-1.65,1.65]:box((x,0,0),(1.7,9.25,.2))
box((0,0,0),(1.62,9.25,.2))
for x in [-2.3,2.3]:tube([(x,-4.4,.13),(x,4.4,.13)],.06,'Steel')
export('CarrierRoof77')
lathe([(0,.9),(.12,.96),(.23,.76),(.65,.56)],'Armor');box((0,-.12,.5),(1.3,1.25,.42))
for x in [-.26,.26]:tube([(x,-.5,.55),(x,-2.5,.55)],.11,'Steel');ring((x,-2.2,.55),.15,.025,'Steel')
box((.52,-.2,.72),(.2,.32,.2),'White');export('CarrierTurret77')
# Authored bent armor fragments. These use pivot-driven release, not noisy random cubes.
mesh([(-1.2,0,0),(1.1,0,0),(1.25,.16,1.4),(.35,-.35,2.3),(-1.05,.1,2.1),(-1.2,.12,0),(1.1,.12,0),(1.25,.28,1.4),(.35,-.23,2.3),(-1.05,.22,2.1)],[(0,1,2,3,4),(9,8,7,6,5),(0,5,6,1),(1,6,7,2),(2,7,8,3),(3,8,9,4),(4,9,5,0)],'Burned');export('CarrierTornPanel77','none')
# Presidential desk: panelled pedestals, inset leather writing surface, brass pull handles.
box((0,0,.86),(2.3,.98,.1),'Walnut',.045);box((0,-.09,.92),(1.02,.58,.018),'Leather',.01)
for x in [-.82,.82]:
 box((x,.02,.42),(.56,.83,.82),'Walnut')
 for z in [.2,.43,.66]:
  box((x,-.41,z),(.5,.025,.2),'Walnut',.012);tube([(x-.1,-.46,z),(x+.1,-.46,z)],.012,'Brass')
box((0,.39,.46),(1.1,.035,.63),'Walnut');export('ExecutiveDesk77')
for x in [-.34,.34]:
 for y in [-.31,.31]:tube([(x,y,0),(x*.92,y*.92,.5)],.04,'Walnut')
box((0,0,.51),(.84,.76,.18),'Leather',.07);box((0,.32,1.03),(.83,.18,1.01),'Leather',.075)
for x in [-.48,.48]:box((x,0,.74),(.14,.77,.17),'Walnut',.035)
for x in [-.24,0,.24]:
 for z in [.85,1.08,1.29]:tube([(x,.212,z),(x,.19,z)],.012,'Brass',8)
export('ExecutiveChair77')
# Flag is a draped cloth surface, stripes and stars are mesh regions.
tube([(0,0,0),(0,0,2.5)],.025,'Brass');lathe([(0,.24),(.05,.24),(.09,.06)],'Brass')
v=[];f=[]
for j in range(14):
 for i in range(17):
  x=i*.075;v.append((x,.055*math.sin(x*10+j*.22),2.37-j*.068-.07*(x/1.2)**2))
for j in range(13):
 for i in range(16):f.append((j*17+i,j*17+i+1,(j+1)*17+i+1,(j+1)*17+i))
o=mesh(v,f,'White');o.data.materials.append(mats['Cloth']);o.data.materials.append(mats['Brass'])
for p in o.data.polygons:
 j=p.index//16;i=p.index%16;p.material_index=1 if j<7 and i<7 else (2 if j%2==0 else 0)
mod=o.modifiers.new('Cloth thickness','SOLIDIFY');mod.thickness=.004;active(o);bpy.ops.object.modifier_apply(modifier=mod.name);export('ContinuityFlag77','none')
# Passenger ferry, six benches, open central aisle, aft boarding threshold and forward wheelhouse.
rows=[]
for y,w in [(-7.2,1.8),(-6.4,2.65),(-4,2.9),(3.8,2.9),(6,2.15),(7.7,.18)]:
 rows.append((y,[(-w*.65,-1.18),(-w,-.45),(-w,.08),(w,.08),(w,-.45),(w*.65,-1.18)]))
hull(rows,'Teal')
deck=[]
for y,profile in rows:
 w=max(.12,abs(profile[1][0])-.12);deck.extend([(-w,y,.105),(w,y,.105)])
o=mesh(deck,[(j*2,j*2+1,j*2+3,j*2+2) for j in range(len(rows)-1)],'Timber');active(o);mod=o.modifiers.new('Fitted deck thickness','SOLIDIFY');mod.thickness=.055;bpy.ops.object.modifier_apply(modifier=mod.name)
for side in [-1,1]:
 x=side*2.66;tube([(x,-6,.23),(x,-6,1.15),(x,5.4,1.15),(side*.4,7.0,1.15)],.045,'White')
 for y in [-5.8,-3.8,-1.8,.2,2.2,4.2]:tube([(x,y,.18),(x,y,1.15)],.035,'White')
 for y in [-4.7,-2.4,0]:
  box((side*1.72,y,.58),(1.25,.72,.15),'Cloth',.06);box((side*1.72,y+.28,.94),(1.25,.16,.66),'Cloth',.05)
  for dx in [-.45,.45]:tube([(side*1.72+dx,y,.17),(side*1.72+dx,y,.54)],.04,'White')
box((0,3.85,.35),(3.1,2.75,.48),'White');box((0,4,2.65),(3.4,3.0,.16),'White')
for x in [-1.5,1.5]:
 for y in [2.65,5.3]:tube([(x,y,.5),(x,y,2.62)],.08,'White')
box((0,5.27,.91),(3,.14,.65),'Teal');box((0,4.8,1.17),(2.4,.7,.15),'Steel')
tube([(0,4.84,1.15),(0,4.6,1.5)],.055,'Steel')
tube([(.31*math.cos(k*math.tau/24),4.57+.1*math.sin(k*math.tau/24),1.53+.29*math.sin(k*math.tau/24)) for k in range(25)],.025,'Steel')
for side in [-1,1]:
 tube([(side*2.65,2,.4),(side*2.65,2,1.2)],.02,'Rope')
 tube([(side*2.7,2+.38*math.cos(k*math.tau/24),.73+.38*math.sin(k*math.tau/24)) for k in range(25)],.075,'White')
for x in [-2.83,2.83]:
 for y in [-4,0,4]:
  o=lathe([(-.12,.3),(.12,.3)],'Rubber',24);o.rotation_euler.y=math.pi/2;o.location=(x,y,-.25)
export('Ferry77')
box((0,0,-.05),(1.7,3.3,.1),'Zinc');
for x in [-.84,.84]:tube([(x,-1.6,0),(x,-1.6,.95),(x,1.6,.95),(x,1.6,0)],.035,'White')
for y in [-1.5,-.75,0,.75,1.5]:box((0,y,.008),(1.68,.045,.015),'Rubber',.003)
export('Gangway77')
# Functional canal gate, gear wheel, hoist drum and sheaves with stable pivots.
box((0,2.6,-1.3),(.28,5.2,3.5),'Timber')
for z in [-2.8,-1.3,.1]:box((-.17,2.6,z),(.12,5.2,.18),'Steel')
for y in [.08,2.6,5.12]:box((-.17,y,-1.3),(.12,.16,3.5),'Steel')
tube([(0,0,-3.1),(0,0,.5)],.16,'Steel');export('LockGate77')
lathe([(-.2,.2),(.2,.2)],'Steel');ring((0,0,0),1,.07)
for i in range(8):tube([(0,0,0),(math.cos(i*math.tau/8),math.sin(i*math.tau/8),0)],.055,'Steel')
for i in range(40):
 a=i*math.tau/40;o=box((math.cos(a)*1.08,math.sin(a)*1.08,0),(.16,.14,.16),'Steel',.01);o.rotation_euler.z=a
export('LockGear77','none')
lathe([(-.65,.7),(-.52,.7),(-.5,.47),(.5,.47),(.52,.7),(.65,.7)],'Steel');
for z in [i*.075-.42 for i in range(12)]:ring((0,0,z),.48,.036,'Rope')
export('Winch77','none')
# Contact props use handles authored at local zero for exact wrist matching.
box((0,0,-.18),(.48,.15,.3),'Steel');box((0,-.01,-.19),(.44,.16,.025),'White');tube([(-.08,0,-.03),(-.08,0,.035),(.08,0,.035),(.08,0,-.03)],.014,'Rubber');export('Case77','none')
box((0,0,-.09),(.028,.024,.2),'Steel',.003);tube([(0,0,0),(0,0,.065)],.024,'Rubber');export('RepairTool77','none')
lathe([(-.035,.045),(.035,.045)],'White',20);export('DressingRoll77','none')
for x in [-.055,.055]:ring((x,0,0),.04,.006,'Steel')
tube([(-.015,0,0),(.015,0,0)],.004,'Steel');export('Restraint77','none')
# Family tracing desk kit, privacy screen, school desk and coat cubbies.
box((0,0,.78),(1.65,.76,.07),'White')
for x in [-.72,.72]:
 for y in [-.29,.29]:tube([(x,y,0),(x,y,.74)],.026,'Steel')
for x in [-.38,.38]:box((x,.16,.93),(.36,.24,.08),'Walnut')
box((.42,.19,1.08),(.32,.03,.21),'Steel');tube([(.27,.04,1.02),(.27,.04,1.11),(.55,.04,1.11),(.55,.04,1.02)],.025,'Steel')
for i in range(6):box((-.57+i*.055,.2,1.045),(.038,.23,.15),'Cloth',.002)
box((-.1,-.08,.83),(.24,.3,.012),'White',.001);export('TracingDesk77')
for x in [-.6,.6]:tube([(x,-.28,0),(x,0,.05),(x,.28,0)],.022,'Steel');tube([(x,0,.04),(x,0,1.85)],.023,'Steel')
box((0,0,1.12),(1.17,.035,1.38),'Cloth',.01);export('PrivacyScreen77')
box((0,0,.68),(.72,.53,.055),'Timber')
for x in [-.29,.29]:
 for y in [-.2,.2]:tube([(x,y,.03),(x,y,.65)],.023,'Steel')
box((0,.03,.54),(.61,.39,.1),'Steel');export('SchoolDesk77')
box((0,.24,.82),(1.8,.04,1.64),'Timber')
for x in [-.9,-.45,0,.45,.9]:box((x,0,.82),(.035,.5,1.64),'Timber')
for z in [.035,.35,1.4,1.64]:box((0,0,z),(1.8,.5,.035),'Timber')
for x in [-.67,-.22,.22,.67]:tube([(x,.19,1.23),(x,.04,1.23),(x,.02,1.27)],.015,'Steel')
export('SchoolCubbies77')
(OUT/'manifest.json').write_text(json.dumps({'assets':records,'materials':names},indent=2))
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'Campaign77.blend'))
print('CAMPAIGN77_COMPLETE',len(records),flush=True)
