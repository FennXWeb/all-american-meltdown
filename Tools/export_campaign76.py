"""Readable author script from the same data compiled into the game.

This output contains spoilers and is never loaded by the runtime journal.
"""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
files = sorted((ROOT / 'Data/Campaign76').glob('*.json'))
documents = [json.loads(path.read_text(encoding='utf-8')) for path in files]
names = {p['id']: p['name'] for d in documents for p in d['cast']}
out = ['# The Country We Left Behind — integrated author script', '',
       'Generated from the executable campaign data. Contains all branches and spoilers.', '',
       'Performance status: temporary first-person staging with existing faces, gestures, '
       'movement marks, lights, props and audio cues. Unrecorded lines are subtitled. '
       'Narrative descriptions of hand actions or destruction are not bespoke animation assets.', '']

def requirements(values):
    return ', '.join(f'`{value}`' for value in values) or 'none'

for path, data in zip(files, documents):
    out += [f'## {path.stem.replace("_", " ")}', '', '### Playable mission sequence', '']
    for stage in data['stages']:
        out += [f'#### {stage["title"]} · `{stage["id"]}`', '',
                f'Mission: `{stage["mission"]}`. Location: `{stage["site"]}`.', '',
                stage['goal'], '',
                'Cast: ' + ', '.join(names.get(p, p) for p in stage['cast']) + '.', '']
        if stage.get('arrival'):
            out += [f'Arrival conversation: `{stage["arrival"]}`.', '']
        if stage.get('enemies'):
            out += [f'Authored encounter: {stage["enemies"]} base opponents; preparation and saved defeats modify the live count.', '']
        for action in stage['actions']:
            out += [f'- **{action["label"]}** (`{action["id"]}`). '
                    f'Requires: {requirements(action["needs"])}. '
                    f'Applies: {requirements(action["effects"])}. '
                    f'Conversation: `{action.get("scene") or "—"}`; next stage: `{action.get("next") or "—"}`.']
        out += ['']
        if stage.get('next'):
            out += [f'Automatic continuation: `{stage["next"]}` after {requirements(stage.get("needs", []))}.', '']
    out += ['### Complete conversations', '']
    for scene in data['scenes']:
        out += [f'#### `{scene["id"]}`', '']
        for beat in scene['beats']:
            speaker = names.get(beat.get('who'), 'Scene')
            condition = f' [when {requirements(beat["needs"])}]' if beat.get('needs') else ''
            out += [f'**{speaker}{condition}:** {beat["text"]}', '']
            marks = []
            if any(beat.get('move', [])):
                marks += ['Move to local mark ' + str(beat['move']) + ' cm']
            if beat.get('gesture'):
                marks += ['Existing gesture: ' + beat['gesture']]
            if beat.get('pause'):
                marks += [f'Minimum beat hold: {beat["pause"]} s']
            if beat.get('cue'):
                marks += ['Existing audio cue: ' + beat['cue']]
            if marks:
                out += ['*' + '; '.join(marks) + '.*', '']
        for choice in scene['choices']:
            out += [f'- **{choice["text"]}** Requires: {requirements(choice["needs"])}. '
                    f'Applies: {requirements(choice["effects"])}. '
                    f'Response: `{choice.get("next") or "—"}`; stage: `{choice.get("stage") or "inherited continuation"}`.']
        out += ['']
        if scene.get('next'):
            out += [f'Continuation: `{scene["next"]}`.', '']

target = ROOT / 'Docs/Campaign76/Script.md'
target.write_text('\n'.join(out), encoding='utf-8')
print(f'Exported {sum(len(d["scenes"]) for d in documents)} complete scenes to {target}')
