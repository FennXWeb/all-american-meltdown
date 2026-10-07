"""Original landmark detail meshes, in metres; no downloaded geometry."""
import sys,math,json
from pathlib import Path
import bpy
sys.path.insert(0,str(Path(__file__).parent));import make_models_v2 as m
m.OUT=m.ROOT/'ArtSource/Syracuse73/Models';m.setup()
def end(n):m.finish(n,collision='convex')
# Molded, curved seat shells use connected lofted surfaces, not primitive seats.
for name,mat in [('TheatreSeat73','Red'),('ArenaSeat73','Cloth')]:
 for which in [0,1]:
  verts=[];faces=[]
  for j in range(9):
   for i in range(11):
    u=(i/10-.5)*.46;t=j/8
    if which:verts.append((u,.19+t*.08,.43+t*.55-.035*(u/.23)**2))
    else:verts.append((u,-.22+t*.44,.43+.035*(u/.23)**2-.03*math.sin(t*math.pi)))
  for j in range(8):
   for i in range(10):a=j*11+i;faces.append((a,a+1,a+12,a+11))
  o=m.mesh('curved upholstery',verts,faces,mat);mod=o.modifiers.new('shell thickness','SOLIDIFY');mod.thickness=.035;bpy.context.view_layer.objects.active=o;bpy.ops.object.modifier_apply(modifier=mod.name)
 for x in [-.27,.27]:
  m.cyl((x,.1,.03),(x,.1,.65),.025,'Steel',n=8);m.box((x,-.03,.65),(.065,.38,.045),'Wood',.015)
  m.box((x,0,.025),(.075,.4,.045),'Steel',.012)
 end(name)
# Art Deco angular winged Spirit of Light silhouette, bespoke extruded profile.
outline=[(-.15,0),(-.38,.7),(-.26,1.3),(-.6,1.55),(-1.5,2.0),(-2.2,3.1),(-1.85,2.95),(-1.45,2.65),(-1.8,3.35),(-1.05,2.88),(-1.3,3.6),(-.65,3.0),(-.25,2.1),(-.2,2.55),(0,2.7),(.2,2.55),(.25,2.1),(.65,3),(1.3,3.6),(1.05,2.88),(1.8,3.35),(1.45,2.65),(1.85,2.95),(2.2,3.1),(1.5,2),(.6,1.55),(.26,1.3),(.38,.7),(.15,0)]
verts=[(x,y,z) for y in [-.14,.14] for x,z in outline];n=len(outline);faces=[tuple(range(n-1,-1,-1)),tuple(range(n,2*n))]+[(i,(i+1)%n,(i+1)%n+n,i+n) for i in range(n)];m.mesh('winged sculpture',verts,faces,'Steel');end('SpiritLight73')
# Fluted theatre pilaster with custom capital and segmented arched pediment.
for x in [-.15,0,.15]:m.cyl((x,0,.18),(x,0,2.7),.07,'Bone',n=12)
for z,w in [(0,.65),(.12,.56),(2.7,.58),(2.83,.70)]:m.box((0,0,z+.06),(w,.26,.12),'Bone',.02)
for i in range(16):
 a=i*math.pi/16;b=(i+1)*math.pi/16;m.cyl((.24*math.cos(a),0,2.9+.24*math.sin(a)),(.24*math.cos(b),0,2.9+.24*math.sin(b)),.045,'Bone',n=8)
end('PalacePilaster73')
# A freestanding scalloped ticket booth with canopy, ticket window and sculpted sill.
m.box((0,0,.55),(1.45,1.2,1.1),'Wood',.04)
for x in [-.7,.7]:
 for y in [-.55,.55]:m.cyl((x,y,1.1),(x,y,2.25),.045,'Steel',n=8)
m.box((0,.52,1.7),(1.3,.035,1),'Glass');m.box((0,-.62,1.05),(1.7,.35,.08),'Wood',.04)
for i in range(14):
 x=-.9+i*.13;m.box((x,0,2.30),(.13,1.65,.13),'Red' if i%2 else 'Bone');m.cyl((x,-.8,2.14),(x+.13,-.8,2.14),.12,'Red' if i%2 else 'Bone',n=10)
end('TicketBooth73')
m.export_all();(m.OUT/'manifest.json').write_text(json.dumps(list(m.RECORDS.values()),indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'Syracuse73.blend'))
