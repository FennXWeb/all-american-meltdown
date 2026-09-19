"""Original survivor anatomy, ten haircuts and a twelve-metre Class A coach.
Metres; +X forward. Components have explicit floor/neck pivots.
"""
import sys,json,math,random
from pathlib import Path
sys.path.insert(0,str(Path(__file__).parent));import make_models_v2 as m
import bpy
m.PALETTE.update({'CamperPearlV14':(.8,.77,.67),'CamperWalnutV14':(.25,.16,.08),'CamperLeatherV14':(.67,.62,.5),'CamperStoneV14':(.5,.49,.43)})
m.OUT=m.ROOT/'ArtSource/ModelsV17';m.setup()
# Coach: rear=-10m, front=2m, wide flat Class A front, actual window openings.
p='CamperPearlV14';w='CamperWalnutV14'
m.box((-4,0,.53),(12,2.5,.15),w,.02)
m.box((-4.03,0,3.18),(12.02,2.54,.16),p,.07)
m.box((-9.98,0,1.86),(.14,2.50,2.58),p,.04)
# Hollow hull: cabin floor is Z=60.5cm, never a solid body slab.
for side in [-1,1]:
 for a,b in ([(-10,-.15),(.85,2)] if side==1 else [(-10,2)]):m.box(((a+b)/2,side*1.235,.93),(b-a,.08,.65),p,.02)
for side in [-1,1]:
 y=side*1.235
 
 for a,b in ([(-10,-.15),(.85,2)] if side==1 else [(-10,2)]):m.box(((a+b)/2,y,1.53),(b-a,.08,.82),p,.025)
 m.box((-4,y,2.96),(12,.08,.32),p,.025)
 
 for a,b in ([(-10,-.15),(.85,2)] if side==1 else [(-10,2)]):m.box(((a+b)/2,side*1.27,1.30),(b-a,.04,.12),'Steel',.018)
 for x in ([-9.9,-8.1,-6.1,-4.1,-2.1,1.92] if side==1 else [-9.9,-8.1,-6.1,-4.1,-2.1,-.1,1.92]):m.box((x,y,2.09),(.28,.10,1.50),p,.02)
 for x in ([-9,-7.1,-5.1,-3.1,-1.1] if side==1 else [-9,-7.1,-5.1,-3.1,-1.1,.9]):
  for z in [1.96,2.80]:m.box((x,y,z),(1.69,.11,.05),'Rubber',.012)
  for dx in [-.845,.845]:m.box((x+dx,y,2.38),(.05,.11,.86),'Rubber',.012)
 # Flush luggage lockers, repeating latches, recessed trim.
 for x in [-8.7,-7.45,-6.2,-4.95,-3.7,-2.45,-1.2]:
  m.box((x,side*1.258,.93),(1.18,.02,.48),'Steel',.018)
  m.box((x,side*1.275,1.09),(.12,.014,.04),'Rubber',.008)
 # Flowing original black/silver side ribbons, shallow meshes following body.
 for offset,mat in [(0,'Rubber'),(.14,'Steel'),(.27,'Rubber')]:
  points=[(-9.8,1.02+offset),(-7.5,1.44+offset),(-5,1.13+offset),(-2.5,1.5+offset),(1.86,1.04+offset)]
  verts=[(x,side*1.29,z+dz) for dz in [0,.10] for x,z in points]
  m.mesh('Sweeping coach stripe',verts,[(i,i+1,i+6,i+5) for i in range(4)],mat)
 # Roof-mounted closed awning cassette, mirror arm, mirror glass.
 m.cyl((-8.9,side*1.30,2.96),(.5,side*1.30,2.96),.065,'Steel',n=12)
 m.line([(1.75,side*1.22,1.95),(1.94,side*1.47,1.91),(1.72,side*1.47,1.91)],.04,'Steel')
 m.box((1.73,side*1.47,2.04),(.13,.13,.39),'Steel',.035)
 m.box((1.66,side*1.475,2.04),(.014,.115,.34),'Glass',.02)
 # Dual rear axles + front arches are visually separated from the hull.
 for x in [1.12,-6.25,-7.55]:
  for a in range(11):
   t=math.pi*a/10;t2=math.pi*(a+1)/10
   m.cyl((x+math.cos(t)*.55,side*1.265,.43+math.sin(t)*.55),(x+math.cos(t2)*.55,side*1.265,.43+math.sin(t2)*.55),.045,'Rubber',n=6)
 # Headlamp towers and rear lamps.
 m.box((1.98,side*.97,1.05),(.08,.25,.53),'Rubber',.045)
 for z in [.92,1.15]:m.cyl((2.02,side*.97,z),(2.045,side*.97,z),.075,'Bone',n=16)
 m.box((-10.07,side*.97,1.17),(.025,.20,.40),'Red',.03)
# Bow fascia below windshield; no bonnet protruding ahead of the cab.
m.box((1.92,0,1.24),(.16,2.48,.40),p,.06)
m.box((2.00,0,.79),(.08,1.45,.40),'Rubber',.03)
for z in [.66,.76,.86,.96]:m.box((2.047,0,z),(.015,1.38,.025),'Steel',.006)
m.box((1.91,0,.57),(.22,2.5,.16),'Steel',.05)
m.box((1.88,0,2.99),(.22,2.45,.26),p,.06)
for side in [-1,1]:m.cyl((1.98,side*1.18,1.46),(1.74,side*1.18,2.88),.07,p,n=12)
# Dashboard at the new cab, no inherited sedan interior.
m.box((1.35,0,1.22),(.60,2.22,.15),'Rubber',.04)
m.box((1.08,-.55,1.28),(.09,.65,.24),'Rubber',.04)
for y in [-.72,-.53,-.34]:m.cyl((1.025,y,1.3),(1.03,y,1.3),.058,'Steel',n=16)
for x in [-7.8,-3.8]:
 m.box((x,0,3.36),(1.12,.85,.24),p,.07)
 for d in range(7):m.box((x-.42+d*.14,0,3.49),(.035,.64,.012),'Rubber')
m.finish('CamperShellV17')
m.box((-.50,0,1.06),(.98,.06,2.10),p,.025)
m.box((-.50,.037,1.43),(.68,.02,.85),'Glass',.012)
m.box((-.88,.055,.90),(.05,.035,.18),'Steel',.01)
m.finish('CamperDoorV17')
exec("def hand(name, side):\n    # Wrist-origin glove: neutral curled grasp, fingers +X, palm toward -Z.\n    # Geometry is mirrored, not negatively scaled, so winding/normals remain valid.\n    cyl((-.65, 0, -.22), (-.018, 0, 0), .070, 'Cloth', r2=.032, n=12)\n    cyl((-.025, 0, -.002), (.009, 0, 0), .034, 'Rubber', r2=.030, n=8)\n    box((-.012, side*.028, .007), (.030, .009, .030), 'Rust', bevel=.002)\n    box((-.012, side*.034, .007), (.016, .005, .017), 'Steel', bevel=.002)\n    box((.035, 0, .0), (.077, .067, .033), 'Rubber', bevel=.009)\n    box((.032, 0, .021), (.050, .052, .011), 'Cloth', bevel=.005)\n    for i in range(4):\n        yy = side*((i-1.5)*.017)\n        length = [.050, .059, .055, .043][i]\n        points = [(.062, yy, .001), (.070+length*.40, yy, -.005),\n                  (.074+length*.60, yy, -.029), (.061+length*.44, yy, -.050)]\n        line(points, .0084 if i < 3 else .0075, 'Rubber', 8)\n        box((.071, yy, .015), (.019, .013, .010), 'Steel', bevel=.003)\n        for xx, zz in [(points[1][0], -.001), (points[2][0], -.026)]:\n            box((xx, yy, zz), (.006, .017, .005), 'Cloth', bevel=.001)\n    line([(.010, side*.028, -.005), (.031, side*.048, -.015),\n          (.060, side*.041, -.039), (.069, side*.025, -.041)], .011, 'Rubber', 8)\n    for yy in (-.022, .022):\n        line([(-.112, yy, .014), (-.070, yy*.9, .023), (-.027, yy*.7, .024)], .0013, 'Bone', 4)\n    for i in range(4):\n        box((-.081+i*.012, side*.033, -.012), (.005, .002, .022), 'Rust', rot=(0, -.2, 0))\n    finish(name, motion='Wrist centre pivot. Neutral grip: fingers +X, knuckles +Z, palm -Z; '\n           'thumb lies '+('+Y' if side > 0 else '-Y')+'. Sleeve extends -X. Rigid gloved hand '\n           'with modelled fingers; animate whole component. Weapon socket wrist locations are '\n           'starting anchors; adjust rotation/position for the final camera and grip.')", m.__dict__)
m.hand("LeftHandV17",1);m.hand("RightHandV17",-1)
m.export_all();(m.OUT/'models_v17_manifest.json').write_text(json.dumps({'assets':list(m.RECORDS.values())},indent=2))
bpy.ops.wm.save_as_mainfile(filepath=str(m.OUT/'AllAmericanMeltdown_V17.blend'))
