# Update 78 art sources

- T_Menu78.png: generated hand-painted roadside dusk menu backdrop (1672 x 941).
- T_Signs78.png: generated 4 x 4 enamel sign atlas (1254 x 1254), with 16 category designs.
- SignPanel78.fbx: authored thin framed sign face, 100 x 25 cm; UV front normal +X.
- TrailTent78.fbx: authored sewn ridge tent with shaped cloth, rolled entry flaps, pole sleeves, guy ropes and stakes.
- World78.blend: editable mesh sources.

Generation originals retained at C:/Users/prett/.codex/generated_images/01a06eee-a2fa-7361-854b-fae8dcb24eb3/exec-6edce526-4fa4-424f-bf5b-1caf72b547bd.png (signs) and exec-7bb12d3d-bc27-44b1-8d5f-6103b2adc09b.png (menu).

Build meshes with Tools/build_world78.py in Blender 4.2. Import with Tools/import_world78.py in Unreal Editor. These scripts do not package or cook the game.

World signs use the generated category artwork plus the actual location/room inscription painted into a cached 512 x 128 texture, on a physically mounted panel. Unsupported floating labels are suppressed. Dynamic slot-machine reels and vehicle instruments retain their functional displays.
