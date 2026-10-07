# Dynamic acoustics

All positional audio catalog events and spatial NPC dialogue now enter `ULWAcoustics64`. Catalog music and 2D UI/ambience retain their existing routing. Local weapon reports keep consistent dry volume, with room reflections added separately. Slot tracks, pitch, volume, engine RPM mixing, and sound assets are unchanged.

## Response

- Existing normal/loud/custom distance attenuation remains in force, including distance-dependent high-frequency absorption.
- Three visibility paths sample direct and side transmission. Partial cover transmits more sound than a fully blocked path. A reverse trace estimates barrier depth, so thick/multiple barriers suppress more than a thin partition.
- Blocked sound is both quieter and low-pass filtered (approximately 650–1,900 Hz at full obstruction). Filter transitions and gain are interpolated rather than switched abruptly. Changes to doors and breakable geometry are picked up by subsequent probes.
- Enclosed vehicles suppress exterior sound; an open motorhome door reduces the suppression. Motorcycles remain open-air. Cabin controls and dialogue from a companion riding in the same car stay clear. Existing cabin engine tone is preserved.
- Ten listener probes estimate enclosure and room dimensions every half-second. Open areas/awnings, cabins, small rooms, medium rooms, halls and large interiors get distinct responses. Reverb decay ranges from 0.18 seconds in a cabin to 2.8 seconds in a large space. Two matching measurements stabilize room transitions; presets blend over 0.8 seconds.
- Wind now also muffles in enclosed cars, matching the existing rain treatment.

This is collision-based perceptual acoustics, not a full wave/portal simulation: it does not calculate long sound paths around multiple corners or classify every construction material. Decorative furniture can affect room estimates. Authored collision must accurately represent openings.

## Runtime cost and authoring

The subsystem caps its own collision queries at 48 per frame, probes up to six sources per 50 ms round-robin update, and tracks at most 256 components. It holds weak source references and does not scan the world for audio components or load sound assets while updating. Sources over the tracking limit retain native Unreal attenuation. When the initial probe budget is exhausted, cabin processing still applies while native obstruction covers the pending source.

Each audio catalog slot has an **Environmental Processing** checkbox, enabled by default. Disable it for an effect with independently authored spatial processing. Music and UI do not require opting out. User/caller volume controls remain independent of environmental gain.

## Validation

- `LethalWorld.Audio64.TransmissionAndRooms`: transmission, cabin separation, partial cover and room classification.
- `LethalWorld.Audio64.LiveGeometry`: real physics geometry, opening an obstruction and ignoring an emitter.
- `LethalWorld.Audio.AllEventTracks`: preserves shared event playlist behavior.
- `-LWV17Smoke -LWUI46Smoke -LWAudio64Smoke`: in-game playback through a wall, opening it, room reverb, local gun-report routing and cabin entry/exit, using isolated test saves.

No packaged build is produced.

## Verified September 20, 2026

Development Editor compilation succeeded (Saved/BuildAudio64Final.log). All three targeted automation tests passed (Saved/Audio64TestsFinal.log). The in-game playback scenario passed all 11 checks with zero failures (Saved/Audio64SmokeFinal.log). Original editor user settings were restored and hash-verified after the automated editors exited. No packaging was performed.
