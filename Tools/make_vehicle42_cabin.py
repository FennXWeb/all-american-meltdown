import sys,math,json
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent));import make_models_v2 as m
import bpy
m.OUT=m.ROOT/'ArtSource/ModelsV42'
for name in ['Wood','Leather','Fabric','Vinyl','Metal','Chrome','Rubber','Glass','Lamp']:m.PALETTE['V42_'+name]=(.3,.3,.28)
m.setup()
for name in ['Kitchen','Sink','Fridge','Wardrobe']:
 h=1.75 if name in ('Fridge','Wardrobe') else .91;mat='V42_Metal' if name=='Fridge' else 'V42_Wood'
 # Framed cabinetry with actual toe recess, panel seams, drawers and handles.
 m.box((0,.015,h/2+.035),(.82,.49,h-.07),mat,.028)
 m.box((0,0,.065),(.72,.43,.13),'V42_Vinyl',.015)
 panels=[(.25,.31),(.60,.30),(.80,.10)] if h<1 else [(.57,1.00),(1.4,.61)]
 for z,hh in panels:
  m.box((0,-.244,z),(.76,.045,hh),mat,.018)
  m.box((0,-.270,z),(.64,.016,hh-.09),mat,.012)
  m.cyl((-.16,-.305,z+hh*.27),(.16,-.305,z+hh*.27),.010,'V42_Chrome',n=16)
  for x in [-.16,.16]:m.cyl((x,-.27,z+hh*.27),(x,-.31,z+hh*.27),.009,'V42_Chrome',n=12)
 if name=='Kitchen':
  m.box((0,0,h),(.89,.56,.045),'V42_Metal',.02)
  m.box((0,.02,h+.03),(.65,.40,.016),'V42_Vinyl',.03)
  for x in [-.19,.19]:
   m.tube((x,.02,h+.036),(x,.02,h+.055),.125,.105,'V42_Metal',n=24)
   for a in [0,math.pi/2]:m.box((x,.02,h+.067),(.27,.009,.014),'V42_Vinyl',.002,rot=(0,0,a))
   m.cyl((x,-.19,h+.03),(x,-.19,h+.06),.022,'V42_Chrome',n=16)
 elif name=='Sink':
  # Counter frame surrounds a genuine recessed basin rather than a black decal.
  for x in [-.33,.33]:m.box((x,0,h),(.21,.56,.05),'V42_Metal',.02)
  for y in [-.22,.22]:m.box((0,y,h),(.48,.12,.05),'V42_Metal',.015)
  v=[(x,y,z) for z,w,d in [(h,.24,.16),(h-.17,.18,.12)] for x,y in [(-w,-d),(w,-d),(w,d),(-w,d)]]
  m.mesh('sink basin',v,[(0,1,5,4),(1,2,6,5),(2,3,7,6),(3,0,4,7),(4,5,6,7)],'V42_Chrome')
  m.cyl((0,0,h-.169),(0,0,h-.166),.022,'V42_Vinyl',n=16)
  m.line([(0,.22,h),(0,.22,h+.22),(0,.16,h+.30),(0,.03,h+.30),(0,-.015,h+.25)],.016,'V42_Chrome',16)
 elif name=='Fridge':
  m.box((.16,-.28,1.56),(.22,.012,.065),'V42_Glass',.008)
  for x in [-.27,-.15,-.03,.09,.21]:m.box((x,-.27,.11),(.045,.009,.04),'V42_Vinyl',.004)
 m.finish('Camper42'+name,collision='none')
m.box((0,0,.28),(1.42,2.,.5),'V42_Wood',.035)
m.loft([(0,0,.54,.66,.93),(0,0,.62,.74,1.01),(0,0,.72,.71,.98)],'V42_Fabric',24)
for y in [-.5,.5]:m.loft([(-.5,y,.71,.14,.31),(-.5,y,.78,.21,.41),(-.5,y,.84,.17,.34)],'V42_Fabric',16)
for x in [-.12,.08,.28,.48]:m.line([(x,-.92,.744),(x,.92,.744)],.004,'V42_Leather',6)
for side in [-1,1]:
 m.box((.1,side*.995,.3),(.93,.035,.32),'V42_Wood',.02);m.cyl((-.08,side*1.025,.34),(.20,side*1.025,.34),.011,'V42_Chrome',n=12)
m.finish('Camper42Bed',collision='none')
for x in [-.96,0]:m.box((x,0,1.06),(.055,.08,2.12),'V42_Metal',.014)
for z in [.03,1.09,2.09]:m.box((-.48,0,z),(.96,.08,.055),'V42_Metal',.014)
m.box((-.48,0,.55),(.92,.058,1.02),'V42_Wood',.02)
m.box((-.48,0,1.60),(.89,.014,.94),'V42_Glass',.025)
m.line([(-.83,-.08,.78),(-.83,-.09,1.01)],.016,'V42_Chrome',16)
m.box((-.74,-.052,.82),(.15,.018,.075),'V42_Vinyl',.012)
m.finish('Camper42Door',collision='none')
# Narrow wire-spoke motocross wheel with laced spokes and knobby tread.
m.tube((0,-.055,0),(0,.055,0),.355,.275,'V42_Rubber',n=48)
for side in [-1,1]:
 m.tube((0,side*.047,0),(0,side*.056,0),.28,.256,'V42_Chrome',n=40)
 for i in range(24):
  a=i*math.tau/24;b=a+side*.28;m.cyl((.052*math.sin(a),side*.028,.052*math.cos(a)),(.264*math.sin(b),side*.05,.264*math.cos(b)),.0035,'V42_Chrome',n=6)
for i in range(32):
 a=i*math.tau/32
 for y in [-.036,0,.036]:m.box((.357*math.sin(a),y,.357*math.cos(a)),(.041,.026,.020),'V42_Rubber',.004,rot=(0,a,0))
m.cyl((0,-.075,0),(0,.075,0),.058,'V42_Metal',n=20)
m.finish('Wheel42_dirtbike',collision='none')
m.export_all();p=m.OUT/'models_v42_manifest.json';data=json.loads(p.read_text());names=set(m.RECORDS);data['assets']=[r for r in data['assets'] if r['name'] not in names]+list(m.RECORDS.values());p.write_text(json.dumps(data,indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'CamperFurnishings42.blend'));print('VEHICLE42_CABIN_DONE')
