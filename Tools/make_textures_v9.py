from pathlib import Path
import numpy as np
from PIL import Image,ImageDraw,ImageFilter,ImageFont
r=Path(__file__).resolve().parents[1];out=r/'ArtSource/TexturesV9';out.mkdir(exist_ok=True)
rng=np.random.default_rng(909);y,x=np.mgrid[:512,:512];noise=rng.normal(0,1,(512,512))
stone=np.stack([190+noise*4,185+noise*4,169+noise*4],axis=-1)
vein=np.abs(np.sin(x*.025+np.sin(y*.013)*3+noise*.08))<.05;stone[vein]*=.53;stone[(x%128<2)|(y%128<2)]*=.6
wood=np.stack([48+np.sin(x*.24+y*.008)*4+noise*2,35+noise*2,28+noise*2],axis=-1);wood[x%85<3]=[100,79,38]
velvet=np.stack([60+noise*3,16+noise,25+noise*2],axis=-1);velvet[(x+y)%3==0]*=.85
paint=np.stack([200+noise*4]*3,axis=-1);paint[rng.random((512,512))<.003]=[70,72,66]
for name,a in [('BoutiqueStoneV9',stone),('BoutiqueWallV9',wood),('VelvetV9',velvet),('VehiclePaintV9',paint)]:
 a=np.clip(a,0,255).astype('uint8');Image.fromarray(a).save(out/f'T_{name}.png')
 h=a.mean(axis=2)/255;gy,gx=np.gradient(h);n=np.stack([-gx*1.5,-gy*1.5,np.ones_like(h)],axis=-1);n/=np.linalg.norm(n,axis=2,keepdims=True);Image.fromarray(np.uint8((n*.5+.5)*255)).save(out/f'T_{name}_N.png')
drops=Image.new('L',(512,512));d=ImageDraw.Draw(drops)
for _ in range(550):
 xx,yy=rng.integers(0,512,2);w=int(rng.integers(2,9));h=int(rng.integers(4,35));d.ellipse((xx,yy,xx+w,yy+h),fill=int(rng.integers(80,255)))
drops.filter(ImageFilter.GaussianBlur(.7)).convert('RGB').save(out/'T_DropsV9.png')
mask=Image.new('L',(512,512));d=ImageDraw.Draw(mask)
for _ in range(25):
 xx,yy=rng.integers(120,390,2);rad=int(rng.integers(35,100));d.ellipse((xx-rad,yy-rad*.7,xx+rad,yy+rad*.7),fill=220)
mask.filter(ImageFilter.GaussianBlur(14)).convert('RGB').save(out/'T_PuddleV9.png')
sign=Image.new('RGB',(1024,256),(20,18,16));d=ImageDraw.Draw(sign);font=ImageFont.truetype('C:/Windows/Fonts/timesbd.ttf',82);small=ImageFont.truetype('C:/Windows/Fonts/times.ttf',31)
d.rectangle((12,12,1011,243),outline=(162,134,72),width=3);d.text((512,75),'V A N T A',font=font,fill=(196,174,112),anchor='mm');d.text((512,164),'A T E L I E R   /   E S T .  1 9 6 4',font=small,fill=(166,146,95),anchor='mm');sign.save(out/'T_BoutiqueSignV9.png')
print('V9 textures generated')
