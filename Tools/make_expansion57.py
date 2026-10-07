"""Authored contour meshes; metres, +X forward. No primitive vehicle hulls or creature bodies."""
import sys,math,json
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent))
import bpy
import make_models_v2 as m
m.OUT=m.ROOT/'ArtSource/ModelsV57'
for name,c in {'Armor57':(.22,.28,.16),'Fur57':(.25,.14,.065),'Chitin57':(.55,.37,.08),'Robot57':(.13,.15,.17)}.items():m.PALETTE[name]=c
m.setup()
atlas=bpy.data.images.load(str(m.ROOT/'ArtSource/TexturesV57/T_Expansion57.png'))
for name,offset in [('Armor57',(0,.5)),('Fur57',(.5,.5)),('Chitin57',(0,0)),('Robot57',(.5,0))]:
 mat=m.MATS[name];n=mat.node_tree.nodes;t=n.new('ShaderNodeTexImage');t.image=atlas
 uv=n.new('ShaderNodeTexCoord');v=n.new('ShaderNodeVectorMath');v.operation='MULTIPLY_ADD';v.inputs[1].default_value=(.49,.49,1);v.inputs[2].default_value=(*offset,0)
 mat.node_tree.links.new(uv.outputs['UV'],v.inputs[0]);mat.node_tree.links.new(v.outputs[0],t.inputs[0]);mat.node_tree.links.new(t.outputs[0],n.get('Principled BSDF').inputs['Base Color'])
def skin(name,rows,mat,n=24):
 # Cross sections (x, halfwidth, lowerz, upperz). Shared vertices give continuous silhouettes.
 verts=[]
 for x,w,lo,hi in rows:
  for i in range(n):
   a=math.tau*i/n;verts.append((x,w*math.cos(a),(lo+hi)/2+(hi-lo)/2*math.sin(a)))
 faces=[tuple(range(n-1,-1,-1)),tuple(range((len(rows)-1)*n,len(rows)*n))]
 for j in range(len(rows)-1):
  for i in range(n):faces.append((j*n+i,j*n+(i+1)%n,(j+1)*n+(i+1)%n,(j+1)*n+i))
 return m.mesh(name,verts,faces,mat)
def panel(name,points,thickness,mat):
 # Individually drawn surfaces, thickness along Z. Deliberately shaped glazing/body panels.
 v=points+[(x,y,z+thickness) for x,y,z in points];k=len(points)
 return m.mesh(name,v,[tuple(range(k-1,-1,-1)),tuple(range(k,2*k))]+[(i,(i+1)%k,(i+1)%k+k,i+k) for i in range(k)],mat)
def done(name,collision='none'):
 o=m.finish(name,collision=collision)
 # Preserve faceted panels, smooth organic contours without changing silhouettes.
 for p in o.data.polygons:
  if o.data.materials[p.material_index].name in ('M_Fur57','M_Chitin57'):p.use_smooth=True
 return o
def seat(x,y,z):
 skin('stitched bucket cushion',[(x-.3,.20,z,z+.09),(x-.2,.27,z-.02,z+.16),(x+.18,.25,z,z+.12),(x+.28,.18,z+.03,z+.08)],'Cloth',16)
 o=m.PARTS[-1];o.location.y=y
 m.loft([(x-.24,y,z+.08,.09,.24),(x-.29,y,z+.32,.095,.28),(x-.32,y,z+.62,.08,.25),(x-.31,y,z+.71,.06,.18)],'Cloth',16)
 m.loft([(x-.30,y,z+.72,.055,.14),(x-.31,y,z+.91,.065,.16),(x-.30,y,z+.97,.025,.12)],'Rubber',12)
def vehicle(kind,L,W,H):
 rear=2.2-L*2
 if kind=='technical':
  # Contoured hood, cab pillars, open pickup bed and wheel-arch side skins.
  skin('pressed hood',[(.3,.91,.85,1.05),(1.6,1.02,.72,.97),(2.2,.87,.60,.77)],'Armor57')
  roof=[(-1.3,-.91,1.92),(-1.3,.91,1.92),(-.20,.86,1.87),(-.2,-.86,1.87)]
  panel('cab roof',roof,.045,'Armor57')
  for side in [-1,1]:
   m.profile([(rear,.50),(rear,1.22),(-1.4,1.22),(-1.4,.5),(-1.7,.5),(-1.9,.88),(-2.4,.99),(-2.8,.83),(-3,.5)],.09,'Armor57',side*W)
   m.profile([(-1.35,.5),(-1.35,1.87),(-1.23,1.88),(-1.20,1.11),(-.28,1.09),(.34,1.05),(.34,.48)],.07,'Armor57',side*.91)
   m.line([(-.20,side*.86,1.89),(.34,side*.91,1.06)],.035,'Armor57')
  panel('cargo floor',[(rear,-W,.51),(rear,W,.51),(-1.35,W,.51),(-1.35,-W,.51)],.035,'Steel')
 else:
  top=H-.25
  # Chamfered armored hull shell, separate sides/roof leave a real accessible cabin volume.
  for side in [-1,1]:
   m.profile([(rear,.42),(rear+.12,top-.15),(rear+.55,top),(-.80,top),(.25,1.65),(1.20,1.3),(2.2,.82),(2.2,.48)],.075,'Armor57',side*W)
  panel('armored floor',[(rear,-W,.42),(rear,W,.42),(2.1,W,.42),(2.1,-W,.42)],.06,'Steel')
  panel('rear cabin roof',[(rear+.20,-W,top),(rear+.20,W,top),(-.78,W,top),(-.78,-W,top)],.06,'Armor57')
  panel('sloped bonnet',[(.45,-W,1.30),(.45,W,1.30),(2.2,W*.8,.85),(2.2,-W*.8,.85)],.06,'Armor57')
  m.profile([(rear,.45),(rear,top), (rear+.07,top),(rear+.07,.45)],W*2,'Armor57')
  for side in [-1,1]:
   m.line([(-.8,side*W,top),(.45,side*W,1.32)],.045,'Steel')
   for x in [rear+.4,rear+1,rear+1.6]:
    m.line([(x,side*(W+.045),.8),(x+.28,side*(W+.045),.8)],.018,'Steel')
   for x in [1.75,1.25]:m.tube((x,side*(W+.03),.77),(x,side*(W+.06),.77),.10,.075,'Steel')
 # Recessed side-door seams, armored window ports, grab bars and service grilles.
 if kind!='technical':
  for side in [-1,1]:
   yy=side*(W+.045)
   m.line([(-1.20,yy,.58),(-1.20,yy,top-.12),(-.85,yy,top-.08),(.36,yy,1.48),(.36,yy,.58),(-1.20,yy,.58)],.009,'Rubber')
   m.profile([(-1.06,1.42),(-1.06,top-.17),(-.89,top-.16),(-.32,1.66),(-.32,1.42)],.012,'Glass',yy)
   m.line([(-1.05,yy+side*.03,1.22),(-.79,yy+side*.03,1.22)],.016,'Steel')
   for x in [rear+.65,rear+1.8,rear+2.85]:
    m.line([(x,yy,.65),(x,yy,top-.16)],.007,'Rubber')
    for z in [.72,top-.23]:m.tube((x,yy,z),(x,yy+side*.02,z),.022,.009,'Steel',8)
   for j in range(10):m.line([(rear+.5+j*.07,yy,1.45),(rear+.5+j*.07,yy,1.75)],.012,'Rubber')
  # Armored front window aperture, hood seam and headlight bezels.
  m.line([(-.80,-W,top),(-.80,W,top),(.45,W,1.32),(.45,-W,1.32),(-.80,-W,top)],.026,'Steel')
  m.line([(-.80,0,top),(.45,0,1.32)],.026,'Steel')
  for side in [-1,1]:m.tube((2.15,side*W*.68,.72),(2.22,side*W*.68,.72),.11,.082,'Steel',20)
  m.line([(rear+.25,-W*.7,top+.06),(rear+.25,W*.7,top+.06),(rear+1.2,W*.7,top+.06),(rear+1.2,-W*.7,top+.06),(rear+.25,-W*.7,top+.06)],.023,'Steel')
 else:
  for side in [-1,1]:
   yy=side*.952;m.line([(-1.18,yy,.53),(-1.18,yy,1.80),(-.3,yy,1.76),(.18,yy,1.14),(.18,yy,.53),(-1.18,yy,.53)],.007,'Rubber')
   m.line([(-.85,yy,1.04),(-.60,yy,1.04)],.018,'Steel')
   m.line([(-.35,yy,1.40),(-.30,yy+side*.24,1.39)],.016,'Steel')
   m.profile([(-.42,1.29),(-.42,1.48),(-.19,1.48),(-.19,1.29)],.05,'Rubber',yy+side*.23)
   m.line([(rear+.1,side*(W+.02),1.24),(-1.40,side*(W+.02),1.24)],.018,'Steel')
  m.profile([(rear,.51),(rear,1.20),(rear+.06,1.20),(rear+.06,.51)],W*2,'Armor57')
 # Frame rails, sprung axles, tow shackles, radiator vanes, external steps.
 for side in [-1,1]:
  m.profile([(rear,.24),(rear,.34),(2.12,.34),(2.12,.24)],.085,'Steel',side*.62)
  for x in [rear+.24,2.05]:m.tube((x,side*.50,.39),(x+.03,side*.50,.39),.07,.042,'Steel')
  for i in range(3):m.tube((-.7,side*(W+.04),.46+i*.09),(-.25,side*(W+.04),.46+i*.09),.018,.011,'Steel')
 for i in range(13):m.profile([(2.18,.54+i*.015),(2.18,.547+i*.015),(2.22,.547+i*.015),(2.22,.54+i*.015)],W*1.25,'Rubber')
 done('Vehicle42_'+kind)
 # Cabin uses the same seat coordinates as runtime. Dashboard has a driver-facing contour.
 skin('padded dashboard',[(-.03,W*.85,.94,1.05),(.20,W*.88,.84,1.13),(.40,W*.86,.88,1.04)],'Rubber',16)
 seat(-.43,-.43,.45)
 if kind=='armoredtruck':seat(-.43,.43,.45)
 if kind=='technical':seat(-.60,.43,.45)
 if kind=='apc':
  for x in [-1.6,-2.6,-3.6]:
   for side in [-1,1]:seat(x,side*.63,.45)
 # Pedals, gear selector, switches are modelled, not decals on an empty box.
 for y in [-.55,-.40]:m.profile([(.2,.45),(.25,.56),(.35,.56),(.32,.44)],.08,'Steel',y)
 m.line([(-.35,0,.45),(-.18,0,.9)],.014,'Steel');skin('gear knob',[(-.22,.04,.87,.96),(-.15,.04,.87,.96)],'Rubber',12)
 done('Cabin42_'+kind)
 # Split glazing for visibility, retaining an actual windshield surface.
 z=1.92 if kind=='technical' else H-.25
 if kind=='technical':panel('laminated windshield',[(-.22,-.82,z-.035),(-.22,.82,z-.035),(.31,.87,1.10),(.31,-.87,1.10)],.008,'Glass')
 else:panel('laminated windshield',[(-.79,-W+.07,z-.035),(-.79,W-.07,z-.035),(.43,W-.07,1.35),(.43,-W+.07,1.35)],.008,'Glass')
 done('Windshield42_'+kind)
for k,L,W,H in [('apc',3.6,1.25,2.5),('armoredtruck',3.1,1.08,2.5),('technical',2.7,1.05,1.95)]:vehicle(k,L,W,H)
# Articulated turret body and trunnion barrel; muzzle axis +X.
skin('welded turret basket',[(-.75,.28,-.2,.35),(-.4,.66,-.3,.50),(.45,.62,-.25,.4),(.75,.28,-.12,.28)],'Armor57',16)
for side in [-1,1]:m.tube((0,side*.65,0),(0,side*.71,0),.18,.12,'Steel')
done('Turret57')
skin('cannon breech',[(-.45,.16,-.16,.16),(.35,.16,-.14,.14),(.65,.07,-.07,.07)],'Robot57',16)
m.tube((.35,0,0),(2.0,0,0),.065,.035,'Steel',20);m.tube((1.75,0,0),(2.05,0,0),.095,.045,'Robot57',16)
done('Cannon57')
m.profile([(-.4,-.1),(-.4,.12),(.3,.12),(.45,.04),(.45,-.06)],.14,'Robot57');m.tube((.3,0,.035),(1.28,0,.035),.03,.014,'Steel',16)
for x in [.4,.5,.6,.7,.8,.9]:m.tube((x,0,.035),(x+.03,0,.035),.045,.031,'Robot57',12)
m.line([(-.3,-.12,-.05),(-.4,-.13,-.2)],.024,'Steel');m.line([(-.3,.12,-.05),(-.4,.13,-.2)],.024,'Steel');done('MachineGun57')
# Helicopter fuselage: compound loft panels with a clear front canopy, tail boom, skids.
skin('aft fuselage',[(-3,.15,.85,1.65),(-2.3,.60,.55,2.10),(-1.4,1.05,.42,2.45),(-.6,1.1,.38,2.50)],'Armor57',32)
skin('lower cockpit',[(-.6,1.1,.38,1.12),(.7,.94,.45,1.13),(1.7,.70,.64,1.2),(2.2,.20,.86,1.24)],'Armor57',32)
panel('cockpit roof',[(-.6,-1.05,2.45),(-.6,1.05,2.45),(.72,.7,2.28),(.72,-.7,2.28)],.05,'Armor57')
for side in [-1,1]:
 m.line([(-.6,side*1.02,1.1),(-.6,side*1.02,2.44),(.72,side*.7,2.28),(2.08,side*.20,1.18)],.035,'Steel')
 m.line([(-2.4,side*1.3,.24),(1.6,side*1.3,.24),(1.9,side*1.3,.38)],.055,'Steel',12)
 for x in [-1.7,.75]:m.line([(x,side*.55,.6),(x,side*1.3,.24)],.042,'Steel',12)
skin('tapered tail boom',[(-6.6,.07,1.45,1.65),(-5,.17,1.20,1.65),(-3,.30,1.0,1.7),(-2.4,.4,.9,1.9)],'Armor57',20)
m.profile([(-6.4,1.4),(-6.7,3.0),(-6.25,2.85),(-5.65,1.45)],.09,'Armor57')
panel('tailplane',[(-5.9,-1.4,1.55),(-5.6,-1.3,1.55),(-5.3,1.3,1.55),(-5.6,1.4,1.55)],.04,'Armor57')
m.tube((-1,0,2.35),(-1,0,3.05),.09,.045,'Steel')
for side in [-1,1]:
 yy=side*1.07
 m.line([(-.57,yy,1.12),(-.57,yy,2.39),(.58,side*.74,2.24),(1.70,side*.43,1.23),(-.57,yy,1.12)],.014,'Steel')
 m.line([(-.43,yy,1.35),(-.18,yy,1.35)],.022,'Steel')
 for x in [-2.10,-1.84,-1.58]:m.line([(x,side*.63,2.2),(x+.08,side*.63,2.3)],.012,'Rubber')
 m.tube((-2.35,side*.45,2.08),(-2.8,side*.45,2.12),.16,.12,'Robot57',20)
 m.line([(-1.1,side*.75,.7),(-1.65,side*.9,.7),(-1.85,side*.9,.8)],.023,'Steel')
m.line([(-2.0,0,2.35),(-2.15,0,3.2)],.012,'Steel')
m.tube((-6.3,-.2,2.3),(-6.3,.26,2.3),.055,.025,'Steel');done('Vehicle42_helicopter')
seat(.1,-.43,.50);seat(.1,.43,.50)
for side in [-1,1]:seat(-1.4,side*.6,.48)
skin('flight console',[(.62,.65,.8,1.20),(.87,.6,.85,1.36),(1.1,.48,1.05,1.33)],'Rubber',16)
for side in [-1,1]:m.line([(.3,side*.43,.5),(.42,side*.43,1.05),(.30,side*.43,1.14)],.025,'Steel')
done('Cabin42_helicopter')
for side in [-1,1]:panel('canopy pane',[(.70,0,2.28),(.70,side*.68,2.28),(2.06,side*.20,1.2),(2.13,0,1.2)],.007,'Glass')
done('Windshield42_helicopter')
for angle in [0,math.pi/2,math.pi,math.pi*1.5]:
 pts=[(.18,-.07,0),(5.7,-.16,.015),(5.8,.10,0),(1,.17,-.035)]
 panel('aerofoil blade',[(x*math.cos(angle)-y*math.sin(angle),x*math.sin(angle)+y*math.cos(angle),z) for x,y,z in pts],.018,'Robot57')
m.tube((0,0,-.1),(0,0,.12),.18,.07,'Steel');done('Rotor57')
for angle in [0,math.pi/2,math.pi,math.pi*1.5]:
 pts=[(.06,-.04,0),(.8,-.08,0),(.83,.05,0),(.12,.05,0)]
 panel('tail rotor aerofoil',[(x*math.cos(angle)-y*math.sin(angle),x*math.sin(angle)+y*math.cos(angle),z) for x,y,z in pts],.018,'Robot57')
done('TailRotor57')
# Lathed pneumatic tires and sculpted steel wheels, with real tread shoulders.
for heavy in [False,True]:
 R=.42 if heavy else .34;verts=[];faces=[];n=48
 rings=[(-.17,.22),(-.17,R*.84),(-.12,R),(0,R*1.02),(.12,R),(.17,R*.84),(.17,.22)]
 for y,rad in rings:
  for i in range(n):a=math.tau*i/n;verts.append((rad*math.cos(a),y,rad*math.sin(a)))
 for j in range(len(rings)-1):
  for i in range(n):faces.append((j*n+i,j*n+(i+1)%n,(j+1)*n+(i+1)%n,(j+1)*n+i))
 m.mesh('tire carcass',verts,faces,'Rubber')
 for side in [-1,1]:
  m.tube((0,side*.15,0),(0,side*.18,0),.23,.17,'Steel',32)
  m.tube((0,side*.14,0),(0,side*.21,0),.08,.028,'Steel',20)
  for i in range(8):
   a=math.tau*i/8;m.line([(.07*math.cos(a),side*.17,.07*math.sin(a)),(.20*math.cos(a),side*.16,.20*math.sin(a))],.026,'Steel')
 for i in range(32):
  a=math.tau*i/32;m.line([(R*math.cos(a),-.10,R*math.sin(a)),(R*1.025*math.cos(a+.02),0,R*1.025*math.sin(a+.02)),(R*math.cos(a+.04),.10,R*math.sin(a+.04))],.012,'Rubber')
 done('Wheel57Heavy' if heavy else 'Wheel57Light')
# Creature modular anatomical surfaces in rest pose, joint origins at body zero.
for kind,mat in [('Bear','Fur57'),('Hornet','Chitin57'),('Rogue','Robot57')]:
 for part in ['Torso','Head','Pelvis','Arm','Leg']:
  if kind=='Bear':
   if part=='Torso':skin('muscular shoulders and rib cage',[(-.8,.20,-.3,.20),(-.5,.48,-.42,.40),(0,.58,-.44,.58),(.40,.53,-.30,.70),(.68,.27,-.18,.35)],mat,32)
   elif part=='Head':
    skin('bear skull and muzzle',[(-.20,.24,-.17,.25),(0,.30,-.23,.32),(.24,.23,-.20,.18),(.48,.16,-.18,.06),(.58,.12,-.14,.04)],mat,28)
    skin('wet nose',[(.53,.12,-.11,.035),(.60,.10,-.10,.025)],'Rubber',16)
    for side in [-1,1]:
     m.loft([(-.07,side*.21,.18,.1,.065),(-.08,side*.23,.39,.065,.055),(-.07,side*.22,.43,.025,.02)],mat,14)
     o=skin('eyelid',[(.10,.035,.13,.17),(.15,.035,.13,.17)],'Rubber',12);o.location.y=side*.243
   elif part=='Pelvis':skin('haunch',[(-.4,.20,-.28,.20),(-.2,.44,-.38,.33),(.2,.40,-.33,.31)],mat,24)
   else:
    m.loft([(0,0,.1,.16,.17),(.02,0,-.18,.20,.19),(-.03,0,-.43,.12,.12),(.04,0,-.70,.12,.11),(.20,0,-.78,.23,.15),(.27,0,-.83,.18,.13)],mat,20)
    for y in [-.10,-.035,.035,.10]:m.tube((.35,y,-.81),(.47,y,-.83),.018,.005,'Bone',8)
  elif kind=='Hornet':
   if part=='Torso':skin('thoracic exoskeleton',[(-.30,.10,-.10,.1),(-.17,.29,-.23,.23),(.12,.31,-.25,.25),(.30,.15,-.12,.16)],mat,24)
   elif part=='Pelvis':
    skin('segmented abdomen',[(-.65,.02,-.07,.03),(-.52,.18,-.13,.10),(-.28,.32,-.22,.20),(-.02,.30,-.22,.22),(.20,.10,-.09,.1)],mat,32)
    m.tube((-.58,0,-.05),(-.87,0,-.12),.025,.002,'Bone')
   elif part=='Head':
    skin('mandible head',[(-.16,.15,-.14,.15),(.02,.26,-.24,.23),(.22,.15,-.18,.08)],mat,24)
    for side in [-1,1]:
     o=skin('compound eye',[(-.04,.055,-.02,.20),(.09,.07,-.03,.19),(.17,.045,0,.10)],'Rubber',16);o.location.y=side*.21
     m.line([(.03,side*.12,.2),(.20,side*.21,.40),(.33,side*.29,.42)],.018,'Robot57')
     m.line([(.2,side*.12,-.1),(.35,side*.18,-.2),(.4,side*.04,-.21)],.028,'Chitin57')
   else:
    m.loft([(0,0,0,.055,.055),(.1,0,-.15,.06,.06),(.22,0,-.26,.034,.034),(.34,0,-.44,.022,.022),(.3,0,-.55,.008,.008)],mat,12)
  else:
   if part=='Torso':
    m.loft([(0,0,-.24,.14,.17),(-.015,0,-.1,.19,.20),(-.025,0,.15,.20,.30),(0,0,.3,.14,.26)],mat,20)
    m.loft([(0,0,.27,.075,.075),(0,0,.45,.060,.060)],'Steel',16)
    for side in [-1,1]:m.line([(.18,side*.18,-.12),(.22,side*.21,.18)],.035,'Steel')
   elif part=='Head':
    m.loft([(0,0,-.12,.09,.09),(.01,0,-.04,.13,.12),(0,0,.16,.14,.12),(-.02,0,.23,.09,.09)],mat,20)
    m.profile([(.125,.03),(.135,.08),(.12,.13),(.10,.03)],.19,'Red')
   elif part=='Pelvis':m.loft([(0,0,-.12,.10,.17),(0,0,.04,.15,.22),(0,0,.16,.11,.17)],mat,16)
   else:
    m.loft([(0,0,.04,.10,.105),(.02,0,-.15,.105,.10),(.04,0,-.26,.065,.068),(.02,0,-.34,.075,.08),(.03,0,-.56,.06,.07),(.10,0,-.65,.13,.07)],mat,16)
    for side in [-1,1]:m.tube((.06,side*.08,-.15),(.07,side*.07,-.48),.023,.012,'Steel')
  done(kind+part+'57','convex')
panel('veined wing',[(0,0,0),(-.12,.62,.03),(-.55,1.2,0),(-.74,1.1,-.01),(-.60,.40,0)],.003,'Glass')
for y in [.3,.5,.7,.9]:m.line([(0,0,.005),(-.55,y,.005)],.004,'Bone')
done('HornetWing57')
# Purpose-built furnishing silhouettes.
m.profile([(-.55,0),(-.55,.85),(-.40,1.05),(.42,1.05),(.60,.90),(.60,0)],1.8,'Wood')
panel('teller transaction shelf',[(-.65,-1,.96),(-.65,1,.96),(-.30,1,.96),(-.30,-1,.96)],.06,'Steel');done('Teller57','convex')
m.profile([(-.3,0),(-.3,1.8),(.3,1.8),(.3,1.2),(.15,1.05),(.28,.85),(.28,0)],.7,'Robot57')
panel('screen',[(-.29,-.26,1.3),(-.29,.26,1.3),(-.30,.26,1.65),(-.30,-.26,1.65)],.005,'Glow');done('ATM57','convex')
for y in [-.65,.65]:m.profile([(-.25,0),(-.25,1.8),(.1,1.8),(.25,0)],.045,'Steel',y)
for z in [.25,.85,1.5]:m.profile([(-.2,z),(-.2,z+.035),(.2,z+.035),(.2,z)],1.35,'Wood')
for y in [-.45,-.15,.15,.45]:
 m.tube((0,y,.3),(.02,y,1.67),.025,.012,'Steel');m.profile([(-.1,.3),(-.1,.65),(.08,.85),(.08,.25)],.06,'Wood',y)
done('GunRack57','convex')
m.profile([(-.03,0),(-.03,.16),(.03,.16),(.03,0)],.9,'Steel')
m.loft([(0,0,.2,.025,.025),(0,0,1.9,.025,.025)],'Steel')
m.profile([(-.015,1),(-.015,1.7),(.015,1.7),(.015,1)],.55,'Bone');done('RangeTarget57','convex')
for y in [-.55,.55]:m.loft([(0,y,0,.04,.04),(0,y,1.8,.04,.04)],'Steel')
for z in [.25,.8,1.4]:
 m.profile([(-.25,z),(-.25,z+.04),(.25,z+.04),(.25,z)],1.3,'Wood')
 for y in [-.42,0,.42]:m.loft([(0,y,z+.06,.13,.14),(0,y,z+.25,.18,.16),(0,y,z+.38,.12,.10)],'Cloth',16)
done('SportsRack57','convex')
m.tube((0,0,1.3),(.18,0,1.3),1.25,.97,'Steel',48)
m.profile([(0,0),(0,2.6),(.12,2.6),(.12,0)],2.5,'Armor57')
for a in range(0,360,45):
 t=math.radians(a);m.line([(-.10,0,1.3),(-.1,.55*math.sin(t),1.3+.55*math.cos(t))],.025,'Steel')
m.tube((-.12,0,1.3),(-.08,0,1.3),.6,.55,'Steel',32);done('VaultDoor57','convex')
m.export_all()
(m.OUT/'models_v57_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2))
bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'Expansion57.blend'))
print('EXPANSION57_MODELS_DONE',len(m.RECORDS),flush=True)
