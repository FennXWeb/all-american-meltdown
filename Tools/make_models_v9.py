"""Original vehicle shells and retail/diner furniture, authored in metres."""
import sys,json,math
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent))
import make_models_v2 as m
import bpy
m.PALETTE['Lane']=(.72,.51,.08)
m.OUT=m.ROOT/'ArtSource/ModelsV9';m.setup()
specs=[('sedan',4.10,.87,1.55,2.70),('boxtruck',7.60,1.15,3.30,4.80),('police',4.60,.95,1.60,2.90),('rv',7.60,1.15,3.10,4.60),('bus',9.80,1.20,3.10,6.40),('van',5.50,1.05,2.45,3.40),('pickup',5.40,1.05,1.70,3.50),('suv',4.90,1.03,2.00,3.00),('muscle',4.70,1.00,1.55,2.85),('supercar',4.50,1.00,1.53,2.70)]
for name,length,w,height,wb in specs:
 rear=2.20-length;work=name in ['boxtruck','rv','bus','van'];paint='Bone' if name=='police' else 'Lane' if name=='bus' else 'Red' if name in ['muscle','supercar'] else 'Steel'
 # A hollow shell, with shaped lower sills and actual wheel openings.
 m.box(((rear+2.2)/2,0,.32),(length-.12,w*1.80,.09),'Rubber')
 for side in [-1,1]:
  bottom=[]
  for i in range(161):
   x=rear+(length*i/160);z=.34
   for axle in [1.32,1.32-wb]:
    if abs(x-axle)<.38:z=max(z,.32+math.sqrt(max(0,.38**2-(x-axle)**2)))
   bottom.append((x,z))
  m.profile(bottom+[(2.2,.73),(1.88,.87),(.8,.91),(.55,1.02),(-1.4,1.02),(rear,.78)],.035,paint,y=side*(w-.035))
  # Door seams, handles, side trim, fender edging and mounted mirrors.
  for x in [-.72,.46]:m.line([(x,side*w,.43),(x,side*w,1.01)],.008,'Rubber')
  for x in [-.85,.18]:m.box((x,side*(w+.009),.91),(.16,.026,.03),'Steel')
  m.line([(.40,side*w,1.08),(.38,side*(w+.13),1.12)],.018,'Steel')
  m.box((.38,side*(w+.16),1.14),(.17,.08,.13),paint,.02)
  for axle in [1.32,1.32-wb]:
   pts=[(axle+.39*math.cos(a*math.pi/12),side*w,.32+.39*math.sin(a*math.pi/12)) for a in range(13)]
   m.line(pts,.025,paint)
 # Bonnet, boot, lamp housings, grille, bumper and number plates.
 m.box((1.43,0,.84),(1.48,w*1.87,.12),paint,.04)
 m.box((2.18,0,.48),(.12,w*2,.16),'Rubber',.025)
 m.box((rear+.03,0,.48),(.10,w*2,.14),'Rubber',.02)
 for side in [-1,1]:
  m.box((2.205,side*w*.67,.72),(.035,.37,.18),'Rubber',.02)
  for y in [-.09,.09]:m.box((2.229,side*w*.67+y,.73),(.012,.14,.12),'Bone',.015)
  m.box((rear-.03,side*w*.72,.73),(.02,.27,.16),'Red',.02)
 for z in [.59,.64,.69,.74]:m.box((2.225,0,z),(.018,.78,.018),'Steel')
 for x in [rear-.04,2.24]:m.box((x,0,.46),(.018,.42,.11),'Bone')
 # Cabin pillars follow the windscreen and terminate on the body.
 roof=2.08 if work else 1.57
 for side in [-1,1]:
  m.line([(.70,side*(w-.05),1.03),(.02,side*(w-.14),roof),(-1.25,side*(w-.14),roof),(-1.48,side*(w-.05),1.03)],.038,paint)
  m.line([(-.73,side*(w-.045),1.02),(-.73,side*(w-.14),roof)],.035,paint)
 m.box((-.62,0,roof+.012),(1.38,(w-.12)*2,.045),paint,.02)
 if not work and name not in ['pickup','suv']:
  m.box(((rear-1.47)/2,0,.91),(-1.47-rear,w*1.90,.10),paint,.03)
 if work or name=='suv':
  top=height;front=-1.46;mid=(rear+front)/2
  m.box((mid,0,top),(front-rear,w*2,.06),paint,.025)
  for side in [-1,1]:
   m.box((mid,side*(w-.025),.86),(front-rear,.045,.75),paint)
   if name in ['van','boxtruck']:m.box((mid,side*(w-.025),(top+1.24)/2),(front-rear,.045,top-1.24),paint)
   else:
    for i in range(max(1,int((front-rear)/.85))):
     x=front-.42-i*.85
     m.box((x,side*(w-.025),(top+1.24)/2),(.04,.045,top-1.24),paint)
     m.box((x-.40,side*(w-.025),top-.16),(.78,.045,.27),paint)
     m.box((x-.40,side*(w-.023),(top+1.24-.27)/2),(.73,.012,top-1.24-.27),'Glass')
  if top>roof+.1:m.box((front,0,(top+roof)/2),(.045,w*2,top-roof),paint)
  if name in ['rv','suv']:m.box((rear,0,(top+.4)/2),(.05,w*2,top-.4),paint)
  if name=='boxtruck':
   for x in [rear+.15,mid,front-.15]:
    for side in [-1,1]:m.box((x,side*w,1.88),(.055,.025,2.65),'Steel')
  if name=='rv':
   for side in [-1,1]:m.box((mid,side*w,1.05),(front-rear,.012,.14),'Red')
 if name=='pickup':
  m.box(((rear-1.46)/2,0,.63),(-1.46-rear,w*1.86,.045),'Steel')
  for side in [-1,1]:m.box(((rear-1.46)/2,side*(w-.035),.99),(-1.46-rear,.055,.58),paint)
 if name=='police':m.box((-.65,0,roof+.07),(.27,1.12,.09),'Rubber')
 if name in ['muscle','supercar']:
  for side in [-1,1]:m.box((rear+.30,side*.64,1.01),(.05,.045,.24),'Steel')
  m.box((rear+.30,0,1.15),(.22,w*1.7,.045),paint,.01)
 m.finish('Vehicle_'+name+'_V9',collision='none')
# Purpose-built bike frame, tank, fork and seat; wheels remain separate rotating parts.
for side in [-1,1]:
 m.line([(-.7,side*.10,.32),(-.25,side*.10,.87),(.5,side*.10,.79),(-.7,side*.10,.32)],.04,'Steel')
 m.line([(.65,side*.09,.33),(.50,side*.09,1.20)],.035,'Steel')
m.box((-.30,0,.92),(.85,.36,.12),'Cloth',.04);m.box((.26,0,.91),(.59,.36,.38),'Red',.065)
m.box((0,0,.53),(.39,.30,.36),'Steel',.03);m.line([(.52,-.42,1.23),(.46,0,1.17),(.52,.42,1.23)],.022,'Steel')
for y in [-.38,.38]:m.box((.53,y,1.23),(.11,.14,.045),'Rubber')
m.box((.63,0,1.04),(.09,.17,.14),'Bone',.02);m.finish('Vehicle_dirtbike_V9',collision='none')
# Shared detailed cockpit, sized around the input controls and first-person eye.
m.box((-.55,0,.38),(2.05,1.60,.08),'Rubber')
m.profile([(.23,.85),(.26,1.07),(.59,1.10),(.67,.95),(.63,.42),(.55,.42),(.53,.84)],1.58,'Rubber')
for side in [-1,1]:
 m.box((-.46,side*.79,.79),(1.88,.055,.34),'Cloth',.015)
 m.box((-.30,side*.755,.79),(.42,.08,.06),'Steel',.01)
for y in [-.59,-.43,-.27]:
 m.cyl((.247,y,1.055),(.222,y,1.055),.062,'Steel',n=20)
 m.cyl((.220,y,1.055),(.215,y,1.055),.054,'Rubber',n=20)
 for a in range(0,360,45):m.box((.210,y+math.sin(math.radians(a))*.044,1.055+math.cos(math.radians(a))*.044),(.009,.008,.008),'Bone')
 m.line([(.207,y,1.055),(.207,y-.028,1.084)],.005,'Red')
m.cyl((.12,-.43,1.08),(.42,-.43,.93),.035,'Steel')
m.box((.28,.52,.89),(.07,.54,.24),'Cloth',.015)
m.box((.25,.05,1.0),(.07,.30,.12),'Steel',.01)
for y in [-.04,.15]:m.cyl((.20,y,1.0),(.19,y,1.0),.018,'Rubber')
m.box((-.12,0,.56),(.8,.18,.20),'Rubber',.02)
m.line([(-.12,0,.63),(-.08,0,.85)],.018,'Steel');m.box((-.08,0,.87),(.09,.07,.055),'Rubber',.01)
for y in [-.58,-.43,-.29]:m.box((.39,y,.48),(.13,.08,.03),'Steel')
m.line([(.05,0,1.57),(.05,0,1.43)],.012,'Steel')
m.box((.05,0,1.43),(.035,.26,.09),'Steel');m.box((.026,0,1.43),(.006,.24,.07),'Glass')
m.finish('VehicleCabinV9',collision='none')
# Diner pieces with floor pivots and useful proportions.
m.box((0,0,.76),(1.30,.85,.055),'Bone',.03);m.box((0,0,.715),(1.20,.76,.025),'Steel')
m.cyl((0,0,.07),(0,0,.72),.055,'Steel');m.box((0,0,.045),(.65,.55,.065),'Steel',.02);m.finish('DinerTableV9',collision='convex')
m.box((0,0,.28),(1.35,.64,.48),'Red',.05);m.box((0,.25,.77),(1.35,.15,.85),'Red',.045)
for x in [-.58,.58]:m.box((x,0,.10),(.10,.55,.18),'Steel')
m.finish('DinerBoothV9',collision='convex')
m.box((0,0,.45),(1.8,.80,.90),'Steel',.02);m.box((0,0,.93),(1.85,.84,.05),'Steel')
for x in [-.60,0,.60]:
 m.box((x,-.413,.47),(.52,.022,.62),'Rubber');m.box((x,-.435,.65),(.32,.03,.025),'Steel')
 for y in [-.22,.22]:m.cyl((x,y,.96),(x,y,.98),.16,'Rubber',n=12)
m.finish('KitchenRangeV9',collision='convex')
m.box((0,0,0),(2.1,1.0,.25),'Steel',.03);m.box((0,.22,.35),(.65,.55,.60),'Steel');m.finish('KitchenHoodV9',collision='none')
# Luxury retail fixtures: brass clothing rails, upholstered seats, folded stock and plinths.
for x in [-.85,.85]:
 m.cyl((x,0,.08),(x,0,1.8),.022,'Bone');m.box((x,0,.035),(.25,.6,.07),'Steel')
m.cyl((-.85,0,1.78),(.85,0,1.78),.024,'Bone')
for i in range(8):
 x=-.66+i*.19;m.line([(x,0,1.79),(x,-.20,1.60),(x,.20,1.60),(x,0,1.79)],.008,'Steel')
 m.box((x,0,1.12),(.12,.44,.90),'Cloth',.025)
m.finish('ClothesRackV9',collision='convex')
m.box((0,0,.15),(.66,.66,.3),'Bone',.02);m.finish('DisplayPlinthV9',collision='convex')
m.box((0,0,.35),(1.4,.56,.3),'Cloth',.08)
for x in [-.52,.52]:m.box((x,0,.13),(.08,.43,.26),'Steel')
m.finish('BoutiqueBenchV9',collision='convex')
m.export_all();(m.OUT/'models_v9_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2))
bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'LethalWorld_V9.blend'))
print('LW_V9_MODELS_COMPLETE',len(m.RECORDS))
