"""Analytical micro-surface PBR maps. Independent authored materials, not inferred geometry from color."""
from PIL import Image
import numpy as np
from pathlib import Path
p=Path(__file__).resolve().parents[1]/'ArtSource/Campaign77';n=1024;s=n//4;rng=np.random.default_rng(77)
normal=np.zeros((n,n,3),dtype=np.uint8);orm=np.zeros_like(normal)
rough=[.58,.91,.38,.53,.34,.93,.53,.32,.68,.92,.55,.7,.9,.59,.78,.98]
metal=[.18,0,.9,0,0,0,0,.9,.06,0,.85,.7,0,0,0,.65]
for i in range(16):
 y,x=np.mgrid[:s,:s];noise=rng.normal(0,.18,(s,s));h=noise*.08
 if i in [4,14]:h+=np.sin(y*.19+np.sin(x*.023)*2)*.11
 if i in [5,9]:h+=np.sin(x*1.6)*np.cos(y*1.6)*.17
 if i==1:h+=np.sin(y*.24)*.12
 dy,dx=np.gradient(h);v=np.stack([-dx,-dy,np.ones_like(h)],axis=2);v/=np.linalg.norm(v,axis=2)[:,:,None]
 row,col=divmod(i,4);normal[row*s:(row+1)*s,col*s:(col+1)*s]=np.clip((v*.5+.5)*255,0,255)
 tile=np.stack([np.clip(.98-abs(h)*.15,.8,1),np.clip(rough[i]+noise*.045,0,1),np.full_like(h,metal[i])],axis=2)
 orm[row*s:(row+1)*s,col*s:(col+1)*s]=np.uint8(tile*255)
Image.fromarray(normal).save(p/'T_SurfaceNormal77.png');Image.fromarray(orm).save(p/'T_SurfaceORM77.png')
print('CAMPAIGN77_SURFACES_PASS')
