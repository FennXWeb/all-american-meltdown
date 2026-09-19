# Wasteland trading cards

100 collectible cards in ten series, each with an individually generated portrait illustration. Original PNGs are `T_TradingCard00136.png` through `T_TradingCard10036.png`; imported textures live in `/Game/Art/Textures`. `catalog.json` contains every title, series, rarity, permanent bonus and asset path. `Prompts.json` records the full successful prompt for each illustration. Generation used the built-in image generation tool, one request per card; no image-editing CLI or shared replacement atlas.

## Playing

Cards are small physical pickups on exposed furniture and floors in POIs. Look at one and press E. They are never inserted in a container's loot table, merchant stock or the backpack. Acquiring a card immediately saves it and applies its permanent bonus once. Collection persists through death and save/load; a new game starts a fresh collection. Already collected card identities disappear instead of granting additional bonuses.

Open the survivor menu and select **Collection**. Browse ten cards per page, filter by collected status or rarity, select a card to inspect its artwork and bonus, or choose **Bonuses** to see the combined effects. Undiscovered card artwork remains concealed.

## Deck and balance

Each ten-card series contains five common, two uncommon, one rare, one epic and one legendary card. The series improve maximum health, maximum stamina, firearm damage, melee damage, damage resistance, experience gain, merchant discounts, healing, reload speed and stamina recovery respectively. Bonuses use existing RPG stat consumers and their existing limits. Rarity scales each series' base bonus by 1, 1.5, 2, 2.5 or 3.

Each POI has a seeded 46% placement attempt, subject to finding a supported flat surface. Card identity is stable for the site and map seed. Per-card normal rarity weights are 16 / 8 / 3 / 1 / 0.35. Dungeons, underground POIs and casinos use 5 / 5 / 4 / 2 / 1, providing better rare-card odds. These weights apply to each card, so tier probabilities also depend on the number of cards in that tier. Duplicate identity draws do not reroll; exploring new locations is required.

## Development

Gameplay: `Source/LethalWorld/LWTradingCards36.cpp`; definitions: `LWTradingCardDefs36.inl`; UI: `LWCollectionHUD36.cpp`. Ownership is saved in `FLWRPGState::TradingCards36` and bonuses are aggregated through `LWRPG::Stat`.

Run `Tools/import_trading_cards36.py` as an Unreal Python commandlet to import all 100 images and rebuild the card-face and rarity-frame materials. It fails if any artwork is absent. The optional `-TradingCardsPartial` flag is only for development previews.

Automation: `LethalWorld.TradingCards36.CollectionPersistenceAndRarity`. Runtime smoke: `-LWV17Smoke -LWTradingCards36Smoke -LWAudioSmoke`, with screenshots under `Saved/ScreenshotsV17/TradingCards36_*.png`. These compile/test/import workflows do not package the game.

## Verified results

Editor target compiled successfully. All 75 Unreal automation tests passed. The final rendered runtime smoke passed 111 checks, including all 100 artwork references, seeded loose-card placement flush on a supporting surface, acquisition, duplicate protection, disk save/load, combat input gating and respawn persistence. All 100 source PNGs are 1024 x 1536 with distinct SHA-256 hashes (ArtworkManifest.json). The collection screen was visually inspected in game. No cook or packaged build was produced.

