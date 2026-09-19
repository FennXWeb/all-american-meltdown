"""Original survivor anatomy, ten haircuts and a twelve-metre Class A coach.
Metres; +X forward. Components have explicit floor/neck pivots.
"""
import sys,json,math,random
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent));import make_models_v2 as m
import bpy
m.PALETTE.update({'CamperPearlV14':(.8,.77,.67),'CamperWalnutV14':(.25,.16,.08),'CamperLeatherV14':(.67,.62,.5),'CamperStoneV14':(.5,.49,.43)})
m.OUT=m.ROOT/'ArtSource/ModelsV16';m.setup()
for female in [False,True]:
 # Tailored anatomy: hips, waist, ribs, chest, clavicle, shoulders, neck.
 rings=[(0,0,.79,.12,.18 if female else .16),(0,0,.89,.135,.21 if female else .18),(0,0,1.0,.115,.15 if female else .18),(.008,0,1.12,.14,.18 if female else .205),(.02,0,1.25,.16 if female else .145,.205 if female else .24),(0,0,1.34,.10,.19 if female else .23),(0,0,1.40,.06,.065)]
 m.loft(rings,'Cloth',n=20)
 # Collar, seams, breast pockets, belt loops and brass buckle.
 m.tube((0,0,1.37),(0,0,1.415),.076,.057,'Cloth',n=16)
 for s in [-1,1]:
  m.box((.15,s*.115,1.23),(.015,.09,.11),'Cloth',.008)
  m.line([(.158,s*.155,1.285),(.16,s*.075,1.285)],.003,'Steel')
  m.box((.133,s*.13,.91),(.015,.025,.09),'Rubber',.006)
 m.box((.143,0,.92),(.022,.07,.045),'Steel',.007)
 m.finish('SurvivorFemaleV16' if female else 'SurvivorMaleV16')
 # Head with brow, cheekbones, chin, ears, nose, lips, eyelids.
 m.loft([(0,0,1.41,.052,.055),(.025,0,1.45,.074,.064),(.025,0,1.49,.091,.083),(0,0,1.55,.102,.091),(-.013,0,1.62,.105,.093),(-.025,0,1.69,.084,.077),(-.03,0,1.72,.038,.042)],'Skin',n=20)
 for s in [-1,1]:
  m.box((-.006,s*.098,1.557),(.047,.026,.070),'Skin',.011)
  m.box((.092,s*.044,1.6),(.014,.039,.019),'Bone',.007)
  m.box((.102,s*.044,1.6),(.004,.017,.016),'Rubber',.003)
  m.line([(.101,s*.024,1.622),(.095,s*.062,1.62)],.007,'Skin')
 m.loft([(.089,0,1.54,.015,.022),(.119,0,1.565,.02,.019),(.098,0,1.61,.008,.011)],'Skin',n=8)
 m.box((.106,0,1.51),(.008,.043,.009),'Skin',.003)
 m.finish('SurvivorHeadFemaleV16' if female else 'SurvivorHeadMaleV16')
# Full articulated leg and arm silhouettes. Pivot at hip / shoulder.
m.loft([(0,0,-.79,.045,.053),(.025,0,-.65,.066,.064),(.015,0,-.44,.075,.074),(0,0,-.37,.068,.07),(-.01,0,-.20,.092,.085),(0,0,0,.105,.097)],'Cloth',n=12)
m.box((.055,0,-.78),(.25,.13,.13),'Rubber',.026)
for z in [-.27,-.29]:m.box((.084,0,z),(.011,.11,.014),'Cloth')
m.finish('SurvivorLegV16')
m.loft([(0,0,0,.09,.08),(0,0,-.12,.085,.076),(.018,0,-.26,.057,.057)],'Cloth',n=12)
m.loft([(.018,0,-.255,.056,.055),(.025,0,-.37,.060,.05),(.03,0,-.48,.038,.037)],'Skin',n=12)
m.box((.037,0,-.52),(.07,.085,.105),'Skin',.013)
for i in range(4):m.cyl((.05,-.027+i*.018,-.53),(.064,-.027+i*.018,-.595),.01,'Skin',n=6)
m.finish('SurvivorArmV16')
# Hair: fitted scalp caps plus distinct groom silhouettes; hair shares a tintable material.
for style in range(10):
 rng=random.Random(610+style)
 # Crown domes cover scalp without a floating rectangular hair block.
 m.loft([(-.025,0,1.62,.107,.095),(-.025,0,1.69,.092,.083),(-.029,0,1.733,.042,.045),(-.03,0,1.744,.005,.006)],'Cloth',n=20)
 if style==1: # textured crop
  for i in range(18):
   a=i*2.4;r=.065*math.sqrt(i/18);x=-.025+math.cos(a)*r;y=math.sin(a)*r
   m.cyl((x,y,1.716),(x+.013,y,1.76+rng.random()*.02),.023,'Cloth',r2=.01,n=6)
 if style==2: # swept side part
  for i in range(9):m.line([(-.09+i*.018,-.08,1.69),(-.06+i*.014,-.025,1.77),(-.04+i*.014,.085,1.71)],.016,'Cloth')
 if style in [3,4,5,8]:
  for i in range(13):
   a=math.pi*.5+i*math.pi/12;x=-.025+math.cos(a)*.10;y=math.sin(a)*.095
   end=1.43 if style==4 else 1.27 if style==3 else 1.52
   m.line([(x,y,1.68),(x-.02,y*1.08,1.57),(x-.035,y*1.12,end)],.023,'Cloth',n=7)
 if style==5: # ponytail
  m.line([(-.12,0,1.66),(-.20,0,1.6),(-.20,.01,1.38)],.045,'Cloth',n=10)
 if style==6: # tight curls
  for i in range(42):
   a=i*2.399;r=.12*math.sqrt(i/42);x=-.025+math.cos(a)*r;y=math.sin(a)*r
   m.cyl((x,y,1.70),(x,y,1.77+(.12-r)*.6),.026,'Cloth',r2=.019,n=7)
 if style==7: # mohawk, narrow crest
  m.loft([(-.03,0,1.69,.1,.023),(-.035,0,1.84,.082,.018),(-.035,0,1.865,.055,.004)],'Cloth',n=12)
 if style==8: # braided crown + braid
  for i in range(14):
   z=1.62-i*.029;m.cyl((-.14,(-1)**i*.016,z),(-.15,(-1)**(i+1)*.016,z-.037),.021,'Cloth',n=8)
 if style==9: # bun
  m.loft([(-.1,0,1.67,.041,.052),(-.14,0,1.72,.068,.07),(-.13,0,1.77,.038,.043)],'Cloth',n=12)
 m.finish('Hair%02dV16'%style)
# Coach: rear=-10m, front=2m, wide flat Class A front, actual window openings.
p='CamperPearlV14';w='CamperWalnutV14'
m.box((-4,0,.53),(12,2.5,.15),w,.02)
m.box((-4.03,0,3.18),(12.02,2.54,.16),p,.07)
m.box((-9.98,0,1.86),(.14,2.50,2.58),p,.04)
m.box((-4,0,.93),(12,2.5,.65),p,.055)
for side in [-1,1]:
 y=side*1.235
 m.box((-4,y,1.53),(12,.08,.82),p,.025)
 m.box((-4,y,2.96),(12,.08,.32),p,.025)
 m.box((-4,side*1.27,1.30),(12,.04,.12),'Steel',.018)
 for x in [-9.9,-8.1,-6.1,-4.1,-2.1,-.1,1.92]:m.box((x,y,2.09),(.28,.10,1.50),p,.02)
 for x in [-9,-7.1,-5.1,-3.1,-1.1,.9]:
  for z in [1.96,2.80]:m.box((x,y,z),(1.69,.11,.05),'Rubber',.012)
  for dx in [-.845,.845]:m.box((x+dx,y,2.38),(.05,.11,.86),'Rubber',.012)
 # Flush luggage lockers, repeating latches, recessed trim.
 for x in [-8.7,-7.45,-6.2,-4.95,-3.7,-2.45,-1.2]:
  m.box((x,side*1.258,.93),(1.18,.02,.48),'Steel',.018)
  m.box((x,side*1.275,1.09),(.12,.014,.04),'Rubber',.008)
 # Flowing original black/silver side ribbons, shallow meshes following body.
 for offset,mat in [(0,'Rubber'),(.14,'Steel'),(.27,'Rubber')]:
  points=[(-9.8,1.02+offset),(-7.5,1.44+offset),(-5,1.13+offset),(-2.5,1.5+offset),(1.86,1.04+offset)]
  verts=[(x,side*1.29,z+dz) for dz in [0,.10] for x,z in points]
  m.mesh('Sweeping coach stripe',verts,[(i,i+1,i+6,i+5) for i in range(4)],mat)
 # Roof-mounted closed awning cassette, mirror arm, mirror glass.
 m.cyl((-8.9,side*1.30,2.96),(.5,side*1.30,2.96),.065,'Steel',n=12)
 m.line([(1.75,side*1.22,1.95),(1.94,side*1.47,1.91),(1.72,side*1.47,1.91)],.04,'Steel')
 m.box((1.73,side*1.47,2.04),(.13,.13,.39),'Steel',.035)
 m.box((1.66,side*1.475,2.04),(.014,.115,.34),'Glass',.02)
 # Dual rear axles + front arches are visually separated from the hull.
 for x in [1.12,-6.25,-7.55]:
  for a in range(11):
   t=math.pi*a/10;t2=math.pi*(a+1)/10
   m.cyl((x+math.cos(t)*.55,side*1.265,.43+math.sin(t)*.55),(x+math.cos(t2)*.55,side*1.265,.43+math.sin(t2)*.55),.045,'Rubber',n=6)
 # Headlamp towers and rear lamps.
 m.box((1.98,side*.97,1.05),(.08,.25,.53),'Rubber',.045)
 for z in [.92,1.15]:m.cyl((2.02,side*.97,z),(2.045,side*.97,z),.075,'Bone',n=16)
 m.box((-10.07,side*.97,1.17),(.025,.20,.40),'Red',.03)
# Bow fascia below windshield; no bonnet protruding ahead of the cab.
m.box((1.92,0,1.24),(.16,2.48,.40),p,.06)
m.box((2.00,0,.79),(.08,1.45,.40),'Rubber',.03)
for z in [.66,.76,.86,.96]:m.box((2.047,0,z),(.015,1.38,.025),'Steel',.006)
m.box((1.91,0,.57),(.22,2.5,.16),'Steel',.05)
m.box((1.88,0,2.99),(.22,2.45,.26),p,.06)
for side in [-1,1]:m.cyl((1.98,side*1.18,1.46),(1.74,side*1.18,2.88),.07,p,n=12)
# Dashboard at the new cab, no inherited sedan interior.
m.box((1.35,0,1.22),(.60,2.22,.15),'Rubber',.04)
m.box((1.08,-.55,1.28),(.09,.65,.24),'Rubber',.04)
for y in [-.72,-.53,-.34]:m.cyl((1.025,y,1.3),(1.03,y,1.3),.058,'Steel',n=16)
for x in [-7.8,-3.8]:
 m.box((x,0,3.36),(1.12,.85,.24),p,.07)
 for d in range(7):m.box((x-.42+d*.14,0,3.49),(.035,.64,.012),'Rubber')
m.finish('CamperShellV16')
m.export_all();(m.OUT/'models_v16_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2))
bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'AllAmericanMeltdown_V16.blend'));print('AAM_V16_MODELS_COMPLETE',len(m.RECORDS))
