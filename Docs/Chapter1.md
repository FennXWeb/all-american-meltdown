# Chapter 1: The Gates Stay Open

An authored campaign inside the procedural world, beginning after character creation. The story follows Mara Vale, a road medic, and the people of Mercy Crossing. The Cinder Choir treats survivors as assets; Liberty's Heroes turns that trade into a claim to citizenship and power.

## Starting and playing

New games begin the chapter after character creation. On an existing save, open Contracts and choose BEGIN CHAPTER 1. The chapter journal tracks the current objective and can place its waypoint. Use E on highlighted story objects or named characters. Dialogue choices and SKIP SCENE are clickable; movement, weapon attacks and inventory shortcuts are blocked while a cinematic or story conversation owns input.

Fort Resolute becomes a permanent governed settlement after signing the charter. Jonah manages it; Elsie trades, Tomas provides medical services, and Mara and Inez can join the crew.

## Cast
- Mara Vale: road medic, survival guide and hostage; she insists the rescue include the other prisoners.
- Jonah Reed: Mercy Crossing's organizer, a practical believer in shared work and elected authority.
- Inez Soto: former transfer driver who supplies evidence and returns to help the settlers.
- Silas Rusk: leader of the Cinder Choir, running prisoner transfers from Ash Crown Foundry.
- Captain Adrienne Voss: Liberty's Heroes officer who executes Rusk and demands disarmament.
- Director Elias Mercer: leader of Liberty's Heroes, ruling Fort Resolute through forced labor and personal authority.

## Mission sequence
1. The Morning After: wake in bed, sweep the radio, leave the bunker.
2. A Voice on the Road: meet Mara, pack emergency supplies, clear the rest stop.
3. A Place at the Table: travel to Mercy Crossing and meet Jonah.
4. The Price of Shelter: defend the settlement; the raiders abduct Mara and burn it.
5. Names in the Ash: recover Jonah's note, secure the field clinic's trauma supplies.
6. Paper Trail: recover the Cinder Tollhouse transfer ledger.
7. Dead Air: restore both parts of Relay Six to contact Inez.
8. A Witness Worth Saving: meet Inez and recover the guarded transfer manifest.
9. The Kennels: clear detention, recover access, rescue Mara.
10. No One Left Behind: learn who is buying the prisoners and plan the assault.
11. The Ash Crown: disable two foundry defense relays and defeat Rusk.
12. Liberty's Welcome: a real-time intervention ends Rusk's reign; choose surrender or resistance.
13. Model Citizens: captivity, a loose bunk fastener, and a shorted cell lock.
14. Property of the State: recover confiscated gear and release the settlers.
15. The People Outside: fight through the muster yard alongside the survivors.
16. A Republic of One: disable Mercer's armor relays and defeat him and his reinforcements.
17. The Gates Stay Open: witness the surrender, accept the settlers' vote and inherit Fort Resolute.

Surrender deliberately leads to an execution ending with a retry at the ultimatum. Resistance leads to capture and the playable escape. Cutscenes are camera moves and actors in the live Unreal world with subtitles and placeholder gibberish speech; scenes can be skipped while preserving their required outcomes.

## Persistence and compatibility
Chapter checkpoints, evidence flags, awarded objectives, boss reinforcement state and confiscated item instances live inside the existing RPG save. Existing save versions remain readable. Item GUIDs, ammunition, attachments, rarity and equipment slots are retained during confiscation. The fortress uses the existing settlement economy, leadership, companion stationing and respawn systems.

Nine permanent story reservations prevent normal POIs and terrain from occupying the authored compounds. These are fixed locations across seeds so the campaign has stable geography. General exploration remains procedural.

## Art and presentation

Four original faction/campaign banners are imported from `ContentSource/Chapter31/T_Chapter31.png`, with four atlas materials in `/Game/Materials/M_Chapter31_0` through `_3`. The import script is `Tools/build_chapter31_content.py`. Environments and characters combine the existing game mesh library with new authored layout geometry. Speech currently uses the game's placeholder gibberish audio and written subtitles. Cutscenes use live cameras, character movement, raid fire, a departing truck, execution, and surrender staging rather than prerecorded videos.

## Validation
Verified September 9, 2026:
- Unreal Editor Development compile succeeded (`Saved/BuildChapter31g.log`).
- Full Unreal automation suite: 70 passed, 0 failed (`Saved/Story31ReportE/index.json`).
- Final automated live chapter replay: 120 checks passed, 0 failed, across 30 steps (`Saved/Story31_RuntimeG.log`). Includes both ultimatum branches, captivity save/load, exact inventory GUID restoration, duplicate-recovery prevention, boss shield gates, and fortress leadership/respawn.
- Scene captures reviewed in `Saved/ScreenshotsV17/Story31_*.png`; adjusted rescue framing, abduction truck orientation, cinematic fill intensity, and banner UV orientation.
- A slow-frame test timing failure was corrected by waiting for actual mission transitions with bounded timeouts. The final replay passed.

No Unreal builds were cooked or packaged. Voice audio remains placeholder gibberish; cinematics use the existing game character models.
