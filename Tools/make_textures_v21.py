from pathlib import Path
import numpy as np
from PIL import Image
out=Path('ArtSource/TexturesV21');out.mkdir(parents=True,exist_ok=True)
y,x=np.mgrid[:512,:512];rng=np.random.default_rng(2121)
noise=rng.normal(0,4,(512,512))
weave=((x//8+y//8)%2)*12+np.sin((x+y)*.8)*3
rgb=np.stack([25+weave+noise,27+weave+noise,29+weave+noise],-1);Image.fromarray(np.uint8(np.clip(rgb,0,255))).save(out/'T_Carbon21.png')
vein=np.sin(x*.018+np.sin(y*.026)*2+np.sin((x+y)*.06)*.3)
rgb=np.stack([173+vein*24+noise,166+vein*23+noise,145+vein*21+noise],-1);Image.fromarray(np.uint8(np.clip(rgb,0,255))).save(out/'T_CasinoMarble21.png')
dx=x%128-64;dy=y%128-64;art=(np.abs(np.sqrt(dx*dx+dy*dy)-48)<2)|(np.abs(dx-dy)<1)|(np.abs(dx+dy)<1)
rgb=np.stack([55+noise,12+noise*.4,22+noise*.6],-1);rgb[art]=[155,115,48];Image.fromarray(np.uint8(np.clip(rgb,0,255))).save(out/'T_CasinoVelvet21.png')
