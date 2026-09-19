from pathlib import Path
import numpy as np,wave
out=Path(__file__).resolve().parents[1]/'ArtSource/Audio';sr=22050;rng=np.random.default_rng(1802030)
for name,d,f in [('DoubleBarrelFire',1.2,65),('DoubleBarrelOpen',.55,230),('DoubleBarrelEject',.38,620),('DoubleBarrelInsert',.3,410),('DoubleBarrelClose',.45,170),('MooseRoar',2.4,80),('TitanRoar',2.8,43),('DeathclawRoar',2.0,110),('ScorpionHiss',1.3,1500),('KarenShriek',1.8,370)]:
 t=np.arange(int(sr*d))/sr;n=rng.uniform(-1,1,len(t));roar='Roar' in name or 'Shriek' in name
 env=np.minimum(t/.025,1)*np.exp(-t*(1.5 if roar else 7)/d)
 x=(n*(.25 if name=='DoubleBarrelFire' or 'Hiss' in name else .07)+sum(np.sin(2*np.pi*(f*k*t+f*.006*np.sin(t*14)))*(.25/k) for k in range(1,6)))*env
 if name=='DoubleBarrelFire':x+=n*.4*np.exp(-t*50)
 with wave.open(str(out/('S_'+name+'.wav')),'wb') as w:w.setnchannels(1);w.setsampwidth(2);w.setframerate(sr);w.writeframes((np.clip(x,-.95,.95)*32767).astype('<i2').tobytes())
print('AAM_V18_AUDIO_COMPLETE 10')
