"""Author deterministic, tileable PS1 surface maps and original synthesized sound effects.
Run with Python 3 + Pillow + numpy. No downloaded or third-party game assets.
"""
from pathlib import Path
import math, wave
import numpy as np
from PIL import Image, ImageDraw, ImageFilter

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'ArtSource'
(OUT / 'Textures').mkdir(parents=True, exist_ok=True)
(OUT / 'Audio').mkdir(parents=True, exist_ok=True)
rng = np.random.default_rng(198706)
N = 256
y, x = np.mgrid[:N, :N]

def noise(size, strength=1):
    a = (rng.random((size, size)) * 255).astype('uint8')
    return np.asarray(Image.fromarray(a).resize((N, N), Image.Resampling.BILINEAR)).astype(float) / 255 * strength

def surface(name, color, kind):
    coarse = noise(8) * .48 + noise(32) * .25 + noise(128) * .17 + noise(256) * .1
    base = np.array(color)[None,None,:] * (.56 + coarse[:,:,None] * .75)
    height = coarse.copy()
    if kind == 'brick':
        mortar = ((y % 32 < 3) | (((x + (y//32 % 2)*32) % 64) < 3))
        base[mortar] = (32, 34, 31)
        height[mortar] *= .3
        base *= (1 - noise(5)[:,:,None] * .25)
    elif kind == 'wood':
        grain = np.sin(x*.7 + noise(16)*6) * .10 + np.sin(x*2.7+y*.02)*.05
        base *= (1+grain[:,:,None])
        lines = (x%64<3)
        base[lines] *= .23
        height += grain
    elif kind == 'metal':
        rust = noise(12) + noise(40)*.35
        mask = rust > .68
        base[mask] = np.stack([rust[mask]*118, rust[mask]*58, rust[mask]*25], axis=-1)
        base *= (1+(x%32<2)[:,:,None]*.27)
        for q in (8, 120, 248):
            for p in (8, 120, 248):
                bolt = ((x-q)**2 + (y-p)**2)<9
                base[bolt] = (41,39,30)
    elif kind == 'asphalt':
        base *= (1 - (noise(90)>.66)[:,:,None]*.35)
    elif kind == 'cloth':
        base *= (1 + ((x%3==0) | (y%3==0))[:,:,None]*.11)
        base[noise(7)>.7] *= .42
    elif kind == 'skin':
        veins = (np.sin(x*.075+np.sin(y*.035)*3)>.98) & (noise(5)>.4)
        base[veins] = (62,45,43)
        base[noise(14)>.72] = (70,30,25)
    elif kind == 'concrete':
        cracks = np.abs(np.sin(x*.026 + noise(10)*3 + np.cos(y*.028))) < .032
        base[cracks] *= .38
    img = Image.fromarray(np.clip(base,0,255).astype('uint8'))
    img.save(OUT/'Textures'/f'T_{name}.png')
    gy,gx = np.gradient(height)
    norm = np.dstack([-gx*2,-gy*2,np.ones_like(gx)])
    norm /= np.linalg.norm(norm,axis=2,keepdims=True)
    Image.fromarray(((norm*.5+.5)*255).astype('uint8')).save(OUT/'Textures'/f'T_{name}_N.png')

for args in [('Concrete',(122,119,101),'concrete'), ('Brick',(116,81,60),'brick'),
             ('Rust',(127,138,124),'metal'),('Wood',(105,79,49),'wood'),
             ('Asphalt',(45,47,42),'asphalt'),('Earth',(74,77,48),'earth'),
             ('Cloth',(78,87,70),'cloth'),('Skin',(128,132,104),'skin'),
             ('Rubber',(29,32,28),'cloth'),('Red',(143,51,28),'metal'),
             ('Steel',(72,82,81),'metal'),('Bone',(167,160,125),'concrete')]:
    surface(*args)

# Worn signage with readable hierarchy and original fictional branding.
fontroot = Path('C:/Windows/Fonts')
from PIL import ImageFont
def font(n):
    return ImageFont.truetype(str(fontroot/'consolab.ttf'),n)
signs = [('Gas','LAST LIGHT','FUEL  /  SERVICE',(179,70,28)),
         ('Motel','VACANCY','SLEEP AT YOUR OWN RISK',(62,95,86)),
         ('Clinic','RELIEF STATION','WATER   +   FIELD RATIONS',(139,135,103)),
         ('Depot','SECTOR 09','MUNICIPAL STORAGE',(127,123,72)),
         ('Diner','DEAD END','DINER  //  OPEN 24H',(146,72,39)),
         ('Warning','QUARANTINE','NO SAFE PASSAGE',(171,133,49))]
for name,title,sub,col in signs:
    im=Image.new('RGB',(512,192),tuple(col)); d=ImageDraw.Draw(im)
    d.rectangle((7,7,504,184),outline=(30,34,27),width=5)
    d.text((256,60),title,font=font(48 if len(title)<13 else 38),fill=(24,29,25),anchor='mm')
    d.text((256,136),sub,font=font(19),fill=(29,34,28),anchor='mm')
    a=np.asarray(im).astype(float)
    damage=rng.random((192,512))
    a*= (.83+damage[:,:,None]*.2)
    a[damage>.986] = (49,45,33)
    Image.fromarray(a.astype('uint8')).save(OUT/'Textures'/f'T_Sign{name}.png')

atlas=Image.new('RGBA',(192,108),(0,0,0,0)); draw=ImageDraw.Draw(atlas)
for code in range(32,128):
    i=code-32; draw.text(((i%16)*12,(i//16)*18-1),chr(code),font=font(14),fill=(255,255,255,255))
# Hard-edged bitmap type is intentional; it remains separate from the 3D camera filter.
a=np.array(atlas);a[:,:,3]=np.where(a[:,:,3]>100,255,0);Image.fromarray(a).save(OUT/'Textures'/'T_Font.png')

SR=32000
def audio(name, dur, fn, level=.85):
    t=np.arange(int(dur*SR))/SR
    n=rng.normal(size=len(t))
    s=fn(t,n)
    s=np.asarray(s); s/=max(1e-6,np.max(np.abs(s))); s*=level
    s[:160]*=np.linspace(0,1,160); s[-320:]*=np.linspace(1,0,320)
    with wave.open(str(OUT/'Audio'/f'S_{name}.wav'),'wb') as w:
        w.setnchannels(1); w.setsampwidth(2); w.setframerate(SR)
        w.writeframes((np.clip(s,-1,1)*32767).astype('<i2').tobytes())

audio('Shotgun',1.6,lambda t,n: n*np.exp(-t*22)*1.9+np.sin(2*np.pi*(72*t-14*t*t))*np.exp(-t*9)+np.convolve(n,np.ones(70)/70,'same')*np.exp(-t*3.8),.95)
audio('Pump',.55,lambda t,n: n*(np.exp(-((t-.07)/.019)**2)+np.exp(-((t-.29)/.024)**2)) + np.sin(t*1750)*np.exp(-t*17),.65)
audio('Reload',.45,lambda t,n: n*(np.exp(-((t-.08)/.009)**2)+.5*np.exp(-((t-.24)/.014)**2)) + np.sin(t*1200)*np.exp(-t*26),.42)
audio('Swing',.5,lambda t,n: np.convolve(n,np.ones(9)/9,'same')*np.sin(np.pi*np.minimum(t/.4,1))**3*np.exp(-t*3),.5)
audio('MetalHit',.8,lambda t,n: (np.sin(t*1433)+np.sin(t*2347)*.5+np.sin(t*3971)*.3)*np.exp(-t*12)+n*np.exp(-t*45),.7)
audio('FleshHit',.48,lambda t,n: np.convolve(n,np.ones(12)/12,'same')*np.exp(-t*24)+np.sin(t*420)*np.exp(-t*22),.8)
audio('StepRoad',.3,lambda t,n: n*np.exp(-t*46)+np.sin(t*400)*np.exp(-t*28),.28)
audio('StepEarth',.38,lambda t,n: np.convolve(n,np.ones(5)/5,'same')*np.exp(-t*21)+n*np.exp(-((t-.1)/.06)**2)*.2,.25)
audio('StepIndoor',.42,lambda t,n: n*np.exp(-t*50)+np.sin(t*620)*np.exp(-t*23)+np.sin(t*930)*np.exp(-t*17)*.2,.25)
audio('Zombie',2.3,lambda t,n: (np.sin(2*np.pi*(65*t+np.sin(t*7)*1.5))+.3*np.sin(t*1240)+np.convolve(n,np.ones(25)/25,'same'))*np.sin(np.pi*t/2.3)**2,.65)
audio('Hurt',.8,lambda t,n: (np.sin(t*420+np.sin(t*25)*3)+np.convolve(n,np.ones(13)/13,'same'))*np.exp(-t*7),.5)
audio('Click',.17,lambda t,n: n*np.exp(-t*60)+np.sin(t*1600)*np.exp(-t*70),.25)
audio('Credit',.42,lambda t,n: (np.sin(t*2*np.pi*780)*(t<.12)+np.sin(t*2*np.pi*1040)*(t>=.12))*np.exp(-t*7),.22)
audio('Wind',18,lambda t,n: np.convolve(n,np.ones(360)/360,'same')*(.7+.3*np.sin(t*.6))+np.sin(t*2*np.pi*41)*.025,.32)
audio('Drone',16,lambda t,n: np.sin(t*2*np.pi*48)*.15+np.sin(t*2*np.pi*48.19)*.13+np.sin(t*2*np.pi*71.9)*.06+np.convolve(n,np.ones(700)/700,'same')*.6,.22)
audio('Generator',4,lambda t,n: np.sin(t*2*np.pi*53)*.4+np.sin(t*2*np.pi*106)*.17+np.convolve(n,np.ones(16)/16,'same')*.2,.4)
print('Authored 30 surface/sign maps and 16 original mono sound assets.')
