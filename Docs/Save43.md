# Save hitch correction

The previous automatic save path moved disk writes off-thread but still copied and serialized the entire save on the game thread. Explicit gameplay saves also waited for the preceding writer and then performed a synchronous serialization/write. Mission checkpoints serialized synchronously as well.

Regular and explicit save requests now coalesce into one pending request. Only a detached value snapshot is captured on the game thread. A background task serializes that snapshot using the existing Unreal save format and writes it to the existing slot. No live world, player, inventory or actor is accessed by the worker. A reflected reference retains each immutable snapshot until completion; a regression test rejects object references in saved properties.

There is at most one disk writer per player. Changes made during a write remain pending and the next snapshot captures the latest state. Load, new-game, recovery and shutdown boundaries drain the necessary work before changing worlds or exiting; gameplay requests never wait on disk. Failed writes retain a dirty state for retry and display the existing failure message.

Mission checkpoint serialization also runs in background tasks. Completed tasks commit in capture order. Pending checkpoints remain available to the death/recovery flow, which waits before reading their bytes. Abandoned/completed missions cannot be resurrected by a late checkpoint result. Full saves wait asynchronously for pending checkpoints so they contain the completed recovery data.

The existing version-2 save schema and slot names are unchanged. Snapshot copying still occurs on the game thread; its cost is measured separately from background serialization and I/O. The large-save regression covers 3,000 extra containers, 8 MB of checkpoint bytes, live mutations during a write, 1,000 coalesced requests, garbage collection during serialization, and loading the latest queued result.

Editor compilation succeeded (`Saved/BuildSave43Final.log`). All 85 automation tests passed (`Saved/Tests43.log`), including the value-only snapshot invariant. The large-save runtime test passed all eight checks (`Saved/Save43Smoke.log`): for a 22,297,420-byte fixture, baseline synchronous serialization took 899.100 ms; game-thread capture/dispatch took 5.464 ms, and 1,000 coalesced explicit requests took 0.003 ms. These timings are from this machine and fixture, not a guarantee for every world size.

Mission recovery passed 121 checks (`Saved/Recovery43Smoke.log`), covering death save/load, checkpoint rollback, mission restart, confiscated equipment, cancellation and contract recovery. The gameplay regression passed all 25 checks (`Saved/Gameplay43Smoke.log`), including request batching, deferral during reload, and latest-state save/load. No packaging.
