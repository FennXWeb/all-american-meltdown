# Chapter-one locations and encounter overhaul — update 52

Implemented source changes:

- Nine authored locations replace the shared compound: Mile Nine diner/service yard, Mercy Crossing farm settlement, field medical campus, Cinder toll offices/impound, Relay Six communications station, Dry Creek freight depot, Kennels detention/infirmary complex, Ash Crown foundry, and Fort Resolute detention/command base.
- Sites span roughly 9.5 km north/south and 4.7 km east/west; the closest two are over 1 km apart. Existing procedural reservations, terrain flattening, discovery, and route generation use the relocated coordinates. The northernmost story site remains south of Canada's border.
- Dedicated rooms, corridors, working doors, breakable side windows, cover, storage, furniture, lighting, and signage. The foundry includes furnaces and a service mezzanine; the prison separates office, infirmary, cells and medical hold; the fortress separates processing, evidence, detention, barracks, mess and command.
- Six new original modular props: dossier, cell keys, medical case, switch panel, relay console, and improvised fastener. Blender sources and exports: ArtSource/ModelsV52. Generator: Tools/make_story52.py; Unreal importer: Tools/import_story52.py.
- The opening bunker radio remains a radio. Later objectives use evidence, machinery, keys and cell controls. Medical recovery has a wounded survivor, treatment, pharmacy access and supplies. Warden/dispatcher objectives require the designated target rather than exterminating every guard. Toll records can be taken without killing everyone. Foundry and command feeds must take damage; pressing Use does not destroy them.
- Evidence and rescue cinematics frame their actual new locations. Major plot beats, named characters, surrender/refusal branches, gear confiscation/recovery, fortress ownership and 30 stage IDs remain compatible.

Rescue combat and checkpoints:

- Ordinary story troops explicitly bypass random legendary rolls; bosses retain their legendary status and authored health.
- Enemies use separate room/yard posts with no initial alert; distant rooms do not all acquire the player at once. At most two story actors have an open firing slot at a time. Spawn positions avoid the player and other assigned posts.
- Story transitions and checkpoint restoration give four seconds of protection and freeze story attackers during the handover/cutscene. This expires automatically and does not remove the encounter.
- Released settlers equip weapons and join the breakout fight. Their initial positions use a detention aisle, away from bunks.
- Chapter checkpoint snapshots retain at least 75% maximum health. Recovery moves nearby players to the stage's protected approach/aisle and stops previous movement. Confiscated gear and objective rewards still use existing rollback/idempotence handling.
- Old saves migrate discovered story markers, nearby player position, fortress settlement and fortress respawn. The versioned migration selects the closest old site once, avoiding chained relocations. Existing stage progress and paid rewards are retained.

Validation status:

- First and second source compilations completed; DLL linking was blocked by the user's running Unreal Editor (Saved/Build52.log, Saved/Build52b.log). Latest source-only check: Saved/Compile52Final.log.
- Generated six FBXs and Blender source successfully (Saved/Models52.log).
- Extended LethalWorld.Story.Chapter1 automation and LWStory31Smoke to cover migration, medical prerequisites, destructible objectives, ordinary troop rarity, immediate rescue damage protection, armed settlers, checkpoint recovery, full chapter completion, and captures of all nine sites.
- Editor closed; final module compiled and linked successfully (Saved/Build52Complete.log and Saved/Build52Visual.log).
- All six props imported with validated bounds/materials (Saved/Import52.log).
- All three chapter automation tests passed (Saved/Tests52.log).
- Full initial gameplay run passed 151 checks, zero failures (Saved/Story52Smoke.log), including both ultimatum branches, gear escrow/recovery, fortress ownership, medical prerequisites, destruction-only feeds, old-save relocation, rescue handover at normal health, armed allies and death/checkpoint retry.
- Visual review prompted additional site-specific paving, roof finishes, entrance canopies, rooftop utilities, farm gables/chimneys and foundry stacks/raised roof structure. Revalidation of those geometry changes is in Saved/Story52FinalSmoke.log.
- No packaged build created.

Final validation: Saved/Story52FinalSmoke.log completed all 151 checks with zero failures after the exterior pass. Reviewed refreshed site captures and the Mara rescue camera in Saved/ScreenshotsV17. Editor build and asset import are complete; no packaging was performed.
