from pathlib import Path
import numpy as np,wave
out=Path('ArtSource/Audio');out.mkdir(exist_ok=True);rate=22050
rng=np.random.default_rng(5086)
def save(n,x):
 x=np.clip(x,-.95,.95);p=out/f'S_{n}.wav'
 with wave.open(str(p),'wb') as w:w.setnchannels(1);w.setsampwidth(2);w.setframerate(rate);w.writeframes((x*32767).astype('<i2').tobytes())
t=np.arange(rate*2)/rate
x=sum(np.sin(2*np.pi*f*t)*a for f,a in [(35,.2),(70,.13),(140,.06),(210,.03)]);x*=.7+.3*np.sin(2*np.pi*14*t);save('CarEngine',x)
t=np.arange(rate*16)/rate;x=np.zeros_like(t)
for i,f in enumerate([164.81,196,220,146.83,164.81,130.81,146.83,123.47]):
 s=(t>=i*2)&(t<(i+1)*2);u=t[s]-i*2;x[s]=(.18*np.sin(2*np.pi*f*u)+.075*np.sin(2*np.pi*f*1.5*u))*np.minimum(u*8,1)*np.exp(-u*1.1)
x+=.015*np.sin(2*np.pi*55*t);save('CarRadio',x)
for name,f,d in [('CarIgnition',65,1.3),('CarImpact',40,.6),('CarSignal',1300,.09),('PickBreak',2400,.22),('LockOpen',800,.28),('WireSpark',1800,.38)]:
 t=np.arange(int(rate*d))/rate;n=rng.uniform(-1,1,len(t));env=np.exp(-t*(5/d));x=(.19*np.sin(2*np.pi*f*t)+.20*n)*env
 if name=='CarIgnition':x=(.16*np.sin(2*np.pi*(f*t+40*t*t))+.07*n)*(np.sin(t*2*np.pi*11)>.0)*np.minimum(t*10,1)*np.minimum((d-t)*10,1)
 save(name,x)
print('LW_V5_AUDIO_COMPLETE 8')
