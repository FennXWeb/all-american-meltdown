"""Original grounded furniture and room dressing. Blender metres; no packaging."""
import sys,json,math
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent))
import make_models_v2 as m
import bpy
m.OUT=m.ROOT/'ArtSource/ModelsV13';m.setup()
def legs(x,y,h,wood='Wood'):
 for a in [-1,1]:
  for b in [-1,1]:m.box((a*x,b*y,h/2),(.055,.055,h),wood,.008)
def drawers(x,y,z,w):
 for zz in [z-.2,z,z+.2]:
  m.box((x,y,zz),(w,.025,.17),'Wood',.009);m.cyl((x-.09,y-.025,zz),(x+.09,y-.025,zz),.011,'Steel')
# Upholstered sofa: feet, rolled arms, separate cushions, piping and loose pillows.
legs(.91,.34,.15)
m.box((0,0,.29),(2.05,.82,.29),'Wood',.035)
m.box((0,.34,.66),(2.10,.22,.76),'Cloth',.08)
for a in [-1,1]:m.box((a*1.02,0,.58),(.22,.92,.5),'Cloth',.075)
for x in [-.65,0,.65]:
 m.box((x,-.035,.49),(.61,.67,.17),'Cloth',.05)
 m.box((x,.22,.79),(.61,.18,.45),'Cloth',.045)
for x in [-.76,.79]:m.box((x,.08,.72),(.32,.19,.32),'Red',.055,rot=(.2,0,x*.2))
m.finish('SofaV13',collision='convex')
for name,x,y,h in [('CoffeeTableV13',1.25,.65,.43),('DiningTableV13',1.8,.95,.78)]:
 legs(x*.4,y*.35,h-.07);m.box((0,0,h-.045),(x,y,.09),'Wood',.025)
 for a in [-1,1]:m.box((0,a*y*.35,h-.15),(x*.85,.045,.15),'Wood')
 m.finish(name,collision='convex')
for name,w,h in [('SideboardV13',1.65,.85),('NightstandV13',.52,.59)]:
 legs(w*.4,.2,.12);m.box((0,0,h*.5+.04),(w,.52,h-.1),'Wood',.015);m.box((0,0,h),(w+.04,.56,.04),'Wood',.009)
 if w>1:
  for x in [-.53,0,.53]:drawers(x,-.272,h*.53,.49)
 else:
  for z in [.25,.46]:m.box((0,-.27,z),(.46,.03,.17),'Wood',.01);m.cyl((-.07,-.30,z),(.07,-.30,z),.01,'Steel')
 m.finish(name,collision='convex')
# Bookcase with unevenly filled shelves; books form groups instead of solid cuboids.
for x in [-.6,.6]:m.box((x,0,.96),(.06,.38,1.92),'Wood',.01)
m.box((0,.17,.96),(1.2,.035,1.92),'Wood')
for j in range(5):
 z=.12+j*.43;m.box((0,0,z),(1.2,.38,.04),'Wood')
 if j<4:
  for k in range(9 if j%2 else 6):
   x=-.51+k*.075;hh=.24+(k%3)*.035;m.box((x,0,z+.02+hh/2),(.052,.25,hh),['Red','Bone','Cloth'][k%3],.004)
   m.box((x,-.129,z+hh*.72),(.047,.003,.012),'Bone')
m.finish('BookcaseV13',collision='convex')
legs(.82,.61,.32);m.box((0,0,.31),(2.05,1.5,.16),'Wood',.025)
m.box((-.99,0,.62),(.09,1.6,1.05),'Wood',.035)
m.box((.99,0,.34),(.07,1.58,.48),'Wood',.02)
m.box((0,0,.49),(1.95,1.42,.23),'Bone',.07)
m.box((.28,0,.615),(1.38,1.44,.045),'Cloth',.018)
for y in [-.36,.36]:m.box((-.66,y,.66),(.45,.59,.15),'Bone',.065,rot=(0,0,y*.08))
for x in [-.22,.08,.38,.68]:m.box((x,0,.64),(.015,1.44,.008),'Red')
m.finish('HomeBedV13',collision='convex')
# Dressing pieces use their support surface as z=0.
m.cyl((0,0,0),(0,0,.010),.12,'Bone',n=20);m.tube((0,0,.010),(0,0,.019),.12,.095,'Bone',n=20)
m.cyl((.19,.03,0),(.19,.03,.10),.034,'Bone',n=12);m.tube((.19,.03,.10),(.19,.03,.105),.034,.026,'Bone',n=12)
for x in [-.16,.16]:m.box((x,-.02,.013),(.012,.18,.012),'Steel')
m.finish('TableSettingV13')
for i in range(5):m.box((i*.009,i*.013,.004+i*.005),(.29,.21,.008),'Bone',rot=(0,0,i*.02))
m.box((.21,0,.025),(.12,.18,.05),'Red',.008);m.cyl((-.23,.03,0),(-.23,.03,.11),.035,'Steel');m.cyl((-.23,.03,.10),(-.20,.03,.20),.004,'Wood')
m.finish('DeskSetV13')
m.cyl((0,0,0),(0,0,.018),.11,'Steel',n=16);m.cyl((0,0,.018),(0,0,.37),.018,'Steel');m.cyl((0,0,.29),(0,0,.52),.17,'Bone',r2=.09,n=12);m.cyl((0,0,.52),(0,0,.54),.015,'Steel');m.finish('TableLampV13')
for i in range(12):m.box((-.49+i*.089,0,.4),(.052,.14,.65),'Bone',.02)
for z in [.15,.66]:m.cyl((-.55,0,z),(.55,0,z),.025,'Steel')
for x in [-.4,.4]:m.box((x,0,.08),(.045,.16,.16),'Steel')
m.finish('RadiatorV13',collision='convex')
# A grouped pantry assortment: boxes, jars, cans, and bottles.
for i in range(6):
 x=-.35+i*.14
 if i%2:m.box((x,0,.095),(.105,.14,.19),'Bone' if i%3 else 'Red',.008)
 else:m.cyl((x,0,0),(x,0,.16),.046,'Steel',n=12);m.cyl((x,0,.162),(x,0,.174),.048,'Bone',n=12)
m.finish('PantryV13')
m.tube((0,0,.015),(0,0,.45),.18,.16,'Steel',n=12);m.cyl((0,0,0),(0,0,.02),.18,'Steel',n=12);m.finish('WasteBinV13',collision='convex')
m.box((0,0,0),(1.05,.24,.07),'Steel',.02)
for y in [-.065,.065]:m.cyl((-.44,y,-.05),(.44,y,-.05),.025,'Bone',n=8)
m.finish('CeilingLightV13')
# Small wall service panel: face is toward local -Y.
m.box((0,0,0),(.48,.10,.63),'Steel',.015);m.box((0,-.058,0),(.40,.02,.55),'Cloth',.005)
for z in [-.15,0,.15]:m.box((-.08,-.076,z),(.14,.025,.055),'Rubber');m.box((.11,-.077,z),(.075,.027,.06),'Red')
m.finish('FuseBoxV13')
# Closed unit gable, scaled to each house span and roof rise.
m.mesh('Gable',[(x,y,z) for x in [-.12,.12] for y,z in [(-1,0),(1,0),(0,1)]],[(0,2,1),(3,4,5),(0,1,4,3),(1,2,5,4),(2,0,3,5)],'Wood');m.finish('GableV13')
# Curtains hang from a mounted rail, without filling the glass opening.
m.cyl((-.91,0,.82),(.91,0,.82),.016,'Steel')
for side in [-1,1]:
 for i in range(5):m.box((side*(.55+i*.065),-.035+(i%2)*.03,0),(.068,.055,1.6),'Cloth',.008)
m.finish('CurtainsV13')
# Framed landscape relief: quiet domestic art instead of warning signs in bedrooms.
m.box((0,0,0),(1.3,.035,.85),'Wood')
m.box((0,-.024,0),(1.18,.012,.73),'Bone')
m.cyl((.30,-.032,.16),(.30,-.04,.16),.105,'Red',n=20)
m.mesh('Landscape',[(-.59,-.045,-.36),(-.59,-.045,-.06),(-.25,-.045,.22),(.12,-.045,-.15),(.31,-.045,.06),(.59,-.045,-.1),(.59,-.045,-.36)],[(0,1,2,3),(0,3,4,5,6)],'Cloth')
for side in [-1,1]:m.box((side*.62,-.04,0),(.055,.055,.85),'Wood',.006);m.box((0,-.04,side*.4),(1.25,.055,.055),'Wood',.006)
m.finish('LandscapeV13')
# Open bathtub: inset basin, rim, feet and faucet instead of a solid placeholder box.
legs(.64,.23,.12,'Steel');m.box((0,0,.14),(1.52,.62,.08),'Bone',.035)
for side in [-1,1]:
 m.box((0,side*.34,.34),(1.64,.10,.42),'Bone',.04);m.box((side*.79,0,.34),(.12,.64,.42),'Bone',.04)
 m.box((0,side*.35,.56),(1.74,.14,.055),'Bone',.025);m.box((side*.81,0,.56),(.14,.70,.055),'Bone',.025)
m.cyl((.57,0,.183),(.57,0,.19),.028,'Steel',n=12)
m.line([(.76,.21,.56),(.76,.21,.76),(.57,.21,.76)],.022,'Steel')
m.finish('BathtubV13',collision='convex')
m.export_all();(m.OUT/'models_v13_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'LethalWorld_V13.blend'));print('LW_V13_MODELS_COMPLETE',len(m.RECORDS))
