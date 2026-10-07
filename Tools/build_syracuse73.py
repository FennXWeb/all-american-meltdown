"""Bake OSM geometry to offline UE tables; ODbL attribution in Docs/Syracuse73.md.
Requires shapely (the existing Saved/Tools71 dependency directory is supported).
No network access at runtime. Geographic axes: +X north, +Y east, centimetres.
"""
import json, math, sys, xml.etree.ElementTree as ET
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'Saved/Tools71'))
from shapely.geometry import Polygon, Point, LineString, box
from shapely.ops import linemerge, polygonize, unary_union
from shapely.strtree import STRtree
REF=ROOT/'ContentSource/Syracuse73'
features={e['id']:e for e in json.loads((REF/'osm.json').read_text(encoding='utf8'))['elements'] if e['type']=='way'}
def xml(file):
 r=ET.parse(REF/file).getroot();ns={n.get('id'):{'lon':float(n.get('lon')),'lat':float(n.get('lat'))} for n in r.findall('node')};out=[]
 for w in r.findall('way'):
  out.append(dict(id=int(w.get('id')),tags={t.get('k'):t.get('v') for t in w.findall('tag')},geometry=[ns[n.get('ref')] for n in w.findall('nd') if n.get('ref') in ns]))
 return out
for f in xml('palace.osm')+xml('fair.osm')+xml('clinton.osm')+xml('clinton-west.osm'):features[f['id']]=f
C=((43.0481-42.8864)*1111000,(-76.1474+78.8784)*813000)
def project(lat,lon):
 p=((lat-42.8864)*1111000,(lon+78.8784)*813000);d=(p[0]-C[0],p[1]-C[1]);r=math.hypot(*d)
 f=10*r if r<=90000 else 900000+(r-90000)*200000/1010000 if r<1100000 else r
 return (C[0]+d[0]*f/r,C[1]+d[1]*f/r) if r else C
def inside(p):
 d=(p[0]-C[0],p[1]-C[1]);r=math.hypot(*d);f=r/10 if r<=900000 else 90000+(r-900000)*1010000/200000 if r<1100000 else r
 b=(C[0]+d[0]*f/r,C[1]+d[1]*f/r) if r else C
 lat=b[0]/1111000+42.8864;lon=b[1]/813000-78.8784
 return 42.995<lat<43.145 and -76.265<lon<-76.065

def points(e):return [project(p['lat'],p['lon']) for p in e.get('geometry',[])]
def polygon(e):return Polygon(points(e)).buffer(0)
def q(s):return 'TEXT('+json.dumps(s,ensure_ascii=True)+')'
def v(p):return '{%.2f,%.2f}'%tuple(p)
def rect(poly):
 r=list(poly.minimum_rotated_rectangle.exterior.coords);a,b=r[:2];w=math.dist(a,b);h=math.dist(b,r[2]);angle=math.degrees(math.atan2(b[1]-a[1],b[0]-a[0]));c=poly.minimum_rotated_rectangle.centroid
 return (c.x,c.y),w,h,angle
# Actual shoreline assembled from the relation's member ways.
lines=[LineString(points(e)) for e in xml('lake.osm') if len(points(e))>1]
lake=max(polygonize(unary_union(lines)),key=lambda p:p.area).simplify(350,preserve_topology=True)
landmarks=[(70,165192338,'NEW YORK STATE FAIRGROUNDS'),(71,186712423,'NIAGARA MOHAWK BUILDING'),(72,159768129,'THE PALACE THEATRE'),(73,156759790,'SYRACUSE UNIVERSITY / HALL OF LANGUAGES'),(74,97100885,'CARRIER DOME'),(76,200830231,'ONCENTER WAR MEMORIAL'),(77,577315668,'EMPOWER FCU AMPHITHEATER AT LAKEVIEW')]
records=[];reserved=[];specialids={i for t,i,n in landmarks}
for t,i,n in landmarks:
 poly=polygon(features[i]);c,w,h,a=rect(poly)
 # Local building forward points toward its public entrance. Canonical theatre X is width.
 if t==72 and w>h:w,h=h,w;a+=90
 if t in (74,76) and w<h:w,h=h,w;a+=90
 if t==72:a=90 # James Street entrance faces north; rear extends south.
 if t==71:a=-90;w=6300;h=2700 # Erie Boulevard is south of the mapped office block.
 if t==75:a=0
 if t==70: c=project(43.07364,-76.22223);w=140000;h=140000;a=0
 if t==73:w=5800;h=3200;a=90
 if t==77:w=16000;h=17000;a=-46
 records.append((t,i,c,w,h,a,n));reserved.append(poly.buffer(600) if t!=77 else Point(c).buffer(13000))
c=project(43.0509,-76.15291);records.append((75,730075,c,6500,8000,0,'CLINTON SQUARE'));reserved.append(box(c[0]-3250,c[1]-4000,c[0]+3250,c[1]+4000))
# Lake is a discoverable shoreline destination, not a building across the water.
c=project(43.0994,-76.2043);records.append((78,730078,c,1200,1200,0,'ONONDAGA LAKE'));reserved.append(Point(c).buffer(900))
# Protect the full-scale mall and airport from ordinary parcels.
for lat,lon,w,h in [(43.071,-76.171,100000,65000),(43.15,-76.11,80000,80000)]:
 c=project(lat,lon);reserved.append(box(c[0]-w/2,c[1]-h/2,c[0]+w/2,c[1]+h/2))
roads=[];names=[];nameids={};streetlines=[]
allowed={'motorway':1200,'motorway_link':700,'trunk':1100,'trunk_link':650,'primary':1050,'primary_link':600,'secondary':900,'secondary_link':550,'tertiary':800,'tertiary_link':500,'residential':650,'unclassified':650,'living_street':500}
for e in features.values():
 t=e.get('tags',{});kind=t.get('highway');ps=points(e)
 fairpath=kind in ('service','pedestrian') and ps and math.dist(ps[0],project(43.07364,-76.22223))<65000
 if (kind not in allowed and not fairpath) or len(ps)<2:continue
 # Preserve every junction vertex: simplification would disconnect feeder streets.
 name=t.get('name',t.get('ref','ACCESS ROAD'));ni=nameids.setdefault(name,len(names))
 if ni==len(names):names.append(name)
 width=allowed.get(kind,400);high=kind.startswith('motorway') or kind.startswith('trunk')
 for a,b in zip(ps,ps[1:]):
  if math.dist(a,b)<50 or not inside(((a[0]+b[0])/2,(a[1]+b[1])/2)):continue
  roads.append((a,b,width,high,ni));streetlines.append(LineString([a,b]))
tree=STRtree(streetlines);roadshapes=[l.buffer(r[2]/2+200,cap_style=2) for l,r in zip(streetlines,roads)]
obstacles=STRtree(roadshapes+reserved+[lake]);placed=[];bldgs=[]
def district(lat,lon,t):
 if t.get('building')=='university' or 43.029<lat<43.043 and -76.14<lon<-76.122:return 5,'UNIVERSITY HILL'
 if 43.043<lat<43.056 and -76.159<lon<-76.142:return 4,'DOWNTOWN'
 if lon<-76.18:return (1 if t.get('building') in ('house','residential','apartments','detached','semidetached_house') else 3),'SOLVAY / WESTSIDE'
 if lat>43.066 and lon>-76.135:return 1,'EASTWOOD'
 if lat>43.054 and lon<-76.151:return 3,'LAKEFRONT / FRANKLIN SQUARE'
 if lat>43.059:return 1,'NORTHSIDE'
 if lon<-76.16:return 1,'WESTSIDE'
 if lat<43.036:return 1,'SOUTHSIDE'
 return 2,'EASTSIDE'
for e in sorted(features.values(),key=lambda e:e['id']):
 t=e.get('tags',{});ps=points(e)
 if 'building' not in t or e['id'] in specialids or len(ps)<4:continue
 poly=Polygon(ps).buffer(0)
 if poly.is_empty or poly.geom_type!='Polygon' or poly.area<900000:continue
 c,w,h,a=rect(poly);lat=sum(p['lat'] for p in e['geometry'])/len(ps);lon=sum(p['lon'] for p in e['geometry'])/len(ps);d,area=district(lat,lon,t)
 # Full mapped central blocks and named buildings; thin repetitive suburban footprints.
 # Retain every surveyed footprint, including ordinary residential frontage.
 if any(poly.intersects(obstacles.geometries[j]) for j in obstacles.query(poly)):continue
 try:floors=int(t.get('building:levels',2 if d==1 else 3 if d!=4 else 5))
 except:floors=3
 floors=max(1,min(floors,18));name=t.get('name',area+' / '+t.get('addr:street','BUILDING'))
 bldgs.append((e['id'],c,w,h,a,d,floors,name,ps[:-1]));placed.append(poly)
# Reuse existing playable houses/shops on roads outside the footprint survey, respecting actual street frontage.
allblocks=STRtree(roadshapes+reserved+[lake]+placed);fill=[];occupied={}
parcel_sizes={7:(2000,1800),8:(2400,2200),9:(3200,2200),4:(2400,2000),5:(2600,2200),14:(2400,2000),18:(3600,3000),13:(4400,3600)}
for e in sorted(features.values(),key=lambda e:e['id']):
 tags=e.get('tags',{});roadkind=tags.get('highway');ps=points(e)
 if roadkind not in ('residential','unclassified','living_street','tertiary','secondary') or len(ps)<2:continue
 line=LineString(ps);width=allowed[roadkind];spacing=3800 if roadkind in ('residential','living_street') else 5100
 for sample in range(int(line.length//spacing)):
  d=(sample+.5)*spacing;mid=line.interpolate(d);before=line.interpolate(max(0,d-100));after=line.interpolate(min(line.length,d+100));dx,dy=after.x-before.x,after.y-before.y;length=math.hypot(dx,dy)
  if length<1 or not inside((mid.x,mid.y)):continue
  normal=(-dy/length,dx/length)
  for side in [-1,1]:
   kind=([7,8,9,7][(e['id']+sample)%4] if roadkind in ('residential','living_street','unclassified') else [4,14,5,18,9][(e['id']+sample)%5]);w,h=parcel_sizes[kind];radius=math.hypot(w,h)/2+180
   c=(mid.x+side*normal[0]*(width/2+h/2+850),mid.y+side*normal[1]*(width/2+h/2+850));cell=(int(c[0]//4000),int(c[1]//4000))
   if any(math.dist(c,old)<radius+r for cx in range(cell[0]-2,cell[0]+3) for cy in range(cell[1]-2,cell[1]+3) for old,r in occupied.get((cx,cy),[])):continue
   poly=Point(c).buffer(radius)
   if any(poly.intersects(allblocks.geometries[j]) for j in allblocks.query(poly)):continue
   if math.dist(c,project(43.038,-76.134))<45000 or math.dist(c,project(43.07364,-76.22223))<90000:continue
   occupied.setdefault(cell,[]).append((c,radius));yaw=math.degrees(math.atan2(side*normal[1],side*normal[0]))-90
   fill.append((0xC7300000+len(fill),c,kind,yaw,tags.get('name','SYRACUSE')))
fair=[]
fair_boundary=polygon(features[165192338])
for e in xml('fair.osm'):
 if 'building' not in e['tags'] or len(points(e))<4:continue
 p=polygon(e)
 if p.is_empty or p.geom_type!='Polygon' or p.area<1000000 or not fair_boundary.covers(p.centroid):continue
 c,w,h,a=rect(p);fair.append((c,w,h,a,e['tags'].get('name','EXHIBITION HALL')))
out=['// Generated from OpenStreetMap contributors, ODbL. See Docs/Syracuse73.md.']
out+=['static const TCHAR* const StreetNames73[]={'+','.join(q(n) for n in names)+'};']
out+=['static const FRoad73 RawRoads73[]={']+[f'{{{v(a)},{v(b)},{w},{str(hi).lower()},{ni}}},' for a,b,w,hi,ni in roads]+['};']
out+=['static const FLandmark73 RawLandmarks73[]={']+[f'{{{t},{v(c)},{v((w,h))},{a:.3f},{q(n)}}},' for t,i,c,w,h,a,n in records]+['};']
out+=['static const FVector2D Lake73[]={'+','.join(v(a) for a in list(lake.exterior.coords)[:-1])+'};']
out+=['static const FBlock73 RawBlocks73[]={']+[f'{{{i}u,{v(c)},{v((w,h))},{a:.3f},{d},{fl},{q(n)}}},' for i,c,w,h,a,d,fl,n,ps in bldgs]+['};']
out+=['static const FFill73 RawFill73[]={']+[f'{{{i}u,{v(c)},{t},{a:.3f},{q(n)}}},' for i,c,t,a,n in fill]+['};']
out+=['static const FFair73 RawFair73[]={']+[f'{{{v(c)},{v((w,h))},{a:.3f},{q(n)}}},' for c,w,h,a,n in fair]+['};']
(ROOT/'Source/LethalWorld/LWSyracuseData73.inl').write_text('\n'.join(out),encoding='utf8')
stats=dict(roads=len(roads),street_names=len(names),mapped_buildings=len(bldgs),playable_parcels=len(fill),landmarks=len(records),fair_halls=len(fair),shoreline_vertices=len(lake.exterior.coords)-1)
(REF/'stats.json').write_text(json.dumps(stats,indent=2));print(stats)
# Lightweight overhead reference used to check correspondence and relative placement.
from PIL import Image,ImageDraw
im=Image.new('RGB',(1500,1500),'#172326');draw=ImageDraw.Draw(im)
def screen(p):return (750+(p[1]-C[1])/900,1000-(p[0]-C[0])/900)
draw.polygon([screen(p) for p in lake.exterior.coords],fill='#36596b')
for a,b,w,hi,n in roads:draw.line([screen(a),screen(b)],fill='#b29f69' if hi else '#56666a',width=2 if hi else 1)
for t,i,c,w,h,a,n in records:pt=screen(c);draw.ellipse((pt[0]-5,pt[1]-5,pt[0]+5,pt[1]+5),fill='#ffad55');draw.text((pt[0]+8,pt[1]),n,fill='white')
im.save(REF/'Layout73.png')
