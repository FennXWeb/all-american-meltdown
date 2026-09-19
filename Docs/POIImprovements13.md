# POI improvement pass

The pitched house roof slopes were inverted. Both panels now slope down from the ridge to the eaves. Solid triangular gables replace the stepped stacks of wall pieces. Ridge caps and eave trim close the silhouette.

The furnishing pass adds 19 original model assets: upholstered sofa, coffee table, dining table, sideboard, nightstand, bookcase, residential bed, table setting, desk clutter, lamp, radiator, pantry stock, waste bin, ceiling light, fuse box, gable, curtains, framed landscape, and open bathtub. Source meshes, FBX exports, and the Blender project live in `ArtSource/ModelsV13`.

Ranch and cottage buildings now have smaller residential footprints within their existing road-connected plots, with an entrance path across the setback. Ranch/cottage layouts now separate living, dining/kitchen, two bedrooms, study, and bathroom functions. Study and bathroom have their own hallway doors. Furniture is grouped around rugs, coffee tables, bedside storage, and work surfaces. Individual framed windows and curtains replace the residential storefront facade; houses use small address plaques rather than large shop signs.

Apartments/motels gain residential beds and bedside detail. Diners have more booth groups and table settings. Retail stock uses actual shelf heights, with denser aisle modules. Furniture-store displays alternate between living, dining, and bedroom groups; police offices use work desks rather than dining tables. Staff rooms gain storage and service details.

Core storage cabinets are no longer randomly omitted. Furniture placement reserves central circulation and door clearance, checks mesh bounds, and only adds tabletop props after the supporting furniture is successfully placed. New sideboards, bookcases, and nightstands are searchable; decorative pieces do not block movement.

Import with `Tools/build_v13_content.py` in Unreal Python. The importer verifies units, axes, bounds, and material slots. No packaged build is produced.
