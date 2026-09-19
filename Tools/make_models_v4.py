import sys,json
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent))
import make_models_v2 as m
import bpy
m.OUT=m.ROOT/'ArtSource'/'ModelsV4';m.setup()
for name,size in [('LockerV4',(.72,.60,1.85)),('FridgeV4',(.78,.68,1.72)),('StoveV4',(.88,.72,.94)),('RadioV4',(1.20,.65,1.15)),('WorkbenchV4',(1.65,.80,.90)),('SinkV4',(.72,.58,.90))]:
 x,y,z=size;m.box((0,0,z*.5),size,'Steel',.015)
 m.box((0,-y*.5-.012,z*.52),(x*.90,.025,z*.90),'Rust',.012)
 if name=='LockerV4':
  for i in range(9):m.box((0,-y*.5-.030,z-.15-i*.025),(x*.6,.01,.01),'Rubber')
  m.box((x*.3,-y*.5-.06,1),(.025,.055,.22),'Steel')
 elif name=='FridgeV4':
  m.box((0,-y*.5-.029,1.23),(x*.92,.015,.012),'Rubber')
  for zz in (.85,1.48):m.box((x*.32,-y*.5-.07,zz),(.035,.05,.24),'Steel')
  m.box((-.1,-y*.5-.032,.9),(.18,.012,.24),'Bone')
 elif name=='StoveV4':
  m.box((0,-y*.5-.045,.45),(.60,.03,.40),'Rubber');m.box((0,-y*.5-.08,.70),(.52,.06,.035),'Steel')
  for xx in (-.23,.23):
   for yy in (-.20,.20):m.tube((xx,yy,z),(xx,yy,z+.025),.12,.09,'Rubber',n=12)
  for xx in (-.3,-.1,.1,.3):m.cyl((xx,-y*.5-.045,.82),(xx,-y*.5-.075,.82),.028,'Bone')
 elif name=='RadioV4':
  m.box((-.25,-y*.5-.025,.87),(.48,.04,.30),'Rubber');m.box((-.25,-y*.5-.050,.88),(.40,.01,.21),'Glow')
  for i in range(7):m.box((.28,-y*.5-.031,.78+i*.028),(.34,.015,.014),'Rubber')
  for xx in (-.40,-.1,.2,.45):m.cyl((xx,-y*.5-.04,.57),(xx,-y*.5-.09,.57),.035,'Bone')
  m.cyl((.45,.10,z),(.45,.10,z+.85),.013,'Steel')
 elif name=='WorkbenchV4':
  m.box((0,0,z+.045),(x+.08,y+.08,.09),'Wood');m.box((-.45,0,z+.16),(.30,.21,.22),'Steel')
  m.box((-.45,-.02,z+.30),(.34,.055,.07),'Rubber')
  for i in range(6):m.box((.2+i*.08,0,z+.10),(.035,.35,.018),'Steel')
 else:
  m.box((0,0,z+.015),(x+.07,y+.06,.05),'Bone');m.box((0,-.04,z+.047),(.46,.35,.018),'Rubber')
  m.line([(0,.20,z),(0,.20,z+.30),(0,-.02,z+.30)],.023,'Steel')
 m.finish(name,collision='convex')
m.box((0,.15,.44),(.40,.28,.80),'Bone',.04);m.box((0,-.12,.26),(.48,.56,.43),'Bone',.06)
m.tube((0,-.18,.48),(0,-.18,.52),.22,.155,'Bone',n=12);m.box((0,.1,.86),(.45,.35,.05),'Bone')
m.finish('ToiletV4',collision='convex');m.export_all()
(m.OUT/'models_v4_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'LethalWorld_V4.blend'))
print('LW_V4_MODELS_COMPLETE',len(m.RECORDS))
