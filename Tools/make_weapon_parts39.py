"""Author 50 interchangeable machined/wooden assemblies, in metres with +X along the bore."""
import sys,json,math
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent))
import bpy
import make_models_v2 as m
m.OUT=m.ROOT/'ArtSource/WeaponParts39';m.OUT.mkdir(exist_ok=True);m.setup()
defs=json.loads((m.ROOT/'Data/WeaponParts39.json').read_text())
for p in defs:
 group=p['group'];i=int(p['id'][-2:]);length,width,height=[x/100 for x in p['size']]
 if group=='Action':
  if i==0:
   m.profile([(-.12,-.025),(.11,-.02),(.22,.05),(.22,.13),(.16,.15),(.17,.08),(.08,.02),(-.12,.025)],.025,'Rust')
  elif i==1:
   m.cyl((-.17,0,0),(.12,0,0),.026,'Wood',.052,16);m.cyl((.12,0,0),(.20,0,0),.052,'Wood',.037,16)
  else:
   m.profile([(-.15,-.045),(.12,-.045),(.18,-.018),(.18,.026),(.12,.05),(-.12,.05),(-.17,.015)],width,'Steel')
   m.open_receiver(-.10,.12,.047,.025,.017)
   if i in [3,8,11]:m.profile([(-.1,-.04),(-.02,-.12),(.07,-.12),(.09,-.04),(.065,-.04),(.045,-.09),(-.005,-.09),(-.05,-.04)],.014,'Steel')
   for x in [-.09,-.035,.02,.075]:m.box((x,0,.067),(.018,width+.012,.012),'Steel',.002)
   m.box((.035,width*.53,.015),(.095,.012,.025),'Rust',.003)
   for x in [-.09,.095]:m.cyl((x,-width*.55,0),(x,width*.55,0),.008,'Steel',n=8)
   if i==10:
    m.cyl((-.16,0,0),(.14,0,0),.07,'Steel',n=16);m.box((0,0,-.07),(.24,.12,.04),'Rubber',.008)
   if i in [9,15]:
    for y in [-.055,.055]:m.tube((-.10,y,0),(.12,y,0),.021,.014,'Steel',12)
   if i==14:m.box((0,0,.025),(.24,.075,.04),'RoadOchre33' if 'RoadOchre33' in m.MATS else 'Bone',.009)
 elif group=='Barrel':
  if i==14:
   m.box((.035,0,0),(.07,.065,.045),'Rubber',.006)
   for y in [-.024,.024]:m.cyl((.05,y,0),(length,y,0),.005,'Steel',n=8)
  else:
   count=6 if i==10 else 2 if i in [8,11] else 1
   for n in range(count):
    y=math.cos(n*math.tau/6)*.034 if count==6 else (n-.5)*.044 if count==2 else 0;z=math.sin(n*math.tau/6)*.034 if count==6 else 0
    outer=.053 if i==9 else .016 if count>1 else .021
    m.tube((0,y,z),(length,y,z),outer,outer*.67,'Steel',16)
    for x in [.025,length*.45,length-.025]:m.tube((x,y,z),(x+.018,y,z),outer+.006,outer,'Steel',12)
   if i in [2,5,6,7,13]:
    m.box((length*.35,0,-.024),(length*.45,.062,.037),'Wood' if i==2 else 'Rubber',.006)
    for v in range(7):m.box((length*(.14+v*.06),0,-.044),(.010,.072,.012),'Steel',.001)
   if i==10:
    for x in [.04,length-.05]:m.tube((x,0,0),(x+.035,0,0),.061,.048,'Steel',18)
   if i==15:
    m.tube((length-.12,0,0),(length,0,0),.047,.027,'Rust',14);m.cyl((length-.08,-.035,-.03),(length+.012,-.02,-.015),.006,'Steel',n=8)
 elif group=='Stock':
  wood=i in [4,5];mat='Wood' if wood else 'Rubber'
  if i==2:
   for z in [-.055,.03]:m.cyl((-.31,0,z),(-.015,0,0),.009,'Steel',n=10)
   m.box((-.31,0,-.015),(.025,.065,.14),'Rubber',.006)
  else:
   m.profile([(0,.025),(-.10,.04),(-.30,.025),(-.34,-.015),(-.34,-.11),(-.29,-.12),(-.12,-.04),(-.02,-.045)],.066,mat)
   m.box((-.335,0,-.04),(.018,.076,.16),'Rubber',.004)
   if i in [0,6,7]:m.box((-.21,0,.044),(.16,.074,.024),'Steel',.006)
   if not wood:
    for x in [-.12,-.16,-.20,-.24]:m.box((x,-.035,-.015),(.016,.006,.040),'Steel',.002)
 elif group=='Grip':
  if i>=4:
   m.cyl((-.28,0,0),(0,0,0),.018,'Steel' if i==4 else 'Wood',.026,12)
   for j in range(12):m.tube((-.27+j*.018,0,0),(-.26+j*.018,0,0),.026,.017,'Rubber',12)
  else:
   m.profile([(-.04,0),(.045,0),(.025,-.055),(.075,-.16),(.045,-.19),(-.018,-.18),(-.065,-.06)],.05,'Wood' if i==1 else 'Rubber')
   for j in range(6):m.box((-.005+j*.009,-.027,-.052-j*.020),(.056,.006,.005),'Steel',.001)
   m.cyl((.023,-.029,-.145),(.023,.029,-.145),.006,'Steel',n=8)
 elif group=='Feed':
  if i==0:m.tube((-.09,0,0),(.20,0,0),.018,.012,'Steel',14)
  elif i==1:
   m.cyl((-.05,0,0),(.05,0,0),.048,'Steel',n=24)
   for j in range(6):
    a=j*math.tau/6;y=math.cos(a)*.028;z=math.sin(a)*.028;m.tube((.05,y,z),(.054,y,z),.010,.006,'Rust',10)
  elif i==2:
   for y in [-.04,.04]:m.box((0,y,-.03),(.15,.014,.10),'Steel',.003)
   for x in [-.075,.075]:m.box((x,0,-.03),(.014,.09,.10),'Steel',.003)
  elif i==3:
   m.cyl((0,-.07,0),(0,.07,0),.022,'Steel',n=14);m.box((-.055,0,-.015),(.09,.09,.025),'Rust',.003)
  elif i==4:
   m.box((0,0,0),(.18,.11,.04),'Steel',.005);m.box((0,-.075,-.08),(.15,.11,.13),'Rubber',.006)
   for j in range(6):m.cyl((-.065+j*.025,-.08,-.02),(-.065+j*.025,.02,-.02),.006,'Bone',n=8)
  else:
   m.cyl((-.06,0,0),(.07,0,0),.036,'Steel',n=16);m.cyl((0,0,0),(0,0,-.09),.022,'Rust',n=12);m.tube((0,0,-.09),(0,0,-.12),.035,.021,'Steel',12)
 ob=m.finish(p['mesh'],collision='convex')
 for poly in ob.data.polygons:poly.use_smooth=False
m.export_all();(m.OUT/'models_39_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'WeaponParts39.blend'));print('WEAPON_PARTS39_COMPLETE',len(m.OBJECTS))
