# Mission recovery and hitch reduction

While a main-story mission is active, death presents three choices: restart from its latest checkpoint, restart the current named mission, or cancel and return to the bunker. With the chapter inactive or complete, the tracked active side contract uses the same recovery system. Scripted execution at the ultimatum also uses these choices.

Main-story checkpoints advance at objective/stage boundaries. Restarting a mission restores the beginning of the current mission title, not the beginning of the entire chapter. Side contracts capture their accepted state and completed-objective boundaries. Existing saves establish their first available snapshot when the active mission next runs; historical states from before this update cannot be reconstructed.

Retries restore the snapshot's inventory, ammunition, credits, XP, objective flags, killed-enemy records, storage, vehicle records and world interactions together. This also removes post-checkpoint death bags and prevents reward/loot duplication. Collected trading cards remain permanently owned. A retry cannot be invoked while alive except at the scripted fatal ending.

Cancelling rolls back to the mission's starting snapshot, removes the active contract or suspends the main story, restores confiscated gear if necessary, and returns the survivor to the bunker. Resume a suspended chapter through the journal's **Resume Chapter 1** button. A cancelled side contract can be accepted again. Other mission snapshot caches are discarded on a rewind so they cannot restore progress from the abandoned future.

Snapshots use the normal save schema but omit the recovery map inside their payloads, so saves cannot recursively embed themselves. Recovery choices persist in death saves; loading one does not silently auto-respawn. New survivors clear all recovery records.

Performance changes:

- Trading-card artwork and card materials load asynchronously through retained streamable handles. UI drawing only checks whether a requested asset is available.
- Cards use texture streaming instead of forcing every texture fully resident.
- Loose-card placement runs at most once per frame after the chunk queue drains, instead of all POI callbacks running together.
- Placement builds its ignored-actor query once, rather than scanning chunk residents for every candidate surface.
- Vehicle recovery builds one set of live vehicle IDs per frame instead of repeatedly iterating every live actor for every saved vehicle.

Validation commands use the Editor target, Unreal automation tests, and the rendered `-LWV17Smoke -LWRecovery37Smoke -LWAudioSmoke` harness. `-CardSyncBaseline37` benchmarks the former synchronous artwork path in that test harness only. No cook or packaged build is required.

## Verification

The Editor build succeeded. All 76 automated tests passed; the rendered recovery test passed 121 checks, including checkpoint/start rollback, inventory and world-state restoration, death-save reloads, prison cancellation, contract restart/cancellation, retained card bonuses, and all 100 asynchronous artwork references. The death menu was visually inspected.

In separate fresh game test processes on this machine, requesting all 100 card illustrations took 2.547 ms with the asynchronous path versus 68.560 ms with the previous synchronous LoadObject path. This measures main-thread request cost for that workload, not average FPS or proof that all world-generation hitches are eliminated. Both runs completed their 121 checks. Logs: Saved/Recovery37Final.log, Saved/Recovery37Baseline.log, Saved/TestsRecovery37.log. All 100 existing card texture assets were resaved with texture streaming enabled. No packaged build was produced.

