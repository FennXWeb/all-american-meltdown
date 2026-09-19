import sys,json
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent))
import make_models_v2 as m
import bpy
m.OUT=m.ROOT/'ArtSource'/'ModelsV5';m.setup()
# 4.5m civil sedan, open window geometry and a genuinely hollow passenger cell.
m.box((0,0,.31),(4.40,1.86,.22),'Rubber',.05)
m.box((1.46,0,.74),(1.45,1.83,.29),'Rust',.06);m.box((-1.83,0,.75),(.78,1.83,.32),'Rust',.06)
for side in [-1,1]:
 m.box((-.18,side*.94,.67),(2.60,.075,.57),'Rust',.025)
 m.box((-.18,side*.977,.98),(2.6,.025,.055),'Steel')
 m.line([(-1.48,side*.9,1.02),(-1.22,side*.80,1.55),(.29,side*.80,1.55),(.76,side*.9,1.01)],.05,'Steel')
 m.line([(-.57,side*.92,.98),(-.57,side*.81,1.53)],.04,'Rust')
 m.box((-.15,side*.97,.82),(.22,.04,.035),'Steel');m.box((.48,side*1.09,1.1),(.20,.14,.13),'Steel',.015)
 m.box((0,side*.98,.45),(4.3,.045,.06),'Steel')
 m.box((2.21,side*.64,.75),(.04,.35,.16),'Bone',.015);m.box((-2.22,side*.68,.72),(.035,.35,.16),'Red')
 m.box((2.21,side*.83,.60),(.035,.12,.08),'Glow')
m.box((-.46,0,1.59),(1.74,1.68,.07),'Rust',.025)
for x in [-2.24,2.24]:m.box((x,0,.45),(.12,1.97,.13),'Steel',.02)
for z in [.62,.67,.72,.77]:m.box((2.214,0,z),(.04,.70,.023),'Rubber')
m.box((-2.251,0,.66),(.024,.45,.14),'Bone')
for y in [-.4,.4]:m.line([(1.06,y,.895),(2.06,y,.895)],.012,'Steel')
m.finish('SedanShellV5',collision='convex')
# Interior floor, footwells, seat cushions/backrests, belts, dashboard, trim and pedals.
m.box((-.22,0,.38),(2.50,1.77,.08),'Cloth')
for side in [-1,1]:
 m.box((-.20,side*.87,.79),(2.30,.055,.23),'Cloth');m.box((-.38,side*.83,.75),(.55,.09,.07),'Rubber')
 m.box((-.52,side*.815,.90),(.25,.035,.03),'Steel')
# Seats are constructed from the vehicle profile in engine.
m.box((.49,0,.99),(.31,1.72,.14),'Rubber',.045)
m.box((.43,-.46,1.105),(.12,.57,.17),'Steel',.025)
for y in [-.61,-.42,-.26]:
 m.cyl((.357,y,1.10),(.35,y,1.10),.072,'Rubber',n=16)
 for a in range(0,360,45):
  import math
  yy=y+math.cos(math.radians(a))*.058;zz=1.10+math.sin(math.radians(a))*.058
  m.box((.345,yy,zz),(.008,.011,.016),'Bone')
 m.line([(.336,y,1.10),(.336,y-.03,1.14)],.007,'Glow')
for y in [-.73,.72]:
 for i in range(5):m.box((.32,y+i*.016,1.01),(.009,.009,.065),'Steel')
m.box((.55,.53,.82),(.035,.51,.25),'Rubber')
m.box((.38,.04,.99),(.025,.25,.10),'Rubber')
for y in [-.035,.075]:m.cyl((.36,y,.98),(.32,y,.98),.016,'Bone')
m.box((-.30,0,.59),(1.0,.18,.25),'Rubber');m.line([(-.20,0,.62),(-.13,0,.84)],.015,'Steel');m.box((-.13,0,.86),(.10,.085,.06),'Bone',.015)
for y in [-.65,-.48,-.30]:m.box((.35,y,.46),(.07,.075,.11),'Steel')
m.box((.13,0,1.45),(.055,.30,.10),'Steel');m.box((.098,0,1.45),(.015,.26,.07),'Glass')
m.finish('SedanCabinV5',collision='convex')
m.tube((-.015,0,0),(.015,0,0),.19,.163,'Rubber',n=24)
m.cyl((-.05,0,0),(.05,0,0),.065,'Steel')
for y,z in [(-.15,.06),(.15,.06),(0,-.16)]:m.line([(0,0,0),(0,y,z)],.019,'Steel')
m.finish('SteeringV5',collision='convex')
m.box((0,0,.105),(.045,.49,.21),'Cloth',.012);m.box((-.03,0,.17),(.025,.13,.025),'Steel');m.finish('GloveLidV5',collision='convex')
m.line([(0,0,0),(-.09,.40,.24)],.012,'Steel');m.line([(-.09,.26,.24),(-.09,.62,.24)],.016,'Rubber');m.finish('WiperV5',collision='convex')
m.tube((0,-.115,0),(0,.115,0),.32,.18,'Rubber',n=24)
m.cyl((0,-.13,0),(0,.13,0),.19,'Steel',n=16)
for i in range(5):
 import math
 x=.11*math.cos(i*math.tau/5);z=.11*math.sin(i*math.tau/5);m.cyl((x,-.138,z),(x,.138,z),.022,'Rubber')
m.finish('SedanWheelV5',collision='convex')
m.export_all();(m.OUT/'models_v5_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'LethalWorld_V5.blend'));print('LW_V5_MODELS_COMPLETE',len(m.RECORDS))
