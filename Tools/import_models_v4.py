"""Run in Unreal Editor Python after the existing material pipeline has completed.

Imports only ArtSource/ModelsV4/models_v4_manifest.json. No map changes, builds,
material creation, or v1 reimports. Existing same-named v2 assets may be updated.
The main agent owns executing this script; the Blender producer never launches UE.
"""
import json
from pathlib import Path

import unreal as u

ROOT = Path(u.Paths.project_dir()).resolve()
SOURCE = ROOT / 'ArtSource' / 'ModelsV4'
DESTINATION = '/Game/Art/Meshes'
LIB = u.EditorAssetLibrary
ASSET = u.AssetToolsHelpers.get_asset_tools()


def main():
    manifest = json.loads((SOURCE / 'models_v4_manifest.json').read_text(encoding='utf-8'))
    records = manifest['assets']
    # Resolve everything first to avoid a partial batch caused by absent source/materials.
    materials = {}
    for record in records:
        if not (SOURCE / record['fbx']).is_file():
            raise RuntimeError('Missing v2 FBX: ' + record['fbx'])
        for name in record['material_slots']:
            if name in materials:
                continue
            material = LIB.load_asset('/Game/Materials/' + name)
            if not material:
                raise RuntimeError('Existing material not found: /Game/Materials/' + name)
            materials[name] = material
    LIB.make_directory(DESTINATION)
    results = []
    errors = []
    for record in records:
        task = u.AssetImportTask()
        task.filename = str(SOURCE / record['fbx'])
        task.destination_path = DESTINATION
        task.destination_name = 'SM_' + record['name']
        task.automated = True
        task.replace_existing = True
        task.replace_existing_settings = True
        task.save = False
        opts = u.FbxImportUI()
        opts.import_mesh = True
        opts.import_materials = False
        opts.import_textures = False
        opts.import_as_skeletal = False
        opts.mesh_type_to_import = u.FBXImportType.FBXIT_STATIC_MESH
        opts.automated_import_should_detect_type = False
        data = opts.static_mesh_import_data
        data.combine_meshes = True
        data.auto_generate_collision = record['collision'] == 'convex'
        data.one_convex_hull_per_ucx = True
        data.generate_lightmap_u_vs = False
        data.import_uniform_scale = 1.0
        data.convert_scene = True
        data.convert_scene_unit = True
        data.force_front_x_axis = False
        data.transform_vertex_to_absolute = True
        data.bake_pivot_in_vertex = False
        data.normal_import_method = u.FBXNormalImportMethod.FBXNIM_IMPORT_NORMALS
        task.options = opts
        task.factory = u.FbxFactory()
        ASSET.import_asset_tasks([task])
        path = DESTINATION + '/SM_' + record['name']
        mesh = LIB.load_asset(path)
        if not mesh or not isinstance(mesh, u.StaticMesh):
            errors.append('Failed static mesh import: ' + path)
            continue
        names = []
        for i, slot in enumerate(mesh.get_editor_property('static_materials')):
            name = str(slot.get_editor_property('imported_material_slot_name'))
            names.append(name)
            if name not in materials:
                errors.append(path + ': unknown imported material slot ' + name)
            else:
                mesh.set_material(i, materials[name])
        if sorted(names) != sorted(record['material_slots']):
            errors.append(path + ': material slot set differs from manifest: ' + str(names))
        actual = mesh.get_bounding_box()
        expected = record['local_bounds_m']
        lo = [actual.min.x, actual.min.y, actual.min.z]
        hi = [actual.max.x, actual.max.y, actual.max.z]
        target = [v*100 for v in expected['min'] + expected['max']]
        error = max(abs(a-b) for a, b in zip(lo+hi, target))
        # 0.05 cm permits importer float rounding without accepting axis/scale mistakes.
        if error > .05:
            errors.append(f'{path}: pivot/axis/unit bounds mismatch, max error {error:.4f} cm; '
                          f'actual min={lo}, max={hi}, expected={target}. '
                          'Do not compensate with actor scale; review FBX conversion settings.')
        LIB.set_metadata_tag(mesh, 'LW_V4_Parent', record['parent'] or '')
        LIB.set_metadata_tag(mesh, 'LW_V4_RestLocationCm', json.dumps(record['rest_location_cm']))
        LIB.set_metadata_tag(mesh, 'LW_V4_Animation', record['animation'])
        LIB.set_metadata_tag(mesh, 'LW_V4_SocketsMetres', json.dumps(record['sockets_m']))
        if record.get('instances_cm'):
            LIB.set_metadata_tag(mesh, 'LW_V4_InstancesCm', json.dumps(record['instances_cm']))
        results.append({'mesh': mesh, 'path': path, 'max_bounds_error_cm': error})
        u.log(f'LW_V4_MESH {record["name"]}: bounds_cm={lo} .. {hi}; slots={names}')
    if errors:
        # Leave imported objects reviewable in memory, but don't deliberately save a failed batch.
        for error in errors:
            u.log_error(error)
        raise RuntimeError(f'V2 import validation failed ({len(errors)} errors); review Output Log.')
    for result in results:
        LIB.save_loaded_asset(result['mesh'])
    u.log(f'LW_MODELS_V4_IMPORT_COMPLETE: {len(results)} meshes; all bounds and materials validated. '
          'Use ArtSource/ModelsV4/README.md for exact component transforms. No actors were modified.')


if __name__ == '__main__':
    main()
