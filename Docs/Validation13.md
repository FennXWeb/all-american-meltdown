# POI pass validation

- Final Unreal Editor compilation succeeded: `Saved/Build13Verified.stdout.log`.
- All 19 FBX assets passed the importer checks for material assignments, axes, scale, and bounds (0.05 cm tolerance): `Saved/Content13Final.log`.
- Final focused runtime suite passed all 2,065 checks: `Saved/Regression13POI.log` (`LW_V9_DONE failures=0 checks=2065`). It exercised all 20 POI types, searchable/persistent furniture, depleted storage regeneration, legacy loot migration, roof slopes and solid gables, two ranch bedrooms, and the kitchen appliance set.
- The focused run used `-LWV9Smoke -LWPOIOnly -LWAudioSmoke` and the existing isolated automation save slot. Unrelated vehicle and weather steps were excluded from this final run.
- Inspected in-engine screenshots of the ranch exterior, kitchen, living room, bedroom, and diner. Final copies are in `Docs/ScreenshotsV13`.
- Original mesh source and reproducible Blender generator: `ArtSource/ModelsV13/LethalWorld_V13.blend`, `Tools/make_models_v13.py`. Models range from 8 to 1,840 triangles and use instanced decoration where applicable.

No packaged build was produced. Existing packaged executables remain unchanged; use the Unreal project/editor for the new content.
