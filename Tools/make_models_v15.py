"""Original low-poly survivor forms and irregular cinematic cloud volumes."""
import sys,json,math,random
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent));import make_models_v2 as m
import bpy
from mathutils import noise
m.OUT=m.ROOT/'ArtSource/ModelsV15';m.setup()
m.loft([(0,0,-.5,.24,.30),(.035,0,-.28,.41,.41),(.03,0,0,.5,.5),(0,0,.28,.46,.46),(-.04,0,.5,.28,.30)],'Skin',n=12);m.finish('SurvivorHeadV15')
m.loft([(0,0,-.5,.38,.34),(0,0,-.15,.43,.4),(0,0,.30,.49,.5),(0,0,.45,.39,.47),(0,0,.5,.26,.24)],'Cloth',n=8);m.finish('SurvivorTorsoV15')
m.box((0,0,0),(1,1,1),'Cloth',.13);m.finish('SurvivorLimbV15')
bpy.ops.mesh.primitive_ico_sphere_add(subdivisions=3,radius=.5);o=bpy.context.object
for v in o.data.vertices:v.co*=1+.30*noise.noise(v.co*12)+.13*noise.noise(v.co*31)
m.add(o,'Concrete');m.finish('IntroCloudV15')
m.loft([(0,0,-.5,.42,.36),(.08,.03,-.2,.5,.43),(-.04,0,.05,.32,.32),(.12,.08,.3,.19,.12),(.17,.1,.5,.01,.01)],'Glow',n=7);m.finish('IntroFlameV15')
m.export_all()
(m.OUT/'models_v15_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2))
bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'AllAmericanMeltdown_V15.blend'))
print('AAM_V15_MODELS_COMPLETE',len(m.RECORDS))
