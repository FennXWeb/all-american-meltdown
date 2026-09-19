"""Import validated furniture FBXs without replacing older asset libraries."""
from pathlib import Path
import unreal as u
root=Path(u.Paths.project_dir()).resolve()
code=(root/'Tools/import_models_v5.py').read_text(encoding='utf-8').replace("'ModelsV5'","'ModelsV13'").replace('models_v5_manifest.json','models_v13_manifest.json').replace('LW_V5','LW_V13')
exec(compile(code,'import_models_v13.py','exec'),{'__name__':'__main__'})
u.log('LW_V13_CONTENT_COMPLETE')
