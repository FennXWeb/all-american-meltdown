# Build 0.11 validation

Unreal Engine 5.8.1, Win64 Development Editor. No cooking or packaging was performed.

| Check | Result | Evidence |
|---|---|---|
| Editor target compilation | Passed | Saved/Build11.stdout.log |
| Unreal automation suite | 41 passed, 0 failed | Saved/Automation11Final.log |
| Tavern/card/music gameplay suite | 77 checks, 0 failed | Saved/RegressionV11Final.log |
| Existing POI/vehicle/weather/world-setup suite | 1,777 checks, 0 failed | Saved/RegressionV9_11.log |

The automation suite includes 300 complete simulated card matches across 100 seeds per game, card conservation, blackjack aces and payouts, Pitch legality/scoring, Last Card penalties, serialized deck/random-state restoration, settlement variation, road connectivity, parcel separation, playlist selection, source fallback and unchanged SFX selection.

The gameplay suite visits a real procedurally generated gaming tavern and checks table/detail meshes, all three game screens, debits and payouts through the player API, real save/load restoration, insufficient funds, repeated settlement actions, forfeits, death/new-game cleanup, and six successful 2D music component starts from a two-track playlist. Test saves use LethalWorld_AutomationV11, separate from the survivor save. Playlist edits used for testing are restored in memory and never saved over the audio catalog.

Screenshots in ScreenshotsV11 were inspected for readable controls/card faces, rule text and doorway clearance. Games use automated opponents and documented house rules; this is not a claim of exhaustive balance testing or manual playthroughs of every seed. Music playback was checked with an active muted audio device, not by an audible listening review. Existing sound assets are retained; no new music recordings were added.
