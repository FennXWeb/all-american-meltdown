"""Luxury coach: authored metric meshes, +X nose, full scale cabin and moving rooms."""
import sys, math, json
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent))
import make_models_v2 as m
import bpy
m.OUT=m.ROOT/'ArtSource/RV66'
colors={'Pearl':(.58,.62,.65),'Walnut':(.20,.095,.042),'Ivory':(.73,.67,.55),'Linen':(.49,.55,.54),'Quartz':(.77,.78,.72),'Chrome':(.38,.42,.44),'Black':(.018,.022,.027),'Glass':(.055,.11,.15),'Brass':(.44,.28,.11),'Light':(.95,.73,.42),'Water':(.11,.42,.57),'Screen':(.035,.13,.11)}
for n,c in colors.items():m.PALETTE['RV66_'+n]=c
m.setup()
def box(p,s,mat='Walnut',b=.008):return m.box(p,s,'RV66_'+mat,b)
def cyl(a,b,r,mat='Chrome',n=16):return m.cyl(a,b,r,'RV66_'+mat,n=n)
def line(p,r=.007,mat='Chrome',n=10):return m.line(p,r,'RV66_'+mat,n)
def finish(n,coll='none'):m.finish('RV66_'+n,collision=coll)
def cushion(p,w,d,h,mat='Ivory'):
 x,y,z=p;m.loft([(x,y,z,w*.46,d*.46),(x,y,z+h*.5,w*.5,d*.5),(x,y,z+h,w*.46,d*.46)],'RV66_'+mat,24)
 for yy in [-1,1]:line([(x-w*.40,y+yy*d*.47,z+h*.62),(x+w*.40,y+yy*d*.47,z+h*.62)],.003,'Linen',6)
def handle(x,y,z,w=.22):
 line([(x-w/2,y,z),(x-w/2,y-.03,z),(x+w/2,y-.03,z),(x+w/2,y,z)],.009)
def cabinet(w=.8,h=.86,d=.53):
 box((0,.012,h/2+.03),(w,d,h-.06));box((0,0,.05),(w-.08,d-.06,.10),'Black')
 for z,hh in [(h*.25,h*.39),(h*.64,h*.34),(h*.89,h*.14)]:
  box((0,-d/2-.01,z),(w-.03,.035,hh));handle(0,-d/2-.03,z+hh*.25,w*.3)
 box((0,0,h+.02),(w+.04,d+.035,.045),'Quartz',.015)

# Exterior shell: walls are explicitly cut around the entry, glazing and two slide rooms.
box((-3.85,0,.565),(12.08,2.55,.11),'Walnut')
box((-3.85,0,3.40),(12.08,2.53,.12),'Pearl',.04)
box((-9.84,0,1.95),(.10,2.54,2.83),'Pearl',.03)
box((-9.77,0,1.93),(.035,2.40,2.76),'Ivory')
for side in [-1,1]:
 y=side*1.245
 # skirt sections avoid three wheel arches, with narrow pillars between wheel openings
 for a,b in [(-9.88,-8.04),(-7.12,-6.54),(-5.56,.80),(1.86,2.18)]:box(((a+b)/2,y,.40),(b-a,.08,.46),'Pearl',.018)
 for a,b in ([(-9.88,-.22),(.94,2.18)] if side==1 else [(-9.88,2.18)]):box(((a+b)/2,y,.82),(b-a,.10,.34),'Pearl')
 box((-3.85,y,3.19),(12.05,.10,.35),'Pearl')
 # slide aperture x=-3.9..-1.2. door aperture x=-.18.. .88 on curb side.
 segments=[(-9.88,-3.96),(-1.14,-.22),(.94,2.12)] if side==1 else [(-9.88,-3.96),(-1.14,2.12)]
 for a,b in segments:
  box(((a+b)/2,y,1.52),(b-a,.10,1.03),'Pearl')
  # side window ribbon broken by credible narrow posts
  for x in [a+.045,b-.045]:box((x,y,2.49),(.09,.10,1.03),'Pearl')
  box(((a+b)/2,y,3.01),(b-a,.10,.10),'Black')
 for x in [-9.7,-8.0,-6.13,-4.03,-1.2,-.22,.94]:
  if side==-1 and x==.94:continue
  box((x,y,2.50),(.065,.105,1.00),'Black')
 # contrasting touring belt and lower polished rub rail
 for a,b in ([(-9.80,-.22),(.94,1.80)] if side==1 else [(-9.80,1.80)]):
  box(((a+b)/2,y+side*.055,1.06),(b-a,.018,.105),'Black')
  box(((a+b)/2,y+side*.058,.71),(b-a,.012,.032),'Chrome')
 # gently tapered nose sheets and roof corner/edge mouldings
 line([(-9.8,y,3.4),(-8,y,3.47),(-1,y,3.47),(1.45,y,3.38),(1.97,y,3.20)],.055,'Pearl',12)
for z,xx,wide in [(1.01,2.18,2.38),(1.2,2.20,2.4),(1.42,2.12,2.35)]:box((xx,0,z),(.15,wide,.22),'Pearl',.06)
box((1.70,0,3.105),(.21,2.39,.47),'Pearl',.055)
box((2.14,0,.58),(.19,2.37,.40),'Pearl',.06)
box((2.245,0,.59),(.018,1.36,.23),'Black',.026)
for z in [.51,.57,.63,.69]:box((2.26,0,z),(.012,1.29,.016),'Chrome',.003)
box((2.19,0,.87),(.10,1.3,.22),'Black',.035)
for yy in [-1,1]:
 box((2.215,yy*.95,1.18),(.022,.30,.24),'Black',.025)
 for z in [1.14,1.25]:box((2.23,yy*.95,z),(.015,.245,.045),'Light',.008)
 line([(2.03,yy*1.14,1.5),(1.83,yy*1.14,2.90),(1.43,yy*1.14,3.35)],.053,'Pearl',12)
 # aerodynamic mirror pods with stalks
 line([(1.83,yy*1.25,2.10),(1.90,yy*1.49,2.20),(1.80,yy*1.52,2.45)],.026,'Chrome')
 cushion((1.79,yy*1.53,2.26),.15,.12,.32,'Pearl')
box((-.6,0,3.53),(1.05,.86,.20),'Pearl',.05)
box((-6.1,0,3.53),(1.05,.86,.20),'Pearl',.05)
finish('Shell')

# Interior architecture, curved overhead valances, plank floor, headliner and dashboard.
box((-3.85,0,.625),(11.90,2.37,.025),'Walnut')
for x in range(28):box((-9.55+x*.416,0,.640),(.004,2.32,.001),'Black',0)
box((-3.85,0,3.32),(11.88,2.36,.035),'Ivory')
for side in [-1,1]:
 for a,b in [(-9.72,-4.0),(-1.15,1.85)]:
  if side==1 and b==1.85:a=.94
  box(((a+b)/2,side*1.184,1.42),(b-a,.035,1.53),'Ivory')
 line([(-9.6,side*1.13,3.28),(-4.0,side*1.13,3.28)],.018,'Light')
 line([(-1.1,side*1.13,3.28),(1.4,side*1.13,3.28)],.018,'Light')
# Wheel wells sealed on all sides; cockpit floor is continuous behind dash.
for side in [-1,1]:
 for x in [1.32,-6.05,-7.55]:box((x,side*.99,.85),(1.0,.37,.43),'Ivory',.08)
# raised shaped instrument binnacle, lower footwell remains open
m.mesh('dashboard',[(x,y,z) for x,z in [(1.25,1.62),(1.42,1.94),(2.02,1.75),(1.95,1.54)] for y in [-1.15,1.15]],[(0,2,3,1),(2,4,5,3),(4,6,7,5),(6,0,1,7),(0,6,4,2),(1,3,5,7)],'RV66_Black')
# vertical gauge carrier consistent with existing instrument positions
box((1.415,-.46,1.765),(.032,.82,.37),'Black',.028)
for x in [-8.0,-6.12]:
 for side in [-1,1]:box((x,side*.90,1.98),(.055,.55,2.66),'Walnut')
# decorative divider at master bed, with open central passage
for side in [-1,1]:box((-8.05,side*.97,2.0),(.045,.40,2.65),'Walnut')
finish('Interior')

# Slide rooms: origin at stowed centre. Furniture moves with these room assemblies.
for side,name in [(-1,'DiningSlide'),(1,'LoungeSlide')]:
 box((0,0,.01),(2.72,.74,.10),'Walnut')
 box((0,side*.36,1.22),(2.72,.075,2.48),'Pearl')
 # real window aperture, replace middle backing with frame geometry
 o=m.PARTS.pop();bpy.data.objects.remove(o,do_unlink=True)
 for x in [-1.32,1.32]:box((x,side*.36,1.28),(.08,.075,2.55),'Pearl')
 for z,hh in [(.55,1.1),(2.46,.24)]:box((0,side*.36,z),(2.70,.075,hh),'Pearl')
 for x in [-1.34,1.34]:box((x,0,1.28),(.055,.73,2.55),'Ivory')
 box((0,0,2.58),(2.76,.79,.075),'Pearl')
 box((0,side*.32,1.06),(2.56,.035,.09),'Black')
 box((0,side*.30,.63),(2.56,.022,1.16),'Ivory')
 line([(-1.25,-side*.25,2.51),(1.25,-side*.25,2.51)],.018,'Light')
 finish(name)

cabinet(.83,.87,.54);box((0,0,.913),(.60,.38,.025),'Black',.025)
for x in [-.16,.16]:
 m.tube((x,0,.929),(x,0,.934),.10,.088,'RV66_Chrome',24)
 cyl((x,-.185,.915),(x,-.185,.94),.020,'Brass')
finish('Cooker','convex')
cabinet(.83,.87,.54)
# Remove solid counter top and frame a deep recessed stainless basin.
o=m.PARTS.pop();bpy.data.objects.remove(o,do_unlink=True)
for x in [-.345,.345]:box((x,0,.89),(.18,.58,.045),'Quartz')
for y in [-.24,.24]:box((0,y,.89),(.51,.10,.045),'Quartz')
v=[(x,y,z) for z,w,d in [(.892,.25,.19),(.70,.20,.145)] for x,y in [(-w,-d),(w,-d),(w,d),(-w,d)]]
m.mesh('recessed sink',v,[(0,1,5,4),(1,2,6,5),(2,3,7,6),(3,0,4,7),(4,5,6,7)],'RV66_Chrome')
cyl((0,0,.701),(0,0,.704),.027,'Black')
line([(0,.23,.91),(0,.23,1.13),(0,.13,1.22),(0,-.01,1.18),(0,-.02,1.10)],.017,'Brass',16)
cyl((.17,.22,.9),(.17,.22,.98),.019,'Brass');finish('Sink','convex')
cabinet(.75,1.88,.56)
for z in [.52,1.45]:box((0,-.30,z),(.7,.05,.78),'Chrome',.02);handle(.15,-.34,z,.25)
box((.05,-.334,1.65),(.24,.009,.08),'Screen');finish('Fridge','convex')
cabinet(.86,.48,.36);finish('Overhead','convex')
cabinet(.65,2.1,.49);finish('Wardrobe','convex')

# Leather dinette seats and sculpted captain chairs with piping, armrests and swivel pedestal.
for name,back in [('Chair',1.09),('Dinette',.92)]:
 cyl((0,0,.04),(0,0,.26),.115,'Chrome');cyl((0,0,.03),(0,0,.06),.27,'Black',24)
 cushion((0,0,.29),.52,.53,.13)
 cushion((-.24,0,.40),.17,.56,back-.40)
 if name=='Chair':
  cushion((-.22,0,back),.14,.31,.20)
  for side in [-1,1]:box((-.04,side*.30,.58),(.48,.072,.07),'Ivory',.025)
 for y in [-.16,0,.16]:line([(-.145,y,.49),(-.16,y,back-.06)],.003,'Linen',6)
 finish(name,'convex')
cushion((0,0,.25),1.75,.60,.15);box((0,.28,.53),(1.82,.13,.68),'Ivory',.06)
for x in [-.89,.89]:box((x,0,.47),(.13,.69,.40),'Ivory',.04)
for x in [-.44,.44]:cushion((x,.09,.60),.39,.18,.35,'Linen')
box((0,0,.13),(1.74,.53,.26),'Walnut');finish('Sofa','convex')
cyl((0,0,.03),(0,0,.69),.065,'Chrome');box((0,0,.03),(.43,.43,.045),'Black',.025)
box((0,0,.735),(.89,.62,.07),'Quartz',.055)
for x in [-.27,.27]:
 cyl((x,0,.774),(x,0,.789),.115,'Ivory',32)
 cyl((x,.17,.78),(x,.17,.87),.037,'Ivory',24)
 line([(x+.04,.17,.80),(x+.065,.17,.80),(x+.065,.17,.85),(x+.04,.17,.85)],.006,'Brass')
finish('Table','convex')

# A bunk is a sleeping platform, padded mattress, pillow, quilt and privacy rail.
box((0,0,.075),(1.90,.70,.15),'Walnut',.015)
cushion((0,0,.15),1.83,.65,.10,'Linen')
cushion((-.65,0,.25),.35,.54,.085,'Ivory')
box((.2,0,.273),(1.15,.625,.035),'Linen',.025)
for x in [-.83,.83]:cyl((x,-.34,.08),(x,-.34,.43),.014,'Brass')
line([(-.83,-.34,.43),(.2,-.34,.43)],.013,'Brass')
finish('Bunk','convex')
for x in [-.14,.14]:cyl((x,0,0),(x,0,2.21),.018,'Brass')
for z in [.24,.55,.86,1.17,1.48,1.79,2.10]:cyl((-.14,0,z),(.14,0,z),.013,'Brass')
finish('Ladder','convex')
box((0,0,.25),(1.40,2.04,.5),'Walnut',.03)
cushion((0,0,.5),1.46,2.04,.17,'Ivory')
box((.16,0,.69),(1.10,2.0,.05),'Linen',.025)
for y in [-.49,.49]:cushion((-.48,y,.67),.40,.77,.13,'Ivory')
box((-.73,0,.64),(.06,2.1,1.2),'Walnut',.02)
for y in [-.70,-.35,0,.35,.70]:box((-.69,y,.89),(.055,.31,.60),'Ivory',.03)
finish('MasterBed','convex')

# Hardware and accessories all have separate interaction pivots.
box((0,0,0),(.032,1.04,.62),'Black',.018);box((-.022,0,0),(.004,.96,.54),'Screen',.003)
for y in [-.42,.42]:box((-.027,y,-.285),(.007,.07,.008),'Chrome')
finish('TV','convex')
o=m.OBJECTS['RV66_TV']
for p in o.data.polygons:
 if o.data.materials[p.material_index].name=='M_RV66_Screen':
  for i in p.loop_indices:
   v=o.data.vertices[o.data.loops[i].vertex_index].co;o.data.uv_layers.active.data[i].uv=((v.y+.48)/.96,(v.z+.27)/.54)
box((0,0,0),(.015,.073,.10),'Brass');box((-.014,0,0),(.018,.047,.055),'Ivory');finish('Switch','convex')
box((0,0,-.5),(1,.018,1),'Linen',.002)
for z in range(50):box((0,-.013,-z*.02-.01),(1,.007,.003),'Ivory',0)
box((0,0,-.995),(1.02,.028,.023),'Brass');finish('Shade')
box((0,.5,0),(7.0,1.0,.025),'Ivory',.01)
for x in [-3.45,3.45]:line([(x,0,-.02),(x,.45,-.05),(x,1,-.08)],.023,'Chrome')
for x in range(35):box((-3.4+x*.20,.5,.014),(.07,1,.003),'Linen',0)
finish('Awning')
cyl((0,0,0),(0,0,-.36),.007,'Water',10);finish('Water')
box((0,0,.12),(.42,.04,.24),'Walnut')
art=m.mesh('art print',[(-.18,-.027,.03),(.18,-.027,.03),(.18,-.027,.21),(-.18,-.027,.21)],[(0,1,2,3)],'RV66_Linen');finish('Art')
# Restore a full, correctly oriented UV rectangle on the framed print.
o=m.OBJECTS['RV66_Art']
for p in o.data.polygons:
 if o.data.materials[p.material_index].name=='M_RV66_Linen':
  for i in p.loop_indices:
   v=o.data.vertices[o.data.loops[i].vertex_index].co;o.data.uv_layers.active.data[i].uv=((v.x+.18)/.36,(v.z-.03)/.18)
box((0,0,0),(.60,.35,.035),'Ivory',.014)
for i in range(16):box((-.265+i*.035,0,-.023),(.014,.28,.008),'Black',.002)
finish('CeilingVent')
cyl((0,0,-.017),(0,0,.017),.085,'Brass',24);cyl((0,0,-.023),(0,0,-.018),.070,'Light',24);finish('Downlight')
box((0,0,0),(.10,.018,.13),'Ivory',.012)
for x in [-.026,.026]:
 for z in [-.025,.025]:box((x,-.012,z),(.008,.003,.018),'Black',.001)
finish('Outlet','convex')
cyl((0,0,0),(0,0,.17),.075,'Ivory',24)
for i in range(9):
 a=i*math.tau/9;line([(0,0,.15),(.05*math.cos(a),.05*math.sin(a),.3),(.12*math.cos(a),.12*math.sin(a),.36)],.013,'Linen')
finish('Plant')
box((0,0,1.0),(.44,.28,2.0),'Pearl',.03);box((0,-.157,1.59),(.40,.04,.42),'Black',.02)
box((0,-.18,1.60),(.35,.006,.30),'Screen');box((0,-.19,1.20),(.20,.025,.04),'Black');finish('DeliveryTerminal','convex')
m.export_all()
(m.OUT/'models_rv66_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values()),'palette':colors},indent=2))
bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'LuxuryCoach66.blend'))
print('RV66_MODELS_COMPLETE',len(m.RECORDS))
