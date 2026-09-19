import sys,json
from pathlib import Path
import bpy
sys.path.insert(0,str(Path(__file__).parent));import make_models_v2 as m
m.OUT=m.ROOT/'ArtSource/ModelsV52';m.setup()
def end(n):m.finish(n,collision='convex')
m.box((0,0,0),(.36,.28,.014),'Wood',.002)
for i in range(5):m.box((i*.002,0,.012+i*.003),(.33,.25,.002),'Concrete')
for row in range(8):m.box((-.025,-.095+row*.025,.031),(.23 if row%3 else .14,.005,.001),'Rubber')
m.box((.09,.07,.032),(.085,.04,.002),'Red');end('Dossier52')
for i in range(3):
 m.cyl((0,i*.022,0),(.08+i*.015,i*.022,0),.008,'Steel',n=10);m.box((.075+i*.015,i*.022,0),(.018,.028,.012),'Steel',.002)
m.cyl((-.025,0,-.008),(-.025,0,.008),.03,'Steel',n=12);end('Keys52')
m.box((0,0,0),(.32,.23,.14),'Concrete',.018);m.box((0,-.122,0),(.15,.012,.035),'Red');m.box((0,-.129,0),(.035,.014,.09),'Red')
for x in [-.09,.09]:m.box((x,0,.08),(.012,.15,.013),'Steel')
end('MedicalCase52')
m.box((0,0,0),(.45,.15,.65),'Steel',.025);m.box((0,-.082,.1),(.31,.02,.22),'Rubber',.008)
for i in range(3):m.box((-.11+i*.11,-.099,.11),(.06,.012,.035),'Glow')
m.cyl((.10,-.1,-.12),(.10,-.23,-.02),.018,'Red');m.box((-.11,-.092,-.12),(.07,.015,.08),'Concrete');end('ControlPanel52')
m.box((0,0,.38),(1.25,.65,.76),'Steel',.04)
for x in [-.37,0,.37]:
 m.box((x,-.33,.59),(.29,.025,.24),'Rubber');m.box((x,-.35,.60),(.22,.012,.12),'Glow');
 for y in range(3):m.box((x-.09+y*.09,-.35,.30),(.045,.025,.035),'Red')
m.cyl((.54,.15,.7),(.54,.15,1.65),.018,'Steel');end('RelayConsole52')
m.cyl((-.06,0,0),(.06,0,0),.012,'Steel',n=8);m.box((-.065,0,0),(.014,.035,.035),'Steel');end('Scrap52')
m.export_all();(m.OUT/'models_v52_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'Story52.blend'))
