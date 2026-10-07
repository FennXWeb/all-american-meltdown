# Story production assets

Editable source: `Campaign77.blend`. Twenty-two meshes are exported as centimetre-correct FBX files with individually fitted pivots, material slots and UVs. The importer checks measured bounds, installs three LODs, and assigns complex query collision to the walking surfaces. Contact props and loose fragments have no blocking collision.

Rebuild using Blender 4.2 with `Tools/build_campaign77.py`; render the actual meshes with `Tools/render_campaign77.py`. Run `Tools/import_campaign77.py` through Unreal Editor Python to import into `/Game/Art/Campaign77`. These scripts do not package the project.

`Carrier77_Exterior.png`, `Carrier77_Office.png` and `Ferry77_Review.png` are neutral-light renders of the mesh source, not concept art. Runtime includes additional office furnishings, glazing, interaction props, lighting and animation.

## Materials and image provenance

The color atlas was generated with the imagegen skill, in **generate** mode, without an input reference. The normal and packed ambient-occlusion/roughness/metallic maps are independently authored with `Tools/campaign77_surface_maps.py`. The normal map describes surface grain; the fitted model supplies the major forms. UVs use inset tile borders to prevent adjacent material colors bleeding across the edges.

Actual output dimensions are 1254 × 1254 for the generated color atlas and 1024 × 1024 for each authored surface map. The prompt requested 2048 × 2048; the returned image was imported at its native resolution.

Generation prompt:

> Create a production game texture atlas, square 2048x2048, exactly 4 by 4 grid of sixteen equal 512x512 material swatches touching edge to edge. Orthographic flat material scans, diffuse albedo ONLY, evenly lit, no perspective, no rendered objects, no cast shadows, no text, no gaps, no borders. Restrained believable late-2000s realistic game art for a fictional US presidential armored command vehicle, small passenger ferry, canal mechanisms and Canadian relief facility. Each tile is uniform uninterrupted material, subtle wear and fine physical grain, no large fixtures. Row 1 left to right: dark desaturated olive painted steel with fine scuffing; charcoal rubber with fine ridges; dark graphite satin steel; off-white painted aluminum with fine scratches. Row 2: rich polished dark walnut with fine grain; muted midnight blue woven wool upholstery; warm cream leather fine pores; reddish copper/brass brushed metal. Row 3: faded medium teal boat paint fine salt wear; tan natural tightly woven rope fibers; dull galvanized zinc mottling; dark navy blue safety mesh fine regular pattern. Row 4: pale warm plaster; gray speckled terrazzo; aged brown oiled timber; charcoal soot and heat blistered steel. Fine detailed flat surfaces suitable for UV mapping, no obvious stylized outlines, not a collage of object photos.

No external model downloads, paid audio generation, or third-party character assets were introduced in this production pass.
