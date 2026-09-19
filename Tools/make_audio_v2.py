"""Synthesize 30 original Lethal World placeholders using only numpy + stdlib.

    python Tools/make_audio_v2.py
    python Tools/make_audio_v2.py --check
    python Tools/make_audio_v2.py --output <scratch-directory>

Default output: ArtSource/Audio/S_<slot>.wav, PCM16, 32 kHz. Effects are mono;
music is stereo. Existing files are preserved unless --overwrite is supplied.
The original 16 sounds from make_surfaces.py are never generated or modified.
There are no recordings, samples, downloaded assets, imitated voices, or borrowed
melodies. TraderVoice is intentionally synthetic nonverbal radio chatter.
"""

from __future__ import annotations

import argparse
import hashlib
import math
from pathlib import Path
import wave

import numpy as np

ROOT = Path(__file__).resolve().parents[1]
SAMPLE_RATE = 32000
MUSIC_SLOTS = ("MenuMusic", "ExploreMusic", "CombatMusic")
NEW_SLOTS = (
    "RevolverFire", "RevolverOpen", "RevolverEject", "RevolverInsert", "RevolverClose",
    "SniperFire", "SniperBolt", "SniperMagOut", "SniperMagIn",
    "SMGFire", "SMGMagOut", "SMGMagIn", "SMGCharge",
    "RifleFire", "RifleMagOut", "RifleMagIn", "RifleCharge",
    "LMGFire", "LMGCover", "LMGBelt", "LMGClose",
    "LootPickup", "InventoryMove", "TraderVoice", "Trade", "BunkerDoor", "DeathDrop",
) + MUSIC_SLOTS


def clock(seconds: float) -> np.ndarray:
    return np.arange(round(seconds * SAMPLE_RATE), dtype=np.float64) / SAMPLE_RATE


def random_for(name: str) -> np.random.Generator:
    # Independent seeds keep an existing event unchanged when the list is expanded.
    seed = int.from_bytes(hashlib.sha256(("LW-original-audio-v2/" + name).encode()).digest()[:8], "little")
    return np.random.default_rng(seed)


def smooth(signal: np.ndarray, width: int) -> np.ndarray:
    return np.convolve(signal, np.ones(width) / width, mode="same")


def noise(rng: np.random.Generator, seconds: float, width: int = 1) -> np.ndarray:
    result = rng.normal(size=len(clock(seconds)))
    return smooth(result, width) if width > 1 else result


def add(destination: np.ndarray, event: np.ndarray, at: float, gain: float = 1.0,
        pan: float = 0.0, wrap: bool = False) -> None:
    start = round(at * SAMPLE_RATE)
    if destination.ndim == 2 and event.ndim == 1:
        angle = (pan + 1) * math.pi / 4
        event = event[:, None] * np.array([math.cos(angle), math.sin(angle)])
    if wrap:
        # Music notes and percussion ring through the loop boundary, without a fade gap.
        indices = (start + np.arange(len(event))) % len(destination)
        np.add.at(destination, indices, gain * event)
    elif start < len(destination):
        count = min(len(event), len(destination) - start)
        destination[start:start + count] += gain * event[:count]


def impact(rng: np.random.Generator, frequency: float, seconds: float = .16,
           material: str = "steel") -> np.ndarray:
    t = clock(seconds)
    ring = sum(gain * np.sin(2 * np.pi * frequency * ratio * t) * np.exp(-t * decay)
               for gain, ratio, decay in ((1, 1, 22), (.42, 1.63, 32), (.22, 2.71, 44)))
    strike = rng.normal(size=len(t)) * np.exp(-t * 180)
    if material == "polymer":
        ring = smooth(ring, 9) * np.exp(-t * 18)
        strike = smooth(strike, 5)
    elif material == "brass":
        ring *= np.exp(t * 9)
        strike *= .45
    return .42 * ring + .65 * strike


def scrape(rng: np.random.Generator, seconds: float, width: int = 8,
           speed: float = 43) -> np.ndarray:
    t = clock(seconds)
    texture = noise(rng, seconds, width)
    ribs = (.55 + .45 * np.sin(2 * np.pi * speed * t)) ** 2
    return texture * ribs * np.sin(np.pi * t / seconds) ** 1.5


def shot(name: str, rng: np.random.Generator) -> np.ndarray:
    # duration, body pitch, decay, air-filter width, crack gain, reflection spacing.
    profiles = {
        "RevolverFire": (1.00, 109, 12, 7, 1.1, .057),
        "SniperFire": (2.10, 57, 5.8, 37, 1.7, .139),
        "SMGFire": (.29, 212, 32, 3, .85, .023),
        "RifleFire": (.72, 146, 16, 11, 1.4, .041),
        "LMGFire": (1.15, 79, 10, 19, 1.25, .078),
    }
    seconds, frequency, decay, width, crack_gain, spacing = profiles[name]
    t = clock(seconds)
    raw = rng.normal(size=len(t))
    crack = (raw - smooth(raw, 13)) * np.exp(-t * (180 if name == "SMGFire" else 110))
    # Integrate a falling frequency for a percussive pressure pulse.
    phase = frequency * t + frequency * .021 * (1 - np.exp(-t * 38))
    body = np.sin(2 * np.pi * phase) * np.exp(-t * decay)
    air = smooth(raw, width) * np.exp(-t * decay * .47)
    result = crack * crack_gain + body * .8 + air * 2.6
    if name == "LMGFire":
        add(result, impact(rng, 713, .16), .047, .20)
        add(result, impact(rng, 481, .12), .107, .14)
    if name == "SniperFire":
        result += smooth(raw, 91) * np.exp(-t * 2.6) * 1.7
    dry = result.copy()
    for index, gain in enumerate((.16, .085, .038), start=1):
        add(result, smooth(dry, 11 * index), spacing * index, gain)
    return result


def mechanism(name: str, rng: np.random.Generator) -> np.ndarray:
    durations = {
        "RevolverOpen": .48, "RevolverEject": .82, "RevolverInsert": .32, "RevolverClose": .42,
        "SniperBolt": .95, "SniperMagOut": .63, "SniperMagIn": .55,
        "SMGMagOut": .38, "SMGMagIn": .33, "SMGCharge": .48,
        "RifleMagOut": .53, "RifleMagIn": .46, "RifleCharge": .64,
        "LMGCover": .84, "LMGBelt": 1.20, "LMGClose": .66,
    }
    result = np.zeros(len(clock(durations[name])))
    if name == "RevolverOpen":
        add(result, impact(rng, 1650, .10), .012, .65)
        add(result, scrape(rng, .17, 5, 66), .08, .5)
        add(result, impact(rng, 690, .20), .24, .8)
    elif name == "RevolverEject":
        add(result, impact(rng, 890, .12), .018)
        for index in range(6):
            add(result, impact(rng, 1760 + index * 137, .28, "brass"), .16 + index * .073, .38 - index * .024)
    elif name == "RevolverInsert":
        add(result, scrape(rng, .11, 4, 83), .005, .4)
        add(result, impact(rng, 2180, .18, "brass"), .10, .65)
    elif name == "RevolverClose":
        add(result, scrape(rng, .14, 10, 29), .003, .8)
        add(result, impact(rng, 397, .24), .125)
        add(result, impact(rng, 1410, .10), .171, .55)
    elif name == "SniperBolt":
        for at, frequency, gain in ((.01, 1270, .7), (.32, 527, .8), (.72, 780, 1), (.80, 1780, .5)):
            add(result, impact(rng, frequency, .13), at, gain)
        add(result, scrape(rng, .24, 4, 92), .09, 1.3)
        add(result, scrape(rng, .29, 7, 66), .43, 1.1)
    elif "Mag" in name:
        family = name.split("Mag")[0]
        base = {"Sniper": 555, "SMG": 1380, "Rifle": 370}[family]
        width = {"Sniper": 5, "SMG": 3, "Rifle": 14}[family]
        material = "polymer" if family == "Rifle" else "steel"
        is_insert = name.endswith("In")
        travel = durations[name] * .43
        add(result, scrape(rng, travel, width, base / 14), .035, 1.5)
        add(result, impact(rng, base * (1 if is_insert else 1.6), .12, material), .012, .45)
        add(result, impact(rng, base, .16, material), travel + .04, 1 if is_insert else .38)
        if is_insert:
            add(result, impact(rng, base * 1.9, .09, material), travel + .10, .4)
    elif name.endswith("Charge"):
        rifle = name.startswith("Rifle")
        travel = .27 if rifle else .17
        add(result, impact(rng, 650 if rifle else 1180, .09), .007, .6)
        add(result, scrape(rng, travel, 6 if rifle else 3, 58 if rifle else 109), .065, 1.8)
        t = clock(.17)
        spring = np.sin(2 * np.pi * (1210 * t - 1300 * t * t)) * np.exp(-t * 20)
        add(result, spring, travel + .07, .24)
        add(result, impact(rng, 360 if rifle else 890, .18), travel + .12, 1.2)
    elif name == "LMGCover":
        add(result, impact(rng, 640, .17), .015, .95)
        add(result, scrape(rng, .43, 7, 23), .12, 1.9)
        t = clock(.40)
        add(result, np.sin(2 * np.pi * (470 * t + 42 * t * t)) * np.sin(np.pi * t / .4) ** 2, .22, .2)
        add(result, impact(rng, 283, .20), .56, .6)
    elif name == "LMGBelt":
        add(result, scrape(rng, .84, 3, 34), .05, .6)
        for index in range(14):
            add(result, impact(rng, 1050 + (index % 5) * 183, .15, "brass"), .055 + index * .069, .28 + (index % 3) * .08)
        add(result, impact(rng, 458, .18), .96, .7)
    elif name == "LMGClose":
        add(result, scrape(rng, .21, 16, 18), .005, 1.4)
        add(result, impact(rng, 189, .30), .20, 1.5)
        add(result, impact(rng, 930, .12), .26, .7)
        add(result, impact(rng, 617, .13), .40, .5)
    return result


def note(midi: float, seconds: float, kind: str = "bell") -> np.ndarray:
    t = clock(seconds)
    frequency = 440 * 2 ** ((midi - 69) / 12)
    if kind == "pad":
        envelope = np.sin(np.pi * t / seconds) ** 2
        return (np.sin(2 * np.pi * frequency * t) + .18 * np.sin(2 * np.pi * frequency * 2.004 * t)) * envelope
    if kind == "bass":
        envelope = (1 - np.exp(-t * 150)) * np.exp(-t * 5 / seconds) * (1 - t / seconds)
        return (np.sin(2 * np.pi * frequency * t) + .18 * np.sin(4 * np.pi * frequency * t)) * envelope
    envelope = (1 - np.exp(-t * 300)) * np.exp(-t * 5 / seconds) * (1 - t / seconds)
    return (np.sin(2 * np.pi * frequency * t) + .24 * np.sin(2 * np.pi * frequency * 2.01 * t)
            + .09 * np.sin(2 * np.pi * frequency * 3.97 * t)) * envelope


def event_sound(name: str, rng: np.random.Generator) -> np.ndarray:
    seconds = {"LootPickup": .64, "InventoryMove": .19, "TraderVoice": 1.45,
               "Trade": .85, "BunkerDoor": 3.6, "DeathDrop": 1.15}[name]
    result = np.zeros(len(clock(seconds)))
    if name == "LootPickup":
        add(result, scrape(rng, .25, 17, 18), .01, 1.5)
        add(result, impact(rng, 530, .13, "polymer"), .12, .5)
        add(result, note(81, .30), .19, .2)
        add(result, note(88, .22), .34, .11)
    elif name == "InventoryMove":
        add(result, scrape(rng, .12, 25, 35), .007, 1.4)
        add(result, impact(rng, 880, .09, "polymer"), .024, .22)
    elif name == "TraderVoice":
        # Two formant-shaped harmonic bursts: a fictional radio vocal, never speech.
        for at, length, fundamental, formants in ((.12, .42, 101, (490, 1260, 2350)), (.63, .61, 86, (370, 920, 1920))):
            t = clock(length)
            vibrato = .035 * np.sin(2 * np.pi * 6.2 * t)
            voice = np.zeros(len(t))
            for harmonic in range(1, 38):
                hz = harmonic * fundamental
                strength = sum(np.exp(-.5 * ((hz - formant) / bandwidth) ** 2)
                               for formant, bandwidth in zip(formants, (95, 170, 240)))
                voice += strength / math.sqrt(harmonic) * np.sin(2 * np.pi * hz * t + harmonic * vibrato)
            voice = np.tanh(voice * 2) + .16 * noise(rng, length, 3)
            add(result, voice * np.sin(np.pi * t / length) ** 1.7, at, .65)
        add(result, impact(rng, 2300, .07), .01, .17)
        add(result, scrape(rng, .10, 2, 118), 1.29, .26)
    elif name == "Trade":
        add(result, impact(rng, 1170, .12), .005, .4)
        for index, midi in enumerate((67, 74, 79)):
            add(result, note(midi, .36), .10 + index * .16, .33)
        add(result, scrape(rng, .16, 8, 66), .49, .35)
    elif name == "BunkerDoor":
        add(result, impact(rng, 136, .37), .015, 1.6)
        t = clock(2.55)
        motor = (np.sin(2 * np.pi * (46 * t + 4 * t * t)) + .35 * np.sin(2 * np.pi * 138 * t))
        motor *= np.sin(np.pi * t / 2.55) ** .8
        add(result, motor, .33, .30)
        add(result, scrape(rng, 2.4, 7, 12), .45, 1.3)
        for at in (.81, 1.49, 2.03, 2.58):
            add(result, impact(rng, 231, .2), at, .25)
        add(result, impact(rng, 93, .44), 2.94, 2.0)
        add(result, impact(rng, 683, .19), 3.14, .62)
    elif name == "DeathDrop":
        add(result, scrape(rng, .25, 19, 28), .008, 1.8)
        t = clock(.4)
        add(result, np.sin(2 * np.pi * (79 * t - 32 * t * t)) * np.exp(-t * 16), .21, 1.0)
        add(result, impact(rng, 190, .25, "polymer"), .22, .9)
        for index, at in enumerate((.34, .46, .61, .79)):
            add(result, impact(rng, 1210 + index * 231, .25, "brass"), at, .3 / (1 + index * .45))
    return result


def music(name: str, rng: np.random.Generator) -> np.ndarray:
    bpm = {"MenuMusic": 80, "ExploreMusic": 64, "CombatMusic": 120}[name]
    beat = 60 / bpm
    seconds = beat * 32  # Eight complete bars in each independently composed loop.
    result = np.zeros((len(clock(seconds)), 2))
    roots = {"MenuMusic": (45, 45, 41, 41, 48, 43, 40, 40),
             "ExploreMusic": (38, 38, 46, 46, 41, 41, 43, 45),
             "CombatMusic": (40, 40, 43, 41, 40, 47, 43, 38)}[name]
    motifs = {"MenuMusic": (12, 19, 15, 22, 19, 10, 15, 7),
              "ExploreMusic": (19, 14, 24, 17, 12, 22, 15, 26),
              "CombatMusic": (0, 12, 7, 3, 0, 10, 7, 15)}[name]
    for bar, root in enumerate(roots):
        at = bar * 4 * beat
        if name != "CombatMusic":
            for interval, pan in ((0, -.65), (7, .65), (15, -.15)):
                add(result, note(root + interval, beat * 6.3, "pad"), at, .14, pan, True)
            for index in range(2):
                add(result, note(root + motifs[(bar + index * 3) % 8], beat * 2.6), at + beat * (index * 2 + .5),
                    .19 if name == "MenuMusic" else .11, (-1) ** bar * .45, True)
            add(result, note(root - 12, beat * 3.7, "bass"), at, .16, 0, True)
        else:
            for index in range(8):
                add(result, note(root - 12 + motifs[(bar + index) % 8], beat * .46, "bass"),
                    at + index * beat * .5, .42 if index % 2 == 0 else .25, 0, True)
            add(result, note(root + 19, beat * 3.1, "pad"), at + beat * .75, .13, (-1) ** bar * .55, True)

        if name in ("MenuMusic", "CombatMusic"):
            for pulse in (range(4) if name == "CombatMusic" else (0, 2)):
                t = clock(.23)
                kick = np.sin(2 * np.pi * (49 * t + 2.2 * (1 - np.exp(-t * 37)))) * np.exp(-t * 23)
                add(result, kick, at + beat * pulse, .48 if name == "CombatMusic" else .12, 0, True)
            if name == "CombatMusic":
                for pulse in (1, 3):
                    t = clock(.18)
                    snare = (noise(rng, .18, 2) + .3 * np.sin(2 * np.pi * 173 * t)) * np.exp(-t * 28)
                    add(result, snare, at + pulse * beat, .29, -.12, True)
                for pulse in range(8):
                    t = clock(.065)
                    hat_noise = rng.normal(size=len(t))
                    hat = (hat_noise - smooth(hat_noise, 13)) * np.exp(-t * 95)
                    add(result, hat, at + pulse * beat * .5, .055, .52, True)
        else:
            add(result, scrape(rng, beat * 2, 75, 3), at + beat, .12, (-1) ** bar * .8, True)

    # Whole-period ambience and circular delays keep stereo tails continuous at wrap.
    t = clock(seconds)
    for channel, frequency in enumerate((roots[0], roots[0] + .025)):
        cycles = round(440 * 2 ** ((frequency - 69) / 12) * seconds)
        result[:, channel] += .025 * np.sin(2 * np.pi * cycles * t / seconds)
    dry = result.copy()
    for delay, gain in ((beat * .75, .13), (beat * 1.5, .075), (beat * 2.25, .045)):
        result += gain * np.roll(dry[:, ::-1], round(delay * SAMPLE_RATE), axis=0)
    return result


def synthesize(name: str) -> np.ndarray:
    if name not in NEW_SLOTS:
        raise ValueError(f"Unknown v2 audio slot: {name}")
    rng = random_for(name)
    if name in MUSIC_SLOTS:
        signal = music(name, rng)
    elif name.endswith("Fire"):
        signal = shot(name, rng)
    elif name in ("LootPickup", "InventoryMove", "TraderVoice", "Trade", "BunkerDoor", "DeathDrop"):
        signal = event_sound(name, rng)
    else:
        signal = mechanism(name, rng)
    signal -= signal.mean(axis=0)
    if name not in MUSIC_SLOTS:
        attack = min(32, len(signal))
        release = min(round(.028 * SAMPLE_RATE), len(signal))
        signal[:attack] *= np.linspace(0, 1, attack)
        signal[-release:] *= np.linspace(1, 0, release)
    peak = float(np.max(np.abs(signal)))
    if not np.isfinite(signal).all() or peak < 1e-8:
        raise ValueError(f"Invalid or silent synthesis: {name}")
    # Keep transients intact and leave headroom. Slot volume supplies further gain.
    level = .68 if name in MUSIC_SLOTS else .86 if name.endswith("Fire") else .67
    return signal * (level / peak)


def pcm16(signal: np.ndarray) -> np.ndarray:
    return np.rint(np.clip(signal, -1, 1) * 32767).astype("<i2")


def inspect_signal(name: str, signal: np.ndarray) -> dict:
    if not np.isfinite(signal).all():
        raise ValueError(f"Non-finite audio: {name}")
    peak = float(np.max(np.abs(signal)))
    rms = float(np.sqrt(np.mean(signal ** 2)))
    if not (0 < peak < .99 and rms > .002):
        raise ValueError(f"Silent or clipping signal: {name}, peak={peak}, rms={rms}")
    seam = float(np.max(np.abs(signal[0] - signal[-1])))
    if name in MUSIC_SLOTS:
        if signal.ndim != 2 or signal.shape[1] != 2:
            raise ValueError(f"Music must be stereo: {name}")
        # A wrap transition should not be an exceptional jump relative to the waveform.
        if seam > max(.025, float(np.quantile(np.abs(np.diff(signal, axis=0)), .999))):
            raise ValueError(f"Audible music loop discontinuity: {name}, seam={seam}")
    elif signal.ndim != 1 or seam > 1e-8:
        raise ValueError(f"Effects must be mono with silent endpoints: {name}")
    return {"slot": name, "seconds": round(len(signal) / SAMPLE_RATE, 3),
            "channels": 1 if signal.ndim == 1 else 2, "peak": round(peak, 4),
            "rms": round(rms, 4), "seam": round(seam, 6),
            "sha256": hashlib.sha256(pcm16(signal).tobytes()).hexdigest()}


def write_wave(path: Path, signal: np.ndarray) -> None:
    with wave.open(str(path), "wb") as output:
        output.setnchannels(1 if signal.ndim == 1 else signal.shape[1])
        output.setsampwidth(2)
        output.setframerate(SAMPLE_RATE)
        output.writeframes(pcm16(signal).tobytes())


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, default=ROOT / "ArtSource" / "Audio")
    parser.add_argument("--overwrite", action="store_true", help="Replace only the 30 named v2 WAVs.")
    parser.add_argument("--check", action="store_true", help="Synthesize and verify all signals without writing files.")
    args = parser.parse_args(argv)
    if not args.check:
        args.output.mkdir(parents=True, exist_ok=True)
    fingerprints = set()
    written = skipped = 0
    for name in NEW_SLOTS:
        signal = synthesize(name)
        report = inspect_signal(name, signal)
        if report["sha256"] in fingerprints:
            raise ValueError(f"Duplicate placeholder: {name}")
        fingerprints.add(report["sha256"])
        destination = args.output / f"S_{name}.wav"
        action = "verified"
        if not args.check:
            if destination.exists() and not args.overwrite:
                action = "preserved existing"
                skipped += 1
            else:
                write_wave(destination, signal)
                action = "wrote"
                written += 1
        print(f"{name:18} {report['seconds']:6.2f}s {report['channels']}ch "
              f"peak={report['peak']:.2f} rms={report['rms']:.3f} {action}")
    print(f"LW_AUDIO_V2: {len(fingerprints)} unique original signals verified; {written} written, {skipped} preserved.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
