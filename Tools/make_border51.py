"""Original Canadian sentinel and living conifer assets, metres, separate articulated limbs."""
import sys,math,json,random
from pathlib import Path
import bpy
sys.path.insert(0,str(Path(__file__).parent));import make_models_v2 as m
m.OUT=m.ROOT/'ArtSource/ModelsV51';m.PALETTE.update({'CanadaArmor51':(.62,.67,.64),'CanadaRed51':(.48,.035,.035),'CanadaLeaf51':(.06,.22,.085)});m.setup()
def box(p,d,mat='CanadaArmor51',b=.025):m.box(p,d,mat,b)
def tube(a,b,r=.05,mat='Steel'):m.cyl(a,b,r,mat,n=16)
def end(n):m.finish(n,collision='convex')
# A heavy, bevelled chest shell with layered plating, exposed backpack cooling and red insignia.
box((0,0,0),(1.0,1.9,1.05),b=.14)
for y in [-.57,.57]:
 box((.54,y,.1),(.16,.65,.72),b=.07);box((.64,y,.12),(.018,.4,.11),'CanadaRed51')
 for z in [-.3,-.16,0,.16,.3]:box((-.53,y,z),(.1,.44,.035),'Rubber',.006)
 tube((-.42,y,-.6),(-.42,y,.5),.1)
leaf=[(0,.34),(.065,.18),(.16,.22),(.12,.08),(.30,.12),(.23,-.02),(.29,-.06),(.065,-.13),(.025,-.30),(-.025,-.30),(-.065,-.13),(-.29,-.06),(-.23,-.02),(-.30,.12),(-.12,.08),(-.16,.22),(-.065,.18)]
m.mesh('maple',[(.67,y,z+.10) for y,z in leaf],[tuple(range(len(leaf)))],'CanadaRed51');end('SentinelTorso51')
box((0,0,0),(.7,.86,.50),b=.11);box((.36,0,.025),(.035,.70,.16),'Rubber');box((.39,0,.025),(.02,.58,.045),'Glass')
tube((-.15,.42,.1),(-.15,.42,.9),.018);end('SentinelHead51')
box((0,0,0),(.72,1.18,.52),b=.10)
for y in [-.46,.46]:box((.40,y,-.15),(.14,.36,.6),b=.04)
end('SentinelPelvis51')
# Each arm is a turret cannon: breech, twin hydraulic struts, barrel shroud, six bored muzzles.
box((0,0,-.35),(.65,.58,.85),b=.10)
for y in [-.32,.32]:tube((-.15,y,.15),(.08,y,-.7),.05)
box((.32,0,-.65),(.90,.58,.56),'Steel',.06)
for i in range(6):
 a=i*math.tau/6;y=math.cos(a)*.19;z=-.65+math.sin(a)*.19
 tube((.45,y,z),(1.52,y,z),.075);tube((1.52,y,z),(1.535,y,z),.055,'Rubber')
for x in [.55,1.12]:box((x,0,-.65),(.09,.62,.58),'CanadaArmor51',.025)
box((0,-.303,-.25),(.35,.014,.14),'CanadaRed51');end('SentinelArm51')
box((0,0,-.46),(.62,.60,1.0),b=.10)
for y in [-.33,.33]:tube((-.12,y,-.05),(-.12,y,-.95),.055)
tube((0,-.38,-.9),(0,.38,-.9),.17)
box((.06,0,-1.25),(.51,.53,.65),b=.06);box((.28,0,-1.65),(1.05,.74,.25),'Rubber',.04)
box((.32,0,-1.27),(.06,.4,.52));end('SentinelLeg51')
# Wearable guard helmet and chest harness, +X forward with head/torso local pivots.
box((-.01,0,.025),(.25,.24,.17),b=.055);box((.13,0,.005),(.09,.27,.027),'CanadaArmor51',.009)
for y in [-.113,.113]:box((0,y,-.035),(.13,.035,.11),'Rubber',.014)
box((.13,0,-.027),(.016,.2,.055),'Glass',.009);end('GuardHelmet51')
box((.025,0,-.06),(.29,.37,.38),'CanadaArmor51',.045)
for y in [-.12,0,.12]:box((.18,y,-.12),(.055,.095,.14),'Cloth',.012)
end('GuardVest51')
# Irregular layered branch fans, one textured/coloured canopy mesh per tree, not stacked primitives.
random.seed(51)
tube((0,0,0),(.05,0,8),.15,'Wood')
for layer in range(13):
 z=1.5+layer*.49;r=(8.4-z)*.38
 for j in range(7):
  a=j*math.tau/7+layer*.7;tip=(math.cos(a)*r,math.sin(a)*r,z-.2)
  tube((0,0,z+.15),tip,.025,'Wood')
  verts=[(0,0,z+.4),(math.cos(a-.3)*r*.7,math.sin(a-.3)*r*.7,z-.3),tip,(math.cos(a+.3)*r*.7,math.sin(a+.3)*r*.7,z-.3)]
  mesh=bpy.data.meshes.new('needles');mesh.from_pydata(verts,[],[(0,1,2),(0,2,3),(2,1,0),(3,2,0)]);mesh.update();o=bpy.data.objects.new('branch',mesh);bpy.context.collection.objects.link(o);m.add(o,'CanadaLeaf51')
# Closed, irregular crowns fill the branch fans with a dense evergreen silhouette.
for tier in range(6):
 base=1.7+tier*.9;radius=(8.5-base)*.34;verts=[]
 for ring,(dz,factor) in enumerate([(0,1),(.38,1.03),(1.8,.08)]):
  for j in range(16):
   a=j*math.tau/16;rad=radius*factor*(1+random.uniform(-.13,.13));verts.append((math.cos(a)*rad,math.sin(a)*rad,base+dz+random.uniform(-.1,.1)))
 faces=[]
 for ring in range(2):
  for j in range(16):faces.append((ring*16+j,ring*16+(j+1)%16,(ring+1)*16+(j+1)%16,(ring+1)*16+j))
 m.mesh('crown',verts,faces,'CanadaLeaf51')
end('LivingPine51')
m.export_all();(m.OUT/'models_v51_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'Border51.blend'))
