"""Original LethalWorld modular low-poly assets, authored in metres, +X forward.

Run: blender.exe --background --python Tools/make_models_v2.py
Optional arguments after --: --skip-render, --skip-roundtrip, --fbx-only.
--fbx-only exports the existing packed .blend without rewriting source/previews/manifest.
Writes ONLY ArtSource/ModelsV2. Does not run Unreal or alter the v1 library.
"""
import argparse
import json
import math
import random
import sys
from pathlib import Path

import bmesh
import bpy
from mathutils import Matrix, Vector

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'ArtSource' / 'ModelsV2'
PALETTE = {
    'Concrete': (.44, .43, .36), 'Brick': (.4, .25, .16),
    'Rust': (.34, .38, .31), 'Wood': (.33, .23, .12),
    'Cloth': (.21, .26, .18), 'Skin': (.42, .46, .32),
    'Rubber': (.065, .07, .06), 'Red': (.46, .12, .07),
    'Steel': (.2, .25, .26), 'Bone': (.6, .55, .4),
    'Glow': (.9, .47, .12), 'Glass': (.055, .10, .09),
}
MATS, PARTS, OBJECTS, RECORDS = {}, [], {}, {}
RNG = random.Random(492071)


def setup():
    OUT.mkdir(parents=True, exist_ok=True)
    bpy.context.preferences.filepaths.save_version = 0
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete(use_global=False)
    for m in list(bpy.data.materials):
        bpy.data.materials.remove(m)
    bpy.context.scene.unit_settings.system = 'METRIC'
    bpy.context.scene.unit_settings.scale_length = 1.0
    for name, color in PALETTE.items():
        m = bpy.data.materials.new('M_' + name)
        m.diffuse_color = (*color, 1)
        m.use_nodes = True
        shader = m.node_tree.nodes.get('Principled BSDF')
        shader.inputs['Base Color'].default_value = (*color, 1)
        shader.inputs['Roughness'].default_value = .78
        shader.inputs['Metallic'].default_value = .35 if name in ('Steel', 'Rust') else 0
        texture = ROOT / 'ArtSource' / 'Textures' / ('T_' + name + '.png')
        if texture.exists():
            node = m.node_tree.nodes.new('ShaderNodeTexImage')
            node.image = bpy.data.images.load(str(texture), check_existing=True)
            node.interpolation = 'Closest'
            m.node_tree.links.new(node.outputs['Color'], shader.inputs['Base Color'])
        if name == 'Glow':
            shader.inputs['Emission Color'].default_value = (*color, 1)
            shader.inputs['Emission Strength'].default_value = 1.5
        MATS[name] = m


def add(o, mat):
    o.data.materials.append(MATS[mat])
    PARTS.append(o)
    return o


def mesh(name, vertices, faces, mat):
    data = bpy.data.meshes.new(name)
    data.from_pydata(vertices, [], faces)
    data.update()
    o = bpy.data.objects.new(name, data)
    bpy.context.collection.objects.link(o)
    return add(o, mat)


def box(p, s, mat='Steel', bevel=0, rot=(0, 0, 0)):
    bpy.ops.mesh.primitive_cube_add(size=1, location=p, rotation=rot)
    o = bpy.context.object
    o.scale = s
    bpy.ops.object.transform_apply(location=False, rotation=False, scale=True)
    add(o, mat)
    if bevel:
        mod = o.modifiers.new('Forged chamfers', 'BEVEL')
        mod.width = bevel
        mod.segments = 1
        mod.affect = 'EDGES'
        bpy.ops.object.modifier_apply(modifier=mod.name)
    return o


def cyl(a, b, r, mat='Steel', r2=None, n=10):
    a, b = Vector(a), Vector(b)
    d = b - a
    bpy.ops.mesh.primitive_cone_add(vertices=n, radius1=r,
                                  radius2=r if r2 is None else r2,
                                  depth=d.length, location=(a + b) / 2)
    o = bpy.context.object
    o.rotation_euler = d.to_track_quat('Z', 'Y').to_euler()
    return add(o, mat)


def tube(a, b, outer, inner, mat='Steel', n=12):
    """Closed annulus, real bore and inner wall; no fake black muzzle cap."""
    a, b = Vector(a), Vector(b)
    q = (b - a).to_track_quat('Z', 'Y')
    verts = []
    for center, radius in ((a, outer), (b, outer), (a, inner), (b, inner)):
        for i in range(n):
            t = i * math.tau / n
            verts.append(center + q @ Vector((radius * math.cos(t), radius * math.sin(t), 0)))
    faces = []
    for i in range(n):
        j = (i + 1) % n
        faces += [(i, j, n+j, n+i), (2*n+j, 2*n+i, 3*n+i, 3*n+j),
                  (j, i, 2*n+i, 2*n+j), (n+i, n+j, 3*n+j, 3*n+i)]
    return mesh('Hollow machined tube', verts, faces, mat)


def open_receiver(x0, x1, z, outer=.027, inner=.020):
    """Partial hollow sleeve with a real +Y ejection opening, axis X."""
    n = 11
    verts = []
    for x, radius in ((x0, outer), (x1, outer), (x0, inner), (x1, inner)):
        for i in range(n):
            angle = .68+(math.tau-1.36)*i/(n-1)
            verts.append((x, radius*math.cos(angle), z+radius*math.sin(angle)))
    faces = []
    for i in range(n-1):
        j = i+1
        faces += [(i, j, n+j, n+i), (2*n+j, 2*n+i, 3*n+i, 3*n+j),
                  (j, i, 2*n+i, 2*n+j), (n+i, n+j, 3*n+j, 3*n+i)]
    for i in (0, n-1):
        faces.append((i, n+i, 3*n+i, 2*n+i))
    return mesh('Open receiver ejection channel', verts, faces, 'Steel')


def profile(points, width, mat='Steel', y=0):
    """Extrude a deliberately designed X/Z outline through Y."""
    n = len(points)
    verts = [(x, y + side * width/2, z) for side in (-1, 1) for x, z in points]
    faces = [tuple(reversed(range(n))), tuple(range(n, 2*n))]
    faces += [(i, (i+1) % n, (i+1) % n+n, i+n) for i in range(n)]
    return mesh('Forged profile', verts, faces, mat)


def loft(rings, mat='Cloth', n=8):
    """rings = (x,y,z, radius_x,radius_y), for bags, wrists and fingers."""
    verts = []
    for x, y, z, rx, ry in rings:
        for i in range(n):
            t = math.tau * i/n
            verts.append((x+rx*math.cos(t), y+ry*math.sin(t), z))
    faces = [tuple(reversed(range(n))), tuple(range((len(rings)-1)*n, len(rings)*n))]
    for k in range(len(rings)-1):
        for i in range(n):
            j = (i+1) % n
            faces.append((k*n+i, k*n+j, (k+1)*n+j, (k+1)*n+i))
    return mesh('Tailored loft', verts, faces, mat)


def line(points, radius=.003, mat='Steel', n=6):
    for a, b in zip(points, points[1:]):
        cyl(a, b, radius, mat, n=n)


def screws(xs, y, z, radius=.004):
    sign = 1 if y > 0 else -1
    for x in xs:
        cyl((x, y, z), (x, y+sign*.002, z), radius, 'Steel', n=6)
        box((x, y+sign*.0022, z), (radius*1.3, .0008, .001), 'Rubber')


def chips(x0, x1, y, z, count=12, band=.022):
    """Sparse exposed steel chips and oxide islands, real shallow geometry."""
    for _ in range(count):
        x = RNG.uniform(x0, x1)
        box((x, y, z+RNG.uniform(-band, band)),
            (RNG.uniform(.002, .010), .0007, RNG.uniform(.001, .003)),
            RNG.choice(['Steel', 'Rust', 'Red']), rot=(0, RNG.uniform(-.8, .8), 0))


def rail(x0, x1, z, width=.032):
    box(((x0+x1)/2, 0, z), (x1-x0, width*.7, .006), 'Steel')
    count = max(2, int((x1-x0)/.017))
    for i in range(count):
        box((x0+(i+.5)*(x1-x0)/count, 0, z+.006),
            (.009, width, .007), 'Steel', bevel=.001)


def grip(x=-.025, mat='Rubber'):
    profile([(x-.037, -.031), (x+.015, -.041), (x+.028, -.066),
             (x-.007, -.165), (x-.064, -.152), (x-.050, -.118)], .047, mat)
    for side in (-1, 1):
        for i in range(6):
            box((x-.033+i*.002, side*.024, -.074-i*.012),
                (.035, .002, .005), 'Rust', rot=(0, -.18, 0))
        screws([x-.027], side*.026, -.133, .003)


def guard(x0=-.005, x1=.083, z=-.043):
    line([(x0, 0, z), (x0+.012, 0, z-.050), (x1-.01, 0, z-.050),
          (x1, 0, z-.035), (x1, 0, z)], .0045, 'Steel', 8)
    line([(x0+.037, 0, z), (x0+.030, 0, z-.022),
          (x0+.018, 0, z-.032)], .004, 'Steel', 8)


def sight(x, z, tall=.035):
    box((x, 0, z+tall*.4), (.015, .023, tall*.8), 'Steel', bevel=.002)
    for y in (-.013, .013):
        box((x, y, z+tall), (.014, .005, .022), 'Steel')
    box((x, 0, z+tall-.004), (.008, .004, .007), 'Glow')


def magwell(x, z, length, width):
    for y in (-width/2, width/2):
        box((x, y, z), (length, .006, .025), 'Steel', bevel=.002)
    for xx in (x-length/2, x+length/2):
        box((xx, 0, z), (.006, width, .025), 'Steel')


def bounds(o):
    coords = [v.co for v in o.data.vertices]
    lo = [min(v[i] for v in coords) for i in range(3)]
    hi = [max(v[i] for v in coords) for i in range(3)]
    return {'min': [round(v, 6) for v in lo], 'max': [round(v, 6) for v in hi],
            'size': [round(b-a, 6) for a, b in zip(lo, hi)]}


def finish(name, pivot=(0, 0, 0), parent=None, motion='', sockets=None, collision='none'):
    bpy.ops.object.select_all(action='DESELECT')
    for o in PARTS:
        o.select_set(True)
    bpy.context.view_layer.objects.active = PARTS[0]
    bpy.ops.object.join()
    o = bpy.context.object
    o.name = 'SM_' + name
    bpy.context.scene.cursor.location = pivot
    bpy.ops.object.origin_set(type='ORIGIN_CURSOR')
    bpy.ops.object.transform_apply(location=False, rotation=True, scale=True)
    o.location = (0, 0, 0)
    # Recalculate winding after mirrored/profile construction.
    bm = bmesh.new()
    bm.from_mesh(o.data)
    bmesh.ops.recalc_face_normals(bm, faces=list(bm.faces))
    bm.to_mesh(o.data)
    bm.free()
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(angle_limit=1.10, island_margin=.012)
    bpy.ops.object.mode_set(mode='OBJECT')
    tri = o.modifiers.new('Explicit stable triangulation', 'TRIANGULATE')
    bpy.ops.object.modifier_apply(modifier=tri.name)
    # Bevel clamping on tiny hardware can collapse a corner to a zero-area face.
    bm = bmesh.new()
    bm.from_mesh(o.data)
    bmesh.ops.dissolve_degenerate(bm, dist=1e-7, edges=list(bm.edges))
    empty = [f for f in bm.faces if f.calc_area() < 1e-12]
    if empty:
        bmesh.ops.delete(bm, geom=empty, context='FACES')
    bmesh.ops.triangulate(bm, faces=list(bm.faces))
    bmesh.ops.recalc_face_normals(bm, faces=list(bm.faces))
    bm.to_mesh(o.data)
    bm.free()
    o.data.update()
    for p in o.data.polygons:
        p.use_smooth = False
    # One canonical slot for each imported name; primitives join without .001 suffixes.
    used = sorted({slot.name for slot in o.data.materials})
    old_names = [slot.name for slot in o.data.materials]
    indices = [used.index(old_names[p.material_index]) for p in o.data.polygons]
    o.data.materials.clear()
    for slot_name in used:
        o.data.materials.append(bpy.data.materials[slot_name])
    for p, idx in zip(o.data.polygons, indices):
        p.material_index = idx
    if not all(math.isfinite(c) for v in o.data.vertices for c in v.co):
        raise RuntimeError('Non-finite geometry: ' + name)
    degenerate = sum(p.area < 1e-12 for p in o.data.polygons)
    if degenerate:
        raise RuntimeError(f'{name}: {degenerate} degenerate triangles')
    bm = bmesh.new()
    bm.from_mesh(o.data)
    boundary = sum(e.is_boundary for e in bm.edges)
    bm.free()
    record = {
        'name': name, 'fbx': o.name+'.fbx', 'asset': '/Game/Art/Meshes/'+o.name,
        'parent': parent, 'pivot_definition': 'Origin is the joint; geometry already rebased to this origin.',
        'rest_location_m': list(pivot), 'rest_location_cm': [round(v*100, 4) for v in pivot],
        'rest_rotation_degrees_xyz': [0, 0, 0], 'rest_scale': [1, 1, 1],
        'local_bounds_m': bounds(o), 'triangles': len(o.data.polygons),
        'vertices': len(o.data.vertices), 'material_slots': used,
        'boundary_edges': boundary, 'collision': collision, 'animation': motion,
        'sockets_m': sockets or {},
    }
    RECORDS[name] = record
    OBJECTS[name] = o
    PARTS.clear()
    o['asset_note'] = motion
    o['rest_location_cm'] = [float(v) for v in record['rest_location_cm']]
    return o


def revolver():
    # Open frame around a separate six-chamber cylinder. Long, octagonal heavy barrel.
    profile([(-.085, -.026), (.036, -.026), (.040, .020), (.027, .079),
             (-.031, .092), (-.078, .053)], .041, 'Steel')
    box((.081, 0, .095), (.112, .030, .015), 'Steel', bevel=.002)
    box((.080, 0, -.007), (.121, .026, .014), 'Steel', bevel=.002)
    box((.137, 0, .043), (.028, .043, .097), 'Steel', bevel=.004)
    tube((.148, 0, .045), (.345, 0, .045), .024, .0065, n=8)
    box((.242, 0, .014), (.197, .020, .027), 'Rust', bevel=.003)
    cyl((.146, -.018, .003), (.294, -.018, .003), .005, 'Steel')
    box((.242, 0, .071), (.19, .014, .008), 'Steel')
    sight(.325, .075, .010)
    sight(-.005, .094, .008)
    grip(-.035, 'Wood')
    guard(-.053, .028, -.030)
    profile([(-.073, .058), (-.094, .090), (-.095, .105), (-.065, .087),
             (-.048, .061)], .013, 'Steel')
    for yy in (-.024, .024):
        screws([-.060, -.010, .138], yy, .033, .0035)
        chips(-.075, .13, yy, .032, 10)
        box((-.015, yy, .066), (.025, .003, .011), 'Rust', bevel=.002)
    finish('Revolver', sockets={'muzzle': [.345, 0, .045], 'right_wrist': [-.102, .009, -.188],
                                'left_wrist': [-.065, -.070, -.183]})
    pivot = (.080, 0, .045)
    # An outer sleeve plus six through-bored chamber walls keeps the end face readable.
    tube((.046, 0, .045), (.116, 0, .045), .042, .035, 'Steel', 12)
    cyl((.042, 0, .045), (.119, 0, .045), .009, 'Steel', n=12)
    for i in range(6):
        t = i*math.tau/6
        yy, zz = .025*math.cos(t), .045+.025*math.sin(t)
        tube((.046, yy, zz), (.116, yy, zz), .0115, .007, 'Steel', 8)
        # Spent-case rim and recessed primer on the rear side, separate from barrel bore.
        cyl((.045, yy, zz), (.048, yy, zz), .0065, 'Bone', n=8)
        cyl((.0445, yy, zz), (.045, yy, zz), .0022, 'Rust', n=6)
        cyl((.049, yy*1.40, .045+(zz-.045)*1.40),
            (.112, yy*1.40, .045+(zz-.045)*1.40), .005, 'Rust', n=6)
    finish('RevolverCylinder', pivot, 'Revolver',
           'Spin about local X in 60-degree steps. For swing-out, add a crane parent at '
           '(8,-3.6,-0.5) cm on Revolver; cylinder rest relative to crane is (0,3.6,5) cm. '
           'Rotate the crane about X; do not spin the entire revolver.')


def sniper():
    profile([(-.49, -.065), (-.47, .025), (-.29, .034), (-.15, -.015),
             (.020, -.031), (.043, -.059), (-.20, -.072), (-.23, -.131),
             (-.29, -.13), (-.28, -.080)], .059, 'Wood')
    box((-.482, 0, -.021), (.024, .064, .111), 'Rubber', bevel=.009)
    box((-.355, 0, .033), (.16, .052, .022), 'Rubber', bevel=.005)
    tube((-.09, 0, .040), (-.039, 0, .040), .027, .020, 'Steel', 12)
    open_receiver(-.039, .101, .040)
    tube((.101, 0, .040), (.205, 0, .040), .027, .020, 'Steel', 12)
    profile([(.14, -.038), (.42, -.023), (.46, -.008), (.45, .018),
             (.18, .016)], .057, 'Wood')
    tube((.193, 0, .043), (.875, 0, .043), .0145, .0055, 'Steel', 12)
    tube((.858, 0, .043), (.923, 0, .043), .023, .009, 'Steel', 10)
    for xx in (.872, .893, .913):
        for yy in (-.022, .022):
            box((xx, yy, .043), (.009, .002, .015), 'Rubber')
    guard(-.095, -.006, -.042)
    magwell(.070, -.044, .075, .043)
    rail(-.095, .154, .072)
    for xx in (-.052, .104):
        box((xx, 0, .101), (.023, .033, .04), 'Steel', bevel=.003)
        tube((xx-.012, 0, .138), (xx+.012, 0, .138), .025, .019, 'Steel')
    tube((-.167, 0, .138), (.218, 0, .138), .020, .014, 'Rubber', 12)
    tube((.197, 0, .138), (.276, 0, .138), .035, .028, 'Steel', 12)
    cyl((.268, 0, .138), (.270, 0, .138), .027, 'Glass', n=12)
    tube((-.184, 0, .138), (-.134, 0, .138), .026, .019, 'Rubber', 12)
    cyl((-.182, 0, .138), (-.180, 0, .138), .018, 'Glass', n=12)
    cyl((.028, 0, .151), (.028, 0, .186), .016, 'Steel')
    cyl((.028, .008, .138), (.028, .038, .138), .014, 'Steel')
    for i in range(8):
        t = math.tau*i/8
        box((.028+.014*math.cos(t), .014*math.sin(t), .180), (.004, .004, .014), 'Rubber')
    # Folded twin bipod legs, sling loops, stock repair and witness marks.
    for yy in (-.039, .039):
        cyl((.378, yy, -.009), (.571, yy*1.12, -.027), .006, 'Steel')
        box((.563, yy*1.12, -.028), (.033, .015, .012), 'Rubber')
        screws([-.425, -.269, .218, .392], yy*.78, -.014)
        chips(-.46, -.26, yy*.78, -.035, 9)
    for xx in (-.396, -.371):
        box((xx, 0, -.030), (.018, .063, .100), 'Cloth', bevel=.008)
    finish('Sniper', sockets={'muzzle': [.923, 0, .043], 'right_wrist': [-.25, .005, -.139],
                             'left_wrist': [.32, -.035, -.09], 'sight': [-.19, 0, .138]})
    pivot = (.020, 0, .040)
    cyl((-.078, 0, .040), (.14, 0, .040), .018, 'Steel', n=12)
    cyl((-.081, 0, .040), (-.060, 0, .040), .022, 'Rubber', n=10)
    line([(-.055, .005, .042), (-.055, .058, .037), (-.040, .080, .003)], .007, 'Steel', 8)
    cyl((-.039, .080, -.010), (-.041, .080, .014), .014, 'Rubber', n=10)
    finish('SniperBolt', pivot, 'Sniper',
           'Unlock by rotating +60 degrees about local X, then translate -9 cm local X. '
           'Reverse translation, then rotation to lock. Bolt and handle share the bore-axis pivot.')
    magazine('SniperMag', (.070, 0, -.047), 'Sniper', .068, .034, .088, .004, 'Steel')


def magazine(name, pivot, parent, length, width, height, curve, mat):
    x, y, z = pivot
    profile([(x-length/2, z+.010), (x+length/2, z+.010),
             (x+length/2+curve*.25, z-height*.42), (x+length/2+curve, z-height),
             (x-length*.40+curve, z-height-.008), (x-length/2+curve*.14, z-height*.47)], width, mat)
    box((x+curve, y, z-height), (length*1.14, width*1.17, .012), 'Rubber', bevel=.003)
    for yy in (-width/2-.0008, width/2+.0008):
        for i in range(3):
            xx = x+(i-1)*length*.25
            line([(xx, yy, z-.020), (xx+curve*.20, yy, z-height*.45),
                  (xx+curve*.74, yy, z-height*.83)], .0017, 'Rubber', 4)
        box((x, yy, z-height*.27), (length*.40, .0012, .013), 'Rust')
        chips(x-length*.36, x+length*.36, yy, z-height*.58, 5, height*.2)
    # Feed lips and visible staggered top cartridges; rounds aim +X.
    for yy in (-width*.43, width*.43):
        box((x, yy, z+.015), (length*.90, .003, .010), 'Steel')
    cyl((x-length*.36, -.005, z+.014), (x+length*.24, -.005, z+.014), .0048, 'Bone', n=8)
    cyl((x+length*.24, -.005, z+.014), (x+length*.42, -.005, z+.014), .0048, 'Steel', r2=.001, n=8)
    finish(name, pivot, parent, 'Origin is the top seating point. Remove along local -Z; '
           'use 16 cm travel (SniperMag: 11 cm) before tilting. Reinsert to the exact rest transform.')


def smg():
    # Short pressed receiver, offset folding stock, straight long magazine and vent cage.
    box((.047, 0, .009), (.266, .057, .076), 'Steel', bevel=.007)
    box((.054, 0, .051), (.222, .050, .011), 'Rust', bevel=.003)
    grip(-.058)
    guard(-.065, .018)
    magwell(.084, -.039, .044, .035)
    tube((.17, 0, .019), (.34, 0, .019), .014, .0045, 'Steel')
    for xx in (.192, .242, .284):
        tube((xx, 0, .019), (xx+.018, 0, .019), .029, .022, 'Rust', 10)
    for i in range(6):
        t = i*math.tau/6
        box((.244, .026*math.sin(t), .019+.026*math.cos(t)),
            (.119, .009, .007), 'Steel', rot=(-t, 0, 0))
    tube((.328, 0, .019), (.373, 0, .019), .021, .009, 'Steel', 10)
    rail(-.061, .149, .063)
    sight(-.050, .066, .021)
    sight(.276, .046, .024)
    for yy in (-.026, .026):
        line([(-.081, yy, .032), (-.314, yy, .022), (-.322, yy, -.069),
              (-.292, yy, -.075), (-.070, yy, -.020)], .006, 'Steel', 8)
    box((-.321, 0, -.026), (.019, .067, .105), 'Rubber', bevel=.004)
    cyl((-.070, -.040, -.001), (-.070, .04, -.001), .014, 'Rust')
    box((.061, -.030, .024), (.084, .003, .019), 'Rubber')
    cyl((.077, -.027, .026), (.077, -.061, .026), .006, 'Steel', n=8)
    box((.077, -.062, .026), (.028, .014, .014), 'Rubber', bevel=.002)
    for yy in (-.031, .031):
        screws([-.058, .006, .141], yy, -.009)
        chips(-.077, .163, yy, .014, 14)
        box((-.006, yy, .025), (.039, .001, .008), 'Bone')
    finish('SMG', sockets={'muzzle': [.373, 0, .019], 'right_wrist': [-.14, .007, -.184],
                          'left_wrist': [.205, -.04, -.065]})
    magazine('SMGMag', (.084, 0, -.047), 'SMG', .035, .027, .18, .007, 'Steel')


def rifle():
    # Longer asymmetric angular rifle, curved magazine and slotted skeletal handguard.
    profile([(-.123, -.025), (.188, -.025), (.210, .013), (.163, .056),
             (-.075, .058), (-.127, .034)], .062, 'Rust')
    box((.028, 0, -.031), (.191, .053, .027), 'Steel', bevel=.004)
    grip(-.063)
    guard(-.071, .018)
    magwell(.097, -.048, .071, .041)
    profile([(-.132, .033), (-.380, .025), (-.414, -.007), (-.406, -.109),
             (-.363, -.107), (-.260, -.034), (-.138, -.022)], .049, 'Rust')
    box((-.406, 0, -.046), (.022, .059, .130), 'Rubber', bevel=.006)
    box((-.289, 0, .035), (.152, .055, .017), 'Rubber', bevel=.004)
    for yy in (-.026, .026):
        profile([(-.36, -.005), (-.19, .008), (-.266, -.023), (-.36, -.08)], .0015, 'Rubber', yy)
    tube((.193, 0, .027), (.605, 0, .027), .013, .0045, 'Steel', 12)
    # Handguard is bars surrounding genuine open ventilation gaps.
    for yy in (-.037, .037):
        box((.303, yy, .047), (.224, .008, .021), 'Rust', bevel=.002)
        box((.303, yy, -.024), (.224, .008, .020), 'Rust', bevel=.002)
        for i in range(6):
            box((.204+i*.041, yy, .012), (.013, .008, .055), 'Rust', bevel=.002)
        chips(.203, .411, yy*1.12, -.018, 10, .006)
    for xx in (.2, .416):
        tube((xx-.006, 0, .01), (xx+.006, 0, .01), .047, .036, 'Steel', 8)
    rail(-.102, .404, .068, .034)
    rail(.247, .392, -.043, .035)
    profile([(.294, -.048), (.318, -.052), (.303, -.096), (.279, -.096)], .028, 'Rubber')
    tube((.581, 0, .027), (.644, 0, .027), .022, .010, 'Steel', 8)
    for yy in (-.021, .021):
        for xx in (.596, .614, .631):
            box((xx, yy, .028), (.008, .002, .013), 'Rubber')
    sight(.448, .039, .034)
    sight(-.076, .080, .022)
    box((.038, .032, .024), (.103, .003, .029), 'Rubber')
    box((.038, .035, .010), (.110, .008, .008), 'Steel')
    cyl((-.103, -.035, .034), (-.103, -.065, .034), .006, 'Steel')
    box((-.11, -.061, .034), (.037, .011, .012), 'Rubber', bevel=.002)
    for yy in (-.033, .033):
        screws([-.073, -.012, .124], yy, -.006)
        chips(-.11, .15, yy, .017, 17)
        box((-.061, yy, .043), (.025, .0015, .007), 'Bone')
    finish('Rifle', sockets={'muzzle': [.644, 0, .027], 'right_wrist': [-.14, .005, -.183],
                            'left_wrist': [.299, -.035, -.11]})
    magazine('RifleMag', (.097, 0, -.052), 'Rifle', .061, .034, .17, .047, 'Rust')


def lmg():
    # Broad feed tray, heavy finned barrel, left feed belt, folding bipod and carry handle.
    box((.015, 0, .001), (.300, .089, .085), 'Steel', bevel=.006)
    box((.085, 0, .048), (.19, .094, .012), 'Rubber')
    for yy in (-.046, .046):
        box((.025, yy, .034), (.265, .009, .023), 'Rust', bevel=.002)
    grip(-.073)
    guard(-.084, .003)
    profile([(-.135, .023), (-.365, .009), (-.432, -.037), (-.421, -.130),
             (-.347, -.119), (-.295, -.055), (-.137, -.029)], .075, 'Wood')
    box((-.425, 0, -.078), (.02, .082, .123), 'Steel', bevel=.005)
    box((-.310, 0, .008), (.154, .076, .020), 'Cloth', bevel=.005)
    tube((.158, 0, .028), (.743, 0, .028), .022, .0065, 'Steel', 12)
    for i in range(10):
        tube((.204+i*.027, 0, .028), (.211+i*.027, 0, .028), .032, .023, 'Rust', 10)
    tube((.724, 0, .028), (.805, 0, .028), .032, .015, 'Steel', 12)
    cyl((.155, 0, -.020), (.621, 0, -.020), .012, 'Steel')
    box((.31, 0, -.044), (.231, .057, .026), 'Wood', bevel=.006)
    for i in range(5):
        box((.220+i*.040, 0, -.051), (.012, .061, .025), 'Steel', bevel=.003)
    # Raised handle beside bore leaves room for lid rotation.
    line([(.308, .048, .047), (.29, .077, .130), (.408, .077, .135),
          (.453, .040, .050)], .008, 'Steel', 8)
    cyl((.303, .077, .131), (.392, .077, .135), .014, 'Rubber', n=10)
    for yy in (-.035, .035):
        line([(.567, yy, .019), (.503, yy*1.8, -.078), (.375, yy*2, -.126)], .007, 'Steel', 8)
        box((.376, yy*2, -.127), (.060, .033, .009), 'Steel', bevel=.002)
    sight(.653, .049, .037)
    sight(-.105, .054, .045)
    # Feed mouth faces -Y and is hollow between tray and lid.
    box((.063, -.064, .003), (.136, .045, .010), 'Steel')
    box((.063, -.078, .030), (.120, .006, .029), 'Rubber')
    box((.020, .047, .013), (.092, .003, .023), 'Rubber')
    cyl((.046, .044, .005), (.046, .081, .005), .007, 'Steel')
    for yy in (-.050, .050):
        screws([-.113, -.053, .136], yy, -.015, .005)
        chips(-.120, .16, yy, .008, 18)
    finish('LMG', sockets={'muzzle': [.805, 0, .028], 'right_wrist': [-.15, .004, -.184],
                          'left_wrist': [.30, -.047, -.111]})
    pivot = (.173, 0, .068)
    box((.037, 0, .066), (.272, .092, .024), 'Rust', bevel=.005)
    cyl((.173, -.057, .068), (.173, .057, .068), .012, 'Steel', n=10)
    for xx in (-.069, -.025, .019, .063, .107):
        box((xx, 0, .081), (.014, .083, .008), 'Steel', bevel=.002)
    box((-.097, 0, .056), (.019, .052, .028), 'Steel', bevel=.003)
    box((.018, 0, .083), (.040, .039, .004), 'Bone')
    finish('LMGCover', pivot, 'LMG',
           'Front hinge, local Y axis. Rotate +75 degrees about Y to lift the rear of the lid. '
           'In Unreal use FQuat(FVector::YAxisVector, angleRadians).')
    pivot = (.063, -.065, .021)
    for i in range(9):
        yy = -.065-i*.0135
        zz = .021 - max(0, i-3)**1.45*.005
        cyl((.034, yy, zz), (.087, yy, zz), .0055, 'Bone', n=8)
        cyl((.087, yy, zz), (.105, yy, zz), .0055, 'Steel', r2=.001, n=8)
        tube((.046, yy, zz), (.054, yy, zz), .0072, .0058, 'Steel', 8)
        box((.057, yy-.007, zz-.002), (.018, .011, .004), 'Rust')
    finish('LMGBelt', pivot, 'LMG',
           'Rigid nine-round belt segment, not a skinned chain. Feed along local +Y by 1.35 cm '
           'per shot and reset after one pitch; hide/swap on depletion. Use multiple instances '
           'or a procedural chain for flexible reload deformation.')


def utilities():
    # Reusable ammunition can. Origin on support surface; positioned under LMG feed when attached.
    box((0, 0, .067), (.190, .133, .134), 'Rust', bevel=.008)
    box((0, 0, .140), (.205, .145, .016), 'Steel', bevel=.003)
    for yy in (-.068, .068):
        for xx in (-.071, .071):
            box((xx, yy, .066), (.014, .008, .115), 'Steel', bevel=.002)
        box((0, yy, .077), (.087, .002, .039), 'Bone')
        for i in range(4):
            box((-.025+i*.017, yy*1.025, .079), (.006, .001, .024), 'Rubber')
        chips(-.084, .084, yy*1.01, .042, 11, .023)
    line([(-.049, 0, .151), (-.045, 0, .180), (.044, 0, .180),
          (.050, 0, .151)], .005, 'Steel', 8)
    box((.099, 0, .116), (.011, .035, .036), 'Steel', bevel=.003)
    finish('AmmoBox', motion='Floor pivot. Suggested LMG-relative location: (6,-14,-22) cm.', collision='convex')
    # Cartridge origin at primer centre, tip +X; rim/body/shoulder/projectile differentiated.
    cyl((0, 0, 0), (.003, 0, 0), .006, 'Bone', n=10)
    cyl((.003, 0, 0), (.041, 0, 0), .0052, 'Bone', r2=.0048, n=10)
    cyl((.041, 0, 0), (.046, 0, 0), .0048, 'Bone', r2=.0032, n=10)
    cyl((.046, 0, 0), (.052, 0, 0), .0032, 'Steel', n=10)
    cyl((.052, 0, 0), (.066, 0, 0), .0032, 'Steel', r2=.0006, n=10)
    cyl((-.0005, 0, 0), (0, 0, 0), .0018, 'Rust', n=8)
    finish('Cartridge', motion='Primer centre pivot. Projectile points +X. Full cartridge, not an empty casing.')


def hand(name, side):
    # Wrist-origin glove: neutral curled grasp, fingers +X, palm toward -Z.
    # Geometry is mirrored, not negatively scaled, so winding/normals remain valid.
    cyl((-.125, 0, -.013), (-.018, 0, 0), .040, 'Cloth', r2=.032, n=8)
    cyl((-.025, 0, -.002), (.009, 0, 0), .034, 'Rubber', r2=.030, n=8)
    box((-.012, side*.028, .007), (.030, .009, .030), 'Rust', bevel=.002)
    box((-.012, side*.034, .007), (.016, .005, .017), 'Steel', bevel=.002)
    box((.035, 0, .0), (.077, .067, .033), 'Rubber', bevel=.009)
    box((.032, 0, .021), (.050, .052, .011), 'Cloth', bevel=.005)
    for i in range(4):
        yy = side*((i-1.5)*.017)
        length = [.050, .059, .055, .043][i]
        points = [(.062, yy, .001), (.070+length*.40, yy, -.005),
                  (.074+length*.60, yy, -.029), (.061+length*.44, yy, -.050)]
        line(points, .0084 if i < 3 else .0075, 'Rubber', 8)
        box((.071, yy, .015), (.019, .013, .010), 'Steel', bevel=.003)
        for xx, zz in [(points[1][0], -.001), (points[2][0], -.026)]:
            box((xx, yy, zz), (.006, .017, .005), 'Cloth', bevel=.001)
    line([(.010, side*.028, -.005), (.031, side*.048, -.015),
          (.060, side*.041, -.039), (.069, side*.025, -.041)], .011, 'Rubber', 8)
    for yy in (-.022, .022):
        line([(-.112, yy, .014), (-.070, yy*.9, .023), (-.027, yy*.7, .024)], .0013, 'Bone', 4)
    for i in range(4):
        box((-.081+i*.012, side*.033, -.012), (.005, .002, .022), 'Rust', rot=(0, -.2, 0))
    finish(name, motion='Wrist centre pivot. Neutral grip: fingers +X, knuckles +Z, palm -Z; '
           'thumb lies '+('+Y' if side > 0 else '-Y')+'. Sleeve extends -X. Rigid gloved hand '
           'with modelled fingers; animate whole component. Weapon socket wrist locations are '
           'starting anchors; adjust rotation/position for the final camera and grip.')


def trader():
    # Body origin at ground level; a compact one-axle salvage vending automaton.
    box((0, 0, .44), (.44, .52, .22), 'Steel', bevel=.045)
    box((-.018, 0, .755), (.39, .47, .47), 'Rust', bevel=.045)
    box((.187, 0, .784), (.044, .37, .35), 'Steel', bevel=.014)
    box((.214, -.044, .844), (.018, .225, .157), 'Rubber', bevel=.008)
    box((.225, -.044, .844), (.009, .196, .129), 'Glass', bevel=.004)
    # Physical display glyphs, three status lamps, coin slot and recessed dispensing chute.
    for i in range(4):
        box((.231, -.115+i*.041, .858), (.005, .021, .010), 'Glow')
    for i in range(3):
        cyl((.217, .143, .79+i*.045), (.232, .143, .79+i*.045), .012,
            'Glow' if i == 2 else 'Red', n=8)
    box((.232, -.017, .732), (.014, .094, .010), 'Rubber')
    box((.219, -.030, .593), (.013, .255, .079), 'Rubber')
    box((.259, -.030, .550), (.097, .278, .013), 'Steel', bevel=.004)
    for yy in (-.171, .111):
        box((.249, yy, .579), (.076, .012, .059), 'Steel', bevel=.003)
    for yy in (-.241, .241):
        for i in range(6):
            box((-.075, yy, .687+i*.029), (.183, .004, .011), 'Rubber')
        for xx in (-.13, .12):
            cyl((xx, yy, .94), (xx, yy*1.03, .94), .011, 'Steel', n=6)
        chips(-.17, .14, yy*1.014, .770, 25, .19)
    # Flexible-looking exposed cable routed in short low-poly segments.
    for yy in (-.24, .24):
        line([(-.10, yy, .89), (-.25, yy, .79), (-.27, yy, .58),
              (-.10, yy, .44)], .016, 'Rubber', 8)
    cyl((0, -.37, .265), (0, .37, .265), .042, 'Steel', n=12)
    cyl((-.017, 0, .99), (-.017, 0, 1.084), .069, 'Steel', n=10)
    # One practical service arm and three-prong tray claw.
    cyl((.017, -.270, .91), (.03, -.317, .91), .066, 'Steel', n=10)
    line([(.03, -.301, .91), (.142, -.385, .75), (.319, -.369, .737)], .032, 'Rust', 8)
    cyl((.139, -.411, .75), (.139, -.359, .75), .046, 'Steel')
    for z in (.708, .751):
        line([(.314, -.370, .737), (.373, -.378, z), (.403, -.353, z)], .012, 'Steel', 6)
    finish('TraderBody', collision='convex', motion='Ground/root pivot. Head and both wheels attach using manifest placements.')
    pivot = (-.017, 0, 1.061)
    box((-.017, 0, 1.151), (.252, .354, .194), 'Rust', bevel=.035)
    box((.111, 0, 1.150), (.024, .294, .126), 'Steel', bevel=.012)
    for yy, rad in [(-.085, .047), (.081, .034)]:
        tube((.12, yy, 1.166), (.165, yy, 1.166), rad, rad*.73, 'Rubber', 10)
        cyl((.157, yy, 1.166), (.16, yy, 1.166), rad*.71, 'Glow', n=10)
        box((.169, yy, 1.176), (.002, rad*1.3, .007), 'Steel')
    for i in range(5):
        box((.130, -.052+i*.026, 1.102), (.005, .011, .022), 'Rubber')
    cyl((-.091, .113, 1.23), (-.118, .117, 1.408), .006, 'Steel', n=6)
    cyl((-.118, .117, 1.394), (-.118, .117, 1.419), .013, 'Red', n=8)
    for yy in (-.174, .174):
        cyl((-.017, yy, 1.150), (-.017, yy*1.10, 1.150), .046, 'Steel', n=10)
        chips(-.12, .09, yy, 1.176, 12, .041)
    finish('TraderHead', pivot, 'TraderBody', 'Neck yaw about Z; pitch about Y. Default joint (-1.7,0,106.1) cm.')
    # Local axle pivot; instantiate twice. Tread is modelled all round the circumference.
    cyl((0, -.071, 0), (0, .071, 0), .257, 'Rubber', n=16)
    for yy in (-.073, .073):
        tube((0, yy, 0), (0, yy*1.11, 0), .178, .122, 'Rust', 12)
        cyl((0, yy, 0), (0, yy*1.16, 0), .107, 'Steel', n=10)
        for i in range(6):
            t = math.tau*i/6
            xx, zz = .077*math.cos(t), .077*math.sin(t)
            cyl((xx, yy, zz), (xx, yy*1.20, zz), .011, 'Bone', n=6)
    for i in range(20):
        t = math.tau*i/20
        box((.253*math.sin(t), 0, .253*math.cos(t)), (.054, .147, .021),
            'Rubber', bevel=.003, rot=(0, t, .13 if i % 2 else -.13))
    finish('TraderWheel', (0, 0, 0), 'TraderBody',
           'Axle pivot, rotate about local Y. Instantiate at (0,-32,26.5) cm and '
           '(0,32,26.5) cm; no mirror scale needed. Rolling radius 26.4 cm.', collision='convex')
    # Authored axle is zero; the default attachment uses the left wheel placement.
    RECORDS['TraderWheel']['rest_location_m'] = [0, -.32, .265]
    RECORDS['TraderWheel']['rest_location_cm'] = [0, -32, 26.5]
    OBJECTS['TraderWheel']['rest_location_cm'] = [0.0, -32.0, 26.5]
    RECORDS['TraderWheel']['instances_cm'] = [[0, -32, 26.5], [0, 32, 26.5]]


def loot():
    # Flattened base, asymmetric sewn panels, compressed mouth, cord and folded handles.
    loft([(0, 0, 0, .15, .13), (.005, 0, .066, .221, .169),
          (-.013, .012, .215, .208, .159), (.005, .0, .336, .146, .120),
          (.007, .01, .378, .082, .058), (.016, .009, .407, .097, .065)], 'Cloth', 12)
    for side in (-1, 1):
        profile([(-.10, .080), (.12, .092), (.12, .226), (.05, .255),
                 (-.096, .216)], .004, 'Rust', y=side*.153)
        for i in range(9):
            box((-.086+i*.023, side*.158, .099+i*.001), (.009, .002, .003), 'Bone')
        line([(-.124, side*.107, .07), (-.165, side*.103, .23),
              (-.086, side*.064, .367)], .003, 'Rubber', 6)
    tube((.007, .01, .370), (.007, .01, .383), .083, .074, 'Rubber', 12)
    line([(.016, -.061, .383), (.111, -.083, .376), (.13, -.121, .263)], .004, 'Bone', 6)
    line([(.016, -.061, .383), (.077, -.089, .431), (.128, -.074, .407),
          (.062, -.080, .377)], .004, 'Bone', 6)
    for yy in (-.073, .073):
        line([(-.106, yy, .311), (-.11, yy, .47), (.043, yy, .496),
              (.113, yy, .316)], .012, 'Cloth', 6)
    box((.099, -.115, .303), (.061, .004, .042), 'Bone', rot=(0, .2, -.2))
    box((.102, -.119, .302), (.038, .002, .009), 'Red', rot=(0, .2, -.2))
    finish('LootBag', motion='Floor-centred pivot. Visible drawcord and empty handle loops.', collision='convex')


def vault():
    # +X is the outward normal. Four separate jamb/header/sill solids leave a real aperture.
    # Interior clear opening: Y +/- .77, Z .12 .. 2.37; frame thickness .32 along X.
    for yy in (-.87, .87):
        box((0, yy, 1.245), (.32, .20, 2.49), 'Concrete', bevel=.018)
        box((.176, yy, 1.245), (.039, .17, 2.46), 'Steel', bevel=.008)
    box((0, 0, 2.46), (.32, 1.56, .18), 'Concrete', bevel=.012)
    box((0, 0, .06), (.32, 1.56, .12), 'Steel', bevel=.008)
    box((.18, 0, 2.46), (.04, 1.56, .14), 'Rust', bevel=.004)
    for yy in (-.87, .87):
        for zz in (.25, .67, 1.10, 1.57, 2.17):
            cyl((.194, yy, zz), (.221, yy, zz), .019, 'Steel', n=6)
    finish('BunkerDoorFrame', motion='Ground centre pivot. Clear opening 154 cm wide, '
           '225 cm tall from Z=12 to 237 cm. +X faces exterior. Includes four separate UCX '
           'collision boxes; use simple collision so the aperture remains passable.', collision='frame_ucx')
    pivot = (.22, -.735, .145)
    box((.220, 0, 1.235), (.16, 1.46, 2.17), 'Rust', bevel=.030)
    box((.307, 0, 1.235), (.025, 1.33, 2.00), 'Steel', bevel=.025)
    for yy in (-.65, .65):
        box((.325, yy, 1.235), (.026, .035, 1.96), 'Rubber')
    for zz in (.29, 2.18):
        box((.325, 0, zz), (.026, 1.31, .035), 'Rubber')
    for zz in (.52, 1.23, 1.94):
        box((.348, 0, zz), (.060, 1.28, .072), 'Steel', bevel=.012)
    for yy in (-.46, .46):
        box((.345, yy, 1.23), (.039, .067, 1.83), 'Steel', bevel=.006)
    for zz in (.53, 1.94):
        cyl((.22, -.735, zz-.10), (.22, -.735, zz+.10), .068, 'Steel', n=12)
        cyl((.22, -.735, zz+.10), (.22, -.735, zz+.12), .077, 'Rust', n=10)
    for zz in (.50, 1.22, 1.95):
        cyl((.358, .50, zz), (.358, .75, zz), .025, 'Steel', n=10)
    for yy in (-.58, .58):
        for zz in (.35, .78, 1.65, 2.10):
            cyl((.321, yy, zz), (.351, yy, zz), .016, 'Steel', n=6)
    box((.336, -.10, 1.82), (.018, .34, .14), 'Bone', bevel=.004)
    for i in range(4):
        box((.347, -.20+i*.063, 1.82), (.004, .034, .071), 'Rubber')
    finish('BunkerDoor', pivot, 'BunkerDoorFrame',
           'Vertical hinge pivot. From closed, rotate -100 degrees about local Z to open '
           'outward toward +X. Decorative bolt ends turn with leaf; no independent bolt '
           'retraction. Clearance around the leaf is intentional.', collision='convex')
    # Wheel authored in leaf-local coordinates, then rebased to spindle root.
    pivot = (.166, .735, 1.085)
    tube((.216, .735, 1.085), (.242, .735, 1.085), .226, .199, 'Steel', 16)
    cyl((.136, .735, 1.085), (.252, .735, 1.085), .040, 'Steel', n=12)
    for i in range(5):
        t = i*math.tau/5
        cyl((.23, .735, 1.085), (.23, .735+.210*math.sin(t), 1.085+.210*math.cos(t)), .013, 'Rust', n=8)
    cyl((.225, .735, 1.285), (.306, .735, 1.285), .021, 'Rubber', n=10)
    finish('BunkerDoorWheel', pivot, 'BunkerDoor', 'Spindle pivot, rotate local X. '
           'Rest location is relative to the hinged BunkerDoor, not to the frame.')


def reflected_export_copy(source):
    """Bake UE's Y handedness correction into a disposable mesh, never object scale.

    UE's FBX importer maps (x,y,z) -> (x,-y,z). Reflect export geometry once here
    so Unreal reconstructs the original authored coordinates. Reversing faces
    preserves outward normals and their UV corners after the negative determinant.
    Bake world transforms too: UCX jambs have nonzero centres that must also reflect.
    """
    reflection = Matrix.Diagonal((1.0, -1.0, 1.0, 1.0))
    original_name = source.name
    source.name = original_name + '_source_export'
    copied = source.copy()
    copied.data = source.data.copy()
    copied.name = original_name
    bpy.context.scene.collection.objects.link(copied)
    transform = reflection @ source.matrix_world
    bm = bmesh.new()
    bm.from_mesh(copied.data)
    bmesh.ops.transform(bm, matrix=transform, verts=list(bm.verts))
    bmesh.ops.reverse_faces(bm, faces=list(bm.faces))
    bm.normal_update()
    bm.to_mesh(copied.data)
    bm.free()
    copied.matrix_world = Matrix.Identity(4)
    copied.hide_set(False)
    copied.hide_viewport = False
    copied.hide_render = False
    copied.data.update()
    assert len(copied.data.vertices) == len(source.data.vertices), original_name
    assert len(copied.data.polygons) == len(source.data.polygons), original_name
    # Compare every reflected vertex and normal, including symmetric meshes and UCX.
    for a, b in zip(source.data.vertices, copied.data.vertices):
        assert (transform @ a.co-b.co).length < 1e-6, original_name
    normal_transform = reflection.to_3x3() @ source.matrix_world.to_3x3().inverted().transposed()
    for a, b in zip(source.data.polygons, copied.data.polygons):
        expected = (normal_transform @ a.normal).normalized()
        assert expected.dot(b.normal) > .9999, (original_name, 'reflected winding/normal', a.index)
        assert a.material_index == b.material_index, original_name
        if source.data.uv_layers:
            # Winding changes loop order. Compare UV by vertex identity, not loop index.
            src_uv = {source.data.loops[i].vertex_index: source.data.uv_layers[0].data[i].uv.copy()
                      for i in a.loop_indices}
            for i in b.loop_indices:
                assert (copied.data.uv_layers[0].data[i].uv-src_uv[copied.data.loops[i].vertex_index]).length < 1e-6, original_name
    return copied, original_name


def export_all():
    for name, o in OBJECTS.items():
        collision_parts = []
        if name == 'BunkerDoorFrame':
            for i, (p, s) in enumerate([
                ((0, -.87, 1.245), (.32, .20, 2.49)),
                ((0, .87, 1.245), (.32, .20, 2.49)),
                ((0, 0, 2.46), (.32, 1.54, .18)),
                ((0, 0, .06), (.32, 1.54, .12)),
            ]):
                c = box(p, s)
                c.name = f'UCX_SM_BunkerDoorFrame_{i:02d}'
                collision_parts.append(c)
            PARTS.clear()
        temporary = []
        try:
            for source in [o] + collision_parts:
                copied, original_name = reflected_export_copy(source)
                temporary.append((source, copied, original_name))
            bpy.ops.object.select_all(action='DESELECT')
            for _, copied, _ in temporary:
                copied.select_set(True)
            bpy.context.view_layer.objects.active = temporary[0][1]
            # Replace only complete files, so a concurrent reader never opens half an FBX.
            target = OUT / RECORDS[name]['fbx']
            staging = target.with_suffix('.pending.fbx')
            bpy.ops.export_scene.fbx(
                filepath=str(staging), use_selection=True,
                object_types={'MESH'}, global_scale=1.0, apply_unit_scale=True,
                axis_forward='-Y', axis_up='Z', bake_anim=False,
                mesh_smooth_type='FACE', use_mesh_modifiers=True,
                use_custom_props=True, path_mode='RELATIVE', embed_textures=False,
            )
            assert staging.read_bytes().startswith(b'Kaydara FBX Binary'), name
            staging.replace(target)
        finally:
            for source, copied, original_name in temporary:
                data = copied.data
                bpy.data.objects.remove(copied, do_unlink=True)
                bpy.data.meshes.remove(data)
                source.name = original_name
            for c in collision_parts:
                data = c.data
                bpy.data.objects.remove(c, do_unlink=True)
                bpy.data.meshes.remove(data)
        print(f'V2_EXPORT {name} triangles={RECORDS[name]["triangles"]} UE_Y_CORRECTED', flush=True)


def roundtrip():
    """Reimport FBX, apply UE's known Y reflection, compare authored geometry/slots."""
    for name, source in OBJECTS.items():
        before = set(bpy.data.objects)
        prior_meshes = set(bpy.data.meshes)
        prior_materials = set(bpy.data.materials)
        prior_images = set(bpy.data.images)
        bpy.ops.import_scene.fbx(filepath=str(OUT / RECORDS[name]['fbx']), use_anim=False)
        imported = set(bpy.data.objects) - before
        candidates = [o for o in imported if o.type == 'MESH' and not o.name.startswith('UCX_')]
        assert len(candidates) == 1, (name, len(candidates))
        o = candidates[0]
        reflection = Matrix.Diagonal((1.0, -1.0, 1.0, 1.0))
        coords = [reflection @ o.matrix_world @ v.co for v in o.data.vertices]
        lo = [min(v[i] for v in coords) for i in range(3)]
        hi = [max(v[i] for v in coords) for i in range(3)]
        expected = RECORDS[name]['local_bounds_m']
        err = max(abs(a-b) for a, b in zip(lo+hi, expected['min']+expected['max']))
        assert err < .00002, (name, 'FBX bound error (m)', err)
        assert len(o.data.polygons) == RECORDS[name]['triangles'], name
        # Blender appends .001 when an existing canonical material is present.
        slots = sorted(m.name.split('.')[0] for m in o.data.materials)
        assert slots == RECORDS[name]['material_slots'], (name, slots)
        assert o.data.uv_layers and len(o.data.uv_layers[0].data) == len(o.data.loops), name
        if name == 'BunkerDoorFrame':
            assert len([v for v in imported if v.name.startswith('UCX_')]) == 4
        RECORDS[name]['fbx_roundtrip'] = {'passed': True, 'max_bound_error_m': round(err, 8),
                                        'space': 'authored coordinates after Unreal Y handedness conversion'}
        for obj in imported:
            bpy.data.objects.remove(obj, do_unlink=True)
        for data in set(bpy.data.meshes)-prior_meshes:
            bpy.data.meshes.remove(data)
        for data in set(bpy.data.materials)-prior_materials:
            bpy.data.materials.remove(data)
        for data in set(bpy.data.images)-prior_images:
            bpy.data.images.remove(data)
    print('V2_ROUNDTRIP_ALL_PASSED', flush=True)


def documentation():
    data = {
        'generator': 'Tools/make_models_v2.py', 'version': 2,
        'source_units': 'metres', 'unreal_units': 'centimetres',
        'forward': '+X', 'up': '+Z', 'side_axis': '+Y',
        'fbx_axis_forward': '-Y', 'fbx_axis_up': 'Z',
        'fbx_geometry_pre_reflection': [1, -1, 1],
        'import_uniform_scale': 1.0,
        'coordinate_note': 'Same FBX axis/unit flags as make_models.py, plus export-only Y vertex '
                           'reflection and reversed winding to compensate Unreal handedness. Import with scene and '
                           'scene-unit conversion on, force_front_x_axis off. Bounds/importer '
                           'validation must pass before runtime placement. Never multiply scale by 100.',
        'assets': list(RECORDS.values()),
    }
    (OUT/'models_v2_manifest.json').write_text(json.dumps(data, indent=2)+'\n', encoding='utf-8')
    lines = [
        '# LethalWorld original modular mesh library v2', '',
        'Generated by `Tools/make_models_v2.py` in Blender 4.2. All surfaces and silhouettes are '
        'original procedural geometry. Existing ArtSource textures are reused read-only. No downloaded meshes.', '',
        '## Import and assembly', '',
        'Run `Tools/import_models_v2.py` inside the main agent\'s Unreal Editor Python session. '
        'It imports only this manifest into `/Game/Art/Meshes`, assigns existing `/Game/Materials/M_*` '
        'by imported slot name, and checks the centimetre bounds. No materials are created. '
        'The generator never launches Unreal.', '',
        'Blender geometry is in metres, +X muzzle/forward, +Z up. FBX uses the existing pipeline\'s '
        '`axis_forward=-Y, axis_up=Z, apply_unit_scale=True`. UE import scale is **1**, '
        'with scene/unit conversion enabled and force-front-X disabled. One metre is 100 UE units. '
        'All Blender mesh object transforms are identity before export and vertices are relative '
        'to the documented pivot. Rest translations below are relative to the named parent; '
        'all rest rotations are zero and scales one. Source mesh bounds exclude UCX collision.', '',
        'Export-only copies reflect Y and reverse winding (including UCX); Unreal\'s handedness '
        'conversion then restores the documented coordinates. The Blender source, preview assemblies, '
        'attachment transforms and manifest bounds are never reflected. No negative object scale is exported.', '',
        'Weapon base meshes exclude their detachable cylinder, bolt, magazines, belt and lid. '
        'Attach these assets at the rest translations for a complete weapon. These are static '
        'mesh components for runtime animation, with **no skeletal rig or baked animation clips**. '
        'The belt is one rigid curved segment. Hands are separate rigid gloves, including sleeve cuffs; '
        'wrist sockets are starting anchors, not final fitted poses.', '',
        '## Exact pivots and local bounds', '',
        'Every dimension below is in **centimetres**. Min/max are pivot-relative, before the rest transform.', '',
        '| Mesh | Parent | Rest XYZ | Local min XYZ | Local max XYZ | Triangles |',
        '|---|---|---|---|---|---:|',
    ]
    fmt = lambda seq: ', '.join(f'{v:.3f}' for v in seq)
    for r in RECORDS.values():
        b = r['local_bounds_m']
        lines.append(f'| {r["name"]} | {r["parent"] or "root"} | {fmt(r["rest_location_cm"])} | '
                     f'{fmt([v*100 for v in b["min"]])} | {fmt([v*100 for v in b["max"]])} | {r["triangles"]} |')
    lines += ['', '## Animation and attachment notes', '']
    for r in RECORDS.values():
        if r['animation']:
            lines.append(f'- **{r["name"]}:** {r["animation"]}')
    lines += ['', 'The cylinder crane, wrist sockets and suggested ammo-can placement are metadata '
              'for native component setup; the importer does not create sockets or actors. TraderWheel '
              'has two instance placements in the JSON. SniperBolt rotates/translates around its '
              'own bore axis. BunkerDoorWheel is a child of BunkerDoor and follows its hinge motion.', '',
              'Use explicit local axis quaternions for the stated motions; Blender XYZ Euler and '
              'Unreal FRotator field/sign conventions should not be interchanged by index.', '',
              '## Collision, materials and verification', '',
              'Viewmodel meshes import without automatic collision. Keep runtime viewmodel components '
              'NoCollision. World props get a simple convex hull; only the vault frame has four '
              'authored UCX boxes preserving the opening. The hinged door gets a convex hull. '
              'Door and robot detail meshes are decorative, not independently simulated mechanisms.', '',
              'Material names exactly match the existing pipeline. UV0 is smart-unwrapped with margins; '
              'meshes use explicit triangles and flat face normals. Low-poly bevels, vent cages, '
              'trigger loops, muzzle bores, magazine lips, wear chips and hardware are geometry. '
              'No normal map baking, lightmap UV1, LOD chain or Nanite build is performed.', '',
              'Each FBX was checked for finite vertices and non-degenerate triangles; the manifest '
              'records boundary edges and optional Blender FBX reimport results (bounds, UVs, '
              'triangle counts, materials and four frame UCX meshes). Unreal validation is deferred '
              'to the main agent, and is built into the companion importer.', '']
    (OUT/'README.md').write_text('\n'.join(lines), encoding='utf-8')


def gallery():
    """Editable assembled source scene, plus two clear contact sheets for review."""
    scene = bpy.context.scene
    source = bpy.data.collections.new('SOURCE - local pivot meshes (hidden)')
    scene.collection.children.link(source)
    for o in OBJECTS.values():
        for col in list(o.users_collection):
            col.objects.unlink(o)
        source.objects.link(o)
    source.hide_render = True
    source.hide_viewport = True
    views = bpy.data.collections.new('REVIEW - assembled instances')
    scene.collection.children.link(views)
    def instance(name, offset, scale=1):
        original = OBJECTS[name]
        obj = bpy.data.objects.new(name+'_review', original.data)
        views.objects.link(obj)
        obj.location = offset
        obj.scale = (scale,)*3
        return obj
    def assembly(name, offset, scale=1):
        instance(name, offset, scale)
        def children(parent, base):
            for r in RECORDS.values():
                if r['parent'] != parent:
                    continue
                poses = [[v/100 for v in t] for t in r.get('instances_cm', [])] or [r['rest_location_m']]
                for pose in poses:
                    pos = Vector(base)+Vector(pose)*scale
                    instance(r['name'], pos, scale)
                    children(r['name'], pos)
        children(name, offset)
    def label(body, p, size=.045):
        curve = bpy.data.curves.new('Label', 'FONT')
        curve.body = body
        curve.size = size
        curve.extrude = 0
        ob = bpy.data.objects.new('Label '+body, curve)
        views.objects.link(ob)
        ob.location = p
        ob.rotation_euler = (math.pi/2, 0, 0)
        curve.materials.append(MATS['Bone'])
    for i, name in enumerate(['Revolver', 'SMG', 'Rifle', 'Sniper', 'LMG']):
        z = 2.02-i*.48
        assembly(name, (0, 0, z))
        label(name.upper(), (-.55, -.10, z+.20), .040)
    assembly('TraderBody', (1.63, 0, .41), .80)
    angle = math.radians(-55)
    for o in list(views.objects):
        if o.name.startswith('Trader'):
            origin = Vector((1.63, 0, .41))
            v = o.location-origin
            o.location = origin+Vector((v.x*math.cos(angle)-v.y*math.sin(angle),
                                       v.x*math.sin(angle)+v.y*math.cos(angle), v.z))
            o.rotation_euler.z = angle
    label('TRADER / MODULAR', (1.25, -.12, 1.72), .033)
    assembly('LootBag', (1.33, 0, -.03), .65)
    assembly('AmmoBox', (1.94, 0, -.03), 1.35)
    assembly('LeftHand', (1.39, 0, 2.02), 1.30)
    assembly('RightHand', (1.94, 0, 2.02), 1.30)
    label('WRIST-PIVOT GLOVES', (1.22, -.12, 2.24), .033)
    assembly('BunkerDoorFrame', (3.28, .02, -.03), .76)
    # Vault lies in YZ; rotate assembled gallery instances so face points toward review camera.
    for o in list(views.objects):
        if o.name.startswith('Bunker'):
            origin = Vector((3.28, .02, -.03))
            v = o.location-origin
            o.location = origin+Vector((v.y, -v.x, v.z))
            o.rotation_euler.z = -math.pi/2
    label('BUNKER / HINGE + SPINDLE', (2.77, -.20, 2.14), .031)
    # Large display surface behind the asset sheet.
    bpy.ops.mesh.primitive_plane_add(size=200, location=(0, 1.0, 0), rotation=(math.pi/2, 0, 0))
    plane = bpy.context.object
    plane.name = 'Review backdrop'
    bg = bpy.data.materials.new('Review_Backdrop')
    bg.diffuse_color = (.027, .032, .038, 1)
    bg.use_nodes = True
    bg.node_tree.nodes['Principled BSDF'].inputs['Base Color'].default_value = (.027, .032, .038, 1)
    bg.node_tree.nodes['Principled BSDF'].inputs['Roughness'].default_value = 1
    plane.data.materials.append(bg)
    scene.world.color = (.20, .20, .20)
    scene.render.engine = 'CYCLES'
    scene.cycles.samples = 24
    scene.cycles.use_denoising = True
    scene.render.resolution_x = 2160
    scene.render.resolution_y = 1450
    scene.render.resolution_percentage = 100
    scene.view_settings.view_transform = 'AgX'
    def light(name, p, energy, size):
        data = bpy.data.lights.new(name, 'AREA')
        data.energy, data.shape, data.size = energy, 'DISK', size
        o = bpy.data.objects.new(name, data)
        scene.collection.objects.link(o)
        o.location = p
        o.rotation_euler = (Vector((.7, 0, 1))-o.location).to_track_quat('-Z', 'Y').to_euler()
    light('Softbox key', (-1, -3, 4), 500, 4)
    light('Cool fill', (4, -2, 2), 350, 3)
    camera = bpy.data.cameras.new('Asset review camera')
    cam = bpy.data.objects.new('Asset review camera', camera)
    scene.collection.objects.link(cam)
    scene.camera = cam
    cam.location = (1.73, -8, 2.9)
    cam.rotation_euler = (Vector((1.73, 0, 1.12))-cam.location).to_track_quat('-Z', 'Y').to_euler()
    camera.type = 'ORTHO'
    camera.ortho_scale = 5.0
    # Packed textures keep the editable source scene portable.
    bpy.ops.file.pack_all()
    bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'LethalWorld_ModelsV2.blend'))
    return cam


def component_review(cam):
    scene = bpy.context.scene
    bpy.data.collections['REVIEW - assembled instances'].hide_render = True
    col = bpy.data.collections.new('REVIEW - detached components')
    scene.collection.children.link(col)
    entries = [('RevolverCylinder', 4), ('SniperBolt', 2.8), ('LMGCover', 2.5),
               ('LMGBelt', 3.8), ('SMGMag', 2.6), ('RifleMag', 2.6), ('SniperMag', 3.6),
               ('Cartridge', 8), ('LeftHand', 2.8), ('RightHand', 2.8),
               ('AmmoBox', 2.8), ('LootBag', 1.2)]
    for index, (name, scale) in enumerate(entries):
        center = Vector((index % 4 * 1.03, 0, 2.12-index//4*.84))
        b = RECORDS[name]['local_bounds_m']
        mid = (Vector(b['min'])+Vector(b['max']))/2
        obj = bpy.data.objects.new(name+'_detached', OBJECTS[name].data)
        col.objects.link(obj)
        if name == 'RevolverCylinder':
            obj.rotation_euler.z = -.75
        elif name == 'SniperBolt':
            obj.rotation_euler.x = math.pi
        obj.location = center-(obj.rotation_euler.to_matrix() @ mid)*scale
        obj.scale = (scale,)*3
        # Mark actual source origin, never bounding-box centre, for assembly review.
        bpy.ops.mesh.primitive_uv_sphere_add(segments=8, ring_count=4, radius=.014,
                                           location=obj.location+Vector((0, -.17, 0)))
        marker = bpy.context.object
        marker.data.materials.append(MATS['Glow'])
        marker.name = name+'_pivot_marker'
        curve = bpy.data.curves.new('Component caption', 'FONT')
        curve.body = name+'  /  '+str(scale)+'x'
        curve.size = .039
        text_obj = bpy.data.objects.new('Caption '+name, curve)
        col.objects.link(text_obj)
        text_obj.location = center+Vector((-.39, -.18, .36))
        text_obj.rotation_euler = (math.pi/2, 0, 0)
        curve.materials.append(MATS['Bone'])
    cam.location = (2.1, -8, 4.9)
    cam.rotation_euler = (Vector((1.56, 0, 1.28))-cam.location).to_track_quat('-Z', 'Y').to_euler()
    cam.data.ortho_scale = 4.40
    scene.render.filepath = str(OUT/'ModelsV2_Components.png')
    bpy.ops.render.render(write_still=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--skip-render', action='store_true')
    parser.add_argument('--skip-roundtrip', action='store_true')
    parser.add_argument('--fbx-only', action='store_true')
    args = parser.parse_args(sys.argv[sys.argv.index('--')+1:] if '--' in sys.argv else [])
    if args.fbx_only:
        # Fast integration repair: do not rebuild, reimport, render, resave, or rewrite metadata.
        manifest = json.loads((OUT/'models_v2_manifest.json').read_text(encoding='utf-8'))
        bpy.ops.wm.open_mainfile(filepath=str(OUT/'LethalWorld_ModelsV2.blend'))
        for material in PALETTE:
            existing = bpy.data.materials.get('M_'+material)
            if existing:
                MATS[material] = existing
        for record in manifest['assets']:
            name = record['name']
            obj = bpy.data.objects['SM_'+name]
            assert bounds(obj) == record['local_bounds_m'], (name, 'source/manifest bounds mismatch')
            assert obj.matrix_world == Matrix.Identity(4), (name, 'source pivot transform')
            OBJECTS[name] = obj
            RECORDS[name] = record
        export_all()
        print(f'V2_FBX_ONLY_COMPLETE {len(RECORDS)} assets; UE_Y_CORRECTED; source/manifest/previews unchanged', flush=True)
        return
    setup()
    revolver()
    sniper()
    smg()
    rifle()
    lmg()
    utilities()
    hand('LeftHand', 1)
    hand('RightHand', -1)
    trader()
    loot()
    vault()
    export_all()
    if not args.skip_roundtrip:
        roundtrip()
    documentation()
    cam = gallery()
    if not args.skip_render:
        bpy.context.scene.render.filepath = str(OUT/'ModelsV2_Review.png')
        bpy.ops.render.render(write_still=True)
        component_review(cam)
    print(f'V2_COMPLETE {len(RECORDS)} assets; {sum(r["triangles"] for r in RECORDS.values())} triangles', flush=True)


if __name__ == '__main__':
    main()
