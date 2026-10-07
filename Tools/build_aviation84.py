"""Metric aviation meshes: shaped airfoils, continuous shells, glazed apertures and cabin fittings."""
import sys, math, json
from pathlib import Path
import bpy
sys.path.insert(0,str(Path(__file__).parent))
import make_models_v2 as m
m.OUT=m.ROOT/'ArtSource/Aviation84'
colors={'Pearl':(.78,.81,.82),'Leather':(.65,.59,.47),'Carpet':(.025,.035,.05),'Walnut':(.19,.075,.033),'Metal':(.35,.41,.46),'Black':(.015,.018,.024),'Glass':(.065,.17,.22),'Light':(.55,.82,1),'Gold':(.55,.36,.10)}
for k,v in colors.items():m.PALETTE['AV84_'+k]=v
m.setup()
def box(p,s,mat='Metal',b=.015):return m.box(p,s,'AV84_'+mat,b)
def mesh(n,v,f,mat='Pearl'):return m.mesh(n,v,f,'AV84_'+mat)
def line(p,r=.01,mat='Metal'):return m.line(p,r,'AV84_'+mat,12)
def cyl(a,b,r,mat='Metal',r2=None,n=24):return m.cyl(a,b,r,'AV84_'+mat,r2,n)
def finish(n):
 o=m.finish('AV84_'+n)
 o.data.set_sharp_from_angle(angle=.65)
 for p in o.data.polygons:p.use_smooth=True
 bpy.context.view_layer.objects.active=o
 mod=o.modifiers.new('Weighted panel normals','WEIGHTED_NORMAL');mod.keep_sharp=True;bpy.ops.object.modifier_apply(modifier=mod.name)
 return o
def loft(n,rings,mat='Pearl',count=48,skip=None,caps=True):
 v=[(x,w*math.cos(i*math.tau/count),z+h*math.sin(i*math.tau/count)) for x,w,h,z in rings for i in range(count)]
 f=[]
 for j in range(len(rings)-1):
  for i in range(count):
   k=(i+1)%count;face=(j*count+i,j*count+k,(j+1)*count+k,(j+1)*count+i)
   center=tuple(sum(v[a][b] for a in face)/4 for b in range(3))
   if not skip or not skip(center):f.append(face)
 if caps:f.extend([tuple(range(count-1,-1,-1)),tuple((len(rings)-1)*count+i for i in range(count))])
 return mesh(n,v,f,mat)
def wing(side,span,root,tip,xroot,z,inner):
 # Closed NACA-inspired section loft, with swept leading edge and tapered chord.
 sections=[(inner,root,xroot,z),(inner+.22*(span-inner),root*.86,xroot-.9,z+.15),(inner+.82*(span-inner),tip*1.7,xroot-3.9,z+.55),(span,tip,xroot-4.9,z+1.35)]
 v=[]
 for y,chord,x,zz in sections:
  for i in range(32):
   a=i*math.tau/32;u=(1-math.cos(a))*.5
   thick=5*.13*(.2969*math.sqrt(max(u,0))-.126*u-.3516*u*u+.2843*u**3-.1036*u**4)
   v.append((x-u*chord,side*y,zz+math.copysign(thick*chord,math.sin(a))))
 f=[(j*32+i,j*32+(i+1)%32,(j+1)*32+(i+1)%32,(j+1)*32+i) for j in range(3) for i in range(32)]
 f.extend([tuple(range(31,-1,-1)),tuple(96+i for i in range(32))]);mesh('laminar wing',v,f)
def nacelle(x,y,z,scale=1):
 rings=[(x+q*scale,w*scale,w*scale,z) for q,w in [(-2,.32),(-1.6,.6),(-.2,.83),(.5,.80),(.65,.70)]]
 o=loft('nacelle',rings,'Pearl',48);o.location.y=y
 cyl((x+.59*scale,y,z),(x+.64*scale,y,z),.65*scale,'Black')
 for k in range(20):
  a=k*math.tau/20;b=a+.19
  mesh('turbine vane',[(x+.66*scale,y+math.cos(a)*r*scale,z+math.sin(a)*r*scale) for r in [.12,.61]]+[(x+.63*scale,y+math.cos(b)*r*scale,z+math.sin(b)*r*scale) for r in [.61,.12]],[(0,1,2,3)],'Metal')
 cyl((x+.67*scale,y,z),(x+.83*scale,y,z),.16*scale,'Metal',0)
def shell(kind,half,w,floor,center,rz,luxury=False):
 # Shell apertures align exactly to separate glazed window and shade meshes.
 xs=sorted(set([-half,-half+.65,-half+2,-half+3]+[x*.25 for x in range(int((-half+4)*4),int((half-4)*4)+1)]+[half-3,half-2,half-1,half]))
 def profile(x):
  if x>half-4:return max(.018,1-((x-(half-4))/4)**2)**.6
  if x<-half+4:return max(.035,(x+half)/4)**.75
  return 1
 def cockpit_cut(p):
  x,y,z=p
  return half-4.3<x<half-1.7 and floor+.90<z<floor+(2.70 if kind=='Airbus' else 2.10)
 def cut(p):
  x,y,z=p
  if kind=='Airbus':window=-11.6<x<11.6 and abs((x+11.5+.5)%1-.5)<.26 and floor+.95<z<floor+1.58
  else:window=-7.3<x<7.0 and abs((x+6.9+.75)%1.5-.75)<.4 and floor+.75<z<floor+1.45
  entry=half-5.4<x<half-4.4 and y>0 and floor-.15<z<floor+2.05
  return (window and abs(y)>w*.8) or entry or cockpit_cut(p)
 rings=[]
 for x in xs:
  h=rz*profile(x);z=center
  if x>half-4:
   # Drop the nose below the windscreen: the pilot sees over a closed hood,
   # rather than looking through a long cut-out toward a floating nose cap.
   t=(x-(half-4))/4;u=min(1,t/.55);u=u*u*(3-2*u)
   top=(center+rz)*(1-u)+(floor+.75)*u
   if t>.55:top=(floor+.75)*(1-(t-.55)/.45)+(floor+.30)*(t-.55)/.45
   v=t*t*(3-2*t);bottom=(center-rz)*(1-v)+(floor+.15)*v
   z=(top+bottom)*.5;h=(top-bottom)*.5
  rings.append((x,w*profile(x),h,z))
 o=loft('fuselage',rings,count=256,skip=cut)
 if luxury:
  o.data.materials.append(m.MATS['AV84_Black'])
  for face in o.data.polygons:
   if sum(o.data.vertices[i].co.z for i in face.vertices)/len(face.vertices)<center-.15:face.material_index=1
 bpy.context.view_layer.objects.active=o;sol=o.modifiers.new('Double-skin pressure hull','SOLIDIFY');sol.thickness=.045;bpy.ops.object.modifier_apply(modifier=sol.name)
 box((0,0,floor-.06),(2*half-7,w*1.81,.12),'Carpet')
 # The windscreen occupies the actual removed hull faces, including the forward
 # nose arc. Side-only glazing left an opaque hull directly ahead of the pilot.
 loft('panoramic cockpit glass',rings,'Glass',256,lambda p:not cockpit_cut(p),False)
 for side in [-1,1]:
  wing(side,17 if kind=='Airbus' else 10,5.8 if kind=='Airbus' else 4.3,1,1,floor-.45,0)
  wing(side,6.3 if kind=='Airbus' else 4.5,3,1,-half+5,center+.6,w*.98)
  nacelle(0,side*6.2,floor-.75,1.35) if kind=='Airbus' else nacelle(-half+4.2,side*2.45,center,.85)
 # Swept, tapered vertical stabilizer, a closed sculpted mesh.
 v=[(-half+5,-.14,center),(-half+1,-.12,center),(-half+1.3,-.055,center+5),(-half+2.4,-.07,center+5),(-half+5,.14,center),(-half+1,.12,center),(-half+1.3,.055,center+5),(-half+2.4,.07,center+5)]
 mesh('tail fin',v,[(0,1,2,3),(4,7,6,5),(0,4,5,1),(1,5,6,2),(2,6,7,3),(3,7,4,0)])
 for side in [-1,1]:line([(-half+4,side*w*.9,center-.15),(half-4,side*w*.96,center-.15)],.026,'Gold')
 if luxury:
  for side in [-1,1]:
   # Swept dark chine, flush luminous accents and sculpted upturned wing tips.
   line([(-half+4,side*w*.95,center+.08),(half-5,side*w*.97,center+.08)],.07,'Black')
   line([(-half+4,side*w*.98,center+.13),(half-5,side*w*.98,center+.13)],.012,'Light')
   mesh('swept winglet',[(-4,side*16.8,floor+1),(-5.8,side*17,floor+1),(-5.5,side*17.7,floor+3.6),(-4.9,side*17.7,floor+3.6)],[(0,1,2,3)],'Metal')
 finish(kind+('Luxury' if luxury else '')+'Shell')
 if luxury:return
 # All wheel/strut geometry is separately retractable.
 for x,y in [(half-5,0),(-2,-w*.85),(-2,w*.85)]:
  cyl((x,y,.55),(x,y,floor-.1),.085);line([(x-1,y,floor-.3),(x,y,.72)],.055)
  for s in [-1,1]:
   cyl((x,y+s*.17,.45),(x,y+s*.38,.45),.45,'Black',n=32)
   cyl((x,y+s*.385,.45),(x,y+s*.397,.45),.25,'Metal',n=24)
 finish(kind+'Gear')
 # Curved pressure door follows the hull rather than bridging it with a flat slab.
 base=1.40 if kind=='Jet' else 1.92
 def dy(z):return base-w*math.sqrt(max(.01,1-((floor+z-center)/rz)**2))
 v=[];f=[];zs=sorted(set([i*2.06/16 for i in range(17)]+[1.1,1.6]));xs=[-.49,-.21,.21,.49]
 for z0,z1 in zip(zs,zs[1:]):
  for j,(x0,x1) in enumerate(zip(xs,xs[1:])):
   if j==1 and z0>=1.1 and z1<=1.6:continue
   k=len(v);v.extend([(x,dy(z)+d,z) for d in [0,.035] for x,z in [(x0,z0),(x1,z0),(x1,z1),(x0,z1)]]);f.extend([tuple(k+i for i in face) for face in [(0,1,2,3),(4,7,6,5),(0,4,5,1),(1,5,6,2),(2,6,7,3),(3,7,4,0)]])
 mesh('curved door',v,f)
 glass=[(x,dy(z)+.012,z) for z in [1.1+i*.1 for i in range(6)] for x in [-.21,.21]]
 mesh('door viewport',glass,[(i*2,i*2+1,i*2+3,i*2+2) for i in range(5)],'Glass')
 line([(-.34,dy(.83)-.035,.83),(-.19,dy(.83)-.035,.83)],.023)
 finish(kind+'Door')
shell('Jet',12,1.45,1.5,2.3,1.65)
shell('Airbus',19,1.98,3,3.55,2.25)
shell('Airbus',19,1.98,3,3.55,2.25,True)
# Upholstered seat: contoured layered cushions, tilted back, piping, armrests and recline hinge.
for luxury in [False,True]:
 width=.63 if luxury else .48
 box((0,0,.24),(.54,width*.8,.39),'Black',.07)
 box((.07,0,.48),(.60,width,.18),'Leather',.07)
 for s in [-1,1]:
  box((-.025,s*(width*.5+.035),.73),(.50,.075,.075),'Walnut',.025)
  line([(-.19,s*width*.43,.53),(.32,s*width*.43,.53)],.007,'Gold')
  cyl((-.15,s*(width*.5+.045),.55),(-.15,s*(width*.5+.055),.55),.075,'Metal')
 if luxury:box((.38,0,.35),(.28,width*.86,.09),'Leather',.035)
 finish('LuxurySeat' if luxury else 'Seat')
 # The back reclines around its own hinge, leaving the base planted on the floor.
 o=box((0,0,.34),(.18,width,.84),'Leather',.08);o.rotation_euler.y=-.10
 box((-.05,0,.77),(.21,width*.80,.25),'Leather',.07)
 finish('LuxurySeatBack' if luxury else 'SeatBack')
# Rounded window frames and glazed centers. +Y is outwards, floor-relative origin at center.
for kind,w,rz,offset,base in [('Jet',1.45,1.65,.32,1.43),('Airbus',1.98,2.25,.73,1.96)]:
 for name,mat in [('Window','Glass'),('Shade','Leather')]:
  pts=[]
  for cx,cz,a in [(.17,.28,0),(-.17,.28,90),(-.17,-.28,180),(.17,-.28,270)]:
   for j in range(9):
    q=math.radians(a+j*90/8);z=cz+.10*math.sin(q);pts.append((cx+.10*math.cos(q),base-w*math.sqrt(max(.01,1-((z+offset)/rz)**2))-.005,z))
  # Fan triangulation follows the curved frame; no planar n-gon across the aperture.
  mid=(0,base-w*math.sqrt(1-(offset/rz)**2)-.005,0)
  mesh(name,pts+[mid],[(len(pts),i,(i+1)%len(pts)) for i in range(len(pts))],mat)
  if name=='Window':line(pts+[pts[0]],.025,'Metal')
  finish(kind+name)
box((0,0,.26),(1.95,1.28,.48),'Walnut',.045);box((0,0,.57),(1.94,1.27,.20),'Leather',.09)
for y in [-.30,.30]:box((-.63,y,.73),(.48,.5,.14),'Pearl',.06)
finish('Bed')
box((0,0,.60),(1.40,.47,1.18),'Walnut',.035);box((0,0,1.22),(1.48,.54,.06),'Metal',.02)
for x in [-.45,0,.45]:
 box((x,-.249,.68),(.39,.025,.95),'Pearl',.012);line([(x-.09,-.275,.95),(x+.09,-.275,.95)],.01)
 cyl((x,.02,1.3),(x,.02,1.5),.04,'Glass');cyl((x,.02,1.5),(x,.02,1.58),.021,'Metal')
finish('Bar')
# Overhead bins have moving hinged fronts, independently selected storage.
box((0,0,.20),(.88,.45,.035));box((0,.22,0),(.88,.035,.40));box((0,0,-.2),(.88,.45,.035))
for x in [-.44,.44]:box((x,0,0),(.035,.45,.40))
finish('Bin')
box((0,-.22,0),(.88,.035,.38),'Pearl',.06);line([(-.10,-.245,-.08),(.10,-.245,-.08)],.009)
finish('BinLid')
box((0,0,0),(.055,1.0,.62),'Black',.025);box((-.03,0,0),(.008,.95,.56),'Glass',.008);finish('TV')
box((0,0,.76),(.45,1.82,.30),'Carpet',.035)
for y in [-.62,0,.62]:box((-.235,y,.83),(.016,.5,.3),'Black',.018)
for y in [-.5,.5]:line([(-.40,y,.73),(-.48,y,.78),(-.48,y+.1,.86),(-.48,y+.2,.78),(-.40,y+.2,.73)],.022)
box((-.50,0,.43),(.62,.27,.70),'Carpet',.04)
for y in [-.055,.055]:line([(-.55,y,.80),(-.42,y,.96)],.025)
finish('Cockpit')
box((0,0,.60),(.60,.42,1.2),'Walnut',.03);box((-.315,0,.8),(.015,.37,.75),'Metal',.03);line([(-.33,-.13,.65),(-.33,-.13,1.0)],.017);finish('Fridge')
box((0,0,.48),(.6,1.8,.2),'Leather',.06);box((-.25,0,.83),(.2,1.8,.64),'Leather',.08)
for y in [-.91,.91]:box((0,y,.66),(.6,.14,.4),'Leather',.065)
finish('Couch')
# Airport baggage carousel: continuous annular belt with recessed centre and ribs.
v=[];f=[];n=96
for sx,sy,z in [(4,1.5,.12),(4,1.5,.48),(3.3,.86,.63),(3.3,.86,.20)]:
 for i in range(n):a=i*math.tau/n;v.append((sx*math.cos(a),sy*math.sin(a),z))
for j in range(3):
 for i in range(n):k=(i+1)%n;f.append((j*n+i,j*n+k,(j+1)*n+k,(j+1)*n+i))
mesh('baggage belt',v,f,'Black')
for i in range(48):
 a=i*math.tau/48;line([(3.32*math.cos(a),.88*math.sin(a),.64),(3.96*math.cos(a),1.48*math.sin(a),.49)],.012,'Metal')
finish('BaggageCarousel')
box((0,0,.68),(3,.85,.12),'Black',.06)
for x in [-1.2,1.2]:box((x,0,.31),(.12,.76,.64),'Metal',.015)
for y in [-.57,.57]:box((0,y,1.23),(1.15,.14,1.18),'Pearl',.05)
box((0,0,1.87),(1.15,1.25,.14),'Pearl',.04)
for x in [-.57,.57]:
 for y in [-.42,-.28,-.14,0,.14,.28,.42]:box((x,y,1.29),(.025,.13,.98),'Black',.007)
box((.65,-.75,1.15),(.5,.3,.35),'Black',.025);finish('SecurityScanner')
box((0,.35,1.02),(.045,.70,2.04),'Walnut',.025)
box((-.026,.35,1.28),(.015,.61,1.30),'Pearl',.018)
line([(-.05,.56,.92),(-.05,.56,1.10)],.012,'Gold');finish('SuiteDoor')
m.export_all()
(m.OUT/'models_aviation84_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values()),'palette':colors},indent=2))
bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'Aviation84.blend'))
print('AVIATION84_MODELS_COMPLETE',len(m.RECORDS))
