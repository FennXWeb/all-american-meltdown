# NPC sound arrays

Generated 75 ElevenLabs SFX on September 12, 2026, for a reported total of 1,975 credits (2,500 cap). Sources, prompts and the charge ledger are in ArtSource/AudioV44. Tooling reads the configured key locally; no credentials or API calls ship with the game.

Each non-human bank contains five distinct clips: Zombie, DogGrowl, MannequinMove, MooseRoar, TitanRoar, DeathclawRoar, ScorpionHiss, KarenShriek, BehemothVoice44, ColossusVoice44 and WorldEaterVoice44. HumanMale44 and HumanFemale44 each contain ten expressive, invented-language vocalizations. Existing voice timing, spatial muffling, subtitle and facial-animation hooks are retained.

Assets import to /Game/Audio/NPC44. The existing /Game/Audio/DA_AudioCatalog Tracks arrays randomly choose a clip per call. Human residents and raiders select their voice bank from Appearance35.Body; story shots route named speakers to the corresponding bank. Legacy Speech0/Speech1/RaiderVoice entries use the male array and Speech2 uses the female array. Giant bosses use dedicated banks in burrow, emergence and attack cues.

Tools/npc_audio44.py performs generation; Tools/import_npc_audio44.py validates counts and imports the arrays. All 75 WAV files passed signal-level/duration checks (Saved/NPC44AudioValidation.json). These automated checks do not constitute subjective listening review. No packaging.

Editor build succeeded (Saved/BuildNPC44.log). Import confirmed 75 clips in 13 groups (Saved/NPC44Import.log). All 54 runtime checks passed (Saved/NPC44Smoke.log): required counts, distinct assets, randomized non-looping resolution, active audio playback and gender routing.
