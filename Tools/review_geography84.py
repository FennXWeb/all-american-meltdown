import sys,json,math,re,csv,os
from pathlib import Path
root=Path(__file__).resolve().parents[1];sys.path.insert(0,str(root/'Saved/Tools84'))
os.environ["MPLCONFIGDIR"]=str(root/"Saved/Matplotlib84")
import matplotlib;matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
data=json.loads((root/'ContentSource/Geography84/derived.json').read_text());C=((43.0481-42.8864)*1111000,(-76.1474+78.8784)*813000)
def project(lat,lon):
 x=(lat-42.8864)*1111000;y=(lon+78.8784)*813000;dx=x-C[0];dy=y-C[1];r=math.hypot(dx,dy);f=r*10 if r<=90000 else 900000+(r-90000)*200000/1010000 if r<1100000 else r
 return (C[0]+dx*f/r,C[1]+dy*f/r) if r else C
def xy(p):return p[1]/100000,p[0]/100000
fig,ax=plt.subplots(figsize=(17,12),facecolor='#111c25');ax.set_facecolor('#27332c')
for poly in data['canada']:ax.fill(*zip(*map(xy,poly)),color='#344e39',zorder=0)
for name,poly in data['water']:ax.fill(*zip(*map(xy,poly)),color='#234f69',zorder=1)
lake73=re.search(r'Lake73\[\]=\{(.*?)\};',(root/'Source/LethalWorld/LWSyracuseData73.inl').read_text(),re.S).group(1)
poly=[tuple(map(float,p)) for p in re.findall(r'\{([-.\d]+),([-.\d]+)\}',lake73)]
ax.fill(*zip(*map(xy,poly)),color='#234f69',zorder=1)
ax.text(10,7,'LAKE ONTARIO',color='#c2dbe8',fontsize=14,ha='center',rotation=8)
ax.text(18.3,5.5,'ONONDAGA\nLAKE',color='#c2dbe8',fontsize=8,ha='center',zorder=6)

ax.add_collection(LineCollection([[xy(a),xy(b)] for a,b,*_ in data['roads']],colors='#a6ad93',linewidths=.28,alpha=.65,zorder=2))
ax.add_collection(LineCollection([[xy(a),xy(b)] for a,b in data['border']],colors='#de8765',linewidths=.7,zorder=3))
towns=re.findall(r'TEXT\("([A-Z /]+)"\),([\d.]+),(-[\d.]+),\d', (root/'Source/LethalWorld/LWNewYork69.cpp').read_text())
for name,la,lo in towns+[('TORONTO','43.6532','-79.3832')]:
 p=xy(project(float(la),float(lo)));ax.scatter(*p,s=13,color='#ffe0a8',zorder=4);ax.annotate(name,p,xytext=(4,3),textcoords='offset points',color='#eadfc7',fontsize=7,zorder=5)
file=root/'Saved/Geography84_Parcels.csv'
if file.exists():
 with file.open(encoding='utf-16' if file.read_bytes()[:2] in [b'\xff\xfe',b'\xfe\xff'] else 'utf-8-sig') as f:
  rows=list(csv.DictReader(f));x=[float(r['y'])/100000 for r in rows];y=[float(r['x'])/100000 for r in rows];ax.scatter(x,y,s=1,color='#f3ae5a',alpha=.6,zorder=3)
ax.set_xlim(-9,39);ax.set_ylim(-3,19);ax.set_aspect('equal');ax.set_title('UPSTATE NEW YORK / SOUTHERN ONTARIO\nGeographic source layout · expanded Syracuse · compressed regional travel',color='white',fontsize=15,pad=16);ax.set_xlabel('World kilometres east from Buffalo',color='white');ax.set_ylabel('World kilometres north from Buffalo',color='white');ax.tick_params(colors='#a5b2b4');ax.text(.01,.01,'Roads / boundaries / water: Natural Earth and OpenStreetMap contributors (ODbL)\nPlot depicts baked geometry, not a satellite image.',transform=ax.transAxes,color='#bfc8c5',fontsize=8)
out=root/'Saved/Review84';out.mkdir(exist_ok=True);fig.savefig(out/'RegionalMap84.png',dpi=140,bbox_inches='tight');print(out/'RegionalMap84.png')
