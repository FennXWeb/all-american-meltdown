import unreal as u
from pathlib import Path
root=Path(u.Paths.project_dir()).resolve()
source=(root/'Tools/import57.py').read_text().split('# Use existing strict')[0]
exec(compile(source,'materials57','exec'),{'__name__':'__main__'})
source=(root/'Tools/import_models_v5.py').read_text().replace("'ModelsV5'","'ModelsV57'").replace('models_v5_manifest.json','models_v57_manifest.json').replace("records = manifest['assets']","records = [r for r in manifest['assets'] if r['name']=='RogueTorso57']")
exec(compile(source,'robot57','exec'),{'__name__':'__main__'})
u.log('EXPANSION57_REFRESH_DONE')
