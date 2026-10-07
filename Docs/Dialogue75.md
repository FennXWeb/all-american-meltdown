# Recorded NPC dialogue (update 75)

The user limited this update to **15,000 ElevenLabs credits** after the initial
request for ten additional voices. The complete batch used **12,351 credits**, also
verified against the account subscription counter. No further generation is queued.

## Cast and coverage

Four new persistent profiles are active for general NPCs. Each has the complete
shared dialogue library, so recruitment, trading, hostility, and other role changes
do not change an NPC's voice.

| Profile | Provider voice | Character |
|---|---|---|
| M04 | Eric | Relaxed, clear American male |
| M05 | Bill | Older, measured American male |
| F04 | Matilda | Warm, expressive American female |
| F05 | Alice | Clear British female |

Provider voice IDs, model, settings, and named casting remain in
`Data/VoiceProfiles59.json`. These use established ElevenLabs voices, not clones of
real actors or newly trained voices. Keep profile IDs and settings immutable when
adding future dialogue.

**781 new recordings** cover 183 shared texts in all four active voices and 49
retained named/story recordings. The 168 original recordings remain, giving **949
clips / 54.4 minutes** in the imported catalog. Retained chapter dialogue is recorded
without re-enabling the removed main story. Numerical prices, quest counters,
settlement names, reputation, and balances remain UI data; spoken quest descriptions
and settlement responses accompany those screens instead of attempting to speak
every possible combination of changing values.

The new chatter script contains **30 companion, 30 friendly-settler, and 18 raider
lines**. Companion contexts include travel, combat, injuries, rain, night, and
vehicle rides. Settlers have general, merchant, warden, medic, Canada, and alarm
lines. Raiders distinguish searching, visual contact, reloading, and suppression.

Old procedural assignments are mapped consistently into the reduced active cast.
Their original saved assignments and all six original profiles are retained. Named
story characters retain their original voices. New NPCs receive a deterministic
gender-matched voice, saved by persistent identity; streaming and appearance changes
do not reroll it.

## Playback

- Each context exhausts its available lines before repeating. The last line of a
  cycle cannot also be the first line of the next cycle.
- A world-level director spaces nearby dialogue and prevents recently heard lines
  being echoed by another NPC. It respects pending audio loads as well as playback.
- Ambient speech waits during conversations, menus, cutscenes, and inventory use.
- Ordinary chatter intervals are 45–80 seconds per resident, with shared pauses.
  Combat calls have their own shorter cadence. Direct dialogue takes priority.
- Existing async audio loading, spatial attenuation, room/car acoustics, facial
  animation, and timed resident subtitles remain connected. No API calls occur in
  gameplay and no credentials enter the assets.

## Editing and generating

1. Edit chatter in `Data/Dialogue75.json` and spoken dialogue in the corresponding
   source/data definitions. `python Tools/dialogue75.py` creates the native chatter
   table and complete recording manifest, without contacting ElevenLabs.
2. Review `Saved/Dialogue75Plan.json` and `ArtSource/Dialogue59/complete75.json`.
3. `python Tools/generate_dialogue75.py` resumes only missing recordings. The
   15,000-credit cap and historical baseline are saved in `budget75.json`; **do not
   reset the baseline to bypass the cap**. Pending/failed requests require review,
   not blind retries. Usage and request fingerprints remain in `usage.json`.
4. Compile the editor and run `Tools/import_dialogue59.py` through Unreal's Python
   commandlet. It validates completeness before publishing the catalog, preserves
   old clips, and skips unchanged sound imports.
5. Run `LethalWorld.Update59.VoiceIdentity`, `LethalWorld.Update75`, and the
   `-LWV17Smoke -LWUI46Smoke -LWDialogue59Smoke` unpackaged game scenario.

Audio quality checks are in `Saved/Dialogue75AudioQA.json`; the generation receipt
is `Saved/Dialogue75Generated.json`. No packaged build is produced.

## Verification

- Editor compilation succeeded: `Saved/BuildVoice75Final.log`.
- Both `Update75` automation tests passed: `Saved/TestsVoice75.log` (pool exhaustion,
  repeat prevention, missing groups, complete recorded coverage, and voice casting).
- Unpackaged game scenario passed **359 checks / zero failures**:
  `Saved/SmokeVoice75.log`. All ten stored profiles played, original save assignments
  remained intact, audio cancellation worked, new chatter played with timed
  subtitles, speech followed the NPC through the acoustic system, and menus blocked
  ambient chatter. Missing recordings did not stall the channel.
- All **949 WAV files** passed format, finite samples, silence, duration, and clipping
  checks. All 781 paid recordings matched their immutable request fingerprints and
  had imported Unreal assets.
- The account counter independently confirmed **12,351 credits** used. The in-game
  test used the isolated automation save; original graphics settings were restored
  and their SHA-256 verified afterward.
