# Story production pass — September 24, 2026

## Implemented

- **Performances:** a production component coordinates precise approaches, world-space wrist targets, two-link elbow solving, kneeling treatment, restraint/release, case handoffs, repair, bracing, cranking and writing. Personal washing remains a player action rather than summoning a helper to the sink. Existing character meshes and facial animation remain in use. Physical cases, restraints, tools and dressing rolls have measured pivots. Save state carries the active work and performance cursor. Completed fixed appliances remain visible.
- **Ferry:** a fitted passenger vessel, benches, wheelhouse, gangway, navigation lights and engine loop. The route follows actual water for 105 seconds; the player walks within the moving deck frame. Living named passengers accompany the crossing, customs removes weapons before boarding, and saved progress reconstructs the boat. The Ontario landing still uses Northbank's onward Toronto transfer.
- **Carrier:** a bespoke four-axle hull, hinged armor, shutters, roof, turret and torn panels. The office includes a new executive desk/chair, draped flag, lighting, records, storage and seating. A 12-second sequence releases the armor, drops the case, moves the president with an escort, collapses the turret and settles the roof. Three staged flashes/blasts supplement the authored mesh motion. Explosion plumes now retain fixed topology instead of rebuilding render buffers every frame.
- **Facilities:** Rome's fitted gates, gear and winch animate when power is restored; motion progress survives stage changes. Meridian has connected residential, lounge, clinic, school, dining and storage wings, including eight furnished classrooms. The transfer ward, ferry landing and Canadian reception/tracing office have dedicated layouts, signs, lighting and work areas.
- **Crowds:** ten individually named civilians use staggered departures, room routes, door approaches and exit queues. The mission waits for the surviving group. Deaths and route cursors persist; a group with no survivors cannot softlock the exit objective.

## Asset delivery

Twenty-two new meshes, 55,860 source triangles in total, three LODs, UV-mapped color, normal and packed surface maps. Editable Blender, FBX, generation/import scripts and preview renders are included. See `ArtSource/Campaign77/README.md` for provenance and rebuild instructions. This is an authored runtime mesh/animation system, not imported motion capture or a Sequencer film.

## Validation record

- `Saved/AutomationCampaign77b.log`: four native tests passed — production persistence/contact math/water path, campaign branches, corridor geography, Canadian country/streaming.
- First rendered run caught a crowded exit bottleneck and a renderer crash during overlapping explosion effects. The next run completed the destruction and full sailing sequence without that crash. It exposed two classroom door obstructions; the boards were moved away from the openings.
- A subsequent measured handoff recorded a maximum wrist-to-target error of **1.70 cm over 141 samples**. The bedside test exposed a medic circling stationary defenders. The assault now stages Mara at the bedside and the defenders at their defensive positions, with the patient aligned to the mattress.
- `Saved/BuildCampaign77Final.log`: Unreal 5.8 Development Editor build succeeded.
- `Saved/Campaign77NativeFinal.log`: all **four native tests passed** on the final build � production continuity, branch continuity, corridor geography and Canadian country/streaming.
- Final rendered runs total **107 passing gameplay checks, zero failures** (31 production + 72 existing campaign + 4 corrected ward evacuation). Original game settings were restored with matching SHA256; all test editors exited. **No packaged build was produced.**
- `Saved/Campaign77BranchesFinal.log`: **72 campaign regression checks passed, zero failures**, across 20 phases (86.2 seconds). Covers real casualty branches, one-time effects, checkpoint/restart/cancel, crossing reconsideration, weapons escrow/return, washing and four-hour rest, ally boundaries/execution, the final opponent and ending cleanup.
- `Saved/Campaign77ProductionFinal.log`: **31 gameplay checks passed, zero failures**, across 13 rendered phases (329.9 seconds). This includes bedside treatment, repair, restraint seating, handoff, canal motion, ten-person school evacuation, custom Carrier/damage/debris, customs, walking on the ferry, reconstructing it mid-voyage and completing arrival. The final handoff measurement was **1.51 cm maximum wrist error over 158 samples**.
- `Saved/Campaign77LaborFinal.log`: four checks passed with zero failures. The ward evacuation resumes from saved route cursors, counts a casualty and waits until all nine survivors reach the receiving point.
- Both Meridian crowd sequences also passed reconstruction and arrival in `Saved/SmokeCampaign77Crowds.log`; that intermediate run exposed the ward issue corrected in the focused final test. Intermediate failure logs are retained as diagnostic evidence.

## Playtesting boundaries

Rendered tests exercise real actor ticks, collision, door navigation, animation, combat actors, streaming, customs, checkpoints and moving-deck travel. They use isolated automation saves. They include teleportation between scenarios and an invulnerable test player; their duration must not be represented as an uninterrupted human campaign playthrough or evidence that combat balance is finished.

Before declaring overall pacing/balance final, human play sessions should cover:

1. Bellwether without preparation, with preparation, and with Lena lost; note whether rescue, combat and records protection remain understandable.
2. The complete Syracuse–Rome–Tug Hill–Watertown journey on foot and by vehicle, including stops for supplies. Judge travel length and fatigue rather than using only straight-line distance.
3. Each of the three Canadian routes, including abandoning and reselecting a route, late boarding, ferry save/reload, and returning for escrowed equipment.
4. Resistance and collaboration routes, with key allies alive/dead and Mara remaining in Canada; play combat at normal difficulty rather than using the test player's invulnerability.
5. Meridian and labor evacuations while the player blocks a doorway, enemies remain active, and civilians are wounded or lost; verify the final tally and save/resume behavior.
6. Carrier assault from multiple approaches, with defenses powered/disabled and different ally support. Confirm readable telegraphs, usable cover, office access and a fair final encounter.

This outstanding human evaluation is explicitly retained; it has not been silently replaced with automated assertions.

## Re-running the checks

Build the `LethalWorldEditor` target, then launch `UnrealEditor.exe` with the project path and one of these argument sets. These run against `LethalWorld_AutomationV2`, not the player save. Preserve and restore `Saved/Config/WindowsEditor/GameUserSettings.ini` when running the gameplay harness.

- Production scenes: `-game -unattended -windowed -ResX=1600 -ResY=900 -LWV17Smoke -LWUI46Smoke -LWCampaign77Smoke`
- Focused repair/treatment: add `-LWCampaign77Treatment` to the production arguments.
- Three evacuation routes, reconstruction and a casualty: add `-LWCampaign77Crowds` to the production arguments.
- Existing campaign branches/recovery/customs/endings: replace `-LWCampaign77Smoke` with `-LWCampaign76Smoke`.
- Native tests: `-unattended -nullrhi -nosound -ExecCmds="Automation RunTests LethalWorld.Update77+LethalWorld.Update76+LethalWorld.Update68.CountryAndStreaming" -TestExit="Automation Test Queue Empty"`

Use a separate `-abslog=` path for each run. Run them sequentially so the automation slot and settings are not written concurrently. No cooking or packaging is involved.
