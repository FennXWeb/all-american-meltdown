"""Original metric EV meshes. Run Blender --background --python this file."""
import sys,math,json
from pathlib import Path
import bpy,bmesh
from mathutils import Vector
sys.path.insert(0,str(Path(__file__).parent))
import make_models_v2 as m
m.OUT=m.ROOT/'ArtSource/Electric74'
colors={'Pearl':(.50,.67,.71),'Ivory':(.72,.71,.66),'Leather':(.53,.47,.38),'Titanium':(.36,.42,.46),'Carbon':(.035,.043,.052),'Solar':(.018,.04,.075),'Black':(.012,.018,.023),'Light':(.32,.86,1),'Red':(.8,.025,.015),'Glass':(.032,.075,.092),'Display':(.02,.04,.055),'Walnut':(.18,.09,.045)}
for k,v in colors.items():m.PALETTE['EV74_'+k]=v
m.setup()
def box(p,s,mat='Carbon',b=.012):return m.box(p,s,'EV74_'+mat,b)
def line(pts,r=.008,mat='Titanium',n=12):return m.line(pts,r,'EV74_'+mat,n)
def cyl(a,b,r,mat='Titanium',n=24):return m.cyl(a,b,r,'EV74_'+mat,n=n)
def mesh(n,v,f,mat='Pearl'):return m.mesh(n,v,f,'EV74_'+mat)
def finish(n):
 o=m.finish('EV74_'+n)
 if n!='Display':
  o.data.set_sharp_from_angle(angle=.70)
  for p in o.data.polygons:p.use_smooth=True
  bpy.context.view_layer.objects.active=o
  mod=o.modifiers.new('Panel-weighted normals','WEIGHTED_NORMAL');mod.keep_sharp=True
  bpy.ops.object.modifier_apply(modifier=mod.name)
 return o
def append_rv(n):
 with bpy.data.libraries.load(str(m.ROOT/'ArtSource/RV66/LuxuryCoach66.blend'),link=False) as (src,dst):dst.objects=['SM_RV66_'+n]
 o=dst.objects[0];bpy.context.collection.objects.link(o);o.location=(0,0,0)
 aliases={'Pearl':'Pearl','Ivory':'Ivory','Linen':'Leather','Brass':'Titanium','Black':'Carbon','Chrome':'Titanium','Light':'Light','Walnut':'Walnut'}
 for i,mat in enumerate(o.data.materials):
  key=mat.name.split('RV66_')[-1].split('.')[0]
  if key in aliases:o.data.materials[i]=m.MATS['EV74_'+aliases[key]]
 m.PARTS.append(o);return o
def remove_islands(o,pred):
 # Remove complete old assemblies, never tear the long floor/roof faces by
 # deleting only their front vertices.
 bm=bmesh.new();bm.from_mesh(o.data);todo=set(bm.verts);bad=[]
 while todo:
  start=todo.pop();group={start};stack=[start]
  while stack:
   v=stack.pop()
   for e in v.link_edges:
    other=e.other_vert(v)
    if other in todo:todo.remove(other);group.add(other);stack.append(other)
  lo=Vector(tuple(min(v.co[i] for v in group) for i in range(3)));hi=Vector(tuple(max(v.co[i] for v in group) for i in range(3)))
  if pred((lo+hi)*.5,lo,hi):bad.extend(group)
 bmesh.ops.delete(bm,geom=bad,context='VERTS');bm.to_mesh(o.data);bm.free()
def xloft(name,rings,mat='Pearl',n=32):
 # Longitudinal, elliptical body panels with shared edge loops and smooth silhouette.
 verts=[]
 for x,w,bottom,top in rings:
  for i in range(n):
   a=i*math.tau/n;verts.append((x,w*math.cos(a),(top+bottom)/2+(top-bottom)/2*math.sin(a)))
 faces=[]
 for k in range(len(rings)-1):
  for i in range(n):j=(i+1)%n;faces.append((k*n+i,k*n+j,(k+1)*n+j,(k+1)*n+i))
 faces.extend([tuple(range(n-1,-1,-1)),tuple((len(rings)-1)*n+i for i in range(n))]);return mesh(name,verts,faces,mat)
def boolean(obj,cut):
 bpy.context.view_layer.objects.active=obj;mod=obj.modifiers.new('Engineered opening','BOOLEAN');mod.operation='DIFFERENCE';mod.solver='EXACT';mod.object=cut;bpy.ops.object.modifier_apply(modifier=mod.name);bpy.data.objects.remove(cut,do_unlink=True)
def seat(p):
 x,y,z=p
 m.loft([(x,y,z,.23,.24),(x+.015,y,z+.10,.26,.27),(x,y,z+.15,.23,.24)],'EV74_Leather',24)
 m.loft([(x-.22,y,z+.08,.075,.255),(x-.29,y,z+.40,.09,.25),(x-.36,y,z+.65,.075,.21),(x-.37,y,z+.77,.06,.15)],'EV74_Leather',24)
 for s in [-1,1]:line([(x+.20,y+s*.21,z+.10),(x-.20,y+s*.23,z+.14),(x-.34,y+s*.18,z+.65)],.012,'Titanium')
 for zz in [.25,.34,.43,.52]:line([(x-.23-zz*.15,y-.15,z+zz),(x-.23-zz*.15,y+.15,z+zz)],.006,'Carbon')
 box((x-.35,y,z+.80),(.15,.28,.18),'Leather',.045)

# Coach structural shell retains all real window, entry, slide and wheel apertures.
o=append_rv('Shell');remove_islands(o,lambda c,lo,hi:c.x>2.05 or lo.z>3.40)
# Rounded, broad front fascia continuous with the retained windscreen sill.
profile=[(-1.12,.39),(-1.23,.48),(-1.23,1.33),(-1.13,1.49),(1.13,1.49),(1.23,1.33),(1.23,.48),(1.12,.39)]
vv=[(x,y*w,z) for x,w in [(1.92,1),(2.16,1),(2.28,.93)] for y,z in profile]
ff=[tuple(range(7,-1,-1)),tuple(range(16,24))]+[(j*8+i,j*8+(i+1)%8,(j+1)*8+(i+1)%8,(j+1)*8+i) for j in range(2) for i in range(8)]
mesh('contoured coach fascia',vv,ff)
line([(2.30,-1.04,.55),(2.34,0,.52),(2.30,1.04,.55)],.055,'Carbon')
line([(2.29,-1.04,1.20),(2.37,-.64,1.21),(2.40,0,1.215),(2.37,.64,1.21),(2.29,1.04,1.20)],.025,'Light')
for side in [-1,1]:
 line([(1.81,side*1.17,1.48),(1.66,side*1.2,2.9),(1.45,side*1.15,3.32)],.045,'Titanium')
 line([(-9.7,side*1.302,.95),(-8.3,side*1.302,.92),(-4.1,side*1.302,.95),(-.30,side*1.302,1.13)],.018,'Light')
 line([(-9.7,side*1.28,3.28),(-6,side*1.28,3.42),(-1,side*1.28,3.44),(1.45,side*1.16,3.33)],.026,'Titanium')
 for x in [-9.55,-.85]:
  box((x,side*1.28,2.99),(.20,.05,.075),'Black');box((x,side*1.31,2.99),(.13,.009,.025),'Light',.008)
line([(-9.91,-1.05,2.78),(-9.94,0,2.83),(-9.91,1.05,2.78)],.025,'Red')
box((-4.1,0,3.50),(10.35,2.26,.05),'Carbon',.04)
for i in range(10):
 for side in [-1,1]:box((-8.76+i*.98,side*.56,3.54),(.93,1.065,.025),'Solar',.006)
for x in [-9.4,1.1]:
 xloft('roof sensor fairing',[(x-.18,.13,3.46,3.48),(x,.18,3.46,3.66),(x+.2,.13,3.46,3.54)],'Pearl',16)
finish('CoachShell')
o=append_rv('Interior');remove_islands(o,lambda c,lo,hi:lo.x>1.1 and 1.5<c.z<2.05)
# Floating dashboard with a clear sightline, shallow storage and ambient strip.
xloft('cockpit console',[(1.12,1.08,1.51,1.60),(1.4,1.14,1.48,1.72),(1.92,1.10,1.45,1.68)],'Carbon',24)
line([(1.15,-1.05,1.63),(1.20,-.45,1.69),(1.22,.3,1.69),(1.18,1.07,1.63)],.011,'Light')
for side in [-1,1]:
 for a,b in [(-9.6,-8.2),(-6.02,-4.05),(-1.10,1.13)]:line([(a,side*1.16,3.22),(b,side*1.16,3.22)],.016,'Light')
 for x in [-8.75,-5.08]:box((x,side*1.167,1.26),(.6,.017,.10),'Titanium')
 for a,b in [(-8.0,-6.2),(-5.9,-4.1),(-3.8,-1.2)]:line([(a,side*.43,.68),(b,side*.43,.68)],.006,'Light')
finish('CoachInterior')

# Low hypercar: continuous surfaces, wheel arches and an actual open cockpit.
body=xloft('monocoque', [(-2.77,.84,.29,.76),(-2.6,1.01,.22,.98),(-2.13,1.04,.20,1.00),(-1.7,1.01,.18,.92),(-1.1,.88,.17,.89),(-.4,.87,.19,.87),(.25,.97,.20,.92),(.9,1.02,.23,.91),(1.50,1.00,.29,.86),(1.92,.91,.35,.64),(2.07,.77,.42,.58)],n=48)
bpy.ops.mesh.primitive_cube_add(size=1,location=(-.67,0,1.45));cut=bpy.context.object;cut.scale=(2.25,1.50,1.6);bpy.ops.object.transform_apply(location=False,rotation=False,scale=True);boolean(body,cut)
for x in [1.32,-1.53]:
 bpy.ops.mesh.primitive_cylinder_add(vertices=40,radius=.415,depth=2.7,location=(x,0,.355),rotation=(math.pi/2,0,0));boolean(body,bpy.context.object)
# Tapered canopy roof, window pillars and sculpted side sills. Glazing is separate.
xloft('canopy roof',[(-1.88,.64,1.06,1.11),(-1.42,.73,1.23,1.30),(-.9,.73,1.27,1.34),(-.34,.66,1.22,1.29)],'Solar',32)
for side in [-1,1]:
 # Closed sculpted doors with a continuous shoulder: cabin opening is only
 # above the belt line, rather than missing the entire side of the body.
 rows=[(-1.35,.80),(-.92,.91),(-.45,.91),(.05,.89),(.62,.84)]
 vs=[]
 for x,w in rows:
  vs.extend([(x,side*(w-.06),.34),(x,side*(w+.012),.60),(x,side*w,.87),(x,side*(w-.025),.92)])
 faces=[(j*4+k,j*4+k+1,(j+1)*4+k+1,(j+1)*4+k) for j in range(4) for k in range(3)]
 mesh('sculpted door',vs,faces,'Pearl')
 line([(-1.32,side*.79,.94),(-.9,side*.885,.955),(-.45,side*.885,.955),(.06,side*.87,.945),(.62,side*.83,.93)],.022,'Carbon')
 box((-.97,side*.902,.855),(.20,.015,.035),'Titanium',.007)
 line([(.70,side*.79,.91),(.12,side*.77,1.12),(-.34,side*.66,1.27)],.026,'Titanium')
 line([(-2.12,side*.89,.95),(-1.88,side*.64,1.10),(-1.42,side*.73,1.27)],.035,'Pearl')
 line([(-1.44,side*.81,.87),(-1.36,side*.78,1.26)],.043,'Carbon')
 line([(-2.09,side*.99,.48),(-1.15,side*.96,.27),(.73,side*1.0,.30)],.054,'Carbon')
 line([(-1.36,side*.88,.77),(-1.20,side*.90,.51),(.72,side*.91,.56),(.75,side*.87,.81)],.004,'Titanium')
 # Sculpted rear intake recess and floating aero bridge.
 mesh('rear intake',[(-1.82,side*1.027,.83),(-.98,side*.91,.71),(-1.46,side*.98,.42),(-1.84,side*1.04,.51)],[(0,1,2,3)],'Black')
 line([(-1.92,side*1.045,.86),(-1.0,side*.93,.75)],.03,'Pearl')
 line([(1.99,side*.34,.60),(1.76,side*.83,.68),(1.3,side*.97,.79)],.023,'Light')
 line([(-2.79,side*.12,.73),(-2.79,side*.70,.78),(-2.62,side*.94,.83)],.022,'Red')
 line([(.25,side*.88,.97),(.30,side*1.07,1.0)],.022,'Titanium');box((.25,side*1.10,1.0),(.20,.12,.08),'Carbon',.025)
 for xx in [-2.5,-2.39,-2.28,-2.17]:box((xx,side*.56,.985),(.045,.27,.012),'Black',.005)
for yy in [-.75,-.35,.35,.75]:box((-2.68,yy,.26),(.35,.023,.14),'Carbon',.01)
box((-2.45,0,1.10),(.26,1.78,.045),'Carbon',.014)
for side in [-1,1]:box((-2.40,side*.58,.96),(.11,.025,.26),'Titanium')
box((1.75,0,.34),(.45,1.7,.055),'Carbon',.014)
finish('HyperShell')
# Front, side and rear glass surfaces, with no opaque full-width cabin overlay.
for vs in [[(.71,-.78,.93),(.71,.78,.93),(-.34,.66,1.23),(-.34,-.66,1.23)], [(-2.07,-.84,.98),(-1.85,-.63,1.1),(-1.85,.63,1.1),(-2.07,.84,.98)]]:mesh('glazing',vs,[(0,1,2,3)],'Glass')
for s in [-1,1]:mesh('side window',[(.65,s*.795,.90),(-1.96,s*.87,.95),(-1.39,s*.74,1.25),(-.35,s*.67,1.24)],[(0,1,2,3)],'Glass')
finish('HyperGlass')
box((-.66,0,.35),(2.3,1.48,.075),'Carbon',.025)
for side in [-1,1]:
 seat((-.48,side*.43,.39))
 box((-.65,side*.775,.65),(2.08,.075,.39),'Carbon',.04)
 line([(.48,side*.727,.83),(-.5,side*.727,.84),(-1.58,side*.75,.84)],.009,'Light')
 box((-.55,side*.71,.68),(.28,.05,.065),'Leather',.02)
box((-.45,0,.58),(1.55,.15,.36),'Carbon',.035)
for x in [-.64,-.37]:cyl((x,0,.77),(x,0,.78),.049,'Titanium')
xloft('hyper dashboard',[(.04,.72,.73,.79),(.29,.76,.73,.88),(.6,.78,.70,.88)],'Carbon',24)
line([(.08,-.71,.80),(.05,0,.82),(.08,.71,.80)],.008,'Light')
for side in [-1,1]:
 box((.08,side*.64,.79),(.02,.18,.035),'Titanium',.004)
 for y in range(8):box((.065,side*.64-.077+y*.022,.79),(.01,.005,.027),'Black',.001)
for yy in [-.5,-.34]:box((.02,yy,.46),(.09,.065,.12),'Titanium',.005)
finish('HyperInterior')

# Turbine wheel and machined brakes, origin at wheel hub; steering remains articulated.
cyl((0,-.115,0),(0,.115,0),.353,'Black',48)
for side in [-1,1]:
 cyl((0,side*.116,0),(0,side*.12,0),.269,'Carbon',48)
 cyl((0,side*.121,0),(0,side*.126,0),.11,'Titanium',32)
 for i in range(10):
  a=i*math.tau/10;b=a+.19
  vs=[(.09*math.cos(a),side*.13,.09*math.sin(a)),(.28*math.cos(b),side*.123,.28*math.sin(b)),(.28*math.cos(b+.16),side*.123,.28*math.sin(b+.16)),(.09*math.cos(a+.27),side*.13,.09*math.sin(a+.27))];mesh('turbine spoke',vs,[(0,1,2,3)],'Titanium')
finish('Wheel')
line([(0,-.19,.04),(0,-.18,-.08),(0,-.12,-.125),(0,.12,-.125),(0,.18,-.08),(0,.19,.04)],.023,'Leather',16)
box((0,0,-.005),(.05,.15,.07),'Carbon',.018)
for side in [-1,1]:line([(0,side*.065,-.005),(0,side*.16,-.05)],.019,'Titanium')
finish('Yoke')
# Dashboard faces -X; explicit UVs keep text unmirrored from driver side.
box((.012,0,0),(.038,.62,.385),'Black',.014)
box((.045,0,-.22),(.035,.10,.18),'Titanium',.008);finish('DashFrame')
display=mesh('display',[(-.012,-.292,-.173),(-.012,.292,-.173),(-.012,.292,.173),(-.012,-.292,.173)],[(0,1,2,3)],'Display');o=finish('Display')
for poly in o.data.polygons:
 for li in poly.loop_indices:
  v=o.data.vertices[o.data.loops[li].vertex_index].co;o.data.uv_layers.active.data[li].uv=((v.y+.292)/.584,(v.z+.173)/.346)
box((0,0,0),(.027,.22,.30),'Titanium',.012);box((-.017,0,.02),(.009,.20,.25),'Display',.008)
for z in [-.085,-.045,0,.045]:line([(-.024,-.07,z),(-.024,.07,z)],.005,'Light')
finish('ControlPanel')
box((0,0,.15),(.30,.27,.30),'Titanium',.022);box((.155,0,.11),(.015,.21,.15),'Black',.008)
cyl((.20,0,.08),(.20,0,.135),.038,'Leather');line([(.16,0,.23),(.2,0,.23),(.2,0,.18)],.012,'Titanium')
box((.15,0,.255),(.012,.17,.05),'Display',.005);finish('Coffee')
box((0,0,.44),(.30,.44,.88),'Walnut',.02);box((0,0,.90),(.32,.45,.035),'Titanium',.014)
box((-.12,0,1.4),(.025,.42,.62),'Glass',.02);line([(-.10,0,.92),(-.10,0,1.12),(.02,0,1.12)],.008,'Titanium')
finish('Spa')
m.export_all();(m.OUT/'models_electric74_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values()),'palette':colors},indent=2))
bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'Electric74.blend'))
print('ELECTRIC74_MODELS_COMPLETE',len(m.RECORDS))
