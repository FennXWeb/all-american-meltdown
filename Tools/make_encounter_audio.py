"""Original short encounter cues. Seeded synthesis; no external recordings."""
from pathlib import Path
import numpy as np,wave
root=Path(__file__).resolve().parents[1];out=root/'ArtSource/Audio';out.mkdir(exist_ok=True)
rate=22050;rng=np.random.default_rng(101056)
for name,length in [('RadioStatic',2.8),('EncounterWarning',1.4),('EncounterResolved',1.1)]:
 t=np.arange(int(rate*length))/rate;n=rng.normal(0,.045,len(t));v=n.copy()
 if name=='RadioStatic':
  for start,freq,dur in [(.25,690,.14),(.5,890,.12),(.9,590,.22),(1.4,810,.15),(1.85,460,.3),(2.3,930,.1)]:
   env=np.maximum(0,np.minimum((t-start)*80,(start+dur-t)*80));env=np.minimum(1,env);v+=env*.15*np.sin(t*freq*2*np.pi)
  v*=.6+.4*np.sin(t*11)**2
 elif name=='EncounterWarning':v=.12*np.sin(2*np.pi*t*(380+120*np.sin(t*7)))*np.exp(-t*1.6)+n*.3
 else:
  v=np.zeros(len(t))
  for start,freq in [(0,330),(.15,440),(.3,550)]:v+=.1*np.sin(t*freq*2*np.pi)*np.exp(-np.maximum(0,t-start)*6)*(t>=start)
 v*=np.minimum(1,t*50)*np.minimum(1,(length-t)*50);v=np.clip(v,-.8,.8)
 with wave.open(str(out/f'S_{name}.wav'),'wb') as f:f.setnchannels(1);f.setsampwidth(2);f.setframerate(rate);f.writeframes((v*32767).astype('<i2').tobytes())
print('Encounter audio generated')
