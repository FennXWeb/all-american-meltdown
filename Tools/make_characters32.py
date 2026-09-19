"""PS2 character library: authored anatomical ring topology, tailored garments and fitted grooms.
Run with Blender 4.2 --background --python Tools/make_characters32.py.
Coordinates are metres, +X forward. Existing dismemberment joint pivots are retained.
"""
import sys, math, json, random
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent))
import bpy,bmesh
from mathutils import Vector
import make_models_v2 as m
m.OUT=m.ROOT/'ArtSource/Characters32';m.setup()
SURF={'Skin32':(0,(.88,.70,.58)), 'Cloth32':(1,(.31,.37,.30)),
      'Leather32':(2,(.65,.62,.57)), 'Hair32':(3,(.20,.12,.07)),
      'Lips32':(0,(.56,.25,.22)), 'SkinDead32':(0,(.50,.56,.43)),
      'ClothDead32':(1,(.26,.27,.22)), 'ClothRaider32':(1,(.35,.21,.15)),
      'Shirt32':(1,(.72,.67,.55)), 'Pants32':(1,(.22,.26,.28)),
      'Armor32':(1,(.13,.16,.18)), 'Eye32':(-1,(.55,.51,.43)),
      'Iris32':(-1,(.10,.055,.024)), 'Mouth32':(-1,(.045,.021,.019)),
      'Metal32':(-1,(.25,.27,.28)), 'Mannequin32':(-1,(.65,.57,.46))}
atlas=bpy.data.images.load(str(m.OUT/'T_CharacterSurface32.png'))
face_atlas=bpy.data.images.load(str(m.OUT/'T_CharacterFaces32.png'))
for i in range(4):SURF['Face%d32'%i]=(4+i,(1,1,1))
for name,(tile,color) in SURF.items():
 mat=bpy.data.materials.new('M_'+name);mat.use_nodes=True;mat.diffuse_color=(*color,1)
 bs=mat.node_tree.nodes.get('Principled BSDF');bs.inputs['Base Color'].default_value=(*color,1);bs.inputs['Roughness'].default_value=.86
 if tile>=0:
  nt=mat.node_tree;uv=nt.nodes.new('ShaderNodeTexCoord');mapping=nt.nodes.new('ShaderNodeVectorMath');mapping.operation='MULTIPLY_ADD'
  q=tile-4 if tile>=4 else tile
  mapping.inputs[1].default_value=(.498,.498,1) if tile>=4 else (.46,.46,1);mapping.inputs[2].default_value=((q%2)*.5+.001,(1-q//2)*.5+.001,0) if tile>=4 else ((q%2)*.5+.02,(1-q//2)*.5+.02,0)
  tex=nt.nodes.new('ShaderNodeTexImage');tex.image=face_atlas if tile>=4 else atlas;tex.interpolation='Linear'
  tint=nt.nodes.new('ShaderNodeMixRGB');tint.blend_type='MULTIPLY';tint.inputs[0].default_value=1;tint.inputs[2].default_value=(*color,1)
  nt.links.new(uv.outputs['UV'],mapping.inputs[0]);nt.links.new(mapping.outputs[0],tex.inputs['Vector']);nt.links.new(tex.outputs['Color'],tint.inputs[1]);nt.links.new(tint.outputs[0],bs.inputs['Base Color'])
  if tile>=4:
   mask=nt.nodes.new('ShaderNodeVertexColor');mask.layer_name='FaceMask'
   blend=nt.nodes.new('ShaderNodeMixRGB');blend.inputs[1].default_value=(.30,.19,.12,1) if tile<6 else (.12,.065,.035,1)
   nt.links.new(mask.outputs['Color'],blend.inputs[0]);nt.links.new(tex.outputs['Color'],blend.inputs[2]);nt.links.new(blend.outputs[0],tint.inputs[1])
 m.MATS[name]=mat

def ringmesh(rings,mat,n=24,wrinkle=0):
 verts=[]
 for j,(x,y,z,rx,ry) in enumerate(rings):
  for i in range(n):
   t=i*math.tau/n;w=1+wrinkle*math.sin(i*3+j*2.7)
   verts.append((x+rx*math.cos(t)*w,y+ry*math.sin(t)*w,z))
 faces=[tuple(range(n-1,-1,-1))]
 for j in range(len(rings)-1):
  for i in range(n):a=j*n+i;b=j*n+(i+1)%n;faces.append((a,b,b+n,a+n))
 faces.append(tuple((len(rings)-1)*n+i for i in range(n)))
 return m.mesh('Anatomical surface',verts,faces,mat)

def line(points,r,mat,n=8):
 for a,b in zip(points,points[1:]):m.cyl(a,b,r,mat,n=n)

def ell(p,s,mat,n=16):
 bpy.ops.mesh.primitive_uv_sphere_add(segments=n,ring_count=10,radius=1,location=p)
 o=bpy.context.object;o.scale=s;bpy.ops.object.transform_apply(location=False,rotation=False,scale=True);return m.add(o,mat)

def finish(name,offset=(0,0,0),scale=(1,1,1)):
 o=m.finish(name)
 used=sorted({f.material_index for f in o.data.polygons});slots=[o.data.materials[i] for i in used];indices=[used.index(f.material_index) for f in o.data.polygons]
 o.data.materials.clear()
 for mat in slots:o.data.materials.append(mat)
 for f,i in zip(o.data.polygons,indices):f.material_index=i
 m.RECORDS[name]['material_slots']=[mat.name for mat in slots]
 colors=o.data.color_attributes.new(name='FaceMask',type='FLOAT_COLOR',domain='CORNER')
 for li,loop in enumerate(o.data.loops):
  x,y,z=o.data.vertices[loop.vertex_index].co;weight=max(0,min(1,(x-.008)/.04))*max(0,min(1,(.084-abs(y))/.024))
  colors.data[li].color=(weight,weight,weight,1)
 # Facial projection is authored after the general garment unwrap, before pivot offset.
 # The generated texture has its eye line at 30%, nose at 56%, mouth at 75%.
 levels=[(-.205,1),(-.142,.745),(-.098,.56),(-.048,.30),(-.025,.20),(.015,.075),(.083,0)]
 def face_v(z):
  for a,b in zip(levels,levels[1:]):
   if a[0]<=z<=b[0]:t=(z-a[0])/(b[0]-a[0]);return 1-(a[1]*(1-t)+b[1]*t)
  return 0 if z>.083 else 1
 for f in o.data.polygons:
  if o.data.materials[f.material_index].name.startswith('M_Face'):
   for li in f.loop_indices:
    v=o.data.vertices[o.data.loops[li].vertex_index].co;o.data.uv_layers.active.data[li].uv=(max(.001,min(.999,v.y/.175+.5)),face_v(v.z))
 for v in o.data.vertices:v.co=Vector(tuple(v.co[i]*scale[i]+offset[i] for i in range(3)))
 for f in o.data.polygons:f.use_smooth=o.data.materials[f.material_index].name not in ('M_Metal32','M_Armor32')
 o.data.update();m.RECORDS[name]['local_bounds_m']=m.bounds(o)
 return o

def head(female=False,dead=False,mannequin=False):
 skin='Mannequin32' if mannequin else 'SkinDead32' if dead else 'Skin32'
 # Continuous skull / jaw mesh. Gaussian sculpt fields form the orbital hollows,
 # brows, cheekbones, nose bridge and muzzle without stacked spherical facial parts.
 profile=[(-.205,.035,.035,.020),(-.185,.054,.049,.025),(-.16,.067,.065,.019),(-.125,.078,.080,.006),(-.08,.083,.089,-.007),(-.035,.091,.090,-.013),(.015,.092,.088,-.022),(.052,.077,.073,-.026),(.077,.040,.040,-.027),(.083,.004,.004,-.027)]
 def interp(z):
  for a,b in zip(profile,profile[1:]):
   if a[0]<=z<=b[0]:t=(z-a[0])/(b[0]-a[0]);return [a[k]*(1-t)+b[k]*t for k in range(1,4)]
  return profile[-1][1:]
 def g(v,c,w):return math.exp(-((v-c)/w)**2)
 verts=[];N=48;R=37
 for j in range(R):
  z=-.205+j*.288/(R-1);rx,ry,cx=interp(z)
  for i in range(N):
   t=math.tau*i/N;y=ry*math.sin(t);x=cx+rx*math.cos(t)
   front=max(0,math.cos(t))**4
   d=.013*g(abs(y),.035,.028)*g(z,-.026,.015)-.020*g(abs(y),.038,.023)*g(z,-.049,.017)
   d+=.014*g(abs(y),.062,.024)*g(z,-.082,.024)+.048*g(y,0,.016)*g(z,-.083,.026)
   d+=.017*g(y,0,.012)*g(z,-.048,.040)+.009*g(y,0,.038)*g(z,-.139,.020)
   x+=front*d
   if female:y*=.93;x-=front*.003*g(z,-.025,.020)
   verts.append((x,y,z))
 faces=[tuple(range(N-1,-1,-1))]
 for j in range(R-1):
  for i in range(N):a=j*N+i;b=j*N+(i+1)%N;faces.append((a,b,b+N,a+N))
 faces.append(tuple((R-1)*N+i for i in range(N)))
 face=m.mesh('Sculpted face',verts,faces,skin)
 if not mannequin:
  face.data.materials.append(m.MATS['Face132' if female else 'Face032'])
  for poly in face.data.polygons:
   center=sum((face.data.vertices[i].co for i in poly.vertices),Vector())/len(poly.vertices)
   if center.x>-.009 and center.z>-.204:poly.material_index=1
 ringmesh([(0,0,-.285,.047,.049),(0,0,-.235,.047,.050),(.005,0,-.18,.044,.045)],skin,20)
 for s in [-1,1]:
  ell((-.018,s*(.086 if female else .093),-.082),(.023,.014,.037),skin)
  if False: # Features are baked into the fitted face projection, not protruding eye disks.
   # Eyes are small almond surfaces set within the orbital hollows.
   y=s*.035*(.93 if female else 1);z=-.048
   vs=[(.076,y,z)];outline=[]
   for i in range(17):t=math.tau*i/16;outline.append((.069,y+.020*math.cos(t),z+.0062*math.sin(t)))
   m.mesh('Almond eye',vs+outline,[(0,i+1,i+2) for i in range(16)],'Eye32')
   ell((.077,y,-.048),(.0016,.006,.006),'Iris32',12)
   line(outline,.0017,skin,6)
   line([(.079,y-s*.020,-.030),(.083,y,-.025),(.066,y+s*.022,-.031)],.0035,'Hair32',6)
   ell((.117,s*.011,-.098),(.0018,.004,.002),'Mouth32',12)
 if False:
  # Relaxed closed lips, no permanent grin or exposed square teeth.
  for dz in [-.0025,.0025]:
   line([(.093,-.025,-.143),(.103,-.012,-.141+dz),(.107,0,-.142+dz),(.103,.012,-.141+dz),(.093,.025,-.143)],.0028,'Lips32',6)
  line([(.097,-.023,-.142),(.108,0,-.142),(.097,.023,-.142)],.001,'Mouth32',6)
 if dead:
  line([(.065,-.069,-.012),(.073,-.061,-.027),(.067,-.059,-.048)],.0025,'Lips32',6)

def torso(female=False,kind='Resident'):
 mat='Mannequin32' if kind=='Mannequin' else 'ClothDead32' if kind=='Zombie' else 'ClothRaider32' if kind=='Raider' else 'Cloth32'
 W=.91 if female else 1
 rings=[(0,0,-.49,.108,.142*W),(.005,0,-.43,.115,.146*W),(0,0,-.36,.109,.150*W),(.003,0,-.29,.123,.172*W),(.007,0,-.20,.131,.192*W),(.005,0,-.11,.139,.209*W),(0,0,-.055,.129,.216*W),(-.01,0,-.012,.097,.188*W),(-.01,0,.014,.050,.066)]
 ringmesh(rings,mat,28,.014 if kind!='Mannequin' else 0)
 if kind=='Mannequin':return
 # Jacket opening, shirt inset, lapels, seam piping and actual sewn pockets.
 m.box((.134,0,-.21),(.013,.026,.36),'Shirt32',.002)
 for s in [-1,1]:
  m.mesh('Folded collar',[(.043,s*.051,.004),(.114,s*.103,-.044),(.143,s*.048,-.115),(.108,s*.021,-.032)],[(0,1,2,3)],mat)
  m.box((.136,s*.09,-.19),(.018,.080,.090),mat,.006)
  m.box((.148,s*.09,-.147),(.008,.088,.022),mat,.003)
  m.box((.12,s*.077,-.395),(.018,.070,.040),mat,.005)
  line([(.13,s*.132,-.11),(.118,s*.125,-.3),(.116,s*.12,-.45)],.0017,'Shirt32',6)
 for z in [-.12,-.20,-.28,-.36]:m.cyl((.148,-.01,z),(.152,-.01,z),.0034,'Metal32',n=8)
 ringmesh([(0,0,-.48,.115,.148*W),(0,0,-.455,.116,.149*W)],'Leather32',24)
 if kind=='Raider':
  m.box((.150,0,-.20),(.033,.255,.25),'Armor32',.018)
  for s in [-1,1]:
   line([(.10,s*.13,-.01),(.176,s*.125,-.11),(.17,s*.13,-.35)],.021,'Leather32',8)
   for z in [-.16,-.21,-.26]:m.box((.173,s*.067,z),(.015,.09,.02),'ClothRaider32',.003)
   m.box((.166,s*.13,-.41),(.055,.078,.095),'Leather32',.009)

def pelvis(female=False,mannequin=False):
 mat='Mannequin32' if mannequin else 'Pants32';W=1.09 if female else 1
 ringmesh([(0,0,.025,.110,.146*W),(-.012,0,-.025,.122,.154*W),(-.016,0,-.10,.124,.163*W),(0,0,-.14,.083,.140*W)],mat,24,.012)
 if mannequin:return
 ringmesh([(0,0,.018,.113,.150*W),(0,0,-.022,.123,.158*W)],'Leather32',24)
 m.box((.121,0,-.004),(.016,.047,.038),'Metal32',.004)
 for s in [-1,1]:
  line([(.092,s*.122,-.035),(.103,s*.11,-.067),(.1,s*.065,-.125)],.0023,'Cloth32',6)
  m.box((-.131,s*.077,-.10),(.012,.069,.068),mat,.004)

def arm(kind='Resident'):
 skin='Mannequin32' if kind=='Mannequin' else 'SkinDead32' if kind=='Zombie' else 'Skin32'
 cloth='Mannequin32' if kind=='Mannequin' else 'ClothRaider32' if kind=='Raider' else 'Cloth32'
 ringmesh([(0,0,.025,.049,.050),(0,0,-.025,.078,.072),(.004,0,-.09,.074,.068),(.012,0,-.18,.061,.060),(.018,0,-.25,.052,.050),(.02,0,-.275,.051,.049)],cloth,20,.014)
 ringmesh([(.02,0,-.26,.050,.047),(.025,0,-.30,.051,.046),(.030,0,-.38,.052,.041),(.035,0,-.46,.037,.030),(.035,0,-.50,.030,.028)],skin,20)
 ringmesh([(.035,0,-.493,.031,.029),(.04,0,-.532,.037,.044),(.046,0,-.582,.032,.044),(.05,0,-.60,.022,.035)],skin,16)
 for i in range(4):
  y=-.031+i*.020;end=-.674+[.014,0,.005,.020][i]
  line([(.05,y,-.582),(.06,y,-.621),(.067,y,end)],.0085,skin,8)
 line([(.057,-.039,-.532),(.078,-.06,-.557),(.082,-.061,-.594)],.011,skin,10)
 if kind!='Mannequin':
  ringmesh([(.02,0,-.245,.055,.052),(.02,0,-.267,.054,.05)],cloth,20)
  line([(.071,0,-.08),(.065,0,-.18),(.058,0,-.24)],.0018,'Shirt32',6)

def leg(mannequin=False):
 mat='Mannequin32' if mannequin else 'Pants32'
 ringmesh([(0,0,.04,.074,.074),(0,0,-.02,.092,.082),(-.008,0,-.13,.092,.083),(-.004,0,-.26,.077,.072),(.022,0,-.36,.06,.062),(.024,0,-.41,.065,.063),(.006,0,-.49,.070,.062),(-.012,0,-.59,.062,.054),(-.018,0,-.69,.044,.044),(-.012,0,-.755,.040,.040)],mat,24,.021 if not mannequin else 0)
 if mannequin:
  ell((.04,0,-.79),(.103,.054,.055),mat);return
 m.box((.061,0,-.82),(.24,.12,.033),'Leather32',.012)
 ringmesh([(.01,0,-.66,.049,.049),(.00,0,-.72,.052,.052),(.045,0,-.77,.104,.058),(.061,0,-.804,.120,.058)],'Leather32',20)
 for z in [-.70,-.724,-.747]:line([(.05,-.028,z),(.058,.028,z-.009)],.0028,'Shirt32',6)
 line([(-.004,.076,-.05),(-.006,.068,-.25),(.006,.06,-.42),(-.013,.055,-.60)],.0021,'Cloth32',6)
 m.box((0,.078,-.24),(.10,.019,.15),mat,.009)
 m.box((0,.089,-.17),(.11,.012,.027),mat,.004)

def hair(style):
 # Structured scalp surface with hairline varying around the temples / nape.
 if style==0:return ringmesh([(-.023,0,.013,.09,.083),(-.025,0,.057,.072,.067),(-.027,0,.087,.03,.032)],'Hair32',28)
 verts=[];N=32;R=8
 for j in range(R):
  for i in range(N):
   a=i*math.tau/N;bottom=.013-.08*max(0,-math.cos(a));z=bottom+( .093-bottom)*j/(R-1)
   q=(z-.005)/.092;r=math.sqrt(max(.02,1-q*q));verts.append((-.024+.097*r*math.cos(a),.090*r*math.sin(a),z))
 m.mesh('Fitted hair cap',verts,[(j*N+i,j*N+(i+1)%N,(j+1)*N+(i+1)%N,(j+1)*N+i) for j in range(R-1) for i in range(N)]+[tuple((R-1)*N+i for i in range(N))],'Hair32')
 if style in [1,2]:
  for i in range(9):
   y=-.077+i*.019
   line([(.050,y,.025),(.037,y*.9,.063+(.014 if style==2 else 0)),(-.055,y*.8,.078)],.008,'Hair32',9)
 if style in [3,4,5,8,9]:
  length={3:.25,4:.13,5:.095,8:.08,9:.09}[style]
  for i in range(13):
   a=math.pi/2+math.pi*i/12;x=-.024+math.cos(a)*.095;y=math.sin(a)*.093
   # Hair strips, broad tapering locks, not sticks jutting out of the scalp.
   ringmesh([(x,y,.018,.017,.018),(x-.005,y*1.05,-.05,.020,.017),(x-.01,y*1.04,-length,.012,.010)],'Hair32',8)
 if style==5:line([(-.11,0,.028),(-.18,0,.004),(-.185,.01,-.22)],.036,'Hair32',12)
 if style==6:
  for i in range(55):
   t=i*2.399;r=.108*math.sqrt(i/55);ell((-.02+r*math.cos(t),r*math.sin(t),.025+.095*(1-r/.14)),(.020,.020,.021),'Hair32',10)
 if style==7:ringmesh([(-.025,0,.06,.094,.019),(-.025,0,.145,.072,.013),(-.032,0,.16,.05,.004)],'Hair32',16)
 if style==8:
  for i in range(11):ell((-.13,(-1)**i*.009,-.01-i*.023),(.020,.019,.018),'Hair32',10)
 if style==9:ell((-.12,0,.047),(.055,.048,.050),'Hair32',20)

for kind in ['Resident','ResidentFemale','Zombie','Raider','Mannequin']:
 female=kind=='ResidentFemale';base='Resident' if female else kind
 torso(female,base);finish(kind+'Torso32')
 head(female,base=='Zombie',base=='Mannequin');finish(kind+'Head32')
 pelvis(female,base=='Mannequin');finish(kind+'Pelvis32')
 arm(base);finish(kind+'Arm32')
 leg(base=='Mannequin');finish(kind+'Leg32')
for female in [False,True]:
 torso(female);pelvis(female)
 # Torso and pelvis use different joint frames in NPCs; build player separately.
 # Discard the assembled scratch geometry and use fitted source components instead.
 for o in m.PARTS:bpy.data.objects.remove(o,do_unlink=True)
 m.PARTS.clear()
 for part,at in [('Torso',(0,0,1.36)),('Pelvis',(0,0,.86))]:
  src=m.OBJECTS[('ResidentFemale' if female else 'Resident')+part+'32'];o=src.copy();o.data=src.data.copy();bpy.context.collection.objects.link(o);o.location=at;m.PARTS.append(o)
 finish('SurvivorFemale32' if female else 'SurvivorMale32')
 head(female);finish('SurvivorHeadFemale32' if female else 'SurvivorHeadMale32',offset=(0,0,1.68))
leg();finish('SurvivorLeg32',scale=(1,1,.95))
arm();finish('SurvivorArm32',scale=(1,1,.94))
for style in range(10):
 hair(style);o=finish('Hair%02d32'%style)
 duplicate=o.copy();duplicate.data=o.data.copy();bpy.context.collection.objects.link(duplicate);m.PARTS.append(duplicate)
 finish('PlayerHair%02d32'%style,offset=(0,0,1.68))

# First-person wrist-origin hands retain the weapon socket convention: fingers +X,
# palm -Z, sleeve -X. Modelled knuckles, finger joints, cuffs and folded sleeves.
for side,name in [(1,'LeftHand32'),(-1,'RightHand32')]:
 o=ringmesh([(0,0,-.60,.070,.066),(.005,0,-.47,.073,.064),(.007,0,-.34,.061,.054),(.009,0,-.20,.055,.044),(.005,0,-.075,.041,.034),(0,0,-.025,.034,.029)],'Cloth32',20,.027)
 for v in o.data.vertices:
  x,y,z=v.co;v.co=(z,y,x+z*.25)
 m.box((-.022,0,0),(.030,.07,.062),'Leather32',.007)
 m.box((.034,0,.002),(.08,.077,.041),'Leather32',.009)
 m.box((.035,0,.027),(.055,.066,.011),'Cloth32',.005)
 for i in range(4):
  y=side*(-.028+i*.018);length=[.046,.056,.052,.041][i]
  points=[(.067,y,.006),(.078+length*.35,y,-.003),(.079+length*.40,y,-.03),(.069,y,-.051)]
  line(points,.008,'Leather32',10);ell((.070,y,.02),(.013,.010,.009),'Cloth32',12)
  for a in [1,2]:ell(points[a],(.009,.009,.009),'Leather32',12)
 line([(.01,side*.03,-.004),(.033,side*.053,-.017),(.062,side*.04,-.040)],.011,'Leather32',10)
 for y in [-.024,.024]:line([(-.12,y,.012),(-.07,y,.028),(-.035,y,.023)],.0015,'Shirt32',6)
 finish(name)

# Creature parts retain their purpose-built skeleton proportions. Merge organic
# volumes into continuous surfaces, remesh and simplify, then replace environment
# materials with character surfaces. Mechanical scooter pieces remain separate.
legacy=m.ROOT/'ArtSource/ModelsV26/AllAmericanMeltdown_V26.blend'
if not legacy.exists():
 legacy=next((m.ROOT/'ArtSource/ModelsV26').glob('*.blend'))
with bpy.data.libraries.load(str(legacy),link=False) as (src,dst):
 dst.objects=[n for n in src.objects if n.startswith(tuple('SM_'+k for k in ['Dog','Moose','Titan','Deathclaw','Scorpion','Karen','Trader']))]
for ob in dst.objects:
 if not ob or ob.type!='MESH':continue
 bpy.context.collection.objects.link(ob)
 old=ob.name.split('.')[0];new=old.removeprefix('SM_').replace('V18','').replace('26','')+'32'
 if new in m.OBJECTS:continue
 ob.data=ob.data.copy()
 for i,slot in enumerate(ob.data.materials):
  n=slot.name
  key='Metal32' if 'Steel' in n else 'Leather32' if 'Rubber' in n else 'Lips32' if 'Red' in n else 'Pants32' if 'Cloth' in n else 'SkinDead32' if 'Skin' in n else 'Leather32' if 'Rust' in n else 'Eye32'
  ob.data.materials[i]=m.MATS[key]
 bpy.ops.object.select_all(action='DESELECT');ob.select_set(True);bpy.context.view_layer.objects.active=ob
 if 'Pelvis' not in new and 'Trader' not in new and 'Scorpion' not in new:
  weights={i:0 for i in range(len(ob.data.materials))}
  for f in ob.data.polygons:weights[f.material_index]+=f.area
  dominant=max(weights,key=weights.get)
  details=ob.copy();details.data=ob.data.copy();bpy.context.collection.objects.link(details)
  bm=bmesh.new();bm.from_mesh(details.data);bmesh.ops.delete(bm,geom=[f for f in bm.faces if f.material_index==dominant],context='FACES');bm.to_mesh(details.data);bm.free()
  # Unify only the dominant body material, keeping horns/eyes/claws readable.
  rem=ob.modifiers.new('Organic joint blending','REMESH');rem.mode='VOXEL';rem.voxel_size=.012 if 'Dog' in new else .021;rem.use_smooth_shade=True
  bpy.ops.object.modifier_apply(modifier=rem.name)
  smooth=ob.modifiers.new('Surface relaxation','SMOOTH');smooth.factor=.45;smooth.iterations=3;bpy.ops.object.modifier_apply(modifier=smooth.name)
  dec=ob.modifiers.new('PS2 triangle budget','DECIMATE');dec.ratio=min(1,2600/max(1,len(ob.data.polygons)));bpy.ops.object.modifier_apply(modifier=dec.name)
  for f in ob.data.polygons:f.material_index=dominant
  if len(details.data.polygons):m.PARTS.append(details)
  else:bpy.data.objects.remove(details,do_unlink=True)
 m.PARTS.append(ob);finish(new)
for record in m.RECORDS.values():
 if any(k in record['name'] for k in ['Torso','Head','Pelvis','Arm','Leg']):record['collision']='convex'

# Export source meshes and explicit validation metadata; smooth imported normals,
# real low-poly silhouette detail and shared diffuse maps are the PS2 presentation.
m.export_all()
(m.OUT/'models_32_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values()),'materials':SURF},indent=2))
bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'AllAmericanMeltdown_Characters32.blend'))
print('CHARACTERS32_COMPLETE',len(m.OBJECTS),flush=True)
