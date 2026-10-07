"""Offline geography import. OSM town streets (ODbL) + Natural Earth regional roads/water/boundaries (public domain).
Raw downloads live in ContentSource/Geography84. Runtime never downloads maps.
"""
import json,math,sys,xml.etree.ElementTree as ET
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'Saved/Tools71'))
from shapely.geometry import shape,box,LineString,Polygon,Point
from shapely.ops import transform,nearest_points,unary_union
from shapely.strtree import STRtree
P=ROOT/'ContentSource/Geography84';C=((43.0481-42.8864)*1111000,(-76.1474+78.8784)*813000)
def project(lat,lon):
 x=(lat-42.8864)*1111000;y=(lon+78.8784)*813000;dx=x-C[0];dy=y-C[1];r=math.hypot(dx,dy);f=r*10 if r<=90000 else 900000+(r-90000)*200000/1010000 if r<1100000 else r
 return (C[0]+dx*f/r,C[1]+dy*f/r) if r else C
bounds=box(-80.3,42.6,-74.25,45.3)
def shapes(g):
 if g.is_empty:return []
 return list(g.geoms) if hasattr(g,'geoms') else [g]
def pts(coords):
 out=[]
 for a,b in zip(coords,coords[1:]):
  n=max(1,math.ceil(math.dist(a,b)/.007))
  out += [project(a[1]+(b[1]-a[1])*i/n,a[0]+(b[0]-a[0])*i/n) for i in range(n)]
 out.append(project(coords[-1][1],coords[-1][0]));return out
def esc(s):return 'TEXT('+json.dumps(s,ensure_ascii=True)+')'
def vec(p):return '{%.2f,%.2f}'%tuple(p)
water=[];countries=[];border=[]
for f in json.loads((P/'natural_lakes.json').read_text('utf8'))['features']:
 name=f['properties'].get('name') or ''
 if name not in ['Lake Ontario','Lake Erie','Oneida Lake','Seneca Lake','Cayuga Lake']:continue
 for g in shapes(shape(f['geometry']).intersection(bounds)):
  if g.geom_type=='Polygon':water.append((name,Polygon(pts(list(g.exterior.coords))).simplify(130).exterior.coords[:]))
# River width is compressed for traversal; its course follows the source data.
for f in json.loads((P/'natural_rivers.json').read_text('utf8'))['features']:
 if f['properties'].get('name')!='St. Lawrence':continue
 for line in shapes(shape(f['geometry']).intersection(bounds)):
  if line.geom_type!='LineString':continue
  for poly in shapes(LineString(pts(list(line.coords))).buffer(4800,cap_style=2).simplify(100)):
   if poly.geom_type=='Polygon':water.append(('St. Lawrence River',list(poly.exterior.coords)))
for f in json.loads((P/'countries.json').read_text('utf8'))['features']:
 if f['properties']['ADMIN']!='Canada':continue
 ca=shape(f['geometry']);g=ca.intersection(bounds)
 for poly in shapes(g):
  if poly.geom_type=='Polygon':countries.append(Polygon(pts(list(poly.exterior.coords))).simplify(90).exterior.coords[:])
 # Only the true international boundary, never edges introduced by our crop.
 for l in shapes(ca.boundary.intersection(bounds.buffer(-.002))):
  if l.geom_type=='LineString':border.extend(zip(pts(list(l.coords)),pts(list(l.coords))[1:]))
roads=[];names={};seen=set()
def road(a,b,w,h,name,osm=False):
 # Short surveyed chords often contain a junction. Dropping them or quantizing
 # parallel roads to a broad cell leaves invisible holes in the driving graph.
 if math.dist(a,b)<.05:return
 key=tuple(sorted((tuple(round(v,2) for v in a),tuple(round(v,2) for v in b))))
 if key in seen:return
 seen.add(key);names.setdefault(name,len(names));roads.append((a,b,w,h,names[name],osm))
for f in json.loads((P/'natural_roads.json').read_text('utf8'))['features']:
 t=f['properties'];g=shape(f['geometry'])
 if t.get('sov_a3') not in ['USA','CAN'] or not g.intersects(bounds):continue
 for l in shapes(g.intersection(bounds)):
  if l.geom_type!='LineString':continue
  h=bool(t.get('expressway')) or t.get('type')=='Major Highway';w=1680 if h else 750
  ps=LineString(pts(list(l.coords))).simplify(70).coords[:];name=str(t.get('name') or t.get('label') or ('REGIONAL HIGHWAY' if h else 'COUNTY ROAD'))
  for a,b in zip(ps,ps[1:]):road(a,b,w,h,name)
regional=len(roads)
for file in sorted(P.glob('*.osm')):
 if file.stem=='syr_airport':continue
 r=ET.parse(file).getroot();nodes={n.get('id'):(float(n.get('lon')),float(n.get('lat'))) for n in r.findall('node')}
 for e in r.findall('way'):
  t={t.get('k'):t.get('v') for t in e.findall('tag')};kind=t.get('highway','')
  if kind not in ['motorway','motorway_link','trunk','primary','secondary','tertiary','residential','unclassified']:continue
  if t.get('access')=='private':continue
  ps=[nodes[n.get('ref')] for n in e.findall('nd') if n.get('ref') in nodes]
  if len(ps)<2:continue
  h=kind in ['motorway','trunk'];w=1680 if h else 850 if kind in ['primary','secondary'] else 500
  name=t.get('name',t.get('ref',file.stem.replace('_',' ').upper()+' ROAD'))
  ps=[project(lat,lon) for lon,lat in ps]
  for a,b in zip(ps,ps[1:]):road(a,b,w,h,name,True)
# Repair disconnected town extracts against the regional network, by component
# rather than by grid cell. These short, visible approaches are gameplay joins,
# not claims that a surveyed street exists there. Never bridge water or borders.
lines=[LineString([r[0],r[1]]) for r in roads];tree=STRtree(lines)
parents=list(range(len(lines)))
def root(i):
 while parents[i]!=i:parents[i]=parents[parents[i]];i=parents[i]
 return i
def union(a,b):
 a,b=root(a),root(b)
 if a!=b:parents[b]=a
for i,l in enumerate(lines):
 for j in tree.query(l,predicate='dwithin',distance=.1):
  if j>i:union(i,int(j))
 for p in [Point(l.coords[0]),Point(l.coords[-1])]:
  for j in tree.query(p,predicate='dwithin',distance=64):union(i,int(j))
groups={}
for i in range(len(lines)):groups.setdefault(root(i),[]).append(i)
main=max(groups,key=lambda k:len(groups[k]));connected=set(groups.pop(main))
wet=unary_union([Polygon(p) for n,p in water]);canadian=unary_union([Polygon(p) for p in countries]);joins=[]
for ids in sorted(groups.values(),key=len,reverse=True):
 if len(ids)<3:continue
 target=[lines[i] for i in sorted(connected)];mt=STRtree(target);options=[]
 for i in ids:
  for xy in lines[i].coords:
   a=Point(xy);l=target[int(mt.nearest(a))];b=nearest_points(a,l)[1];d=a.distance(b)
   if d>100000 or d<.1:continue
   options.append((d,xy,tuple(b.coords[0])))
 for d,a,b in sorted(options):
  l=LineString([a,b])
  if wet.intersects(l) or canadian.contains(Point(a))!=canadian.contains(Point(b)):continue
  road(a,b,650,False,'REGIONAL APPROACH',False);joins.append((a,b));connected.update(ids);break
print('connected town approaches',len(joins),flush=True)
# Keep the source survey and polyline boundaries inspectable outside C++.
(P/'derived.json').write_text(json.dumps(dict(roads=roads,names=list(names),water=[(n,list(p)) for n,p in water],canada=countries,border=border)),encoding='utf8')
f=ROOT/'Source/LethalWorld/LWGeographyData84.inl'
with f.open('w',encoding='utf8') as o:
 o.write('// Generated by Tools/bake_geography84.py; see Docs/Geography84-Sources.md.\n')
 o.write('static const TCHAR* const RoadNames84[]={'+','.join(esc(n) for n in names)+'};\n')
 o.write('static const FMapRoad84 MapRoads84[]={\n')
 for a,b,w,h,n,osm in roads:o.write('{%s,%s,%d,%s,%d,%s},\n'%(vec(a),vec(b),w,str(h).lower(),n,str(osm).lower()))
 o.write('};\n')
 for label,polys in [('Water',[(n,p) for n,p in water]),('Canada',[('Canada',p) for p in countries])]:
  for i,(n,ps) in enumerate(polys):o.write('static const FPoint84 %sPolygon%d[]={%s};\n'%(label,i,','.join(vec(p) for p in ps)))
  o.write('static const FPolygon84 %sPolygons84[]={\n'%label)
  for i,(n,ps) in enumerate(polys):o.write('{%s,%sPolygon%d,UE_ARRAY_COUNT(%sPolygon%d)},\n'%(esc(n),label,i,label,i))
  o.write('};\n')
 o.write('static const FMapRoad84 BorderLines84[]={\n')
 for a,b in border:o.write('{%s,%s,0,false,0,false},\n'%(vec(a),vec(b)))
 o.write('};\n')
print('regional chords',regional,'total',len(roads),'names',len(names),'lakes',len(water),'Canada polygons',len(countries),'border edges',len(border))
