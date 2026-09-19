import bpy,sys,json
from pathlib import Path
root=Path(__file__).resolve().parents[1];sys.path.insert(0,str(root/'Tools'));import make_models_v2 as m
bpy.ops.wm.open_mainfile(filepath=str(root/'ArtSource/ModelsV42/VehicleOverhaul42.blend'))
m.OUT=root/'ArtSource/ModelsV49';m.OUT.mkdir(exist_ok=True);m.OBJECTS.clear();m.RECORDS.clear()
rows=json.loads((root/'ArtSource/ModelsV42/models_v42_manifest.json').read_text())['assets']
for model,roof in [('sedan',1.57),('muscle',1.50),('supercar',1.37)]:
 name='Cabin42_'+model;o=bpy.data.objects['SM_'+name];count=0
 for v in o.data.vertices:
  x,y,z=v.co
  if -.04<x<.18 and .18<abs(y)<.70 and roof-.14<z<roof-.085:
   v.co.z+=.08;count+=1
 assert count>20,(name,count)
 o.data.update();row=next(r for r in rows if r['name']==name)
 lo=[min(v.co[i] for v in o.data.vertices) for i in range(3)];hi=[max(v.co[i] for v in o.data.vertices) for i in range(3)]
 row['local_bounds_m']={'min':lo,'max':hi,'size':[hi[i]-lo[i] for i in range(3)]};m.RECORDS[name]=row;m.OBJECTS[name]=o
 print('STOWED_VISOR49',name,count,flush=True)
m.export_all();(m.OUT/'models_v49_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2))
