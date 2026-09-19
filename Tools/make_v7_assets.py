"""Original procedural surface and sound assets for the 0.7 editor update."""
from pathlib import Path
import numpy as np
from PIL import Image, ImageDraw
import wave
root=Path(__file__).resolve().parents[1]; out=root/'ArtSource'/'TexturesV7';out.mkdir(exist_ok=True)
rng=np.random.default_rng(7081);n=512;y,x=np.mgrid[:n,:n]
colors={'PlasterV7':(126,128,109),'BrickV7':(103,72,57),'WallpaperV7':(104,117,97),'ParquetV7':(113,83,54),'TileV7':(147,151,132),'CorrugatedV7':(105,113,109),'DoorWoodV7':(89,64,42),'DoorPaintV7':(82,101,95),'PosterV7':(171,149,103)}
for name,col in colors.items():
 noise=rng.normal(0,6,(n,n)); slow=12*np.sin(x*.016)*np.sin(y*.021)+8*np.cos((x+y)*.025)
 a=np.array(col)[None,None,:]+(noise+slow)[:,:,None]; mask=np.zeros((n,n),bool)
 if 'Brick' in name: mask=(y%64<5)|((x+(y//64%2)*64)%128<5)
 if 'Tile' in name: mask=(x%64<4)|(y%64<4)
 if 'Parquet' in name: mask=(y%64<4)|((x+(y//64%2)*128)%256<3);a+=np.sin(x*.3+np.sin(y*.04))[:,:,None]*9
 if 'Wallpaper' in name: a+=((np.cos(x*np.pi/32)*np.cos(y*np.pi/32))**8)[:,:,None]*28;mask=x%64<2
 if 'Corrugated' in name:a+=np.sin(x*np.pi/16)[:,:,None]*23;mask=y%256<4
 if 'DoorWood' in name:a+=np.sin(y*.4+np.sin(x*.025)*4)[:,:,None]*12;mask=(x%240<8)|(y%240<8)
 if 'DoorPaint' in name:mask=(x%240<6)|(y%240<6)
 a[mask]*=.48
 im=Image.fromarray(np.uint8(np.clip(a,0,255)));d=ImageDraw.Draw(im)
 for i in range(45):
  px,py=rng.integers(0,n,2);d.line((int(px),int(py),int(px+rng.integers(-12,12)),int(py+rng.integers(12,85))),fill=(65,61,47),width=1)
 if name=='PosterV7':
  d.rectangle((28,28,484,484),outline=(57,54,38),width=8);d.polygon([(256,70),(430,380),(80,380)],outline=(107,52,33),width=10);d.rectangle((241,150,271,283),fill=(89,57,34));d.ellipse((241,305,271,335),fill=(89,57,34))
 im.save(out/f'T_{name}.png')
 # Texture-derived tangent-space normal, kept deliberately subtle.
 h=np.asarray(im).mean(axis=2)/255;gy,gx=np.gradient(h);v=np.dstack((-gx*2,-gy*2,np.ones_like(h)));v/=np.linalg.norm(v,axis=2)[:,:,None];Image.fromarray(np.uint8((v*.5+.5)*255)).save(out/f'T_{name}_N.png')
audio=root/'ArtSource'/'Audio';audio.mkdir(exist_ok=True);rate=22050
for name,duration in [('LevelUp',1.7),('PoliceSiren',4.0)]:
 t=np.arange(int(duration*rate))/rate;v=np.zeros_like(t)
 if name=='LevelUp':
  for i,f in enumerate([329.63,415.30,493.88,659.25]):
   u=t-i*.18;env=np.where(u>=0,np.minimum(np.maximum(u,0)*80,1)*np.exp(-np.maximum(u,0)*4),0);v+=.2*env*(np.sin(2*np.pi*f*u)+.23*np.sin(2*np.pi*f*2*u))
 else:
  freq=650+310*np.sin(2*np.pi*t/4);phase=2*np.pi*np.cumsum(freq)/rate;v=.27*np.sin(phase)+.07*np.sin(phase*3)
 with wave.open(str(audio/f'S_{name}.wav'),'wb') as w:w.setnchannels(1);w.setsampwidth(2);w.setframerate(rate);w.writeframes(np.int16(np.clip(v,-.95,.95)*32767).tobytes())
print('V7 assets: 9 original surfaces + normals, level chime, siren')
