# Movement, combat, hostile behavior and quick looting — update 54

## Controls
- Sprint + crouch (Shift + Ctrl by default): slide. Costs stamina, decelerates, then returns to normal movement; holding Ctrl keeps a crouch.
- P: toggle prone. Move normally to crawl. Standing requires clear headroom.
- Space: jump, vault a low obstacle, or climb a reachable ledge. Ledges up to 2.2 m require clear overhead space, a walkable surface and room for the full capsule. This is ledge climbing, not unrestricted wall climbing.
- Aim at an unlocked container: a compact list appears without pausing. Wheel selects, E takes the selected stack, Y takes all that fits, L opens the transfer inventory.
- The full container inventory also has a Take All button. Traders are excluded. Locks, storage capacity, loaded magazine ownership, and settlement theft penalties remain enforced.
- New controls appear in key binding settings. The quick-loot prompts use the current bindings.

## Systems
- Sliding, low collision stance, smooth crouch/prone eye-height changes, buffered landing jumps, short jump grace, swept vault/climb movement, landing feedback and traversal hand presentation.
- Faster grounded acceleration/braking, softened recoil impulses, movement/stance-dependent accuracy, sustained-fire spread and recovery, strafe/turn weapon sway, sprint poses and wall proximity lowering. Existing magazine, chamber and reload transactions remain authoritative.
- Hostiles search their last perceived contact instead of receiving a live hidden player location. Raiders react to nearby rounds, seek cover under pressure or while reloading, peek, retreat at close range and change approach sides. Creature approach lanes and committed, variable melee windups create openings to dodge.
- Hostile obstacle routes sample floor height and clearance and permit stairs and usable doors. Incremental searches share a frame budget, cap nodes and retain partial routes. They do not teleport enemies or guarantee a path through physically unreachable geometry.
- Giants use distance-driven, wide strides with ground-contact audio and distance-faded camera rumble. Deathclaws blend to a quadrupedal charging pose. Other creatures and humanoids gain movement-driven footfalls. Injuries, mannequin observation rules, ragdolls and giant car attacks remain authoritative.
- Eleven editable action/creature audio events are added non-destructively to the audio catalog. They support track arrays and initially reuse existing sounds with event-specific pitch/volume; no new ElevenLabs generation was performed.

## Validation
- Unreal Editor Development build succeeded (`Saved/Build54Arrival.log`). No packaged build was produced.
- All 46 live gameplay checks passed (`Saved/Gameplay54Final.log`): stance/headroom, slide recovery, low vault/high ledge and blocked traversal, quick transfer/locks/capacity/magazine ownership, combat reactions, actual hostile movement around an obstacle, deathclaw charge pose, titan foot contact, and prone save serialization/restoration.
- Nine automation tests passed (`Saved/Automation54.log`, `Saved/AudioSave54Tests.log`), covering inventory transfer, magazine/chamber/revolver/shotgun behavior, weapon ownership, sight aperture, audio track arrays and detached save snapshots.
- All 20 menu/dialogue firing regression checks passed (`Saved/UIClick54.log`), including held clicks, input-mode transitions and fresh gameplay presses after release.
- Quick-loot and deathclaw screenshots were inspected. These checks are targeted regression tests; they do not replace extended playtesting of combat balance or traversal throughout the procedural world.
