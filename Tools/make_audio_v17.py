"""Deterministic original placeholder vehicle layers and punch/door effects."""
from pathlib import Path
import numpy as np, wave
out=Path(__file__).resolve().parents[1]/'ArtSource/Audio';out.mkdir(exist_ok=True)
sr=22050;rng=np.random.default_rng(1702030)
def save(name,x):
 x=np.clip(x,-.92,.92)
 with wave.open(str(out/('S_'+name+'.wav')),'wb') as w:
  w.setnchannels(1);w.setsampwidth(2);w.setframerate(sr);w.writeframes((x*32767).astype('<i2').tobytes())
t=np.arange(sr*4)/sr
for name,f in [('CarLoadDiesel',42),('CarLoadPetrol',86),('CarLoadSport',126),('CarLoadBike',168),('CarOverrun',62)]:
 x=sum(np.sin(2*np.pi*f*k*t)*(.25/k**1.3) for k in range(1,9));x*=.8+.2*np.sin(2*np.pi*12*t);save(name,x)
for name,f in [('CarTires',240),('CarBrake',900),('CarSkid',1550)]:
 # Periodic random Fourier noise, avoiding a discontinuity at loop boundaries.
 x=np.zeros(len(t))
 for k in range(1,65):x+=np.sin(2*np.pi*(f+k*13)*t+rng.uniform(0,6.28))*.016
 if name!='CarTires':x+=.16*np.sin(2*np.pi*f*t)*(.75+.25*np.sin(2*np.pi*4*t))
 save(name,x)
for name,d,f in [('PunchSwing',.28,90),('PunchHit',.22,65),('CarDoor',.55,135),('CarGearShift',.23,180),('CarEngineStop',.75,48),('CarAirBrake',.8,2000)]:
 t=np.arange(int(sr*d))/sr;n=rng.uniform(-1,1,len(t));env=np.sin(np.minimum(t/.02,1)*np.pi/2)*np.exp(-t*6/d)
 x=(n*(.28 if name in ['CarAirBrake','PunchSwing'] else .12)+.25*np.sin(2*np.pi*f*t))*env
 save(name,x)
print('AAM_V17_AUDIO_COMPLETE 14')
