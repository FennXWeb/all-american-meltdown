"""Authored polygon surfaces for the 35 character wardrobe. Metres, +X front."""
import sys,math,json
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent))
import bpy,bmesh
from mathutils import Vector
import make_models_v2 as m
m.OUT=m.ROOT/'ArtSource/Characters35';m.setup()
SURF={'Skin35':(0,(1,1,1)), 'Cotton35':(1,(.5,.55,.5)), 'Denim35':(2,(.65,.72,.85)), 'Leather35':(3,(.45,.3,.21)), 'Hair35':(4,(.2,.12,.07)), 'Knit35':(5,(.45,.48,.48)), 'Rubber35':(6,(.3,.3,.3)), 'Canvas35':(7,(.55,.52,.41)), 'Eye35':(-1,(.56,.54,.49)), 'Iris35':(-1,(.10,.075,.04)), 'Pupil35':(-1,(.008,.007,.006)), 'Lip35':(-1,(.29,.115,.09)), 'Metal35':(-1,(.32,.32,.3)), 'Face035':(-2,(1,1,1)), 'Face135':(-3,(1,1,1))}
for key,(_,col) in SURF.items():
 mat=bpy.data.materials.new('M_'+key);mat.diffuse_color=(*col,1);m.MATS[key]=mat

def gauss(v,c,w):return math.exp(-((v-c)/w)**2)
def surface(rows,mat,n=24,fold=0,shape=None):
 vs=[]
 for j,(x,y,z,rx,ry) in enumerate(rows):
  for i in range(n):
   a=math.tau*i/n;f=fold*(math.sin(a*5+j*.8)+.35*math.sin(a*9-j*.6));p=[x+(rx+f)*math.cos(a),y+(ry+f)*math.sin(a),z]
   if shape:p=shape(p,a,j)
   vs.append(p)
 fs=[tuple(range(n-1,-1,-1))]+[(j*n+i,j*n+(i+1)%n,(j+1)*n+(i+1)%n,(j+1)*n+i) for j in range(len(rows)-1) for i in range(n)]+[tuple((len(rows)-1)*n+i for i in range(n))]
 return m.mesh('Tailored surface',vs,fs,mat)
def ribbon(points,width,mat):
 vs=[]
 for p in points:vs.extend([(p[0],p[1]-width,p[2]),(p[0],p[1]+width,p[2])])
 return m.mesh('Sewn edge',vs,[(i*2,i*2+1,i*2+3,i*2+2) for i in range(len(points)-1)],mat)
def patch(x,y,z,w,h,mat):
 return m.mesh('Tailored pocket',[(x-.009,y-w/2,z-h/2),(x,y+w/2,z-h/2),(x+.003,y+w/2,z+h/2),(x-.006,y-w/2,z+h/2),(x+.009,y,z)],[(0,1,4),(1,2,4),(2,3,4),(3,0,4)],mat)
def finish(name):
 for piece in m.PARTS:
  bm=bmesh.new();bm.from_mesh(piece.data);bmesh.ops.delete(bm,geom=[v for v in bm.verts if not v.link_faces],context='VERTS');bm.to_mesh(piece.data);bm.free()
 ob=m.finish(name,collision='convex');
 color=ob.data.color_attributes.new(name='CustomizationMask',type='FLOAT_COLOR',domain='CORNER')
 for c in color.data:c.color=(1,0,0,1)

 for p in ob.data.polygons:p.use_smooth=True
 if any(k in name for k in ['Trousers','NPCLeg','Footwear']):
  for v in ob.data.vertices:v.co.z*=1.03
 if name.startswith(('Hair','Beard')):
  for loop in ob.data.loops:
   v=ob.data.vertices[loop.vertex_index].co;ob.data.uv_layers.active.data[loop.index].uv=((math.atan2(v.y,v.x+.014)/math.tau+.5)%1,(.14-v.z)/.5)

 if name.endswith('Head35'):
  levels=[(-.168,1),(-.109,.745),(-.064,.56),(-.035,.30),(-.014,.20),(.02,.075),(.076,0)]
  for poly in ob.data.polygons:
   if 'Face' not in ob.data.materials[poly.material_index].name:continue
   for li in poly.loop_indices:
    v=ob.data.vertices[ob.data.loops[li].vertex_index].co;value=0
    for a,b in zip(levels,levels[1:]):
     if a[0]<=v.z<=b[0]:t=(v.z-a[0])/(b[0]-a[0]);value=a[1]*(1-t)+b[1]*t;break
    ob.data.uv_layers.active.data[li].uv=(max(.001,min(.999,v.y/.16+.5)),1-value)
    def smooth(a,b,x):
     t=max(0,min(1,(x-a)/(b-a)));return t*t*(3-2*t)
    weight=smooth(.005,.055,v.x)*(1-smooth(.052,.078,abs(v.y)))*smooth(-.185,-.145,v.z)
    color.data[li].color=(weight,0,0,1)
 return ob

# CC0 MakeHuman hm08 anatomy; only graphical mesh data is used.
MH_VERT=[];MH_FACES={};_group=''
for _line in (m.OUT/'Reference/base.obj').read_text().splitlines():
 if _line.startswith('v '):MH_VERT.append(tuple(map(float,_line.split()[1:])))
 elif _line.startswith('g '):_group=_line[2:];MH_FACES.setdefault(_group,[])
 elif _line.startswith('f '):MH_FACES[_group].append(tuple(int(t.split('/')[0])-1 for t in _line.split()[1:]))
HEAD_BVH=None

def head(female=False):
 global HEAD_BVH
 from mathutils.bvhtree import BVHTree
 levels=[(5.85,-.242),(5.95,-.225),(6.10,-.168),(6.65,-.109),(6.93,-.064),(7.28,-.035),(8.49,.076)]
 def point(v):
  lateral,up,front=v
  for a,b in zip(levels,levels[1:]):
   if a[0]<=up<=b[0]:t=(up-a[0])/(b[0]-a[0]);z=a[1]*(1-t)+b[1]*t;break
  else:z=.076 if up>8.49 else -.242
  x=front*.1-.070;y=lateral*.1
  if z<-.175:
   t=min(1,(-z-.175)/.065);y*=1-.28*t;x=x*(1-.24*t)+.012*t
  if female:
   y*=1-.045*gauss(z,-.13,.055);x-=.002*gauss(z,-.014,.025)
  return x,y,z
 faces=[f for f in MH_FACES['body'] if all(MH_VERT[i][1]>=5.85 for i in f)]
 inds=sorted({i for f in faces for i in f});lookup={i:j for j,i in enumerate(inds)}
 vs=[point(MH_VERT[i]) for i in inds];fs=[tuple(lookup[i] for i in f) for f in faces]
 ob=m.mesh('Anatomical head',vs,fs,'Face135' if female else 'Face035');ob.data.materials.append(m.MATS['Skin35'])
 for poly in ob.data.polygons:
  center=sum((ob.data.vertices[i].co for i in poly.vertices),Vector())/len(poly.vertices)
  # One continuous face material; vertex red blends the portrait into neutral skin.
 # Close the cut at the shirt collar with a level anatomical neck hem.
 bm=bmesh.new();bm.from_mesh(ob.data)
 for v in bm.verts:
  if v.co.z<-.19 and any(e.is_boundary for e in v.link_edges):v.co.z=-.246
 bm.to_mesh(ob.data);bm.free()
 # Preserve topology around the eyelids and lips; decimate larger flatter regions only.
 bpy.context.view_layer.objects.active=ob;ob.select_set(True)
 dec=ob.modifiers.new('Game topology','DECIMATE');dec.ratio=.88;bpy.ops.object.modifier_apply(modifier=dec.name)
 # The anatomical neck is already complete; a second tube caused intersecting skin panels.
 if not female:HEAD_BVH=BVHTree.FromPolygons([v.co for v in ob.data.vertices],[tuple(p.vertices) for p in ob.data.polygons])
 for group in ['helper-l-eye','helper-r-eye']:
  faces=MH_FACES[group];inds=sorted({i for f in faces for i in f});lookup={i:j for j,i in enumerate(inds)}
  m.mesh('Eyeball', [point(MH_VERT[i]) for i in inds],[tuple(lookup[i] for i in f) for f in faces],'Eye35')
  eye=[point(MH_VERT[i]) for i in inds];y=sum(v[1] for v in eye)/len(eye);z=sum(v[2] for v in eye)/len(eye);xx=max(v[0] for v in eye)
  for rad,offset,mat in [(.0056,.0010,'Iris35'),(.0027,.0015,'Pupil35')]:
   m.mesh('Eye color',[(xx+offset,y,z)]+[(xx+offset-.014+math.sqrt(.014**2-rad**2),y+rad*math.cos(i*math.tau/20),z+rad*math.sin(i*math.tau/20)) for i in range(21)],[(0,i+1,i+2) for i in range(20)],mat)

TOPM=['Cotton35','Cotton35','Cotton35','Leather35','Knit35','Canvas35','Canvas35','Cotton35','Skin35']
def torso(style,female=False):
 mat=TOPM[style];f=.94 if female else 1;loose=[.006,0,.009,.018,.024,.019,.012,.011,0][style]
 rows=[(0,0,-.49,.107+loose,.145*f+loose),(-.006,0,-.43,.111+loose,.145*f+loose),(-.004,0,-.36,.106+loose,.147*f+loose),(0,0,-.29,.117+loose,.165*f+loose),(0,0,-.20,.131+loose,.190*f+loose),(0,0,-.12,.137+loose,.205*f+loose),(-.005,0,-.055,.118+loose,.212*f+loose),(-.012,0,-.02,.08+loose,.165*f+loose),(-.009,0,.005,.047,.055)]
 if style==5:rows=[(0,0,-.69,.14,.19),(0,0,-.60,.125,.17)]+rows
 def cut(p,a,j):
  x,y,z=p
  if female:x+=max(0,math.cos(a))**3*.024*gauss(abs(y),.08,.055)*gauss(z,-.17,.075)
  x+=.006*math.cos(a*2)*gauss(z,-.20,.07)
  return(x,y,z)
 surface(rows,mat,32,.0028 if style not in [1,8] else .001,cut)
 if style==8 and female:surface([(0,0,-.25,.130,.183),(.006,0,-.19,.163,.2),(.003,0,-.115,.151,.206)],'Cotton35',32)
 if style in [0,3,5,6,7]:
  for s in [-1,1]:
   m.mesh('Folded lapel',[(.052,s*.045,.002),(.106,s*.11,-.044),(.145,s*.055,-.13),(.103,s*.025,-.025)],[(0,1,2,3)],mat)
   patch(.147+loose,s*.08,-.19,.075,.085,mat);patch(.149+loose,s*.08,-.144,.082,.019,mat)
   if style in [3,5,6]:patch(.125+loose,s*.08,-.40,.08,.065,mat)
  ribbon([(.12+loose,0,-.46),(.143+loose,0,-.22),(.12+loose,0,-.06)],.005,'Metal35' if style==3 else 'Cotton35')
 if style==4:
  surface([(-.038,0,-.04,.073,.09),(-.05,0,.015,.075,.088),(-.038,0,.054,.044,.06)],mat,24,.002)
  for s in [-1,1]:ribbon([(.095,s*.04,-.03),(.141,s*.038,-.19)],.0025,'Cotton35')
 if style==6:
  for s in [-1,1]:patch(.16,s*.10,-.3,.09,.13,'Canvas35')

def hand():
 surface([(.028,0,-.48,.028,.024),(.030,0,-.515,.027,.035),(.036,0,-.55,.024,.036),(.04,0,-.572,.021,.035)],'Skin35',16)
 for i in range(4):
  y=-.026+i*.017;end=-.64+[.013,0,.005,.021][i]
  surface([(.04,y,-.555,.009,.008),(.043,y,-.585,.009,.008),(.05,y,-.610,.0075,.007),(.055,y,end,.004,.005)],'Skin35',8)
 surface([(.035,-.031,-.51,.013,.012),(.05,-.045,-.535,.012,.010),(.06,-.048,-.565,.007,.007)],'Skin35',10)
def arm(style):
 end=[-.28,-.055,-.21,-.475,-.48,-.48,-.055,-.30,.02][style];mat=TOPM[style]
 rows=[(0,0,.017,.050,.05),(.002,0,-.028,.068,.063),(.006,0,-.09,.065,.058),(.012,0,-.17,.052,.05),(.016,0,-.26,.043,.042),(.021,0,-.29,.046,.04),(.025,0,-.37,.047,.035),(.028,0,-.44,.033,.027),(.028,0,-.485,.027,.024)]
 nearest=min(rows,key=lambda r:abs(r[2]-end));skinrows=[(nearest[0],0,end-.0005,nearest[3],nearest[4])]+[r for r in rows if r[2]<end-.001]
 surface(skinrows,'Skin35',24,shape=lambda p,a,j:(p[0]+.008*max(0,math.cos(a))*gauss(p[2],-.14,.06),p[1],p[2]))
 if style!=8:
  cloth=[(x,y,z,rx+.006,ry+.006) for x,y,z,rx,ry in rows if z>=end];r=min(rows,key=lambda r:abs(r[2]-end));cloth.append((r[0],0,end,r[3]+.006,r[4]+.006));surface(cloth,mat,24,.0025)
 hand()
def pelvis(style,female=False):
 mat=['Denim35','Canvas35','Cotton35','Denim35','Knit35','Knit35'][style];w=1.05 if female else 1
 surface([(0,0,.015,.111,.15*w),(-.008,0,-.025,.120,.153*w),(-.012,0,-.085,.12,.16*w),(-.005,0,-.14,.077,.134*w)],mat,28,.001)
 surface([(0,0,.013,.114,.151*w),(0,0,-.014,.118,.156*w)],'Leather35',28)
 patch(.121,0,0,.039,.031,'Metal35')
 for s in [-1,1]:patch(-.128,s*.072,-.078,.066,.065,mat)
def leg(style,shoe=False):
 mat=['Denim35','Canvas35','Cotton35','Denim35','Knit35','Knit35'][style];wide=[1,1.1,.95,1.05,1.08,.88][style]
 rows=[(0,0,.025,.078,.077),(0,0,-.045,.084,.079),(-.004,0,-.13,.083,.075),(0,0,-.24,.069,.064),(.016,0,-.34,.056,.054),(.014,0,-.395,.052,.050),(-.002,0,-.48,.059,.052),(-.014,0,-.57,.05,.043),(-.01,0,-.67,.036,.034),(-.01,0,-.74,.033,.034)]
 rows=[(x,y,z,rx*wide,ry*wide) for x,y,z,rx,ry in rows];surface(rows,'Skin35' if style==3 else mat,24,.002 if style!=5 else .0005)
 if style==3:surface([(x,y,z,rx+.009,ry+.009) for x,y,z,rx,ry in rows[:5]],mat,24,.003)
 if style==1:
  for s in [-1,1]:patch(.033,s*.081,-.22,.065,.105,mat)
 if shoe:boot(0)
def boot(style):
 mat='Leather35' if style in [0,2,3] else 'Canvas35';h=[-.64,-.72,-.56,-.69][style]
 surface([(-.01,0,h,.039,.040),(-.008,0,-.735,.043,.044),(.018,0,-.775,.077,.047),(.045,0,-.81,.108,.052),(.045,0,-.829,.11,.052)],mat,24)
 surface([(.045,0,-.828,.111,.053),(.045,0,-.847,.111,.053)],'Rubber35',24)
 for z in [-.75,-.767,-.782]:ribbon([(.056,-.025,z),(.06,.025,z-.004)],.002,'Cotton35')

def hair(style):
 if style==0:return
 # A single shaped scalp shell with continuous coiffure; no bead/tube stack.
 n=48;rows=16;vs=[];short=style in [1,2,3,4,7,8,13,14,17];length={5:.19,6:.11,9:.10,10:.09,11:.22,12:.15,15:.08,16:.09,18:.21,19:.26}.get(style,.025)
 for j in range(rows):
  t=j/(rows-1)
  for i in range(n):
   a=i*math.tau/n;front=max(0,math.cos(a));bottom=.012-.065*(1-front)+.018*abs(math.sin(a))**8;bottom+=.0025*math.sin(a*19)+.0015*math.cos(a*31)
   if style in [3,4,13,14]:bottom+=.022*front
   if not short:bottom-=length*(1-max(0,min(1,(math.cos(a)-.2)/.45)))
   scalpz=.076*(1-t)+bottom*t
   hp=[(-.5,.084,.080),(-.03,.089,.083),(.025,.084,.079),(.058,.062,.061),(.076,.003,.003)]
   for aa,bb in zip(hp,hp[1:]):
    if aa[0]<=scalpz<=bb[0]:tt=(scalpz-aa[0])/(bb[0]-aa[0]);xx=aa[1]*(1-tt)+bb[1]*tt+.005;yy=aa[2]*(1-tt)+bb[2]*tt+.005;break
   z=scalpz+({1:.003,2:.017,3:.014,4:.012,13:.04,14:.008}.get(style,.016))*(1-t)
   if style==7:xx*=1.15;yy*=1.22;z+=.035*(1-t)
   if style==8:yy*=.30;z+=.07*(1-t)
   if style in [3,4,13]:z+=.022*gauss(math.sin(a),.3,.55)*math.sin(t*math.pi)
   if style in [7,12,17]:ripple=.004*(math.sin(a*13+t*31)+.6*math.sin(a*21-t*14));xx+=ripple;yy+=ripple
   x=-.014+xx*math.cos(a);y=yy*math.sin(a)
   if style==4:x-=.012*(1-t)
   if style==3:y+=.011*(1-t)
   if style==2:z-=.009*front*t*t
   if style==18:x+=.008*math.sin(t*18+a*6)*t
   if style==19:y+=.008*math.sin(t*15+a*8)*t

   if not short and t>.6:x-=.014*(t-.6);y*=1.04
   ray=Vector((math.cos(a),math.sin(a),0));hit=HEAD_BVH.ray_cast(Vector((-.014,0,z))+ray*.35,-ray)
   if hit[0]:
    radius=(Vector((x+.014,y,0))).length;required=(hit[0]-Vector((-.014,0,z))).length+.0035
    if required>radius:x=-.014+required*math.cos(a);y=required*math.sin(a)
   # Alternate tapered tips instead of a continuous straight lower rim.
   if j==rows-1 and style!=1:z-=.004+(i%3)*.004
   vs.append((x,y,z))
 # Separate overlapping locks follow the scalp surface, with raised centres and tapered ends.
 # The base fills the scalp while these bevelled strips give the silhouette real strands.
 if style!=1:
  for strand in range(n):
   lock=[];uv=[];start=2+(strand%4);end=rows-1
   for j in range(start,end+1):
    t=(j-start)/max(1,end-start);angle=(strand+(.65*math.sin(t*5+strand) if style in [7,12,17,19] else .3*t))*math.tau/n
    angle+={3:1.35,4:.9,13:1.1}.get(style,.12)*(1-t)**1.3
    i=math.floor(angle/math.tau*n)%n;p0=Vector(vs[j*n+i]);p1=Vector(vs[j*n+(i+1)%n]);center=p0.lerp(p1,(angle/math.tau*n)%1)
    radial=Vector((math.cos(angle),math.sin(angle),.28*(1-t))).normalized();side=Vector((-math.sin(angle),math.cos(angle),0))
    width=(.0035 if style in [2,14,17] else .005)*(max(.04,math.sin(math.pi*(.08+.9*t))))
    if j==end:center.z-=.006+(strand%5)*.0015;width=.00025
    for k in range(3):
     u=k/2;v=center+side*((u*2-1)*width)+radial*(.0035+math.sin(u*math.pi)*.0035*math.sin(math.pi*t))
     lock.append(tuple(v));uv.append((u,t))
   f=[(j*3+k,j*3+k+1,(j+1)*3+k+1,(j+1)*3+k) for j in range(end-start) for k in range(2)]
   ob=m.mesh('Layered tapered hair lock',lock,f,'Hair35')
 fs=[(j*n+i,j*n+(i+1)%n,(j+1)*n+(i+1)%n,(j+1)*n+i) for j in range(rows-1) for i in range(n)];fs.append(tuple(range(n-1,-1,-1)));m.mesh('Sculpted coiffure',vs,fs,'Hair35')
 if style in [9,10,15,16]:
  pts=[(-.096,0,.024,.025,.025),(-.135,0,.016,.028,.027),(-.152,.005,-.05,.024,.02),(-.145,.01,-.13,.018,.017),(-.13,.014,-.22,.006,.008)]
  if style in [10,16]:pts=[(-.096,0,.035,.026,.03),(-.14,0,.035,.035,.036),(-.16,0,.035,.008,.009)]
  if style==16:pts=[(x,y,z-.07,rx,ry) for x,y,z,rx,ry in pts]
  surface(pts,'Hair35',20,.002 if style==15 else 0)

def beard(style):
 if style==0:return
 profile=[(-.168,.039,.036,.025),(-.145,.052,.05,.015),(-.11,.069,.064,.002),(-.07,.08,.075,-.004)]
 def point(y,z):
  hit=HEAD_BVH.ray_cast(Vector((.4,y,z)),Vector((-1,0,0)))
  return (hit[0].x+.0015 if hit[0] and hit[0].x>.018 else -1,y,z)

 if style not in [1,3]:
  vs=[];n=29;rows=13;span={2:.33,5:.55,7:.30}.get(style,.96)
  for j in range(rows):
   z=-.083-j*.083/(rows-1)
   for a,b in zip(profile,profile[1:]):
    if a[0]<=z<=b[0]:t=(z-a[0])/(b[0]-a[0]);ry=a[2]*(1-t)+b[2]*t;break
   for i in range(n):
    y=(-1+2*i/(n-1))*ry*span;pp=point(y,z)
    if style>=8 and j>8:pp=(pp[0]+.002*(j-8),pp[1],pp[2]-(j-8)*(.009 if style==9 else .004))
    vs.append(pp)
  fs=[]
  for j in range(rows-1):
   for i in range(n-1):
    if any(vs[k][0]<0 for k in [j*n+i,j*n+i+1,(j+1)*n+i+1,(j+1)*n+i]):continue
    mid=(Vector(vs[j*n+i])+Vector(vs[(j+1)*n+i+1]))*.5
    cheekline=-.100+.035*(min(1,abs(mid.y)/.067)**1.5)
    if mid.z>cheekline:continue
    if -.125<mid.z<-.100 and abs(mid.y)<.027:continue
    if style==4 and j<5:continue
    fs.append((j*n+i,j*n+i+1,(j+1)*n+i+1,(j+1)*n+i))
  m.mesh('Conformal facial hair',vs,fs,'Hair35')
 if style!=4:
  vs=[point(-.029+i*.058/12,-.096-j*.009) for j in range(2) for i in range(13)]
  m.mesh('Moustache',vs,[(i,i+1,14+i,13+i) for i in range(12) if all(vs[k][0]>0 for k in [i,i+1,14+i,13+i])],'Hair35')


old=json.loads((m.OUT/'models_35_manifest.json').read_text())
for female in [False,True]:
 head(female);finish(('Female' if female else 'Male')+'AnatomicalHead35')
for style in range(1,20):hair(style);finish('Hair%02d35'%style)
for name,rec in m.RECORDS.items():rec['collision']='convex';rec['local_bounds_m']=m.bounds(m.OBJECTS[name])
m.export_all()
names=set(m.RECORDS)
old['assets']=[a for a in old['assets'] if a['name'] not in names]+list(m.RECORDS.values())
(m.OUT/'models_35_manifest.json').write_text(json.dumps(old,indent=2))
(m.OUT/'models_38_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values()),'materials':SURF},indent=2))
bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'AllAmericanMeltdown_HeadsHair38.blend'))
print('CHARACTERS38_COMPLETE',len(m.OBJECTS),flush=True)

