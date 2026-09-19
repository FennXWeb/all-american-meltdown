# Build 0.11 — Settlement taverns and music playlists

## Taverns

The starting settlement requests a tavern, as do approximately 65% of other settlements. A tavern uses a plot that has already passed road and building-clearance checks, preferring the diner and falling back to another settlement building. It retains that plot's approved footprint and road entrance. Settlements with no valid building plots remain open camps. These are friendly, roadside buildings with a wood-floor public room, bar, stocked back shelves, kitchen, booths, seating, a merchant host and civilian patrons. Three quarters have all three wagering tables; the others are social taverns. Placement and table availability are determined by the world seed. Existing settlements update when their chunks regenerate.

Walk to a card table and press E. Wagers are 10, 20, 50 or 100 credits. The Rules button explains the local rules. Leave or Escape closes the table without discarding your round. Resume at any table of that game. Only one wager can be active at a time. Forfeit ends it and loses the wager. Death also forfeits it. The world continues to run while you play.

Wagers are debited before dealing. Deck order, hands, random generator state, Pitch match scores and payout status are saved together with the player's credit balance after every successful action. An already settled round cannot pay twice. New Game clears the round; older saves start with no active round.

### Blackjack

One player against a dealer who stands on all 17s, including soft 17. Aces soften as needed. Hit, stand and first-two-card double are supported. Blackjack pays 3:2; ordinary wins pay 1:1; ties return the stake. No splits or insurance. The result reveals the dealer's cards. Each round uses a freshly shuffled standard deck.

### Pitch

Four-point partnership Pitch with three AI seats. You and North form a team against East and West. Six cards per player, four points available per hand: High, Low, Jack, Game. High and Low belong to their original holders; Jack goes to the team capturing the trump jack. Game counts captured tens (10), aces (4), kings (3), queens (2) and jacks (1); a tie awards no Game point.

This tavern uses one auction: you offer first, then East, North and West can raise. Minimum bid is 2, maximum 4; if everyone passes the dealer is forced to bid 2. The bidder selects a held suit as trump and leads a trump. Follow suit or trump; when void of the led suit, any card is legal. Missing the bid subtracts the bid from that team's match score. First to 7 wins; simultaneous qualifying scores favor the bidding team if it made its bid. The wager covers the entire match. Continue advances tricks and hands so you can read the table.

### Last Card

An original presentation of a two-player UNO-style house game with a 108-card deck. Match color or symbol; wilds select a color. Skip and Reverse both skip the opponent in this two-player version. Draw Two and Wild Draw Four draw and skip; penalties cannot stack. Wild Draw Four is legal only when no current-color card is held. No challenge rule.

Draw one, then either play that drawn card if legal or pass. Press LAST CARD before playing down to one; forgetting draws two. Finishing on a draw card applies its penalty before ending the game. The discard is recycled when the draw pile runs out, retaining its top card. Large hands paginate.

## Music authoring

Open **/Game/Audio/DA_AudioCatalog** in Unreal's Content Browser. Expand Slots and a music entry such as **CombatMusic**, **ExploreMusic** or **MenuMusic**. Enable Music if creating a custom music slot. Add entries to **Tracks** and assign imported SoundWaves, SoundCues or MetaSound Sources.

A nonempty playlist takes priority over Source. Each activation chooses randomly among distinct, loadable entries; repeats are allowed. Empty or missing entries are skipped. If no playlist entry loads, the existing Source assignment is used, then the legacy asset fallback. Duplicating an entry does not increase its selection weight. Existing Source assignments are preserved and continue to work without migration.

Volume, Pitch and Loop apply to the selected track. SoundWaves honor Loop without changing their source assets; SoundCue/MetaSound graphs must implement their own loop behavior. Music remains 2D and crossfades through the existing world music controller. Combat-to-exploration-to-combat selects again on the next combat activation. With Loop disabled, finishing a track allows the controller to select another. Tracks now also work for non-music SFX slots; see [all-event audio tracks](AudioTracks.md). SFX retain their existing spatialization and attenuation settings.

Use the asset's validation action to find missing tracks, duplicate entries, missing fallback assets and loop mismatches. No new music recordings are included; add your tracks to the new array.

## Files and validation

Rules: LWCardRules.cpp / LWCardGame.h. Table actor and credit integration: LWCardTable.cpp. Interface: LWCardHUD.cpp. Taverns: LWPOI.cpp and LWGeneration.h. Tests: LWCardTests.cpp and the V11 gameplay suite. Playlist selection and validation: LWAudioCatalog.cpp/.h.

No packaged build is produced.
