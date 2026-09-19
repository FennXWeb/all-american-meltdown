"""Offline signal checks for the generated vehicle library; no API calls."""
import json, wave
from pathlib import Path
import numpy as np

root = Path(__file__).resolve().parents[1]
source = root / 'ArtSource/AudioV42'
manifest = json.loads((source / 'manifest.json').read_text())
report = []
for row in manifest['sounds']:
    with wave.open(str(source / row['file']), 'rb') as wav:
        assert wav.getnchannels() == 1 and wav.getsampwidth() == 2, row['slot']
        rate = wav.getframerate()
        samples = np.frombuffer(wav.readframes(wav.getnframes()), dtype='<i2').astype(float) / 32768
    duration = len(samples) / rate
    peak = float(np.max(np.abs(samples)))
    rms = float(np.sqrt(np.mean(samples ** 2)))
    assert row['duration'] - .25 < duration < row['duration'] + .25, (row['slot'], duration)
    assert .001 < rms < .3 and peak < .9, (row['slot'], rms, peak)
    seam = float(abs(samples[-1] - samples[0]))
    # A loop seam should be comparable to ordinary adjacent sample movement.
    adjacent = float(np.quantile(np.abs(np.diff(samples)), .999))
    if row['loop']:
        assert seam < max(.02, adjacent * 2), (row['slot'], 'loop discontinuity', seam)
    report.append(dict(slot=row['slot'], seconds=duration, rms=rms, peak=peak, seam=seam))
usage = json.loads((source / 'usage.json').read_text())
assert len(usage) == len(report) and all(v['state'] == 'complete' for v in usage.values())
credits = sum(v['cost'] for v in usage.values())
assert credits <= 7800
destination = root / 'Saved/Vehicle42AudioValidation.json'
destination.write_text(json.dumps(dict(files=len(report), reported_credits=credits, results=report), indent=2))
print('VEHICLE42_AUDIO_VALIDATED', len(report), 'reported credits', credits)
