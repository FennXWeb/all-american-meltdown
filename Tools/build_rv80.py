"""Repair coach joinery and instrument fascia without rebuilding saved furnishings."""
import sys, json
from pathlib import Path
import bpy, bmesh
from mathutils import Vector
sys.path.insert(0, str(Path(__file__).parent))
import make_models_v2 as m
m.OUT=m.ROOT/'ArtSource/RV80'
for key,color in {'Ivory':(.73,.67,.55),'Walnut':(.20,.095,.042),'Black':(.018,.022,.027),'Chrome':(.38,.42,.44),'Brass':(.44,.28,.11),'Glass':(.055,.11,.15),'Light':(.95,.73,.42)}.items():
 m.PALETTE['RV66_'+key]=color
m.setup()
def box(at,size,mat='Ivory',bevel=.008):return m.box(at,size,'RV66_'+mat,bevel)
def finish(name):return m.finish('RV80_'+name,collision='none')

with bpy.data.libraries.load(str(m.ROOT/'ArtSource/RV66/LuxuryCoach66.blend'),link=False) as(src,dst):
 dst.objects=['SM_RV66_Interior']
o=dst.objects[0];bpy.context.collection.objects.link(o);o.location=(0,0,0)
for i,mat in enumerate(o.data.materials):
 key=mat.name.split('.')[0].removeprefix('M_')
 if key in m.MATS:o.data.materials[i]=m.MATS[key]
bm=bmesh.new();bm.from_mesh(o.data);todo=set(bm.verts);bad=[]
while todo:
 start=todo.pop();group={start};stack=[start]
 while stack:
  v=stack.pop()
  for e in v.link_edges:
   other=e.other_vert(v)
   if other in todo:todo.remove(other);group.add(other);stack.append(other)
 lo=Vector(tuple(min(v.co[i] for v in group) for i in range(3)))
 hi=Vector(tuple(max(v.co[i] for v in group) for i in range(3)))
 if lo.x>1.1 and 1.5<((lo+hi)*.5).z<2.05:bad.extend(group)
bmesh.ops.delete(bm,geom=bad,context='VERTS');bm.to_mesh(o.data);bm.free();m.PARTS.append(o)
# Gauges mount ahead of this vertical fascia at x=1.19, never inside the sloped top.
profile=[(1.22,1.49),(1.22,1.94),(1.43,1.94),(2.00,1.78),(2.00,1.49)]
verts=[(x,y,z) for x,z in profile for y in [-1.15,1.15]]
faces=[(2*i,2*((i+1)%5),2*((i+1)%5)+1,2*i+1) for i in range(5)]
faces += [(0,8,6,4,2),(1,3,5,7,9)]
m.mesh('sealed dashboard',verts,faces,'RV66_Black')
box((1.213,-.48,1.775),(.025,.88,.34),'Black',.025)
for y in [-.98,.42,.93]:
 box((1.196,y,1.85),(.014,.20,.09),'Chrome')
 for z in [1.818,1.84,1.862,1.884]:box((1.183,y,z),(.012,.18,.009),'Black',.002)
# Radio bezel, climate bank and passenger glovebox surround.
box((1.206,.04,1.68),(.025,.33,.20),'Chrome',.015)
box((1.185,.04,1.54),(.023,.32,.065),'Black')
for y in [-.045,.04,.125]:m.cyl((1.17,y,1.54),(1.185,y,1.54),.022,'RV66_Chrome',n=16)
box((1.22,.73,1.375),(.11,.76,.22),'Ivory',.025)
box((1.156,.73,1.38),(.022,.69,.15),'Walnut',.014)
box((1.139,.73,1.42),(.012,.17,.018),'Chrome',.003)
finish('Interior')

# Shared sealed firewall: closes the exterior-facing footwell without blocking feet.
box((1.985,0,1.125),(.07,2.38,1.02),'Black')
box((1.966,0,.70),(.10,2.38,.16),'Ivory')
for side in [-1,1]:
 # Slide aperture jambs cover the 5cm clearance on each side of the moving room.
 for x in [-3.94,-1.16]:box((x,side*1.19,1.97),(.105,.15,2.68),'Ivory')
 box((-2.55,side*1.19,3.255),(2.89,.15,.12),'Ivory')
 # Side window rear/front returns join the shell to interior lining.
 for x in [-9.70,-8.00,-6.13,-4.03,-.22]:box((x,side*1.205,2.55),(.075,.105,.98),'Black')
 # Upper nose corners: solid pillars connect the tall side opening to windshield.
 box((2.105,side*1.19,2.43),(.11,.15,1.16),'Ivory')
# Entry jambs and full height lintel. Keep a clear walking aperture below 2.74m.
for x in [-.185,.90]:box((x,1.20,1.70),(.12,.16,2.18),'Ivory')
box((.355,1.20,3.045),(1.20,.16,.61),'Ivory')
box((.94,-1.205,2.53),(.065,.10,1.00),'Black')
finish('Joinery')

# Telescoping U return, scaled only along its deployment axis at runtime.
# The origin is the fixed inboard edge; overlap continues into the moving room.
box((0,.18,0),(2.76,.36,.08),'Walnut',0)
box((0,.18,2.60),(2.76,.36,.08),'Ivory',0)
for x in [-1.355,1.355]:box((x,.18,1.30),(.07,.36,2.60),'Ivory',0)
finish('SlideSeal')

# Flush DIN radio, rocker switches and steering-column controls. Units in metres.
box((.012,0,0),(.042,.275,.125),'Black',.009)
box((-.012,0,0),(.012,.252,.105),'Chrome',.006)
box((-.020,0,0),(.014,.241,.093),'Black',.005)
box((-.029,0,.012),(.004,.154,.043),'Glass',.003)
for i in range(14):box((-.032,-.069+i*.010,.009),(.003,.005,.005+(i%4)*.004),'Light',.001)
for y in [-.104,.104]:
 m.cyl((-.043,y,.005),(-.026,y,.005),.013,'RV66_Chrome',n=20)
 m.cyl((-.046,y,.005),(-.044,y,.005),.010,'RV66_Black',n=20)
for y in [-.066,-.040,-.014,.014,.040,.066]:box((-.031,y,-.027),(.012,.019,.009),'Chrome',.002)
finish('Radio')
box((.008,0,0),(.022,.09,.067),'Chrome',.008)
box((-.003,0,0),(.019,.076,.055),'Black',.006)
box((-.016,0,0),(.018,.050,.037),'Black',.006)
box((-.027,0,.012),(.004,.022,.003),'Light',.001)
finish('Rocker')
m.cyl((0,0,0),(0,.185,-.015),.007,'RV66_Black',n=12)
m.cyl((0,.17,-.014),(0,.235,-.020),.014,'RV66_Black',n=16)
for y in [.184,.2,.22]:m.cyl((0,y,-.016),(0,y+.004,-.017),.0145,'RV66_Chrome',n=16)
finish('Stalk')
m.cyl((-.025,0,0),(.01,0,0),.023,'RV66_Chrome',n=24)
m.cyl((-.027,0,0),(-.026,0,0),.015,'RV66_Black',n=20)
box((-.029,0,0),(.006,.003,.023),'Chrome',.001)
finish('Ignition')
# Fitted lower console for the electric coach, below the existing illuminated dash.
box((1.185,0,1.425),(.12,2.16,.33),'Black',.045)
box((1.24,.06,1.095),(.22,.46,.47),'Black',.035)
box((1.11,.77,1.38),(.035,.57,.21),'Ivory',.018)
box((1.088,.77,1.43),(.01,.16,.016),'Chrome',.003)
finish('ElectricLowerDash')
m.export_all()
(m.OUT/'models_rv80_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2))
bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'CoachRepairs80.blend'))
