"""Idempotent production pass after campaign76's continuity pass."""
import json
from pathlib import Path
root=Path(__file__).resolve().parents[1]
files={p:json.loads(p.read_text(encoding='utf-8')) for p in (root/'Data/Campaign76').glob('*.json')}
st={s['id']:s for d in files.values() for s in d['stages']};sc={s['id']:s for d in files.values() for s in d['scenes']}
for sid,aid in [('resistance_exit','shelter_count'),('final_evacuation','evac_count')]:
 for a in st[sid]['actions']:
  if a['id']==aid and 'crowd_clear77' not in a['needs']:a['needs'].append('crowd_clear77')
st['resistance_exit']['goal']='Unlock the service gate, escort the families through the central passage, and count them at the sheltered exit.'
st['final_evacuation']['goal']='Keep the school and residential evacuation lane clear. Account for the surviving group before dispatching supplies.'
for a in st['crossing_ferry']['actions']:
 if a['id']=='cross_ferry':a['label']='Process weapons and board the Night Ferry'
for a in st['bell_attack']['actions']:
 if a['id']=='treat_lena':a['at']=[530,250,0]
for a in st['rome_repair']['actions']:
 a['at']={'crane_lock':[850,850,0],'pit_drain':[800,-1250,0],'freight_gate':[850,-400,0]}[a['id']]
for a in st['guard_convoy']['actions']:
 if a['id']=='convoy_manifest':a['at']=[1190,350,0]
# Handoffs operate actual cases and contact targets. Keep canon text and conditional speakers.
for sid in ['weller_tape','route_witness','border_arrival','world_records']:
 for b in sc[sid]['beats']:
  if b.get('who'):b['pause']=max(b.get('pause',0),.8)
st['carrier_battle']['cast']=list(dict.fromkeys(st['carrier_battle']['cast']+['richardson']))
for d in files.values():
 for stage in d['stages']:
  for a in stage['actions']:
   if a.get('mesh')=='MedicalCase52' and a['id'] not in ['canada_sleep']:a['mesh']='Case77'
for p,d in files.items():p.write_text(json.dumps(d,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
print('CAMPAIGN77_AUTHOR_PASS')
