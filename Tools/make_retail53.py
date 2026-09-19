import sys,json,math
from pathlib import Path
import bpy
sys.path.insert(0,str(Path(__file__).parent));import make_models_v2 as m
m.OUT=m.ROOT/'ArtSource/ModelsV53';m.setup()
def end(n):m.finish(n,collision='convex')
def ring(r,z):
 for i in range(40):
  a=i*math.tau/40;b=(i+1)*math.tau/40;m.cyl((r*math.cos(a),r*math.sin(a),z),(r*math.cos(b),r*math.sin(b),z),.016,'Steel',n=6)
def shirt(x,y,z,a,color):
 profile=[(-.08,0),(-.16,-.035),(-.29,-.13),(-.22,-.28),(-.15,-.21),(-.14,-.62),(.14,-.62),(.15,-.21),(.22,-.28),(.29,-.13),(.16,-.035),(.08,0)]
 verts=[]
 for depth in [-.025,.025]:
  for u,v in profile:verts.append((x+u*math.cos(a)-depth*math.sin(a),y+u*math.sin(a)+depth*math.cos(a),z+v))
 n=len(profile);faces=[tuple(range(n-1,-1,-1)),tuple(range(n,n*2))]+[(i,(i+1)%n,(i+1)%n+n,i+n) for i in range(n)]
 m.mesh('sewn_garment',verts,faces,color)
 m.cyl((x,y,z+.02),(x,y,z+.14),.012,'Steel',n=6)
# Circular boutique rack with individually cut hanging shirts.
for a in [0,math.pi/2,math.pi,math.pi*1.5]:m.cyl((0,0,.04),(.65*math.cos(a),.65*math.sin(a),.04),.03,'Steel')
m.cyl((0,0,.03),(0,0,1.55),.035,'Steel');ring(.68,1.55)
for i in range(18):
 a=i*math.tau/18;shirt(.66*math.cos(a),.66*math.sin(a),1.39,a,['Cloth','Red','Rubber','Bone'][i%4])
end('RoundRack53')
for x in [-.9,.9]:
 m.cyl((x,0,.04),(x,0,1.8),.028,'Steel');m.cyl((x,-.3,.04),(x,.3,.04),.03,'Steel')
m.cyl((-.9,0,1.8),(.9,0,1.8),.028,'Steel')
for i in range(12):shirt(-.78+i*.14,0,1.64,math.pi/2,['Cloth','Red','Bone'][i%3])
end('GarmentRail53')
for x in [-.85,.85]:
 for y in [-.4,.4]:m.box((x,y,.43),(.045,.045,.86),'Steel')
m.box((0,0,.88),(1.9,1,.06),'Wood',.02)
for i in range(6):
 for j in range(3):m.box((-.6+(i%3)*.6,-.22+(i//3)*.44,.94+j*.035),(.40,.32,.03),['Cloth','Red','Rubber'][i%3],.014)
end('DisplayTable53')
m.box((0,0,.10),(.48,.42,.20),'Steel',.025);m.box((0,-.216,.09),(.41,.014,.075),'Rubber');m.box((0,0,.23),(.40,.32,.06),'Rubber',.01)
for i in range(5):
 for j in range(4):m.box((-.14+i*.07,-.10+j*.055,.27),(.043,.034,.016),'Bone',.004)
m.box((.15,.10,.38),(.13,.10,.23),'Steel',.012);m.box((.15,.045,.42),(.10,.012,.09),'Glow');end('CashRegister53')
m.box((0,0,.47),(2.1,.65,.94),'Wood',.02);m.box((0,0,.97),(2.2,.78,.06),'Bone',.015)
for x in [-.7,0,.7]:m.box((x,-.335,.5),(.62,.018,.75),'Rubber',.008)
end('SalesCounter53')
for x in [-1.55,1.55]:
 for y in [-.5,.5]:m.box((x,y,1.9),(.10,.10,3.8),'Steel')
for z in [.16,1.35,2.55,3.75]:
 m.box((0,0,z),(3.2,1.08,.12),'Rust')
 if z<3:
  for i in range(3):m.box((-1+i,0,z+.4),(.75,.75,.7),'Wood',.015)
end('PalletRack53')
for y in [-.48,0,.48]:m.box((0,y,.07),(1.25,.10,.14),'Wood')
for x in [-.55,-.28,0,.28,.55]:m.box((x,0,.17),(.19,1.12,.07),'Wood')
for x in [-.31,.31]:
 for y in [-.27,.27]:
  m.box((x,y,.52),(.56,.49,.62),'Concrete',.012);m.box((x,y,.845),(.12,.51,.02),'Wood')
end('LoadedPallet53')
for x in [-.25,.25]:
 m.box((x,-.35,.13),(.16,1.4,.12),'Red',.015);m.cyl((x-.045,-.87,.07),(x+.045,-.87,.07),.065,'Rubber')
m.box((0,.3,.28),(.65,.3,.46),'Red',.025);m.cyl((0,.4,.3),(0,.6,1.2),.025,'Steel');m.cyl((-.22,.6,1.2),(.22,.6,1.2),.035,'Rubber');end('PalletJack53')
for x in [-.85,.85]:
 for y in [-.35,.35]:m.box((x,y,.48),(.06,.06,.96),'Steel')
m.box((0,0,.98),(1.9,.85,.08),'Wood',.02);m.box((-.5,0,1.17),(.55,.4,.30),'Concrete',.01);m.box((.55,0,1.05),(.35,.28,.08),'Rubber');m.cyl((.1,.18,1.03),(.1,.18,1.18),.10,'Wood',n=12);end('PackingBench53')
m.box((0,0,1.05),(.85,.10,2.1),'Wood',.025);m.box((0,-.06,1.08),(.72,.016,1.86),'Glass');end('FittingMirror53')
m.export_all();(m.OUT/'models_v53_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'Retail53.blend'))
