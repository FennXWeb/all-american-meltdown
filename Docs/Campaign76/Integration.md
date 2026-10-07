# Campaign integration status

## Starting and continuing

Newly created survivors begin at Bellwether after the existing identity and starting-attribute screens. Existing saves remain opt-in: open **Contracts → Campaign → Start at Bellwether**. Starting retains the survivor and their equipment, but relocates them to the plaza.

The Campaign page tracks the current objective and recent known facts. **Resume Campaign** restores an interrupted conversation when nearby, or routes the player back when elsewhere. Dialogue pauses retain the current line. Continue never chooses a response. Normal manual save slots, autosave, and the existing death screen handle persistence and mission recovery.

## Written and connected

- 84 playable stages across 46 mission identifiers, including mutually exclusive routes and ending stages.
- 154 connected conversations, 460 conditional dialogue/staging beats, and 25 named character records.
- Bellwether through both endgame paths, three alternative Canadian entry routes, voluntary return, seven conditional global endings, and individual character/settlement accounts.
- Fuel allocation, repairs, physical defense, treatment, documents, negotiation, route preparation, moving rescue groups, real customs inventory escrow, optional safe rest, collaboration/defection, actual named-character deaths, and the Carrier's external defense/shutter encounter.
- Persistent choices, one-time rewards, health/deaths, defeated encounters, scene cursors, evidence, resources, faction leadership and checkpoint snapshots.

Full executable text and stage dependencies are readable in [Script.md](Script.md). [Continuity.md](Continuity.md) records the timeline, limits of character knowledge, relationships, branch dependencies and unresolved threads. Both are author-only; the player journal receives only encountered facts.

## Environments and presentation

The approved limited story corridor adds Bellwether, Rome, Guard/holding facilities, Tug Hill, a Freehold, the Weller cabin, Watertown, ferry/crossing approaches, a labor facility, Meridian and its transport hall. Existing POI assets supply these fictional facilities. Dry-land approach roads connect the stops around the existing lake shape. The regional layout is compressed, not a surveyed geographical recreation.

The clinic uses an accessible existing Destiny USA storefront. Northbank has a reserved reception and tracing office in Toronto, with private interview spaces, rest bays, washing facilities and workstations. Rome now has a dedicated workshop and animated canal machinery. Meridian has a continuous concourse joining residential, school, care, lounge, dining and storage wings. The labor transfer ward and ferry landing have dedicated layouts. Other corridor stops continue using existing POI assets.

The camera remains first person. A production component supplies measured arm contact targets, kneeling treatment, repair/brace/crank/write/wash actions, physical case transfers and seated restraint performances. These run on the current deformable NPC model system. The Carrier has a new mesh set, office dressing, tracking defenses, staged shutter/armor releases, bent debris, light bursts and a cast evacuation. It remains an encounter object rather than a drivable vehicle.

The Night Ferry now traverses a water channel for 105 seconds. The player can walk on the moving deck; passengers and the operator remain attached to the vessel. Weapons processing happens before departure. Journey state reconstructs from saves, and the Ontario landing connects to the existing Toronto receiving transfer.

## Canon conflicts resolved

- The campaign takes place in autumn 2030 after September 2028 attacks. The older opening's incompatible chronology/worldwide-collapse wording was adjusted.
- The retired Liberty's Heroes story remains retired. This campaign has separate state and does not reactivate its objectives or confiscated-gear logic.
- Bellwether replaces the previously requested generic Syracuse/bunker start for newly created campaign survivors, as required by the supplied canon. Existing survivor saves require explicit in-game opt-in.
- Humanitarian entry is a new narrative route through the existing Canadian customs system. The sandbox's paid passport remains available, but the story does not require a million-credit purchase.
- Existing game systems, sandbox factions/creatures, Canadian currency, landmarks, menus, models and general map remain in place. The new plot introduces no Fallout factions or lore.

## Production verification

The requested production systems are implemented; their runtime verification is recorded in [Production77.md](Production77.md). Generated mesh/material sources and provenance are in `ArtSource/Campaign77/README.md`.

Bespoke acted recordings were not part of this production pass. Missing exact lines remain subtitled. No paid ElevenLabs generation was used.

Automated and rendered gameplay tests are **not a complete human playthrough**. Subjective travel pacing and long-term combat balance across all choices still require play sessions. The validation notes distinguish tests actually run from that remaining evaluation.

## Validation

Original integration verification, September 23, 2026 (local time). The September 24 production build and regression results are in [Production77.md](Production77.md):

- Unreal 5.8 Development Editor target: **Succeeded**. Log: `Saved/BuildCampaign76.log`.
- Native automation: **3 passed** — campaign branch continuity, story-corridor geography, and the existing Canadian country/streaming regression. Log: `Saved/AutomationCampaign76.log`.
- Isolated unpackaged gameplay: **71 checks passed across 19 phases**, zero failures. Covered dialogue attack gating and interruption; fuel choices; actual Lena death and continuation; Syracuse clinic/cooperation; one-time effects; checkpoint/restart/cancel; witness death; route reconsideration through nested ferry choices; real customs escrow/return; physical Toronto receiving scene; four-hour bed rest; Mara's Canada boundary; actual former-ally execution; Carrier defenses/office opening; human Richardson death; ending/checkpoint/weather cleanup. Log: `Saved/SmokeCampaign76.log`.
- Rendered scene captures reviewed in `Saved/ScreenshotsV17/Campaign76_*.png`. Review caught and corrected duplicate Canadian default furnishing and an occluded Carrier office.
- Original user settings restored from the pre-test backup, with matching SHA256. Test processes exited. No packaged build was produced.

The gameplay harness drives selected interactions and transitions; this is not a complete manual playthrough of all 111 dialogue choices or every travel segment.

Authoring pipeline: `python Tools/campaign76.py --author`, then `python Tools/export_campaign76.py`. The compiler rejects duplicate IDs, missing references, unknown speakers and unreachable stages/scenes. Runtime data is compiled into native tables; it does not parse campaign JSON during play.
