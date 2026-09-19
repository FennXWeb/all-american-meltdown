# Build 0.12 validation

- Editor compile succeeded: `Saved/Build12Verified.stdout.log`.
- Content import succeeded: `Saved/Content12Final.log`.
- All 42 automation tests passed: `Saved/Automation12Verified.log`.
- The match simulation exercises 600 seeded matches across the three games, alternating immediate and paced opponent turns. It checks completion, card conservation, and bounded payouts.
- Split-hand tests cover incremental bets, active-hand transitions, dealer pacing, surrender, auction turn order, and saved state.
- All 62 tavern integration checks passed: `Saved/Regression12Final.log`. This covers world placement, table interaction, saved hands, wager debits, payout protection, forfeits, death/new-game reset, and music playback.
- Offscreen screenshots were inspected for Blackjack, Pitch, and Last Card. Texture sampling and Pitch label placement were corrected after the first render. Final screenshots are in `Docs/ScreenshotsV12`.

An initial split-hand array assertion was fixed. A later test expectation was corrected to account for the existing starting dealer (West); game auctions already opened correctly to the dealer's left. Final runs above passed.

The automated integration suite uses its isolated V11 save slot. No packaged build was produced. Mouse drag behavior was reviewed in source; these automated checks do not replace manual mouse-input playtesting.
