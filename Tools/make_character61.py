"""One approval-only survivor. Builds real meshes, packed Blender source and FBX.
Does not replace any runtime assets. Anatomy derived from the existing CC0 hm08.
"""
import bpy, bmesh, math, random, json
from pathlib import Path
from mathutils import Vector
from mathutils.bvhtree import BVHTree

ROOT=Path('X:/LethalWorld'); OUT=ROOT/'ArtSource/Character61'; OUT.mkdir(exist_ok=True)
random.seed(61)
bpy.ops.object.select_all(action='SELECT'); bpy.ops.object.delete(use_global=False)
bpy.context.preferences.filepaths.save_version=0
sc=bpy.context.scene; sc.unit_settings.system='METRIC'
atlas=bpy.data.images.load(str(OUT/'T_SurvivorMaterials61.png'))
portrait=bpy.data.images.load(str(OUT/'T_SurvivorFace61.png'))

def material(name,cell=None,color=(.2,.2,.2),rough=.7):
 m=bpy.data.materials.new(name);m.use_nodes=True;n=m.node_tree.nodes;l=m.node_tree.links;p=n.get('Principled BSDF');p.inputs['Roughness'].default_value=rough
 p.inputs['Base Color'].default_value=(*color,1)
 if cell is not None:
  tex=n.new('ShaderNodeTexImage');tex.image=atlas
  uv=n.new('ShaderNodeUVMap');uv.uv_map='MaterialUV'
  scale=n.new('ShaderNodeVectorMath');scale.operation='SCALE';scale.inputs[3].default_value=.47
  offset=n.new('ShaderNodeVectorMath');offset.operation='ADD';offset.inputs[1].default_value=((cell%2)*.5+.015,(1-cell//2)*.5+.015,0)
  fract=n.new('ShaderNodeVectorMath');fract.operation='FRACTION'
  l.new(uv.outputs[0],fract.inputs[0]);l.new(fract.outputs[0],scale.inputs[0]);l.new(scale.outputs[0],offset.inputs[0]);l.new(offset.outputs[0],tex.inputs[0]);l.new(tex.outputs['Color'],p.inputs['Base Color'])
  bump=n.new('ShaderNodeBump');bump.inputs['Strength'].default_value=.075;bump.inputs['Distance'].default_value=.0001;l.new(tex.outputs['Color'],bump.inputs['Height']);l.new(bump.outputs[0],p.inputs['Normal'])
 return m
cloth=material('61 | olive cotton twill',0);leather=material('61 | weathered leather',1,rough=.6)
skin=material('61 | skin',2);hairmat=material('61 | layered chestnut hair',3,rough=.82)
hairmat.node_tree.nodes.get('Principled BSDF').inputs['Specular IOR Level'].default_value=.14
if (OUT/'T_HairCards61.png').exists():
 hairmat=material('61 | alpha strand hair cards',color=(.04,.018,.01),rough=.78)
 hn=hairmat.node_tree.nodes;hl=hairmat.node_tree.links;hp=hn.get('Principled BSDF');hp.inputs['Specular IOR Level'].default_value=.14
 ht=hn.new('ShaderNodeTexImage');ht.image=bpy.data.images.load(str(OUT/'T_HairCards61.png'));ht.image.pack();hu=hn.new('ShaderNodeUVMap');hu.uv_map='MaterialUV';hl.new(hu.outputs[0],ht.inputs[0]);hl.new(ht.outputs['Color'],hp.inputs['Base Color']);hl.new(ht.outputs['Alpha'],hp.inputs['Alpha'])
pants=material('61 | charcoal canvas',0);pn=pants.node_tree.nodes;pl=pants.node_tree.links;pp=pn.get('Principled BSDF');src=pp.inputs['Base Color'].links[0].from_socket
mix=pn.new('ShaderNodeMixRGB');mix.blend_type='MULTIPLY';mix.inputs[0].default_value=1;mix.inputs[2].default_value=(.35,.40,.47,1);pl.new(src,mix.inputs[1]);pl.new(mix.outputs[0],pp.inputs['Base Color'])
rubber=material('61 | boot soles',color=(.023,.025,.028),rough=.88)
thread=material('61 | stitching',color=(.27,.28,.18));metal=material('61 | aged hardware',color=(.18,.17,.13),rough=.42);metal.node_tree.nodes.get('Principled BSDF').inputs['Metallic'].default_value=.8
shirt=material('61 | undershirt',color=(.13,.12,.10),rough=.92)
eyewhite=material('61 | ivory sclera',color=(.58,.54,.46),rough=.26)
iris=material('61 | hazel iris',color=(.13,.078,.027),rough=.27)
it=iris.node_tree.nodes.new('ShaderNodeTexImage');it.image=portrait
iu=iris.node_tree.nodes.new('ShaderNodeUVMap');iu.uv_map='MaterialUV'
iris.node_tree.links.new(iu.outputs[0],it.inputs[0]);iris.node_tree.links.new(it.outputs['Color'],iris.node_tree.nodes.get('Principled BSDF').inputs['Base Color'])
pupil=material('61 | pupil',color=(.004,.003,.002),rough=.16)

def mesh(name,verts,faces,mat,uv=None):
 me=bpy.data.meshes.new(name);me.from_pydata(verts,[],faces);me.update();o=bpy.data.objects.new(name,me);bpy.context.collection.objects.link(o);me.materials.append(mat)
 layer=me.uv_layers.new(name='MaterialUV')
 for face in me.polygons:
  face.use_smooth=True
  for li in face.loop_indices:
   vi=me.loops[li].vertex_index;v=me.vertices[vi].co
   layer.data[li].uv=uv[vi] if uv else (v.y*4+.5,v.z*4)
 return o

def curve(name,pts,r,mat):
 c=bpy.data.curves.new(name,'CURVE');c.dimensions='3D';c.resolution_u=2;c.bevel_depth=r;c.bevel_resolution=1;s=c.splines.new('POLY');s.points.add(len(pts)-1)
 for p,v in zip(s.points,pts):p.co=(*v,1)
 o=bpy.data.objects.new(name,c);bpy.context.collection.objects.link(o);o.data.materials.append(mat);return o

vs=[];groups={};g=''
for line in (ROOT/'ArtSource/Characters35/Reference/base.obj').read_text().splitlines():
 if line.startswith('v '):
  x,z,y=map(float,line.split()[1:]);vs.append(Vector((y*.108,x*.108,(z+8.1676)*.108)))
 elif line.startswith('g '):g=line[2:];groups[g]=[]
 elif line.startswith('f '):groups[g].append(tuple(int(t.split('/')[0])-1 for t in line.split()[1:]))

def extract(name,predicate,mat,inflate=0,fold=False):
 fs=[f for f in groups['body'] if predicate(sum((vs[i] for i in f),Vector())/len(f))]
 ids=sorted({i for f in fs for i in f});index={v:i for i,v in enumerate(ids)}
 o=mesh(name,[vs[i] for i in ids],[tuple(index[i] for i in f) for f in fs],mat)
 if inflate:
  for v in o.data.vertices:
   p=v.co;f=inflate
   if fold:f+=.002*math.sin(p.z*119+p.y*38)+.0015*math.sin(p.z*67-p.y*46)
   v.co+=v.normal*f
 return o

# Continuous skin topology; clothing covers the anatomical source rather than replacing joints with primitives.
body=extract('Survivor61 | anatomical body',lambda p:True,skin)
faceuv=body.data.uv_layers.new(name='FaceUV')
mask=body.data.color_attributes.new(name='FaceBlend',type='FLOAT_COLOR',domain='CORNER')
def smooth(a,b,x):
 t=max(0,min(1,(x-a)/(b-a)));return t*t*(3-2*t)
def facev(z):
 rows=[(1.51,0),(1.544,.045),(1.593,.232),(1.626,.365),(1.669,.556),(1.80,1)]
 for a,b in zip(rows,rows[1:]):
  if a[0]<=z<=b[0]:return a[1]+(b[1]-a[1])*(z-a[0])/(b[0]-a[0])
 return 0 if z<1.51 else 1
for loop in body.data.loops:
 v=body.data.vertices[loop.vertex_index].co;faceuv.data[loop.index].uv=(.5-v.y/.206,facev(v.z))
 w=smooth(.065,.115,v.x)*smooth(1.515,1.555,v.z)*(1-smooth(.068,.093,abs(v.y)))
 mask.data[loop.index].color=(w,w,w,1)
skinface=skin.copy();skinface.name='61 | blended face and skin';body.data.materials[0]=skinface
n=skinface.node_tree.nodes;l=skinface.node_tree.links;p=n.get('Principled BSDF');base=p.inputs['Base Color'].links[0].from_socket
uv=n.new('ShaderNodeUVMap');uv.uv_map='FaceUV';tx=n.new('ShaderNodeTexImage');tx.image=portrait;l.new(uv.outputs[0],tx.inputs[0]);vc=n.new('ShaderNodeVertexColor');vc.layer_name='FaceBlend';mx=n.new('ShaderNodeMixRGB');l.new(vc.outputs['Color'],mx.inputs[0]);l.new(base,mx.inputs[1]);l.new(tx.outputs['Color'],mx.inputs[2]);l.new(mx.outputs[0],p.inputs['Base Color']);p.inputs['Subsurface Weight'].default_value=.025

for group in ['helper-l-eye','helper-r-eye']:
 fs=groups[group];ids=sorted({i for f in fs for i in f});ix={v:i for i,v in enumerate(ids)};eye=mesh('Survivor61 | eye',[vs[i] for i in ids],[tuple(ix[i] for i in f) for f in fs],eyewhite)
 center=sum((vs[i] for i in ids),Vector())/len(ids);front=max(vs[i].x for i in ids)
 for radius,mat,depth in [(.0066,iris,.0003),(.0029,pupil,.0007)]:
  vv=[(front+depth,center.y,center.z)]+[(front+depth-.0006,center.y+radius*math.cos(a*math.tau/32),center.z+radius*math.sin(a*math.tau/32)) for a in range(32)]
  eyeuv=[(.346,.558)]+[(.346+.025*math.cos(a*math.tau/32),.558+.025*math.sin(a*math.tau/32)) for a in range(32)]
  mesh('Survivor61 | iris detail',vv,[(0,1+i,1+(i+1)%32) for i in range(32)],mat,eyeuv)

jacket=extract('Survivor61 | fitted field jacket',lambda p:1.015<p.z<1.555 and abs(p.y)<.408,cloth,.018,False)
trousers=extract('Survivor61 | cargo trousers',lambda p:.195<p.z<1.055 and abs(p.y)<.29,pants,.014,True)
for ob in [jacket,trousers]:
 bpy.context.view_layer.objects.active=ob;ob.select_set(True)
 mod=ob.modifiers.new('Tailored cloth relaxation','SMOOTH');mod.factor=.8;mod.iterations=8;bpy.ops.object.modifier_apply(modifier=mod.name)
 for v in ob.data.vertices:
  p=v.co
  if ob==jacket:
   if p.z<1.045:p.z=1.035
   if abs(p.y)>.385:p.y=math.copysign(.397,p.y)
   if p.z>1.535:p.z=1.548
   if 1.20<p.z<1.43 and p.x>.10:
    weight=math.sin(math.pi*(p.z-1.20)/.23)**.4
    p.x=p.x*(1-weight)+(.159-.12*abs(p.y))*weight
   if 1.06<p.z<1.3:p.y*=1+.075*math.sin(math.pi*(p.z-1.06)/.24)
  else:
   if p.z<.222:p.z=.212
  p+=v.normal*(.0018*math.sin(p.z*110+p.y*24)+.0011*math.sin(p.z*70-p.y*32))
 mod=ob.modifiers.new('Garment thickness','SOLIDIFY');mod.thickness=.002;bpy.ops.object.modifier_apply(modifier=mod.name);ob.select_set(False)
# Remove covered body faces to keep this dressed sample lean and prevent skin intersections.
bm=bmesh.new();bm.from_mesh(body.data)
bmesh.ops.delete(bm,geom=[f for f in bm.faces if f.calc_center_median().z<.245 or (.18<f.calc_center_median().z<1.50 and abs(f.calc_center_median().y)<.38)],context='FACES');bm.to_mesh(body.data);bm.free()

for side in [-1,1]:
 ends=[v.co for v in jacket.data.vertices if side*v.co.y>.394]
 center=sum(ends,Vector())/len(ends);bins=[]
 for i in range(24):
  a=math.tau*i/24
  v=min(ends,key=lambda p:abs(math.atan2(math.sin(math.atan2(p.z-center.z,p.x-center.x)-a),math.cos(math.atan2(p.z-center.z,p.x-center.x)-a))))
  bins.append(Vector((v.x,side*.403,v.z)))
 cv=[p+Vector((0,-side*.017,0)) for p in bins]+bins
 cuff=mesh('Jacket | rolled cuff',cv,[(i,(i+1)%24,24+(i+1)%24,24+i) for i in range(24)],cloth)
 curve('Jacket | cuff seam',bins+[bins[0]],.0012,thread)

def bvh(ob):return BVHTree.FromPolygons([v.co for v in ob.data.vertices],[tuple(p.vertices) for p in ob.data.polygons])
jb=bvh(jacket);pb=bvh(trousers)
def project(tree,y,z,offset=.004):
 hit=tree.ray_cast(Vector((.6,y,z)),Vector((-1,0,0)))
 return (hit[0]+Vector((offset,0,0))) if hit[0] else Vector((.13,y,z))
def seam(name,tree,yz,r=.0008):return curve(name,[project(tree,y,z,.003) for y,z in yz],r,thread)

# Sewn construction follows the actual surface; functional-looking pockets have depth and flaps.
for side in [-1,1]:
 for z,w,h in [(1.335,.081,.085),(1.115,.095,.082)]:
  cy=side*.089;v=[project(jb,cy-w/2,z-h/2,.006),project(jb,cy+w/2,z-h/2,.006),project(jb,cy+w/2,z+h/2,.004),project(jb,cy-w/2,z+h/2,.004),project(jb,cy,z,.019)]
  po=mesh('Jacket | bellows pocket',v,[(0,1,4),(1,2,4),(2,3,4),(3,0,4)],cloth)
  curve('Pocket | stitched edge',[v[i]+Vector((.0007,0,0)) for i in [0,1,2,3,0]],.00075,thread)
  flap=[project(jb,cy-w*.55,z+h/2+.014,.009),project(jb,cy+w*.55,z+h/2+.014,.009),project(jb,cy+w*.43,z+h/2-.016,.019),project(jb,cy,z+h/2-.024,.021),project(jb,cy-w*.43,z+h/2-.016,.019)]
  mesh('Jacket | folded pocket flap',flap,[(0,1,2,3,4)],cloth)
 seam('Jacket | tailored chest seam',jb,[(side*(.034+i*.003),1.43-i*.006) for i in range(27)])
 seam('Trouser | knee panel',pb,[(side*(.16+.029*math.cos(i*math.tau/32)),.53+.035*math.sin(i*math.tau/32)) for i in range(33)])
 # Trouser cargo pocket mounted on the outside of each thigh.
 y=side*.142;z=.78;v=[(.035,y,z+.06),(.085,y,z+.04),(.084,y,z-.07),(.024,y,z-.07),(.053,y+side*.017,z)]
 mesh('Trousers | thigh cargo pocket',v,[(0,1,4),(1,2,4),(2,3,4),(3,0,4)],pants)
 curve('Trousers | cargo stitching',[v[i] for i in [0,1,2,3,0]],.0009,thread)

# Center placket, raised collar and zipper hardware.
seam('Jacket | center placket left',jb,[(-.008,1.035+i*.011) for i in range(42)],.0012)
seam('Jacket | center placket right',jb,[(.008,1.035+i*.011) for i in range(42)],.0012)
for i in range(6):
 p=project(jb,0,1.08+i*.071,.009)
 bpy.ops.mesh.primitive_cylinder_add(vertices=12,radius=.0042,depth=.0018,location=p,rotation=(0,math.pi/2,0));bpy.context.object.name='Jacket | brass snap';bpy.context.object.data.materials.append(metal)
for side in [-1,1]:
 mesh('Jacket | shaped collar',[(.048,side*.047,1.554),(.079,side*.067,1.526),(.125,side*.058,1.471),(.106,side*.027,1.493),(.069,side*.026,1.552)],[(0,1,2,3,4)],cloth)
 curve('Jacket | collar piping',[(.048,side*.047,1.554),(.079,side*.067,1.526),(.125,side*.058,1.471),(.106,side*.027,1.493)],.0012,thread)

# Anatomically fitted belt rather than a floating box.
vv=[]
for z in [1.028,1.064]:
 for i in range(64):
  a=i*math.tau/64;vv.append((.123*math.cos(a)+.01,.156*math.sin(a),z))
mesh('Survivor61 | belt',vv,[(i,(i+1)%64,64+(i+1)%64,64+i) for i in range(64)],leather)
curve('Belt | buckle',[(.142,-.022,1.030),(.142,.022,1.030),(.142,.022,1.063),(.142,-.022,1.063),(.142,-.022,1.030)],.0025,metal)

# Boot sole and lace stitching follow the original foot shape.
for side in [-1,1]:
 cy=side*.231
 # Shaped toe box, instep, heel and ankle rings; no individual anatomical toes.
 bv=[];rows=[(.010,.070,.143,.060),(.035,.072,.141,.059),(.069,.066,.132,.056),(.098,.036,.093,.050),(.132,.000,.054,.044),(.185,-.002,.046,.044),(.244,-.002,.047,.045)]
 for z,x,rx,ry in rows:
  for i in range(32):
   a=i*math.tau/32;bv.append((x+rx*math.cos(a),cy+ry*math.sin(a),z))
 boots=mesh('Boot | shaped upper',bv,[(j*32+i,j*32+(i+1)%32,(j+1)*32+(i+1)%32,(j+1)*32+i) for j in range(len(rows)-1) for i in range(32)],leather)
 bb=bvh(boots)
 for z in [.075,.093,.111,.131,.155,.181,.205]:
  curve('Boot | crossed lace',[project(bb,cy-.026,z,.004),project(bb,cy+.025,z+.012,.004)],.0015,thread)
  curve('Boot | crossed lace',[project(bb,cy+.026,z,.004),project(bb,cy-.025,z+.012,.004)],.0015,thread)
 # Outline under the foot is a sculpted polygon sole with rounded toe.
 vs2=[]
 for z in [.007,.027]:
  for i in range(32):
   a=i*math.tau/32;vs2.append((.072+.149*math.cos(a),cy+.062*math.sin(a),z))
 mesh('Boot | tread sole',vs2,[tuple(range(31,-1,-1)),tuple(range(32,64))]+[(i,(i+1)%32,32+(i+1)%32,32+i) for i in range(32)],rubber)

# Hair cards follow overlapping combed scalp paths, taper to split ends and have strand-direction UVs.
# Each lock has a root and a flexible three-joint chain, exported with a baked spring response.
armdata=bpy.data.armatures.new('Survivor61_Rig');rig=bpy.data.objects.new('Survivor61_Rig',armdata);bpy.context.collection.objects.link(rig);bpy.context.view_layer.objects.active=rig;rig.select_set(True);bpy.ops.object.mode_set(mode='EDIT')
root=armdata.edit_bones.new('root');root.head=(0,0,0);root.tail=(0,0,.2)
headbone=armdata.edit_bones.new('head');headbone.head=(.06,0,1.53);headbone.tail=(.06,0,1.79);headbone.parent=root
bpy.ops.object.mode_set(mode='OBJECT');rig.select_set(False)
hairlocks=[]
capmat=material('Hair61 | roots',3,rough=.9)
cap=extract('Hair61 | fitted scalp underlayer',lambda p:p.z>1.68+.064*smooth(-.02,.12,p.x),capmat,.002)
bm=bmesh.new();bm.from_mesh(cap.data)
for v in bm.verts:
 if any(e.is_boundary for e in v.link_edges):v.co.z=1.688+.064*smooth(-.02,.12,v.co.x)
bm.to_mesh(cap.data);bm.free()
for layer in range(3):
 for i in range(32):
  a=i*math.tau/32+layer*.047;front=max(0,math.cos(a));back=max(0,-math.cos(a));pts=[]
  for j in range(9):
   t=j/8;theta=.06+t*(1.45+.40*back+.15*front+.16*math.sin(a));aa=a+.43*t+.04*math.sin(t*math.pi)
   radius=1+layer*.035
   x=.062+.100*math.sin(theta)*math.cos(aa)*radius
   y=.085*math.sin(theta)*math.sin(aa)*radius
   z=1.710+.108*math.cos(theta)*radius-.035*t*t*back
   # Combed fringe sweeps to one side, never a solid helmet edge.
   z+=.008*math.sin(i*2.1)*t;x+=.003*math.sin(j+i);pts.append(Vector((x,y,z)))
  width=.015+random.random()*.005;verts=[];uvs=[]
  for j,p in enumerate(pts):
   t=j/8;tangent=Vector((-math.sin(a),math.cos(a),0));w=width*(.85+.15*math.sin(t*math.pi))*(1-.85*t**5)
   for c in range(3):verts.append(p+tangent*w*(c-1)+Vector((0,0,.0015*(c==1))));uvs.append(((i%8+.07+c*.43)/8,1-t*.99))
  o=mesh('Hair61 | layered lock %03d'%(layer*32+i),verts,[(j*3+c,j*3+c+1,(j+1)*3+c+1,(j+1)*3+c) for j in range(8) for c in range(2)],hairmat,uvs)
  hairlocks.append((o,pts,layer*32+i))
  # Fine uneven silhouette strands use low-sided curves, not large tube clumps.
  if layer==2:
   for k in [-1,1]:curve('Hair61 | flyaway',[p+Vector((.001*k,.003*k,0)) for p in pts[3:]],.00032,hairmat)

bpy.context.view_layer.objects.active=rig;rig.select_set(True);bpy.ops.object.mode_set(mode='EDIT')
for o,pts,idx in hairlocks:
 parent=armdata.edit_bones['head']
 for k in range(3):
  b=armdata.edit_bones.new('hair_%03d_%d'%(idx,k));b.head=pts[k*2];b.tail=pts[(k+1)*2];b.parent=parent;parent=b
bpy.ops.object.mode_set(mode='OBJECT')
for o,pts,idx in hairlocks:
 gs=[o.vertex_groups.new(name='hair_%03d_%d'%(idx,k)) for k in range(3)]
 for v in o.data.vertices:
  t=(v.index//3)/8*2;lo=min(2,int(t));hi=min(2,lo+1);f=t-lo;gs[lo].add([v.index],1-f,'REPLACE')
  if hi!=lo:gs[hi].add([v.index],f,'REPLACE')
 mod=o.modifiers.new('Hair strand bones','ARMATURE');mod.object=rig;o.parent=rig

# Damped angular springs, integrated at 120Hz and keyed at 24fps. Bound root stiffness
# and limited tip travel prevent wind from separating the hairstyle from the scalp.
sc.render.fps=24;sc.frame_start=1;sc.frame_end=96
for o,pts,idx in hairlocks:
 for k in range(3):
  bone=rig.pose.bones['hair_%03d_%d'%(idx,k)];bone.rotation_mode='XYZ';angle=0;velocity=0
  for frame in range(1,97):
   for sub in range(5):
    t=(frame-1+sub/5)/24;force=(math.sin(t*3.5+idx*.14)+.45*math.sin(t*7.3+idx*.3))*(1+k)*.42
    acceleration=force-35*angle-7*velocity;velocity+=acceleration/120;angle+=velocity/120
   bone.rotation_euler.x=angle;bone.rotation_euler.z=angle*.45;bone.keyframe_insert('rotation_euler',frame=frame,group=bone.name)
rig['HairPhysics']='Pinned roots; 3-joint strands; damped spring integration at 120 Hz; 24 fps baked wind sample. Runtime solver integration awaits approval.'
sc.frame_set(1)

# Convert small construction curves to exportable meshes.
for ob in list(bpy.context.scene.objects):
 if ob.type=='CURVE':
  bpy.ops.object.select_all(action='DESELECT');ob.select_set(True);bpy.context.view_layer.objects.active=ob;bpy.ops.object.convert(target='MESH')
# Reacquire loop layers after BMesh topology edits; old RNA handles can address the
# copied base layer after a new layer is allocated. Assign the final projection last.
for loop in body.data.loops:
 v=body.data.vertices[loop.vertex_index].co
 body.data.uv_layers['FaceUV'].data[loop.index].uv=(.5-v.y/.206,facev(v.z))
 w=smooth(.065,.115,v.x)*smooth(1.515,1.555,v.z)*(1-smooth(.068,.093,abs(v.y)))
 body.data.color_attributes['FaceBlend'].data[loop.index].color=(w,w,w,1)
character=[o for o in sc.objects if o.type in {'MESH','ARMATURE'}]
# Rigid head and eyes use head bone; the sample intentionally retains a neutral body pose.
for ob in character:
 if ob.type!='MESH' or ob.parent:continue
 ob.parent=rig

# Neutral material review, no gameplay grain or filters to conceal the geometry.
floor=material('Studio | floor',color=(.027,.034,.040),rough=.8)
bpy.ops.mesh.primitive_plane_add(size=200);ground=bpy.context.object;ground.name='Review stage';ground.data.materials.append(floor);ground.location.z=-.014
sc.world.use_nodes=True;sc.world.node_tree.nodes['Background'].inputs[0].default_value=(.19,.23,.29,1);sc.world.node_tree.nodes['Background'].inputs[1].default_value=.35
for loc,power,size,color in [((3,-3,4),370,3,(1,.89,.76)),((1,3,2.5),230,2,(.72,.84,1)),((-2,1,3),430,2,(1,.90,.78))]:
 bpy.ops.object.light_add(type='AREA',location=loc);o=bpy.context.object;o.data.energy=power;o.data.shape='DISK';o.data.size=size;o.data.color=color;o.rotation_euler=(Vector((0,0,1.0))-o.location).to_track_quat('-Z','Y').to_euler()
bpy.ops.object.camera_add(location=(3.7,-2.6,2.05));cam=bpy.context.object;sc.camera=cam;cam.data.type='ORTHO';cam.data.ortho_scale=2.15;cam.rotation_euler=(Vector((.05,0,.94))-cam.location).to_track_quat('-Z','Y').to_euler()
sc.render.engine='CYCLES';sc.cycles.samples=32;sc.cycles.use_denoising=True;sc.cycles.transparent_max_bounces=32;sc.render.resolution_x=1100;sc.render.resolution_y=1400;sc.render.resolution_percentage=100
sc.view_settings.view_transform='AgX'
for img in [atlas,portrait]:img.pack()
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'Survivor61_Approval.blend'))
bpy.ops.object.select_all(action='DESELECT')
for o in character:o.select_set(True)
bpy.context.view_layer.objects.active=rig
bpy.ops.export_scene.fbx(filepath=str(OUT/'Survivor61_Approval.fbx'),use_selection=True,object_types={'MESH','ARMATURE'},add_leaf_bones=False,bake_anim=True,bake_anim_use_all_actions=False,bake_anim_use_nla_strips=False,path_mode='COPY',embed_textures=True,axis_forward='-Z',axis_up='Y')
manifest={'approval_only':True,'name':'Survivor 61','mesh_objects':len([o for o in character if o.type=='MESH']),'triangles':sum(sum(len(p.vertices)-2 for p in o.data.polygons) for o in character if o.type=='MESH'),'hair_locks':len(hairlocks),'hair_bones':len(hairlocks)*3,'hair_physics':'baked damped spring review; runtime not integrated','anatomy':'CC0 MakeHuman hm08; see ../Characters35/Reference','runtime_assets_changed':False}
(OUT/'manifest.json').write_text(json.dumps(manifest,indent=2))
sc.render.filepath=str(OUT/'Survivor61_Full.png');bpy.ops.render.render(write_still=True)
cam.location=(2.7,-1.15,1.88);cam.rotation_euler=(Vector((.08,0,1.64))-cam.location).to_track_quat('-Z','Y').to_euler();cam.data.ortho_scale=.55;sc.render.resolution_x=1100;sc.render.resolution_y=1100
sc.render.filepath=str(OUT/'Survivor61_Head.png');bpy.ops.render.render(write_still=True)
cam.location=(-3,-2.4,2.0);cam.rotation_euler=(Vector((0,0,1.0))-cam.location).to_track_quat('-Z','Y').to_euler();cam.data.ortho_scale=2.12;sc.render.resolution_x=1100;sc.render.resolution_y=1400
sc.render.filepath=str(OUT/'Survivor61_Back.png');bpy.ops.render.render(write_still=True)
print('CHARACTER61_COMPLETE',json.dumps(manifest))
