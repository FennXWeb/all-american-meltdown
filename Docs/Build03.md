# Lethal World 0.3 — Roadside Encounters

The five POIs now have purpose-specific interiors: a fuel shop and workshop, three private motel rooms off a corridor, clinic waiting and treatment areas, warehouse aisles and a dispatch office, and a diner with a separate kitchen. New beds, chairs, drawer cabinets and service benches are original modeled assets.

Use **E** to open or close doors and search vehicles. Glass blocks movement and gunfire until broken. Shooting a fuel pump triggers a damaging explosion; nearby pumps can chain. Walls block explosion damage. Open doors, destroyed windows, spent pumps and vehicle cargo persist through saves and chunk reloads. Use F5 to save before quitting.

Signs have original generated face textures with modeled metal borders, fasteners and stand-off brackets. The lettering sits on a dedicated UV-mapped face outside the facade. Gas forecourts are set back from the service street.

Three enemies join the zombie:

- **Raider:** masked rifleman, three-shot bursts, magazine pauses, pursuit and strafing.
- **Rabid dog:** low quadruped silhouette, fast pursuit and biting attacks.
- **Mannequin:** approaches only while outside the player's visible line of sight. It becomes hostile when attacked or within 2.2 metres with sight of the player. Watching it stops its movement; an already hostile mannequin can still strike within reach.

All three use connected physical ragdolls on death. Existing bunker protection, weapons, inventory, ammunition and traders remain available.

## Authoring

- Original Blender source, 24 FBXs and geometry metadata: `ArtSource/ModelsV3`.
- Sign textures and generation provenance: `ArtSource/SignsV3`.
- Six new synthesized placeholder audio slots: DoorHinge, GlassBreak, FuelExplosion, DogGrowl, RaiderVoice, MannequinMove. Replace their assignments in **Tools → Lethal World Audio Manager**; all 52 slots remain centrally managed.
- `Tools/make_models_v3.py`: reproducible Blender geometry generation.
- `Tools/make_audio_v3.py`: reproducible placeholder audio generation.
- `Tools/build_v3_content.py`: validated model import and material/audio setup.
- `-LWV3Smoke -LWAudioSmoke -RenderOffscreen`: isolated gameplay verification and POI/enemy screenshots. Its save slot is `LethalWorld_AutomationV3`.

This remains a prototype: furniture uses authored modular geometry, character motions are procedural, and the new sounds are synthesized placeholders. Enemy pathfinding uses the existing bounded local search rather than a full navigation mesh. Existing saves retain gear and world records, but remodeled interiors may change the space around a saved location.
