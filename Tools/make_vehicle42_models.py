"""Authored panel surfaces, open wheel arches and fitted cabin meshes. Blender 4.2."""
import sys,math,json
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent));import make_models_v2 as m
import bpy,bmesh
from mathutils import Vector
m.OUT=m.ROOT/'ArtSource/ModelsV42'
colors=json.loads('{}')
for p in (m.ROOT/'ArtSource/TexturesV42').glob('*.png'):m.PALETTE[p.stem[2:]]=(.45,.45,.42)
m.PALETTE['V42_Glass']=(.06,.12,.14);m.setup()
for name,mat in m.MATS.items():
 p=m.ROOT/'ArtSource/TexturesV42'/('T_'+name+'.png')
 if p.exists():
  sh=mat.node_tree.nodes.get('Principled BSDF');t=mat.node_tree.nodes.new('ShaderNodeTexImage');t.image=bpy.data.images.load(str(p));mat.node_tree.links.new(t.outputs['Color'],sh.inputs['Base Color'])
SPECS=[('sedan',2.05,.87,1.57,2.70,4),('police',2.30,.95,1.65,2.9,4),('boxtruck',3.8,1.15,3.3,4.8,2),('rv',6.05,1.28,3.43,7.37,5),('bus',4.9,1.20,3.1,6.4,11),('van',2.75,1.05,2.45,3.4,2),('pickup',2.7,1.05,1.80,3.5,4),('dirtbike',1.15,.34,1.20,1.45,1),('suv',2.45,1.03,2.0,3.,7),('muscle',2.35,1.00,1.50,2.85,4),('supercar',2.25,1.,1.37,2.7,2)]

def finish(name):
 m.finish(name,collision='none')
 # Preserve crisp seams while smoothing curved body panels, tyres and upholstery.
 o=m.OBJECTS[name]
 for p in o.data.polygons:p.use_smooth=True
 bm=bmesh.new();bm.from_mesh(o.data)
 for e in bm.edges:
  if e.is_manifold and e.calc_face_angle()>.65:e.smooth=False
 bm.to_mesh(o.data);bm.free()
 mod=o.modifiers.new('Weighted panel normals','WEIGHTED_NORMAL');mod.keep_sharp=True
 bpy.context.view_layer.objects.active=o
 try:bpy.ops.object.modifier_apply(modifier=mod.name)
 except:pass

def line(points,r,mat):m.line(points,r,mat)
def surface(name,rows,mat):
 n=len(rows[0]);v=[p for row in rows for p in row];f=[]
 for a in range(len(rows)-1):
  for b in range(n-1):i=a*n+b;f.append((i,i+1,i+n+1,i+n))
 return m.mesh(name,v,f,mat)
def seat(p,width=.49,lux=False):
 x,y,z=p;mat='V42_Leather' if lux else 'V42_Fabric'
 m.loft([(x,y,z,.22,width*.42),(x,y,z+.05,.27,width*.51),(x,y,z+.14,.25,width*.49),(x,y,z+.17,.21,width*.4)],mat,16)
 m.loft([(x-.23,y,z+.13,.075,width*.46),(x-.26,y,z+.36,.10,width*.52),(x-.31,y,z+.70,.08,width*.45),(x-.32,y,z+.77,.06,width*.36)],mat,16)
 for side in [-1,1]:
  line([(x+.18,y+side*width*.43,z+.15),(x-.20,y+side*width*.48,z+.15),(x-.26,y+side*width*.44,z+.65)],.018,mat)
  m.cyl((x-.29,y+side*.075,z+.72),(x-.29,y+side*.075,z+.85),.011,'V42_Chrome',n=10)
 m.loft([(x-.29,y,z+.82,.045,.14),(x-.30,y,z+.89,.065,.165),(x-.30,y,z+1.00,.055,.145)],mat,16)
 for s in [-1,1]:m.box((x,y+s*.17,z-.08),(.44,.035,.045),'V42_Metal',.008)
 line([(x-.32,y-width*.49,z+.7),(x+.14,y-width*.45,z+.14)],.014,'V42_Rubber')
 m.box((x+.05,y+width*.5,z+.19),(.06,.032,.10),'V42_Rubber',.012)
 for i in range(5):line([(x-.16+i*.07,y-width*.32,z+.173),(x-.16+i*.07,y+width*.32,z+.173)],.003,'V42_Vinyl')

for car,L,W,H,WB,N in SPECS:
 glassbase=.93 if car=='supercar' else 1.13
 paint='V42_Paint_'+car;rear=2.20-2*L;front=2.20;heavy=car in ('bus','rv','boxtruck');cargo=car in ('van','boxtruck');sport=car in ('supercar','muscle')
 radius=.43 if car=='rv' else .39 if heavy else .34
 axles=[1.32,1.32-WB]+([-7.55] if car=='rv' else [])
 if car=='dirtbike':
  for s in [-1,1]:
   line([(-.63,s*.11,.47),(-.25,s*.13,1.0),(.43,s*.11,.85),(.30,s*.14,.38),(-.63,s*.11,.47)],.035,'V42_Metal')
   m.cyl((.62,s*.13,.42),(.38,s*.13,1.16),.034,'V42_Chrome',n=16)
   line([(.38,0,1.16),(.24,s*.3,1.22),(.12,s*.34,1.20)],.018,'V42_Metal')
   m.cyl((.12,s*.26,1.2),(.12,s*.36,1.2),.027,'V42_Rubber',n=16)
  m.loft([(-.06,0,.73,.25,.14),(-.08,0,.94,.26,.17),(-.1,0,1.04,.16,.12)],paint,16)
  m.loft([(-.41,0,.87,.32,.13),(-.41,0,.96,.32,.15),(-.41,0,.99,.26,.13)],'V42_Leather',16)
  for z in [.49,.53,.57,.61,.65]:m.box((-.05,0,z),(.29,.30,.023),'V42_Metal',.012)
  m.cyl((-.08,-.13,.38),(-.08,.13,.38),.15,'V42_Metal',n=20)
  line([(.15,-.14,.57),(.34,-.18,.48),(.15,-.23,.30),(-.62,-.21,.65)],.038,'V42_Chrome')
  m.tube((-.55,-.21,.64),(-.85,-.21,.76),.075,.045,'V42_Metal',n=20)
  for x in [-.72,.72]:
   rows=[]
   for i in range(19):
    a=math.pi*.10+math.pi*.8*i/18;rows.append([(x+.41*math.cos(a),y,.34+.41*math.sin(a)) for y in [-.11,.11]])
   surface('curved mudguard',rows,paint)
  finish('Vehicle42_'+car);m.box((.28,0,1.14),(.13,.17,.07),'V42_Vinyl',.02);finish('Cabin42_'+car);continue
 # Longitudinal sculpted shoulder and wheel arch surfaces.
 xs=sorted(set([rear+(front-rear)*i/46 for i in range(47)]+[a+(radius+.065)*math.cos(math.pi*i/24) for a in axles for i in range(25) if rear<a+(radius+.065)*math.cos(math.pi*i/24)<front]))
 xs=sorted(set(round(x,3) for x in xs))
 for side in [-1,1]:
  rows=[]
  for x in xs:
   tip=min(1,(x-rear)/.35,(front-x)/.40);w=W*(.91+.09*max(0,tip));low=.31
   for a in axles:
    if abs(x-a)<radius+.065:low=max(low,radius+math.sqrt(max(0,(radius+.065)**2-(x-a)**2)))
   shoulder=(.92 if sport else 1.05 if not heavy else 1.23)+.045*math.cos(x*1.8)
   shoulder=max(shoulder,low+.05)
   rows.append([(x,side*(w-.035),low),(x,side*w,low+.045),(x,side*(w+.018),shoulder-.12),(x,side*(w-.02),shoulder),(x,side*(w-.15),shoulder+.025)])
  surface('pressed fender shoulder and open arch',rows,paint)
  for axle in axles:
   pts=[(axle+(radius+.07)*math.cos(math.pi*i/24),side*(W+.005),radius+(radius+.07)*math.sin(math.pi*i/24)) for i in range(25)]
   line(pts,.014,'V42_Vinyl')
  # Lower sill and door seams stay clear of the wheel openings.
  line([(axles[1]+radius+.1,side*(W-.025),.35),(axles[0]-radius-.1,side*(W-.025),.35)],.028,'V42_Vinyl')
 # Grilles, recessed slats, plate recesses, bumpers and lamp lenses.
 for x,sign in [(front,1),(rear,-1)]:
  # A continuous curved fascia closes the shell behind the lamps and grille.
  w=W*.91;sh=(.92 if sport else 1.05 if not heavy else 1.23)+.045*math.cos(x*1.8)
  border=[(-w+.035,.31),(-w,.355),(-w-.018,sh-.12),(-w+.02,sh),(-w+.15,sh+.025),(0,sh+.080),(w-.15,sh+.025),(w-.02,sh),(w+.018,sh-.12),(w,.355),(w-.035,.31)]
  v=[(x,0,.62)]+[(x,y,z) for y,z in border];m.mesh('closed front and rear fascia',v,[(0,i+1,(i+1)%len(border)+1) for i in range(len(border))],paint)

  m.box((x-sign*.065,0,.55),(.13,W*1.83,.21),'V42_Vinyl',.06)
  m.box((x+sign*.005,0,.70),(.015,W*.85,.22),'V42_Rubber',.025)
  for z in [.63,.68,.73,.78]:m.box((x+sign*.018,0,z),(.016,W*.81,.009),'V42_Chrome',.003)
  m.box((x+sign*.02,0,.43),(.013,.36,.14),'V42_Metal',.009)
  for side in [-1,1]:
   m.box((x+sign*.005,side*W*.72,.86),(.045,W*.42,.13),'V42_Lamp' if sign>0 else 'V42_RedLamp',.045)
   for k in [-1,0,1]:m.box((x+sign*.032,side*W*.72+k*.07,.86),(.016,.008,.085),'V42_Chrome',.003)
 if car=='rv':cabrear=rear+.13;cabfront=1.75;roof=H-.11;base=1.48
 elif heavy:cabrear=rear+.12;cabfront=.62;roof=H-.10;base=1.24
 elif car=='van':cabrear=rear+.15;cabfront=.25;roof=H-.10;base=1.15
 else:cabrear=(-1.4 if car not in ('suv','pickup') else -2.0);cabfront=.03;roof=H;base=1.05 if not sport else .94
 if car=='boxtruck':cabrear=-1.2;roof=2.12
 # Bonnet and rear deck are bowed surfaces, not scaled boxes.
 for a,b,z in [(1.98 if car=='rv' else .73,front,1.02 if not sport else .89),(rear,cabrear-.10,1.02 if not sport else .88)]:
  if b<=a:continue
  deck=[]
  for i in range(19):
   x=a+(b-a)*i/18;tip=max(0,min(1,(x-rear)/.35,(front-x)/.40));w=W*(.91+.09*tip);sh=(.92 if sport else 1.05 if not heavy else 1.23)+.045*math.cos(x*1.8)
   deck.append([(x,y*(w-.15),sh+.025+.055*(1-y*y)) for y in [-1,-.8,-.4,0,.4,.8,1]])
  surface('continuous curved bonnet',deck,paint)
  for side in [-1,1]:line([(a,side*W*.68,z+.024),(b,side*W*.68,z-.01)],.004,'V42_Vinyl')
 # Roof skin, liner and perimeter pillars. Large vehicles have repeated windows.
 roofwidth=W*(.99 if car=='rv' else .94 if heavy else .84)
 surface('crowned roof', [[(cabrear+(cabfront-cabrear)*i/12,y*roofwidth,roof+.055*(1-y*y)) for y in [-1,-.8,-.4,0,.4,.8,1]] for i in range(13)],paint)
 # Seal curved roof edges and the rear cabin window to the body.
 for x in [cabrear,cabfront]:
  surface('roof header', [[(x,y*roofwidth,roof+lift+.055*(1-y*y)) for y in [-1,-.75,-.4,0,.4,.75,1]] for lift in [-.07,0]],paint)
 if car=='rv':
  surface('coach front crown', [[(xx,y*1.24,zz+.04*(1-y*y)) for y in [-1,-.8,-.4,0,.4,.8,1]] for xx,zz in [(1.74,2.88),(1.76,3.0),(1.75,roof)]],paint)
  for side in [-1,1]:line([(2.1,side*1.24,1.25),(1.98,side*1.15,1.46),(1.74,side*1.15,2.88),(1.75,side*1.24,roof)],.055,paint)
  m.box((rear+.035,0,(roof+.31)/2),(.07,W*1.96,roof-.31),paint,.06)
 elif not cargo:
  m.mesh('rear glass',[(cabrear-.28,-W*.90,base),(cabrear-.28,W*.90,base),(cabrear,W*.84,roof),(cabrear,-W*.84,roof)],[(0,1,2,3)],'V42_Glass')
 for side in [-1,1]:
  if car=='rv':
   sections=[(-9.84,-8.12),(-7.94,-6.22),(-6.04,-4.32),(-4.14,-2.42),(-2.24,-.52),(-.34,.0),(.91,1.70)]
   for a,b in sections:
    m.box(((a+b)/2,side*(W-.045),2.38),(b-a,.055,.80),'V42_Glass',.02)
    for xx in [a,b]:m.box((xx,side*(W-.03),2.38),(.065,.06,.87),'V42_Vinyl',.015)
    for zz in [1.94,2.82]:m.box(((a+b)/2,side*(W-.03),zz),(b-a+.05,.07,.065),'V42_Chrome',.012)
   for a,b in ([(rear+.1,front-.08)] if side<0 else [(rear+.1,-.18),(.88,front-.08)]):
    for z,height in [(1.51,.83),(3.08,.48)]:m.box(((a+b)/2,side*(W-.025),z),(b-a,.055,height),paint,.025)
  else:
   n=max(1,int((cabfront-cabrear)/.85));cuts=[cabrear+(cabfront-cabrear)*i/n for i in range(n+1)]
   for a,b in zip(cuts,cuts[1:]):
    if not cargo:m.box(((a+b)/2,side*W*.89,(roof+base+.13)/2),(b-a-.07,.018,roof-base-.13),'V42_Glass',.018)
    else:m.box(((a+b)/2,side*W*.94,(roof+base)/2),(b-a,.045,roof-base),paint,.022)
    line([(a,side*W*.91,base),(a,side*W*.84,roof)],.032,'V42_Vinyl')
   line([(cabrear-.28,side*W*.9,base),(cabrear,side*W*.84,roof),(cabfront,side*W*.84,roof),(.74,side*W*.91,glassbase)],.035,paint)
   line([(cabrear,side*W*.9,base),(cabfront+.5,side*W*.91,base)],.016,'V42_Chrome')
  # Recessed sculpted door perimeter and flush handle.
  if car!='rv':
   for x in [-.25]+([-1.2] if N>2 and not heavy else []):
    line([(x+.3,side*(W+.022),1.0),(x+.3,side*(W+.022),.42),(x-.5,side*(W+.022),.42),(x-.55,side*(W+.022),1.0)],.005,'V42_Rubber')
    m.box((x-.25,side*(W+.03),.94),(.17,.025,.035),'V42_Chrome',.012)
  line([(.30,side*W*.88,1.13),(.18,side*(W+.15),1.19)],.024,'V42_Vinyl')
  m.loft([(.16,side*(W+.18),1.15,.14,.06),(.15,side*(W+.20),1.20,.15,.08),(.15,side*(W+.20),1.26,.12,.07)],paint,12)
  m.box((.035,side*(W+.20),1.21),(.009,.14,.07),'V42_Chrome',.02)
 if car=='boxtruck':
  for side in [-1,1]:
   m.box(((rear-1.35)/2,side*(W-.015),2.02),(-1.35-rear,.05,2.54),paint,.025)
   for x in [rear+.12+i*.4 for i in range(int((-1.35-rear)/.4))]:m.box((x,side*(W+.025),2.02),(.025,.02,2.52),'V42_Metal',.006)
  m.box(((rear-1.35)/2,0,3.30),(-1.35-rear,W*2,.05),paint,.025)
  m.box((-1.35,0,2.02),(.06,W*2,2.54),paint,.025)
 if car=='pickup':
  for side in [-1,1]:m.box(((rear-2.03)/2,side*(W-.05),.95),(-2.03-rear,.08,.60),paint,.03)
  m.box(((rear-2.03)/2,0,.53),(-2.03-rear,W*1.85,.05),'V42_Vinyl',.02)
  for y in [-.7,-.5,-.3,-.1,.1,.3,.5,.7]:m.box(((rear-2.03)/2,y,.58),(-2.03-rear,.025,.025),'V42_Metal',.006)
 if car=='supercar':
  for side in [-1,1]:
   m.mesh('recessed side intake',[(-1.22,side*.975,.52),(-1.12,side*.97,1.0),(-.50,side*.99,.85),(-.76,side*.99,.48)],[(0,1,2,3)],'V42_Rubber')
   line([(-1.19,side*.987,.53),(-1.07,side*.985,.98),(-.48,side*1.0,.84)],.021,paint)
   m.cyl((-1.65,side*.66,.98),(-1.75,side*.68,1.22),.025,'V42_Metal',n=12)
  m.box((-1.78,0,1.24),(.30,1.84,.045),'V42_Vinyl',.02)
 if car=='bus':
  for side in [-1,1]:
   for z in [.95,1.15,1.4]:m.box(((rear+front)/2,side*(W+.035),z),(front-rear-.12,.04,.05),'V42_Vinyl',.01)
 if car=='rv':
  for side in [-1,1]:
   for z in [1.04,1.25,1.42]:line([(rear+.15,side*(W+.035),z+.15),(-5,side*(W+.035),z),(-1,side*(W+.035),z+.14)],.024,'V42_Vinyl')
  for x in [-7,-3]:m.box((x,0,H+.1),(.95,.86,.22),'V42_Vinyl',.12)
 finish('Vehicle42_'+car)
 # Fitted glass shares the pillar endpoints and has full-rectangle rain UVs.
 bottom=(1.98,1.15,1.46) if car=='rv' else (.74,W*.91,glassbase)
 top=(1.74,1.15,2.88) if car=='rv' else (cabfront,W*.84,roof)
 bx,bw,bz=bottom;tx,tw,tz=top
 m.mesh('fitted windshield',[(bx,-bw,bz),(bx,bw,bz),(tx,tw,tz),(tx,-tw,tz)],[(0,1,2,3)],'V42_Glass');finish('Windshield42_'+car)
 ob=m.OBJECTS['Windshield42_'+car]
 for poly in ob.data.polygons:
  for li in poly.loop_indices:
   v=ob.data.vertices[ob.data.loops[li].vertex_index].co;vv=(v.z-bz)/(tz-bz);ww=bw+(tw-bw)*vv;ob.data.uv_layers.active.data[li].uv=(v.y/(2*ww)+.5,vv)
 # Interior is a separate detailed mesh: footwell, shaped dashboard, door cards,
 # seats, seatbelts, vents, pedals, handbrake, cupholders and headlining.
 floor=.61 if car=='rv' else .43
 floorfront=2.12 if car=='rv' else 1.3
 m.box(((rear+.1+floorfront)/2,0,floor-.045),(floorfront-rear-.1,W*1.83,.085),'V42_Carpet',.025)
 if car=='rv':
  m.box((2.04,0,1.055),(.08,W*1.90,.89),'V42_Vinyl',.025)
  # Closed inboard wheel tubs; tyre tops remain below these upholstered arches.
  for axle in axles:
   for side in [-1,1]:
    ring=[(axle+.51*math.cos(i*math.pi/16),.43+.51*math.sin(i*math.pi/16)) for i in range(17)]
    for y in [side*1.065,side*1.30]:
     m.mesh('wheel tub end',[(x,y,z) for x,z in ring], [tuple(range(17))], 'V42_Vinyl')
    surface('wheel tub arch',[[(x,side*1.065,z),(x,side*1.30,z)] for x,z in ring],'V42_Leather')

 dx=1.3 if car=='rv' else 0;dz=.65 if car=='rv' else -.20 if car=='supercar' else .27 if car in ('bus','boxtruck') else 0
 rows=[]
 for i in range(25):
  y=-W*.9+W*1.8*i/24;bulge=.07*math.cos(y/W*math.pi)
  rows.append([(dx+.17+bulge,y,dz+.86),(dx+.14+bulge,y,dz+1.05),(dx+.28,y,dz+1.13),(dx+.66,y,dz+1.14),(dx+.75,y,dz+1.04)])
 surface('moulded dashboard',rows,'V42_Vinyl')
 for yy in [-W*.74,-.12,.14,W*.70]:
  m.box((dx+.155,yy,dz+1.01),(.035,.16,.075),'V42_Rubber',.016)
  for k in range(5):m.box((dx+.13,yy-.064+k*.032,dz+1.01),(.015,.01,.06),'V42_Metal',.004)
 # Binnacle cowl, feet clear under dashboard.
 line([(dx+.18,-.79,dz+1.05),(dx+.22,-.73,dz+1.22),(dx+.22,-.2,dz+1.22),(dx+.18,-.14,dz+1.05)],.03,'V42_Vinyl')
 for yy in [-.55,-.4,-.26]:
  m.box((dx+.24,yy,floor+.065),(.10,.055,.025),'V42_Rubber',.012,rot=(0,.35,0))
  m.cyl((dx+.28,yy,floor+.07),(dx+.40,yy,floor+.30),.012,'V42_Metal',n=8)
 m.box((dx-.15,0,floor+.22),(.74,.26,.39),'V42_Vinyl',.06)
 m.cyl((dx+.01,0,floor+.42),(dx-.04,0,floor+.66),.018,'V42_Metal',n=12)
 m.loft([(dx-.04,0,floor+.63,.028,.031),(dx-.04,0,floor+.68,.044,.038),(dx-.04,0,floor+.71,.025,.025)],'V42_Leather',12)
 for xx in [-.31,-.5]:m.tube((dx+xx,0,floor+.415),(dx+xx,0,floor+.44),.065,.051,'V42_Rubber',n=20)
 line([(dx-.24,-.19,floor+.26),(dx-.52,-.19,floor+.34)],.025,'V42_Leather')
 # Radio face and climate controls, buttons have separate runtime hit targets.
 m.box((dx+.115,.015,dz+.99),(.025,.25,.11),'V42_Metal',.012)
 m.box((dx+.097,.015,dz+1.015),(.012,.16,.036),'V42_Glass',.004)
 for yy in [-.08,.11]:m.cyl((dx+.08,yy,dz+.97),(dx+.10,yy,dz+.97),.019,'V42_Rubber',n=16)
 for yy in [-.07,0,.07]:m.cyl((dx+.10,yy,dz+.85),(dx+.14,yy,dz+.85),.026,'V42_Metal',n=16)
 if car!='rv':
  seat((-.43,-.43,.49+dz),lux=sport or car=='suv')
  if car in ('bus','boxtruck'):m.box((-.43,-.43,.57),(.45,.43,.27),'V42_Metal',.04)
  for i in range(N-1):
   if car=='bus':x=-1.12-(i//2);y=.43 if i%2 else -.43
   elif car=='suv' and i>0:x=-1.12 if i<4 else -2.05;y=(i-2)*.55 if i<4 else (-.43 if i==4 else .43)
   else:x=-.43 if i==0 else -1.12-((i-1)//2);y=.43 if i==0 else (-.43 if (i-1)%2 else .43)
   seat((x,y,.49+(0 if car=='bus' else dz)),lux=sport or car=='suv')
   if car=='boxtruck':m.box((x,y,.57),(.45,.43,.27),'V42_Metal',.04)
 for side in [-1,1]:
  for x in ([-.4,-1.25] if N>2 and not heavy else [-.4]):
   surface('sculpted door card',[[(xx,side*(W-.08),.47),(xx,side*(W-.10),.72),(xx,side*(W-.16),.79),(xx,side*(W-.12),1.00)] for xx in [x-.39,x-.26,x+.25,x+.42]],'V42_Leather')
   m.box((x,side*(W-.19),.80),(.39,.10,.06),'V42_Vinyl',.025)
   m.box((x+.17,side*(W-.14),.94),(.12,.022,.035),'V42_Chrome',.01)
   m.tube((x-.2,side*(W-.085),.64),(x-.2,side*(W-.07),.64),.073,.06,'V42_Vinyl',n=20)
 if car!='rv':
  m.box(((cabrear+cabfront)/2,0,roof-.055),(cabfront-cabrear,W*1.65,.025),'V42_Fabric',.02)
  for side in [-1,1]:m.box((cabfront+.04,side*.43,roof-(.03 if car in ('sedan','muscle','supercar') else .11)),(.21,.45,.035),'V42_Vinyl',.024)
  m.box((cabfront+.04,0,roof-.21),(.06,.25,.085),'V42_Vinyl',.018)
 if car in ('van','boxtruck'):
  for side in [-1,1]:
   for z in [.75,1.15,1.55]:m.box(((rear-1.3)/2,side*(W-.2),z),(-1.3-rear,.30,.035),'V42_Metal',.008)
   for x in [rear+.18,-1.4]:m.box((x,side*(W-.3),1.12),(.035,.035,1.30),'V42_Metal',.006)
 finish('Cabin42_'+car)
 # Individually animated wheel with tyre shoulders, tread blocks, inner barrel,
 # brake disc, hub bolts and forked alloy spokes. Root lies at wheel centre.
 rr=.34 if car!='rv' else .43;ww=.14
 rings=[(-ww*.92,rr*.72),(-ww,rr*.88),(-ww*.83,rr*.99),(-ww*.60,rr),(ww*.6,rr),(ww*.83,rr*.99),(ww,rr*.88),(ww*.92,rr*.72)]
 verts=[(rad*math.sin(math.tau*i/48),y,rad*math.cos(math.tau*i/48)) for y,rad in rings for i in range(48)];faces=[]
 for j in range(len(rings)-1):
  for i in range(48):k=j*48+i;faces.append((k,j*48+(i+1)%48,(j+1)*48+(i+1)%48,k+48))
 m.mesh('rounded tyre casing',verts,faces,'V42_Rubber')
 for i in range(40):
  a=i*math.tau/40
  for side in [-1,1]:m.box((rr*math.sin(a),side*.061,rr*math.cos(a)),(.017,.07,.009),'V42_Vinyl',.002,rot=(0,a,side*.25))
 for side in [-1,1]:
  m.tube((0,side*.115,0),(0,side*.13,0),rr*.73,rr*.66,'V42_Chrome',n=40)
  m.cyl((0,side*.075,0),(0,side*.085,0),rr*.57,'V42_Metal',n=32)
  for i in range(7 if sport else 6):
   a=i*math.tau/(7 if sport else 6)
   for d in [-.055,.055]:m.cyl((rr*.15*math.sin(a),side*.14,rr*.15*math.cos(a)),(rr*.68*math.sin(a+d),side*.12,rr*.68*math.cos(a+d)),.012,'V42_Chrome',n=6)
  m.cyl((0,side*.11,0),(0,side*.145,0),.07,'V42_Metal',n=16)
  for i in range(5):a=i*math.tau/5;m.cyl((.048*math.sin(a),side*.14,.048*math.cos(a)),(.048*math.sin(a),side*.154,.048*math.cos(a)),.008,'V42_Chrome',n=6)
 finish('Wheel42_'+car)
# Shared functional interior hardware.
seat((0,0,.36),lux=True);finish('Seat42')
# Steering ring is toroidal rather than a filled disc.
m.tube((-.018,0,0),(.018,0,0),.185,.154,'V42_Leather',n=40)
for a in [-.65,.65,math.pi]:m.cyl((0,0,0),(0,.16*math.sin(a),.16*math.cos(a)),.018,'V42_Metal',n=10)
m.box((-.025,0,0),(.06,.10,.09),'V42_Leather',.03);finish('Steering42')
# Dial planes have deliberately mapped UVs, facing the seated driver (-X).
for typ in ['Speed','RPM','Fuel','Temp']:
 o=m.mesh('dial',[(-.003,-.1,-.1),(-.003,.1,-.1),(-.003,.1,.1),(-.003,-.1,.1)],[(0,3,2,1)],'V42_Dial'+typ);finish('Dial42'+typ)
 o=m.OBJECTS['Dial42'+typ]
 for p in o.data.polygons:
  for li in p.loop_indices:
   v=o.data.vertices[o.data.loops[li].vertex_index].co;o.data.uv_layers.active.data[li].uv=(v.y/.2+.5,v.z/.2+.5)
m.mesh('needle',[(-.008,-.003,-.016),(-.008,.003,-.016),(-.008,.001,.086),(-.008,-.001,.086)],[(0,3,2,1)],'V42_Amber');m.cyl((-.012,0,0),(-.003,0,0),.009,'V42_Chrome',n=16);finish('Needle42')
m.export_all();(m.OUT/'models_v42_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'VehicleOverhaul42.blend'))
print('VEHICLE42_MODELS_DONE',len(m.RECORDS),flush=True)
