"""Thirty deterministic authored finish patterns and normalized mono weapon SFX."""
from pathlib import Path
import numpy as np
from PIL import Image,ImageDraw
import wave,json
root=Path(__file__).resolve().parents[1];out=root/'ArtSource/TexturesV24';out.mkdir(parents=True,exist_ok=True)
n=512;y,x=np.mgrid[:n,:n]/n;rng=np.random.default_rng(2401)
palettes=[[(.15,.32,.48),(.75,.83,.80)],[(.3,.07,.43),(.88,.49,.88)],[(.10,.10,.12),(.95,.68,.19)]]
for tier in range(2,5):
 for k in range(10):
  if k==0:mask=((np.floor(x*11)+np.floor(y*13)+np.floor((x+y)*7))%3)/2
  elif k==1:mask=((np.mod(x*16,1)<.10)|(np.mod(y*16,1)<.10)).astype(float)
  elif k==2:mask=(np.sin(x*60+np.sin(y*15)*3)>.4).astype(float)
  elif k==3:mask=(np.mod(np.sin(x*12)+np.cos(y*14)+np.sin((x+y)*20)*.3,.2)<.025).astype(float)
  elif k==4:mask=(np.mod(x*8+y*8,1)>.5).astype(float)
  elif k==5:mask=(np.cos(x*70)*np.cos(y*40)+np.cos(y*80)>.45).astype(float)
  elif k==6:mask=np.power((np.sin(x*18+y*5+np.sin(y*24)*2)+1)/2,12)
  elif k==7:mask=((np.mod(x*9,1)-.5)**2+(np.mod(y*9,1)-.5)**2<.025).astype(float)
  elif k==8:mask=(np.sin(x*42+np.sin(y*25)*3)*np.cos(y*30)>.6).astype(float)
  else:mask=(np.mod(y*18+np.sin(x*12)*.2,1)<.25).astype(float)
  a,b=map(np.array,palettes[tier-2]);b=np.roll(b,k%3);c=a[None,None,:]*(1-mask[:,:,None])+b[None,None,:]*mask[:,:,None]
  noise=rng.normal(0,.018,(n,n,1));scratches=(rng.random((n,n,1))>.996);c=np.clip(c+noise+scratches*.15,0,1)
  Image.fromarray((c*255).astype('uint8')).save(out/f'T_WeaponSkin24_{tier}_{k:02d}.png')
# Each report has a distinct attack spectrum and decay; peak and RMS limits avoid erratic gain.
audio=root/'ArtSource/AudioV24';audio.mkdir(exist_ok=True)
for name,freq,dur in [('MissileFire24',60,.9),('MinigunFire24',125,.11),('SawedOffFire24',85,.48),('DeagleFire24',170,.4),('M4Fire24',240,.24),('TaserFire24',1500,.5),('FlameFire24',110,.20)]:
 t=np.arange(int(44100*dur))/44100;noise=rng.normal(0,1,len(t));low=np.convolve(noise,np.ones(9)/9,'same')
 if name=='TaserFire24':v=(np.sin(t*freq*2*np.pi)*.35+noise*.18)*(np.sin(t*65*2*np.pi)>.05)*np.exp(-t*5)
 elif name=='FlameFire24':v=(low*.8+np.sin(t*freq*2*np.pi)*.1)*np.sin(np.pi*t/dur)**.4
 else:v=(noise*.22*np.exp(-t*65)+low*.5+np.sin(2*np.pi*(freq*t-20*t*t))*.4)*np.exp(-t*6/dur)
 v=np.tanh(v*2);v*=min(.9/max(.001,np.max(np.abs(v))),.19/max(.001,np.sqrt(np.mean(v*v))))
 with wave.open(str(audio/(name+'.wav')),'wb') as w:w.setparams((1,2,44100,0,'NONE','not compressed'));w.writeframes((v*32767).astype('<i2').tobytes())
print('ARSENAL24_TEXTURES_AUDIO_COMPLETE')
