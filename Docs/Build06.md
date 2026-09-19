# Build 0.6 — Wayfarer

## Inventory and player menu

**Tab** opens the player menu. Inventory, map, skills, contracts and crew share one tab bar. I, K, J and O still open their respective sections. Switching tabs retains an open container; transfers still require it to be unlocked and within reach.

**Shift + left click** transfers an item between your inventory and an open container, stash or merchant. Matching stacks combine when transferring storage items; linked magazines travel with their guns. A transfer that cannot fit leaves its source unchanged. Merchant quick transfers use the normal buy/sell transaction and prices.

**Auto Sort** combines compatible stacks and packs items by footprint and category. Equipped gear stays equipped, ammunition and magazine identities remain intact, and items with different condition or car-key identities never merge. There are separate sort controls for carried items and open storage. Merchant stock cannot be sorted. Extremely dense irregular layouts retain their original placement if the packing pass cannot improve it; compatible stacks still combine safely.

The HUD no longer displays the acoustic state, carried-gear risk slogan, inventory cell totals or row/dimension summaries shown in the feedback screenshots. Item details retain useful ammunition, condition and price information.

## Equipment keys

| Key | Equipped slot |
|---|---|
| 1 | Primary |
| 2 | Secondary |
| 3 | Sidearm |
| 4 | Melee |

Empty slots leave your current weapon selected. Keys 5–8 no longer select weapons. Hotwiring still uses 1–6 for wire selection while that minigame is open.

## World and vehicles

Driveways stop outside foundations. Building placement checks road clearance, including nearby connecting roads. Settlement buildings sit around the paved square with a clear arterial entrance instead of overlapping it. Car collision clearance has been increased above the raised asphalt, and terrain corrections lift the chassis before horizontal movement.

Settlements now include market awnings, supply stacks, repair equipment, tables, chairs and lighting. Six ambient residents join the five service/recruit NPCs. Residents walk between activity spots and pause to work or converse. Ambient residents speak short passing lines but offer no interaction. Merchants stop moving during conversations and trading, and their shop follows their current location.

## Companions

Followers use changing nearby destinations, separation and obstacle routing instead of a formation rigidly tied to your facing direction.

Sedans have **three companion seats**, in addition to the driver. Nearby active companions board automatically while the car is stopped or moving slowly. Seated companions have a basic sitting pose and do not collide with the chassis. They leave when you exit or send them home. Additional companions run behind the car. A companion left more than **120 metres** away for **eight seconds** leaves the active squad and returns to their bunker bedroom; they remain unlocked and can be reassigned later.

Existing saves remain supported. World geometry is rebuilt when its chunks reload; a restart reloads the settlement layouts and road clearance changes. Inventory, car identities and secured loot are retained.

## Development

Post-release audio fix (revised): the player's own gunshots use listener-relative playback at the catalog's configured volume and pitch, with no random per-shot pitch variation. This bypasses positional attenuation, panning and occlusion before playback begins, rather than modifying an already-playing positional sound. Remote reports and enemy hearing still originate at the muzzle; distant NPC gunfire retains normal spatial processing. This source change is left for the project owner to package.

Validation: editor module compiled successfully (`Saved/GunAudioStableBuild.stdout.log`); the editor-based weapon/gameplay suite passed 518 checks (`Saved/GunAudioStableGameplay.log`). The suite uses a live but muted audio device and verifies gameplay regression behavior, not perceived loudness. No build was cooked or packaged for this fix.

The update reuses the existing original Unreal art assets. New behavior lives in `LWInventoryTools`, `LWPlayerTools`, `LWSocial`, and the existing generation, HUD, vehicle and resident classes. Run the live suite with `-LWV6Smoke -LWAudioSmoke`; it uses an isolated save slot and includes the previous vehicle/security suite. Driving remains a terrain-following arcade prototype rather than a full Chaos vehicle simulation.
