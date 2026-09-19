# Build 0.12 - Tavern card tables

Blackjack, four-point partnership Pitch, and Last Card now use an illustrated tabletop with portrait cards, fanned hands, decks, discards, wagers, and paced opponent turns. Click a legal card, or drag it onto the center of the table. Click the deck to draw. Choose a chip denomination and Deal to start a round.

Blackjack supports one split, doubling individual hands, and late surrender. Split aces receive one card each. The dealer stands on soft 17. Naturals pay 3:2; split 21 pays 1:1. There is no insurance or resplitting.

Pitch has a clockwise auction starting left of the rotating dealer, public bids, visible tricks, partnership scoring, and opponents that conserve trumps and support their partner. The house uses four-point Pitch with a match target of seven.

Last Card is the existing two-player color-matching house game, with drawn-card restrictions, wild color choice, draw penalties, and the last-card call. Its rules are available at the table.

Leaving saves the deck, hands, pending opponent turn, and committed wager. Reopening resumes play; rewards are applied once. The world continues to run while seated at a card table.

Artwork is in `ArtSource/Cards/T_CardAtlasV12.png`; its prompt and provenance are in `Docs/CardArtwork12.md`. The in-game atlas is `/Game/Art/Textures/T_CardAtlasV12`. Table material variants are in `/Game/Materials/M_Card*V12`.

The audio catalog now exposes CardDeal, CardShuffle, and CardChips. Their source files can be replaced through the existing audio catalog. The content importer preserves existing assignments.

Run `Tools/build_v12_content.py` through the Unreal editor Python commandlet to import these assets into another checkout. No packaged build is produced.
