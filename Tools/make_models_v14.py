"""Original camper and rail-mounted accessories, metres, +X forward."""
import sys,json,math
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent))
import make_models_v2 as m
import bpy
m.PALETTE.update({'CamperPearlV14':(.8,.77,.67),'CamperWalnutV14':(.25,.16,.08),'CamperLeatherV14':(.67,.62,.5),'CamperStoneV14':(.5,.49,.43)})
m.OUT=m.ROOT/'ArtSource/ModelsV14';m.setup()
# All appliances authored individually for interaction; the shell is hollow.
p='CamperPearlV14';w='CamperWalnutV14';c='CamperLeatherV14';q='CamperStoneV14'
m.box((-1.6,0,.46),(7.5,2.25,.12),w,.025)
m.box((-2.96,0,3.06),(4.83,2.30,.13),p,.05)
m.box((-.60,0,2.86),(.12,2.22,.36),p,.025)
m.box((-5.34,0,1.74),(.10,2.28,2.55),p,.035)
for side in [-1,1]:
 y=side*1.10
 m.box((-1.75,y,.91),(7.2,.08,.76),p,.02)
 m.box((-2.99,y,2.83),(4.70,.08,.36),p,.02)
 for x in [-5.29,-3.82,-2.8,-1.4,-.7]:m.box((x,y,2.04),(.09,.09,1.24),p,.012)
 # Window surrounds: real apertures, inset sills and upper cabinets.
 for x,ww in [(-4.55,1.3),(-3.3,.84),(-2.1,1.2),(-1.05,.54)]:
  for zz in [1.3,2.62]:m.box((x,y,zz),(ww,.12,.055),'Rubber',.012)
  m.box((x,side*.88,2.81),(ww,.38,.30),w,.016)
  m.cyl((x-.1,side*.675,2.76),(x+.1,side*.675,2.76),.012,'Steel')
 # Skirt storage hatches and body trim.
 for x in [-4.5,-3.4,-2.3,-1.2]:
  m.box((x,side*1.145,.77),(.9,.018,.4),p,.02);m.box((x,side*1.16,.85),(.14,.025,.035),'Steel',.008)
 m.box((-1.7,side*1.15,1.17),(7.0,.035,.07),'Steel',.008)
 m.cyl((.1,side*1.10,1.15),(-.66,side*1.10,2.68),.065,p,n=8)
 m.cyl((.14,side*1.1,1.16),(1.50,side*1.06,.97),.07,p)
 m.box((1.88,side*.77,.8),(.33,.48,.16),'Bone',.035)
 m.cyl((.10,side*1.08,1.34),(.36,side*1.32,1.35),.025,'Steel')
 m.box((.38,side*1.32,1.46),(.08,.18,.3),'Rubber',.025)
 m.box((.43,side*1.32,1.46),(.012,.13,.24),'Glass',.01)
m.box((.8,0,1.08),(1.46,2.10,.055),p,.015,rot=(0,.13,0))
m.box((1.32,0,.62),(1.52,2.2,.31),p,.1)
m.box((2.02,0,.42),(.22,2.26,.17),'Steel',.035)
m.box((2.08,0,.69),(.05,.86,.2),'Rubber',.01)
for y in [-.34,-.17,0,.17,.34]:m.box((2.12,y,.69),(.04,.05,.14),'Steel')
m.box((-.34,0,2.70),(.5,2.22,.13),p,.03)
# Rooftop AC and vent (attached within roof perimeter).
m.box((-2.1,0,3.19),(1.05,.77,.19),p,.05)
for x in [-2.45,-2.3,-2.15,-2,-1.85]:m.box((x,0,3.29),(.035,.55,.014),'Rubber')
m.box((-4.25,0,3.15),(.5,.5,.06),'Glass',.025)
m.finish('CamperShellV14')
# High back passenger chair at local floor pivot; back faces -X.
m.box((0,0,.18),(.36,.36,.36),'Steel',.015)
m.box((0,0,.42),(.60,.47,.17),c,.055)
m.box((-.29,0,.76),(.16,.47,.69),c,.045)
m.box((-.32,0,1.13),(.14,.31,.20),c,.04)
for side in [-1,1]:
 m.box((-.05,side*.28,.68),(.47,.065,.09),w,.025)
 m.cyl((-.22,side*.25,.43),(-.22,side*.25,.65),.022,'Steel')
m.box((-.02,.235,.47),(.07,.035,.08),'Red',.007)
m.finish('CamperSeatV14')
# Bed, mattress, stitched duvet, reading lights and wardrobes.
m.box((0,0,.30),(1.42,2.0,.55),w,.025);m.box((0,0,.62),(1.45,2.0,.20),c,.06)
m.box((.18,0,.74),(1.1,1.96,.09),c,.025)
for y in [-.5,.5]:m.box((-.52,y,.77),(.35,.78,.13),c,.06)
for side in [-1,1]:m.box((-.71,side*.75,1.13),(.045,.19,.15),'Bone',.035)
m.finish('CamperBedV14')
# Kitchen cabinetry with gas hob, sink bowl and fridge as separate interactive models.
for name in ['CamperKitchenV14','CamperSinkV14','CamperFridgeV14']:
 h=1.75 if 'Fridge' in name else .91
 m.box((0,0,h/2),(.84,.52,h),w,.018)
 m.box((0,-.273,h/2),(.77,.025,h-.10),p if 'Fridge' in name else w,.01)
 m.cyl((-.20,-.30,h*.65),(.20,-.30,h*.65),.012,'Steel')
 if 'Fridge' in name:
  m.box((0,-.295,1.25),(.77,.02,.025),'Rubber');m.box((0,-.302,1.61),(.15,.01,.065),'Glow')
 else:
  m.box((0,0,h),(.9,.58,.055),q,.012)
  if 'Kitchen' in name:
   for x in [-.23,.23]:m.tube((x,0,h+.03),(x,0,h+.055),.13,.09,'Rubber',n=12)
   for x in [-.23,.23]:m.cyl((x,-.2,h+.03),(x,-.2,h+.06),.028,'Steel')
  else:
   m.box((0,0,h+.033),(.53,.37,.018),'Steel',.05);m.box((0,0,h+.045),(.44,.28,.02),'Rubber',.045)
   m.cyl((0,.2,h),(0,.2,h+.28),.018,'Steel');m.cyl((0,.2,h+.28),(0,.03,h+.28),.018,'Steel')
 m.finish(name)
# Accessories: correctly bored optics, rails, clamps, distinct bodies.
for name in ['att_light','att_laser','att_reflex','att_holo','att_scope4','att_scope8','att_vertical','att_angled']:
 m.box((0,0,0),(.065,.034,.016),'Steel',.004)
 for x in [-.025,.025]:m.cyl((x,-.022,0),(x,.022,0),.005,'Steel',n=8)
 if name in ['att_light','att_laser']:
  m.cyl((-.025,0,.017),(.075,0,.017),.018 if name=='att_light' else .01,'Rubber',n=12)
  m.tube((.06,0,.017),(.08,0,.017),.024 if name=='att_light' else .014,.018 if name=='att_light' else .009,'Steel')
  m.cyl((.076,0,.017),(.077,0,.017),.017 if name=='att_light' else .008,'Bone' if name=='att_light' else 'Red')
 elif 'scope' in name:
  length=.29 if name.endswith('8') else .20
  m.tube((-.07,0,.043),(length-.07,0,.043),.026,.020,'Rubber',n=16)
  for x in [-.03,.06]:m.tube((x,0,.043),(x+.02,0,.043),.031,.027,'Steel');m.box((x+.01,0,.019),(.024,.04,.034),'Steel')
  m.cyl((.05,0,.06),(.05,0,.09),.015,'Rubber')
 elif name in ['att_reflex','att_holo']:
  height=.052 if name=='att_holo' else .041
  for side in [-1,1]:m.box((0,side*.022,height*.5+.008),(.025,.009,height),'Rubber',.004)
  m.box((0,0,height+.008),(.027,.047,.008),'Rubber',.003)
 else:
  m.box((.014,0,-.05),(.035,.033,.1 if name=='att_vertical' else .055),'Rubber',.008,rot=(0,.35 if name=='att_angled' else 0,0))
  for z in [-.025,-.045,-.065]:m.box((.014,0,z),(.038,.035,.007),'Steel')
 m.finish(name+'V14')
# Rebuild the original receiver from its native authoring function, omitting only the scope assembly.
import inspect,textwrap
source=inspect.getsource(m.sniper)
a=source.index('    for xx in (-.052, .104):');b=source.index('    # Folded twin bipod',a)
source=source[:a]+source[b:]
source=source[:source.index('    pivot = (.020')]
source=source.replace("finish('Sniper',", "finish('SniperBareV14',")
exec(source,m.__dict__);m.sniper()
m.export_all();(m.OUT/'models_v14_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'LethalWorld_V14.blend'));print('LW_V14_MODELS_COMPLETE',len(m.RECORDS))
