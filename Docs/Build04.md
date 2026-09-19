# Lethal World 0.4 — Survivor Network

This native Unreal update adds persistent character progression, settlement contracts, friendly survivors, recruitable companions, and an expanded underground home. It retains the weapons, physical ammunition, survival, loot, roads, traders, destructible props and hostile encounters from 0.3.

## Getting started

Deploy, open **Tab**, and find a green **WAYSTATION** marker. Click its location to draw a road route. **E** talks to residents. Wardens offer contracts, merchants open the trading inventory, medics provide treatment, and Ash/Rook can join your crew. The initial hire fee is 200 credits; you start with 150 and can earn more from combat, trading and contracts.

| Input | Action |
|---|---|
| K | Attributes and perks |
| J | Contract journal and objective tracking |
| O | Crew assignments |
| E | Talk, interact, or close the current panel |
| Escape | Close panel, otherwise pause |
| B | Route to Shelter 01 |
| F5 / F9 | Save / load survivor record |

## Progression

Seven attributes—Strength, Awareness, Endurance, Presence, Intellect, Agility and Fortune—each contain seven perks, for **49 unlocks**. Most perks have three ranks. Spend one point to raise an attribute or purchase one perk rank. Every level grants **two skill points**; Lifelong Learner grants one extra point on subsequent levels. Attributes begin at 1 and cap at 10. Perks require an attribute score and survivor level; later ranks require three additional levels each. Survivor level caps at 100.

Earn XP from defeated hostiles, newly discovered roadside parcels, first use of a radio, recruiting, successful Presence dialogue, and completing contracts. Effects cover weapon specializations, critical hits, health, stamina, survival drain, movement, reloads, recoil, loot quality, purchase prices, repair costs, companion capacity, and rewards. Loot improvements affect newly generated containers rather than rerolling existing stock.

## Contracts and conversations

Twelve authored contracts form prerequisite chains. Six may be active at once. Objectives include visits, kills, delivering supplies, using furniture, talking to a specific resident role, and recruitment. Only the current stage advances; the journal shows counts, completion and payment status. Claim rewards at a **warden**. Supply hand-ins consume the exact required quantity from carried inventory; failed hand-ins leave supplies and credits untouched. Paid contracts cannot award rewards again.

The journal's Track button routes to known visit destinations or a waystation for hand-in. Combat and supply collection have no fixed target location. Roads can be followed using the amber path already drawn in the world.

Residents play positional synthesized gibberish through the existing occlusion-aware audio system. Passing lines display above their heads; full conversations have selectable branches, rumors, role-specific services and a Presence-gated exchange. The three voice recordings are original placeholders that can be replaced in the Audio Slots editor.

## Companions and home

One companion follows by default. **Squad Leader** has two ranks, adding one follower each; **Commander** adds another, allowing four with all three capacity unlocks. Up to ten survivors can be recruited. A recruit beyond your active limit goes to their assigned bunker bedroom. Use **O** or a recruited survivor's dialogue to send them home or bring them along.

Followers navigate around obstacles, catch up when separated, fire at visible hostiles, and accompany bunker transitions. They are recoverable allies: lethal damage downs them temporarily instead of deleting the recruit. Rally speeds recovery. Their identities, bedroom assignments and following status persist across saves and world streaming. Returning home uses a transition to the protected underground area.

Shelter 01 now has ten individually numbered bedrooms, private storage lockers, connecting halls, a workshop, medical room, mess hall and communications room. The whole interior is protected from death. The original shared stash remains available.

| Furniture | Interaction |
|---|---|
| Bed | Restores health and stamina, consumes a little hunger/thirst |
| Chair | Recovers stamina |
| Locker | Opens persistent grid storage |
| Water station / sink | Restores thirst; 60-second filter recharge |
| Cooker | Consumes one ration to refill hunger |
| Workbench | Repairs equipped damaged armor using scrap |
| Radio | Civil-band message, one-time XP, contract objectives |

POIs now include additional service rooms, furnished private motel rooms, washrooms, and these usable furnishings. Doorway spans and headers fit the 160 × 236 cm leaves, with thresholds in bunker openings.

## Authoring

Open `Content/Data/DA_RPGCatalog` in Unreal to edit perk identities, categories, rank gates, effects and numerical values, as well as quest prerequisites, ordered objectives and rewards. Keep identities stable for saved characters. Native defaults live in `LWRPG.cpp`. Dialogue branches and role actions live in `LWPlayerRPG.cpp`; companion behavior lives in `LWResident.cpp`. This build's branching dialogue content is authored in C++, rather than a visual dialogue-graph editor.

Seven original furniture models are generated by `Tools/make_models_v4.py`; sources, FBXs and the Blender scene are under `ArtSource/ModelsV4`. Three synthesized speech WAVs are generated by `Tools/make_audio_v4.py`. `Tools/build_v4_content.py` imports these assets and creates the RPG data asset without overwriting existing catalog edits. The audio catalog now has 55 default slots.

Use `Tools/Build.ps1 -Content -Package` to rebuild and package. `Tools/Verify.ps1` runs native automation and all three gameplay suites. The V4 gameplay harness uses `-LWV4Smoke` and an isolated `LethalWorld_AutomationV4` save; it never replaces your survivor save.

This is a playable prototype. Residents use modular stylized human models, conversations have a compact authored set of branches, and speech remains synthesized gibberish. It does not include multiplayer, companion equipment management, a visual dialogue editor, or unrestricted town building.
