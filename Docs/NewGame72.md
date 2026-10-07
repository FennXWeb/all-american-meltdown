# New-game and campaign reset

New-game setup now offers difficulty only, followed directly by the survivor creator. The seed, terrain and density controls and replayable opening story sequence are removed. A new survivor starts at 06:00 on the street near central Syracuse, using the fixed Upstate New York layout and standard internal generation values.

The former Chapter 1 campaign no longer starts or appears in the contracts menu. Its reserved plots, connecting roads and artificial terrain flattening are removed. Generic side contracts, encounters, companion systems, character customization and the existing bunker remain available.

Legacy serialized campaign types remain readable. Loading a legacy save clears campaign progress and checkpoints, removes its discovered map pins and obsolete fortress respawn, and returns confiscated possessions and fortress shop consignments to inventory or stash. Any overflow becomes a loot bag at the restored player position. Players saved inside a retired campaign compound move to the Syracuse starting area. Independent progression and side quests are retained.

Validation: `LethalWorld.Update72` automation tests and `-LWV17Smoke -LWUI46Smoke -LWNewGame72Smoke` runtime smoke scenario. Builds use the Editor target only; no packaged build is produced.
