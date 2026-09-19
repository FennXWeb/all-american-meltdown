"""Update 18 original modular meshes, metres, +X forward. No packaging."""
import sys,math,json
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent));import make_models_v2 as m
import bpy
m.OUT=m.ROOT/'ArtSource/ModelsV18'
m.PALETTE.update({'ArcadeMuralV18':(.10,.18,.30),'CreatureHideV18':(.28,.22,.15),'PolicePaintV18':(.19,.26,.30)})
m.setup()
def ell(p,s,mat='Skin'):
 bpy.ops.mesh.primitive_uv_sphere_add(segments=12,ring_count=8,radius=1,location=p);o=bpy.context.object;o.scale=s;bpy.ops.object.transform_apply(location=False,rotation=False,scale=True);return m.add(o,mat)
def limb(a,b,r,mat='Skin',end=.07):m.cyl(a,b,r,mat,r2=end,n=10);ell(a,(r,r,r),mat)
def horn(points,r=.07):
 for i in range(len(points)-1):m.cyl(points[i],points[i+1],r*(1-i/(len(points)-1)),'Bone',r2=r*max(0,1-(i+1)/(len(points)-1)),n=7)
# Break-action shotgun: receiver fixed; two real open bores and wood fore-end rotate at hinge.
m.box((-.17,0,-.055),(.38,.075,.12),'Wood',.02,rot=(0,-.12,0));m.box((-.355,0,-.075),(.025,.09,.16),'Rubber',.008)
m.box((.025,0,0),(.15,.10,.085),'Steel',.012);m.box((-.04,0,-.09),(.075,.06,.16),'Wood',.015,rot=(0,.25,0));m.tube((-.025,-.035,-.10),(-.025,.035,-.10),.048,.040,'Steel');m.cyl((-.025,0,-.06),(-.005,0,-.11),.006,'Steel');m.box((.018,0,.052),(.07,.025,.012),'Steel',.003)
for y in [-.036,.036]:m.cyl((.09,y,-.025),(.11,y,-.025),.012,'Steel')
m.finish('DoubleBarrelV18')
for y in [-.024,.024]:m.tube((0,y,.028),(.68,y,.028),.023,.018,'Steel',n=16)
m.box((.3,0,-.020),(.32,.105,.055),'Wood',.014);m.box((.33,0,.054),(.65,.016,.008),'Steel',.002);ell((.66,0,.064),(.008,.008,.008),'Bone');m.finish('DoubleBarrelBarrelsV18')
# Furniture with actual profile, control panels, trims and fitted detail.
m.box((0,0,.40),(.74,.70,.80),'Rubber',.03);m.box((0,.18,1.20),(.74,.32,.90),'ArcadeMuralV18',.035);m.box((0,-.18,1.03),(.76,.44,.10),'Steel',.02)
m.box((0,-.008,1.40),(.62,.035,.47),'Glass',.02);m.box((0,-.03,1.40),(.53,.012,.38),'ArcadeMuralV18',.008)
m.box((0,-.08,1.77),(.77,.20,.18),'ArcadeMuralV18',.012)
for x in [-.22,.05]:m.cyl((x,-.27,1.08),(x,-.27,1.18),.012,'Steel');ell((x,-.27,1.19),(.033,.033,.033),'Red')
for x in [-.10,-.02,.17,.25]:m.cyl((x,-.30,1.08),(x,-.30,1.10),.020,'Red',n=12)
m.box((0,-.363,.55),(.20,.015,.28),'Steel',.012);m.box((0,-.376,.63),(.12,.01,.017),'Rubber');m.finish('ArcadeCabinetV18',collision='complex')
m.box((0,0,.74),(1.5,.85,.12),'Wood',.02)
for x in [-.63,.63]:m.box((x,0,.36),(.13,.65,.72),'Rubber',.015)
m.box((0,.1,.90),(1.35,.60,.18),'Glass',.01,rot=(.10,0,0));m.box((0,.38,1.40),(1.35,.12,.72),'ArcadeMuralV18',.015)
for x in [-.48,0,.48]:ell((x,0,1.01),(.09,.09,.04),'Steel')
m.finish('PinballV18',collision='complex')
m.box((0,0,.70),(2.1,1.15,.18),'Steel',.05)
for x in [-.8,.8]:m.box((x,0,.32),(.18,.90,.64),'Rubber',.025)
m.box((0,0,.801),(1.9,.96,.015),'Bone');m.box((0,0,.814),(.015,.96,.01),'Red')
for x in [-.7,.7]:m.cyl((x,0,.82),(x,0,.89),.085,'Red',n=16)
m.finish('AirHockeyV18',collision='complex')
m.box((0,0,.92),(1.4,.65,.08),'Wood',.015);m.box((0,.26,.5),(1.4,.08,1),'PolicePaintV18',.015)
for x in [-.6,.6]:m.box((x,0,.44),(.08,.55,.88),'Steel',.01)
m.box((-.27,.02,1.19),(.54,.09,.38),'Rubber',.01);m.box((-.27,-.03,1.19),(.47,.012,.29),'Glass');m.box((.3,-.1,.98),(.28,.2,.08),'Rubber',.01)
for i in range(6):m.box((.2+i*.04,-.17,1.025),(.025,.045,.012),'Bone')
m.finish('PoliceDispatchV18',collision='complex')
m.box((0,0,.70),(1.3,.5,1.4),'PolicePaintV18',.012)
for z in [.22,.65,1.09]:
 m.box((0,-.26,z),(1.20,.02,.37),'Steel',.005);m.box((0,-.29,z+.02),(.18,.04,.035),'Rubber',.004);m.box((-.4,-.28,z+.1),(.20,.015,.07),'Bone')
m.finish('EvidenceCabinetV18',collision='complex')
for x in [-.8,.8]:m.box((x,0,.42),(.05,.6,.84),'Steel',.008)
m.box((0,0,.85),(1.8,.65,.12),'Cloth',.03);m.box((.58,0,.94),(.48,.56,.10),'Bone',.03)
for x in [-.8,.8]:m.box((x,.3,.70),(.05,.05,1.4),'Steel',.008)
m.finish('CellBunkV18',collision='complex')
m.box((0,0,.45),(2.1,.7,.9),'PolicePaintV18',.02);m.box((0,0,.94),(2.2,.82,.08),'Steel',.012)
for x in [-.7,0,.7]:m.box((x,-.36,.45),(.64,.02,.76),'Wood',.01);m.box((x,-.39,.7),(.15,.03,.035),'Steel')
m.finish('ShopCounterV18',collision='complex')
m.box((0,0,.23),(1.15,1.15,.46),'Concrete',.045);m.box((0,0,.48),(1.05,1.05,.03),'Wood')
for i in range(8):a=i*math.tau/8;limb((0,0,.5),(.4*math.cos(a),.4*math.sin(a),1.25),.03,'Wood',.008);ell((.35*math.cos(a),.35*math.sin(a),1.18),(.23,.22,.18),'Cloth')
m.finish('PlazaPlanterV18',collision='complex')
# Body parts have explicit pivots, used by the matching runtime creature definitions.
for kind in ['Moose','Titan','Deathclaw','Scorpion','Karen']:
 mat='CreatureHideV18' if kind in ['Moose','Deathclaw','Scorpion'] else 'Skin'
 if kind=='Moose':
  ell((-.15,0,0),(1.05,.43,.62),mat);ell((.40,0,.25),(.48,.47,.55),mat);m.finish(kind+'TorsoV18')
  ell((.15,0,-.07),(.46,.22,.22),mat);ell((.47,0,-.18),(.20,.22,.20),'Rubber')
  for side in [-1,1]:
   horn([(0,side*.13,.1),(-.18,side*.45,.5),(-.4,side*.80,.75)],.065)
   for i in range(5):horn([(-.15-i*.06,side*(.3+i*.11),.35+i*.06),(.02+i*.03,side*(.45+i*.11),.80+i*.09)],.045)
   ell((-.14,side*.25,.15),(.17,.09,.06),mat);ell((.20,side*.205,.07),(.05,.02,.045),'Glow')
  m.finish(kind+'HeadV18');ell((0,0,0),(.43,.38,.45),mat);m.finish(kind+'PelvisV18')
  for part in ['Arm','Leg']:
   limb((0,0,0),(.10,0,-.6),.11,mat,.07);limb((.1,0,-.6),(0,0,-1.15),.07,mat,.05);m.box((.06,0,-1.18),(.22,.16,.13),'Rubber',.02);m.finish(kind+part+'V18')
 elif kind=='Scorpion':
  for i in range(5):ell((-.6+i*.27,0,0),(.25,.40-i*.025,.22),mat)
  horn([(-.7,0,0),(-1,0,.2),(-1.15,0,.60),(-.95,0,1.0),(-.60,0,1.15),(-.30,0,.90)],.16)
  m.finish(kind+'TorsoV18');ell((0,0,0),(.25,.3,.18),mat)
  for side in [-1,1]:ell((.15,side*.12,.1),(.04,.04,.04),'Glow')
  m.finish(kind+'HeadV18');ell((0,0,0),(.2,.3,.18),mat);m.finish(kind+'PelvisV18')
  limb((0,0,0),(.4,0,.08),.12,mat,.10);ell((.55,0,.1),(.22,.17,.13),mat)
  for side in [-1,1]:horn([(.55,side*.12,.1),(.78,side*.19,.12),(.98,side*.06,.12)],.07)
  m.finish(kind+'ArmV18')
  for i in range(4):limb((.32-i*.22,0,0),(.40-i*.30,.50+i*.025,.1),.055,mat,.035);limb((.40-i*.30,.50+i*.025,.1),(.60-i*.48,.85+i*.025,-.50),.04,mat,.008)
  m.finish(kind+'LegV18')
 else:
  big=kind=='Titan';claw=kind=='Deathclaw';fat=kind=='Karen';scale=1.0
  if fat:ell((0,0,-.10),(.36,.43,.45),'Red');ell((.12,0,-.3),(.40,.42,.30),'Red')
  else:
   ell((0,0,-.12),(.26,.40 if big else .30,.45),mat)
   for side in [-1,1]:ell((.17,side*.19,-.02),(.18,.21,.20),mat);ell((-.08,side*.29,.13),(.22,.22,.20),mat)
   for z in [-.19,-.33]:ell((.20,0,z),(.09,.21,.08),mat)
  if claw:
   for i in range(6):horn([(-.20,0,.18-i*.12),(-.45,0,.35-i*.10)],.06)
  m.finish(kind+'TorsoV18')
  ell((0,0,0),(.20 if claw else .15,.19 if claw else .16,.23 if claw else .21),mat);ell((.21 if claw else .10,0,-.1),(.30 if claw else .17,.15 if claw else .14,.12 if claw else .1),mat)
  for side in [-1,1]:ell((.14,side*.1,.04),(.025,.025,.02),'Glow')
  if claw:
   for side in [-1,1]:horn([(-.08,side*.13,.1),(-.18,side*.26,.38),(.04,side*.3,.50)],.075)
   for i in range(7):m.cyl((.14+i*.016,-.09,-.09),(.14+i*.016,-.09,-.16),.014,'Bone',r2=0,n=5)
  if fat:ell((-.06,0,.1),(.17,.19,.19),'Bone');ell((-.14,0,-.05),(.10,.18,.18),'Bone')
  m.finish(kind+'HeadV18')
  if fat:
   m.box((.10,0,-.32),(.85,.63,.15),'Steel',.05);m.box((0,0,-.02),(.52,.62,.14),'Rubber',.03);m.box((-.24,0,.23),(.08,.63,.48),'Rubber',.03)
   for x in [-.18,.5]:
    for y in [-.34,.34]:m.cyl((x,y-.05,-.41),(x,y+.05,-.41),.14,'Rubber',n=16)
   m.cyl((.57,0,-.23),(.55,0,.45),.035,'Steel');m.cyl((.55,-.28,.45),(.55,.28,.45),.025,'Steel')
  else:ell((0,0,0),(.20,.25,.20),'Cloth' if big else mat)
  if claw:horn([(-.15,0,0),(-.65,0,-.1),(-1.05,0,-.25),(-1.45,0,-.12)],.14)
  m.finish(kind+'PelvisV18')
  limb((0,0,0),(.08,0,-.34),.16 if big else .13,mat,.10);ell((.02,0,-.15),(.16,.14,.23),mat);limb((.08,0,-.34),(.2,0,-.65),.12,mat,.075);ell((.22,0,-.68),(.11,.12,.1),mat)
  for j in range(4):
   y=(j-1.5)*.05
   if claw:horn([(.23,y,-.70),(.37,y,-.86),(.43,y,-1.02)],.025)
   else:limb((.22,y,-.7),(.27,y,-.82),.025,mat,.02)
  m.finish(kind+'ArmV18')
  if claw:
   limb((0,0,0),(.18,0,-.40),.19,mat,.13);limb((.18,0,-.40),(-.20,0,-.82),.12,mat,.09);limb((-.20,0,-.82),(.02,0,-1.04),.09,mat,.07);ell((.1,0,-1.05),(.27,.14,.1),mat)
  else:limb((0,0,0),(.07,0,-.40),.17 if big else .13,mat,.10);limb((.07,0,-.40),(-.08,0,-.77),.10,mat,.07);ell((.02,0,-.78),(.24,.12,.1),'Rubber')
  if claw:
   for j in range(3):horn([(.24,(j-1)*.09,-1.05),(.49,(j-1)*.10,-1.10)],.035)
  m.finish(kind+'LegV18')
for r in m.RECORDS.values():
 if r["name"].startswith(("Moose","Titan","Deathclaw","Scorpion","Karen")):r["collision"]="convex"
m.export_all();(m.OUT/'models_v18_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'AllAmericanMeltdown_V18.blend'))
