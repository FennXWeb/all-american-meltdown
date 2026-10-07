# Persistent NPC voices — starter library

The approved starter scope is three male and three female voices. There are 28 recordings per profile (168 clips, approximately 5.5 minutes total): a preview plus shared greetings, companion remarks, combat calls, medical reactions and encounter responses. ElevenLabs reported 2,922 credits used against the approved 3,851 cap. No further generation is automatic.

| Stable profile | ElevenLabs voice | Named cast |
|---|---|---|
| M01 | Roger | Jonah, narrator |
| M02 | Brian | Tomas, Mercer |
| M03 | Harry | Rusk |
| F01 | Sarah | Mara, Tessa |
| F02 | Jessica | Inez |
| F03 | Laura | Elsie, Voss |

`Data/VoiceProfiles59.json` stores the actual provider voice IDs, model, settings and named casting. Never replace a profile's voice ID to change an individual NPC. Add a new profile and an explicit casting entry instead. Existing NPC assignments are saved in `FLWRPGState.VoiceProfiles59`; procedural identities use a fixed, case-normalized CRC assignment on first use. Streaming out, appearance changes and loading a save do not reroll that assignment. Story aliases such as “Mara, through the wall” resolve to the same identity as Mara in person.

## Original coverage (superseded by update 75)

See [Dialogue75.md](Dialogue75.md) for the expanded cast, complete recorded script,
current assignment rules, credit receipt, and chatter behavior. The section below
documents the original six-voice starter batch.

The enabled catalog replaces human gibberish with recorded lines where available. **The full dialogue library is not voiced yet.** Unrecorded story scenes, longer conversations and dynamically assembled text remain subtitled without gibberish. Creature vocalizations remain unchanged. Trader robots use a spoken sales greeting. No speech API calls, secrets, subscriptions or network access are required while playing.

Recordings load asynchronously. Each speaker has one cancellable channel; a new line cancels their previous line and pending load. Audio follows the NPC, uses world attenuation/occlusion, and drives existing facial animation. Subtitle duration follows recorded duration. Cinematic shots wait for their speech channel before advancing when a recording exists.

## Adding dialogue without changing voices

1. Keep the existing profile registry. Use the same stable profile ID for the NPC.
2. Create a JSON batch under `ArtSource/Dialogue59/` with a `lines` array. Each entry has `profile`, `text`, `key`, and `file`. Use `Tools/dialogue59.py`'s `line_key(profile, text)` to form the key and append `.wav` for the filename. Keys normalize whitespace, preserve punctuation/case, and hash UTF-8 text.
3. Run `python Tools/dialogue59.py --manifest PATH --generate --budget TOTAL_CAP`. The cap includes all recorded spending in this library's ledger. Generation skips completed clips and refuses silent voice/settings changes or ambiguous interrupted requests.
4. Run `Tools/import_dialogue59.py` through Unreal Editor's Python commandlet, then restart the game. Import merges the batches into `/Game/Audio/Dialogue59/DA_Dialogue59`; it must not enable incomplete batches.

The generation ledger records request fingerprints, clip lengths and returned credit costs. Pending/failed requests require review before retrying; they are never silently charged again. Source WAV/MP3 files are retained alongside the manifest for future reimport. The runtime asset catalog contains no API keys.

## Validation

The editor target compiled successfully (`Saved/Build59f.log`). The automated identity/key/save-serialization regression passed (`Saved/Tests59.log`). The in-game scenario passed all 22 checks (`Saved/Smoke59c.log`): every voice played through asynchronous loading, cancellation worked, missing recordings did not stall, and resident speech followed its actor with world attenuation and subtitles. All 168 processed WAV files passed silence, peak, duration and edge-pop checks. Test runs used the automation save, and user graphics settings were restored afterward.

No packaged build is produced.

## Voice previews

- [M01 - Roger](../ArtSource/Dialogue59/M01_cc976d7d8a118ee26407e2a5ab26847d.mp3)
- [M02 - Brian](../ArtSource/Dialogue59/M02_cc976d7d8a118ee26407e2a5ab26847d.mp3)
- [M03 - Harry](../ArtSource/Dialogue59/M03_cc976d7d8a118ee26407e2a5ab26847d.mp3)
- [F01 - Sarah](../ArtSource/Dialogue59/F01_cc976d7d8a118ee26407e2a5ab26847d.mp3)
- [F02 - Jessica](../ArtSource/Dialogue59/F02_cc976d7d8a118ee26407e2a5ab26847d.mp3)
- [F03 - Laura](../ArtSource/Dialogue59/F03_cc976d7d8a118ee26407e2a5ab26847d.mp3)
