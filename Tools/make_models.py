"""Blender 4.2 source generator. All dimensions in metres; FBX carries unit metadata."""
import bpy, math, random
from pathlib import Path
from mathutils import Vector
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'ArtSource'/'Models'; OUT.mkdir(parents=True,exist_ok=True)
random.seed(198706)
bpy.ops.object.select_all(action='SELECT'); bpy.ops.object.delete(use_global=False)
mats={}
palette={'Concrete':(.44,.43,.36),'Brick':(.4,.25,.16),'Rust':(.34,.38,.31),'Wood':(.33,.23,.12),
         'Cloth':(.21,.26,.18),'Skin':(.42,.46,.32),'Rubber':(.065,.07,.06),'Red':(.46,.12,.07),
         'Steel':(.2,.25,.26),'Bone':(.6,.55,.4),'Glow':(.9,.47,.12),'Glass':(.055,.10,.09)}
for name,col in palette.items():
    m=bpy.data.materials.new('M_'+name); m.diffuse_color=(*col,1); mats[name]=m
parts=[]
def finish(o,mat):
    o.data.materials.append(mats[mat]); parts.append(o); return o
def box(p,s,mat='Rust',rot=(0,0,0),bevel=0):
    bpy.ops.mesh.primitive_cube_add(size=1,location=p,rotation=rot); o=bpy.context.object
    o.scale=s; bpy.ops.object.transform_apply(location=False,rotation=False,scale=True)
    if bevel:
        mod=o.modifiers.new('Worn edges','BEVEL'); mod.width=bevel; mod.segments=1
        bpy.ops.object.modifier_apply(modifier=mod.name)
    return finish(o,mat)
def cylinder(a,b,r,mat='Steel',r2=None,verts=8):
    a,b=Vector(a),Vector(b); d=b-a
    bpy.ops.mesh.primitive_cone_add(vertices=verts,radius1=r,radius2=r if r2 is None else r2,depth=d.length,location=(a+b)/2)
    o=bpy.context.object; o.rotation_euler=d.to_track_quat('Z','Y').to_euler(); return finish(o,mat)
def ico(p,s,mat='Skin',sub=1):
    bpy.ops.mesh.primitive_ico_sphere_add(subdivisions=sub,radius=1,location=p); o=bpy.context.object; o.scale=s
    return finish(o,mat)
def export(name):
    global parts
    bpy.ops.object.select_all(action='DESELECT')
    for o in parts: o.select_set(True)
    bpy.context.view_layer.objects.active=parts[0]
    bpy.ops.object.join(); o=bpy.context.object; o.name='SM_'+name
    bpy.context.scene.cursor.location=(0,0,0); bpy.ops.object.origin_set(type='ORIGIN_CURSOR')
    bpy.ops.object.transform_apply(location=False,rotation=True,scale=True)
    bpy.ops.object.mode_set(mode='EDIT'); bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(angle_limit=1.15,island_margin=.015)
    bpy.ops.object.mode_set(mode='OBJECT')
    bpy.ops.export_scene.fbx(filepath=str(OUT/(o.name+'.fbx')),use_selection=True,object_types={'MESH'},
        apply_unit_scale=True,axis_forward='-Y',axis_up='Z',bake_anim=False,mesh_smooth_type='FACE')
    bpy.ops.object.delete(use_global=False); parts=[]

# Forged hexagonal crowbar, hooked split claw, taped grip, chipped red lacquer.
for a,b in [((0,0,0),(.49,0,0)),((.49,0,0),(.58,0,.035)),((.58,0,.035),(.60,0,.095))]:
    cylinder(a,b,.011,'Red',verts=6)
for yy in (-.009,.009): box((.592,yy,.111),(.052,.010,.013),'Steel',rot=(0,.4,0))
cylinder((-.07,0,-.009),(0,0,0),.012,'Steel',r2=.007,verts=6)
for i in range(12): cylinder((.07+i*.009,0,0),(.075+i*.009,0,0),.015,'Rubber')
export('Crowbar')
# Ash baseball bat: flared barrel, knob, leather wrap, nail scars.
cylinder((0,0,0),(.035,0,0),.031,'Wood',verts=10)
cylinder((.035,0,0),(.29,0,0),.016,'Wood',r2=.02,verts=10)
cylinder((.29,0,0),(.64,0,0),.02,'Wood',r2=.048,verts=10)
cylinder((.64,0,0),(.83,0,0),.048,'Wood',r2=.04,verts=10)
for i in range(15): cylinder((.06+i*.011,0,0),(.065+i*.011,0,0),.020,'Rubber')
for i in range(7): box((.59+i*.026,-.042,.018),(.018,.006,.018),'Red',rot=(.2,.1,.5))
export('Bat')
# Pump shotgun: open bore, tube magazine, walnut stock, ribbed pump, trigger guard.
box((.20,0,0),(.24,.054,.073),'Steel',bevel=.008)
cylinder((.27,0,.026),(.79,0,.026),.019,'Steel',verts=12)
cylinder((.27,0,-.021),(.66,0,-.021),.014,'Steel',verts=10)
cylinder((.30,0,-.021),(.48,0,-.021),.03,'Wood',verts=10)
for i in range(8): cylinder((.31+i*.02,0,-.021),(.315+i*.02,0,-.021),.032,'Rubber')
box((.00,0,-.024),(.18,.065,.073),'Wood',rot=(0,-.15,0),bevel=.01)
box((-.13,0,-.056),(.19,.083,.13),'Wood',rot=(0,-.10,0),bevel=.015)
box((-.228,0,-.056),(.018,.085,.133),'Rubber',rot=(0,-.1,0))
for a,b in [((.08,0,-.041),(.10,0,-.09)),((.10,0,-.09),(.18,0,-.09)),((.18,0,-.09),(.20,0,-.039))]: cylinder(a,b,.006,'Steel')
box((.19,-.028,.014),(.067,.006,.028),'Rubber')
box((.755,0,.049),(.012,.012,.02),'Glow')
box((.19,0,.051),(.02,.023,.015),'Steel')
export('Shotgun')
# Coat sleeves and fingered leather gloves, forward axis X.
for side in (-1,1):
    y=side*.17
    cylinder((-.16,y,-.20),(.17,y*.65,-.055),.055,'Cloth',r2=.039)
    cylinder((.15,y*.65,-.063),(.23,y*.46,-.041),.041,'Rubber',r2=.032)
    for f in range(4): cylinder((.22,y*.46+(f-1.5)*.015,-.042),(.26,y*.35+(f-1.5)*.015,-.069),.009,'Rubber')
export('Arms')
# Segmented infected, each part pivots at its anatomical joint for runtime animation.
ico((0,0,-.22),(.19,.12,.25),'Cloth',2); box((.12,0,-.21),(.052,.22,.25),'Red')
for yy in (-.065,.015,.065): cylinder((.153,yy,-.12),(.168,yy,-.30),.009,'Bone')
export('ZombieTorso')
ico((0,0,-.095),(.10,.09,.135),'Skin',2)
box((.075,0,-.16),(.052,.135,.033),'Red')
for yy in (-.047,.047):
    ico((.084,yy,-.071),(.022,.022,.022),'Rubber'); ico((.101,yy,-.071),(.011,.009,.008),'Glow')
for yy in (-.045,-.025,0,.025,.045): box((.102,yy,-.14),(.015,.009,.017),'Bone')
ico((.02,0,.015),(.088,.085,.04),'Rubber')
export('ZombieHead')
cylinder((0,0,0),(.015,0,-.35),.062,'Cloth',r2=.041)
cylinder((.015,0,-.35),(.11,0,-.64),.039,'Skin',r2=.031)
ico((.12,0,-.68),(.055,.04,.062),'Skin')
for i in range(4): cylinder((.14,(i-1.5)*.02,-.68),(.18,(i-1.5)*.023,-.78),.01,'Skin')
export('ZombieArm')
cylinder((0,0,0),(.013,0,-.4),.085,'Cloth',r2=.055)
cylinder((.013,0,-.4),(0,0,-.76),.055,'Cloth',r2=.041)
box((.05,0,-.77),(.23,.11,.12),'Rubber',bevel=.022)
export('ZombieLeg')
ico((0,0,-.09),(.15,.13,.14),'Cloth'); export('ZombiePelvis')
# Derelict station wagon with crushed hood, trim, roof rack, wheels and dark glass.
box((0,0,.61),(4.25,1.75,.48),'Rust',bevel=.12)
box((-.32,0,1.04),(2.42,1.55,.53),'Glass',bevel=.14)
box((-.38,0,1.35),(2.55,1.65,.12),'Rust',bevel=.03)
for yy in (-.81,.81):
    for xx in (-1.5,-.6,.75): box((xx,yy,1.10),(.07,.07,.47),'Rust',rot=(0,-.16,0))
    box((-.30,yy,.84),(2.5,.09,.12),'Rust')
    for xx in (-1.3,1.25):
        cylinder((xx,yy-.11,.4),(xx,yy+.11,.4),.36,'Rubber',verts=12)
        cylinder((xx,yy-.12,.4),(xx,yy+.12,.4),.17,'Steel',verts=8)
box((1.5,0,.9),(1.2,1.6,.09),'Rust',rot=(0,.08,.04))
for xx in (-2.1,2.1):
    box((xx,0,.46),(.1,1.82,.13),'Steel')
    for yy in (-.6,.6): box((xx,yy,.69),(.1,.28,.14),'Bone' if xx>0 else 'Red')
for yy in (-.57,.57): cylinder((-1.45,yy,1.46),(.57,yy,1.46),.025,'Steel')
export('Wreck')
# Fuel dispenser with gauge panel, worn safety stripe and hanging hose.
box((0,0,.06),(.68,.55,.12),'Concrete',bevel=.025)
box((0,0,.62),(.50,.38,1.12),'Red',bevel=.04)
box((0,0,1.27),(.61,.44,.44),'Rust',bevel=.035)
box((.307,0,1.31),(.015,.29,.13),'Rubber')
for yy in (-.085,-.028,.028,.085): box((.319,yy,1.31),(.008,.031,.055),'Bone')
for a,b in [((0,.25,1.25),(0,.40,.89)),((0,.40,.89),(.02,.43,.27)),((.02,.43,.27),(.15,.36,.21)),((.15,.36,.21),(.18,.28,.90))]: cylinder(a,b,.019,'Rubber')
box((.17,.27,.98),(.08,.045,.19),'Steel',rot=(0,-.28,0)); export('FuelPump')
# Barrel, wire crate, utility cabinet, desk, shelving.
cylinder((0,0,.03),(0,0,.88),.28,'Red',verts=12)
for zz in (.06,.23,.65,.86): cylinder((0,0,zz),(0,0,zz+.025),.295,'Steel',verts=12)
export('Barrel')
box((0,0,.4),(.9,.9,.8),'Wood')
for z in (.06,.72):
    for yv in (-.46,.46): box((0,yv,z),(.92,.045,.09),'Steel')
export('Crate')
box((0,0,.48),(1.25,.64,.90),'Rust',bevel=.04)
for i in range(9): box((.63,0,.27+i*.048),(.008,.43,.014),'Rubber')
cylinder((-.45,-.25,.07),(-.45,.25,.07),.13,'Rubber')
cylinder((.45,-.25,.07),(.45,.25,.07),.13,'Rubber')
cylinder((-.42,0,.94),(-.42,0,1.20),.04,'Steel'); export('Generator')
box((0,0,.78),(1.4,.7,.065),'Wood')
for xx in (-.6,.6):
    for yy in (-.28,.28): box((xx,yy,.38),(.06,.06,.76),'Steel')
box((-.4,0,.56),(.32,.55,.3),'Rust'); export('Desk')
for xx in (-.63,.63):
    for yy in (-.24,.24): box((xx,yy,.94),(.04,.04,1.88),'Steel')
for zz in (.10,.62,1.15,1.78): box((0,0,zz),(1.32,.56,.06),'Rust')
export('Shelf')
box((0,0,.4),(2.4,.52,.8),'Concrete',bevel=.16)
for yy in (-.27,.27):
    for i in range(8): box((-1.02+i*.28,yy,.43),(.14,.018,.27),'Red',rot=(0,-.4,0))
export('Barrier')
for i in range(9):
    ico((random.uniform(-.6,.6),random.uniform(-.6,.6),random.uniform(.03,.14)),(random.uniform(.1,.4),random.uniform(.1,.3),random.uniform(.08,.19)),'Concrete')
export('Rubble')
# Bare forked trees and a utility pole with transformers and insulators.
for a,b,r,rr in [((0,0,0),(.1,.06,2.6),.19,.12),((.1,.06,2.6),(-.17,.1,5.2),.12,.025),
                ((.1,.06,2.4),(1.3,.22,4.1),.09,.025),((1.3,.22,4.1),(1.55,.48,4.7),.025,.006),
                ((.02,.08,3.4),(-1.2,-.6,4.6),.067,.015),((-.17,.1,4.8),(.47,-.15,5.8),.023,.005)]: cylinder(a,b,r,'Wood',r2=rr,verts=6)
export('DeadTree')
cylinder((0,0,0),(0,0,7.8),.17,'Wood',r2=.11,verts=8)
box((0,0,7.1),(.15,2.5,.15),'Wood')
for yy in (-1.05,-.48,.48,1.05):
    cylinder((0,yy,7.13),(0,yy,7.47),.048,'Steel')
    for z in (7.24,7.32,7.40): cylinder((0,yy,z),(0,yy,z+.03),.076,'Bone')
cylinder((.27,0,5.8),(.27,0,6.7),.28,'Rust',verts=10); export('Pole')
print('Authored FBX mesh library:', len(list(OUT.glob('*.fbx'))))
