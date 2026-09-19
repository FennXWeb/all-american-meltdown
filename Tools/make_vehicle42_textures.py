from pathlib import Path
from PIL import Image,ImageDraw,ImageFont
import numpy as np,math
ROOT=Path(__file__).resolve().parents[1];OUT=ROOT/'ArtSource/TexturesV42';OUT.mkdir(parents=True,exist_ok=True)
rng=np.random.default_rng(42042)
colors={'Paint_sedan':(94,116,112),'Paint_police':(204,207,202),'Paint_boxtruck':(193,181,153),'Paint_rv':(198,190,170),'Paint_bus':(215,150,30),'Paint_van':(139,149,157),'Paint_pickup':(119,58,44),'Paint_dirtbike':(168,40,30),'Paint_suv':(65,80,63),'Paint_muscle':(91,121,146),'Paint_supercar':(228,100,23),'Leather':(43,40,35),'Vinyl':(28,32,35),'Fabric':(64,71,68),'Metal':(121,127,132),'Rubber':(22,24,24),'Chrome':(185,189,187),'Wood':(80,50,30),'Carpet':(39,39,36),'Lamp':(226,218,187),'RedLamp':(158,17,12),'Amber':(192,102,17)}
for name,c in colors.items():
 n=1024 if name.startswith('Paint') else 512;y,x=np.mgrid[:n,:n];noise=rng.normal(0,2 if name.startswith('Paint') else 4,(n,n))
 if name=='Fabric':noise+=((x%6<2)^(y%6<2))*8
 if name=='Leather':noise+=np.sin(x*.9+np.sin(y*.7))*2
 if name=='Metal':noise+=np.sin(y*1.7)*4
 if name=='Wood':noise+=np.sin(x*.11+np.sin(y*.025)*2)*12
 a=np.clip(np.array(c)[None,None,:]+noise[:,:,None],0,255).astype('uint8');im=Image.fromarray(a);d=ImageDraw.Draw(im)
 if name.startswith('Paint'):
  for i in range(95):
   px,py=map(int,rng.integers(0,n,2));l=int(rng.integers(2,20));d.line((px,py,px+l,py+int(rng.integers(-2,3))),fill=tuple(max(0,v-28) for v in c),width=1)
 im.save(OUT/('T_V42_'+name+'.png'))
fontpath='C:/Windows/Fonts/arialbd.ttf'
for name,labels in [('Speed',[str(i) for i in range(0,161,20)]),('RPM',[str(i) for i in range(0,11)]),('Fuel',['E','1/2','F']),('Temp',['C','90','H'])]:
 im=Image.new('RGB',(512,512),(11,17,17));d=ImageDraw.Draw(im);f=ImageFont.truetype(fontpath,29);small=ImageFont.truetype(fontpath,21)
 d.ellipse((8,8,503,503),outline=(117,125,120),width=9);d.ellipse((27,27,484,484),outline=(48,56,54),width=4)
 ticks=(len(labels)-1)*5
 for i in range(ticks+1):
  a=math.radians(-130+260*i/ticks);r=213;rr=188 if i%5==0 else 200
  d.line((256+math.sin(a)*r,256-math.cos(a)*r,256+math.sin(a)*rr,256-math.cos(a)*rr),fill=(190,210,182) if i<ticks*.85 else (195,90,47),width=4 if i%5==0 else 2)
 for i,t in enumerate(labels):
  a=math.radians(-130+260*i/(len(labels)-1));d.text((256+math.sin(a)*160,256-math.cos(a)*160),t,font=f,anchor='mm',fill=(206,222,201))
 d.text((256,334),{'Speed':'MPH','RPM':'RPM x1000','Fuel':'FUEL','Temp':'COOLANT'}[name],font=small,anchor='mm',fill=(149,175,148));im.save(OUT/('T_V42_Dial'+name+'.png'))
print('VEHICLE42_TEXTURES_DONE')
