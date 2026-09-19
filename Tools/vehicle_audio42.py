"""Offline ElevenLabs vehicle library. No keys or API calls ship in the game.
--generate spends credits; reruns skip completed entries, never retry ambiguous requests.
"""
import argparse,io,json,os,sys,urllib.request,urllib.error,winreg
from pathlib import Path
import numpy as np
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'Saved/Audio42Deps'))
OUT=ROOT/'ArtSource/AudioV42';OUT.mkdir(parents=True,exist_ok=True)
ENGINES={'sedan':'worn 1990s naturally aspirated inline four sedan, soft uneven gasoline purr','police':'large police cruiser 4.6 litre V8, smooth authoritative burble','boxtruck':'commercial box truck inline six turbodiesel, clattering heavy compression','rv':'luxury rear engine motorcoach diesel pusher, deep insulated low rumble','bus':'old school bus diesel, metallic rattles and heavy exhaust pulses','van':'cargo van gasoline V6, hollow resonant exhaust','pickup':'pickup truck large displacement V8, throaty workhorse rumble','dirtbike':'single cylinder four stroke motocross bike, sharp thudding exhaust','suv':'large SUV V6, refined low torque growl','muscle':'classic muscle car crossplane big block V8, loping cam and thunderous exhaust','supercar':'mid engine twin turbo flat plane V8 supercar, exotic high frequency mechanical howl'}
ROWS=[]
def add(slot,prompt,duration=1.5,loop=False):ROWS.append(dict(slot=slot,prompt=prompt,duration=duration,loop=loop,file=slot+'.wav'))
for car,desc in ENGINES.items():
 for state,rpm in [('Idle','steady warm idle, 800 RPM'),('Low','steady loaded engine at moderate RPM, gentle acceleration torque, constant speed'),('High','steady engine at high RPM under full throttle load, constant speed')]:
  add('Vehicle42_'+car+'_'+state,desc+'. '+rpm+'. Isolated dry close engine recording for a racing game layered engine loop. Constant continuous engine tone, no gear changes, no rev sweep, no driving past, no start or stop, no music or voices.',4,True)
for slot,prompt in {
 'CarDoorOpen':'car door handle latch click then heavy door hinge opening',
 'CarDoorClose':'single weighty car door closing with a solid low thud and latch',
 'CarGloveOpen':'plastic glove compartment latch then damped hinged lid opening',
 'CarGloveClose':'glove compartment gently clicks shut',
 'CarSwitch':'single tactile automotive dashboard rocker switch click',
 'CarSignal':'single mechanical automotive turn signal relay tick tock',
 'CarGearShift':'automatic car transmission shifts gear, subtle mechanical clunk',
 'CarEngineStop':'car engine shutting down, short soft descending mechanical shudder',
 'CarAirBrake':'heavy vehicle air brakes release with a short sharp compressed air hiss',
 'CarHorn':'single ordinary dual tone car horn blast',
 'CarCargoOpen':'heavy vehicle cargo latch releases and hinged metal hatch creaks open',
 'CarCargoClose':'heavy metal cargo hatch thumps shut and latches',
 'CarSeat':'leather vehicle seat compresses and creaks as someone sits',
 'CarHandbrake':'car handbrake lever ratchets and clicks into place',
 'CarRadioSwitch':'old automotive radio clicks on with a brief tuning static burst',
 'CarFuelCap':'fuel filler cap unscrews with plastic ratchet clicks',
 'CarImpact':'vehicle metal panel denting with a heavy collision crunch, no explosion',
 'CarGlassBreak':'automotive safety glass shatters and small fragments scatter'
}.items():add(slot,prompt+'. Isolated dry Foley, one action, no voices or music.')
for slot,prompt in {
 'CarTires':'continuous tires rolling on dry asphalt, soft road friction hiss',
 'CarBrake':'continuous restrained disc brake friction under moderate braking',
 'CarSkid':'continuous tire skid squeal on asphalt under hard braking',
 'CarWipers':'rhythmic rubber windshield wipers sweeping wet glass with quiet motor',
 'CarWind42':'continuous muffled wind around moving car body, no voices',
 'PoliceSiren':'American police emergency siren alternating wail, isolated'
}.items():add(slot,prompt+'. Seamless continuous isolated game sound loop.',3,True)
for family,desc in [('Petrol','gasoline sedan'),('Diesel','heavy turbodiesel truck'),('Sport','high performance V8 sports car'),('Bike','single cylinder dirt bike')]:add('CarStart42'+family,desc+' ignition starter cranks and engine catches, dry isolated recording, no speech.',1.5)
(OUT/'manifest.json').write_text(json.dumps({'estimated_credits':sum(r['duration']*40 for r in ROWS),'sounds':ROWS},indent=2))

def key():
 for root,path in [(winreg.HKEY_CURRENT_USER,'Environment'),(winreg.HKEY_LOCAL_MACHINE,r'SYSTEM\CurrentControlSet\Control\Session Manager\Environment')]:
  try:
   with winreg.OpenKey(root,path) as r:
    k=str(winreg.QueryValueEx(r,'ELEVENLABS_API_KEY')[0]).strip()
   if k:return k
  except FileNotFoundError:pass
 k=os.environ.get('ELEVENLABS_API_KEY','').strip()
 if not k:raise RuntimeError('ELEVENLABS_API_KEY unavailable')
 return k

def generate(limit):
 import soundfile as sf
 k=key();ledger=OUT/'usage.json';usage=json.loads(ledger.read_text()) if ledger.exists() else {};spent=sum(v.get('cost',0) for v in usage.values())
 for row in ROWS:
  target=OUT/row['file'];slot=row['slot'];estimate=row['duration']*40
  if target.exists():continue
  if slot in usage:raise RuntimeError('Unfinished billed request; review usage.json before resuming '+slot)
  if spent+estimate>limit:raise RuntimeError('Generation budget reached')
  req=urllib.request.Request('https://api.elevenlabs.io/v1/sound-generation?output_format=mp3_44100_128',data=json.dumps({'text':row['prompt'],'duration_seconds':row['duration'],'loop':row['loop'],'model_id':'eleven_text_to_sound_v2','prompt_influence':.65}).encode(),headers={'xi-api-key':k,'Content-Type':'application/json'})
  usage[slot]={'cost':estimate,'state':'pending'};ledger.write_text(json.dumps(usage,indent=2))
  try:
   with urllib.request.urlopen(req,timeout=100) as r:raw=r.read();cost=float(r.headers.get('character-cost') or estimate)
  except urllib.error.HTTPError as e:
   print('Generation stopped HTTP',e.code,'slot',slot,flush=True);raise SystemExit(1)
  (OUT/(slot+'.mp3')).write_bytes(raw);usage[slot]={'cost':cost,'state':'received'};ledger.write_text(json.dumps(usage,indent=2));spent+=cost
  samples,rate=sf.read(io.BytesIO(raw),dtype='float32',always_2d=True);samples=samples.mean(axis=1);samples-=samples.mean()
  if row['loop']:
   n=min(int(rate*.10),len(samples)//8);t=np.linspace(0,1,n);blend=samples[-n:]*(1-t)+samples[:n]*t;samples=np.concatenate([samples[n:-n],blend])
  else:
   n=min(int(rate*.008),len(samples)//8);samples[:n]*=np.linspace(0,1,n);samples[-n:]*=np.linspace(1,0,n)
  rms=float(np.sqrt(np.mean(samples*samples)));gain=min(.19/max(rms,1e-6),.88/max(float(np.max(np.abs(samples))),1e-6));samples*=gain
  sf.write(target,samples,rate,subtype='PCM_16');usage[slot]['state']='complete';usage[slot]['rms']=float(np.sqrt(np.mean(samples*samples)));usage[slot]['seconds']=len(samples)/rate;ledger.write_text(json.dumps(usage,indent=2))
  print('GENERATED',slot,'credits',cost,'batch_total',spent,flush=True)
 print('VEHICLE42_AUDIO_DONE',len(ROWS),'credits',spent,flush=True)
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--generate',action='store_true');p.add_argument('--budget',type=int,default=7800);a=p.parse_args()
 print('Sounds',len(ROWS),'estimated credits',sum(r['duration']*40 for r in ROWS),flush=True)
 if a.generate:generate(a.budget)
