import sys,json,math
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent))
import bpy
import make_models_v2 as m
m.OUT=m.ROOT/'ArtSource/Workbench39';m.OUT.mkdir(exist_ok=True);m.setup()
# A steel tool bench: drawers, vise, parts trays and a perforated tool rack.
m.box((0,0,.89),(2.10,.86,.075),'Wood',.012)
for x in [-.92,.92]:
 for y in [-.32,.32]:m.box((x,y,.43),(.065,.065,.86),'Steel',.005)
m.box((0,.27,.25),(1.9,.035,.035),'Steel',.003)
for x in [-.67,.67]:
 m.box((x,0,.61),(.47,.73,.46),'Rust',.008)
 for z in [.44,.61,.78]:
  m.box((x,-.378,z),(.435,.025,.135),'Steel',.003);m.cyl((x-.08,-.408,z),(x+.08,-.408,z),.012,'Rubber',n=10)
m.box((0,-.04,.94),(.90,.57,.018),'Rubber',.006)
for x in [-1,1]:m.box((x,.37,1.24),(.055,.06,.80),'Steel',.003)
m.box((0,.395,1.42),(2,.024,.40),'Rust',.004)
for x in range(-9,10):
 for z in range(4):m.box((x*.1,.377,1.28+z*.095),(.012,.006,.012),'Rubber')
for x in [-.8,-.5,-.2,.15,.48,.78]:
 m.cyl((x,.34,1.33),(x,.34,1.52),.011,'Steel',n=8)
 m.box((x,.335,1.31),(.035,.035,.07),'Rubber',.004)
# Articulated vise and crank, separate magnetic trays with loose rounds.
m.box((-.74,-.13,.985),(.34,.24,.13),'Steel',.008)
for x in [-.84,-.64]:m.box((x,-.13,1.06),(.055,.25,.13),'Rust',.005)
m.cyl((-.96,-.13,.995),(-.53,-.13,.995),.02,'Steel',n=14)
m.cyl((-.5,-.13,.91),(-.5,-.13,1.09),.01,'Steel',n=10)
for y in [-.27,.02]:
 m.box((.73,y,.957),(.34,.20,.022),'Steel',.004)
 for x in [.56,.90]:m.box((x,y,.982),(.014,.20,.05),'Steel',.002)
 for yy in [y-.10,y+.10]:m.box((.73,yy,.982),(.34,.014,.05),'Steel',.002)
 for j in range(4):m.cyl((.63+j*.055,y-.05,.98),(.63+j*.055,y+.055,.98),.011,'Bone',n=8)
m.cyl((.80,.33,.94),(.80,.33,1.86),.022,'Steel',n=12)
m.cyl((.80,.33,1.86),(.42,.09,1.86),.022,'Steel',n=12)
m.box((.4,.05,1.82),(.36,.23,.08),'Steel',.014)
m.box((.4,.05,1.774),(.28,.18,.012),'Bone',.006)
m.finish('WeaponBench39',collision='convex');m.export_all()
(m.OUT/'bench39_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2))
bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'WeaponBench39.blend'));print('WEAPON_BENCH39_COMPLETE')
