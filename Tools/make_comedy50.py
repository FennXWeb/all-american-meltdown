"""Comedy arsenal: independently animated mechanisms, metres, original meshes."""
import sys, math, json
from pathlib import Path
import bpy
sys.path.insert(0,str(Path(__file__).parent));import make_models_v2 as m
m.OUT=m.ROOT/'ArtSource/ModelsV50';m.PALETTE.update({'ComedySkin50':(.55,.34,.22),'ComedyPlastic50':(.65,.69,.56)});m.setup()
def box(p,d,mat='Steel',b=.004):m.box(p,d,mat,b)
def tube(a,b,r=.015,mat='Steel'):m.cyl(a,b,r,mat,n=16)
def finish(n):m.finish(n,collision='none')
def grip(x=-.12,scale=1):
 box((x,0,-.1*scale),(.07*scale,.05*scale,.17*scale),'Rubber')
 box((x+.07*scale,0,-.09*scale),(.1*scale,.045*scale,.016*scale))
 tube((x+.115*scale,0,-.025*scale),(x+.115*scale,0,-.09*scale),.008*scale)
def sights(a,b,z):
 for x in [a,b]:
  for y in [-.016,.016]:box((x,y,z),(.02,.009,.03))
# Ten chambers in a pentagonal double ring, ten distinct open muzzle bores.
box((.02,0,0),(.22,.19,.13));grip(-.1)
box((-.26,0,-.025),(.27,.085,.11),'Wood');box((-.4,0,-.055),(.04,.09,.18),'Rubber')
for y in [-.103,.103]:box((.03,y,0),(.19,.012,.10),'Rust')
sights(-.02,.09,.11);finish('TenBarrel50')
for row in range(2):
 for col in range(5):
  y=(col-2)*.038;z=(row-.5)*.042+.03
  tube((0,y,z),(.70,y,z),.019);tube((.698,y,z),(.713,y,z),.014,'Rubber')
for x in [.08,.48]:box((x,0,.03),(.025,.205,.102),'Rust')
box((.33,0,-.04),(.38,.18,.035),'Wood');finish('TenBarrels50')
# Oversized polymer pistol, serrated slide, trigger guard, ejection port, sights.
box((.18,0,.025),(.73,.105,.115),'Steel',.013);box((.13,0,-.045),(.51,.10,.035),'Rubber');grip(-.08,1.8)
box((.16,-.056,.045),(.12,.008,.05),'Rubber');tube((.48,0,.045),(.66,0,.045),.03);tube((.659,0,.045),(.663,0,.045),.021,'Rubber')
for x in [-.15,-.12,-.09,-.06]:
 for y in [-.056,.056]:box((x,y,.02),(.008,.006,.085),'Rubber',.001)
sights(-.13,.48,.094);finish('GiantGlock50')
box((0,0,-.095),(.072,.071,.21));box((0,0,-.20),(.09,.083,.02),'Rubber');finish('GiantMag50')
# AK stamped receiver with rivets, curved extended magazine, wooden stock/handguard.
box((.02,0,0),(.38,.07,.085));tube((-.15,0,.044),(.18,0,.044),.034);grip()
box((-.32,0,-.01),(.26,.08,.11),'Wood');box((-.46,0,-.045),(.035,.085,.17),'Rubber')
tube((.19,0,.03),(.68,0,.03));tube((.20,0,.066),(.48,0,.066),.012)
box((.3,0,0),(.23,.09,.075),'Wood');sights(.12,.57,.096)
for x in [-.12,-.08,.03,.14]:
 for y in [-.037,.037]:tube((x,y,0),(x,y*1.04,0),.005)
box((.03,-.045,.014),(.08,.014,.015));finish('QuestionableAK50')
for n in range(12):
 a=n/11*1.1;x=.025+.14*(1-math.cos(a));z=-n*.025
 box((x,0,z),(.075,.047,.037),'Steel',.003)
 for y in [-.026,.026]:box((x,y,z),(.048,.006,.019),'Rust',.001)
finish('AKMag50')
# A posed hand with an extended index and raised thumb; rounded joints and folded fingers.
box((.14,0,-.016),(.115,.074,.043),'ComedySkin50',.018)
tube((.075,0,-.02),(-.10,0,-.06),.032,'ComedySkin50')
tube((.18,-.022,.0),(.26,-.022,.015),.012,'ComedySkin50');tube((.26,-.022,.015),(.335,-.022,.016),.010,'ComedySkin50')
tube((.13,.04,-.005),(.13,.065,.045),.014,'ComedySkin50');tube((.13,.065,.045),(.16,.063,.076),.012,'ComedySkin50')
for j in range(3):
 y=-.005+j*.02;tube((.19,y,-.014),(.213,y,-.04),.012,'ComedySkin50');tube((.213,y,-.04),(.17,y,-.055),.011,'ComedySkin50')
finish('FingerGuns50')
# Thin plastic receiver with exposed seam, moulding ribs, screw heads; barrel separate at flexible neck.
box((.04,0,0),(.38,.065,.086),'ComedyPlastic50',.012);grip()
box((-.3,0,-.018),(.26,.066,.11),'ComedyPlastic50',.008);box((-.435,0,-.04),(.03,.065,.16),'Rubber')
box((.08,0,-.065),(.07,.06,.06),'ComedyPlastic50');box((.025,0,-.17),(.065,.042,.19),'Rubber')
for x in [-.12,-.03,.07,.16]:
 for y in [-.034,.034]:box((x,y,0),(.006,.003,.068),'Red',.001)
sights(-.1,.17,.064);finish('BudgetCut50')
tube((0,0,0),(.40,0,0),.018,'ComedyPlastic50');box((.10,0,0),(.19,.068,.055),'ComedyPlastic50')
for x in [.03,.055,.08,.105,.13,.155]:box((x,0,.033),(.009,.075,.008),'Red',.001)
tube((.38,0,0),(.415,0,0),.023,'Rubber');finish('BudgetBarrel50')
m.export_all();(m.OUT/'models_v50_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'Comedy50.blend'));print('COMEDY50_COMPLETE')
