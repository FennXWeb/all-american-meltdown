"""Bake OSM Hancock airport outlines to metre-accurate local coordinates (north +X)."""
import sys,json,xml.etree.ElementTree as ET
from pathlib import Path
root=Path(__file__).resolve().parents[1];sys.path.insert(0,str(root/'Saved/Tools71'))
from shapely.geometry import Polygon,Point,box,LineString
from shapely.ops import unary_union,triangulate
tree=ET.parse(root/'ContentSource/Geography84/syr_airport.osm').getroot()
nodes={n.attrib['id']:((float(n.attrib['lat'])-43.1133)*11110000,(float(n.attrib['lon'])+76.1112)*8130000) for n in tree.findall('node')}
buildings=[];taxi=[];aprons=[]
for w in tree.findall('way'):
 tags={t.attrib['k']:t.attrib['v'] for t in w.findall('tag')};points=[nodes[n.attrib['ref']] for n in w.findall('nd') if n.attrib['ref'] in nodes]
 if len(points)<2:continue
 if w.attrib['id'] in ['87080914','159452639','159452640','159452641','159452642']:buildings.append(Polygon(points).buffer(0))
 if tags.get('aeroway')=='taxiway':taxi.append(points)
 if tags.get('aeroway')=='apron' and len(points)>3:aprons.append(Polygon(points).buffer(0))
terminal=unary_union(buildings).simplify(35,preserve_topology=True)
polys=list(terminal.geoms) if hasattr(terminal,'geoms') else [terminal]
out=['// OpenStreetMap contributors, ODbL. Baked by Tools/bake_airport84.py.','namespace AirportData84 {']
def points(name,pts):out.append('static const FVector2D '+name+'[]={'+','.join('{%.1f,%.1f}'%p for p in pts)+'};')
edges=[];floors=[]
for p in polys:
 pts=list(p.exterior.coords);edges.extend(zip(pts,pts[1:]))
 for t in triangulate(p):
  if p.covers(t.representative_point()):floors.extend(list(t.exterior.coords)[:3])
points('Floor',floors);points('Walls',[v for e in edges for v in e]);points('Taxi',[v for p in taxi for e in zip(p,p[1:]) for v in e])
stairs=[(-7300,-4300),(4000,-2000),(17100,-4800)]
stairs=[p for p in stairs if terminal.covers(box(p[0]-80,p[1]-80,p[0]+1100,p[1]+340))]
upper=terminal.difference(unary_union([box(x,y,x+1000,y+260) for x,y in stairs]));upper_tri=[]
for t in triangulate(upper):
 if upper.covers(t.representative_point()):upper_tri.extend(list(t.exterior.coords)[:3])
points('UpperFloor',upper_tri);points('Stairs',stairs)
zones=[]
def zone(x,y,kind,yaw=0):
 if terminal.covers(Point(x,y).buffer(450)) and not any(box(sx-400,sy-400,sx+1400,sy+660).contains(Point(x,y)) for sx,sy in stairs):zones.append((x,y,kind,yaw))
# Lower-level check-in counters and baggage, upper concourses' gate lounges.
for x in range(-7600,1600,1100):zone(x,-4000,0)
for x in range(12600,19500,1100):zone(x,-4500,0)
for x,y in [(1200,-2200),(10500,-2200),(7000,-3800)]:zone(x,y,1)
for xlo,xhi,sign in [(-7000,1500,1),(14500,21500,-1)]:
 for y in range(1700,13300,1400):
  section=terminal.intersection(LineString([(xlo,y),(xhi,y)]))
  lines=list(section.geoms) if hasattr(section,'geoms') else [section]
  lines=[l for l in lines if l.geom_type=='LineString' and not l.is_empty and l.length>900]
  if lines:
   q=max(lines,key=lambda l:l.length).interpolate(.5,normalized=True);zone(q.x,q.y,2,sign*90)
for x in [3000,5500,8000]:zone(x,-1500,3)
out.append('struct FZone {FVector2D At;int Kind;float Yaw;};')
out.append('static const FZone Zones[]={'+','.join('{{%.1f,%.1f},%d,%.1f}'%z for z in zones)+'};')
out.append('}');(root/'Source/LethalWorld/LWAirportData84.inl').write_text('\n'.join(out))
(root/'ContentSource/Geography84/airport84.json').write_text(json.dumps({'terminal':[list(p.exterior.coords) for p in polys],'taxiways':taxi,'aprons':[list(p.exterior.coords) for p in aprons]}))
print('AIRPORT84',len(edges),'wall segments',len(floors)//3,'floor triangles',len(taxi),'taxiways')
