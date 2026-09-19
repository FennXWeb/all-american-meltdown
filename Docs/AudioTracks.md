# Tracks for every audio event

Open `/Game/Audio/DA_AudioCatalog`, expand **Slots**, then expand any event and add files to **Tracks**. This works for SFX, weapon reports, footsteps, voices, vehicles, ambience, and music, including custom event keys. Music does not need to be enabled to use Tracks.

Each playback randomly chooses one distinct, loadable track. Repeats are allowed; duplicate entries do not increase selection weight. Empty or missing tracks are skipped. If none loads, playback falls back to **Source**, then the existing legacy asset when fallback is allowed. Existing Source-only entries need no changes.

The selected file retains the event's Volume, Pitch, Loop, and attenuation settings. Music remains 2D; positional effects retain spatialization/occlusion, and local weapon reports retain listener-relative playback. The Music checkbox controls music behavior, not access to Tracks.

For loops, selection happens when the audio component starts; that selected file loops until stopped. It does not choose a new file at each loop boundary. SoundWave looping follows the catalog setting; SoundCue/MetaSound graphs must implement their own loop behavior.

Editor source changes only; no packaging required to edit the catalog in Unreal.
