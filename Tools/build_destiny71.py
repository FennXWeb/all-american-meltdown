"""Reproducible Destiny USA public-layout reconstruction.
Sources: OSM way 108262911 (ODbL); Destiny USA Directory.pdf v0114.
Directory is schematic: its public circulation topology is registered to OSM,
not represented as a measured architectural survey. Generated file is offline data.
"""
from pathlib import Path
import sys, json, math, xml.etree.ElementTree as ET
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'Saved/Tools71'))
import numpy as np, fitz
from shapely.geometry import Polygon,Point,LineString,MultiPoint,box
from shapely.ops import unary_union, triangulate, transform, nearest_points
from shapely import constrained_delaunay_triangles
from PIL import Image,ImageDraw
REF=ROOT/'Saved/References71';OUT=ROOT/'Source/LethalWorld/LWDestinyData71.inl'
r=ET.parse(REF/'site.osm').getroot();ns={n.get('id'):(float(n.get('lon')),float(n.get('lat'))) for n in r.findall('node')}
w=r.find("way[@id='108262911']");ll=[ns[n.get('ref')] for n in w.findall('nd')][:-1]
ea=[((lon+76.1705)*81300,(lat-43.0712)*111100) for lon,lat in ll]
origin=np.array([(min(v[0] for v in ea)+max(v[0] for v in ea))/2,(min(v[1] for v in ea)+max(v[1] for v in ea))/2])
v=np.array(ea[72])-ea[0];v/=np.linalg.norm(v);u=np.array([-v[1],v[0]])
uv=np.array([[np.dot(np.array(p)-origin,u),np.dot(np.array(p)-origin,v)] for p in ea]);shell=Polygon(uv).buffer(0)
# Corresponding outside corners. Internal positions are interpolated from the published diagram.
anchors=[(1203.714,524.508,0),(1221.864,550.407,72),(1088.519,496.58,14),(1058.224,496.58,16),(926.304,487.967,34),(870.062,487.967,35),(771.674,530.20,47),(758.512,553.151,49),(694.818,594.483,51),(825.686,667.681,55),(938.257,660.598,57),(1037.953,633.76,60),(1131.673,594.244,63)]
# Piecewise affine registration keeps the mapped outline exact and avoids imposing false precision on schematic interiors.
sp=np.array([[x,y] for x,y,i in anchors]);tp=np.array([uv[i] for x,y,i in anchors]);tris=triangulate(MultiPoint(sp));maps=[]
for t in tris:
 a=np.array(t.exterior.coords)[:3];ids=[int(np.argmin(np.linalg.norm(sp-p,axis=1))) for p in a];mat=np.column_stack([a,np.ones(3)]);coef=np.linalg.solve(mat,tp[ids]);maps.append((t,coef))
def warp(x,y):
 p=Point(x,y);t,c=min(maps,key=lambda tc:tc[0].distance(p));return np.array([x,y,1.])@c
pdf=fitz.open(REF/'Directory.pdf');draws=pdf[1].get_drawings()
def points(d):
 out=[]
 for it in d['items']:
  if it[0]=='l':
   if not out:out.append(tuple(it[1]))
   out.append(tuple(it[2]))
  elif it[0]=='c':
   a,b,c,e=[np.array(tuple(v)) for v in it[1:]]
   if not out:out.append(tuple(a))
   for i in range(1,9):
    t=i/8;out.append(tuple((1-t)**3*a+3*(1-t)**2*t*b+3*(1-t)*t*t*c+t**3*e))
  elif it[0]=='re':out.extend([tuple(v) for v in [it[1].tl,it[1].tr,it[1].br,it[1].bl]])
 return out
raw={0:[],1:[],2:[],3:[]}
for i,d in enumerate(draws):
 f=d['fill'];rr=d['rect'];L=None
 if not f:continue
 if abs(f[0]-.769)<.01 and abs(f[1]-.11)<.01:L=1;offset=(0,0)
 elif abs(f[0]-.88)<.02 and abs(f[1]-.8)<.02 and f[2]<.4:L=2;offset=(5.63,239.14)
 elif rr.x0>690 and rr.y1<210 and abs(f[0]-.49)<.02 and abs(f[2]-.54)<.02:L=3;offset=(0,461)
 elif i in [1365,1369]:L=3;offset=(0,461)
 elif abs(f[0]-.81)<.01 and abs(f[2]-.58)<.01 and rr.y0>720:L=0;offset=(0,-231)
 if L is None:continue
 pts=points(d)
 if len(pts)<3:continue
 # Densify before the non-global affine transform so edges track triangulation seams.
 dense=[]
 for a,b in zip(pts,pts[1:]+pts[:1]):
  steps=max(1,int(math.dist(a,b)/2))
  for k in range(steps):dense.append(warp(a[0]+(b[0]-a[0])*k/steps+offset[0],a[1]+(b[1]-a[1])*k/steps+offset[1]))
 poly=Polygon(dense).buffer(0).intersection(shell.buffer(-.2)).simplify(.18,preserve_topology=True)
 for g in getattr(poly,'geoms',[poly]):
  if g.geom_type=='Polygon' and g.area>20:raw[L].append((i,g))
# Public halls traced from directory centerlines; preserved on every retail level.
paths=[[(976,496),(976,542),(976,589),(941,612)],[(836,542),(976,542),(1108,542),(1160,542)],[(833,542),(822,568),(794,592),(770,622)],[(976,612),(850,612),(822,605),(794,592)]]
halls=[LineString([warp(*p) for p in line]) for line in paths]
# Expansion is the southwest wing; the original two-level building/commons are distinct.
expansion=shell.intersection(Polygon([warp(680,675),warp(680,529),warp(838,529),warp(838,542),warp(827,577),warp(976,589),warp(1040,636),warp(1040,690)]).buffer(0))
# The first two public levels share their footprint. Third is expansion + mapped cinema/lobby only.
upper=unary_union([expansion]+[g for i,g in raw[3]]+[h.buffer(7) for h in halls[2:]]+[LineString([warp(976,542),warp(976,589),warp(941,612)]).buffer(7)]+[Point(warp(976,542)).buffer(14),LineString([warp(976,542),warp(976,496)]).buffer(7)]).buffer(2).intersection(shell)
# Original octagonal atrium, Canyon void, and the old concourse's long upper-level light wells.
atrium=Point(warp(976,542)).buffer(10,resolution=2)
canyon=LineString([warp(777,613),warp(811,581)]).buffer(6,cap_style=1)
longvoid=LineString([warp(877,542),warp(948,542)]).buffer(2.8,cap_style=2)
# Fixed escalator banks follow directory circulation; metres and floor indices.
esc=[]
for p,levels in [((965,574),(0,1)),((846,541),(1,)),((1099,536),(0,1)),((785,610),(1,2)),((817,578),(1,2)),((943,612),(1,2))]:
 c=warp(*p);line=min(halls,key=lambda h:h.distance(Point(c)));s=line.project(Point(c));d=np.array(line.interpolate(min(line.length,s+1)).coords[0])-np.array(line.interpolate(max(0,s-1)).coords[0]);d/=np.linalg.norm(d)
 for l in levels:esc.append((l,c,d))
# Avoid cutting store floors for stair holes: clip all store polygons out of circulation, atria and stair access.
access=unary_union([LineString([c-d*9,c+d*9]).buffer(3.2,cap_style=2) for l,c,d in esc])
voids={0:Polygon(),1:Polygon(),2:unary_union([atrium,canyon,longvoid]),3:unary_union([atrium,canyon])}
carousel=(uv[20]+uv[30])*.5;inward=warp(976,542)-carousel;carousel+=inward/np.linalg.norm(inward)*3
publiccourts=unary_union([atrium.buffer(3),canyon.buffer(3),Point(carousel).buffer(15)])
retail={};floorpolys={};ceilings={}
for L in range(4):
 clean=[]
 for i,g in raw[L]:
  g=g.difference(publiccourts if L else Polygon()).difference(access).difference(unary_union([h.buffer(3.0) for h in halls]) if L else Polygon()).buffer(0)
  for sub in getattr(g,'geoms',[g]):
   if sub.geom_type=='Polygon' and sub.area>25:clean.append((i,sub))
 retail[L]=clean
 floors=shell if L in (1,2) else upper if L==3 else shell.difference(expansion)
 holes=[LineString([c-d*6,c+d*6]).buffer(2.7,cap_style=2) for l,c,d in esc if l+1==L]
 floorpolys[L]=floors.difference(unary_union(holes+[voids[L]]))
# Stair landings bridge the Canyon edge. Keep them independent of the atrium cuts.
for L in range(4):
 pads=[]
 for l,c,d in esc:
  for level,t in [(l,-7.5),(l+1,7.5)]:
   if level==L:pads.append(LineString([c+d*(t-1.49),c+d*(t+1.49)]).buffer(3,cap_style=2))
 floorpolys[L]=unary_union([floorpolys[L]]+pads).intersection(shell).buffer(0)
 # The schematic's narrow cross-Canyon bridges otherwise disconnect after registration.
 if L>=2:
  pieces=[p for p in getattr(floorpolys[L],'geoms',[floorpolys[L]]) if p.area>1]
  g=max(pieces,key=lambda p:p.area);pieces.remove(g)
  while pieces:
   p=min(pieces,key=lambda p:p.distance(g));a,b=nearest_points(g,p)
   g=unary_union([g,p,LineString([a,b]).buffer(2.5,cap_style=3)]);pieces.remove(p)
  floorpolys[L]=g.intersection(shell)
upper=unary_union([upper,floorpolys[3]])
# Physical entrance locations picked from exterior corridor endpoints, not arbitrary points through stores.
entries=[]
for p in [(770,622),(944,630),(978,496),(1110,541),(843,533)]:
 c=Point(warp(*p));edge=shell.exterior.interpolate(shell.exterior.project(c));entries.append(np.array(edge.coords[0]))
# Mesh panels carry a top surface and a thickness; triangulation is offline and has real openings.
def polys(g):return [p for p in getattr(g,'geoms',[g]) if p.geom_type=='Polygon' and p.area>.01]
def coords(poly):return [list(map(float,p)) for p in poly.exterior.coords[:-1]]
def local(p):
 e=np.array(p)[0]*u+np.array(p)[1]*v;return [round(float(e[1]*100),2),round(float(e[0]*100),2)]
panels=[]
def panel(g,z,mat,thick=24):
 for p in polys(g):
  vert=[];ind=[]
  for t in constrained_delaunay_triangles(p).geoms:
   q=list(t.exterior.coords)[:3];base=len(vert);vert.extend(local(v) for v in q);ind.extend([base,base+1,base+2])
  panels.append({'z':z,'mat':mat,'thick':thick,'v':vert,'t':ind,'edge':[[local(q) for q in p.exterior.coords[:-1]]]+[[local(q) for q in ring.coords[:-1]] for ring in p.interiors]})
for L,g in floorpolys.items():panel(g,L*600,'Concrete' if L==0 else 'Terrazzo65')
panel(expansion,570,'Concrete',590)
# Retail soffits leave the public atria open. Roof terraces have the same holes as the skylights.
panel(shell.difference(upper).difference(atrium).difference(longvoid),1800,'RV66_Ivory',32)
panel(upper.difference(atrium).difference(canyon),2500,'RV66_Ivory',32)
panel(longvoid.difference(upper),1800,'WindowGlass',12)
panel(shell.difference(expansion).difference(access).difference(unary_union([p for _,p in retail[0]])),580,'Concrete',24)
for L,zones in retail.items():
 for i,g in zones:panel(g,L*600+565,'Acoustic65',16)
# Data for walls, shopfronts, furnishings, routes and fixtures.
zones=[]
for L,zz in retail.items():
 allshops=unary_union([p for _,p in zz]);frontunion=unary_union([h.buffer(9) for h in halls])
 for i,g in zz:
  ed=[];pp=coords(g)
  for a,b in zip(pp,pp[1:]+pp[:1]):
   a=np.array(a);b=np.array(b);d=b-a;ln=np.linalg.norm(d)
   if ln<.4:continue
   n=np.array([-d[1],d[0]])/ln;mid=(a+b)/2
   if not g.contains(Point(mid+n*.3)):n=-n
   outside=Point(mid-n*1.2)
   front=ln>3 and shell.contains(outside) and not allshops.buffer(.15).contains(outside)
   ed.append({'a':local(a),'b':local(b),'in':local(n/100),'front':bool(front)})
  # Place furnishings on a clear aisle grid wholly inside the real-shaped retail footprints.
  bb=g.bounds;fixtures=[];inner=g.buffer(-2)
  for x in np.arange(math.ceil(bb[0]/5)*5,bb[2],5):
   for y in np.arange(math.ceil(bb[1]/5)*5,bb[3],5):
    if inner.contains(Point(x,y)) and not access.buffer(2).contains(Point(x,y)):fixtures.append(local((x,y)))
  fronts=[e for e in ed if e['front']]
  zones.append({'id':i,'l':L,'area':g.area,'center':local(g.representative_point().coords[0]),'edge':ed,'fixtures':fixtures,'fronts':fronts})
# Corridor lamps, seating and roof trusses share a topology, so they stay off shop floor/facade boundaries.
route=[]
for h in halls:
 for s in np.arange(0,h.length,7):
  p=h.interpolate(s);n=h.interpolate(min(s+1,h.length));route.append((local(p.coords[0]),local((np.array(n.coords[0])-p.coords[0])/100)))
data={'panels':panels,'zones':zones,'shell':[local(q) for q in uv],'entries':[local(q) for q in entries], 'escalators':[{'l':l,'p':local(c),'d':local(d/100)} for l,c,d in esc], 'routes':route,'atrium':local(warp(976,542)),'carousel':local(carousel),'canyon':[local(warp(*p)) for p in [(777,613),(811,581)]], 'origin_ll':[-76.1705+origin[0]/81300,43.0712+origin[1]/111100]}
# Preserve surveyed bridge location/orientation from OSM rather than drawing one across an arbitrary aisle.
bw=r.find("way[@id='249375762']");bp=[]
for n in bw.findall('nd'):
 lon,lat=ns[n.get('ref')];e=np.array([(lon+76.1705)*81300,(lat-43.0712)*111100])-origin;bp.append([e[1]*100,e[0]*100])
data['bridge']=bp[:-1]
# Actual parking aisles and access roads; clip the research map to the reserved game parcel.
roads=[]
parcel=Polygon([np.array([east,north])@np.array([u,v]).T for north,east in [(-500,-325),(500,-325),(500,325),(-500,325)]])
for way in r.findall('way'):
 tags={t.get('k'):t.get('v') for t in way.findall('tag')}
 if tags.get('highway') not in ['service','tertiary','secondary','residential']:continue
 if tags.get('tunnel')=='yes' or tags.get('covered')=='yes':continue
 path=[]
 for node in way.findall('nd'):
  lon,lat=ns[node.get('ref')];e=np.array([(lon+76.1705)*81300,(lat-43.0712)*111100])-origin;path.append([e@u,e@v])
 if len(path)<2:continue
 cut=LineString(path).intersection(parcel.buffer(-8)).difference(shell.buffer(2))
 for line in getattr(cut,'geoms',[cut]):
  if line.geom_type!='LineString':continue
  for a,b in zip(line.coords,list(line.coords)[1:]):
   if math.dist(a,b)<2:continue
   roads.append({'a':local(a),'b':local(b),'parking':tags.get('service')=='parking_aisle','width':600 if tags.get('service')=='parking_aisle' else int(tags.get('lanes','2'))*340})
data['roads']=roads
json.dump(data,open(REF/'Layout71.json','w'),separators=(',',':'))
# Human-reviewable plan overlays of all four levels.
im=Image.new('RGB',(1800,1200),(22,28,31));dr=ImageDraw.Draw(im)
for L in range(4):
 ox=(L%2)*900+450;oy=(L//2)*600+285
 def pix(p):return (ox+p[0]*1.15,oy+p[1]*1.15)
 for g in polys(floorpolys[L]):
  dr.polygon([pix(q) for q in g.exterior.coords],fill=(185,182,168))
  for h in g.interiors:dr.polygon([pix(q) for q in h.coords],fill=(22,28,31))
 for i,g in retail[L]:dr.polygon([pix(q) for q in g.exterior.coords],fill=(94+L*22,120,135),outline=(40,45,50))
 dr.line([pix(q) for q in list(uv)+[uv[0]]],fill=(244,200,120),width=2);dr.text((L%2*900+30,L//2*600+25),['COMMONS / PARKING','LEVEL 1','LEVEL 2','LEVEL 3 / CINEMA'][L],fill='white')
im.save(REF/'ReconstructionPlan.png')
# Generated C++ data uses an initializer function so the engine never parses a JSON file while streaming.
def vec(q):return 'FVector2D('+','.join(f'{v:.2f}' for v in q)+')'
f=['// Generated by Tools/build_destiny71.py. OSM contributors (ODbL); official directory registration.']
calls=[]
for index,p in enumerate(panels):
 name=f'AddPanel71_{index}';f.append(f'static void {name}(FData71& D){{');calls.append(f'{name}(D);')
 f.append('{FPanel71 P;P.Z=%.2ff;P.Thickness=%.2ff;P.Material=TEXT("%s");'%(p['z'],p['thick'],p['mat']));f.append('P.Vertices={'+','.join(vec(q) for q in p['v'])+'};');f.append('P.Indices={'+','.join(map(str,p['t']))+'};');f.append('P.Rings={'+','.join('{'+','.join(vec(q) for q in ring)+'}' for ring in p['edge'])+'};D.Panels.Add(MoveTemp(P));}}')
for index,z in enumerate(zones):
 name=f'AddZone71_{index}';f.append(f'static void {name}(FData71& D){{');calls.append(f'{name}(D);')
 f.append('{FZone71 Z;Z.Id=%d;Z.Level=%d;Z.Center=%s;Z.Area=%.2ff;'%(z['id'],z['l'],vec(z['center']),z['area']));f.append('Z.Edges={'+','.join('{'+vec(e['a'])+','+vec(e['b'])+','+vec(e['in'])+','+str(e['front']).lower()+'}' for e in z['edge'])+'};');f.append('Z.Fixtures={'+','.join(vec(q) for q in z['fixtures'])+'};D.Zones.Add(MoveTemp(Z));}}')
f.append('static FData71 MakeData71(){FData71 D;'+''.join(calls))
f.append('D.Shell={'+','.join(vec(q) for q in data['shell'])+'};D.Entries={'+','.join(vec(q) for q in data['entries'])+'};D.Bridge={'+','.join(vec(q) for q in data['bridge'])+'};')
f.append('D.Escalators={'+','.join('{%d,%s,%s}'%(e['l'],vec(e['p']),vec(e['d'])) for e in data['escalators'])+'};')
f.append('D.Route={'+','.join('{'+vec(p)+','+vec(d)+'}' for p,d in route)+'};')
f.append('D.Roads={'+','.join('{'+vec(e['a'])+','+vec(e['b'])+','+str(e['width'])+','+str(e['parking']).lower()+'}' for e in roads)+'};')
f.append('D.Atrium=%s;D.Carousel=%s;D.CanyonA=%s;D.CanyonB=%s;return D;}'%(vec(data['atrium']),vec(data['carousel']),vec(data['canyon'][0]),vec(data['canyon'][1])))
OUT.write_text('\n'.join(f))
print('OSM area m2',round(shell.area),'panels',len(panels),'zones',len(zones),'triangles',sum(len(p['t'])//3 for p in panels),'fixtures',sum(len(z['fixtures']) for z in zones),'escalators',len(esc));print('Extents north/east',[(min(p[k] for p in data['shell']),max(p[k] for p in data['shell'])) for k in [0,1]])

