"""Original modular environment and enemy geometry; Blender 4.2, metres, +X front."""
import sys, json, math
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent))
import make_models_v2 as m
import bpy
m.OUT=m.ROOT/'ArtSource'/'ModelsV3'
m.setup()

def ell(p,s,mat):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=12,ring_count=8,location=p)
    o=bpy.context.object;o.scale=s
    bpy.ops.object.transform_apply(location=False,rotation=False,scale=True)
    m.add(o,mat)

# Full size hinge at origin; panel runs along local +X and rises from the threshold.
m.box((.80,0,1.18),(1.60,.065,2.36),'Wood',.01)
for x in (.12,1.48):m.box((x,-.045,1.18),(.07,.025,2.28),'Steel')
for z in (.14,1.15,2.22):m.box((.8,-.045,z),(1.45,.025,.065),'Steel')
for z in (.25,1.9):m.cyl((0,0,z),(0,0,z+.15),.03,'Rust')
for y in (-.09,.09):
    m.box((1.40,y,1.1),(.04,.06,.17),'Steel',.006)
    m.cyl((1.4,y,1.14),(1.24,y,1.14),.018,'Steel')
m.finish('POIDoor',collision='convex',motion='Hinge yaw 0 to 100 degrees')

for name,style in [('SignFrameV3',0),('SignMarqueeV3',1),('SignPylonV3',2)]:
    m.box((0,0,0),(.12,2.12,1.12),'Rust',.02)
    for y in (-1.03,1.03):m.box((.065,y,0),(.025,.035,1.08),'Steel')
    for z in (-.53,.53):m.box((.065,0,z),(.025,2.06,.035),'Steel')
    for y in (-.92,.92):
        for z in (-.43,.43):m.cyl((.06,y,z),(.078,y,z),.015,'Steel',n=6)
        m.box((-.20,y,-.15),(.38,.055,.07),'Steel')
    if style==1:
        for y in range(-9,10,2):ell((.11,y*.1,.58),(.025,.025,.025),'Glow')
        m.box((.02,0,.65),(.28,2.4,.06),'Red')
    if style==2:
        for y in (-.65,.65):m.box((-.08,y,-1.5),(.13,.13,2.0),'Rust')
    m.finish(name)

# Furniture uses individual drawers, rails, handles and worn upholstery.
for name in ['ClinicBedV3','MotelBedV3']:
    for x in (-.85,.85):
        for y in (-.48,.48):m.cyl((x,y,0),(x,y,.5),.027,'Steel')
    m.box((0,0,.43),(1.95,1.12,.12),'Steel')
    m.box((0,0,.56),(1.90,1.08,.16),'Cloth',.035)
    m.box((-.65,0,.68),(.42,.90,.12),'Bone',.04)
    m.box((.30,0,.66),(1.1,1.1,.04),'Red' if name.startswith('Motel') else 'Cloth')
    for y in (-.59,.59):m.cyl((-.9,y,.86),(.9,y,.86),.018,'Steel')
    m.finish(name,collision='convex')
m.box((0,0,.47),(.64,.60,.12),'Red',.025)
m.box((-.27,0,.83),(.11,.60,.72),'Red',.03)
for x in (-.24,.24):
    for y in (-.24,.24):m.cyl((x,y,0),(x,y,.44),.025,'Steel')
m.finish('ChairV3',collision='convex')
m.box((0,0,.46),(1.5,.62,.90),'Steel',.015)
m.box((0,0,.95),(1.58,.69,.07),'Bone',.015)
for x in (-.48,0,.48):
    for z in (.27,.65):
        m.box((x,-.326,z),(.44,.025,.30),'Rust',.005)
        m.cyl((x-.12,-.37,z+.06),(x+.12,-.37,z+.06),.012,'Steel')
m.finish('CabinetV3',collision='convex')
m.box((0,0,.7),(1.1,.75,1.4),'Rust',.02)
for z in (.25,.6,.95):m.box((.0,-.39,z),(.98,.02,.02),'Steel')
m.box((0,-.42,1.22),(.90,.08,.20),'Rubber')
for x in (-.3,0,.3):ell((x,-.47,1.22),(.055,.015,.055),'Bone')
m.finish('ServiceBenchV3',collision='convex')

# Distinct seven-part articulated bodies use the existing physical joint system.
for prefix,mat in [('Raider','Cloth'),('Mannequin','Bone')]:
    ell((0,0,-.18),(.145,.22,.29),mat)
    if prefix=='Raider':
        m.box((.13,0,-.15),(.08,.34,.35),'Rubber',.015)
        for y in (-.12,0,.12):m.box((.185,y,-.19),(.06,.085,.14),'Cloth',.005)
        m.box((-.16,0,-.16),(.14,.30,.38),'Rust',.02)
    m.finish(prefix+'Torso',collision='convex')
    ell((0,0,-.12),(.11,.10,.145),mat)
    m.cyl((0,0,-.30),(0,0,-.22),.045,'Steel' if prefix=='Mannequin' else 'Skin')
    if prefix=='Raider':
        ell((.085,0,-.15),(.065,.10,.08),'Rubber')
        for y in (-.047,.047):ell((.108,y,-.07),(.025,.034,.02),'Glass')
        m.cyl((.11,0,-.15),(.18,0,-.15),.035,'Steel')
    m.finish(prefix+'Head',collision='convex')
    ell((0,0,-.06),(.13,.17,.13),mat);m.finish(prefix+'Pelvis',collision='convex')
    for part,length,r in [('Arm',.60,.065),('Leg',.82,.085)]:
        ell((0,0,0),(r,r,r),'Steel' if prefix=='Mannequin' else 'Cloth')
        m.cyl((0,0,-.03),(0,0,-length*.48),r,mat,r2=r*.75)
        ell((0,0,-length*.49),(r*.8,r*.8,r*.8),mat)
        m.cyl((0,0,-length*.52),(.02,0,-length),r*.72,mat,r2=r*.55)
        ell((.05,0,-length),(.11 if part=='Leg' else .055,r*.8,.05),'Rubber' if prefix=='Raider' else mat)
        m.finish(prefix+part,collision='convex')

ell((0,0,0),(.37,.17,.20),'Cloth');m.finish('DogTorso',collision='convex')
ell((0,0,0),(.16,.12,.14),'Cloth');ell((.16,0,-.03),(.13,.075,.07),'Skin')
for y in (-.09,.09):
    m.cyl((-.05,y,.06),(-.10,y,.25),.06,'Cloth',r2=.005,n=5)
    ell((.11,y,.045),(.025,.018,.02),'Red')
for y in (-.06,.06):
    for x in (.15,.21,.27):m.cyl((x,y,-.04),(x,y,-.10),.009,'Bone',r2=.002,n=5)
m.finish('DogHead',collision='convex')
ell((0,0,0),(.17,.14,.17),'Cloth');m.line([(0,0,.04),(-.23,0,.12),(-.40,0,.04)],.035,'Cloth');m.finish('DogPelvis',collision='convex')
for part in ('Arm','Leg'):
    m.cyl((0,0,0),(-.05,0,-.17),.045,'Cloth',r2=.032)
    m.cyl((-.05,0,-.17),(.03,0,-.34),.03,'Skin',r2=.02)
    ell((.07,0,-.35),(.085,.045,.035),'Rubber');m.finish('Dog'+part,collision='convex')
m.export_all()
(m.OUT/'models_v3_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2))
bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'LethalWorld_V3.blend'))
print('LW_V3_MODELS_COMPLETE',len(m.RECORDS))
