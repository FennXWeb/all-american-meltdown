# Update 18 — settlements, convoys and wasteland threats

This update is implemented in the Unreal project. Packaging remains a manual user step.

## Weapons and vehicles

The double barrel shotgun uses two persistent chambers, with live, spent and empty states. Each trigger pull fires one barrel. Reloading opens the hinged barrel assembly, ejects spent shells, inserts loose 12-gauge shells one at a time and closes the action. Interrupting a reload preserves rounds already inserted. Live shells remain loaded during a partial reload. The gun appears in applicable loot and merchant tables and uses existing weapon rarity bonuses. Its classic break-action model has no attachment mounts.

Cars lose health from weapons and collisions. Damage reduces acceleration; critically damaged cars catch fire before exploding, leaving a saved, undrivable wreck. Explosions use radial damage and can ignite nearby vehicles. Burning effects and an occupant warning give the player time to leave.

Squad Leader now has eight ranks. With Commander, the active companion limit reaches ten. Overflow companions seek nearby usable vehicles, assign a companion driver and passengers, and follow the player's vehicle using the generated road network. Companions can unlock and hotwire a parked vehicle, taking ten seconds before departure; already usable vehicles depart after boarding. Burning, destroyed, occupied or moving vehicles are excluded. Convoys brake for obstructions and disembark when the player leaves their vehicle. Crew without a usable nearby vehicle retain the existing on-foot following behavior. A convoy separated by more than 120 metres for eight seconds releases its vehicle and sends its crew back to the bunker.

## Places and enemies

Police station: public reception, dispatcher consoles, briefing area, bullpen, interrogation, evidence cabinets, locker facilities and separate holding cells with usable bunks.

Arcade: new shaped cabinet models, pinball tables, air hockey tables, prize counter, repair/storage area and washroom. A generated distressed space-and-racing mural appears on arcade surfaces.

Crossroads Plaza replaces the enclosed strip mall layout with four separate rear storefronts, cafe and laundromat wings, planted pedestrian space, seating and parking. Storefront doors, windows and storage are individual interactions.

New creatures: mutated moose, ten-foot muscular titan, horned deathclaw, giant scorpion and Zombie Karen on a mobility scooter. Each has a multipart model, health/movement profile and sound slot. Moose charge, deathclaws lunge, titans wind up heavy attacks, scorpions sting and drain stamina, and the scooter zombie rushes the player. New creature weights are available in the enemy spawn table; large creatures spawn outside.

## Beds and settlements

Interact with a bed to sleep or set it as the respawn point. The motorhome bed follows the saved vehicle identity, including after the vehicle moves. An unavailable or destroyed vehicle respawn falls back to the bunker.

Settlements have seeded camp/community/town sizes, distinct settlement names and persistent unique settler names. Speak to an interactive resident and choose **Settlement affairs**.

- Reputation ranges from -100 to +100.
- Attacking costs 15 reputation and alerts the settlement for 12 game hours.
- Taking settlement-owned goods costs 5 reputation and alerts the settlement. Leaders may use local storage.
- Contract completion earns 12 reputation. Trading earns reputation with a daily cap of five points.
- At -100 reputation the settlement remains hostile permanently. Other hostility expires while the negative reputation remains.
- Membership requires 40 reputation. Leadership requires 100.
- Leaders can station companions, choose the settlement as a respawn point, and collect income and consignment earnings.
- Leadership income is 2/3/4 credits per game hour by settlement size.
- Up to 12 consignments are supported. Listed items leave the player's inventory, including linked magazines. They sell after a randomized 18–96 game hours for a premium over ordinary resale. Collect proceeds through settlement dialogue.
- Membership is revoked below 40 reputation; leadership is revoked below 75.

Settlement state, names, listings, earnings, crew assignments and respawn choice are stored in the existing survivor save. Existing saves can load without starting over; regenerated layouts replace previous layouts when their chunks load.

## Assets and editable sources

- `Tools/make_models_v18.py`: 35 new meshes and the editable Blender scene in `ArtSource/ModelsV18`.
- `ArtSource/TexturesV18/T_ArcadeMuralV18.png`: mural generated with the built-in image tool.
- `Tools/make_audio_v18.py`: ten generated placeholder effects; replace them through the existing audio catalog.
- `Tools/build_v18_content.py`: additive import preserving existing audio replacements and other catalog entries.

Image prompt: "Create one square game-ready flat diffuse texture image, edge-to-edge without border: a distressed 1990s American arcade wall mural for a post-apocalyptic PS1 horror game. Deep midnight navy, faded magenta and electric cyan. Stylized retro space fighter spacecraft, meteor craters, rings of a planet, flowing checkerboard racing track, stars and lightning silhouettes. Rich layered printed ink, chipped paint, grime on old vinyl, restrained scratches, faded patches. Strong readable graphic shapes and authentic screenprinted illustration, not generic neon abstract. No text, no logos, no perspective, no scene, no lighting or mockup. Full frame flat mural texture ready to apply to walls and arcade cabinet side panels."

Validation: 45 automated tests and 376 rendered gameplay checks passed; see [Validation18.md](Validation18.md). All 35 meshes passed import validation. No package was created.

