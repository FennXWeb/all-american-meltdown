"""Original arsenal models in metres, rigid moving parts with matching runtime pivots."""
import sys,math,json
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent));import make_models_v2 as m
import bpy
m.OUT=m.ROOT/'ArtSource/ModelsV24';m.setup()
def box(p,d,mat='Steel',bevel=.004):m.box(p,d,mat,bevel)
def barrel(a,b,r=.015,mat='Steel'):m.cyl(a,b,r,mat,n=12)
def grip(x=-.1):
 box((x,0,-.11),(.055,.045,.16),'Rubber');box((x+.045,0,-.075),(.08,.05,.014));barrel((x+.085,0,-.015),(x+.085,0,-.075),.008)
def rail(a,b,z):
 box(((a+b)/2,0,z),(b-a,.033,.012))
 for i in range(int((b-a)/.018)):box((a+i*.018,0,z+.008),(.009,.041,.012),'Rubber',.001)
def sight(x,z):
 for y in [-.015,.015]:box((x,y,z+.012),(.016,.009,.03))
 box((x,0,z),(.024,.04,.013))
def finish(n):m.finish(n,collision='none')
# Shoulder launcher, insulated tube, vented backblast cone, sight, forward grip.
barrel((-.3,0,.03),(.83,0,.03),.071,'Rust');barrel((-.36,0,.03),(-.25,0,.03),.085,'Steel')
for x in [-.22,.18,.68]:barrel((x,0,.03),(x+.025,0,.03),.077)
box((-.16,0,-.065),(.28,.12,.065),'Rubber');grip(-.08);grip(.38);rail(-.04,.30,.11);sight(.25,.13)
for x in [-.3,.75]:box((x,0,.11),(.025,.03,.07))
finish('MissileLauncher24')
barrel((-.18,0,0),(.20,0,0),.048,'Rust');m.cyl((.2,0,0),(.30,0,0),.048,'Red',r2=.004,n=12)
for i in range(4):
 a=i*math.pi/2;box((-.12,.058*math.sin(a),.058*math.cos(a)),(.12,.018 if i%2 else .055,.055 if i%2 else .018))
finish('Missile24')
# Rotary gun receiver and separate six-barrel rotor.
barrel((-.22,0,0),(.20,0,0),.095,'Steel');box((-.13,0,-.09),(.32,.14,.075),'Rust');grip(-.14)
barrel((-.1,-.095,.1),(.12,-.095,.1),.018,'Rubber');barrel((-.1,-.095,.1),(-.1,0,.07),.015);barrel((.12,-.095,.1),(.12,0,.07),.015)
rail(-.13,.16,.11);finish('Minigun24')
box((0,0,0),(.25,.13,.20),'Rust');box((0,0,.105),(.27,.15,.018));finish('MinigunBox24')
for i in range(6):
 a=i*math.tau/6;y=.043*math.sin(a);z=.043*math.cos(a);barrel((0,y,z),(.64,y,z),.017);barrel((.63,y,z),(.655,y,z),.014,'Rubber')
for x in [.05,.43,.59]:barrel((x,0,0),(x+.025,0,0),.071,'Steel')
finish('MinigunBarrels24')
# Sawed-off: compact walnut pistol stock and break-action barrel assembly.
box((.045,0,0),(.13,.075,.065),'Steel');box((-.065,0,-.025),(.14,.065,.08),'Wood');grip(-.08);sight(.06,.04);finish('SawedOff24')
for y in [-.023,.023]:barrel((0,y,.03),(.32,y,.03),.022);barrel((.31,y,.03),(.325,y,.03),.016,'Rubber')
box((.14,0,-.007),(.20,.062,.028),'Wood');box((.15,0,.052),(.30,.01,.01));finish('SawedOffBarrels24')
# Heavy pistol angular slide, bore, controls, serrations, integral rails.
box((.04,0,-.006),(.25,.049,.035),'Rust');grip(-.04);box((.02,-.03,.01),(.05,.009,.012));finish('DesertEagle24')
box((.09,0,.03),(.32,.055,.06),'Steel');barrel((.22,0,.035),(.29,0,.035),.014);sight(-.035,.068);sight(.23,.067)
for x in [-.05,-.035,-.02,-.005]:box((x,-.029,.032),(.005,.003,.045),'Rubber',.001)
finish('DeagleSlide24');box((0,0,-.055),(.038,.036,.12),'Steel');box((0,0,-.12),(.047,.04,.012),'Rubber');finish('DeagleMag24')
# M4: telescopic stock, flattop, quad rail, gas block, suppressor-ready muzzle.
box((.08,0,0),(.34,.064,.095));barrel((-.36,0,.0),(-.08,0,0),.016);box((-.29,0,-.034),(.18,.065,.11),'Rubber');grip(-.12)
box((.10,0,-.06),(.07,.061,.055));barrel((.25,0,.03),(.64,0,.03),.013)
box((.32,0,.015),(.25,.077,.073),'Rust');rail(-.06,.44,.073)
for x in [.24,.27,.30,.33,.36,.39,.42]:
 for y in [-.04,.04]:box((x,y,.014),(.018,.005,.038),'Rubber',.001)
box((.47,0,.053),(.021,.035,.075));barrel((.61,0,.03),(.65,0,.03),.019);sight(-.03,.08);finish('M424')
# Taser: safety-yellow nonlethal chassis and replaceable two-dart cassette.
box((.045,0,.02),(.20,.068,.065),'Bone');grip(-.055);box((.01,-.036,.04),(.04,.007,.025),'Red');sight(-.02,.065);finish('Taser24')
box((.02,0,0),(.11,.066,.055),'Rubber')
for z in [-.013,.013]:barrel((.07,0,z),(.105,0,z),.007,'Steel')
finish('TaserCartridge24')
# Flame projector: heatshield with rows of vents, pilot, valves and side tank.
barrel((-.14,0,0),(.73,0,0),.03,'Steel');grip(-.13);grip(.27)
barrel((.31,0,0),(.67,0,0),.052,'Rust')
for x in [.35,.4,.45,.5,.55,.6]:
 for y in [-.05,.05]:box((x,y,0),(.026,.006,.023),'Rubber',.001)
barrel((.60,0,-.065),(.75,0,-.045),.009);box((-.04,0,.053),(.08,.085,.045),'Red');rail(-.06,.2,.07);barrel((.06,-.02,-.02),(.06,-.09,-.04),.016,'Rubber');finish('Flamethrower24')
barrel((0,0,-.15),(0,0,.08),.071,'Red');barrel((0,0,.08),(0,0,.12),.019);box((0,0,-.03),(.15,.15,.025),'Rubber');finish('FuelTank24')
m.export_all();(m.OUT/'models_v24_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'Arsenal24.blend'))
print('ARSENAL24_MODELS_COMPLETE',len(m.RECORDS))
