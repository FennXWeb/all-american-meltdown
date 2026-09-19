"""Rebuild segmented NPCs with anatomical forms, clothing construction and facial features."""
import sys,math,json
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent));import make_models_v2 as m
import bpy
m.OUT=m.ROOT/'ArtSource/ModelsV26';m.setup()
def ell(p,s,mat='Skin'):
 bpy.ops.mesh.primitive_uv_sphere_add(segments=24,ring_count=16,radius=1,location=p);o=bpy.context.object;o.scale=s;bpy.ops.object.transform_apply(location=False,rotation=False,scale=True);return m.add(o,mat)
def bone(a,b,r,mat='Skin',r2=None):m.cyl(a,b,r,mat,r2=r2 if r2 is not None else r*.7,n=16)
def face(z=0,mat='Skin',mutant=False):
 ell((-.015,0,z-.04),(.102,.092,.125),mat);ell((.04,0,z-.12),(.080,.074,.058),mat)
 for side in [-1,1]:
  ell((.074,side*.042,z-.005),(.025,.031,.023),'Rubber');ell((.096,side*.042,z-.006),(.009,.020,.013),'Bone');ell((.103,side*.042,z-.006),(.006,.007,.009),'Rubber')
  ell((.068,side*.046,z+.020),(.032,.041,.014),mat);ell((-.014,side*.096,z-.042),(.027,.020,.042),mat)
  ell((.080,side*.058,z-.055),(.027,.022,.027),mat)
 ell((.104,0,z-.048),(.035,.022,.037),mat)
 m.box((.103,0,z-.113),(.014,.073,.016),'Rubber',.006)
 for j in range(6):m.box((.112,-.027+j*.011,z-.110),(.009,.008,.011),'Bone',.002)
 bone((0,0,z-.18),(0,0,z-.245),.048,mat)
 if mutant:
  for j in range(3):bone((.083,-.06+j*.028,z+.03),(.103,-.045+j*.028,z-.07),.006,'Red',.003)
for kind in ['Zombie','Raider','Resident','Mannequin']:
 skin='Bone' if kind=='Mannequin' else 'Skin';cloth=skin if kind=='Mannequin' else 'Cloth'
 # ribcage, paired pectorals, sternum, waist and shoulder caps
 ell((-.012,0,-.20),(.13,.185,.245),cloth)
 for side in [-1,1]:ell((.070,side*.082,-.12),(.074,.088,.104),cloth);ell((0,side*.158,-.035),(.090,.071,.083),cloth)
 ell((0,0,-.39),(.100,.115,.100),cloth);bone((0,-.11,.005),(0,.11,.005),.028,skin)
 if kind!='Mannequin':
  m.box((.122,0,-.23),(.008,.016,.36),'Steel',.002)
  for side in [-1,1]:m.box((.118,side*.08,-.19),(.024,.10,.10),'Cloth',.009);m.box((.134,side*.08,-.15),(.007,.085,.014),'Bone',.002)
  if kind=='Raider':
   m.box((.12,0,-.18),(.045,.30,.24),'Rust',.016)
   for y in [-.1,0,.1]:m.box((.154,y,-.31),(.06,.075,.12),'Rubber',.01)
  if kind=='Zombie':
   for z in [-.12,-.18,-.24]:bone((.13,-.13,z),(.135,-.02,z-.03),.016,'Bone',.012)
 m.finish(kind+'Torso')
 face(0,skin,kind=='Zombie')
 if kind=='Raider':m.box((.104,0,-.095),(.035,.14,.085),'Cloth',.025)
 if kind in ['Raider','Resident']:
  ell((-.025,0,.053),(.1,.095,.050),'Rubber')
 m.finish(kind+'Head')
 ell((0,0,-.10),(.13,.155,.135),cloth)
 if kind!='Mannequin':m.box((.02,0,-.02),(.24,.32,.045),'Rubber',.014);m.box((.145,0,-.02),(.012,.065,.041),'Steel',.004)
 m.finish(kind+'Pelvis')
 # Deltoid, biceps, elbow, forearm, palm and separated fingers.
 ell((0,0,-.065),(.070,.069,.10),cloth);bone((0,0,-.06),(.015,0,-.29),.064,cloth,.047);ell((.015,0,-.29),(.050,.048,.049),skin);bone((.015,0,-.29),(.04,0,-.53),.048,skin,.034);ell((.047,0,-.565),(.043,.026,.057),skin)
 for i in range(4):bone((.053,-.025+i*.017,-.59),(.073,-.025+i*.017,-.67),.009,skin,.007)
 bone((.073,.025,-.55),(.11,.028,-.59),.013,skin,.008);m.finish(kind+'Arm')
 ell((0,0,-.15),(.083,.079,.19),cloth);bone((0,0,-.12),(.015,0,-.39),.078,cloth,.053);ell((.026,0,-.41),(.064,.058,.070),cloth);bone((.015,0,-.44),(-.01,0,-.73),.058,cloth,.038);ell((.045,0,-.79),(.115,.061,.059),'Rubber')
 m.box((.045,0,-.833),(.23,.125,.023),'Rubber',.008)
 for x in [.00,.025,.05,.075]:bone((x,-.046,-.77),(x,.046,-.77),.004,'Bone',.004)
 m.finish(kind+'Leg')
# Original creature construction, rebuilt with denser surface forms plus articulated surface detail.
original_finish=m.finish
def finish_creature(name,*args,**kwargs):
 if 'Torso' in name:
  if name.startswith(('Titan','Deathclaw')):
   for side in [-1,1]:
    ell((.16,side*.18,-.12),(.13,.16,.16),'Skin' if name.startswith('Titan') else 'Rust')
    for z in [-.28,-.38,-.48]:ell((.15,side*.065,z),(.075,.062,.050),'Skin')
  if name.startswith('Moose'):
   for i in range(9):bone((-.5+i*.1,0,.40),(-.55+i*.1,0,.62),.027,'Rubber',.003)
 if 'Head' in name and name.startswith(('Titan','Karen')):
  for side in [-1,1]:ell((.16,side*.07,.01),(.03,.04,.024),'Rubber');ell((.183,side*.07,.01),(.01,.021,.014),'Bone')
 if 'Arm' in name and name.startswith(('Titan','Deathclaw')):
  ell((.025,0,-.13),(.13,.12,.18),'Skin');ell((.03,0,-.40),(.10,.10,.16),'Skin')
 return original_finish(name,*args,**kwargs)
m.finish=finish_creature
src=(m.ROOT/'Tools/make_models_v18.py').read_text();a=src.index('# Body parts');b=src.index('m.export_all()',a)
# Helpers preserve the original joint pivots and body proportions.
def limb(a,b,r,mat='Skin',end=.07):bone(a,b,r,mat,end);ell(a,(r,r,r),mat)
def horn(points,r=.07):
 for i in range(len(points)-1):bone(points[i],points[i+1],r*(1-i/(len(points)-1)),'Bone',max(.002,r*(1-(i+1)/(len(points)-1))))
# Map the old creature-only hide slot to the existing textured rust material.
exec(src[a:b].replace("'CreatureHideV18'","'Rust'"))
m.finish=original_finish
# Canine anatomy: ribcage, tapered muzzle, jaw, ears, hocks and paws.
ell((0,0,0),(.36,.145,.19),'Skin');ell((.22,0,.055),(.17,.17,.21),'Skin');m.finish('DogTorso')
ell((0,0,0),(.16,.11,.14),'Skin');ell((.16,0,-.03),(.15,.075,.065),'Skin');ell((.28,0,-.015),(.055,.065,.045),'Rubber')
for side in [-1,1]:horn([(-.04,side*.07,.07),(-.08,side*.10,.26)],.06);ell((.06,side*.094,.04),(.025,.017,.022),'Bone');ell((.073,side*.103,.04),(.012,.01,.016),'Rubber')
for x in [.10,.15,.20]:bone((x,-.055,-.04),(x,-.055,-.09),.012,'Bone',.002)
m.finish('DogHead');ell((0,0,0),(.17,.14,.17),'Skin');horn([(-.08,0,.04),(-.30,0,.10),(-.43,0,.025)],.048);m.finish('DogPelvis')
for part in ['Arm','Leg']:
 bone((0,0,0),(.04,0,-.17),.052,'Skin',.03);ell((.04,0,-.17),(.045,.035,.045),'Skin');bone((.04,0,-.17),(-.02,0,-.34),.032,'Skin',.021);ell((.025,0,-.36),(.072,.041,.035),'Rubber');m.finish('Dog'+part)
# Trader chassis: panniers, ventilated battery box, working-looking suspension and optics.
m.box((0,0,.66),(.52,.50,.63),'Steel',.055)
for side in [-1,1]:
 m.box((0,side*.34,.65),(.43,.18,.42),'Cloth',.03)
 for z in [.54,.62,.70,.78]:m.box((.274,side*.11,z),(.013,.16,.024),'Rubber',.003)
 bone((0,side*.18,.4),(0,side*.32,.25),.032,'Steel')
m.box((.28,0,.89),(.022,.32,.13),'Bone',.01);m.finish('TraderBody')
m.box((0,0,.07),(.32,.30,.20),'Steel',.025)
for y in [-.085,.085]:bone((.14,y,.09),(.20,y,.09),.050,'Rubber');bone((.20,y,.09),(.21,y,.09),.037,'Glass')
bone((-.10,0,.14),(-.12,0,.37),.01,'Steel');m.finish('TraderHead')
m.cyl((0,-.07,0),(0,.07,0),.26,'Rubber',n=24)
for i in range(20):a=i*math.tau/20;m.box((.25*math.cos(a),0,.25*math.sin(a)),(.05,.17,.04),'Rubber',.004,rot=(0,-a,0))
m.finish('TraderWheel')
m.export_all();(m.OUT/'models_v26_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'NPCs26.blend'))
