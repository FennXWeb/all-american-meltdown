"""Distinct field packs and binocular night vision, with buckles, straps and lenses."""
import sys,json
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent))
import make_models_v2 as m
import bpy
m.OUT=m.ROOT/'ArtSource/ModelsV25';m.setup()
for k,name in enumerate(['SlingPack25','Backpack25','HikingPack25','MilitaryPack25','ExpeditionPack25']):
 w=.25+k*.035;h=.32+k*.075;d=.13+k*.025
 m.box((0,0,h/2),(w,d,h),'Cloth',.025)
 m.box((0,-d*.5-.025,h*.33),(w*.82,.065,h*.36),'Cloth',.016)
 m.box((0,0,h*.95),(w*1.04,d*1.05,.065),'Rubber',.018)
 for side in [-1,1]:
  x=side*w*.30
  m.box((x,d*.6,h*.5),(.045,.026,h*.82),'Rubber',.01)
  m.box((x,-d*.5-.065,h*.48),(.035,.018,.038),'Steel',.003)
  m.box((x,-d*.5-.06,h*.65),(.022,.013,h*.48),'Rubber',.003)
  if k>=2:m.box((side*(w*.5+.033),0,h*.33),(.09,d*.8,h*.38),'Cloth',.015)
 if k>=3:
  for z in [.15,.20,.25]:m.box((0,-d*.5-.063,z),(w*.85,.012,.018),'Rubber',.002)
 if k==4:
  m.cyl((-.22,0,.035),(.22,0,.035),.065,'Cloth',n=16)
  for x in [-.12,.12]:m.box((x,-.06,.035),(.024,.018,.14),'Rubber',.003)
 m.box((0,0,h+.035),(w*.4,.032,.022),'Rubber',.008)
 m.finish(name,collision='convex')
for y in [-.041,.041]:
 m.cyl((-.025,y,.035),(.13,y,.035),.033,'Rubber',n=16)
 m.cyl((.115,y,.035),(.145,y,.035),.036,'Steel',n=16)
 m.cyl((.145,y,.035),(.147,y,.035),.027,'Glass',n=16)
 for x in [.0,.025,.055]:m.cyl((x,y,.035),(x+.01,y,.035),.035,'Rubber',n=16)
m.box((.045,0,.065),(.07,.12,.025),'Steel',.008)
m.box((.025,0,.11),(.045,.03,.075),'Steel',.006)
m.box((-.035,0,.125),(.10,.07,.03),'Rubber',.012)
m.finish('NightVision25',collision='convex')
m.export_all();(m.OUT/'models_v25_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2))
bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'FieldGear25.blend'))
