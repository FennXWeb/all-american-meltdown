from PIL import Image, ImageDraw
from pathlib import Path
import math
root=Path(__file__).resolve().parents[1]
im=Image.new('RGBA',(256,256),(17,24,18,255));d=ImageDraw.Draw(im)
d.rectangle((12,12,243,243),outline=(192,183,125),width=5)
for angle in (0,120,240):
    d.pieslice((40,40,216,216),angle-27,angle+27,fill=(190,107,48))
d.ellipse((94,94,162,162),fill=(17,24,18))
d.ellipse((111,111,145,145),fill=(192,183,125))
for y in range(0,256,4):d.line((0,y,255,y),fill=(10,15,11,80))
out=root/'Build'/'Windows';out.mkdir(parents=True,exist_ok=True)
im.save(out/'Application.ico',sizes=[(16,16),(32,32),(48,48),(64,64),(128,128),(256,256)])
