"""Casino furnishings and original mid-engine supercar. Blender, metres, +X forward."""
import sys,math,json
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent));import make_models_v2 as m
import bpy
m.OUT=m.ROOT/'ArtSource/ModelsV21';m.PALETTE.update({'CasinoGold21':(.66,.43,.12),'CasinoVelvet21':(.22,.035,.065),'CasinoMarble21':(.64,.62,.52),'SuperOrange21':(.94,.28,.025),'Carbon21':(.025,.028,.03),'WindshieldV9':(.06,.09,.10) });m.setup()
# Tapered slot cabinet with recessed reels, coin tray and illuminated crown.
m.profile([(-.37,0),(.35,0),(.30,1.72),(.10,1.94),(-.21,1.94),(-.30,1.18),(-.43,.97)],.68,'Carbon21')
# Profiles extrude along Y; turn cabinet to face -Y for room placement.
for o in m.PARTS:o.rotation_euler.z=math.pi/2
m.box((0,-.30,1.48),(.59,.075,.39),'CasinoGold21',.02);m.box((0,-.346,1.48),(.52,.025,.29),'Rubber',.008)
for x in [-.18,0,.18]:m.box((x,-.364,1.48),(.153,.012,.25),'Bone',.008)
m.box((0,-.45,1.05),(.72,.28,.075),'CasinoGold21',.025);m.box((.18,-.50,1.10),(.14,.08,.025),'Red',.01);m.box((0,-.38,.56),(.49,.17,.07),'Steel',.02)
m.box((0,-.20,1.92),(.68,.17,.22),'CasinoGold21',.025);m.box((0,-.293,1.92),(.53,.012,.12),'Glow',.006);m.finish('SlotMachineV21',collision='complex')
# Plush lounge chair.
m.box((0,0,.42),(.78,.73,.20),'CasinoVelvet21',.09);m.box((0,.28,.81),(.78,.18,.76),'CasinoVelvet21',.08)
for x in [-.34,.34]:m.box((x,0,.64),(.16,.74,.22),'CasinoVelvet21',.06);m.cyl((x,.23,.03),(x,.23,.36),.028,'CasinoGold21');m.cyl((x,-.23,.03),(x,-.23,.36),.028,'CasinoGold21')
m.finish('CasinoChairV21',collision='complex')
m.cyl((0,0,0),(0,0,4.04),.23,'CasinoMarble21',n=16)
for z in [.10,3.94]:m.cyl((0,0,z-.10),(0,0,z+.10),.34,'CasinoGold21',n=16)
m.finish('CasinoColumnV21',collision='convex')
# Chandelier hangs from origin, ornaments below ceiling.
m.cyl((0,0,0),(0,0,-.48),.025,'CasinoGold21')
for ring,r in [(0,.75),(1,.46)]:
 z=-.5-ring*.38
 for i in range(16):
  a=i*math.tau/16;b=(i+1)*math.tau/16;m.cyl((r*math.cos(a),r*math.sin(a),z),(r*math.cos(b),r*math.sin(b),z),.025,'CasinoGold21')
  m.cyl((r*math.cos(a),r*math.sin(a),z),(r*math.cos(a),r*math.sin(a),z-.26),.022,'Glow',r2=.007,n=6)
m.finish('CasinoChandelierV21',collision='none')
m.box((0,0,.53),(2.25,.74,1.06),'CasinoVelvet21',.035);m.box((0,0,1.1),(2.35,.82,.10),'CasinoMarble21',.02)
for x in [-1,-.5,0,.5,1]:m.box((x,-.38,.52),(.024,.025,.94),'CasinoGold21')
m.finish('CasinoCounterV21',collision='complex')
m.box((0,0,.42),(1.0,.45,.84),'Wood',.02)
for z in [.12,.48,.83]:m.box((0,0,z),(1.1,.5,.045),'CasinoGold21')
for i in range(6):m.cyl((-.42+i*.17,0,.86),(-.42+i*.17,0,1.12),.045,'Glass',n=8);m.cyl((-.42+i*.17,0,1.12),(-.42+i*.17,0,1.22),.018,'Glass',n=8)
m.finish('CasinoBarShelfV21',collision='complex')
# Supercar surface strips: sculpted shoulders, genuine open wheel arches and side intake cavities.
xs=[-2.30,-2.12,-1.80,-1.38,-1.0,-.7,-.25,.25,.7,1.05,1.32,1.62,1.94,2.2]
width=[.80,.94,1.,1.,.94,.89,.87,.88,.94,.99,1.,.96,.87,.75]
height=[.76,.88,1.00,1.07,.93,.80,.75,.77,.86,.93,1.02,.91,.69,.51]
base_x,base_w,base_h=xs[:],width[:],height[:]
def interp(x,values):
 for i in range(len(base_x)-1):
  if base_x[i]<=x<=base_x[i+1]:
   t=(x-base_x[i])/(base_x[i+1]-base_x[i]);return values[i]*(1-t)+values[i+1]*t
 return values[-1]
xs=sorted(set(xs+[axle+.41*math.cos(i*math.pi/24) for axle in [1.32,-1.38] for i in range(25)]))
width=[interp(x,base_w) for x in xs];height=[interp(x,base_h) for x in xs]
for side in [-1,1]:
 vertices=[]
 for x,w,h in zip(xs,width,height):
  low=.28
  for axle in [1.32,-1.38]:
   if abs(x-axle)<.41:low=max(low,.32+math.sqrt(max(0,.41**2-(x-axle)**2)))
  vertices.extend([(x,side*w,low),(x,side*w,h),(x,side*(w-.19),h+.025)])
 faces=[]
 for i in range(len(xs)-1):faces.extend([(i*3,i*3+1,(i+1)*3+1,(i+1)*3),(i*3+1,i*3+2,(i+1)*3+2,(i+1)*3+1)])
 m.mesh('sculpted flank',vertices,faces,'SuperOrange21')
 # Black intake insert and flying buttress, door outline, swept mirror.
 m.mesh('side intake',[(-1.12,side*.954,.50),(-1.0,side*.96,.97),(-.42,side*.90,.81),(-.7,side*.91,.43)],[(0,1,2,3)],'Carbon21')
 m.line([(-1.12,side*.963,.53),(-.84,side*.94,.91),(-.40,side*.91,.84)],.03,'SuperOrange21')
 m.line([(.48,side*.905,.89),(.32,side*.91,.39),(-.69,side*.91,.37),(-.85,side*.93,.77)],.008,'Carbon21')
 m.cyl((.42,side*.80,1.03),(.32,side*1.03,1.11),.017,'Carbon21');m.box((.32,side*1.09,1.12),(.22,.13,.08),'Carbon21',.04)
 # Roof spine: sides stay open for first-person view; windshield remains runtime wet glass.
 m.line([(.70,side*.85,.93),(.02,side*.76,1.37),(-1.05,side*.73,1.37),(-1.48,side*.87,1.00)],.034,'Carbon21')
 for axle in [1.32,-1.38]:
  m.line([(axle+.405*math.cos(a*math.pi/24),side*.999,.32+.405*math.sin(a*math.pi/24)) for a in range(25)],.022,'SuperOrange21')
 m.mesh('side glass',[(.70,side*.85,.93),(.02,side*.76,1.37),(-1.05,side*.73,1.37),(-1.48,side*.87,1.00)],[(0,1,2,3)],'WindshieldV9')
 m.line([(2.13,side*.54,.58),(2.02,side*.77,.64),(1.82,side*.86,.69)],.023,'Bone')
 m.line([(-2.315,side*.34,.76),(-2.32,side*.78,.76),(-2.24,side*.91,.82)],.016,'Red')
# Sculpted hood and rear deck, without enclosing cockpit.
for rear in [False,True]:
 sections=[(x,w,h) for x,w,h in zip(xs,width,height) if x>=.7] if not rear else [(x,w,h) for x,w,h in zip(xs,width,height) if x<=-1.0]
 v=[]
 for x,w,h in sections:v.extend([(x,-w+.19,h+.025),(x,0,h-.08),(x,w-.19,h+.025)])
 f=[]
 for i in range(len(sections)-1):
  for j in [0,1]:f.append((i*3+j,i*3+j+1,(i+1)*3+j+1,(i+1)*3+j))
 m.mesh('contoured deck',v,f,'SuperOrange21')
roof=[]
for x in [.02,-.5,-1.05]:
 for i in range(9):
  y=(i/8*2-1)*.75;roof.append((x,y,1.37+.045*(1-(y/.75)**2)))
m.mesh('curved roof',roof,[(r*9+i,r*9+i+1,(r+1)*9+i+1,(r+1)*9+i) for r in range(2) for i in range(8)],'Carbon21')
m.mesh('rear glass',[(-1.05,-.73,1.37),(-1.05,.73,1.37),(-1.48,.87,1),(-1.48,-.87,1)],[(0,1,2,3)],'WindshieldV9')
m.box((0,0,.26),(4.45,1.74,.055),'Carbon21',.025)
m.box((2.08,0,.35),(.25,1.72,.10),'Carbon21',.035)
for side in [-1,1]:m.box((1.99,side*.5,.47),(.12,.44,.15),'Carbon21',.02)
m.box((-2.25,0,.40),(.12,1.77,.23),'Carbon21',.025)
for y in [-.6,-.3,0,.3,.6]:m.box((-2.18,y,.28),(.38,.025,.17),'Carbon21')
for side in [-1,1]:m.tube((-2.31,side*.28,.70),(-2.40,side*.28,.70),.055,.04,'Steel',n=16);m.cyl((-1.96,side*.61,.9),(-1.96,side*.61,1.18),.025,'Carbon21')
m.box((-1.99,0,1.20),(.25,1.77,.035),'Carbon21',.012)
for i in range(7):m.box((-1.30-i*.08,0,1.02-i*.023),(.028,.68,.025),'Carbon21')
m.finish('SupercarV21',collision='none')
for polygon in m.OBJECTS['SupercarV21'].data.polygons:polygon.use_smooth=True
# Wheel hub is centered at origin; runtime keeps independently animated wheels.
m.tube((0,-.14,0),(0,.14,0),.35,.245,'Rubber',n=32)
for side in [-1,1]:
 m.tube((0,side*.135,0),(0,side*.15,0),.252,.225,'Steel',n=24)
 for i in range(10):
  a=i*math.tau/10;m.cyl((.055*math.sin(a),side*.15,.055*math.cos(a)),(.235*math.sin(a+.13),side*.15,.235*math.cos(a+.13)),.015,'Carbon21',n=6)
 m.cyl((0,side*.13,0),(0,side*.16,0),.05,'Steel',n=12)
m.finish('SuperWheelV21',collision='none')
m.export_all();(m.OUT/'models_v21_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'CasinoSupercarV21.blend'))
