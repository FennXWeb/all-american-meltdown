# The Gilded Republic Resort and supercar

Seven accessible floors, connected by physical switchback stairs:

- Two gaming floors with 168 playable slot machines and 32 full card tables.
- A shopping promenade with four functioning merchants and furnished shops.
- Restaurants, kitchens, bars and lounge seating.
- Two hotel floors with 32 private rooms, usable beds, storage and bathrooms.
- A presidential/VIP floor with dining, a private card table, bed and locked high-tier storage.

The resort has a marble forecourt, attached 24-bay parking lot, chandeliers, gold trim, patterned carpet and illuminated signs. Each parking bay has a seeded 65% occupancy chance. Occupied bays strongly favor supercars, with muscle cars and SUVs making up the remainder. Vehicles retain their normal VIN, key, lock, damage and storage systems; streaming does not overwrite existing vehicle records.

Resorts are rare destinations beside city-region arterial roads. Their full footprint is checked against neighboring roads and buildings. They appear in the map/POI naming system and have no generic interior enemy spawns. Existing saves are retained; a new world provides the cleanest view of changed generation.

## Gambling

Use E at a slot machine to wager 10 credits. Reels animate and stop in sequence. Matching pairs pay 5; triples pay 60, 90, 160, 400 or 2,000 depending on the symbol. The theoretical return is approximately 94.16% of stakes. The wager, payout and persistent spin sequence are saved together before presentation, preventing duplicated payouts from repeated input or interrupted animation. Idle machines do not tick.

Card tables use the existing complete blackjack, pitch and color-card game rules, decks, betting, animations and UI. Hotel and VIP beds support the existing sleep and respawn interactions.

## Supercar

New body and wheel meshes replace the old supercar throughout the game. Features include a lower curved cabin, orange bodywork, sculpted fenders, wheel-arch openings, side intakes, dark trim, glazing, rear wing, diffuser, exhausts and separate rotating wheels. The cockpit, seats, controls, windshield and first-person eye height are adjusted together. Existing driving, cargo, keys, weather, lights and wipers remain connected.

## Source and verification

- `Tools/make_models_v21.py`: eight meshes and editable Blender source in `ArtSource/ModelsV21`.
- `Tools/make_textures_v21.py`: carpet, marble and carbon textures.
- `Tools/build_v21_content.py`: imports models/materials and adds the resort spawn profile.
- `Tools/VerifyCasino21.ps1`: editor automation and offscreen gameplay checks.

No packaged build is created.

## Validation — September 7, 2026

Editor Development compilation succeeded (`Saved/BuildCasino21e.stdout.log`). All 47 editor automation tests passed, including slot payout mathematics and resort placement; the sampled seed produced 10 resorts across 169 regions. The final offscreen suite passed 184 checks with zero failures: asset loading, parking mix, seven floors, stair treads/headroom, room beds, card/slot counts, slot accounting, and supercar entry, ignition, headlights, wipers, acceleration and braking.

Logs: `Saved/Casino21_Automation.log`, `Saved/Casino21_FinalRendered.log`. In-engine previews: `Saved/ScreenshotsV17/Casino21_*.png` and `Saved/ScreenshotsV17/Supercar21_*.png`. These are sampled automated checks and visual inspections, not an exhaustive playthrough of every seed. No packaging was run.
