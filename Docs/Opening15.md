# All American Meltdown / Build 0.15

## New survivor flow

New Survivor opens the existing world seed/difficulty setup. Continue starts a 162-second, nine-scene in-engine prologue with timed subtitles, camera movement, transitions and original synthesized audio. Pause/Resume controls stop both the cinematic clock and its audio; Escape or Enter also toggles cinematic pause. Skip Intro moves directly to character creation, never directly into gameplay. Existing saves continue without replaying the opening.

The prologue is a fictional, civilian-focused disaster and survival story. It covers shortages, street fires, interrupted broadcasts, the nuclear exchange, taking shelter, isolation and life outside the bunker. It does not expand the requested political backstory about U.S. officials or policies. The complete timed script is in `Source/LethalWorld/LWOpening.cpp`. This build uses subtitles and sound design, not recorded spoken narration.

## Character creation

- Name: up to 18 letters/spaces; click the name field and type, using Backspace to erase.
- Six skin tones, three face shapes, four hairstyles, four hair colors, three body frames and four clothing palettes.
- Live 3D preview with a rotate control. Appearance has no gameplay stat bonuses. Clothing colors are also applied to first-person sleeves, preserving glove and metal materials.
- 28 starting category points, initially four in each of the seven existing categories. Use minus/plus to redistribute; each category must remain between one and ten, and all points must be assigned.
- Category values determine access to existing perk tiers. Level-up skill points continue to purchase perks; creation does not bypass perk level requirements.

Enter Bunker commits the world settings, identity and category allocation together, then starts at the safe bunker spawn. Back abandons the draft without replacing the previous survivor save. Identity and categories survive save/load. Internal project/module and legacy survivor save names remain stable for compatibility; the visible game title and menus use All American Meltdown.

## Content authoring

`/Game/Audio/DA_AudioCatalog` contains IntroArchive, IntroUnrest, IntroAlarm, IntroBlast, IntroAftermath and IntroShelter. Each scene is 18 seconds. Replace Source or add playlist Tracks. The original sound generator is `Tools/make_intro_audio.py`; import with `Tools/build_v15_content.py` after compiling the editor target.

Five original Blender-authored meshes provide the survivor head, torso and limbs, irregular cloud volumes, and flame shapes. Source scene: `ArtSource/ModelsV15/AllAmericanMeltdown_V15.blend`; generator: `Tools/make_models_v15.py`.

The generated logo is `ArtSource/Branding15/T_AllAmericanMeltdown.png`, imported as `/Game/Art/Textures/T_AllAmericanMeltdown` and shown on the main and pause menus. Generated using the built-in imagegen tool; no raster postprocessing. The verbatim generation prompt is retained in `ArtSource/Branding15/prompt.txt`. Prompt: Original title logo for a fictional post-apocalyptic survival horror game. Exact words ALL AMERICAN above the larger MELTDOWN. Distressed condensed slab lettering, aged ivory and oxidized burnt orange, subtly melting lower edges, fractured star and radiation sunburst. Crusty PS1-era survival horror, weathered screenprint, dark charcoal green background, readable centered landscape composition. No other text, watermark, real-world party symbols or political figures.

## HUD

Vitals and ammunition use thin translucent panels near the bottom corners. Credits remain visible above the weapon strip. The smaller minimap retains its route, scale and orientation information. Location, day/time, heading, level/skill points, tracked quest progress, nearby encounter objective, NPC subtitles, hit feedback, weapon slots, control hints and vehicle information remain available.

No game package was produced.
